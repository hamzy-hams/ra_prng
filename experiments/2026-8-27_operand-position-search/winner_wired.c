// winner_wired.c
//
// ***SUPERSEDED (2026-08-28) by winner_wired_v2.c*** as this experiment's
// recommended candidate -- see RESULTS.md's "Survey of the other 12
// winners" section. winner_wired_v2.c (wiring "v08" in that survey) has
// marginally better avalanche (min-bit 0.4730 vs this file's 0.4723) AND
// ~6% fewer instructions, so it is not a trade-off, strictly better on
// both axes measured. This file is kept, unmodified, as a still-valid,
// still-fully-validated (128GB PractRand clean) historical artifact --
// the analysis/root-cause narrative below remains accurate for THIS
// wiring, just no longer the top pick.
//
// Operand-position-rewired variant of baseline.c (pruned_winner): same op
// set (TAP6, TAP7, ROT_C, SHR13; hash_access=sequential), same 4/18 active
// ops -- only ONE wiring slot changed relative to baseline.c: `a`'s update
// XORs `o` with `d` (this step's carry-in swap index) instead of `b`.
//
// Found by experiments/2026-8-27_operand-position-search/operand_search.py,
// an exhaustive 108-wiring search over wiring.py's 4 swappable slots, using
// quality_gate.py's new avalanche_gate_min_bit()-style per-bit floor (not
// just the scalar average that let baseline.c's defect through). This was
// the single best of 13 wirings that passed both tiers (min per-bit
// avalanche fraction 0.472304, overall 0.487783 -- see RESULTS.md) --
// picked over the other 12 winners because it changes only this one slot,
// making it the smallest, most legible fix and the easiest to verify
// against baseline.c line-by-line.
//
// Motivation (see ../2026-8-26_operation-pruning-research and this
// directory's HANDOVER.md): baseline.c's seed bits 5 and 6 are nearly
// avalanche-dead (253/255 output positions show 0 bit difference when
// either is flipped) -- invisible to the sibling experiment's
// avalanche_gate() because it only checks the 32-bit *average*. This
// rewiring closes that gap entirely: every one of the 32 seed bits now
// lands in the ~0.47-0.50 avalanche-fraction band (see RESULTS.md's full
// per-bit table), matching the original unpruned algorithm's health.
//
// Derivation (from wired_prng.py's permutation_cycle with
// Wiring(a_xor_operand="d", c_shift_operand="b", rotc_amount_source="b",
// rotc_xor_operand="a") -- verified bit-identical against wired_prng.py
// before trusting this file, see operand_search.py's Tier 0/1 runs):
//   o = (M[(i+6)&0xFF] << 6) ^ (M[(i+7)&0xFF] << 7)
//   a = (d ^ o) ^ (cons + a)      <-- only line changed vs. baseline.c
//   b = (cons + a) ^ (o + d)
//   c = rot32((b >> 13) ^ a, b)
//   d = c & 0xFF
//
// Written as a direct structural mirror of baseline.c (itself a mirror of
// src/ra_prng2/c/ra_prng2.c) so that `perf stat` numbers are comparable
// apples-to-apples: op count/instruction shape is identical to baseline.c,
// only which operand feeds one XOR differs. Measured (5-repeat `perf
// stat`, see RESULTS.md): this DOES cost a small but real, reproducible
// amount -- ~3.2% more instructions, ~8.2% more cycles than baseline.c
// (likely a slightly longer loop-carried dependency chain through `d`,
// which is itself derived from `c` -- not the "noise, no direct
// speedup/slowdown" HANDOVER.md predicted for a same-op-count rewiring).
// Speedup vs. the original unpruned algorithm remains ~2.3x even so.
//
// Copyright (c) 2025 Hamas A. Rahman (derivative research variant)
// Licensed under CC BY-NC-SA 4.0, matching the original this derives from.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define TOTAL_RNG 200000000UL // matches baseline.c's benchmark constant

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

// One full 255-step permutation cycle, rewired candidate.
static void ra_permutation_cycle(uint32_t cons, size_t it,
                                  const uint32_t *M, uint32_t *L,
                                  uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = (M[(uint8_t)(i + 6)] << 6) ^ (M[(uint8_t)(i + 7)] << 7);

        a = (d ^ o) ^ (cons + a);   // <-- was (b ^ o) ^ (cons + a) in baseline.c
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
