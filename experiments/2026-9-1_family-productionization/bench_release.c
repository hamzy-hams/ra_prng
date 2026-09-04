// bench_release.c
// Release-benchmark for ra_core.c (family-productionization), 2026-09-04.
// Purpose: produce the 3-number-per-mode table requested for the README
// Performance section (init time / generate-per-word steady-state /
// real measured throughput), for the 5 standard modes tracked by this repo
// (memory `feedback_orbit_singleblock_benchmark_standard`): singleblock
// K=1, singleblock K=255, orbit --multistream K=1, orbit --multistream
// K=255, orbit --stream -- plus Philox4x32-10 and Xoshiro256** as reference
// baselines, measured in the same harness for apples-to-apples comparison.
//
// Two linking strategies, both established precedent in this folder:
//   1. Real per-call/per-stream timing (the "real throughput" column) uses
//      extern declarations into ra_core.c's PUBLIC entry points
//      (ra_core_orbit / ra_core_singleblock), exactly like bench_ra_core.c.
//      ra_core.c itself is NOT modified or recompiled with different flags
//      for this -- linked as an object file built with -Dmain=<other name>
//      to avoid a duplicate main() symbol.
//   2. Init-only timing (the "init time" column) needs to call
//      ra_init_state_orbit/ra_init_state_singleblock in isolation, but both
//      are `static` in ra_core.c and therefore not externally linkable.
//      Verbatim-copied below instead, same convention as
//      ../2026-9-3_init-loop-optimization/diag_init_candidates.c and
//      ../2026-9-2_singleblock-o-width-fix/diag_wideo_singleblock.c
//      (self-contained diagnostic copy, not extern-linked). Copied
//      byte-for-byte from ra_core.c as of 2026-09-04 (lines ~144-317,
//      after the rolling-o + variable-length-M[]-init promotion) --
//      rot32, fmix32, GUARD_L/GUARD_M, ra_init_state_orbit,
//      ra_init_state_singleblock. This file does not call, copy, or modify
//      ra_permutation_cycle_orbit/ra_permutation_cycle_singleblock (the
//      cycle/generate functions) -- generate-per-word is DERIVED from the
//      real total-call time minus this measured init time (see README
//      methodology note), not measured by a separate copied cycle-only
//      harness. This avoids a second verbatim copy of the (larger,
//      recently-changed) cycle functions while still keeping init time
//      honest and independently verifiable against the real numbers.
//
// Philox4x32-10 and Xoshiro256**/SplitMix64: byte-for-byte copies of the
// KAT-verified reference implementations already used elsewhere in this
// repo (bench_ra_core.c's Philox with the corrected PHILOX_M0; ../2026-9-3_init-loop-optimization/diag_init_candidates.c's
// Xoshiro+SplitMix64) -- not re-derived here.
//
// Compile:
//   gcc -O3 -march=native -std=gnu17 -include stdalign.h -Dmain=ra_core_unused_main -c ra_core.c -o ra_core_nomain.o -Wall -Wextra
//   gcc -O3 -march=native -std=gnu17 bench_release.c ra_core_nomain.o -o bench_release -Wall -Wextra
//
// Usage:
//   ./bench_release all        -- run every mode, print one line per
//                                  measurement (machine-parseable), plus a
//                                  final markdown table to stdout.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdalign.h>

// ------------------------------------------------------------------------
// 1. extern-linked public entry points (ra_core.c, unmodified)
// ------------------------------------------------------------------------
extern uint32_t ra_core_orbit(uint32_t key, size_t rng, FILE *raw_stream);
extern uint32_t ra_core_singleblock(uint32_t key, size_t rng, FILE *raw_stream);

// ------------------------------------------------------------------------
// 2. verbatim-copied init routines (static in ra_core.c, 2026-09-04 state,
//    lines ~144-317). Do not edit independently of ra_core.c -- if
//    ra_core.c's init routines change, this copy must be re-synced.
// ------------------------------------------------------------------------
static inline uint32_t rot32_c(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

static inline uint32_t fmix32_c(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6bu;
    h ^= h >> 13;
    h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}

#define GUARD_L_C 0x38916df4u
#define GUARD_M_C 0x6c26fc92u

static void ra_init_state_orbit_c(uint32_t *L, uint32_t *M, uint32_t key) {
    uint32_t keyterm_l = fmix32_c(GUARD_L_C ^ (0x9e3779b7u * key));
    uint32_t keyterm_m = fmix32_c(GUARD_M_C ^ (0x06a0dd9bu * key));
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * 0x9e3779b7u) * keyterm_l;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        L[i] = rot32_c(l_val, r);
        M[i] = rot32_c(m_val, r);
    }
}

