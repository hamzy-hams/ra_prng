// pruned_winner.c
// Specialized, hardcoded (no runtime op-flag branching) C implementation of
// the winning operation-pruning candidate: ops={TAP6,TAP7,ROT_C,SHR13},
// hash_access=sequential, original shift width (13) for SHR13. 14 of 18
// tracked operations removed relative to the baseline; validated clean up
// to 8GB PractRand (seed=1) and 3 extra seeds at 1GB -- see STATUS.md/
// RESULTS.md "MIN_ACTIVE_ROTS constraint: SUCCESS".
//
// Written as a direct structural mirror of src/ra_prng2/c/ra_prng2.c (same
// file shape: rot32, ra_hash, ra_init_state, ra_permutation_cycle,
// ra_reseed, ra_core, main with the same TOTAL_RNG/--stream CLI) so that
// `perf stat` numbers are comparable apples-to-apples against the original
// -- NOT compiled from pruned_prng.c's generic bitmask-branching harness,
// which has runtime `has()` branch checks the original doesn't and would
// bias any speed comparison against the pruning itself.
//
// Derivation (from pruned_prng.py's permutation_cycle with only
// TAP6,TAP7,ROT_C,SHR13 active -- verified bit-identical against
// pruned_prng.c/pruned_prng.py with this exact op set before trusting this
// file, see the self-check in this session):
//   o = (M[(i+6)&0xFF] << 6) ^ (M[(i+7)&0xFF] << 7)
//   a = (b ^ o) ^ (cons + a)
//   b = (cons + a) ^ (o + d)
//   c = rot32((b >> 13) ^ a, b)
//   d = c & 0xFF
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

// One full 255-step permutation cycle, the pruned candidate's operation set.
static void ra_permutation_cycle(uint32_t cons, size_t it,
                                  const uint32_t *M, uint32_t *L,
                                  uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = (M[(uint8_t)(i + 6)] << 6) ^ (M[(uint8_t)(i + 7)] << 7);

        a = (b ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((b >> 13) ^ a, b);

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
