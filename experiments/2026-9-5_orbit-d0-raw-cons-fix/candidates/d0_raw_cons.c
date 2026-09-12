// d0_raw_cons.c -- orbit-d0-raw-cons-fix experiment.
//
// Follow-up to orbit-init-warmup-fix (d0_hash candidate FAILED PractRand
// K=1+hashed @512MB, see that RESULTS.md). d0_hash seeded the initial `d`
// (it==0 only) with a hashed, decorrelated-from-cons value:
//   d0 = fmix32(GUARD_D ^ key) & 0xFF
// This candidate tests a narrower variant the user asked for explicitly:
// give `d` a nonzero SEED VALUE, but WITHOUT the fmix32 hash step --
// derive it directly/structurally from `cons` (== key at it==0) instead
// of a pseudo-random-looking hash of it:
//   d0 = cons & 0xFF   (== key & 0xFF at it==0)
// `a0 = key` (unchanged, same as production and as d0_hash).
//
// Per the same repo-wide algebraic fact established this session: `b`'s
// and `c`'s initial values in ra_permutation_cycle_orbit are NEVER READ
// before being overwritten in round 1 (both are recomputed fresh from the
// updated `a` before any use) -- so "making b/c nonzero at init" has
// ZERO effect on output and is intentionally NOT touched here. `a` is
// already nonzero (=cons) and `o` is already a generic nonzero 8-tap
// value -- the only live lever is `d`.
//
// Loop body (ra_permutation_cycle_orbit_combo, i<255..1>) is a LITERAL
// VERBATIM copy of production; only ra_init_warmup_orbit (a0/d0 seed at
// it==0) differs, matching the exact scoping precedent of d0_hash/
// d0_realround (it>=1 untouched, d=0 as production -- K=1 never reaches
// it>=1 anyway, and K=255 always has rng<=255 so it==0 always too, see
// HANDOVER.md section 2 note re-used by that prior experiment).
//
// rot32/fmix32/GUARD_L/GUARD_M/ra_hash/ra_reseed/ra_init_state_orbit
// copied verbatim from ../../2026-9-1_family-productionization/ra_core.c
// L144-207. splitmix32 copied verbatim from
// ../../2026-9-1_family-productionization/multikey_stream_hashed.c L16-22.
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

// <-- ONLY CHANGE vs production: d0 = raw low byte of cons/key, NO fmix32.
static void ra_init_warmup_orbit(uint32_t key, uint32_t *a0, uint32_t *d0) {
    *d0 = key & 0xFFu;
    *a0 = key;
}

static void ra_permutation_cycle_orbit_combo(uint32_t cons, size_t it,
                                              uint32_t a0, uint32_t d0,
                                              const uint32_t *M, uint32_t *L,
                                              uint64_t *count, FILE *raw_stream) {
    uint32_t a = (it == 0) ? a0 : cons;
    uint32_t b = (uint32_t)it, c = 0;
    uint32_t d = (it == 0) ? d0 : 0u;

    uint32_t o = (M[(uint8_t)(255 + 0)] << 0) ^ (M[(uint8_t)(255 + 1)] << 1) ^
                 (M[(uint8_t)(255 + 2)] << 2) ^ (M[(uint8_t)(255 + 3)] << 3) ^
                 (M[(uint8_t)(255 + 4)] << 4) ^ (M[(uint8_t)(255 + 5)] << 5) ^
                 (M[(uint8_t)(255 + 6)] << 6) ^ (M[(uint8_t)(255 + 7)] << 7);
    uint32_t o_term7 = M[(uint8_t)(255 + 7)] << 7;

    for (uint32_t i = 255; i > 0; --i) {
        a = (d ^ o) ^ (cons + a);
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

static uint32_t ra_core_orbit_combo(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_state_orbit(L, M, key);

    uint32_t a0, d0;
    ra_init_warmup_orbit(key, &a0, &d0);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_orbit_combo(cons, it, a0, d0, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--single") == 0 && argc >= 4) {
        uint32_t key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t rng = (size_t)strtoull(argv[3], NULL, 0);
        ra_core_orbit_combo(key, rng, stdout);
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
            ra_core_orbit_combo(key, block_len, stdout);
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
            ra_core_orbit_combo(key, block_len, stdout);
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
