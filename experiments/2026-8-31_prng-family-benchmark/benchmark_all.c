// benchmark_all.c
// See HANDOVER.md in this directory for the full spec this file implements
// (12-candidate PRNG family benchmark, uniform seed+generate-in-one-call
// methodology so seeding cost is never hidden for state-heavy candidates).
//
// Read-only precedent this file copies/ports from (never edited in place):
//   src/ra_prng2/c/ra_prng2.c                                  (#1)
//   experiments/2026-8-27_operand-position-search/winner_wired_v2.c (#2)
//   experiments/2026-8-30_addressable-init-research/tahap6_bench.c (#3,#4,#5,#6)
//   benchmarks/comparisons/RNGing_speed/src/{xoshiro256,pcg32,chacha20,dev_urandom}.c (#7,#8,#9,#10)
//
// #1/#2/#3 all define bare `ra_hash`/`ra_permutation_cycle`/`ra_reseed`
// (identical names, different bodies) in their own source files -- renamed
// here with per-candidate suffixes ONLY to avoid symbol collision in this
// single translation unit. The recurrence/formula bodies are unmodified
// byte-for-byte copies; see each source file for the original names.
//
// Critical design rule (HANDOVER.md section 3): every core_fn_t below does
// seed + generate its `rng` words in ONE call, no persistent cross-call
// state. This is what makes MT19937/xoshiro256**/chacha20's real seeding
// cost show up in reinit-sweep instead of being hidden the way the old
// Philox pattern in tahap6_bench.c's mode_reinit_sweep hid it (ctr/key reset
// via plain literals outside any real reseed function -- fine for Philox's
// near-zero reseed, wrong for anything with non-trivial seed setup).
//
// Compile:
//   gcc -O3 -march=native -std=gnu17 -include stdalign.h benchmark_all.c \
//       -o benchmark_all -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

typedef uint32_t (*core_fn_t)(uint32_t key, size_t rng, FILE *raw_stream);
typedef struct { const char *name; core_fn_t fn; long max_rng; } core_entry_t;

// ----------------------------------------------------------------------
// #1 -- ra_prng2 original (paper-exact). Byte-for-byte copy of
// src/ra_prng2/c/ra_prng2.c's ra_core/ra_init_state/ra_permutation_cycle/
// ra_reseed/ra_hash. Init state is seed-independent (fixed) -- key only
// enters as `cons` in ra_core_p1, per HANDOVER §2 row 1.
// ----------------------------------------------------------------------

static void ra_hash_p1(uint32_t *N, uint32_t *out8) {
    for (uint8_t i = 0; i < 8; ++i) out8[i] = 0;
    for (uint8_t i = 0; i < 8; ++i) {
        out8[i] ^= N[(uint8_t)N[i]];
        for (uint8_t j = 0; j < 32; j++) out8[i] ^= N[j * 8 + i];
    }
}

static void ra_init_state_p1(uint32_t *L, uint32_t *M) {
    for (int i = 0; i < 256; ++i) {
        M[i] = (uint32_t)(i * 0x06a0dd9bUL + 0x06a0dd9bUL);
        L[i] = (uint32_t)(i * 0x9e3779b7UL + 0x9e3779b7UL);
    }
}

static void ra_permutation_cycle_p1(uint32_t cons, size_t it,
                                     const uint32_t *M, uint32_t *L,
                                     uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = 0;
        for (uint8_t e = 0; e < 8; ++e) o ^= (M[(uint8_t)(i + e)] << e);

        a = (rot32(b ^ o, d) ^ (cons + a));
        b = (rot32(cons + a, i) ^ (o + d));
        o = (rot32(a ^ o, i) << 9 ^ (b >> 18));
        c = rot32((o + c << 14) ^ (b >> 13) ^ a, b);
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = (uint32_t)(((uint64_t)c * (i + 1)) >> 32);

        if (*count <= 1) break;
        --(*count);

        uint32_t tmp = L[i]; L[i] = L[d]; L[d] = tmp;
    }
}

static uint32_t ra_reseed_p1(uint32_t *M, const uint32_t *L) {
    uint32_t tmp8[8];
    for (uint16_t i = 0; i < 256; ++i) M[i] ^= L[i];
    ra_hash_p1(M, tmp8);
    uint32_t new_cons = 0;
    for (uint8_t e = 0; e < 8; ++e) new_cons ^= tmp8[e] << e;
    return new_cons;
}

static uint32_t ra_core_p1(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;
    uint32_t cons = key;
    ra_init_state_p1(L, M);
    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_p1(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed_p1(M, L);
    }
    return cons;
}

// ----------------------------------------------------------------------
// #2 -- winner_wired_v2 (best non-addressable). Byte-for-byte copy of
// experiments/2026-8-27_operand-position-search/winner_wired_v2.c. Also
// seed-independent init (same as #1's fixed-table pattern).
// ----------------------------------------------------------------------

static void ra_hash_p2(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}

