// tahap5_bench.c
// Tahap 5 (HANDOVER_TAHAP5.md): implementation-level speed optimization of
// ra_init_state_addressable, formula-preserving (output bit-identical to
// winner_wired_addressable.c). Forks tahap4_bench.c's structure -- does NOT
// modify tahap4_bench.c, winner_wired_addressable.c, winner_wired_v2.c, or
// any Philox file in benchmarks/comparisons/ in place.
//
// ra_hash, ra_permutation_cycle, ra_reseed, rot32, Philox4x32-10 (corrected
// PHILOX_M0, see tahap4_bench.c's header comment for the citation/rationale
// -- unchanged here) are byte-for-byte copies of tahap4_bench.c's. The ONLY
// structural change vs tahap4_bench.c is ra_core() taking an init_fn_t
// function pointer so multiple ra_init_state_addressable variants can be
// benchmarked side by side through the SAME modes (init-cost isolated AND
// throughput/reinit-sweep end-to-end), selected via a CLI <variant> arg.
//
// Variants (Rank numbers refer to HANDOVER_TAHAP5.md section 4):
//   v0_baseline -- exact copy of the frozen formula, control/no-op variant.
//   v1_rolv     -- Rank 2: explicit AVX-512VL _mm256_rolv_epi32 rotate,
//                  replacing the sllv+srlv+or pattern GCC emits for rot32
//                  inside the auto-vectorized loop (confirmed via objdump
//                  in the Tahap 5 prep session AND independently reproduced
//                  in this session's planning phase, 2026-08-30).
//
// Rank 1 (-mprefer-vector-width=512) and Rank 3 (-funroll-loops) are
// evaluated as SEPARATE COMPILE of this same source with v0_baseline only
// (see tahap5_benchmark.py) -- no code variant needed for those, per
// HANDOVER_TAHAP5.md section 4's isolation requirement.
//
// A manual accumulator/strength-reduction variant was tried and REJECTED
// during this Tahap's planning (empirically verified 2026-08-30): rewriting
// i*C as a running accumulator (l += C per iteration) is bit-identical but
// 4.4-5.0x SLOWER, because it introduces a loop-carried dependency chain
// that defeats GCC's auto-vectorization entirely (falls back to a scalar
// rol-per-element loop instead of AVX-512VL 8-lane). Not included here --
// see RESULTS.md for the measured numbers.
//
// Compile (default, evaluates Rank 2 / Rank 3-manual):
//   gcc -O3 -march=native -std=gnu17 -include stdalign.h
//       tahap5_bench.c -o tahap5_bench -Wall -Wextra
// Compile (Rank 1 isolation):
//   gcc -O3 -march=native -mprefer-vector-width=512 -std=gnu17
//       -include stdalign.h tahap5_bench.c -o tahap5_bench_zmm512 -Wall -Wextra
// Compile (Rank 3 flag-only isolation):
//   gcc -O3 -march=native -funroll-loops -std=gnu17 -include stdalign.h
//       tahap5_bench.c -o tahap5_bench_unroll -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <immintrin.h>

#define ADDR_L_MIX_CONST 0x9e3779b7u // Tahap 1, see HANDOVER.md

// ----------------------------------------------------------------------
// rot32 / ra_hash / ra_permutation_cycle / ra_reseed -- byte-for-byte
// copies of tahap4_bench.c's (which are themselves byte-for-byte copies of
// winner_wired_addressable.c's). Untouched.
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

// ----------------------------------------------------------------------
// ra_init_state_addressable variants
// ----------------------------------------------------------------------

// v0_baseline -- exact copy of the frozen formula (control).
static void ra_init_state_addressable_v0_baseline(uint32_t *L, uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * ADDR_L_MIX_CONST + 0x9e3779b7u * key);
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

// v1_rolv -- Rank 2: explicit AVX-512VL rotate intrinsic, replacing the
// sllv+srlv+or pattern GCC's auto-vectorizer emits for rot32. 8 lanes
// (i..i+7) processed per iteration, matching the compiler's own chosen
// vector width (YMM/256-bit) for direct apples-to-apples comparison.
// Algebra matches what GCC already does for l_val/m_val (i*C+key*C ->
// C*(i+key), single add + one mullo per output array) -- the only change
// is the rotate itself. Explicit &31 mask kept (not proven redundant).
static void ra_init_state_addressable_v1_rolv(uint32_t *L, uint32_t *M, uint32_t key) {
    const __m256i key_v = _mm256_set1_epi32((int)key);
    const __m256i cL_v = _mm256_set1_epi32((int)ADDR_L_MIX_CONST);
    const __m256i cM_v = _mm256_set1_epi32((int)0x06a0dd9bu);
    const __m256i mask31 = _mm256_set1_epi32(31);
    const __m256i inc8 = _mm256_set1_epi32(8);
    __m256i i_v = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);

    for (int i = 0; i < 256; i += 8) {
        __m256i sum = _mm256_add_epi32(i_v, key_v);       // i + key
        __m256i l_val = _mm256_mullo_epi32(sum, cL_v);    // C_L*(i+key)
        __m256i m_val = _mm256_mullo_epi32(sum, cM_v);    // C_M*(i+key)
        __m256i r_v = _mm256_and_si256(_mm256_xor_si256(key_v, i_v), mask31);

        __m256i l_rot = _mm256_rolv_epi32(l_val, r_v);
        __m256i m_rot = _mm256_rolv_epi32(m_val, r_v);

        _mm256_store_si256((__m256i *)&L[i], l_rot);
        _mm256_store_si256((__m256i *)&M[i], m_rot);

        i_v = _mm256_add_epi32(i_v, inc8);
    }
}

