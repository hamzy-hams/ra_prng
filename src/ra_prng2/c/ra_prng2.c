// ra_prng2.c
// Copyright (c) 2025 Hamas A. Rahman
// Licensed under CC BY-NC-SA 4.0
// github.com/hamzy-hams
// gcc -O3 -march=native ra_prng2.c -o prng2

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define TOTAL_RNG 1999

// Rotate a 32-bit value n by r bits
static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31u;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

// Hash M[256] -> out8[8]: nonlinear XOR reduction
static void ra_hash(uint32_t *N, uint32_t *out8) {
    uint32_t idx;
    for (uint8_t i = 0; i < 8; ++i) out8[i] = 0;
    for (uint8_t i = 0; i < 8; ++i) {
        idx = N[i] & 0xFF;
        out8[i] ^= N[idx];
        for (uint8_t j = 0; j < 32; ++j) {
            out8[i] ^= N[j * 8 + i];
        }
    }
}

// Core PRNG: run 'iterations' outer iterations, return last cons
uint32_t ra_core(uint32_t seed, size_t iterations) {
    if (iterations == 0) {
        return seed;
    }

    uint32_t L[256] __attribute__((aligned(64))), M[256] __attribute__((aligned(64))), tmp8[8];
    uint32_t cons = seed;

    // Array state initialization
    for (int i = 0; i < 256; ++i) {
        M[i] = (uint32_t)(i * 0x06a0dd9bUL + 0x06a0dd9bUL);
        L[i] = (uint32_t)(i * 0x9e3779b7UL + 0x9e3779b7UL);
    }

    for (size_t it = 0; it < iterations; ++it) {
        uint32_t a = cons;
        uint32_t b = (uint32_t)it;
        uint32_t c = 0;
        uint32_t d = 0;

        // Permutation step: inner loop i from 255 down to 1
        for (uint32_t i = 255; i > 0; --i) {
            // Aggregate mixer o from M
            uint32_t o = 0;
            for (uint8_t e = 0; e < 8; ++e) {
                o ^= (M[(uint8_t)(i + e)] << e);
            }

            // Core transforms
            a = (rot32(b ^ o, d) ^ (cons + a));
            b = (rot32(cons + a, i) ^ (o + d));
            o = (rot32(a ^ o, i) << 9) ^ (b >> 18);
            c = rot32((o + (c << 14)) ^ (b >> 13) ^ a, b);

            // Lemire's fast reduction: random index d in [0, i]
            d = (uint32_t)(((uint64_t)c * (uint64_t)(i + 1)) >> 32);

            // Swap L[i] <-> L[d]
            uint32_t tmp = L[i];
            L[i] = L[d];
            L[d] = tmp;
        }

        // Mix M ^= L (entropy diffusion)
        for (int i = 0; i < 256; ++i) {
            M[i] ^= L[i];
        }

        // Hash M to produce next cons
        ra_hash(M, tmp8);

        uint32_t new_cons = 0;
        for (uint8_t e = 0; e < 8; ++e) {
            new_cons ^= (tmp8[e] << e);
        }
        cons = new_cons;
    }

    return cons;
}

int main(void) {
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    uint32_t last_cons = ra_core(1, TOTAL_RNG);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    printf("Generated %lu outer iterations in %.3f seconds\n",
           (unsigned long)TOTAL_RNG, elapsed);
    printf("Last cons: %u\n", last_cons);

    return 0;
}