static void ra_init_state_singleblock_c(uint32_t *M, uint32_t key, size_t rng) {
    uint32_t keyterm_m = fmix32_c(GUARD_M_C ^ (0x06a0dd9bu * key));
    int len = (rng + 7 > 256) ? 256 : (int)(rng + 7);
    int start = (256 - (int)rng) & 0xFF;
    int end = start + len;

    int first_end = (end <= 256) ? end : 256;
    for (int i = start; i < first_end; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32_c(m_val, r);
    }
    if (end > 256) {
        for (int i = 0; i < end - 256; ++i) {
            uint32_t r = key ^ (uint32_t)i;
            uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
            M[i] = rot32_c(m_val, r);
        }
    }
}

// ------------------------------------------------------------------------
// 3. Philox4x32-10 (corrected PHILOX_M0) and Xoshiro256**/SplitMix64 --
//    byte-for-byte copies of the KAT-verified reference implementations.
// ------------------------------------------------------------------------
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

// ------------------------------------------------------------------------
// timing helpers
// ------------------------------------------------------------------------
static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

// Real per-call total time, reinit-sweep style: many repeated calls, fresh
// key each call, min-of-TRIALS best wall-clock time. Returns ns/word
// (total_time*1e9 / (cycles*k)) -- this IS the "real throughput" number,
// what an actual caller doing this K-reinit workload experiences.
static double bench_real_reinit(uint32_t (*fn)(uint32_t, size_t, FILE *), long long k,
                                 long long cycles_target_words) {
    long long cycles = cycles_target_words / k;
    if (cycles < 20) cycles = 20;
    if (cycles > 300000) cycles = 300000;

    const int TRIALS = 5;
    double best = -1.0;
    volatile uint32_t sink = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        for (long long c = 0; c < cycles; ++c) {
            uint32_t key = (uint32_t)(c * 2654435761u + 1u);
            sink ^= fn(key, (size_t)k, NULL);
        }
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    (void)sink;
    return best * 1e9 / (double)(cycles * k);
}

// Real single-call streaming time (orbit --stream pattern): one call, huge
// N, min-of-TRIALS. Returns ns/word.
static double bench_real_stream(uint32_t (*fn)(uint32_t, size_t, FILE *), long long n) {
    const int TRIALS = 3;
    double best = -1.0;
    volatile uint32_t sink = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        sink ^= fn(0xC0FFEEu, (size_t)n, NULL);
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    (void)sink;
    return best * 1e9 / (double)n;
}

// Init-only isolation: many iterations, varying key, min-of-TRIALS. Returns
// ns/call.
static double bench_init_orbit(long long iterations) {
    const int TRIALS = 7;
    double best = -1.0;
    alignas(64) uint32_t L[256], M[256];
    volatile uint32_t sink = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        for (long long c = 0; c < iterations; ++c) {
            ra_init_state_orbit_c(L, M, (uint32_t)(c * 2654435761u + 1u));
            sink ^= M[0] ^ L[0];
        }
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    (void)sink;
    return best * 1e9 / (double)iterations;
}

static double bench_init_singleblock(long long iterations, size_t rng) {
    const int TRIALS = 7;
    double best = -1.0;
    alignas(64) uint32_t M[256];
    volatile uint32_t sink = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        for (long long c = 0; c < iterations; ++c) {
            ra_init_state_singleblock_c(M, (uint32_t)(c * 2654435761u + 1u), rng);
            sink ^= M[0];
        }
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    (void)sink;
    return best * 1e9 / (double)iterations;
}

// Philox/Xoshiro "seed setup" cost, isolated the same way (their generate
// loop never runs -- rng=0 short-circuits ra_core_philox/ra_core_xoshiro256
// to only the setup lines, since produced<rng is false immediately... but
// both loops use size_t rng, and rng=0 IS a valid immediate-return case for
// both -- confirmed by reading the code above, produced starts at 0 for
// philox (while(0<0) never runs) and the for-loop bound is 0 for xoshiro).
static double bench_init_philox(long long iterations) {
    const int TRIALS = 7;
    double best = -1.0;
    volatile uint32_t sink = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        for (long long c = 0; c < iterations; ++c) {
            sink ^= ra_core_philox((uint32_t)(c * 2654435761u + 1u), 0, NULL);
        }
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    (void)sink;
    return best * 1e9 / (double)iterations;
}

static double bench_init_xoshiro(long long iterations) {
    const int TRIALS = 7;
    double best = -1.0;
    volatile uint32_t sink = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        for (long long c = 0; c < iterations; ++c) {
            sink ^= ra_core_xoshiro256((uint32_t)(c * 2654435761u + 1u), 0, NULL);
        }
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    (void)sink;
    return best * 1e9 / (double)iterations;
}

