// o_fmix_every_round.c -- Candidate B, orbit-o-fmix-fix experiment
//
// Same rationale as o_fmix_init_only.c (Candidate A) -- see that file's
// header comment for full context. Candidate B differs only in ALSO
// applying fmix32() to `o_next` every time `o` is recomputed inside the
// loop (i>1), not just the initial `o` at i=255. This is more consistent
// across a full K=255 cycle (every round's `o` is full-avalanche-mixed
// before being used in a/b), at a small extra cost per round.
//
// rot32/fmix32/GUARD_L/GUARD_M/ra_hash/ra_reseed/ra_init_state_orbit copied
// VERBATIM (byte-for-byte, no modification) from
// ../../2026-9-1_family-productionization/ra_core.c L144-207.
// ra_permutation_cycle_orbit is copied from the same file L217-252 with
// EXACTLY TWO lines changed vs production: `o = fmix32(o);` after the
// initial 8-tap computation, and `o = fmix32(o_next);` replacing the plain
// `o = o_next;` inside the loop -- everything else untouched.
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

static inline uint32_t tap_o8(const uint32_t *M, uint32_t i) {
    return (M[(uint8_t)(i + 0)] << 0) ^ (M[(uint8_t)(i + 1)] << 1) ^
           (M[(uint8_t)(i + 2)] << 2) ^ (M[(uint8_t)(i + 3)] << 3) ^
           (M[(uint8_t)(i + 4)] << 4) ^ (M[(uint8_t)(i + 5)] << 5) ^
           (M[(uint8_t)(i + 6)] << 6) ^ (M[(uint8_t)(i + 7)] << 7);
}

// NOTE: production's rolling-register update for `o` ("o_next = new_term ^
// ((o ^ o_term7) << 1)") is only algebraically valid when `o` is still in
// its raw XOR-tap form -- the o_term7 XOR-cancel trick assumes `o` is
// linearly decomposable into per-tap terms. Once `o` has been fmix32'd
// (nonlinear), that trick silently breaks (caught by the mandatory
// byte-exact vs oracle check, see combo_prng_ofmix.py). So this candidate
// recomputes `o` via a fresh naive 8-tap (tap_o8) every round instead of
// the rolling register -- more per-round work, but the only correct way to
// keep "o for round i" well-defined once mixing is involved.
static void ra_permutation_cycle_orbit_ofmix(uint32_t cons, size_t it,
                                              const uint32_t *M, uint32_t *L,
                                              uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    uint32_t o = fmix32(tap_o8(M, 255));  // CANDIDATE B change 1/2: mix the initial o

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
            o = fmix32(tap_o8(M, i - 1));  // CANDIDATE B change 2/2: mix every re-derived o
        }
    }
}

static uint32_t ra_core_orbit_ofmix(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_state_orbit(L, M, key);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_orbit_ofmix(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--single") == 0 && argc >= 4) {
        uint32_t key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t rng = (size_t)strtoull(argv[3], NULL, 0);
        ra_core_orbit_ofmix(key, rng, stdout);
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
            ra_core_orbit_ofmix(key, block_len, stdout);
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
            ra_core_orbit_ofmix(key, block_len, stdout);
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
