// diag_init_keyterm_mul.c -- DIAGNOSTIC/CANDIDATE. Winning fix for the
// multikey/aggressive-reinit BCFN defect (see ../2026-9-1_family-
// productionization/'s Tahap 2 discovery + this folder's HANDOVER.md):
// hash ONLY the key-dependent term (`0x9e3779b7u*key` for L,
// `0x06a0dd9bu*key` for M) ONCE per reinit -- since key and the constants
// never change within one reinit, this term is computed once and reused
// across all 256 elements -- then COMBINE it with the per-index step by
// MULTIPLY (not `+`/`^`), so the per-key STEP SIZE itself becomes a
// pseudorandom, key-unique value instead of only a shared shape shifted by
// an offset. Rotation (`r = key^i`, still the RAW key, not hashed) stays
// exactly as in ra_core.c's current formula.
//
// Verified (HANDOVER.md candidate 4): clean through 32GB multikey
// PractRand, overhead ~1.00-1.01x (free within noise) -- cheapest and only
// fully-passing candidate found. Additive (`+`) and XOR (`^`) combine of
// the same hashed term both FAILED (see diag_init_keyterm_add.c /
// diag_init_keyterm_xor.c) -- multiply is the mechanism that actually
// disrupts the per-key relationship the BCFN leak was keying off.
//
// Naming (2026-09-01 finalization): ra_init_orbit/ra_init_singleblock are
// the canonical names this mechanism will carry once promoted -- the
// current ra_core.c's *_orbit/*_singleblock functions were renamed to
// *_affine to free these names up (see ra_core.c's header comment). NOT
// yet applied to ra_core.c -- awaiting non-regression verification against
// ra_core_orbit_affine's existing single-key guarantees.
//
// rot32 / ra_permutation_cycle_orbit: byte-for-byte copies of ra_core.c's
// cycle (completely unchanged by this candidate -- this is also the
// canonical name, since the cycle itself was never in question).
// --stream <base_key> <n>: same multikey chaining as multikey_stream.c /
// diag_init_avalanche.c (Weyl-stepped keys, up to 255-word blocks per key).

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

static inline uint32_t fmix32(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6bu;
    h ^= h >> 13;
    h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}

// Candidate: hash only the per-key term, computed once per reinit
// (key/constant don't change within a reinit, so no need to recompute per
// element), combined with the per-index step by MULTIPLY -- cheap (2
// fmix32 calls total) and the only combine operator that passes.
static void ra_init_orbit(uint32_t *L, uint32_t *M, uint32_t key) {
    uint32_t keyterm_l = fmix32(0x9e3779b7u * key);
    uint32_t keyterm_m = fmix32(0x06a0dd9bu * key);
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * 0x9e3779b7u) * keyterm_l;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

// M-only analog of ra_init_orbit, for the singleblock (no-L) mode --
// mirrors ra_core.c's ra_init_state_singleblock_affine's M-only structure.
// Not yet wired into a core+cycle wrapper here; that belongs to the
// non-regression verification step, not this naming pass.
static void ra_init_singleblock(uint32_t *M, uint32_t key) {
    uint32_t keyterm_m = fmix32(0x06a0dd9bu * key);
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32(m_val, r);
    }
}

static void ra_permutation_cycle_orbit(uint32_t cons, size_t it,
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
        uint32_t tmp = L[i]; L[i] = L[d]; L[d] = tmp;
    }
}

static void ra_core_orbit(uint32_t key, size_t block_len, FILE *raw_stream) {
    if (block_len == 0) return;
    alignas(64) uint32_t L[256], M[256];
    ra_init_orbit(L, M, key);
    uint64_t count = block_len;
    ra_permutation_cycle_orbit(key, 0, M, L, &count, raw_stream);
}

int main(int argc, char **argv) {
    if (argc < 4 || strcmp(argv[1], "--stream") != 0) {
        fprintf(stderr, "usage: %s --stream <base_key> <n>\n", argv[0]);
        return 1;
    }
    uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
    size_t n = (size_t)strtoull(argv[3], NULL, 0);

    uint64_t block_idx = 0;
    size_t remaining = n;
    while (remaining > 0) {
        size_t block_len = remaining < 255 ? remaining : 255;
        uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
        ra_core_orbit(key, block_len, stdout);
        remaining -= block_len;
        ++block_idx;
    }
    return 0;
}
