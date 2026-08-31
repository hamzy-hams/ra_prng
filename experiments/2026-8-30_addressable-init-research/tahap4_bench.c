// tahap4_bench.c
// Tahap 4 (HANDOVER.md): speed benchmark of winner_wired_addressable
// (Kandidat 5, reuse/continue mode) against Philox4x32-10. NEW FILE --
// does not modify winner_wired_addressable.c, winner_wired_v2.c, or any
// Philox file in benchmarks/comparisons/ in place.
//
// Addressable core (rot32, ra_hash, ra_permutation_cycle, ra_reseed,
// ra_init_state_addressable, ra_core) is a byte-for-byte copy of
// winner_wired_addressable.c's (which is itself a byte-for-byte copy of
// winner_wired_v2.c's, minus ra_init_state -> ra_init_state_addressable).
// main() here is new -- not copied -- so there is no `main` symbol clash.
//
// Philox4x32-10 round function structure is copied from
// benchmarks/comparisons/source/philox.c and verified line-by-line against
// the official Random123 reference (DEShawResearch/random123,
// include/Random123/philox.h) on 2026-08-30 -- identical except for one
// constant. THE REPO'S PHILOX_M0 (0xD256D193U) IS A BUG: that value is
// actually PHILOX_M2x32_0 (the Philox2x32 variant's multiplier), not
// PHILOX_M4x32_0. The correct production Philox4x32-10 constant is
// 0xD2511F53U. This file uses the CORRECTED constant (see PHILOX_M0 below)
// -- benchmarks/comparisons/source/philox.c and its siblings are left
// untouched per user decision (2026-08-30), since fixing them is out of
// scope for this research and would invalidate their own stored results.
// The `verify` mode below runs 3 official Random123 known-answer-test
// vectors (from random123/tests/kat_vectors) against this file's own
// philox4x32_10() to prove the correction actually produces production
// Philox output, not just "structurally similar" code.
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h
//          tahap4_bench.c -o tahap4_bench -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define ADDR_L_MIX_CONST 0x9e3779b7u // Tahap 1, see HANDOVER.md

// ----------------------------------------------------------------------
// Addressable core -- byte-for-byte copy of winner_wired_addressable.c
// ----------------------------------------------------------------------

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

static void ra_hash(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}

static void ra_init_state_addressable(uint32_t *L, uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * ADDR_L_MIX_CONST + 0x9e3779b7u * key);
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

static void ra_permutation_cycle(uint32_t cons, size_t it,
                                  const uint32_t *M, uint32_t *L,
                                  uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = (M[(uint8_t)(i + 6)] << 6) ^ (M[(uint8_t)(i + 7)] << 7);

        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & 0xFFu;

        if (*count <= 1) break;
        --(*count);

        uint32_t tmp = L[i];
        L[i] = L[d];
        L[d] = tmp;
    }
}

static uint32_t ra_reseed(uint32_t *M, const uint32_t *L) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];

    uint32_t tmp8[8];
    ra_hash(M, tmp8);

    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

