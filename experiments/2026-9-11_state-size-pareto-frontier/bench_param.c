// bench_param.c -- HANDOVER.md Tahap F (4.5): speed benchmark for the
// parametrized core, per N (compile with -DRA_N=<n> -DRA_RESEED_MODE=<mode>).
// Same harness pattern/methodology as
// ../2026-9-1_family-productionization/bench_release.c (clock_gettime
// CLOCK_MONOTONIC, min-of-TRIALS, init_ns/gen_ns_per_word/real_ns_per_word/
// real_MBps), extended with PCG32 (not previously ported to this harness --
// old benchmarks/comparisons/ used a different clock()+perf-stat
// methodology, not reused here) and RA_N-agnostic since ra_core_param.c is
// #include'd directly (no extern-link/copy-paste of static init routines
// needed, unlike bench_release.c which had to hand-copy ra_core.c's static
// init functions because it links against a nomain .o).
//
// Reports the 5 standard numbers (feedback_orbit_singleblock_benchmark_standard):
// singleblock K=1, singleblock K=N-1, orbit multistream K=1, orbit
// multistream K=N-1, orbit --stream -- plus Philox4x32-10/Xoshiro256**/PCG32
// references, all at the SAME RA_N (baselines don't have a state-size
// parameter, but re-measuring them per N build keeps the comparison
// apples-to-apples against whatever machine load exists at run time).
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h \
//   -DRA_N=<n> -DRA_RESEED_MODE=<mode> bench_param.c -o bench_param_N<n>_<mode> -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdalign.h>

#define RA_CORE_PARAM_NO_MAIN
#include "ra_core_param.c"

// ------------------------------------------------------------------------
// Philox4x32-10 and Xoshiro256**/SplitMix64 -- byte-for-byte copies from
// bench_release.c (KAT-verified reference implementations).
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
// PCG32 (O'Neill, minimal C oneseq/XSH-RR variant -- canonical public-domain
// reference form, pcg32_random_r from pcg-random.org/download.html). Not
// previously ported to this CLOCK_MONOTONIC harness (only an older
// clock()+perf-stat version existed in benchmarks/comparisons/) -- fresh
// port here, straight from the published reference algorithm, not derived
// from that older file.
// ------------------------------------------------------------------------
typedef struct { uint64_t state; uint64_t inc; } pcg32_random_t;

