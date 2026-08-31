// tahap6_bench.c
// Tahap 6 (HANDOVER_TAHAP6.md): removes L[256] entirely for the rng<=255
// (single-block, no-reseed) "addressable penuh/agresif" use case -- one
// key/address = one init = up to 255 output words, then discard/re-init
// for a new address. L's only functional reader (ra_reseed) is provably
// unreachable in that range. Forks tahap5_bench.c's structure -- does NOT
// modify tahap4_bench.c, tahap5_bench.c, tahap5_benchmark.py,
// winner_wired_addressable.c, winner_wired_addressable_v2.c, or any Philox
// file in benchmarks/comparisons/ in place.
//
// rot32, ra_hash, ra_permutation_cycle, ra_reseed,
// ra_init_state_addressable_baseline (== winner_wired_addressable.c's
// ra_init_state_addressable, renamed here only to sit clearly next to its
// no-L sibling), ra_core_baseline (== winner_wired_addressable.c's
// ra_core), and Philox4x32-10 (corrected PHILOX_M0 -- see tahap4_bench.c's
// header comment for the Random123 citation/rationale, unchanged here) are
// byte-for-byte copies of tahap5_bench.c's v0_baseline path. Kept
// UNCHANGED as the ground-truth control for both validation and
// benchmarking.
//
// New in this file: ra_init_state_full / ra_permutation_cycle_full /
// ra_core_singleblock -- a scoped fast path valid ONLY for rng in [1,255].
// Dead-code proof (see HANDOVER_TAHAP6.md for the full writeup): L[] is
// written by ra_init_state_addressable_baseline and shuffled by the
// L[i]<->L[d] swap in ra_permutation_cycle, but its contents are NEVER
// read by anything that influences a/b/c/d/o -- the only functional
// reader of L[]'s contents is ra_reseed's `M[i] ^= L[i]`. ra_reseed is
// only reached when `count > 1` after a ra_permutation_cycle pass, i.e.
// only when rng > 255 (iteration = rng/255+1 == 1 for rng<=255, so the
// `for` loop in ra_core_baseline runs its single iteration and returns
// before ra_reseed is ever called). So for rng<=255, L is dead weight:
// real cycles spent (half of ra_init_state_addressable_baseline's loop
// body, plus 2 loads+2 stores per hot-loop iteration for the swap, plus
// doubling the L+M stack/cache footprint to L+M instead of M alone) that
// never affect the actual output word c.
//
// d = c & 0xFFu is KEPT -- unlike the swap, it is read again on the NEXT
// loop iteration in the a/b formula (`a = (d ^ o) ^ ...`, `b = ... ^ (o +
// d)`), independent of whether L exists or the swap happens. Only its use
// as an L-swap index is removed, not d itself.
//
// ra_core_singleblock hard-aborts (fprintf+abort, not a bare assert() that
// -DNDEBUG would compile out) if called with rng>255 -- it is a scoped
// research candidate, not a general drop-in replacement for
// ra_core_baseline, and a silent wraparound/truncation would be far
// harder to catch than a loud crash.
//
// Validation gate (mandatory before trusting any speed number from this
// file, per HANDOVER_TAHAP5.md section 5-7's established convention):
// `./tahap6_bench validate` exhaustively compares ra_core_singleblock
// against ra_core_baseline word-for-word for every key in
// {0, 0xFFFFFFFF, 5 fixed "arbitrary" keys, 0..31} (39 keys) x every rng
// in 1..255 (255 values) = 9,945 combinations, in-process via fmemopen
// buffers (no subprocess/cmp needed at this combination count). If and
// only if ALL 9,945 combinations are bit-identical, the existing 128GB
// PractRand / 0-collision / 0-cross-correlation guarantees for the
// underlying formula carry over automatically to ra_core_singleblock --
// but ONLY for rng<=255 usage. ra_core_singleblock cannot even run for
// rng>255 (hard guard), so no statistical claim is ever made for that
// range.
//
// Compile:
//   gcc -O3 -march=native -std=gnu17 -include stdalign.h
//       tahap6_bench.c -o tahap6_bench -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define ADDR_L_MIX_CONST 0x9e3779b7u // Tahap 1, see HANDOVER.md

// ----------------------------------------------------------------------
// Baseline core -- byte-for-byte copy of winner_wired_addressable.c's
// (via tahap5_bench.c's v0_baseline path). Untouched, kept as ground
// truth for validation and comparison.
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

static void ra_init_state_addressable_baseline(uint32_t *L, uint32_t *M, uint32_t key) {
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

static uint32_t ra_core_baseline(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_state_addressable_baseline(L, M, key);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

// ----------------------------------------------------------------------
// Tahap 6: no-L fast path. Valid ONLY for rng in [1,255].
// ----------------------------------------------------------------------

static void ra_init_state_full(uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
        M[i] = rot32(m_val, r);
    }
}

static void ra_permutation_cycle_full(uint32_t cons, size_t it,
                                       const uint32_t *M,
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
        // No L[] swap here: L's only functional reader (ra_reseed) is
        // unreachable for rng<=255 -- see ra_core_singleblock's guard.
    }
}

