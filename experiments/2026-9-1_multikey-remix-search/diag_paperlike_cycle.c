// diag_paperlike_cycle.c -- DIAGNOSTIC ONLY, this session. Not part of the
// production API, not linked against ra_core.c.
//
// Question (user, 2026-09-01): the BCFN failure seen when concatenating
// many short (255-word) addressable-keyed blocks -- identical in both
// ra_core_orbit and ra_core_singleblock -- comes from the shared
// addressable init formula (`ra_init_state_orbit`/`_singleblock`'s
// M[i] = rot32(i*0x06a0dd9b + 0x06a0dd9b*key, key^i)`, affine in key). Is
// the defect in that INIT step, or would the ORIGINAL paper-exact cycle
// (src/ra_prng2/c/ra_prng2.c's richer permutation_cycle -- 8-fold `o` XOR,
// extra rot32 layers, multiply-high-bits `d`) have enough internal mixing
// to compensate for the same weak init?
//
// This file: `rot32`, `ra_hash`, `ra_permutation_cycle`, `ra_reseed` are
// byte-for-byte copies of src/ra_prng2/c/ra_prng2.c's (read-only, never
// modifies that file). Only the init is swapped for the CURRENT addressable
// formula (byte-for-byte copy of ra_core.c's ra_init_state_orbit). If this
// combination ALSO fails the same PractRand BCFN test under the same
// multikey-concatenation methodology, the defect is in the init, not the
// cycle. If it passes clean, the richer cycle compensates and the fix
// belongs in ra_permutation_cycle instead.
//
// --stream <base_key> <n>: same multikey chaining as multikey_stream.c
// (Weyl-stepped keys, 255-word blocks, no reseed between blocks -- matches
// the exact aggressive-reinit shape that exposed the anomaly).

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// ---- byte-for-byte copy of src/ra_prng2/c/ra_prng2.c ----

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFF;
}

static void ra_hash(uint32_t *N, uint32_t *out8) {
    for (uint8_t i = 0; i < 8; ++i) out8[i] = 0;
    for (uint8_t i = 0; i < 8; ++i) {
        out8[i] ^= N[(uint8_t)N[i]];
        for (uint8_t j = 0; j < 32; j++) {
            out8[i] ^= N[j * 8 + i];
        }
    }
}

static void ra_permutation_cycle(uint32_t cons, size_t it,
                                  const uint32_t *M, uint32_t *L,
                                  uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons;
    uint32_t b = (uint32_t)it;
    uint32_t c = 0;
    uint32_t d = 0;

    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = 0;
        for (uint8_t e = 0; e < 8; ++e) {
            o ^= (M[(uint8_t)(i + e)] << e);
        }

        a = (rot32(b ^ o, d) ^ (cons + a));
        b = (rot32(cons + a, i) ^ (o + d));
        o = (rot32(a ^ o, i) << 9 ^ (b >> 18));
        c = rot32((o + c << 14) ^ (b >> 13) ^ a, b);
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = (uint32_t)(((uint64_t)c * (i + 1)) >> 32);

        if (*count <= 1) break;
        --(*count);

        uint32_t tmp = L[i];
        L[i] = L[d];
        L[d] = tmp;
    }
}

static uint32_t ra_reseed(uint32_t *M, const uint32_t *L) {
    uint32_t tmp8[8];
    for (uint16_t i = 0; i < 256; ++i) M[i] ^= L[i];
    ra_hash(M, tmp8);
    uint32_t new_cons = 0;
    for (uint8_t e = 0; e < 8; ++e) new_cons ^= tmp8[e] << e;
    return new_cons;
}

// ---- byte-for-byte copy of ra_core.c's ra_init_state_orbit ----

static void ra_init_state_addressable(uint32_t *L, uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * 0x9e3779b7u + 0x9e3779b7u * key);
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
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
    ra_init_state_addressable(L, M, key);
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