static uint32_t ra_core(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_state_addressable(L, M, key);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

// ----------------------------------------------------------------------
// Philox4x32-10 -- structure copied from benchmarks/comparisons/source/
// philox.c, PHILOX_M0 corrected to the real Philox4x32_0 constant (see
// header comment above for the bug this fixes).
// ----------------------------------------------------------------------

#define PHILOX_M0 0xD2511F53U // CORRECTED (repo's philox.c has 0xD256D193U, which is PHILOX_M2x32_0 -- wrong variant)
#define PHILOX_M1 0xCD9E8D57U
#define PHILOX_W0 0x9E3779B9U
#define PHILOX_W1 0xBB67AE85U

static inline void philox4x32_round(uint32_t ctr[4], uint32_t key[2]) {
    uint64_t p0 = (uint64_t)ctr[0] * PHILOX_M0;
    uint64_t p1 = (uint64_t)ctr[2] * PHILOX_M1;
    uint32_t hi0 = (uint32_t)(p0 >> 32);
    uint32_t lo0 = (uint32_t)p0;
    uint32_t hi1 = (uint32_t)(p1 >> 32);
    uint32_t lo1 = (uint32_t)p1;
    uint32_t c1 = ctr[1];
    uint32_t c3 = ctr[3];
    ctr[0] = hi1 ^ c1 ^ key[0];
    ctr[1] = lo1;
    ctr[2] = hi0 ^ c3 ^ key[1];
    ctr[3] = lo0;
    key[0] += PHILOX_W0;
    key[1] += PHILOX_W1;
}

static void philox4x32_10(uint32_t out[4], const uint32_t ctr_in[4], const uint32_t key_in[2]) {
    uint32_t ctr[4] = { ctr_in[0], ctr_in[1], ctr_in[2], ctr_in[3] };
    uint32_t key[2] = { key_in[0], key_in[1] };
    for (int i = 0; i < 10; i++) {
        philox4x32_round(ctr, key);
    }
    memcpy(out, ctr, 4 * sizeof(uint32_t));
}

static inline void philox_ctr_increment(uint32_t ctr[4]) {
    if (++ctr[0] == 0) {
        if (++ctr[1] == 0) {
            if (++ctr[2] == 0) {
                ++ctr[3];
            }
        }
    }
}

// ----------------------------------------------------------------------
// verify mode: official Random123 known-answer-test vectors
// (random123/tests/kat_vectors, philox4x32 10-round entries)
// ----------------------------------------------------------------------

static int run_kat_checks(void) {
    struct { uint32_t ctr[4]; uint32_t key[2]; uint32_t expect[4]; } kats[] = {
        { {0,0,0,0}, {0,0}, {0x6627e8d5u, 0xe169c58du, 0xbc57ac4cu, 0x9b00dbd8u} },
        { {0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu}, {0xffffffffu,0xffffffffu},
          {0x408f276du, 0x41c83b0eu, 0xa20bc7c6u, 0x6d5451fdu} },
        { {0x243f6a88u,0x85a308d3u,0x13198a2eu,0x03707344u}, {0xa4093822u,0x299f31d0u},
          {0xd16cfe09u, 0x94fdccebu, 0x5001e420u, 0x24126ea1u} },
    };
    int all_ok = 1;
    for (size_t k = 0; k < sizeof(kats)/sizeof(kats[0]); ++k) {
        uint32_t out[4];
        philox4x32_10(out, kats[k].ctr, kats[k].key);
        int ok = memcmp(out, kats[k].expect, sizeof(out)) == 0;
        printf("KAT %zu: %s (got %08x %08x %08x %08x, expected %08x %08x %08x %08x)\n",
               k, ok ? "PASS" : "FAIL",
               out[0], out[1], out[2], out[3],
               kats[k].expect[0], kats[k].expect[1], kats[k].expect[2], kats[k].expect[3]);
        if (!ok) all_ok = 0;
    }
    return all_ok;
}

// ----------------------------------------------------------------------
// benchmark modes
// ----------------------------------------------------------------------

static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

static void mode_init_cost(long repeats) {
    volatile uint32_t sink = 0;

    alignas(64) uint32_t L[256], M[256];
    double t0 = now_seconds();
    for (long i = 0; i < repeats; ++i) {
        ra_init_state_addressable(L, M, (uint32_t)i);
        sink ^= L[0] ^ M[0];
    }
    double t1 = now_seconds();
    double addr_total = t1 - t0;

    uint32_t ctr[4] = {0,0,0,0};
    uint32_t key[2] = {0xDEADBEEFu, 0xFEEDC0DEu};
    double t2 = now_seconds();
    for (long i = 0; i < repeats; ++i) {
        uint32_t out[4];
        philox4x32_10(out, ctr, key);
        philox_ctr_increment(ctr);
        sink ^= out[0];
    }
    double t3 = now_seconds();
    double philox_total = t3 - t2;

    double addr_ns_per_init = addr_total * 1e9 / (double)repeats;
    double philox_ns_per_call = philox_total * 1e9 / (double)repeats;

    printf("mode init-cost repeats %ld addr_ns_per_init %.3f philox_ns_per_call %.3f ratio %.3f checksum %u\n",
           repeats, addr_ns_per_init, philox_ns_per_call,
           addr_ns_per_init / philox_ns_per_call, sink);
}

// Each N is timed over several trials; MIN (not mean) is reported, since
// system-noise/scheduling jitter can only ever inflate a trial's time, not
// deflate it below the true cost -- min is the standard robust estimator
// for microbenchmarks. More trials for small N (cheap, need more samples
// to escape noise), fewer for huge N (each trial already takes long enough
// to be stable, and huge*many trials would be slow).
static long trials_for_n(long long n) {
    if (n < 1000) return 200;
    if (n < 100000) return 30;
    if (n < 10000000) return 5;
    return 1;
}

static void mode_throughput(int argc, char **argv, int start) {
    volatile uint32_t sink = 0;
    for (int a = start; a < argc; ++a) {
        long long n = atoll(argv[a]);
        if (n <= 0) continue;
        long trials = trials_for_n(n);

        double addr_best = -1.0;
        for (long t = 0; t < trials; ++t) {
            double t0 = now_seconds();
            uint32_t last_cons = ra_core(12345u, (size_t)n, NULL);
            double t1 = now_seconds();
            sink ^= last_cons;
            double dt = t1 - t0;
            if (addr_best < 0 || dt < addr_best) addr_best = dt;
        }

        long long blocks = (n + 3) / 4;
        double philox_best = -1.0;
        for (long t = 0; t < trials; ++t) {
            uint32_t ctr[4] = {0,0,0,0};
            uint32_t key[2] = {0xDEADBEEFu, 0xFEEDC0DEu};
            double t2 = now_seconds();
            for (long long b = 0; b < blocks; ++b) {
                uint32_t out[4];
                philox4x32_10(out, ctr, key);
                philox_ctr_increment(ctr);
                sink ^= out[0] ^ out[1] ^ out[2] ^ out[3];
            }
            double t3 = now_seconds();
            double dt = t3 - t2;
            if (philox_best < 0 || dt < philox_best) philox_best = dt;
        }

        printf("N %lld addr_seconds %.6f addr_ns_per_word %.3f philox_seconds %.6f philox_ns_per_word %.3f trials %ld checksum %u\n",
               n, addr_best, addr_best * 1e9 / (double)n,
               philox_best, philox_best * 1e9 / (double)n, trials, sink);
    }
}

static void mode_reinit_sweep(int argc, char **argv, int start, long long cycles_target_words) {
    volatile uint32_t sink = 0;
    for (int a = start; a < argc; ++a) {
        long long k = atoll(argv[a]);
        if (k <= 0) continue;

        long long cycles = cycles_target_words / k;
        if (cycles < 10) cycles = 10;
        if (cycles > 200000) cycles = 200000;

        const int TRIALS = 3; // min-of-trials, same noise rationale as mode_throughput
        double addr_total = -1.0, philox_total = -1.0;

        for (int trial = 0; trial < TRIALS; ++trial) {
            double t0 = now_seconds();
            for (long long c = 0; c < cycles; ++c) {
                uint32_t key = (uint32_t)(c * 2654435761u + 1u); // fresh key per cycle, avoid key=0 edge trivially
                uint32_t last = ra_core(key, (size_t)k, NULL);
                sink ^= last;
            }
            double t1 = now_seconds();
            double dt = t1 - t0;
            if (addr_total < 0 || dt < addr_total) addr_total = dt;
        }

        long long blocks_per_cycle = (k + 3) / 4;
        for (int trial = 0; trial < TRIALS; ++trial) {
            double t2 = now_seconds();
            for (long long c = 0; c < cycles; ++c) {
                uint32_t ctr[4] = {0,0,0,0};
                uint32_t key[2] = { (uint32_t)(c * 40503u + 1u), 0xFEEDC0DEu };
                for (long long b = 0; b < blocks_per_cycle; ++b) {
                    uint32_t out[4];
                    philox4x32_10(out, ctr, key);
                    philox_ctr_increment(ctr);
                    sink ^= out[0] ^ out[1] ^ out[2] ^ out[3];
                }
            }
            double t3 = now_seconds();
            double dt = t3 - t2;
            if (philox_total < 0 || dt < philox_total) philox_total = dt;
        }

        double addr_ns_per_word = addr_total * 1e9 / (double)(cycles * k);
        double philox_ns_per_word = philox_total * 1e9 / (double)(cycles * k);

        printf("K %lld cycles %lld addr_ns_per_word_steadystate %.3f philox_ns_per_word %.3f ratio %.3f checksum %u\n",
               k, cycles, addr_ns_per_word, philox_ns_per_word,
               addr_ns_per_word / philox_ns_per_word, sink);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage:\n"
                         "  %s verify\n"
                         "  %s init-cost <repeats>\n"
                         "  %s throughput <n1> <n2> ...\n"
                         "  %s reinit-sweep <cycles_target_words> <k1> <k2> ...\n",
                argv[0], argv[0], argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "verify") == 0) {
        int ok = run_kat_checks();
        return ok ? 0 : 1;
    }
    // --stream mode exists ONLY to bit-identical-check this file's copied
    // ra_core against winner_wired_addressable's own --stream output --
    // not used by any benchmark mode.
    if (strcmp(argv[1], "--stream") == 0 && argc >= 4) {
        uint32_t key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t rng = (size_t)strtoull(argv[3], NULL, 0);
        ra_core(key, rng, stdout);
        return 0;
    }
    if (strcmp(argv[1], "init-cost") == 0 && argc >= 3) {
        mode_init_cost(atol(argv[2]));
        return 0;
    }
    if (strcmp(argv[1], "throughput") == 0 && argc >= 3) {
        mode_throughput(argc, argv, 2);
        return 0;
    }
    if (strcmp(argv[1], "reinit-sweep") == 0 && argc >= 4) {
        long long cycles_target_words = atoll(argv[2]);
        mode_reinit_sweep(argc, argv, 3, cycles_target_words);
        return 0;
    }

    fprintf(stderr, "Unknown mode or missing args: %s\n", argv[1]);
    return 1;
}