static void ra_init_state_p2(uint32_t *L, uint32_t *M) {
    for (int i = 0; i < 256; ++i) {
        M[i] = (uint32_t)(i * 0x06a0dd9bUL + 0x06a0dd9bUL);
        L[i] = (uint32_t)(i * 0x9e3779b7UL + 0x9e3779b7UL);
    }
}

static void ra_permutation_cycle_p2(uint32_t cons, size_t it,
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

static uint32_t ra_reseed_p2(uint32_t *M, const uint32_t *L) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];
    uint32_t tmp8[8];
    ra_hash_p2(M, tmp8);
    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

static uint32_t ra_core_p2(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;
    uint32_t cons = key;
    ra_init_state_p2(L, M);
    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_p2(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed_p2(M, L);
    }
    return cons;
}

// ----------------------------------------------------------------------
// #3/#4/#5 -- addressable "continuing" and addressable agresif-minimal
// (singleblock). Byte-for-byte copy of tahap6_bench.c (function names kept
// as in that file since they're already unique in this TU).
// ----------------------------------------------------------------------

#define ADDR_L_MIX_CONST 0x9e3779b7u

static void ra_hash_addr(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}

static void ra_init_state_addressable_baseline(uint32_t *L, uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * ADDR_L_MIX_CONST + 0x9e3779b7u * key);
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

static void ra_permutation_cycle_addr(uint32_t cons, size_t it,
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

static uint32_t ra_reseed_addr(uint32_t *M, const uint32_t *L) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];
    uint32_t tmp8[8];
    ra_hash_addr(M, tmp8);
    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

static uint32_t ra_core_baseline(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;
    uint32_t cons = key;
    ra_init_state_addressable_baseline(L, M, key);
    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_addr(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed_addr(M, L);
    }
    return cons;
}

static void ra_init_state_full(uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
        M[i] = rot32(m_val, r);
    }
}

static void ra_permutation_cycle_full(uint32_t cons, size_t it,
                                       const uint32_t *M,
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
    }
}

static uint32_t ra_core_singleblock(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) {
        fprintf(stderr, "ra_core_singleblock: rng=%zu > 255 out of scope.\n", rng);
        abort();
    }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_full(M, key);
    ra_permutation_cycle_full(cons, /*it=*/0, M, &count, raw_stream);
    return cons;
}

// ----------------------------------------------------------------------
// #6 -- Philox4x32-10. Byte-for-byte copy of tahap6_bench.c (corrected
// PHILOX_M0=0xD2511F53U -- do NOT copy from benchmarks/comparisons/, those
// still have the Philox2x32 constant bug).
// ----------------------------------------------------------------------

#define PHILOX_M0 0xD2511F53U
#define PHILOX_M1 0xCD9E8D57U
#define PHILOX_W0 0x9E3779B9U
#define PHILOX_W1 0xBB67AE85U

static inline void philox4x32_round(uint32_t ctr[4], uint32_t key[2]) {
    uint64_t p0 = (uint64_t)ctr[0] * PHILOX_M0;
    uint64_t p1 = (uint64_t)ctr[2] * PHILOX_M1;
    uint32_t hi0 = (uint32_t)(p0 >> 32), lo0 = (uint32_t)p0;
    uint32_t hi1 = (uint32_t)(p1 >> 32), lo1 = (uint32_t)p1;
    uint32_t c1 = ctr[1], c3 = ctr[3];
    ctr[0] = hi1 ^ c1 ^ key[0];
    ctr[1] = lo1;
    ctr[2] = hi0 ^ c3 ^ key[1];
    ctr[3] = lo0;
    key[0] += PHILOX_W0;
    key[1] += PHILOX_W1;
}

static void philox4x32_10(uint32_t out[4], const uint32_t ctr_in[4], const uint32_t key_in[2]) {
    uint32_t ctr[4] = { ctr_in[0], ctr_in[1], ctr_in[2], ctr_in[3] };
    uint32_t key[2] = { key_in[0], key_in[1] };
    for (int i = 0; i < 10; i++) philox4x32_round(ctr, key);
    memcpy(out, ctr, 4 * sizeof(uint32_t));
}

static inline void philox_ctr_increment(uint32_t ctr[4]) {
    if (++ctr[0] == 0) if (++ctr[1] == 0) if (++ctr[2] == 0) ++ctr[3];
}

// Philox core_fn_t wrapper: key seeds ctr[0]/key[0], counter increments
// per output word -- seed+generate together, matching every other core.
static uint32_t ra_core_philox(uint32_t key, size_t rng, FILE *raw_stream) {
    uint32_t ctr[4] = {0, 0, 0, 0};
    uint32_t pkey[2] = { key, 0xFEEDC0DEu };
    uint32_t last = key;
    size_t produced = 0;
    while (produced < rng) {
        uint32_t out[4];
        philox4x32_10(out, ctr, pkey);
        philox_ctr_increment(ctr);
        for (int i = 0; i < 4 && produced < rng; i++, produced++) {
            last = out[i];
            if (raw_stream) fwrite(&last, sizeof(uint32_t), 1, raw_stream);
        }
    }
    return last;
}

