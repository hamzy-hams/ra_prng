// diag_init_avalanche.c -- DIAGNOSTIC ONLY. Candidate fix: current affine
// init formula UNCHANGED, but each L[i]/M[i] gets one MurmurHash3 finalizer
// pass (fmix32) applied individually after the formula+rot32. Attacks the
// linear/cyclical relationship between M[i] and M[i+1] (same shape for
// every key, only offset by key) that survives key-hashing alone
// (diag_hashed_init.c still failed) -- O(256) extra work, NOT a full
// O(255) cycle rerun like diag_warmup_discard.c/diag_warmup_depth.c.
// Current production wired cycle kept completely unchanged.
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

static void ra_init_state_avalanche(uint32_t *L, uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * 0x9e3779b7u + 0x9e3779b7u * key);
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
        L[i] = fmix32(rot32(l_val, r));
        M[i] = fmix32(rot32(m_val, r));
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

static void ra_core_avalanche_init(uint32_t key, size_t block_len, FILE *raw_stream) {
    if (block_len == 0) return;
    alignas(64) uint32_t L[256], M[256];
    ra_init_state_avalanche(L, M, key);
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
        ra_core_avalanche_init(key, block_len, stdout);
        remaining -= block_len;
        ++block_idx;
    }
    return 0;
}