static uint32_t ra_core_singleblock(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) {
        fprintf(stderr,
            "ra_core_singleblock: rng=%zu > 255 out of scope (no-reseed "
            "fast path valid only for rng in [1,255]); use ra_core_baseline.\n",
            rng);
        abort();
    }

    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;

    ra_init_state_full(M, key);
    ra_permutation_cycle_full(cons, /*it=*/0, M, &count, raw_stream);
    return cons; // == key always for rng<=255 (cons is never reassigned
                 // without ra_reseed, same as ra_core_baseline).
}

typedef uint32_t (*core_fn_t)(uint32_t key, size_t rng, FILE *raw_stream);

typedef struct { const char *name; core_fn_t fn; long max_rng; } core_entry_t;

static const core_entry_t CORES[] = {
    { "baseline", ra_core_baseline, -1 },      // -1 == no cap
    { "singleblock", ra_core_singleblock, 255 },
};

static const core_entry_t *find_core(const char *name) {
    for (size_t i = 0; i < sizeof(CORES) / sizeof(CORES[0]); ++i) {
        if (strcmp(CORES[i].name, name) == 0) return &CORES[i];
    }
    fprintf(stderr, "Unknown core '%s'. Known: ", name);
    for (size_t i = 0; i < sizeof(CORES) / sizeof(CORES[0]); ++i) {
        fprintf(stderr, "%s ", CORES[i].name);
    }
    fprintf(stderr, "\n");
    exit(1);
}

// ----------------------------------------------------------------------
// Philox4x32-10 -- byte-for-byte copy of tahap4_bench.c's / tahap5_bench.c's
// (corrected PHILOX_M0). See tahap4_bench.c header comment for citation.
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
// validate: exhaustive bit-identical check, ra_core_singleblock vs
// ra_core_baseline, for every key in {0, 0xFFFFFFFF, 5 fixed keys, 0..31}
// x every rng in 1..255. In-process via fmemopen (no subprocess/cmp
// needed at this scale). Mandatory gate before trusting ANY benchmark
// number in this file -- see header comment.
// ----------------------------------------------------------------------

static int run_validate_singleblock(void) {
    uint32_t keys[2 + 5 + 32];
    int nkeys = 0;
    keys[nkeys++] = 0u;
    keys[nkeys++] = 0xFFFFFFFFu;
    static const uint32_t extra_keys[5] = {
        0xDEADBEEFu, 0x12345678u, 0xCAFEBABEu, 0x9E3779B9u, 0x7F4A7C15u,
    };
    for (int i = 0; i < 5; ++i) keys[nkeys++] = extra_keys[i];
    for (uint32_t k = 0; k < 32; ++k) keys[nkeys++] = k;

    long total = 0, mismatches = 0;
    uint32_t buf_base[255], buf_sb[255];

    for (int ki = 0; ki < nkeys; ++ki) {
        uint32_t key = keys[ki];
        for (size_t n = 1; n <= 255; ++n) {
            FILE *fb = fmemopen(buf_base, n * sizeof(uint32_t), "wb");
            ra_core_baseline(key, n, fb);
            fclose(fb);

            FILE *fs = fmemopen(buf_sb, n * sizeof(uint32_t), "wb");
            ra_core_singleblock(key, n, fs);
            fclose(fs);

            ++total;
            if (memcmp(buf_base, buf_sb, n * sizeof(uint32_t)) != 0) {
                ++mismatches;
                fprintf(stderr, "MISMATCH key=%u n=%zu\n", key, n);
            }
        }
    }

    printf("validate-singleblock: %ld combinations checked (%d keys x 255 lengths), %ld mismatches\n",
           total, nkeys, mismatches);
    if (mismatches == 0) {
        printf("validate-singleblock: PASS -- ra_core_singleblock is bit-identical to "
               "ra_core_baseline for all rng in [1,255] across all tested keys.\n");
    } else {
        printf("validate-singleblock: FAIL -- do not trust any benchmark number from this "
               "binary; treat ra_core_singleblock as a new, unvalidated formula.\n");
    }
    return mismatches == 0 ? 0 : 1;
}

// ----------------------------------------------------------------------
// benchmark modes -- same methodology as tahap4_bench.c/tahap5_bench.c
// (min-of-trials, CLOCK_MONOTONIC, volatile checksum sink), parametrized
// by core.
// ----------------------------------------------------------------------

static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

