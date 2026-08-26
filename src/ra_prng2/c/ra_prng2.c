// ra_prng2.C
// Copyright (c) 2025 Hamas A. Rahman
// Licensed under CC BY-NC-SA 4.0
// github.com/hamzy-hams
// g++ -O3 -march=native ra_prng2.C -o prng2

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define TOTAL_RNG 200000000UL //example

// Rotate an 32-bit value n by r bits
static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFF;
}

void ra_hash(uint32_t *N, uint32_t *out8) {
    // XOR each of the 32 blocks into the first 8 bytes
    for (uint8_t i = 0; i < 8; ++i) out8[i] = 0;
    for (uint8_t i = 0; i < 8; ++i) {
        //idx = N[i] & 0xFF;
        out8[i] ^= N[(uint8_t)N[i]];
        for (uint8_t j = 0; j < 32; j++) {
            out8[i] ^= N[j * 8 + i];
        }
    }
}

static void ra_init_state(uint32_t *L, uint32_t *M) {
    for (int i = 0; i < 256; ++i) {
        M[i] = (uint32_t)(i * 0x06a0dd9bUL + 0x06a0dd9bUL);
        L[i] = (uint32_t)(i * 0x9e3779b7UL + 0x9e3779b7UL);
    }
}

// One full 255-step permutation cycle. If raw_stream is non-NULL, every `c`
// value (the real per-step RNG output, per the paper) is fwrite'n to it as
// raw uint32_t as soon as it's computed. *count is mutated in place exactly
// like the original inline loop did, so the caller's post-call
// `count <= 1` check preserves the original truncation semantics exactly.
static void ra_permutation_cycle(uint32_t cons, size_t it,
                                  const uint32_t *M, uint32_t *L,
                                  uint64_t *count, FILE *raw_stream) {
    // Variables initiation
    uint32_t  a = cons;
    uint32_t  b = it;
    uint32_t  c = 0;
    uint32_t  d = 0;

    // Permutation step
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = 0;
        for (uint8_t e = 0; e < 8; ++e) {
            o ^= (M[(uint8_t)(i + e)] << e);
        }

        a = (rot32(b ^ o, d) ^ (cons + a));
        b = (rot32(cons + a, i) ^ (o + d));
        o = (rot32(a ^ o, i) << 9 ^ (b >> 18));
        c = rot32((o + c << 14) ^ (b >> 13) ^ a, b);
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = (uint32_t)(((uint64_t)c * (i + 1)) >> 32);

        if (*count <= 1) {
            break;
        }
        --(*count);

        // Internal state swapping
        uint32_t tmp = L[i];
        L[i] = L[d];
        L[d] = tmp;
    }
}

// Mix M with L, hash the result, and derive the next cons -- one
// self-contained step so the caller never has to see the tmp8 intermediate.
static uint32_t ra_reseed(uint32_t *M, const uint32_t *L) {
    uint32_t tmp8[8];

    // Mixing state step
    for (uint16_t i = 0; i < 256; ++i) {
        M[i] ^= L[i];
    }

    // Hash to next seed (cons)
    ra_hash(M, tmp8);

    // Build next cons from bits of tmp8
    uint32_t new_cons = 0;
    for (uint8_t e = 0; e < 8; ++e) {
        new_cons ^= tmp8[e] << e;
    }
    return new_cons;
}

// Core PRNG: churn state 'iterations' times, return last cons.
// raw_stream == NULL preserves the original behavior/performance exactly.
uint32_t ra_core(uint32_t seed, size_t rng, FILE *raw_stream) {

    if (rng == 0) {
        return seed;
    }

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    // Init internal array states L and M
    uint32_t cons = seed;
    ra_init_state(L, M);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle(cons, it, M, L, &count, raw_stream);

        if (count <= 1) {
            return cons;
        }

        // Reseed: mix state, hash, and derive next cons
        cons = ra_reseed(M, L);
    }
    return cons;
}

int main(int argc, char **argv) {
    uint32_t last_cons;
    uint32_t seed;

    if (argc >= 4 && strcmp(argv[1], "--stream") == 0) {
        // Streaming mode: dump the raw `c` stream to stdout so it can be
        // piped straight into dieharder/PractRand/etc. Status text goes to
        // stderr so stdout stays pure binary.
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

    printf("Generated %lu pseudorandom updates across threads in %.3f seconds\n",
           (unsigned long)TOTAL_RNG, elapsed);
    printf("Last cons from RNGing: %lu", last_cons);
    printf("\n");

    return 0;
}
