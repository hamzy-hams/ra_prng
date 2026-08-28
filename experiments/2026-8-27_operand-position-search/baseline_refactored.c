// pruned_winner_refactored.c
// Micro-refactor of pruned_winner.c (kept untouched as the validated
// reference) -- same winning candidate (ops={TAP6,TAP7,ROT_C,SHR13},
// hash_access=sequential, original shift widths), only `ra_permutation_cycle`
// changes:
//
//   1. Sliding-window TAP6/TAP7 load reuse: M is `const` and never written
//      inside this function, and the loop counter `i` decrements by 1 each
//      step, so idx7(i) = (i+7)&0xFF == idx6(i+1) = ((i+1)+6)&0xFF -- i.e.
//      the TAP7 index this iteration is *always* the TAP6 index from the
//      iteration immediately before it. One of the two per-iteration array
//      reads is therefore provably redundant; it's replaced by carrying the
//      previous iteration's TAP6 load forward in a register (`m7`), primed
//      once before the loop with M[6] (== idx6(256), the index the
//      nonexistent "iteration 256" would have read).
//   2. Dropped the dead `c = 0` initialization before the loop (never read
//      before being overwritten by `c = rot32(...)` in the first
//      iteration) -- `c` is now scoped inside the loop body instead.
//
// ra_core/ra_reseed/ra_init_state/ra_hash/main are byte-for-byte identical
// to pruned_winner.c -- deliberately not restructured, since that file's
// whole point is a fair, apples-to-apples `perf stat` mirror of
// src/ra_prng2/c/ra_prng2.c. Must be validated bit-identical against
// pruned_winner.c before any perf number from this file is trusted.
//
// Copyright (c) 2025 Hamas A. Rahman (derivative research variant)
// Licensed under CC BY-NC-SA 4.0, matching the original this derives from.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define TOTAL_RNG 200000000UL // matches src/ra_prng2/c/ra_prng2.c's benchmark constant

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

// hash_access=sequential, HASH_SELFIDX off, HASH_DEPTH fixed at 32.
static void ra_hash(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}

static void ra_init_state(uint32_t *L, uint32_t *M) {
    for (int i = 0; i < 256; ++i) {
        M[i] = (uint32_t)(i * 0x06a0dd9bUL + 0x06a0dd9bUL);
        L[i] = (uint32_t)(i * 0x9e3779b7UL + 0x9e3779b7UL);
    }
}

// One full 255-step permutation cycle, sliding-window TAP6/TAP7 reuse.
static void ra_permutation_cycle(uint32_t cons, size_t it,
                                  const uint32_t *M, uint32_t *L,
                                  uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, d = 0;

    // Priming read: represents the TAP6 load that "iteration i=256" would
    // have done, i.e. M[(256+6)&0xFF] == M[6]. This becomes iteration
    // i=255's TAP7 value.
    uint32_t m7 = M[6];

    for (uint32_t i = 255; i > 0; --i) {
        uint32_t m6 = M[(uint8_t)(i + 6)];
        uint32_t o = (m6 << 6) ^ (m7 << 7);
        m7 = m6; // carried forward: becomes next iteration's TAP7 value

        a = (b ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        uint32_t c = rot32((b >> 13) ^ a, b);

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

uint32_t ra_core(uint32_t seed, size_t rng, FILE *raw_stream) {
    if (rng == 0) return seed;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = seed;
    ra_init_state(L, M);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

int main(int argc, char **argv) {
    uint32_t last_cons;
    uint32_t seed;

    if (argc >= 4 && strcmp(argv[1], "--stream") == 0) {
        seed = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t rng = (size_t)strtoull(argv[3], NULL, 0);

        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        last_cons = ra_core(seed, rng, stdout);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

        fprintf(stderr, "Streamed %zu pseudorandom outputs in %.3f seconds\n", rng, elapsed);
        fprintf(stderr, "Last cons from RNGing: %u\n", last_cons);
        return 0;
    }

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    seed = 1;
    last_cons = ra_core(seed, TOTAL_RNG, NULL);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    printf("Generated %lu pseudorandom updates in %.3f seconds\n",
           (unsigned long)TOTAL_RNG, elapsed);
    printf("Last cons from RNGing: %lu\n", (unsigned long)last_cons);

    return 0;
}