static void mode_init_cost(const char *core_name, long repeats) {
    volatile uint32_t sink = 0;
    alignas(64) uint32_t L[256], M[256];
    double addr_total;

    if (strcmp(core_name, "baseline") == 0) {
        double t0 = now_seconds();
        for (long i = 0; i < repeats; ++i) {
            ra_init_state_addressable_baseline(L, M, (uint32_t)i);
            sink ^= L[0] ^ M[0];
        }
        double t1 = now_seconds();
        addr_total = t1 - t0;
    } else if (strcmp(core_name, "singleblock") == 0) {
        double t0 = now_seconds();
        for (long i = 0; i < repeats; ++i) {
            ra_init_state_full(M, (uint32_t)i);
            sink ^= M[0];
        }
        double t1 = now_seconds();
        addr_total = t1 - t0;
    } else {
        find_core(core_name); // unknown -- prints error and exit(1)
        return;
    }

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

    printf("mode init-cost core %s repeats %ld addr_ns_per_init %.3f philox_ns_per_call %.3f ratio %.3f checksum %u\n",
           core_name, repeats, addr_ns_per_init, philox_ns_per_call,
           addr_ns_per_init / philox_ns_per_call, sink);
}

static long trials_for_n(long long n) {
    if (n < 1000) return 200;
    if (n < 100000) return 30;
    if (n < 10000000) return 5;
    return 1;
}

static void mode_throughput(const core_entry_t *core, int argc, char **argv, int start) {
    volatile uint32_t sink = 0;
    for (int a = start; a < argc; ++a) {
        long long n = atoll(argv[a]);
        if (n <= 0) continue;
        if (core->max_rng >= 0 && n > core->max_rng) {
            fprintf(stderr,
                "throughput: N=%lld exceeds core '%s' max_rng=%ld -- refusing "
                "(sweep array must stay <= %ld for this core).\n",
                n, core->name, core->max_rng, core->max_rng);
            exit(1);
        }
        long trials = trials_for_n(n);

        double addr_best = -1.0;
        for (long t = 0; t < trials; ++t) {
            double t0 = now_seconds();
            uint32_t last_cons = core->fn(12345u, (size_t)n, NULL);
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

static void mode_reinit_sweep(const core_entry_t *core, int argc, char **argv, int start, long long cycles_target_words) {
    volatile uint32_t sink = 0;
    for (int a = start; a < argc; ++a) {
        long long k = atoll(argv[a]);
        if (k <= 0) continue;
        if (core->max_rng >= 0 && k > core->max_rng) {
            fprintf(stderr,
                "reinit-sweep: K=%lld exceeds core '%s' max_rng=%ld -- refusing "
                "(sweep array must stay <= %ld for this core).\n",
                k, core->name, core->max_rng, core->max_rng);
            exit(1);
        }

        long long cycles = cycles_target_words / k;
        if (cycles < 10) cycles = 10;
        if (cycles > 200000) cycles = 200000;

        const int TRIALS = 3;
        double addr_total = -1.0, philox_total = -1.0;

        for (int trial = 0; trial < TRIALS; ++trial) {
            double t0 = now_seconds();
            for (long long c = 0; c < cycles; ++c) {
                uint32_t key = (uint32_t)(c * 2654435761u + 1u);
                uint32_t last = core->fn(key, (size_t)k, NULL);
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
                         "  %s validate\n"
                         "  %s --stream <core> <key> <n>\n"
                         "  %s init-cost <core> <repeats>\n"
                         "  %s throughput <core> <n1> <n2> ...\n"
                         "  %s reinit-sweep <core> <cycles_target_words> <k1> <k2> ...\n"
                         "Cores: baseline singleblock (singleblock valid only for rng in [1,255])\n",
                argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "verify") == 0) {
        int ok = run_kat_checks();
        return ok ? 0 : 1;
    }
    if (strcmp(argv[1], "validate") == 0) {
        return run_validate_singleblock();
    }
    // --stream mode exists ONLY to bit-identical-check a core's output
    // against winner_wired_addressable's own --stream output (for
    // core=baseline) or against ra_core_baseline in this same file (for
    // core=singleblock, via `validate` instead -- --stream here is for
    // ad-hoc/manual inspection).
    if (strcmp(argv[1], "--stream") == 0 && argc >= 5) {
        const core_entry_t *core = find_core(argv[2]);
        uint32_t key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t rng = (size_t)strtoull(argv[4], NULL, 0);
        core->fn(key, rng, stdout);
        return 0;
    }
    if (strcmp(argv[1], "init-cost") == 0 && argc >= 4) {
        mode_init_cost(argv[2], atol(argv[3]));
        return 0;
    }
    if (strcmp(argv[1], "throughput") == 0 && argc >= 4) {
        const core_entry_t *core = find_core(argv[2]);
        mode_throughput(core, argc, argv, 3);
        return 0;
    }
    if (strcmp(argv[1], "reinit-sweep") == 0 && argc >= 5) {
        const core_entry_t *core = find_core(argv[2]);
        long long cycles_target_words = atoll(argv[3]);
        mode_reinit_sweep(core, argc, argv, 4, cycles_target_words);
        return 0;
    }

    fprintf(stderr, "Unknown mode or missing args: %s\n", argv[1]);
    return 1;
}
