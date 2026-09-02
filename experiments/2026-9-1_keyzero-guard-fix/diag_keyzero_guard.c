// diag_keyzero_guard.c -- DIAGNOSTIC/CANDIDATE. Fix for the key=0
// degenerate-state defect found in production-candidate-battery/RESULTS.md
// (Step 3): ra_core.c's current init (Kandidat 4, multiply-combined
// per-key hashed term) computes keyterm_l/keyterm_m = fmix32(CONST*key);
// for key=0 this is fmix32(0)==0 (fmix32 is a bijection fixing 0), which
// zeroes L[]/M[] entirely via multiply (a zero divisor `+`/`^` don't have),
// producing a permanent all-zero output stream.
//
// Fix (user-specified 2026-09-01, exact form, not redesigned): XOR a
// balanced-popcount guard constant into the input BEFORE fmix32, so
// key=0 no longer maps to fmix32(0):
//   keyterm_l = fmix32(GUARD_L ^ (0x9e3779b7u * key));
//   keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
// GUARD_L=0x38916df4, GUARD_M=0x6c26fc92 -- both popcount==16, distinct
// from each other and from all 4 pre-existing constants in ra_core.c's
// init path (0x9e3779b7 popcount=21, 0x06a0dd9b popcount=15, fmix32's
// 0x85ebca6b popcount=18, 0xc2b2ae35 popcount=16) -- see
// pick_guard_constants.py for the deterministic derivation + proof.
//
// At key=0 the new keyterm is fmix32(GUARD_L)/fmix32(GUARD_M), both
// nonzero (verified in --edgecheck below), so L[]/M[] are no longer
// forced to all-zero.
//
// Does this just move the "keyterm==0" key elsewhere instead of removing
// it? Algebraically yes: `GUARD_L ^ (0x9e3779b7u*key) == 0` still has
// exactly one solution, key = GUARD_L*inverse(0x9e3779b7) mod 2^32 =
// 0x777d91ac (similarly 0x9c0387d6 for keyterm_m=0, since
// 0x9e3779b7/0x06a0dd9b are odd hence invertible mod 2^32) -- these are
// tested explicitly in --edgecheck. But UNLIKE key=0, these keys do NOT
// reproduce the catastrophic all-zero-forever attractor: the original
// defect needed TWO things to coincide -- keyterm(s)==0 (zeroing
// L[]/M[]) AND cons=key==0 (so the permutation cycle's a=cons=0 initial
// state has no nonzero term to inject). At key=0x777d91ac, M/L may again
// be affected by one zeroed keyterm, but cons=key=0x777d91ac != 0, so
// `a=(d^o)^(cons+a)` still grows from a nonzero `cons+a` every step --
// the cycle does NOT collapse to the zero fixed point (empirically
// confirmed non-degenerate in --edgecheck below). The two constructions
// are not equivalent in risk: key=0 is a foreseeable default
// (zero-initialized memory, `int key = 0;`), while 0x777d91ac/0x9c0387d6
// are arbitrary 32-bit values with no reason to occur by default -- and
// even if hit, they don't collapse the generator, only (at most) drop
// one of the two keyterms' contribution for that single reinit.
//
// Provenance: rot32/fmix32/ra_permutation_cycle_orbit/ra_hash/ra_reseed
// byte-for-byte copies of ra_core.c's (unchanged by this fix -- only the
// keyterm computation inside ra_init_orbit/ra_init_singleblock changes).
// Multikey chaining CLI mirrors
// ../2026-9-1_multikey-remix-search/diag_init_keyterm_mul.c's --stream
// contract; unbounded reseeding CLI mirrors
// ../2026-9-1_multikey-remix-search/diag_orbit_regression.c's.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define GUARD_L 0x38916df4u
#define GUARD_M 0x6c26fc92u

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

