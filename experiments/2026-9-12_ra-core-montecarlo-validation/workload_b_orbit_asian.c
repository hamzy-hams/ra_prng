// Workload B -- orbit niche: continuous/addressed, sedikit alamat panjang.
//
// Design locked in HANDOVER.md before this file was written. Task: Monte
// Carlo pricing of an ARITHMETIC-AVERAGE ASIAN CALL via Geometric Brownian
// Motion, N_PATHS=1000 independent long paths (N_STEPS=10000 timesteps
// each), each path derived from ONE address via a single continuous
// generator call (steady-state, no reinit mid-path) -- this is exactly the
// "few addresses, each one runs long" regime ra_core_orbit / Orbit
// Addressing was designed for (see
// experiments/2026-8-30_addressable-init-research/HANDOVER.md:108-115).
// Asian option payoff needs the WHOLE path, not just the final value, so it
// forces genuine continuous-stream usage -- can't be gamed by a generator
// that only looks good on a short burst.
//
// Compared: ra_core_orbit (REAL production function, included directly --
// not a hand copy, since Workload A already validated an isolated copy and
// this workload's correctness-critical payoff numbers should rest on the
// actual production multi-cycle/reseed path, not a re-derivation of it)
// vs philox4x32-10 vs xoshiro256** (both byte-for-byte copies from
// experiments/2026-9-1_family-productionization/bench_release.c, its own
// KAT-verified reference implementations -- same provenance already reused
// in Workload A for xoshiro).
//
// All three generators are read through the SAME fmemopen+FILE* path (one
// open/close per path, not per word) so none gets an artificial I/O
// advantage or penalty -- same fairness principle as Workload A's header
// comment, adapted because orbit's real multi-cycle implementation only
// exposes a FILE*-based interface.
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
// philox4x32-10 -- byte-for-byte copy from
// experiments/2026-9-1_family-productionization/bench_release.c
// ---------------------------------------------------------------------
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

// key selects the stream (Philox's native counter-addressed jump: distinct
// key => independent stream, position within the stream advances via ctr)
// -- this IS Philox's designed "jump to stream #i" mechanism, structurally
// analogous to Orbit Addressing's key-derived stream (both derive an
// independent stream from an address with no reseed cost), so this is the
// fair steady-state-vs-steady-state comparison HANDOVER.md calls for.
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

// ---------------------------------------------------------------------
// xoshiro256** -- byte-for-byte copy, same provenance as Workload A.
// No jump-ahead implementation exists anywhere in this repo (checked via
// grep before writing this file, per HANDOVER.md's explicit "cek dulu,
// jangan asumsikan ada"): the fair steady-state analog here is reseed-from-
// key-then-generate-long-stream, structurally the same pattern Orbit
// Addressing itself uses (derive stream from key, no jump-ahead machinery
// either) -- not a disadvantage unique to xoshiro in this comparison.
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

// ---------------------------------------------------------------------
// GBM / Asian-option simulation parameters (locked before running).
// ---------------------------------------------------------------------
#define N_PATHS   1000
#define N_STEPS   10000   // even, so Box-Muller pairs consume it exactly
#define S0        100.0
#define K_STRIKE  100.0
#define RISK_FREE 0.05
#define SIGMA     0.20
#define MATURITY  1.0

static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

// Consumes N_STEPS raw uniform 32-bit words (Box-Muller pairs, exactly one
// word per timestep on average), simulates one GBM path, returns the
// discounted arithmetic-average-Asian-call payoff for that path.
static double asian_payoff_from_words(const uint32_t *w) {
    const double dt = MATURITY / (double)N_STEPS;
    const double drift = (RISK_FREE - 0.5 * SIGMA * SIGMA) * dt;
    const double vol = SIGMA * sqrt(dt);
    double S = S0;
    double sum = 0.0;
    for (int t = 0; t < N_STEPS; t += 2) {
        double u1 = ((double)w[t] + 0.5) / 4294967296.0;
        double u2 = ((double)w[t + 1] + 0.5) / 4294967296.0;
        double R = sqrt(-2.0 * log(u1));
        double theta = 2.0 * M_PI * u2;
        double z0 = R * cos(theta);
        double z1 = R * sin(theta);
        S *= exp(drift + vol * z0);
        sum += S;
        S *= exp(drift + vol * z1);
        sum += S;
    }
    double avg = sum / (double)N_STEPS;
    double payoff = avg - K_STRIKE;
    if (payoff < 0.0) payoff = 0.0;
    return exp(-RISK_FREE * MATURITY) * payoff;
}

