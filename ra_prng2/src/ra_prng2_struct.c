// ra_prng_struct.C
// (based on original by Hamas A. Rahman)
// Compile: g++ -O3 -march=native ra_prng_struct.C -o prng_struct

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define STATE_SIZE 256

// Rotate a 32-bit value n by r bits
static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31u;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

// Hash routine ported exactly from original ra_hash
static void ra_hash(const uint32_t *N, uint32_t *out8) {
    // initialize
    for (uint8_t i = 0; i < 8; ++i) out8[i] = 0u;

    for (uint8_t i = 0; i < 8; ++i) {
        out8[i] ^= N[(uint8_t)N[i]]; // index wrap via cast to uint8_t
        for (uint8_t j = 0; j < 32; ++j) {
            out8[i] ^= N[(uint32_t)j * 8u + i];
        }
    }
}

// Struct that contains entire PRNG internal state
typedef struct {
    uint32_t L[STATE_SIZE] __attribute__((aligned(64)));
    uint32_t M[STATE_SIZE] __attribute__((aligned(64)));
    uint32_t tmp8[8];
    uint32_t cons;                 // current "constant" / seed-like value
    uint64_t outputs_generated;    // how many outer iterations (cons produced)
    uint64_t remaining_count;      // if >0, decrement across inner iterations (mimic original count). 0 = unlimited
} RA_PRNG;

// Initialize struct (like beginning of ra_core)
void ra_prng_init(RA_PRNG *r, uint32_t seed, uint64_t remaining_count /*0 = unlimited*/) {
    r->cons = seed;
    r->outputs_generated = 0;
    r->remaining_count = remaining_count;

    for (int i = 0; i < STATE_SIZE; ++i) {
        r->M[i] = (uint32_t)(i * 0x06a0dd9bUL + 0x06a0dd9bUL);
        r->L[i] = (uint32_t)(i * 0x9e3779b7UL + 0x9e3779b7UL);
    }
    // tmp8 left uninitialized until used
}

// Perform one outer-iteration (one production of next cons).
// Returns the new cons.
uint32_t ra_prng_next(RA_PRNG *r) {
    // Equivalent to one iteration of outer loop in original ra_core
    uint32_t a = r->cons;
    uint32_t b = (uint32_t)r->outputs_generated; // "it" in original code
    uint32_t c = 0u;
    uint32_t d = 0u;

    // inner permutation loop: i from 255 down to 1
    for (uint32_t i = 255u; i > 0u; --i) {
        uint32_t o = 0u;
        // o ^= (M[(uint8_t)(i + e)] << e) for e in 0..7
        for (uint8_t e = 0; e < 8; ++e) {
            uint8_t idx = (uint8_t)(i + e); // wrap mod 256
            o ^= (r->M[idx] << e);
        }

        a = (rot32(b ^ o, d) ^ (r->cons + a));
        b = (rot32(r->cons + a, i) ^ (o + d));
        o = (rot32(a ^ o, i) << 9) ^ (b >> 18);
        // careful with precedence: original had o = (rot32(a ^ o, i) << 9 ^ (b >> 18));
        // to match C semantics we compute as above
        c = rot32(((o + c) << 14) ^ (b >> 13) ^ a, b);

        // multiply to get top 32 bits as original
        d = (uint32_t)(((uint64_t)c * (uint64_t)(i + 1u)) >> 32);

        // If remaining_count is used, decrement and possibly break (mimic original)
        if (r->remaining_count > 0) {
            if (r->remaining_count <= 1) {
                // In original code when count <= 1, it breaks inner loop and returns cons.
                // We'll early-return current cons without finishing this outer iteration.
                return r->cons;
            }
            --(r->remaining_count);
        }

        // Internal state swapping L[i] <-> L[d]
        uint32_t tmp = r->L[i];
        r->L[i] = r->L[d];
        r->L[d] = tmp;
    }

    // After inner loop finishes, mix M ^= L
    for (uint32_t i = 0u; i < STATE_SIZE; ++i) {
        r->M[i] ^= r->L[i];
    }

    // Hash to next cons
    ra_hash(r->M, r->tmp8);

    // Build next cons from bits of tmp8: new_cons ^= tmp8[e] << e
    uint32_t new_cons = 0u;
    for (uint8_t e = 0; e < 8; ++e) {
        new_cons ^= (r->tmp8[e] << e);
    }

    r->cons = new_cons;
    r->outputs_generated++;

    return r->cons;
}

// Convenience: run many outer iterations (like ra_core with rng parameter).
// Runs up to 'iterations' times or until remaining_count causes early return.
// Returns last cons.
uint32_t ra_prng_advance(RA_PRNG *r, size_t iterations) {
    uint32_t last = r->cons;
    for (size_t k = 0; k < iterations; ++k) {
        last = ra_prng_next(r);
    }
    return last;
}

// Demonstrasi: panggil next() dan cetak 25 nilai
int main(void) {
    RA_PRNG rng;
    uint32_t seed;
    printf("seed: ");
    scanf("%d", &seed);
    // remaining_count = 0 => unlimited (no early stopping). 
    // Jika ingin meniru TOTAL_RNG dari kode asli, set remaining_count = TOTAL_RNG;
    ra_prng_init(&rng, seed, 0u);

    printf("Generating outputs from RA_PRNG (seed=%u)\n", seed);
    for (int i = 0; i < 5; ++i) {
        uint32_t v = ra_prng_next(&rng);
        printf("[%2d] cons = %10u (0x%08X)\n", i, v, v);
    }
    return 0;
}