// ------------------------------------------------------------------------
// main: run everything, print raw lines + a markdown table
// ------------------------------------------------------------------------
static void report_row(const char *label, double init_ns, double gen_ns_per_word,
                        double real_ns_per_word) {
    double real_mbps = (real_ns_per_word > 0)
        ? (1e9 / real_ns_per_word) * 4.0 / (1024.0 * 1024.0)
        : 0.0;
    printf("RAW %-28s init_ns=%.3f gen_ns_per_word=%.3f real_ns_per_word=%.3f real_MBps=%.2f\n",
           label, init_ns, gen_ns_per_word, real_ns_per_word, real_mbps);
}

int main(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "all") != 0) {
        fprintf(stderr, "usage: %s all\n", argv[0]);
        return 1;
    }

    const long long CYCLES_TARGET = 30000000LL; // ~30M words total per K sweep
    const long long STREAM_N = 20000000LL;       // orbit --stream sample size
    const long long INIT_ITERS_HEAVY = 3000000LL; // K=255-ish init isolation
    const long long INIT_ITERS_LIGHT = 20000000LL; // K=1-ish init isolation (cheap loop)

    // -- singleblock K=1 / K=255 --
    double sb1_init = bench_init_singleblock(INIT_ITERS_LIGHT, 1);
    double sb1_real = bench_real_reinit(ra_core_singleblock, 1, CYCLES_TARGET);
    double sb1_gen = sb1_real - sb1_init; // K=1: total_time(1) == init(1) + 1*gen
    report_row("singleblock K=1", sb1_init, sb1_gen, sb1_real);

    double sb255_init = bench_init_singleblock(INIT_ITERS_HEAVY, 255);
    double sb255_real = bench_real_reinit(ra_core_singleblock, 255, CYCLES_TARGET);
    double sb255_gen = (sb255_real * 255.0 - sb255_init) / 255.0;
    report_row("singleblock K=255", sb255_init, sb255_gen, sb255_real);

    // -- orbit multistream K=1 / K=255 (init constant, used for both + stream) --
    double orbit_init = bench_init_orbit(INIT_ITERS_HEAVY);
    double orbit1_real = bench_real_reinit(ra_core_orbit, 1, CYCLES_TARGET);
    double orbit1_gen = orbit1_real - orbit_init;
    report_row("orbit multistream K=1", orbit_init, orbit1_gen, orbit1_real);

    double orbit255_real = bench_real_reinit(ra_core_orbit, 255, CYCLES_TARGET);
    double orbit255_gen = (orbit255_real * 255.0 - orbit_init) / 255.0;
    report_row("orbit multistream K=255", orbit_init, orbit255_gen, orbit255_real);

    // -- orbit --stream (continuous, large N; init amortized to ~0) --
    double orbit_stream_real = bench_real_stream(ra_core_orbit, STREAM_N);
    double orbit_stream_gen = orbit_stream_real - (orbit_init / (double)STREAM_N);
    report_row("orbit --stream", orbit_init, orbit_stream_gen, orbit_stream_real);

    // -- Philox4x32-10 reference, same 5 measurement points --
    double ph_init = bench_init_philox(INIT_ITERS_HEAVY);
    double ph1_real = bench_real_reinit(ra_core_philox, 1, CYCLES_TARGET);
    double ph1_gen = ph1_real - ph_init;
    report_row("philox4x32-10 K=1", ph_init, ph1_gen, ph1_real);

    double ph255_real = bench_real_reinit(ra_core_philox, 255, CYCLES_TARGET);
    double ph255_gen = (ph255_real * 255.0 - ph_init) / 255.0;
    report_row("philox4x32-10 K=255", ph_init, ph255_gen, ph255_real);

    double ph_stream_real = bench_real_stream(ra_core_philox, STREAM_N);
    report_row("philox4x32-10 stream", ph_init, ph_stream_real, ph_stream_real);

    // -- Xoshiro256** reference, same 5 measurement points --
    double xo_init = bench_init_xoshiro(INIT_ITERS_HEAVY);
    double xo1_real = bench_real_reinit(ra_core_xoshiro256, 1, CYCLES_TARGET);
    double xo1_gen = xo1_real - xo_init;
    report_row("xoshiro256** K=1", xo_init, xo1_gen, xo1_real);

    double xo255_real = bench_real_reinit(ra_core_xoshiro256, 255, CYCLES_TARGET);
    double xo255_gen = (xo255_real * 255.0 - xo_init) / 255.0;
    report_row("xoshiro256** K=255", xo_init, xo255_gen, xo255_real);

    double xo_stream_real = bench_real_stream(ra_core_xoshiro256, STREAM_N);
    report_row("xoshiro256** stream", xo_init, xo_stream_real, xo_stream_real);

    return 0;
}