// Pearson correlation between two raw-word arrays of length n (as doubles).
static double pearson_corr(const uint32_t *a, const uint32_t *b, size_t n) {
    double mean_a = 0.0, mean_b = 0.0;
    for (size_t i = 0; i < n; i++) { mean_a += a[i]; mean_b += b[i]; }
    mean_a /= (double)n; mean_b /= (double)n;
    double cov = 0.0, var_a = 0.0, var_b = 0.0;
    for (size_t i = 0; i < n; i++) {
        double da = (double)a[i] - mean_a;
        double db = (double)b[i] - mean_b;
        cov += da * db;
        var_a += da * da;
        var_b += db * db;
    }
    return cov / sqrt(var_a * var_b);
}

typedef struct {
    const char *name;
    uint32_t *words;      // N_PATHS * N_STEPS, retained for independence test
    double payoffs[N_PATHS];
    double mean, se;
    double gen_wall_best;
} gen_result_t;

static void gen_orbit(gen_result_t *r) {
    r->words = malloc((size_t)N_PATHS * N_STEPS * sizeof(uint32_t));
    const uint32_t key_base = 0xC0FFEEu;
    const uint32_t weyl = 0x9e3779b7u;
    const uint32_t addr_hi_fixed = 0u;
    const int TRIALS = 5;
    double best = -1.0;
    for (int trial = 0; trial < TRIALS; trial++) {
        double t0 = now_seconds();
        for (int p = 0; p < N_PATHS; p++) {
            uint32_t key = key_base + (uint32_t)p * weyl;
            uint32_t *buf = &r->words[(size_t)p * N_STEPS];
            FILE *f = fmemopen(buf, (size_t)N_STEPS * sizeof(uint32_t), "wb");
            setvbuf(f, NULL, _IONBF, 0);
            ra_core_orbit(key, addr_hi_fixed, N_STEPS, f);
            fclose(f);
        }
        double dt = now_seconds() - t0;
        if (best < 0 || dt < best) best = dt;
    }
    r->gen_wall_best = best;
    for (int p = 0; p < N_PATHS; p++)
        r->payoffs[p] = asian_payoff_from_words(&r->words[(size_t)p * N_STEPS]);
}

static void gen_philox(gen_result_t *r) {
    r->words = malloc((size_t)N_PATHS * N_STEPS * sizeof(uint32_t));
    const int TRIALS = 5;
    double best = -1.0;
    for (int trial = 0; trial < TRIALS; trial++) {
        double t0 = now_seconds();
        for (int p = 0; p < N_PATHS; p++) {
            uint32_t key = (uint32_t)((uint32_t)p * 2654435761u + 1u);
            uint32_t *buf = &r->words[(size_t)p * N_STEPS];
            FILE *f = fmemopen(buf, (size_t)N_STEPS * sizeof(uint32_t), "wb");
            setvbuf(f, NULL, _IONBF, 0);
            ra_core_philox(key, N_STEPS, f);
            fclose(f);
        }
        double dt = now_seconds() - t0;
        if (best < 0 || dt < best) best = dt;
    }
    r->gen_wall_best = best;
    for (int p = 0; p < N_PATHS; p++)
        r->payoffs[p] = asian_payoff_from_words(&r->words[(size_t)p * N_STEPS]);
}

static void gen_xoshiro(gen_result_t *r) {
    r->words = malloc((size_t)N_PATHS * N_STEPS * sizeof(uint32_t));
    const int TRIALS = 5;
    double best = -1.0;
    for (int trial = 0; trial < TRIALS; trial++) {
        double t0 = now_seconds();
        for (int p = 0; p < N_PATHS; p++) {
            uint32_t key = (uint32_t)((uint32_t)p * 2654435761u + 1u);
            uint32_t *buf = &r->words[(size_t)p * N_STEPS];
            FILE *f = fmemopen(buf, (size_t)N_STEPS * sizeof(uint32_t), "wb");
            setvbuf(f, NULL, _IONBF, 0);
            ra_core_xoshiro256(key, N_STEPS, f);
            fclose(f);
        }
        double dt = now_seconds() - t0;
        if (best < 0 || dt < best) best = dt;
    }
    r->gen_wall_best = best;
    for (int p = 0; p < N_PATHS; p++)
        r->payoffs[p] = asian_payoff_from_words(&r->words[(size_t)p * N_STEPS]);
}

