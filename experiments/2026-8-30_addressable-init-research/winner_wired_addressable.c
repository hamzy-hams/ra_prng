// winner_wired_addressable.c
// Tahap 2 (HANDOVER.md): C implementation of the Tahap 1-frozen addressable
// init spec (Kandidat 5). NEW FILE -- does not modify winner_wired_v2.c (or
// any winner_wired* variant) in place, per this research's read-only
// constraint. `ra_permutation_cycle`, `ra_reseed`, `ra_core`, `rot32`,
// `ra_hash` are byte-for-byte copies of winner_wired_v2.c's (verified via
// this project's Python port + sanity_check() before this file was written,
// see ../2026-8-30_addressable-init-research/RESULTS.md). The ONLY change is
// `ra_init_state` -> `ra_init_state_addressable(L, M, key)`, which makes the
// initial (L, M) state a function of `key` instead of a fixed formula --
// "continue" mode (no reinit across the 255-word reseed cycles) is therefore
// identical in mechanism to winner_wired_v2.c, since ra_permutation_cycle/
// ra_reseed/ra_core's loop structure is untouched.
//
// Naming (Tahap 1, 2026-08-30): the per-address parameter is `key` (Philox
// terminology), not `seed`. `ADDR_L_MIX_CONST` is a frozen mixing constant,
// not an address dimension -- `counter` is reserved for a possible future
// second address axis and is NOT used here (address = key only, confirmed
// by user in Tahap 1).
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h
//          winner_wired_addressable.c -o winner_wired_addressable
// (same flags as winner_wired.c/winner_wired_v2.c, see
// ../2026-8-27_operand-position-search/RESULTS.md)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define TOTAL_RNG 200000000UL // matches winner_wired_v2.c's benchmark constant
#define ADDR_L_MIX_CONST 0x9e3779b7u // Tahap 1: frozen, see HANDOVER.md

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

// hash_access=sequential, HASH_SELFIDX off, HASH_DEPTH fixed at 32.
// Byte-for-byte copy of winner_wired_v2.c's ra_hash().
static void ra_hash(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}

// Tahap 1 frozen formula (HANDOVER.md "Tahap 1: Spesifikasi Final"). Kandidat
// 5, address = key only (counter intentionally unused).
static void ra_init_state_addressable(uint32_t *L, uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * ADDR_L_MIX_CONST + 0x9e3779b7u * key);
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

// One full 255-step permutation cycle. Byte-for-byte copy of
// winner_wired_v2.c's ra_permutation_cycle() -- untouched, per this file's
// header comment.
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

// Byte-for-byte copy of winner_wired_v2.c's ra_reseed().
static uint32_t ra_reseed(uint32_t *M, const uint32_t *L) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];

    uint32_t tmp8[8];
    ra_hash(M, tmp8);

    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

// Same structure as winner_wired_v2.c's ra_core(), except ra_init_state()
// (fixed, seed-independent) is replaced with ra_init_state_addressable(),
// and the parameter is named `key` (Tahap 1 naming). `cons` still starts as
// `key`'s value -- only the name changed, matching the Python harness
// (ra_core_from_state() in tahap0_prototype.py) this was validated against.
uint32_t ra_core(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_state_addressable(L, M, key);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

int main(int argc, char **argv) {
    uint32_t last_cons;
    uint32_t key;

    if (argc >= 4 && strcmp(argv[1], "--stream") == 0) {
        key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t rng = (size_t)strtoull(argv[3], NULL, 0);

        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        last_cons = ra_core(key, rng, stdout);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

        fprintf(stderr, "Streamed %zu pseudorandom outputs in %.3f seconds\n", rng, elapsed);
        fprintf(stderr, "Last cons from RNGing: %u\n", last_cons);
        return 0;
    }

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    key = 1;
    last_cons = ra_core(key, TOTAL_RNG, NULL);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    printf("Generated %lu pseudorandom updates in %.3f seconds\n",
           (unsigned long)TOTAL_RNG, elapsed);
    printf("Last cons from RNGing: %lu\n", (unsigned long)last_cons);

    return 0;
}
