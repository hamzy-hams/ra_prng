// ra_prng3.c
// g++ -O3 -march=native ra_prng3.C -o prng3
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define TOTAL_RNG 255000000UL

// Rotate an 64-bit value n by r bits
static inline uint64_t rot64(uint64_t n, uint64_t r) {
    r &= 63;
    return ((n << r) | (n >> (64 - r))) ;
}

uint64_t ZepXORhash(uint64_t *M) {
    uint64_t f = 0;
    for (uint8_t i = 0; i < 8; ++i) {
        uint8_t  r   = M[i];
        uint64_t tmp = M[i] ^ M[r];
        for (uint8_t j = 0; j < 32; ++j) {
            tmp ^= M[j * 8 + i];
        }
        f ^= tmp;
    }
    return f;
}


// Core PRNG: churn state 'iterations' times, return last seed
uint64_t ZepFold(uint64_t seed, size_t rng) {

    if (rng == 0) {
        return seed;
    }

    alignas(64) uint64_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;
    // Init internal array states L and M
    uint64_t cons = seed;
    for (int i = 0; i < 256; ++i) {
        M[i] = (uint64_t)(i * 0x06a0dd9b9e3779b7UL + 0x06a0dd9b9e3779b7UL);
        L[i] = (uint64_t)(i * 0x9e3779b706a0dd9bUL + 0x9e3779b706a0dd9bUL);
    }

    for (size_t it = 0; it < iteration; ++it) {
        // Variables initioation
        uint64_t  a = cons;
        uint64_t  b = it;
        uint64_t  c = 0;
        uint64_t  d = 0;
        uint8_t   e = 0;

        // Permutation step
        for (uint64_t i = 255; i > 0; --i) {
            uint64_t o = M[i];

            a  = rot64(b ^ o, c) ^ (cons + a);
            b  = rot64(cons + a, i) ^ (o + c);
            o  = rot64(a ^ o, b) ^ (b >> 37);
            c  = (o << 18) ^ (b >> 29) ^ a;
            d  = rot64(L[i] ^ (b << 59), c);
            // fair random indexing using lemire fast reduction method
            e = (uint32_t)(c) * (i + 1) >> 32;
            
            // Internal state swapping and rotating
            L[i] = L[e];
            L[e] = d;

            // printf("%lu ", d);

            if (count <= 1) {
                break;
            }
            --count;
        }

        if (count <= 1) {
            return cons;
        }
        // Mixing state step
        for (uint16_t i = 0; i < 256; ++i) {
            M[i] ^= L[i];
        }

        // Hash to next seed (cons)
        cons = ZepXORhash(M);
    }
    return cons;
}

int main(void) {
    uint64_t last_cons;
    uint64_t seed;

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    // Parallel region: each thread computes ZepFold with distinct seed
    seed = 1;
    last_cons = ZepFold(seed, TOTAL_RNG);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    printf("Generated %lu pseudorandom updates across threads in %.3f seconds\n",
           (unsigned long)TOTAL_RNG, elapsed);
    printf("Last cons from RNGing: %lu", last_cons);
    printf("\n");

    return 0;
}
