// ad_mul_combine.c -- orbit-ad-mul-combine-fix experiment
//
// Follow-up to orbit-o-fmix-fix (RESULTS.md there): that session found that
// mixing `o` alone (before or every round) does NOT change the K=1+hashed
// defect signature at all -- and the 32-combo search earlier found varying
// the `c` finalizer alone doesn't either. Both point away from `o`/`c` in
// isolation and toward the COMBINATION point itself:
//   a = (d ^ o) ^ (cons + a);
//   b = (cons + a) ^ (o + d);
// This candidate tests the single targeted change the user asked for:
// replace the `(d ^ o)` XOR-combine in the `a` update with `(d * o)`
// (multiply-combine) -- same technique (`+`/`^` combine failing,
// multiply-combine succeeding) that fixed the earlier, different
// affine-in-key BCFN defect in multikey_remix_search
// (project_multikey_remix_search memory) -- untested here until now.
// ONLY this one line changes; `b`, `c`, `o`, `ra_init_state_orbit` are
// byte-for-byte production, unmixed (no fmix32 on `o` this time).
//
// Note (expected, not a bug): `d` starts at 0 for the very first round
// (i=255, before any `c` has been computed), so `d*o = 0` there -- `o`'s
// contribution vanishes from `a` entirely for round 1 under this formula
// (though `b` still folds `o` in via `(cons+a)^(o+d)`). This is an
// inherent property of the exact formula requested, not an implementation
// slip -- noted in RESULTS.md, not "fixed" by e.g. seeding d!=0.
//
// rot32/fmix32/GUARD_L/GUARD_M/ra_hash/ra_reseed/ra_init_state_orbit copied
// VERBATIM (byte-for-byte, no modification) from
// ../../2026-9-1_family-productionization/ra_core.c L144-207.
// splitmix32 copied verbatim from
// ../../2026-9-1_family-productionization/multikey_stream_hashed.c L16-22.
//
// ra_core.c is NOT modified by this file.

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

#define GUARD_L 0x38916df4u
#define GUARD_M 0x6c26fc92u

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

/* splitmix32 -- copied verbatim from
 * ../../2026-9-1_family-productionization/multikey_stream_hashed.c L16-22 */
static uint32_t splitmix32(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    z = z ^ (z >> 31);
    return (uint32_t)(z >> 32);
}

static void ra_init_state_orbit(uint32_t *L, uint32_t *M, uint32_t key) {
    uint32_t keyterm_l = fmix32(GUARD_L ^ (0x9e3779b7u * key));
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * 0x9e3779b7u) * keyterm_l;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

static void ra_permutation_cycle_orbit_admul(uint32_t cons, size_t it,
                                              const uint32_t *M, uint32_t *L,
                                              uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    uint32_t o = (M[(uint8_t)(255 + 0)] << 0) ^ (M[(uint8_t)(255 + 1)] << 1) ^
                 (M[(uint8_t)(255 + 2)] << 2) ^ (M[(uint8_t)(255 + 3)] << 3) ^
                 (M[(uint8_t)(255 + 4)] << 4) ^ (M[(uint8_t)(255 + 5)] << 5) ^
                 (M[(uint8_t)(255 + 6)] << 6) ^ (M[(uint8_t)(255 + 7)] << 7);
    uint32_t o_term7 = M[(uint8_t)(255 + 7)] << 7;

    for (uint32_t i = 255; i > 0; --i) {
        a = (d * o) ^ (cons + a);   // <-- ONLY CHANGE vs production: '^' -> '*' between d and o
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 17u;

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & 0xFFu;

        if (*count <= 1) break;
        --(*count);

        uint32_t tmp = L[i];
        L[i] = L[d];
        L[d] = tmp;

        if (i > 1) {
            uint32_t new_term = M[(uint8_t)(i - 1)];
            uint32_t o_next = new_term ^ ((o ^ o_term7) << 1);
            o_term7 = M[(uint8_t)(i + 6)] << 7;
            o = o_next;
        }
    }
}

static uint32_t ra_core_orbit_admul(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_state_orbit(L, M, key);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_orbit_admul(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--single") == 0 && argc >= 4) {
        uint32_t key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t rng = (size_t)strtoull(argv[3], NULL, 0);
        ra_core_orbit_admul(key, rng, stdout);
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--stream") == 0 && argc >= 5) {
        uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t n = (size_t)strtoull(argv[3], NULL, 0);
        size_t K = (size_t)strtoull(argv[4], NULL, 0);
        if (K == 0 || K > 255) { fprintf(stderr, "K must be in [1,255]\n"); return 1; }
        uint64_t block_idx = 0;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < K ? remaining : K;
            uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
            ra_core_orbit_admul(key, block_len, stdout);
            remaining -= block_len;
            ++block_idx;
        }
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--stream-hashed") == 0 && argc >= 5) {
        uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t n = (size_t)strtoull(argv[3], NULL, 0);
        size_t K = (size_t)strtoull(argv[4], NULL, 0);
        if (K == 0 || K > 255) { fprintf(stderr, "K must be in [1,255]\n"); return 1; }
        uint64_t state = (uint64_t)(uint32_t)base_key;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < K ? remaining : K;
            uint32_t key = splitmix32(&state);
            ra_core_orbit_admul(key, block_len, stdout);
            remaining -= block_len;
        }
        return 0;
    }
    fprintf(stderr,
        "usage:\n"
        "  %s --single <key> <rng>\n"
        "  %s --stream <base_key> <n> <K>\n"
        "  %s --stream-hashed <base_key> <n> <K>\n",
        argv[0], argv[0], argv[0]);
    return 1;
}