// ----------------------------------------------------------------------
// #7 -- xoshiro256**. Math ported from benchmarks/comparisons/RNGing_speed/
// src/xoshiro256.c (rotl/next/state recurrence copied verbatim); that file
// seeds ONCE before its own timing loop, which is the exact bias this
// harness must not repeat, so only the math is reused, not its benchmark
// structure. A single uint32_t key can't fill 256 bits of state directly,
// so state is expanded via SplitMix64(key) -- a documented, standard
// seeding recipe recommended by xoshiro's own authors for exactly this
// "small seed -> wide state" case.
// ----------------------------------------------------------------------

static inline uint64_t xoshiro_rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

static uint64_t splitmix64_step(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

static uint32_t ra_core_xoshiro256(uint32_t key, size_t rng, FILE *raw_stream) {
    uint64_t sm_state = (uint64_t)key;
    uint64_t s[4];
    s[0] = splitmix64_step(&sm_state);
    s[1] = splitmix64_step(&sm_state);
    s[2] = splitmix64_step(&sm_state);
    s[3] = splitmix64_step(&sm_state);

    uint32_t last = key;
    for (size_t i = 0; i < rng; i++) {
        const uint64_t result = xoshiro_rotl(s[1] * 5, 7) * 9;
        const uint64_t t = s[1] << 17;
        s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3];
        s[2] ^= t; s[3] = xoshiro_rotl(s[3], 45);
        last = (uint32_t)result;
        if (raw_stream) fwrite(&last, sizeof(uint32_t), 1, raw_stream);
    }
    return last;
}

// ----------------------------------------------------------------------
// #8 -- pcg32. Byte-for-byte math port of benchmarks/comparisons/
// RNGing_speed/src/pcg32.c (pcg32_srandom/pcg32_random_r), itself already
// a faithful copy of O'Neill's public "minimal C implementation". Seeding
// is 2 multiply-adds -- nearly free, no special treatment needed.
// ----------------------------------------------------------------------