typedef void (*init_fn_t)(uint32_t *, uint32_t *, uint32_t);

typedef struct { const char *name; init_fn_t fn; } variant_entry_t;

static const variant_entry_t VARIANTS[] = {
    { "v0_baseline", ra_init_state_addressable_v0_baseline },
    { "v1_rolv", ra_init_state_addressable_v1_rolv },
};

static init_fn_t find_variant(const char *name) {
    for (size_t i = 0; i < sizeof(VARIANTS) / sizeof(VARIANTS[0]); ++i) {
        if (strcmp(VARIANTS[i].name, name) == 0) return VARIANTS[i].fn;
    }
    fprintf(stderr, "Unknown variant '%s'. Known: ", name);
    for (size_t i = 0; i < sizeof(VARIANTS) / sizeof(VARIANTS[0]); ++i) {
        fprintf(stderr, "%s ", VARIANTS[i].name);
    }
    fprintf(stderr, "\n");
    exit(1);
}

// ----------------------------------------------------------------------
// ra_core -- same structure as tahap4_bench.c's, except init is now
// selected via function pointer instead of hardcoded, so multiple variants
// share the exact same permutation/reseed loop.
// ----------------------------------------------------------------------

static uint32_t ra_core(uint32_t key, size_t rng, FILE *raw_stream, init_fn_t init_fn) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    init_fn(L, M, key);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

// ----------------------------------------------------------------------
// Philox4x32-10 -- byte-for-byte copy of tahap4_bench.c's (corrected
// PHILOX_M0). See tahap4_bench.c header comment for the citation.
// ----------------------------------------------------------------------

#define PHILOX_M0 0xD2511F53U
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
// benchmark modes -- same methodology as tahap4_bench.c (min-of-trials,
// CLOCK_MONOTONIC, volatile checksum sink), parametrized by variant.
// ----------------------------------------------------------------------

static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

static void mode_init_cost(init_fn_t init_fn, long repeats) {
    volatile uint32_t sink = 0;

    alignas(64) uint32_t L[256], M[256];
    double t0 = now_seconds();
    for (long i = 0; i < repeats; ++i) {
        init_fn(L, M, (uint32_t)i);
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

static long trials_for_n(long long n) {
    if (n < 1000) return 200;
    if (n < 100000) return 30;
    if (n < 10000000) return 5;
    return 1;
}

static void mode_throughput(init_fn_t init_fn, int argc, char **argv, int start) {
    volatile uint32_t sink = 0;
    for (int a = start; a < argc; ++a) {
        long long n = atoll(argv[a]);
        if (n <= 0) continue;
        long trials = trials_for_n(n);

        double addr_best = -1.0;
        for (long t = 0; t < trials; ++t) {
            double t0 = now_seconds();
            uint32_t last_cons = ra_core(12345u, (size_t)n, NULL, init_fn);
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

static void mode_reinit_sweep(init_fn_t init_fn, int argc, char **argv, int start, long long cycles_target_words) {
    volatile uint32_t sink = 0;
    for (int a = start; a < argc; ++a) {
        long long k = atoll(argv[a]);
        if (k <= 0) continue;

        long long cycles = cycles_target_words / k;
        if (cycles < 10) cycles = 10;
        if (cycles > 200000) cycles = 200000;

        const int TRIALS = 3;
        double addr_total = -1.0, philox_total = -1.0;

        for (int trial = 0; trial < TRIALS; ++trial) {
            double t0 = now_seconds();
            for (long long c = 0; c < cycles; ++c) {
                uint32_t key = (uint32_t)(c * 2654435761u + 1u);
                uint32_t last = ra_core(key, (size_t)k, NULL, init_fn);
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
                         "  %s --stream <variant> <key> <n>\n"
                         "  %s init-cost <variant> <repeats>\n"
                         "  %s throughput <variant> <n1> <n2> ...\n"
                         "  %s reinit-sweep <variant> <cycles_target_words> <k1> <k2> ...\n"
                         "Variants: v0_baseline v1_rolv\n",
                argv[0], argv[0], argv[0], argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "verify") == 0) {
        int ok = run_kat_checks();
        return ok ? 0 : 1;
    }
    // --stream mode exists ONLY to bit-identical-check a variant's ra_core
    // against winner_wired_addressable's own --stream output.
    if (strcmp(argv[1], "--stream") == 0 && argc >= 5) {
        init_fn_t fn = find_variant(argv[2]);
        uint32_t key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t rng = (size_t)strtoull(argv[4], NULL, 0);
        ra_core(key, rng, stdout, fn);
        return 0;
    }
    if (strcmp(argv[1], "init-cost") == 0 && argc >= 4) {
        init_fn_t fn = find_variant(argv[2]);
        mode_init_cost(fn, atol(argv[3]));
        return 0;
    }
    if (strcmp(argv[1], "throughput") == 0 && argc >= 4) {
        init_fn_t fn = find_variant(argv[2]);
        mode_throughput(fn, argc, argv, 3);
        return 0;
    }
    if (strcmp(argv[1], "reinit-sweep") == 0 && argc >= 5) {
        init_fn_t fn = find_variant(argv[2]);
        long long cycles_target_words = atoll(argv[3]);
        mode_reinit_sweep(fn, argc, argv, 4, cycles_target_words);
        return 0;
    }

    fprintf(stderr, "Unknown mode or missing args: %s\n", argv[1]);
    return 1;
}
