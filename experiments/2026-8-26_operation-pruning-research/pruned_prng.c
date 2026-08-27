// pruned_prng.c
// C port of pruned_prng.py for speed -- pure-Python generation proved too
// slow (~0.8 MB/s) to run PractRand tiers large enough to reliably catch
// broken candidates (see RESULTS.md "gate-size traps": 1MB and 8MB both let
// structurally broken candidates through that failed at 16-32MB). This port
// must be cross-validated bit-for-bit against pruned_prng.py's ALL_OPS
// (bitmask 0x3FFFF, hash_access=strided) before being trusted for anything,
// per the periodicity experiment's C-port precedent
// (experiments/2026-8-25_periodicity-heuristic-validation/cycle_measure.c).
//
// Op bitmask bit order (must match operations.py's canonical SCAN_ORDER --
// see that file / ablation_search.py for the authoritative list):
//   0-7:   TAP0..TAP7
//   8:     ROT_A   9: ROT_B   10: ROT_O   11: ROT_C
//   12:    SHL9    13: SHR18  14: SHL14   15: SHR13
//   16:    MULT_REDUCE
//   17:    HASH_SELFIDX
// hash_access: 0 = strided (original), 1 = sequential.
//
// Usage:
//   ./pruned_prng <seed> <iterations> <ops_bitmask_hex> <hash_access> [s9 s18 s14 s13] [--stream]
//       s9/s18/s14/s13 are the SHL9/SHR18/SHL14/SHR13 shift widths (default
//       9/18/14/13, the originals) -- only meaningful for whichever of those
//       flags are ON in the bitmask; added for the post-convergence "shift
//       repair" search (see RESULTS.md), which holds the op set fixed and
//       scans these widths for the point of maximum avalanche diffusion.
//   --stream streams raw little-endian uint32_t `c` values to stdout;
//   without it, prints the final `cons` value (for the self-check).

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ALL_OPS_MASK 0x3FFFFu

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

static inline int has(uint32_t ops, int bit) {
    return (ops >> bit) & 1u;
}

// bit indices, matching operations.py's canonical order
enum {
    B_TAP0, B_TAP1, B_TAP2, B_TAP3, B_TAP4, B_TAP5, B_TAP6, B_TAP7,
    B_ROT_A, B_ROT_B, B_ROT_O, B_ROT_C,
    B_SHL9, B_SHR18, B_SHL14, B_SHR13,
    B_MULT_REDUCE, B_HASH_SELFIDX
};

static void ra_hash(const uint32_t *N, uint32_t *out8, uint32_t ops, int hash_access) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        if (has(ops, B_HASH_SELFIDX)) out8[i] ^= N[(uint8_t)N[i]];
        if (hash_access == 1) {
            int base = i * 32;
            for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
        } else {
            for (int j = 0; j < 32; ++j) out8[i] ^= N[j * 8 + i];
        }
    }
}

static void init_state(uint32_t *L, uint32_t *M) {
    for (int i = 0; i < 256; ++i) {
        M[i] = (uint32_t)(i * 0x06a0dd9bUL + 0x06a0dd9bUL);
        L[i] = (uint32_t)(i * 0x9e3779b7UL + 0x9e3779b7UL);
    }
}

static inline uint32_t build_o(const uint32_t *M, uint32_t i, uint32_t ops) {
    uint32_t o = 0;
    for (uint32_t e = 0; e < 8; ++e) {
        if (has(ops, (int)e)) o ^= (M[(uint8_t)(i + e)] << e);
    }
    return o;
}

// One full 255-step permutation cycle. Writes each step's `c` to
// raw_stream (if non-NULL) as it's produced. Mutates L via the swap.
static void permutation_cycle(uint32_t cons, uint32_t it, uint32_t *M, uint32_t *L,
                               uint32_t ops, const uint32_t widths[4], FILE *raw_stream) {
    uint32_t a = cons, b = it, c = 0, d = 0;
    uint32_t s9 = widths[0], s18 = widths[1], s14 = widths[2], s13 = widths[3];

    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = build_o(M, i, ops);

        uint32_t raw_a = b ^ o;
        a = (has(ops, B_ROT_A) ? rot32(raw_a, d) : raw_a) ^ (cons + a);

        uint32_t raw_b = cons + a;
        b = (has(ops, B_ROT_B) ? rot32(raw_b, i) : raw_b) ^ (o + d);

        uint32_t rotated_o = has(ops, B_ROT_O) ? rot32(a ^ o, i) : (a ^ o);
        uint32_t left = has(ops, B_SHL9) ? (rotated_o << s9) : 0;
        uint32_t right = has(ops, B_SHR18) ? (b >> s18) : 0;
        if (has(ops, B_SHL9) && has(ops, B_SHR18)) o = left ^ right;
        else if (has(ops, B_SHL9)) o = left;
        else if (has(ops, B_SHR18)) o = right;
        else o = 0;

        uint32_t term_oc = has(ops, B_SHL14) ? ((o + c) << s14) : 0;
        uint32_t term_b13 = has(ops, B_SHR13) ? (b >> s13) : 0;
        uint32_t pre_rot_c = term_oc ^ term_b13 ^ a;
        c = has(ops, B_ROT_C) ? rot32(pre_rot_c, b) : pre_rot_c;

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = has(ops, B_MULT_REDUCE) ? (uint32_t)(((uint64_t)c * (i + 1)) >> 32) : (c & 0xFFu);

        uint32_t tmp = L[i];
        L[i] = L[d];
        L[d] = tmp;
    }
}

static uint32_t reseed(uint32_t *M, const uint32_t *L, uint32_t ops, int hash_access) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];

    uint32_t tmp8[8];
    ra_hash(M, tmp8, ops, hash_access);

    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

static uint32_t run(uint32_t seed, size_t iterations, uint32_t ops, int hash_access,
                     const uint32_t widths[4], FILE *raw_stream) {
    static uint32_t L[256], M[256];
    init_state(L, M);
    uint32_t cons = seed;
    for (size_t it = 0; it < iterations; ++it) {
        permutation_cycle(cons, (uint32_t)it, M, L, ops, widths, raw_stream);
        cons = reseed(M, L, ops, hash_access);
    }
    return cons;
}

int main(int argc, char **argv) {
    if (argc < 5) {
        fprintf(stderr, "usage: %s <seed> <iterations> <ops_bitmask_hex> <hash_access:0|1> "
                         "[s9 s18 s14 s13] [--stream]\n", argv[0]);
        return 1;
    }
    uint32_t seed = (uint32_t)strtoul(argv[1], NULL, 0);
    size_t iterations = (size_t)strtoull(argv[2], NULL, 0);
    uint32_t ops = (uint32_t)strtoul(argv[3], NULL, 16);
    int hash_access = atoi(argv[4]);

    uint32_t widths[4] = {9, 18, 14, 13};
    int next = 5;
    if (argc >= 9 && strcmp(argv[5], "--stream") != 0) {
        widths[0] = (uint32_t)strtoul(argv[5], NULL, 0);
        widths[1] = (uint32_t)strtoul(argv[6], NULL, 0);
        widths[2] = (uint32_t)strtoul(argv[7], NULL, 0);
        widths[3] = (uint32_t)strtoul(argv[8], NULL, 0);
        next = 9;
    }
    int stream_mode = (argc > next && strcmp(argv[next], "--stream") == 0);

    uint32_t final_cons = run(seed, iterations, ops, hash_access, widths, stream_mode ? stdout : NULL);

    if (!stream_mode) {
        printf("%u\n", final_cons);
    }
    return 0;
}
