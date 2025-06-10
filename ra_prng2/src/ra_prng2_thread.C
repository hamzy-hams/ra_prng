// parallel_prng.c
// g++ -O3 -march=native -fopenmp ra_prng_thread2.C -o parallel_prng2
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>
#include <time.h>

#define TOTAL_ITER 1UL
#define NUM_THREADS 1

// Rotate an 32-bit value n by r bits
static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFF;
}

void ZepXORhash(uint32_t *N, uint32_t *out8) {
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

// Core PRNG: churn state 'iterations' kali, kembalikan IV terakhir
uint32_t ZepFold(uint32_t seed, size_t iterations) {
    alignas(64) uint32_t L[256], M[256], tmp8[8];
    uint32_t cons = seed;

    // Init internal array states L and M
    for (int i = 0; i < 256; ++i) {
        M[i] = (uint32_t)(i * 0x06a0dd9bUL + 0x06a0dd9bUL);
        L[i] = (uint32_t)(i * 0x9e3779b7UL + 0x9e3779b7UL);
    }

    for (size_t it = 0; it < iterations; ++it) {
        // Variables initioation
        uint32_t  a = cons;
        uint32_t  b = it;
        uint32_t  c = 0;

        // Permutation step
        for (uint32_t i = 255; i > 0; --i) {
            uint32_t o = 0;
            for (uint8_t e = 0; e < 8; ++e) {
                o ^= (M[(uint8_t)(i + e)] << e);
            }

            a = (rot32(b ^ o, c) ^ (cons + a));
            b = (rot32(cons + a, i) ^ (o + c));
            o = (rot32(a ^ o, i) << 9 ^ (b >> 18));
            c = rot32((o + c << 14) ^ (b >> 13) ^ a, b);
            
            c = (uint32_t)(((uint64_t)c * (i + 1)) >> 32);

            // Internal state swapping
            o = L[i];
            L[i] = L[c];
            L[c] = o;
        }

        // Mixing state step
        for (uint16_t i = 0; i < 256; ++i) {
            M[i] ^= L[i];
        }

        // Hash to next seed (cons)
        ZepXORhash(M, tmp8);

        // Build next cons from bits of tmp8
        uint32_t new_cons = 0;
        for (uint8_t e = 0; e < 8; ++e) {
            new_cons ^= tmp8[e] << e;
        }
        cons = new_cons;
    }
    return cons;
}

int main(void) {
    uint32_t last_iv[NUM_THREADS];
    uint8_t worker_id[NUM_THREADS];
    size_t chunk = TOTAL_ITER / NUM_THREADS;

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    // Parallel region: each thread computes ZepFold with distinct seed
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int tid = omp_get_thread_num();
        uint32_t seed = (uint32_t)(1u << tid);
        last_iv[tid] = ZepFold(seed, chunk);
        worker_id[tid] = tid;
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    printf("Generated %lu pseudorandom updates across %d threads in %.3f seconds\n",
           (unsigned long)TOTAL_ITER, NUM_THREADS, elapsed);
    printf("Last IVs from each thread: ");
    for (int i = 0; i < NUM_THREADS; ++i) {
        printf("%u from worker (%u)", last_iv[i], worker_id[i]);
    }
    printf("\n");

    return 0;
}
