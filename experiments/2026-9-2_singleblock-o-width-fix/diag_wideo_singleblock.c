// diag_wideo_singleblock.c
//
// Experiment: HANDOVER.md (this folder) Kandidat A -- widen `o` in
// ra_permutation_cycle_singleblock from 2 M[] taps to 8, restoring the
// original pre-pruning build_o() shape from
// ../2026-8-26_operation-pruning-research/pruned_prng.c (build_o with all
// TAP0-7 on), to see if a richer-entropy first round closes the K-small
// BCFN defect characterized in
// ../2026-9-2_singleblock-k-threshold-characterization/RESULTS.md.
//
// Self-contained diagnostic: copies (does not modify) rot32/fmix32/
// GUARD_L/GUARD_M/ra_init_state_singleblock byte-for-byte from
// ../2026-9-1_family-productionization/ra_core.c (lines 86-98, 100-105,
// 205-212). ra_core.c itself is NOT touched by this experiment.
// ra_permutation_cycle_singleblock is `static` in ra_core.c, so it cannot
// be extern-linked -- hence the verbatim copy here (pattern established by
// ../2026-9-2_singleblock-prereseed-experiment/diag_prereseed.c).
//
// Two candidate cores, selectable via CLI, for same-run A/B:
//   - "narrow" : byte-for-byte ra_core_singleblock as it is in ra_core.c
//                right now (o = 2 taps, M[i+6]<<6 ^ M[i+7]<<7). Control.
//   - "wideo"  : ONLY the `o = ...` line changed to an 8-tap XOR
//                (all TAP0-7 on). a/b/c/d update formulas UNCHANGED. No
//                other pruned_prng.c ops (ROT_A/ROT_B/SHL9/etc.) added,
//                no L[] swap.
//
// CLI:
//   diag_wideo_singleblock --stream <narrow|wideo> <base_key> <n> <K>
//       Chains ra_core_singleblock_<variant>(key, block_len<=K, stdout)
//       calls, key derived via Weyl/golden-ratio increment (0x9E3779B9),
//       writes raw uint32 stream to stdout (for piping into PractRand's
//       RNG_test stdin32). Writes ALL words, no discard.
//   diag_wideo_singleblock --single <narrow|wideo> <key> <rng>
//       Single call, prints each output word as decimal to stdout (for
//       sanity checks).
//   diag_wideo_singleblock --bench <narrow|wideo> <cycles_target_words> <K>
//       Reinit-sweep microbench, mirrors
//       ../2026-9-1_family-productionization/bench_ra_core.c's
//       mode_reinit_sweep: cycles = clamp(cycles_target_words/K,10,200000),
//       per-cycle key = c*2654435761u+1u, TRIALS=3 min-of-trials timing,
//       reports ns_per_word.
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h
//          diag_wideo_singleblock.c -o diag_wideo_singleblock -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

static inline uint32_t fmix32(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6bu;
    h ^= h >> 13;
    h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}

#define GUARD_L 0x38916df4u
#define GUARD_M 0x6c26fc92u

static void ra_init_state_singleblock(uint32_t *M, uint32_t key) {
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32(m_val, r);
    }
}

// ---- narrow (= production ra_permutation_cycle_singleblock, unchanged) ----
static void ra_permutation_cycle_singleblock_narrow(uint32_t cons, size_t it,
                                                     const uint32_t *M,
                                                     uint64_t *count,
                                                     FILE *raw_stream) {
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
    }
}

// ---- wideo: ONLY the o= line differs from narrow -- 8-tap XOR mirroring
// pruned_prng.c's build_o() with all TAP0-7 on. a/b/c/d formulas identical
// to narrow, isolating o-width as the sole experimental variable. ----
static void ra_permutation_cycle_singleblock_wideo(uint32_t cons, size_t it,
                                                    const uint32_t *M,
                                                    uint64_t *count,
                                                    FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = (M[(uint8_t)(i + 0)] << 0) ^ (M[(uint8_t)(i + 1)] << 1) ^
                     (M[(uint8_t)(i + 2)] << 2) ^ (M[(uint8_t)(i + 3)] << 3) ^
                     (M[(uint8_t)(i + 4)] << 4) ^ (M[(uint8_t)(i + 5)] << 5) ^
                     (M[(uint8_t)(i + 6)] << 6) ^ (M[(uint8_t)(i + 7)] << 7);

        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & 0xFFu;

        if (*count <= 1) break;
        --(*count);
    }
}

static uint32_t ra_core_singleblock_narrow(uint32_t key, size_t rng,
                                            FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_singleblock_narrow(cons, 0, M, &count, raw_stream);
    return cons;
}

static uint32_t ra_core_singleblock_wideo(uint32_t key, size_t rng,
                                           FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_singleblock_wideo(cons, 0, M, &count, raw_stream);
    return cons;
}

typedef uint32_t (*core_fn_t)(uint32_t key, size_t rng, FILE *raw_stream);

static core_fn_t pick_variant(const char *name) {
    if (strcmp(name, "narrow") == 0) return ra_core_singleblock_narrow;
    if (strcmp(name, "wideo") == 0) return ra_core_singleblock_wideo;
    fprintf(stderr, "unknown variant '%s' (want narrow|wideo)\n", name);
    exit(1);
}

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--stream") == 0 && argc >= 6) {
        core_fn_t fn = pick_variant(argv[2]);
        uint32_t base_key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t n = (size_t)strtoull(argv[4], NULL, 0);
        size_t K = (size_t)strtoull(argv[5], NULL, 0);
        if (K == 0 || K > 255) {
            fprintf(stderr, "K must be in [1,255]\n");
            return 1;
        }
        uint64_t block_idx = 0;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < K ? remaining : K;
            uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
            fn(key, block_len, stdout);
            remaining -= block_len;
            ++block_idx;
        }
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--single") == 0 && argc >= 5) {
        core_fn_t fn = pick_variant(argv[2]);
        uint32_t key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t rng = (size_t)strtoull(argv[4], NULL, 0);
        fn(key, rng, stdout);
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--bench") == 0 && argc >= 5) {
        core_fn_t fn = pick_variant(argv[2]);
        long long cycles_target_words = atoll(argv[3]);
        long long k = atoll(argv[4]);
        if (k <= 0) { fprintf(stderr, "K must be > 0\n"); return 1; }

        long long cycles = cycles_target_words / k;
        if (cycles < 10) cycles = 10;
        if (cycles > 200000) cycles = 200000;

        const int TRIALS = 3;
        double best = -1.0;
        volatile uint32_t sink = 0;
        for (int trial = 0; trial < TRIALS; ++trial) {
            double t0 = now_seconds();
            for (long long c = 0; c < cycles; ++c) {
                uint32_t key = (uint32_t)(c * 2654435761u + 1u);
                sink ^= fn(key, (size_t)k, NULL);
            }
            double t1 = now_seconds();
            double dt = t1 - t0;
            if (best < 0 || dt < best) best = dt;
        }

        double ns_per_word = best * 1e9 / (double)(cycles * k);
        fprintf(stderr, "sink=%u\n", sink);
        printf("%s K %lld cycles %lld ns_per_word %.4f\n",
               argv[2], k, cycles, ns_per_word);
        return 0;
    }
    fprintf(stderr,
        "usage:\n"
        "  %s --stream <narrow|wideo> <base_key> <n> <K>\n"
        "  %s --single <narrow|wideo> <key> <rng>\n"
        "  %s --bench <narrow|wideo> <cycles_target_words> <K>\n",
        argv[0], argv[0], argv[0]);
    return 1;
}
