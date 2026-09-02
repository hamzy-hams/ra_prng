// diag_hashed_init.c -- DIAGNOSTIC ONLY, this session. Not part of the
// production API, not linked against ra_core.c.
//
// User's first fix direction (2026-09-01): improve the INIT by hashing
// `key` (via SplitMix32, well-established strong avalanche) before it
// enters the existing affine M[]/L[] formula, keeping the CURRENT
// (production) wired cycle untouched. If this alone clears the BCFN
// failure seen under multikey concatenation, the fix is fully localized to
// init -- no cycle change needed.
//
// `rot32`, `ra_hash`, `ra_permutation_cycle`, `ra_reseed` are byte-for-byte
// copies of ra_core.c's (the current production wired cycle). Only the
// init function is new: `ra_init_state_hashed_key` computes
// `keyhash = splitmix32(key)` once, then reuses the EXACT SAME formula
// shape ra_init_state_orbit already has, with `keyhash` substituted for
// every occurrence of `key`.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

static void ra_hash(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}

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
        uint32_t tmp = L[i]; L[i] = L[d]; L[d] = tmp;
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

// ---- new: key hashed via SplitMix32's finalizer before the existing
// affine formula uses it. Formula shape otherwise UNCHANGED from
// ra_init_state_orbit. ----

static inline uint32_t splitmix32_avalanche(uint32_t x) {
    x ^= x >> 16; x *= 0x7feb352dU;
    x ^= x >> 15; x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

static void ra_init_state_hashed_key(uint32_t *L, uint32_t *M, uint32_t key) {
    uint32_t keyhash = splitmix32_avalanche(key);
    for (int i = 0; i < 256; ++i) {
        uint32_t r = keyhash ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * 0x9e3779b7u + 0x9e3779b7u * keyhash);
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * keyhash);
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

static uint32_t ra_core_diag(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;
    uint32_t cons = key;
    ra_init_state_hashed_key(L, M, key);
    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
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
        ra_core_diag(key, block_len, stdout);
        remaining -= block_len;
        ++block_idx;
    }
    return 0;
}
