// Workload A -- singleblock niche: bulk/reinit-per-batch Monte Carlo.
//
// Design locked in HANDOVER.md before this file was written. Task: Monte
// Carlo estimation of pi via N independent trials, each reinitialized with
// a fresh key, each generating EXACTLY 255 words (singleblock's validated
// K=255 sweet spot). Compares ra_core_singleblock vs xoshiro256** vs PCG32
// under the identical reinit-per-trial usage pattern (this IS all three
// generators' shared niche -- not the mismatched comparison flagged in
// BENCHMARK_METHODOLOGY_FINDINGS.md, which was specifically about
// orbit -- orbit is not involved in this workload at all).
//
// Fidelity: sb_init_state/sb_cycle_buf below are byte-for-byte copies of
// src/ra_core/c/ra_core.c's ra_init_state_singleblock/
// ra_permutation_cycle_singleblock (reusing that file's own rot32/fmix32/
// GUARD_M/GUARD_M2 via the #include below, so there is zero chance of the
// copy drifting from the constants). Buffer output instead of FILE*/
// fwrite is used for ALL THREE generators equally, so no generator gets
// an artificial I/O-setup advantage or penalty -- matching the project's
// own established pattern in bench_release.c (which does the same
// "verbatim copy for isolation" for orbit/singleblock and never uses
// FILE* for Philox/Xoshiro either). Verified via run_parity_check()
// against the real ra_core_singleblock through a memory-backed FILE*
// before the timed run.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define RA_CORE_NO_MAIN
#include "/home/zep/Documents/research/ra_prng/src/ra_core/c/ra_core.c"

// ---------------------------------------------------------------------
// ra_core_singleblock recurrence -- byte-for-byte copy of
// ra_init_state_singleblock/ra_permutation_cycle_singleblock above,
// buffer-output variant instead of FILE*/fwrite.
// ---------------------------------------------------------------------
static inline __attribute__((always_inline)) void sb_init_state(uint32_t *M, uint32_t key, uint32_t addr_hi, size_t rng) {
    uint32_t keyterm_m_key   = fmix32(GUARD_M  ^ (0x06a0dd9bu * key));
    uint32_t keyterm_m_param = fmix32(GUARD_M2 ^ (0x9e3779b1u * addr_hi));
    uint32_t keyterm_m = keyterm_m_key * keyterm_m_param;
    int len = (rng + 7 > 256) ? 256 : (int)(rng + 7);
    int start = (256 - (int)rng) & 0xFF;
    int end = start + len;

    int first_end = (end <= 256) ? end : 256;
    for (int i = start; i < first_end; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32(m_val, r);
    }
    if (end > 256) {
        for (int i = 0; i < end - 256; ++i) {
            uint32_t r = key ^ (uint32_t)i;
            uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
            M[i] = rot32(m_val, r);
        }
    }
}

// Buffer-output variant: writes to out[] instead of fwrite. `count` is the
// number of words wanted (<=255 for this workload, always exactly 255).
static inline __attribute__((always_inline)) void sb_cycle_buf(uint32_t cons, size_t it, const uint32_t *M,
                                 uint64_t count, uint32_t *out) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    uint32_t o = (M[(uint8_t)(255+0)]<<0)^(M[(uint8_t)(255+1)]<<1)^(M[(uint8_t)(255+2)]<<2)^
                 (M[(uint8_t)(255+3)]<<3)^(M[(uint8_t)(255+4)]<<4)^(M[(uint8_t)(255+5)]<<5)^
                 (M[(uint8_t)(255+6)]<<6)^(M[(uint8_t)(255+7)]<<7);
    uint32_t o_term7 = M[(uint8_t)(255+7)] << 7;
    size_t produced = 0;
    for (uint32_t i = 255; i > 0; --i) {
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 17u;
        out[produced++] = c;
        d = c & 0xFFu;
        if (count <= 1) break;
        --count;
        if (i > 1) {
            uint32_t new_term = M[(uint8_t)(i-1)];
            uint32_t o_next = new_term ^ ((o ^ o_term7) << 1);
            o_term7 = M[(uint8_t)(i+6)] << 7;
            o = o_next;
        }
    }
}

static void ra_core_singleblock_into(uint32_t key, size_t rng, uint32_t *out) {
    uint32_t M[256];
    sb_init_state(M, key, 0u, rng);
    sb_cycle_buf(key, 0, M, (uint64_t)rng, out);
}

// ---------------------------------------------------------------------
// xoshiro256** -- byte-for-byte copy from
// experiments/2026-9-1_family-productionization/bench_release.c
// ---------------------------------------------------------------------
static inline uint64_t xoshiro_rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}
static uint64_t splitmix64_step(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}
static void xoshiro256_into(uint32_t key, size_t rng, uint32_t *out) {
    uint64_t sm_state = (uint64_t)key;
    uint64_t s[4];
    s[0] = splitmix64_step(&sm_state);
    s[1] = splitmix64_step(&sm_state);
    s[2] = splitmix64_step(&sm_state);
    s[3] = splitmix64_step(&sm_state);
    for (size_t i = 0; i < rng; i++) {
        const uint64_t result = xoshiro_rotl(s[1] * 5, 7) * 9;
        const uint64_t t = s[1] << 17;
        s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3];
        s[2] ^= t; s[3] = xoshiro_rotl(s[3], 45);
        out[i] = (uint32_t)result;
    }
}

