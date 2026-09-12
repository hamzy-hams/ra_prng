// Clean microbenchmark for the orbit-vs-singleblock speed gap
// investigation (goal: "cari penyebab kenapa orbit stream bisa jauh lebih
// lambat dari singleblock, padahal perbedaannya cuma di swap L[]").
//
// FINDING 1 (see disassembly evidence in session transcript): the
// project's usual raw_stream=NULL microbenchmark methodology (bench_release.c
// style) is INVALID for ra_core_singleblock specifically. Because
// ra_core_singleblock's `cons` is never reassigned for rng<=255 (no
// ra_reseed call is reachable), `ra_core_singleblock(key, addr_hi, rng,
// NULL)` is PROVABLY always == key with zero observable side effects. GCC
// -O3 exploits this and dead-code-eliminates the ENTIRE 255-word hash loop
// -- confirmed via objdump: the "singleblock K=255" reinit loop compiled
// down to a 4-instruction closed-form XOR accumulation with NO call into
// any singleblock function at all. ra_core_orbit is NOT affected the same
// way because `cons` for rng>255 genuinely flows through ra_reseed(M,L),
// which depends on the real computed state.
//
// This file re-implements both hot loops standalone (same formula, copied
// from src/ra_core/c/ra_core.c) but writes each generated word into a
// `volatile uint32_t *accum` instead of calling fwrite/relying on the
// return value. A volatile write can never be proven side-effect-free, so
// the compiler cannot eliminate the loop for EITHER variant -- this gives
// a fair, symmetric measurement of the two recurrences' real cost.
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}
static inline uint32_t fmix32(uint32_t h) {
    h ^= h >> 16; h *= 0x85ebca6bu;
    h ^= h >> 13; h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}
#define GUARD_L 0x38916df4u
#define GUARD_M 0x6c26fc92u
#define GUARD_L2 0x3eb13b90u
#define GUARD_M2 0x392456deu

static void init_state_orbit(uint32_t *L, uint32_t *M, uint32_t key, uint32_t addr_hi) {
    uint32_t keyterm_l_key   = fmix32(GUARD_L  ^ (0x9e3779b7u * key));
    uint32_t keyterm_l_param = fmix32(GUARD_L2 ^ (0x85ebca77u * addr_hi));
    uint32_t keyterm_l = keyterm_l_key * keyterm_l_param;
    uint32_t keyterm_m_key   = fmix32(GUARD_M  ^ (0x06a0dd9bu * key));
    uint32_t keyterm_m_param = fmix32(GUARD_M2 ^ (0x9e3779b1u * addr_hi));
    uint32_t keyterm_m = keyterm_m_key * keyterm_m_param;
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        L[i] = rot32((uint32_t)(i * 0x9e3779b7u) * keyterm_l, r);
        M[i] = rot32((uint32_t)(i * 0x06a0dd9bu) * keyterm_m, r);
    }
}
static void init_state_singleblock(uint32_t *M, uint32_t key, uint32_t addr_hi) {
    uint32_t keyterm_m_key   = fmix32(GUARD_M  ^ (0x06a0dd9bu * key));
    uint32_t keyterm_m_param = fmix32(GUARD_M2 ^ (0x9e3779b1u * addr_hi));
    uint32_t keyterm_m = keyterm_m_key * keyterm_m_param;
    for (int i = 0; i < 256; ++i) {  // full init here (fair vs orbit); windowing is a separate, already-understood optimization
        uint32_t r = key ^ (uint32_t)i;
        M[i] = rot32((uint32_t)(i * 0x06a0dd9bu) * keyterm_m, r);
    }
}

// Same recurrence as ra_permutation_cycle_orbit, but writes to *accum
// (volatile) instead of fwrite. Returns true on early-stop (mirrors the
// production bool-flag fix).
static bool cycle_orbit(uint32_t cons, size_t it, const uint32_t *M, uint32_t *L,
                         uint64_t *count, volatile uint32_t *accum) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    uint32_t o = (M[(uint8_t)(255+0)]<<0)^(M[(uint8_t)(255+1)]<<1)^(M[(uint8_t)(255+2)]<<2)^
                 (M[(uint8_t)(255+3)]<<3)^(M[(uint8_t)(255+4)]<<4)^(M[(uint8_t)(255+5)]<<5)^
                 (M[(uint8_t)(255+6)]<<6)^(M[(uint8_t)(255+7)]<<7);
    uint32_t o_term7 = M[(uint8_t)(255+7)] << 7;
    for (uint32_t i = 255; i > 0; --i) {
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 17u;
        *accum ^= c;
        d = c & 0xFFu;
        if (*count <= 1) return true;
        --(*count);
        uint32_t tmp = L[i]; L[i] = L[d]; L[d] = tmp;
        if (i > 1) {
            uint32_t new_term = M[(uint8_t)(i-1)];
            uint32_t o_next = new_term ^ ((o ^ o_term7) << 1);
            o_term7 = M[(uint8_t)(i+6)] << 7;
            o = o_next;
        }
    }
    return false;
}