static uint32_t pcg32_random_r(pcg32_random_t *rng) {
    uint64_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32_t rot = (uint32_t)(oldstate >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

static void pcg32_srandom_r(pcg32_random_t *rng, uint64_t initstate, uint64_t initseq) {
    rng->state = 0U;
    rng->inc = (initseq << 1u) | 1u;
    pcg32_random_r(rng);
    rng->state += initstate;
    pcg32_random_r(rng);
}

static uint32_t ra_core_pcg32(uint32_t key, size_t rng, FILE *raw_stream) {
    pcg32_random_t state;
    pcg32_srandom_r(&state, (uint64_t)key, 0xDA3E39CB94B95BDBULL);
    uint32_t last = key;
    for (size_t i = 0; i < rng; i++) {
        last = pcg32_random_r(&state);
        if (raw_stream) fwrite(&last, sizeof(uint32_t), 1, raw_stream);
    }
    return last;
}

// ------------------------------------------------------------------------
// timing helpers -- verbatim methodology from bench_release.c.
// ------------------------------------------------------------------------
static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

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

static double bench_real_reinit4(uint32_t (*fn)(uint32_t, uint32_t, size_t, FILE *),
                                  long long k, long long cycles_target_words) {
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
            sink ^= fn(key, 0u, (size_t)k, NULL);
        }
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    (void)sink;
    return best * 1e9 / (double)(cycles * k);
}

static double bench_real_stream4(uint32_t (*fn)(uint32_t, uint32_t, size_t, FILE *), long long n) {
    const int TRIALS = 3;
    double best = -1.0;
    volatile uint32_t sink = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        sink ^= fn(0xC0FFEEu, 0u, (size_t)n, NULL);
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    (void)sink;
    return best * 1e9 / (double)n;
}

static double bench_init_orbit(long long iterations) {
    const int TRIALS = 7;
    double best = -1.0;
    alignas(64) uint32_t L[RA_N], M[RA_N];
    volatile uint32_t sink = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        for (long long c = 0; c < iterations; ++c) {
            ra_init_state_orbit(L, M, (uint32_t)(c * 2654435761u + 1u), 0u);
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
    alignas(64) uint32_t M[RA_N];
    volatile uint32_t sink = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        for (long long c = 0; c < iterations; ++c) {
            ra_init_state_singleblock(M, (uint32_t)(c * 2654435761u + 1u), 0u, rng);
            sink ^= M[0];
        }
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    (void)sink;
    return best * 1e9 / (double)iterations;
}

static double bench_init_generic3(uint32_t (*fn)(uint32_t, size_t, FILE *), long long iterations) {
    const int TRIALS = 7;
    double best = -1.0;
    volatile uint32_t sink = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        for (long long c = 0; c < iterations; ++c) {
            sink ^= fn((uint32_t)(c * 2654435761u + 1u), 0, NULL);
        }
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    (void)sink;
    return best * 1e9 / (double)iterations;
}

// ------------------------------------------------------------------------
static void report_row(const char *label, double init_ns, double gen_ns_per_word,
                        double real_ns_per_word) {
    double real_mbps = (real_ns_per_word > 0)
        ? (1e9 / real_ns_per_word) * 4.0 / (1024.0 * 1024.0)
        : 0.0;
    printf("RAW %-32s init_ns=%.3f gen_ns_per_word=%.3f real_ns_per_word=%.3f real_MBps=%.2f\n",
           label, init_ns, gen_ns_per_word, real_ns_per_word, real_mbps);
}

int main(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "all") != 0) {
        fprintf(stderr, "usage: %s all\n", argv[0]);
        return 1;
    }

    printf("# RA_N=%d RESEED_MODE=%s\n", RA_N,
           (RA_RESEED_MODE == RESEED_MUTUAL) ? "MUTUAL" : "LOWONLY");

    const long long CYCLES_TARGET = 30000000LL;
    const long long STREAM_N = 20000000LL;
    const long long INIT_ITERS_HEAVY = 3000000LL;
    const long long INIT_ITERS_LIGHT = 20000000LL;
    const long long KMAX = RA_N - 1;

    double sb1_init = bench_init_singleblock(INIT_ITERS_LIGHT, 1);
    double sb1_real = bench_real_reinit4(ra_core_singleblock, 1, CYCLES_TARGET);
    double sb1_gen = sb1_real - sb1_init;
    report_row("singleblock K=1", sb1_init, sb1_gen, sb1_real);

    double sbmax_init = bench_init_singleblock(INIT_ITERS_HEAVY, (size_t)KMAX);
    double sbmax_real = bench_real_reinit4(ra_core_singleblock, KMAX, CYCLES_TARGET);
    double sbmax_gen = (sbmax_real * (double)KMAX - sbmax_init) / (double)KMAX;
    report_row("singleblock K=N-1", sbmax_init, sbmax_gen, sbmax_real);

    double orbit_init = bench_init_orbit(INIT_ITERS_HEAVY);
    double orbit1_real = bench_real_reinit4(ra_core_orbit, 1, CYCLES_TARGET);
    double orbit1_gen = orbit1_real - orbit_init;
    report_row("orbit multistream K=1", orbit_init, orbit1_gen, orbit1_real);

    double orbitmax_real = bench_real_reinit4(ra_core_orbit, KMAX, CYCLES_TARGET);
    double orbitmax_gen = (orbitmax_real * (double)KMAX - orbit_init) / (double)KMAX;
    report_row("orbit multistream K=N-1", orbit_init, orbitmax_gen, orbitmax_real);

    double orbit_stream_real = bench_real_stream4(ra_core_orbit, STREAM_N);
    double orbit_stream_gen = orbit_stream_real - (orbit_init / (double)STREAM_N);
    report_row("orbit --stream", orbit_init, orbit_stream_gen, orbit_stream_real);

    double ph_init = bench_init_generic3(ra_core_philox, INIT_ITERS_HEAVY);
    double ph1_real = bench_real_reinit(ra_core_philox, 1, CYCLES_TARGET);
    report_row("philox4x32-10 K=1", ph_init, ph1_real - ph_init, ph1_real);
    double ph255_real = bench_real_reinit(ra_core_philox, 255, CYCLES_TARGET);
    report_row("philox4x32-10 K=255", ph_init, (ph255_real * 255.0 - ph_init) / 255.0, ph255_real);
    double ph_stream_real = bench_real_stream(ra_core_philox, STREAM_N);
    report_row("philox4x32-10 stream", ph_init, ph_stream_real, ph_stream_real);

    double xo_init = bench_init_generic3(ra_core_xoshiro256, INIT_ITERS_HEAVY);
    double xo1_real = bench_real_reinit(ra_core_xoshiro256, 1, CYCLES_TARGET);
    report_row("xoshiro256** K=1", xo_init, xo1_real - xo_init, xo1_real);
    double xo255_real = bench_real_reinit(ra_core_xoshiro256, 255, CYCLES_TARGET);
    report_row("xoshiro256** K=255", xo_init, (xo255_real * 255.0 - xo_init) / 255.0, xo255_real);
    double xo_stream_real = bench_real_stream(ra_core_xoshiro256, STREAM_N);
    report_row("xoshiro256** stream", xo_init, xo_stream_real, xo_stream_real);

    double pcg_init = bench_init_generic3(ra_core_pcg32, INIT_ITERS_HEAVY);
    double pcg1_real = bench_real_reinit(ra_core_pcg32, 1, CYCLES_TARGET);
    report_row("pcg32 K=1", pcg_init, pcg1_real - pcg_init, pcg1_real);
    double pcg255_real = bench_real_reinit(ra_core_pcg32, 255, CYCLES_TARGET);
    report_row("pcg32 K=255", pcg_init, (pcg255_real * 255.0 - pcg_init) / 255.0, pcg255_real);
    double pcg_stream_real = bench_real_stream(ra_core_pcg32, STREAM_N);
    report_row("pcg32 stream", pcg_init, pcg_stream_real, pcg_stream_real);

    return 0;
}