static uint32_t ra_core_pcg32(uint32_t key, size_t rng, FILE *raw_stream) {
    uint64_t state = 0U;
    uint64_t inc = (0xda3e39cb94b95bdbULL << 1u) | 1u; // fixed odd increment selector
    state = state * 6364136223846793005ULL + inc;
    state += (uint64_t)key;
    state = state * 6364136223846793005ULL + inc;

    uint32_t last = key;
    for (size_t i = 0; i < rng; i++) {
        uint64_t oldstate = state;
        state = oldstate * 6364136223846793005ULL + inc;
        uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
        uint32_t rot = oldstate >> 59u;
        last = (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
        if (raw_stream) fwrite(&last, sizeof(uint32_t), 1, raw_stream);
    }
    return last;
}

// ----------------------------------------------------------------------
// #9 -- chacha20. Block-function math ported from benchmarks/comparisons/
// RNGing_speed/src/chacha20.c's quarter-round/state layout (verified
// against RFC 8439 section 2.3.2's official test vector during this
// session -- the round logic is correct; the REFERENCE FILE'S OWN benchmark
// main() has a transcription bug in its hardcoded nonce words, byte-order
// swapped vs the RFC's canonical representation, so its numeric main()
// constants must NOT be reused here, only the chacha20_block() math).
// Key+nonce (256+96 bits) expanded from the single uint32_t key via
// SplitMix64, same recipe as xoshiro256** above -- seeding is real cost
// here (CSPRNG key schedule), so it's timed inside this call like everyone
// else, per HANDOVER §2 row 9.
// ----------------------------------------------------------------------

static inline uint32_t chacha_rotl32(uint32_t x, int r) { return (x << r) | (x >> (32 - r)); }

#define CHACHA_QR(a, b, c, d) \
    a += b; d ^= a; d = chacha_rotl32(d,16); \
    c += d; b ^= c; b = chacha_rotl32(b,12); \
    a += b; d ^= a; d = chacha_rotl32(d, 8); \
    c += d; b ^= c; b = chacha_rotl32(b, 7);

static void chacha20_block(uint32_t out[16], const uint32_t key[8], uint32_t counter, const uint32_t nonce[3]) {
    const uint32_t constants[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
    uint32_t state[16];
    memcpy(state + 0, constants, 16);
    memcpy(state + 4, key, 32);
    state[12] = counter;
    memcpy(state + 13, nonce, 12);

    uint32_t x[16];
    memcpy(x, state, sizeof x);
    for (int i = 0; i < 10; i++) {
        CHACHA_QR(x[0], x[4], x[8], x[12]);
        CHACHA_QR(x[1], x[5], x[9], x[13]);
        CHACHA_QR(x[2], x[6], x[10], x[14]);
        CHACHA_QR(x[3], x[7], x[11], x[15]);
        CHACHA_QR(x[0], x[5], x[10], x[15]);
        CHACHA_QR(x[1], x[6], x[11], x[12]);
        CHACHA_QR(x[2], x[7], x[8], x[13]);
        CHACHA_QR(x[3], x[4], x[9], x[14]);
    }
    for (int i = 0; i < 16; i++) out[i] = x[i] + state[i];
}

static uint32_t ra_core_chacha20(uint32_t key, size_t rng, FILE *raw_stream) {
    uint64_t sm_state = (uint64_t)key;
    uint32_t ckey[8];
    for (int i = 0; i < 4; i++) {
        uint64_t v = splitmix64_step(&sm_state);
        ckey[2 * i] = (uint32_t)v;
        ckey[2 * i + 1] = (uint32_t)(v >> 32);
    }
    uint64_t nv = splitmix64_step(&sm_state);
    uint32_t nonce[3] = { (uint32_t)nv, (uint32_t)(nv >> 32), key };
    uint32_t counter = 0;

    uint32_t last = key;
    size_t produced = 0;
    while (produced < rng) {
        uint32_t block[16];
        chacha20_block(block, ckey, counter++, nonce);
        for (int i = 0; i < 16 && produced < rng; i++, produced++) {
            last = block[i];
            if (raw_stream) fwrite(&last, sizeof(uint32_t), 1, raw_stream);
        }
    }
    return last;
}

// ----------------------------------------------------------------------
// #11 -- MT19937. New implementation (none existed in this repo). Classic
// LAZY twist exactly as the official mt19937ar.c reference: mti starts at
// N+1 after init_genrand(seed), and the state-regenerating twist happens
// automatically inside the first genrand_int32() call, not eagerly right
// after seeding. State is kept in LOCAL variables (not the reference's
// file-static globals) so this core_fn_t has no cross-call state, per the
// harness's seed+generate-in-one-call rule. Seeded via init_genrand(key)
// (single uint32_t, matching this file's uniform core_fn_t signature) --
// the C++ standard library's std::mt19937 default-seed(5489) engine uses
// this exact same classic recurrence, giving a well-known cross-check
// value (KAT below).
// ----------------------------------------------------------------------

#define MT_N 624
#define MT_M 397
#define MT_MATRIX_A 0x9908b0dfUL
#define MT_UPPER_MASK 0x80000000UL
#define MT_LOWER_MASK 0x7fffffffUL

typedef struct { uint32_t mt[MT_N]; int mti; } mt19937_state_t;

static void mt19937_init_genrand(mt19937_state_t *s, uint32_t seed) {
    s->mt[0] = seed;
    for (s->mti = 1; s->mti < MT_N; s->mti++) {
        s->mt[s->mti] = (1812433253UL * (s->mt[s->mti - 1] ^ (s->mt[s->mti - 1] >> 30)) + (uint32_t)s->mti);
    }
    // mti intentionally left at MT_N (not reset to N+1): init_genrand always
    // fills the full table, so the first genrand call's twist condition
    // (mti >= N) is already true -- equivalent to the reference's N+1
    // sentinel for "never seeded", just reached via a real seed here.
}

static uint32_t mt19937_genrand_int32(mt19937_state_t *s) {
    static const uint32_t mag01[2] = {0x0UL, MT_MATRIX_A};
    uint32_t y;
    if (s->mti >= MT_N) {
        int kk;
        for (kk = 0; kk < MT_N - MT_M; kk++) {
            y = (s->mt[kk] & MT_UPPER_MASK) | (s->mt[kk + 1] & MT_LOWER_MASK);
            s->mt[kk] = s->mt[kk + MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (; kk < MT_N - 1; kk++) {
            y = (s->mt[kk] & MT_UPPER_MASK) | (s->mt[kk + 1] & MT_LOWER_MASK);
            s->mt[kk] = s->mt[kk + (MT_M - MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (s->mt[MT_N - 1] & MT_UPPER_MASK) | (s->mt[0] & MT_LOWER_MASK);
        s->mt[MT_N - 1] = s->mt[MT_M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
        s->mti = 0;
    }
    y = s->mt[s->mti++];
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);
    return y;
}

static uint32_t ra_core_mt19937(uint32_t key, size_t rng, FILE *raw_stream) {
    mt19937_state_t s;
    mt19937_init_genrand(&s, key);
    uint32_t last = key;
    for (size_t i = 0; i < rng; i++) {
        last = mt19937_genrand_int32(&s);
        if (raw_stream) fwrite(&last, sizeof(uint32_t), 1, raw_stream);
    }
    return last;
}

// ----------------------------------------------------------------------
// #12 -- SplitMix64. New implementation (repo only has a Python internal
// mixer at tahap0_prototype.py:119-166 -- not trusted as an automatic
// correct port per HANDOVER §2 row 12; this is an independent C
// implementation of Vigna's public constants). Each 64-bit output is
// truncated to its low 32 bits to match this harness's uint32_t word
// granularity, one call per output word. Seeding is 1 word -- nearly free.
// ----------------------------------------------------------------------

static uint32_t ra_core_splitmix64(uint32_t key, size_t rng, FILE *raw_stream) {
    uint64_t state = (uint64_t)key;
    uint32_t last = key;
    for (size_t i = 0; i < rng; i++) {
        uint64_t v = splitmix64_step(&state);
        last = (uint32_t)v;
        if (raw_stream) fwrite(&last, sizeof(uint32_t), 1, raw_stream);
    }
    return last;
}

// ----------------------------------------------------------------------
// CORES[] -- 10 entries (all seed+generate candidates except #10
// /dev/urandom, which has no key/reinit concept and gets its own mode).
// #4/#5 in HANDOVER's roster are two specific sweep points (K=1, K=255) of
// this SAME "singleblock" entry, not separate implementations -- matches
// tahap6_bench.c's own CORES[] precedent of one "singleblock" row.
// ----------------------------------------------------------------------

static const core_entry_t CORES[] = {
    { "paperorig",   ra_core_p1,          -1 },
    { "wiredv2",     ra_core_p2,          -1 },
    { "addrcont",    ra_core_baseline,    -1 },
    { "singleblock", ra_core_singleblock, 255 },
    { "philox",      ra_core_philox,      -1 },
    { "xoshiro256",  ra_core_xoshiro256,  -1 },
    { "pcg32",       ra_core_pcg32,       -1 },
    { "chacha20",    ra_core_chacha20,    -1 },
    { "mt19937",     ra_core_mt19937,     -1 },
    { "splitmix64",  ra_core_splitmix64,  -1 },
};
#define NUM_CORES (sizeof(CORES) / sizeof(CORES[0]))

static const core_entry_t *find_core(const char *name) {
    for (size_t i = 0; i < NUM_CORES; ++i) if (strcmp(CORES[i].name, name) == 0) return &CORES[i];
    fprintf(stderr, "Unknown core '%s'. Known:", name);
    for (size_t i = 0; i < NUM_CORES; ++i) fprintf(stderr, " %s", CORES[i].name);
    fprintf(stderr, "\n");
    exit(1);
}

// ----------------------------------------------------------------------
// KAT vectors. Sources documented per block; see HANDOVER §5 for the
// gate this implements: verify() must PASS for every candidate with a KAT
// block before ANY benchmarking is trusted.
// ----------------------------------------------------------------------

static int kat_philox(void) {
    struct { uint32_t ctr[4]; uint32_t key[2]; uint32_t expect[4]; } kats[] = {
        { {0,0,0,0}, {0,0}, {0x6627e8d5u, 0xe169c58du, 0xbc57ac4cu, 0x9b00dbd8u} },
        { {0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu}, {0xffffffffu,0xffffffffu},
          {0x408f276du, 0x41c83b0eu, 0xa20bc7c6u, 0x6d5451fdu} },
        { {0x243f6a88u,0x85a308d3u,0x13198a2eu,0x03707344u}, {0xa4093822u,0x299f31d0u},
          {0xd16cfe09u, 0x94fdccebu, 0x5001e420u, 0x24126ea1u} },
    };
    int ok_all = 1;
    for (size_t k = 0; k < sizeof(kats) / sizeof(kats[0]); ++k) {
        uint32_t out[4];
        philox4x32_10(out, kats[k].ctr, kats[k].key);
        int ok = memcmp(out, kats[k].expect, sizeof(out)) == 0;
        printf("  [philox] KAT %zu: %s\n", k, ok ? "PASS" : "FAIL");
        if (!ok) ok_all = 0;
    }
    return ok_all;
}

// Source: RFC 8439 section 2.4.2's Poly1305-key-generation worked example
// re-purposed as a raw chacha20_block() KAT (block counter=0). Fetched and
// cross-checked directly against RFC 8439 text during this session.
static int kat_chacha20(void) {
    uint32_t key[8] = {
        0x83828180u,0x87868584u,0x8b8a8988u,0x8f8e8d8cu,
        0x93929190u,0x97969594u,0x9b9a9998u,0x9f9e9d9cu,
    };
    uint32_t nonce[3] = { 0x00000000u, 0x03020100u, 0x07060504u };
    uint32_t out[16];
    chacha20_block(out, key, 0u, nonce);
    static const uint32_t expect[16] = {
        0x8ba0d58au,0xcc815f90u,0x27405081u,0x7194b24au,
        0x37b633a8u,0xa50dfde3u,0xe2b8db08u,0x46a6d1fdu,
        0x7da03782u,0x9183a233u,0x148ad271u,0xb46773d1u,
        0x3cc1875au,0x8607def1u,0xca5c3086u,0x7085eb87u,
    };
    int ok = memcmp(out, expect, sizeof(out)) == 0;
    printf("  [chacha20] KAT (RFC 8439 sec 2.6.2 block, counter=0): %s\n", ok ? "PASS" : "FAIL");

    // Second, independent vector: RFC 8439 sec 2.3.2, counter=1, key=00:01:..:1f.
    // This is the vector that caught the existing repo reference file's
    // nonce byte-order bug during this session's research.
    uint32_t key2[8] = {
        0x03020100u,0x07060504u,0x0b0a0908u,0x0f0e0d0cu,
        0x13121110u,0x17161514u,0x1b1a1918u,0x1f1e1d1cu,
    };
    uint32_t nonce2[3] = { 0x09000000u, 0x4a000000u, 0x00000000u };
    uint32_t out2[16];
    chacha20_block(out2, key2, 1u, nonce2);
    static const uint32_t expect2[16] = {
        0xe4e7f110u,0x15593bd1u,0x1fdd0f50u,0xc47120a3u,
        0xc7f4d1c7u,0x0368c033u,0x9aaa2204u,0x4e6cd4c3u,
        0x466482d2u,0x09aa9f07u,0x05d7c214u,0xa2028bd9u,
        0xd19c12b5u,0xb94e16deu,0xe883d0cbu,0x4e3c50a2u,
    };
    int ok2 = memcmp(out2, expect2, sizeof(out2)) == 0;
    printf("  [chacha20] KAT (RFC 8439 sec 2.3.2 block, counter=1): %s\n", ok2 ? "PASS" : "FAIL");
    return ok && ok2;
}

// Source: xoshiro256_next() is a byte-for-byte port of this repo's own
// benchmarks/comparisons/RNGing_speed/src/xoshiro256.c -- KAT here checks
// this file's copy reproduces THAT file's own hardcoded main() seed
// bit-identically (its recurrence is Blackman & Vigna's public-domain
// xoshiro256** reference: the "rotl(s[1]*5,7)*9" scrambler is that
// algorithm's distinctive signature).
static int kat_xoshiro256(void) {
    uint64_t s[4] = {
        0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL,
        0xF0E1D2C3B4A59687ULL, 0x1234567890ABCDEFULL,
    };
    static const uint64_t expect[4] = {
        0x6666666666666c65ULL, 0x26f9cc9f72450d4dULL,
        0x83f9a07f50000477ULL, 0xbf1c5914f01d3354ULL,
    };
    int ok_all = 1;
    for (int i = 0; i < 4; i++) {
        uint64_t result = xoshiro_rotl(s[1] * 5, 7) * 9;
        uint64_t t = s[1] << 17;
        s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3];
        s[2] ^= t; s[3] = xoshiro_rotl(s[3], 45);
        int ok = result == expect[i];
        printf("  [xoshiro256**] KAT out[%d] (repo reference seed): %s\n", i, ok ? "PASS" : "FAIL");
        if (!ok) ok_all = 0;
    }
    return ok_all;
}

// Source: pcg32_random_r() is a byte-for-byte port of this repo's own
// benchmarks/comparisons/RNGing_speed/src/pcg32.c (itself O'Neill's public
// "minimal C implementation", pcg-random.org) -- KAT reuses that file's own
// seed(42,54).
static int kat_pcg32(void) {
    uint64_t state = 0U;
    uint64_t inc = (54ULL << 1u) | 1u;
    state = state * 6364136223846793005ULL + inc;
    state += 42ULL;
    state = state * 6364136223846793005ULL + inc;

    static const uint32_t expect[6] = {
        0xa15c02b7u, 0x7b47f409u, 0xba1d3330u, 0x83d2f293u, 0xbfa4784bu, 0xcbed606eu,
    };
    int ok_all = 1;
    for (int i = 0; i < 6; i++) {
        uint64_t oldstate = state;
        state = oldstate * 6364136223846793005ULL + inc;
        uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
        uint32_t rot = oldstate >> 59u;
        uint32_t out = (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
        int ok = out == expect[i];
        printf("  [pcg32] KAT out[%d] (seed=42,seq=54): %s\n", i, ok ? "PASS" : "FAIL");
        if (!ok) ok_all = 0;
    }
    return ok_all;
}

// Source: official mt19937ar.c reference test (Matsumoto & Nishimura),
// init_by_array({0x123,0x234,0x345,0x456}, 4) -> first output 1067595299 is
// the canonical, extremely widely-cited test vector for the core twist+
// temper recurrence. Second vector: init_genrand(5489) -> 3499211612 is the
// C++ standard library's documented std::mt19937 default-seed engine
// output -- cross-checks the init_genrand front-end this harness actually
// uses for its core_fn_t (key is a single uint32_t, not an array).
static int kat_mt19937(void) {
    mt19937_state_t s;

    // Vector A: init_by_array (validates the twist/temper recurrence itself).
    uint32_t init_key[4] = {0x123, 0x234, 0x345, 0x456};
    s.mt[0] = 19650218UL;
    for (s.mti = 1; s.mti < MT_N; s.mti++)
        s.mt[s.mti] = (1812433253UL * (s.mt[s.mti-1] ^ (s.mt[s.mti-1] >> 30)) + (uint32_t)s.mti);
    {
        int i = 1, j = 0;
        int k = (MT_N > 4 ? MT_N : 4);
        for (; k; k--) {
            s.mt[i] = (s.mt[i] ^ ((s.mt[i-1] ^ (s.mt[i-1] >> 30)) * 1664525UL)) + init_key[j] + (uint32_t)j;
            i++; j++;
            if (i >= MT_N) { s.mt[0] = s.mt[MT_N-1]; i = 1; }
            if (j >= 4) j = 0;
        }
        for (k = MT_N - 1; k; k--) {
            s.mt[i] = (s.mt[i] ^ ((s.mt[i-1] ^ (s.mt[i-1] >> 30)) * 1566083941UL)) - (uint32_t)i;
            i++;
            if (i >= MT_N) { s.mt[0] = s.mt[MT_N-1]; i = 1; }
        }
        s.mt[0] = 0x80000000UL;
    }
    uint32_t outA = mt19937_genrand_int32(&s);
    int okA = outA == 1067595299u;
    printf("  [mt19937] KAT init_by_array({0x123,0x234,0x345,0x456}) out[0]: %s (got %u, expect 1067595299)\n",
           okA ? "PASS" : "FAIL", outA);

    // Vector B: init_genrand(5489) -- what this harness's core_fn_t actually uses.
    mt19937_init_genrand(&s, 5489UL);
    uint32_t outB = mt19937_genrand_int32(&s);
    int okB = outB == 3499211612u;
    printf("  [mt19937] KAT init_genrand(5489) out[0]: %s (got %u, expect 3499211612, std::mt19937 default)\n",
           okB ? "PASS" : "FAIL", outB);

    return okA && okB;
}

// Source: Vigna's publicly documented SplitMix64 constants
// (0x9E3779B97F4A7C15 / 0xBF58476D1CE4E5B9 / 0x94D049BB133111EB, shifts
// 30/27/31), independently reimplemented and computed fresh in this
// session (not copied from the repo's untrusted Python port, per HANDOVER
// §2 row 12).
static int kat_splitmix64(void) {
    uint64_t state = 0;
    static const uint64_t expect[4] = {
        0xe220a8397b1dcdafULL, 0x6e789e6aa1b965f4ULL,
        0x06c45d188009454fULL, 0xf88bb8a8724c81ecULL,
    };
    int ok_all = 1;
    for (int i = 0; i < 4; i++) {
        uint64_t v = splitmix64_step(&state);
        int ok = v == expect[i];
        printf("  [splitmix64] KAT out[%d] (seed=0): %s\n", i, ok ? "PASS" : "FAIL");
        if (!ok) ok_all = 0;
    }
    return ok_all;
}

static int run_verify_all(void) {
    printf("verify: running KAT checks for every candidate with an official/cross-checkable reference...\n");
    int ok = 1;
    ok &= kat_philox();
    ok &= kat_xoshiro256();
    ok &= kat_pcg32();
    ok &= kat_chacha20();
    ok &= kat_mt19937();
    ok &= kat_splitmix64();
    printf("verify: %s\n", ok ? "ALL PASS" : "AT LEAST ONE FAILURE -- do not trust any benchmark number until fixed");
    return ok;
}

// ----------------------------------------------------------------------
// validate: exhaustive bit-identical check, singleblock vs addrcont, same
// pattern as tahap6_bench.c's run_validate_singleblock (in-process,
// fmemopen). Byte-for-byte candidates whose "original" lives in a
// DIFFERENT file (#1/#2/#6 vs ra_prng2.c/winner_wired_v2.c/tahap6_bench.c
// itself) are cross-checked by the Python driver instead (separate
// binaries + cmp), since their originals can't be pasted into this file
// without becoming a second, driftable copy.
// ----------------------------------------------------------------------

static int run_validate_singleblock(void) {
    uint32_t keys[2 + 5 + 32];
    int nkeys = 0;
    keys[nkeys++] = 0u;
    keys[nkeys++] = 0xFFFFFFFFu;
    static const uint32_t extra_keys[5] = {
        0xDEADBEEFu, 0x12345678u, 0xCAFEBABEu, 0x9E3779B9u, 0x7F4A7C15u,
    };
    for (int i = 0; i < 5; ++i) keys[nkeys++] = extra_keys[i];
    for (uint32_t k = 0; k < 32; ++k) keys[nkeys++] = k;

    long total = 0, mismatches = 0;
    uint32_t buf_base[255], buf_sb[255];

    for (int ki = 0; ki < nkeys; ++ki) {
        uint32_t key = keys[ki];
        for (size_t n = 1; n <= 255; ++n) {
            FILE *fb = fmemopen(buf_base, n * sizeof(uint32_t), "wb");
            ra_core_baseline(key, n, fb);
            fclose(fb);
            FILE *fs = fmemopen(buf_sb, n * sizeof(uint32_t), "wb");
            ra_core_singleblock(key, n, fs);
            fclose(fs);
            ++total;
            if (memcmp(buf_base, buf_sb, n * sizeof(uint32_t)) != 0) {
                ++mismatches;
                fprintf(stderr, "MISMATCH key=%u n=%zu\n", key, n);
            }
        }
    }
    printf("validate-singleblock: %ld combinations checked, %ld mismatches\n", total, mismatches);
    return mismatches == 0 ? 0 : 1;
}

// ----------------------------------------------------------------------
// reinit-sweep: generalized to any CORES[] entry. Prints call_ns (time for
// ONE seed+generate(K) call, best-of-3-trials/cycles) alongside
// ns_per_word_steadystate, so the Python driver can OLS-fit
// call_ns(K) = a + b*K uniformly across all 10 candidates -- a is the
// fixed seeding-cost estimate, b is steady-state ns/word.
// ----------------------------------------------------------------------

static void mode_reinit_sweep(const core_entry_t *core, int argc, char **argv, int start, long long cycles_target_words) {
    volatile uint32_t sink = 0;
    for (int a = start; a < argc; ++a) {
        long long k = atoll(argv[a]);
        if (k <= 0) continue;
        if (core->max_rng >= 0 && k > core->max_rng) {
            fprintf(stderr, "reinit-sweep: K=%lld exceeds core '%s' max_rng=%ld -- skipping.\n",
                    k, core->name, core->max_rng);
            continue;
        }

        long long cycles = cycles_target_words / k;
        if (cycles < 10) cycles = 10;
        if (cycles > 200000) cycles = 200000;

        const int TRIALS = 3;
        double total = -1.0;
        for (int trial = 0; trial < TRIALS; ++trial) {
            double t0 = now_seconds();
            for (long long c = 0; c < cycles; ++c) {
                uint32_t key = (uint32_t)(c * 2654435761u + 1u);
                uint32_t last = core->fn(key, (size_t)k, NULL);
                sink ^= last;
            }
            double t1 = now_seconds();
            double dt = t1 - t0;
            if (total < 0 || dt < total) total = dt;
        }

        double call_ns = total * 1e9 / (double)cycles;
        double ns_per_word = call_ns / (double)k;
        printf("core %s K %lld cycles %lld call_ns %.3f ns_per_word_steadystate %.3f checksum %u\n",
               core->name, k, cycles, call_ns, ns_per_word, sink);
    }
}

static void mode_urandom_flat(long long n) {
    FILE *ur = fopen("/dev/urandom", "rb");
    if (!ur) { perror("fopen"); exit(1); }
    uint32_t *buf = malloc((size_t)n * sizeof(uint32_t));
    if (!buf) { fprintf(stderr, "urandom-flat: out of memory for n=%lld\n", n); exit(1); }

    double t0 = now_seconds();
    size_t got = fread(buf, sizeof(uint32_t), (size_t)n, ur);
    double t1 = now_seconds();
    fclose(ur);

    if (got != (size_t)n) { fprintf(stderr, "urandom-flat: short read (%zu of %lld)\n", got, n); }
    double dt = t1 - t0;
    double ns_per_word = dt * 1e9 / (double)got;
    double mb_per_s = ((double)got * 4.0) / (1024.0 * 1024.0) / dt;
    printf("core dev_urandom N %lld ns_per_word %.3f mb_per_s %.3f\n", (long long)got, ns_per_word, mb_per_s);
    free(buf);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage:\n"
            "  %s verify\n"
            "  %s validate\n"
            "  %s --stream <core> <key> <n>\n"
            "  %s reinit-sweep <core> <cycles_target_words> <k1> <k2> ...\n"
            "  %s urandom-flat <n>\n",
            argv[0], argv[0], argv[0], argv[0], argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "verify") == 0) return run_verify_all() ? 0 : 1;
    if (strcmp(argv[1], "validate") == 0) return run_validate_singleblock();
    if (strcmp(argv[1], "--stream") == 0 && argc >= 5) {
        const core_entry_t *core = find_core(argv[2]);
        uint32_t key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t rng = (size_t)strtoull(argv[4], NULL, 0);
        core->fn(key, rng, stdout);
        return 0;
    }
    if (strcmp(argv[1], "reinit-sweep") == 0 && argc >= 5) {
        const core_entry_t *core = find_core(argv[2]);
        long long cycles_target_words = atoll(argv[3]);
        mode_reinit_sweep(core, argc, argv, 4, cycles_target_words);
        return 0;
    }
    if (strcmp(argv[1], "urandom-flat") == 0 && argc >= 3) {
        mode_urandom_flat(atoll(argv[2]));
        return 0;
    }
    fprintf(stderr, "Unknown mode or missing args: %s\n", argv[1]);
    return 1;
}