static void compute_stats(gen_result_t *r) {
    double sum = 0.0;
    for (int p = 0; p < N_PATHS; p++) sum += r->payoffs[p];
    r->mean = sum / N_PATHS;
    double var = 0.0;
    for (int p = 0; p < N_PATHS; p++) {
        double d = r->payoffs[p] - r->mean;
        var += d * d;
    }
    var /= (double)(N_PATHS - 1);
    r->se = sqrt(var / (double)N_PATHS);
}

// 50 pseudo-random distinct pairs out of N_PATHS streams, fixed seed (not
// one of the PRNGs under test -- plain LCG, only used to pick which pairs
// to sample, doesn't touch the statistics being validated).
static void run_independence_test(gen_result_t *r) {
    uint64_t lcg = 0x2545F4914F6CDD1DULL;
    int n_pairs = 50;
    double threshold = 1.96 / sqrt((double)N_STEPS);
    int violations = 0;
    double max_abs_corr = 0.0;
    for (int k = 0; k < n_pairs; k++) {
        lcg = lcg * 6364136223846793005ULL + 1442695040888963407ULL;
        int i = (int)((lcg >> 33) % N_PATHS);
        lcg = lcg * 6364136223846793005ULL + 1442695040888963407ULL;
        int j = (int)((lcg >> 33) % N_PATHS);
        if (i == j) { k--; continue; }
        double corr = pearson_corr(&r->words[(size_t)i * N_STEPS],
                                    &r->words[(size_t)j * N_STEPS], N_STEPS);
        double a = fabs(corr);
        if (a > max_abs_corr) max_abs_corr = a;
        if (a > threshold) violations++;
    }
    printf("  Independence (%d pairs, |r| threshold=%.4f): max|r|=%.4f, violations=%d/%d  %s\n",
           n_pairs, threshold, max_abs_corr, violations, n_pairs,
           violations == 0 ? "PASS" : "FLAGGED (see RESULTS.md)");
}

int main(void) {
    printf("Workload B -- Asian option Monte Carlo, N_PATHS=%d, N_STEPS=%d/path (continuous, no reinit mid-path)\n",
           N_PATHS, N_STEPS);
    printf("GBM: S0=%.1f K=%.1f r=%.2f sigma=%.2f T=%.1f\n\n", S0, K_STRIKE, RISK_FREE, SIGMA, MATURITY);

    gen_result_t orbit_r = { .name = "orbit" };
    gen_result_t philox_r = { .name = "philox4x32-10" };
    gen_result_t xoshiro_r = { .name = "xoshiro256**" };

    gen_orbit(&orbit_r);
    gen_philox(&philox_r);
    gen_xoshiro(&xoshiro_r);

    gen_result_t *all[3] = { &orbit_r, &philox_r, &xoshiro_r };
    for (int g = 0; g < 3; g++) {
        compute_stats(all[g]);
        printf("%-16s mean_payoff=%.6f  SE=%.6e  wall=%.4fs (min-of-5, %d paths x %d steps)\n",
               all[g]->name, all[g]->mean, all[g]->se, all[g]->gen_wall_best, N_PATHS, N_STEPS);
        run_independence_test(all[g]);
    }

    printf("\nCross-generator consistency (pairwise |diff| < 3*sqrt(SE_i^2+SE_j^2)):\n");
    bool all_consistent = true;
    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 3; j++) {
            double diff = fabs(all[i]->mean - all[j]->mean);
            double pooled = 3.0 * sqrt(all[i]->se * all[i]->se + all[j]->se * all[j]->se);
            bool ok = diff < pooled;
            if (!ok) all_consistent = false;
            printf("  %-14s vs %-14s: |diff|=%.6e  3*SE_pooled=%.6e  %s\n",
                   all[i]->name, all[j]->name, diff, pooled, ok ? "PASS" : "FAIL");
        }
    }
    printf("\nOverall cross-generator consistency: %s\n", all_consistent ? "PASS" : "FAIL");

    for (int g = 0; g < 3; g++) free(all[g]->words);
    return 0;
}
