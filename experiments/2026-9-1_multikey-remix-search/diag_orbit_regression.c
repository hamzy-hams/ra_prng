// diag_orbit_regression.c -- DIAGNOSTIC. Non-regression check for
// ra_init_orbit (diag_init_keyterm_mul.c's winning multikey mix-init fix)
// against ra_core.c's ra_core_orbit_affine's EXISTING single-key/long-stream
// guarantees (128GB PractRand, cross-correlation, collision-scan,
// dieharder). See ../2026-9-1_multikey-remix-search/HANDOVER.md
// "Verification bar" item 2.
//
// rot32/fmix32/ra_init_orbit/ra_permutation_cycle_orbit: byte-for-byte
// copies of diag_init_keyterm_mul.c's (which are themselves unchanged vs.
// ra_core.c's cycle). ra_hash/ra_reseed: byte-for-byte copies of ra_core.c's.
//
// ra_core_orbit_stream: NEW for this file -- an unbounded-stream, reseeding
// wrapper (copy of ra_core.c's ra_core_orbit_affine's exact loop structure)
// that calls ra_init_orbit instead of ra_init_state_orbit_affine. This is
// deliberately a DIFFERENT function from diag_init_keyterm_mul.c's
// ra_core_orbit, which is a single <=255-word block with no reseed (built
// for multikey/aggressive-reinit chaining, not continuous streaming) --
// naming reconciliation (which one keeps the bare "ra_core_orbit" name once
// promoted to ra_core.c) is deferred to that later session, per user
// decision 2026-09-01.
//
// CLI: --stream <key> <n> (n unbounded, no mode arg) -- matches
// experiments/2026-8-29_parallelization-research/common.py's
// stream_values()/stream_popen() default contract, same shape as
// winner_wired_addressable, so tahap3_cross_correlation.py/
// tahap3_collision_scan.py's re-pointing pattern (ADDR_BIN override)
// works unmodified against this binary.

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

// hash_access=sequential, HASH_SELFIDX off, HASH_DEPTH fixed at 32.
// Byte-for-byte copy of ra_core.c's ra_hash.
static void ra_hash(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}

// Byte-for-byte copy of ra_core.c's ra_reseed.
static uint32_t ra_reseed(uint32_t *M, const uint32_t *L) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];

    uint32_t tmp8[8];
    ra_hash(M, tmp8);

    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

// Unbounded-stream orbit wrapper, structurally identical to ra_core.c's
// ra_core_orbit_affine, but with ra_init_orbit as the init.
uint32_t ra_core_orbit_stream(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_orbit(L, M, key);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_orbit(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

int main(int argc, char **argv) {
    if (argc < 4 || strcmp(argv[1], "--stream") != 0) {
        fprintf(stderr, "usage: %s --stream <key> <n>\n", argv[0]);
        return 1;
    }
    uint32_t key = (uint32_t)strtoul(argv[2], NULL, 0);
    size_t n = (size_t)strtoull(argv[3], NULL, 0);
    ra_core_orbit_stream(key, n, stdout);
    return 0;
}
