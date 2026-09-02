// diag_warmup_discard.c -- DIAGNOSTIC ONLY, this session. Not part of the
// production API, not linked against ra_core.c.
//
// User's hypothesis (2026-09-01): the BCFN correlation seen across many
// different keys isn't a general "wired cycle lacks mixing" problem -- it's
// concentrated in the FIRST 255-word cycle right after init, because
// a=cons=key and b=(uint32_t)it=0 at the very start, before any ra_reseed
// has folded L into M. ra_core_orbit's 128GB clean result doesn't contradict
// this: that test is one key, billions of post-reseed cycles, so the
// first-cycle contamination is diluted into invisibility. Singleblock's
// (and multikey_stream's) failure mode measures EXACTLY the first cycle,
// for thousands of different keys back to back -- the worst possible angle
// to catch this if it's real.
//
// Test: keep the CURRENT production wired cycle AND the CURRENT suspect
// addressable init completely unchanged (byte-for-byte copies from
// ra_core.c). Per key, run ONE full 255-step cycle with output discarded
// (raw_stream=NULL, count starts at 255 so it never breaks early), do ONE
// ra_reseed, THEN run the real cycle with output. If this alone clears the
// BCFN failure, the defect is confirmed to be "first cycle only" and the
// fix is a cheap warm-up-and-discard step -- no cycle or init replacement
// needed.
//
// --stream <base_key> <n>: same multikey chaining as multikey_stream.c
// (Weyl-stepped keys, up to 255-word blocks per key).

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

static uint32_t ra_reseed(uint32_t *M, const uint32_t *L) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];
    uint32_t tmp8[8];
    ra_hash(M, tmp8);
    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

static void ra_init_state_orbit(uint32_t *L, uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * 0x9e3779b7u + 0x9e3779b7u * key);
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
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

// Per key: init, run ONE discarded warm-up cycle (full 255 steps, no early
// break), reseed, then run the real cycle with output capped at block_len.
static void ra_core_warmup(uint32_t key, size_t block_len, FILE *raw_stream) {
    if (block_len == 0) return;
    alignas(64) uint32_t L[256], M[256];
    ra_init_state_orbit(L, M, key);

    uint64_t warmup_count = 255;
    ra_permutation_cycle_orbit(key, /*it=*/0, M, L, &warmup_count, NULL);
    uint32_t cons = ra_reseed(M, L);

    uint64_t count = block_len;
    ra_permutation_cycle_orbit(cons, /*it=*/1, M, L, &count, raw_stream);
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
        ra_core_warmup(key, block_len, stdout);
        remaining -= block_len;
        ++block_idx;
    }
    return 0;
}
