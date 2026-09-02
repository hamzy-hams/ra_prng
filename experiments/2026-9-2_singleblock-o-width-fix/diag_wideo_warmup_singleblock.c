// diag_wideo_warmup_singleblock.c
//
// Experiment: HANDOVER.md (this folder) Kandidat B -- fallback combining
// the wide-o fix (Kandidat A, see diag_wideo_singleblock.c) with a small
// internal warm-up: N rounds of the permutation cycle are run and
// DISCARDED (not written to the stream, not counted against `rng`) before
// real output begins, so a,b,c,d evolve away from the always-identical
// cold-start (a=cons=key,b=0,c=0,d=0) before any word is emitted.
//
// Triggered because Kandidat A alone (wide-o, no warm-up), when extended
// to 16GB on the VPS, showed BCFN growing back to FAIL for K=1,2,4 and
// "very suspicious" for K=8,16 -- wide-o delays/shrinks the defect by
// orders of magnitude but does not eliminate it structurally. This
// combines wide-o with a small discarded warm-up to see if the needed N
// is small enough to be an acceptable overhead (the old warm-up-only fix,
// using narrow-o, needed N>=128 of 255, i.e. 5.5-13x overhead, and was
// rejected -- see project_multikey_remix_search memory).
//
// Self-contained: copies rot32/fmix32/GUARD_L/GUARD_M/
// ra_init_state_singleblock byte-for-byte from
// ../2026-9-1_family-productionization/ra_core.c (unchanged). Does not
// touch ra_core.c. ra_permutation_cycle_singleblock is `static` there, so
// this file re-implements the cycle inline (single unified loop, see
// below) rather than extern-linking.
//
// The warm-up-then-continue semantics are ONE continuous i=255..1 loop:
// the first N iterations update a,b,c,d but are discarded (no fwrite, no
// `count` decrement, no break-check); the remaining iterations behave
// exactly like the normal cycle, continuing from the warm-up's a,b,c,d
// (not reset). At K=1 this gives exactly (N+1) total rounds executed per
// real output word: N discarded + 1 that writes then immediately breaks
// (count<=1).
//
// CLI:
//   diag_wideo_warmup_singleblock --stream <narrow|wideo> <base_key> <n> <K> <N>
//       Chains blocks (Weyl increment 0x9E3779B9), each block runs N
//       discarded warm-up rounds then up to K real output rounds. Writes
//       raw uint32 stream to stdout for RNG_test stdin32.
//   diag_wideo_warmup_singleblock --single <narrow|wideo> <key> <rng> <N>
//       Single call, decimal words to stdout (sanity check; N=0 must
//       match diag_wideo_singleblock's --single output byte-for-byte).
//   diag_wideo_warmup_singleblock --bench <narrow|wideo> <cycles_target_words> <K> <N>
//       Reinit-sweep microbench (mirrors bench_ra_core.c's
//       mode_reinit_sweep), reports ns_per_word including warm-up cost.
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h
//          diag_wideo_warmup_singleblock.c -o diag_wideo_warmup_singleblock -Wall -Wextra

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

static inline uint32_t narrow_o(const uint32_t *M, uint32_t i) {
    return (M[(uint8_t)(i + 6)] << 6) ^ (M[(uint8_t)(i + 7)] << 7);
}

static inline uint32_t wide_o(const uint32_t *M, uint32_t i) {
    return (M[(uint8_t)(i + 0)] << 0) ^ (M[(uint8_t)(i + 1)] << 1) ^
           (M[(uint8_t)(i + 2)] << 2) ^ (M[(uint8_t)(i + 3)] << 3) ^
           (M[(uint8_t)(i + 4)] << 4) ^ (M[(uint8_t)(i + 5)] << 5) ^
           (M[(uint8_t)(i + 6)] << 6) ^ (M[(uint8_t)(i + 7)] << 7);
}

// Single unified loop: first N iterations are discarded warm-up (update
// a,b,c,d only), remaining iterations behave like the normal cycle,
// continuing from the warm-up's final a,b,c,d (no reset in between).
static uint32_t ra_core_singleblock_warmup(uint32_t key, size_t rng,
                                            size_t N, int use_wide,
                                            FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    ra_init_state_singleblock(M, key);

    uint32_t cons = key;
    uint32_t a = cons, b = 0, c = 0, d = 0;
    uint64_t count = rng;
    size_t warmup_left = N;

    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = use_wide ? wide_o(M, i) : narrow_o(M, i);

        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);

        if (warmup_left > 0) {
            --warmup_left;
            d = c & 0xFFu;
            continue;
        }

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & 0xFFu;

        if (count <= 1) break;
        --count;
    }
    return cons;
}

static int pick_use_wide(const char *name) {
    if (strcmp(name, "narrow") == 0) return 0;
    if (strcmp(name, "wideo") == 0) return 1;
    fprintf(stderr, "unknown variant '%s' (want narrow|wideo)\n", name);
    exit(1);
}

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--stream") == 0 && argc >= 7) {
        int use_wide = pick_use_wide(argv[2]);
        uint32_t base_key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t n = (size_t)strtoull(argv[4], NULL, 0);
        size_t K = (size_t)strtoull(argv[5], NULL, 0);
        size_t N = (size_t)strtoull(argv[6], NULL, 0);
        if (K == 0 || K > 255) {
            fprintf(stderr, "K must be in [1,255]\n");
            return 1;
        }
        uint64_t block_idx = 0;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < K ? remaining : K;
            uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
            ra_core_singleblock_warmup(key, block_len, N, use_wide, stdout);
            remaining -= block_len;
            ++block_idx;
        }
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--single") == 0 && argc >= 6) {
        int use_wide = pick_use_wide(argv[2]);
        uint32_t key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t rng = (size_t)strtoull(argv[4], NULL, 0);
        size_t N = (size_t)strtoull(argv[5], NULL, 0);
        ra_core_singleblock_warmup(key, rng, N, use_wide, stdout);
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--bench") == 0 && argc >= 6) {
        int use_wide = pick_use_wide(argv[2]);
        long long cycles_target_words = atoll(argv[3]);
        long long k = atoll(argv[4]);
        size_t N = (size_t)strtoull(argv[5], NULL, 0);
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
                sink ^= ra_core_singleblock_warmup(key, (size_t)k, N,
                                                    use_wide, NULL);
            }
            double t1 = now_seconds();
            double dt = t1 - t0;
            if (best < 0 || dt < best) best = dt;
        }

        double ns_per_word = best * 1e9 / (double)(cycles * k);
        fprintf(stderr, "sink=%u\n", sink);
        printf("%s K %lld N %zu cycles %lld ns_per_word %.4f\n",
               argv[2], k, N, cycles, ns_per_word);
        return 0;
    }
    fprintf(stderr,
        "usage:\n"
        "  %s --stream <narrow|wideo> <base_key> <n> <K> <N>\n"
        "  %s --single <narrow|wideo> <key> <rng> <N>\n"
        "  %s --bench <narrow|wideo> <cycles_target_words> <K> <N>\n",
        argv[0], argv[0], argv[0]);
    return 1;
}