// ---------------------------------------------------------------------
// PCG32 -- byte-for-byte copy from benchmarks/comparisons/source/pcg.c
// (canonical O'Neill PCG-XSH-RR reference algorithm).
// ---------------------------------------------------------------------
typedef struct { uint64_t state; uint64_t inc; } pcg32_random_t;
static void pcg32_srandom(pcg32_random_t *rng, uint64_t initstate, uint64_t initseq) {
    rng->state = 0U;
    rng->inc   = (initseq << 1u) | 1u;
    rng->state = rng->state * 6364136223846793005ULL + rng->inc;
    rng->state += initstate;
    rng->state = rng->state * 6364136223846793005ULL + rng->inc;
}
static uint32_t pcg32_random_r(pcg32_random_t *rng) {
    uint64_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
static void pcg32_into(uint32_t key, size_t rng, uint32_t *out) {
    pcg32_random_t r;
    pcg32_srandom(&r, (uint64_t)key, 54u);
    for (size_t i = 0; i < rng; i++) out[i] = pcg32_random_r(&r);
}

// ---------------------------------------------------------------------
// Fidelity check: sb_cycle_buf/sb_init_state must match the REAL
// ra_core_singleblock exactly.
// ---------------------------------------------------------------------
static bool run_parity_check(void) {
    uint32_t keys_to_test[] = {0u, 1u, 42u, 0xC0FFEEu, 0xFFFFFFFFu, 1000000u};
    uint32_t ref[255], buf[1024];
    for (size_t k = 0; k < sizeof(keys_to_test)/sizeof(keys_to_test[0]); ++k) {
        uint32_t key = keys_to_test[k];
        FILE *f = fmemopen(buf, sizeof(buf), "wb");
        setvbuf(f, NULL, _IONBF, 0);
        ra_core_singleblock(key, 0u, 255, f);
        fclose(f);
        memcpy(ref, buf, sizeof(ref));
        uint32_t mine[255];
        ra_core_singleblock_into(key, 255, mine);
        if (memcmp(ref, mine, sizeof(ref)) != 0) {
            fprintf(stderr, "PARITY FAIL for key=%u\n", key);
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
// Monte Carlo pi estimation harness.
// ---------------------------------------------------------------------
static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

typedef void (*gen_into_fn)(uint32_t key, size_t rng, uint32_t *out);

// N trials, each generates 255 words, uses first 254 (127 (x,y) pairs) for
// pi estimation via unit-circle hit test. Word #255 is discarded on
// purpose -- keeps every trial hitting exactly K=255, singleblock's
// validated sweet spot, rather than K=254.
static double run_pi_estimation(gen_into_fn fn, long long n_trials, uint64_t *out_inside, uint64_t *out_total_points) {
    uint64_t inside = 0;
    uint32_t buf[255];
    for (long long t = 0; t < n_trials; ++t) {
        uint32_t key = (uint32_t)(t * 2654435761u + 1u);
        fn(key, 255, buf);
        for (int p = 0; p < 127; ++p) {
            uint32_t wx = buf[p * 2];
            uint32_t wy = buf[p * 2 + 1];
            double x = ((double)wx / 4294967295.0) * 2.0 - 1.0;
            double y = ((double)wy / 4294967295.0) * 2.0 - 1.0;
            if (x * x + y * y <= 1.0) inside++;
        }
    }
    *out_inside = inside;
    *out_total_points = (uint64_t)n_trials * 127ULL;
    return 4.0 * (double)inside / (double)(*out_total_points);
}

static void bench_and_report(const char *name, gen_into_fn fn, long long n_trials) {
    const int TRIALS = 5;
    double best = -1.0;
    uint64_t inside = 0, total_points = 0;
    double pi_est = 0.0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        pi_est = run_pi_estimation(fn, n_trials, &inside, &total_points);
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    double true_pi = M_PI;
    double se = sqrt(true_pi * (4.0 - true_pi) / (double)total_points);
    double err = fabs(pi_est - true_pi);
    bool pass = err < 4.0 * se;
    printf("%-16s pi_est=%.6f  |err|=%.6f  SE=%.6e  4*SE=%.6e  %s  wall=%.4fs (min-of-%d, %lld trials x 255 words)\n",
           name, pi_est, err, se, 4.0*se, pass ? "PASS" : "FAIL", best, TRIALS, n_trials);
}

int main(void) {
    if (!run_parity_check()) {
        fprintf(stderr, "FATAL: sb_cycle_buf does not match production ra_core_singleblock. Aborting.\n");
        return 1;
    }
    printf("Parity check vs production ra_core_singleblock: OK\n\n");

    long long N = 1000000LL;
    printf("Workload A -- Monte Carlo pi estimation, N=%lld trials, K=255 words/trial, reinit per trial\n", N);
    printf("True pi = %.6f\n\n", M_PI);

    bench_and_report("singleblock", ra_core_singleblock_into, N);
    bench_and_report("xoshiro256**", xoshiro256_into, N);
    bench_and_report("pcg32", pcg32_into, N);

    return 0;
}