static void ra_init_singleblock(uint32_t *M, uint32_t key) {
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
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

static void ra_permutation_cycle_singleblock(uint32_t cons, size_t it,
                                              const uint32_t *M,
                                              uint64_t *count,
                                              FILE *raw_stream) {
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
    }
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

// Unbounded reseeding orbit stream -- for single-key edge-case / PractRand.
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

// Single <=255-word block, singleblock mode (no reseed).
static void ra_core_singleblock_block(uint32_t key, size_t block_len,
                                       FILE *raw_stream) {
    if (block_len == 0) return;
    alignas(64) uint32_t M[256];
    ra_init_singleblock(M, key);
    uint64_t count = block_len;
    ra_permutation_cycle_singleblock(key, 0, M, &count, raw_stream);
}

// Multikey chaining: new key every `block_len`-word block, Weyl-stepped --
// same pattern as diag_init_keyterm_mul.c / multikey_stream.c.
static void multikey_chain(uint32_t base_key, size_t n, size_t block_len,
                            FILE *raw_stream) {
    uint64_t block_idx = 0;
    size_t remaining = n;
    while (remaining > 0) {
        size_t this_block = remaining < block_len ? remaining : block_len;
        uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
        ra_core_singleblock_block(key, this_block, raw_stream);
        remaining -= this_block;
        ++block_idx;
    }
}

static int is_all_zero(const uint32_t *buf, size_t n) {
    for (size_t i = 0; i < n; ++i) if (buf[i] != 0) return 0;
    return 1;
}

// --edgecheck: verify key=0 (and other edge/small keys) no longer produce
// a degenerate all-zero state, in BOTH orbit (multi-cycle, reseeded) and
// singleblock modes. Also prints keyterm_l/keyterm_m at key=0 directly.
static int run_edgecheck(void) {
    uint32_t keyterm_l0 = fmix32(GUARD_L ^ (0x9e3779b7u * 0u));
    uint32_t keyterm_m0 = fmix32(GUARD_M ^ (0x06a0dd9bu * 0u));
    printf("key=0 keyterm_l=0x%08x keyterm_m=0x%08x (both must be nonzero)\n",
           keyterm_l0, keyterm_m0);
    int ok = (keyterm_l0 != 0 && keyterm_m0 != 0);

    // 0x777d91ac / 0x9c0387d6: the two keys where keyterm_l/keyterm_m
    // individually become 0 under the new guard (see header comment) --
    // included to empirically confirm they do NOT reproduce the
    // catastrophic all-zero attractor the way key=0 did.
    uint32_t test_keys[] = {0u, 0xFFFFFFFFu, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u,
                             0x777d91acu, 0x9c0387d6u};
    int nkeys = (int)(sizeof(test_keys) / sizeof(test_keys[0]));

    for (int ki = 0; ki < nkeys; ++ki) {
        uint32_t key = test_keys[ki];

        // orbit, unbounded, 2000 words (crosses >=3 reseed cycles, same
        // depth as production-candidate-battery/RESULTS.md's original
        // discovery repro).
        uint32_t orbit_buf[2000];
        FILE *fo = fmemopen(orbit_buf, sizeof(orbit_buf), "wb");
        ra_core_orbit_stream(key, 2000, fo);
        fclose(fo);
        int orbit_degenerate = is_all_zero(orbit_buf, 2000);

        // singleblock, one 255-word block.
        uint32_t sb_buf[255];
        FILE *fs = fmemopen(sb_buf, sizeof(sb_buf), "wb");
        ra_core_singleblock_block(key, 255, fs);
        fclose(fs);
        int sb_degenerate = is_all_zero(sb_buf, 255);

        printf("key=0x%08x  orbit[0..3]=%08x %08x %08x %08x  orbit_all_zero=%s  "
               "singleblock[0..3]=%08x %08x %08x %08x  singleblock_all_zero=%s\n",
               key, orbit_buf[0], orbit_buf[1], orbit_buf[2], orbit_buf[3],
               orbit_degenerate ? "YES(FAIL)" : "no",
               sb_buf[0], sb_buf[1], sb_buf[2], sb_buf[3],
               sb_degenerate ? "YES(FAIL)" : "no");

        if (orbit_degenerate || sb_degenerate) ok = 0;
    }

    printf(ok ? "edgecheck: PASS -- no degenerate (all-zero) state for any tested key.\n"
              : "edgecheck: FAIL -- at least one key still produces a degenerate state.\n");
    return ok ? 0 : 1;
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--edgecheck") == 0) {
        return run_edgecheck();
    }
    if (argc >= 4 && strcmp(argv[1], "--stream") == 0) {
        // unbounded orbit stream (single key, reseeding) -- for PractRand
        // single-key sanity / edge-case streaming.
        uint32_t key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t n = (size_t)strtoull(argv[3], NULL, 0);
        ra_core_orbit_stream(key, n, stdout);
        return 0;
    }
    if (argc >= 4 && strcmp(argv[1], "--multikey255") == 0) {
        // multikey chaining, 255-word blocks -- singleblock K=255 pattern,
        // same shape as the BCFN defect discovery.
        uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t n = (size_t)strtoull(argv[3], NULL, 0);
        multikey_chain(base_key, n, 255, stdout);
        return 0;
    }
    if (argc >= 4 && strcmp(argv[1], "--multikey1") == 0) {
        // multikey chaining, 1-word blocks -- singleblock K=1 pattern
        // (most aggressive reinit rate).
        uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t n = (size_t)strtoull(argv[3], NULL, 0);
        multikey_chain(base_key, n, 1, stdout);
        return 0;
    }
    fprintf(stderr,
        "usage:\n"
        "  %s --edgecheck\n"
        "  %s --stream <key> <n>\n"
        "  %s --multikey255 <base_key> <n>\n"
        "  %s --multikey1 <base_key> <n>\n",
        argv[0], argv[0], argv[0], argv[0]);
    return 1;
}