// Same recurrence as ra_permutation_cycle_singleblock (no L, no swap),
// writes to *accum instead of fwrite.
static void cycle_singleblock(uint32_t cons, size_t it, const uint32_t *M,
                               uint64_t *count, volatile uint32_t *accum) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    uint32_t o = (M[(uint8_t)(255+0)]<<0)^(M[(uint8_t)(255+1)]<<1)^(M[(uint8_t)(255+2)]<<2)^
                 (M[(uint8_t)(255+3)]<<3)^(M[(uint8_t)(255+4)]<<4)^(M[(uint8_t)(255+5)]<<5)^
                 (M[(uint8_t)(255+6)]<<6)^(M[(uint8_t)(255+7)]<<7);
    uint32_t o_term7 = M[(uint8_t)(255+7)] << 7;
    for (uint32_t i = 255; i > 0; --i) {
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 17u;
        *accum ^= c;
        d = c & 0xFFu;
        if (*count <= 1) break;
        --(*count);
        if (i > 1) {
            uint32_t new_term = M[(uint8_t)(i-1)];
            uint32_t o_next = new_term ^ ((o ^ o_term7) << 1);
            o_term7 = M[(uint8_t)(i+6)] << 7;
            o = o_next;
        }
    }
}

static uint32_t ra_hash8(const uint32_t *N) {
    uint32_t out = 0;
    for (int i = 0; i < 8; ++i) {
        uint32_t v = 0;
        for (int j = 0; j < 32; ++j) v ^= N[i*32+j];
        out ^= (v << i);
    }
    return out;
}
static uint32_t reseed(uint32_t *M, const uint32_t *L) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];
    return ra_hash8(M);
}

static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

// orbit_true: full multi-cycle stream, N words, single init (steady state).
static double bench_orbit_true(long long n) {
    const int TRIALS = 5;
    double best = -1.0;
    alignas(64) uint32_t L[256], M[256];
    volatile uint32_t accum = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        init_state_orbit(L, M, 0xC0FFEEu, 0u);
        uint64_t count = (uint64_t)n;
        uint64_t iteration = (uint64_t)n / 255 + 1;
        uint32_t cons = 0xC0FFEEu;
        double t0 = now_seconds();
        for (uint64_t it = 0; it < iteration; ++it) {
            bool done = cycle_orbit(cons, it, M, L, &count, &accum);
            if (done) break;
            cons = reseed(M, L);
        }
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    return best * 1e9 / (double)n;
}

// singleblock_true: N words via repeated fresh 255-word blocks (K-reinit
// style, since singleblock structurally has no cross-block continuation --
// that IS the point of the no-L design). Each block uses a different key
// so results can't collapse to a trivial repeated computation.
static double bench_singleblock_true(long long n) {
    const int TRIALS = 5;
    double best = -1.0;
    volatile uint32_t accum = 0;
    long long blocks = n / 255;
    if (blocks < 1) blocks = 1;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        for (long long blk = 0; blk < blocks; ++blk) {
            uint32_t key = (uint32_t)(blk * 2654435761u + 1u);
            uint32_t M[256];
            init_state_singleblock(M, key, 0u);
            uint64_t count = 255;
            cycle_singleblock(key, 0, M, &count, &accum);
        }
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    (void)accum;
    return best * 1e9 / (double)(blocks * 255);
}

int main(void) {
    long long N = 50000000LL;
    printf("orbit_true (full cycle, swap+reseed):        %.4f ns/word\n", bench_orbit_true(N));
    printf("singleblock_true (no swap, no reseed):        %.4f ns/word\n", bench_singleblock_true(N));
    return 0;
}
