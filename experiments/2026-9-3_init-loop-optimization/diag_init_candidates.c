// diag_init_candidates.c
//
// Priority 1 (HANDOVER.md, this folder): test whether the 256-element init
// loop in ra_init_state_orbit/ra_init_state_singleblock
// (../2026-9-1_family-productionization/ra_core.c) can be sped up with
// ZERO output change, portable C only (no intrinsics/inline-asm/arch-
// specific restructuring -- plain C that -O3 -march=native picks up on its
// own).
//
// Self-contained diagnostic: copies (does not modify) rot32/fmix32/
// GUARD_L/GUARD_M/ra_hash/ra_reseed/ra_permutation_cycle_orbit/
// ra_permutation_cycle_singleblock byte-for-byte from
// ../2026-9-1_family-productionization/ra_core.c (lines 86-98, 100-125,
// 153-176, 214-236). ra_core.c itself is NOT touched by this experiment.
// These functions are `static` in ra_core.c, so they cannot be
// extern-linked -- hence the verbatim copy here, same pattern as
// ../2026-9-2_singleblock-o-width-fix/diag_wideo_singleblock.c.
//
// Four cores, selectable via CLI, for same-run A/B (baseline = control,
// byte-for-byte the production init; accum = Priority 1 candidate):
//   - "orbit_baseline"       : ra_init_state_orbit, verbatim.
//   - "orbit_accum"          : same L[i]/M[i] values, computed via a ring-
//                              arithmetic accumulation identity instead of
//                              a per-iteration multiply-by-i (see below).
//   - "singleblock_baseline" : ra_init_state_singleblock, verbatim.
//   - "singleblock_accum"    : accum analog, M[] only.
//
// Candidate math (the "concrete starting hypothesis" from HANDOVER.md):
// in uint32_t arithmetic (the ring Z/2^32Z), for a fixed loop-invariant
// per-call value B=keyterm:
//   (i*A mod 2^32) * B mod 2^32  ==  i*(A*B mod 2^32) mod 2^32
// (ring multiplication is associative/commutative -- an exact identity,
// not an approximation). So step = (A*B) mod 2^32 can be computed ONCE
// outside the loop, and the pre-rotate value accumulated via
// val += step each iteration (starting at 0, i must stay strictly
// increasing 0..255 for the accumulation to track i*step) instead of a
// fresh multiply-by-i and multiply-by-keyterm every time. This is
// bit-identical BY CONSTRUCTION as a matter of ring algebra -- but per
// HANDOVER.md this must still be verified against the actual C
// implementation (integer promotion/cast order subtleties are real, not
// hypothetical) via verify_init_candidates.py BEFORE any speed claim, not
// assumed from the algebra alone. It also does NOT touch rot32(val, r)
// itself, which the assembly diagnostic that motivated this experiment
// identified as the actually expensive part (variable-shift vpsrlvd/
// vpsllvd) -- so a speedup here is not guaranteed even if bit-identical.
//
// All four init functions are marked noinline: purely a diagnostic aid so
// each keeps a separate label in -S assembly output for step 4's
// instruction-count diff. Not present in production, does not change any
// output.
//
// CLI:
//   diag_init_candidates --stream <core> <key> <n>
//       Single call, raw uint32 stream to stdout -- byte-for-byte same
//       format as production ra_core's --stream mode, for direct
//       subprocess diff. <core> in {orbit_baseline,orbit_accum,
//       singleblock_baseline,singleblock_accum}; singleblock cores reject
//       n>255 same as production.
//   diag_init_candidates --bench <core> <cycles_target_words> <K>
//       Reinit-sweep microbench, mirrors
//       ../2026-9-1_family-productionization/bench_ra_core.c's
//       mode_reinit_sweep: cycles = clamp(cycles_target_words/K,10,200000),
//       per-cycle key = c*2654435761u+1u, TRIALS=3 min-of-trials timing,
//       reports ns_per_word.
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h
//          diag_init_candidates.c -o diag_init_candidates -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

// ---- verbatim from ra_core.c L86-98 -----------------------------------
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

// ---- verbatim from ra_core.c L100-105 ----------------------------------
#define GUARD_L 0x38916df4u
#define GUARD_M 0x6c26fc92u

// ---- verbatim from ra_core.c L107-125 ----------------------------------
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

// ---- baseline init (verbatim copy of ra_core.c L139-149, L205-212) -----
__attribute__((noinline))
static void ra_init_state_orbit_baseline(uint32_t *L, uint32_t *M, uint32_t key) {
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

__attribute__((noinline))
static void ra_init_state_singleblock_baseline(uint32_t *M, uint32_t key) {
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32(m_val, r);
    }
}

// ---- old-era init (verbatim copy of winner_wired_addressable.c L52-60,
// pre-2026-09-01 "Kandidat 5 affine-in-key" formula: no fmix32, no
// GUARD_L/GUARD_M weak-key fix, no multiply-combined keyterm. Added
// 2026-09-03 to answer "is fmix32+GUARD really ~100x expensive" -- run in
// this SAME harness/machine so the comparison isn't confounded by the old
// 2026-08-31 benchmark's own methodology/noise. Diagnostic only, NOT
// applied anywhere; this formula is the one already known to have the
// K-small structural defect, see project_singleblock_kmin_defect memory --
// never a candidate for re-adoption, timing-only reference point. -----
__attribute__((noinline))
static void ra_init_state_singleblock_oldera(uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
        M[i] = rot32(m_val, r);
    }
}

// ---- Priority 1 candidate: multiply-chain -> accumulate (see header) ---
__attribute__((noinline))
static void ra_init_state_orbit_accum(uint32_t *L, uint32_t *M, uint32_t key) {
    uint32_t keyterm_l = fmix32(GUARD_L ^ (0x9e3779b7u * key));
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    uint32_t step_l = 0x9e3779b7u * keyterm_l; // (A*B) mod 2^32, once
    uint32_t step_m = 0x06a0dd9bu * keyterm_m;
    uint32_t l_val = 0, m_val = 0;             // l_val == i*step_l mod 2^32
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
        l_val += step_l;
        m_val += step_m;
    }
}

__attribute__((noinline))
static void ra_init_state_singleblock_accum(uint32_t *M, uint32_t key) {
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    uint32_t step_m = 0x06a0dd9bu * keyterm_m;
    uint32_t m_val = 0;
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        M[i] = rot32(m_val, r);
        m_val += step_m;
    }
}

// ---- verbatim from ra_core.c L153-176, L214-236 -------------------------
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

        uint32_t tmp = L[i];
        L[i] = L[d];
        L[d] = tmp;
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

// ---- core wrappers, one pair per init variant ---------------------------
static uint32_t ra_core_orbit_baseline(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;
    uint32_t cons = key;
    ra_init_state_orbit_baseline(L, M, key);
    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_orbit(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

static uint32_t ra_core_orbit_accum(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;
    uint32_t cons = key;
    ra_init_state_orbit_accum(L, M, key);
    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_orbit(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

static uint32_t ra_core_singleblock_baseline(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock_baseline(M, key);
    ra_permutation_cycle_singleblock(cons, 0, M, &count, raw_stream);
    return cons;
}

static uint32_t ra_core_singleblock_accum(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock_accum(M, key);
    ra_permutation_cycle_singleblock(cons, 0, M, &count, raw_stream);
    return cons;
}

static uint32_t ra_core_singleblock_oldera(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock_oldera(M, key);
    ra_permutation_cycle_singleblock(cons, 0, M, &count, raw_stream);
    return cons;
}

// ---- Philox4x32-10 and xoshiro256** reference cores (2026-09-03, user
// request: compare current production ra_core_singleblock/orbit against
// Philox and Xoshiro at K=1/K=255 in the SAME harness/machine as the
// oldera-vs-current speed investigation above). Byte-for-byte copies of
// ../2026-8-31_prng-family-benchmark/benchmark_all.c's #6/#7 (KAT-verified
// there against Random123's official Philox4x32-10 test vectors and this
// repo's own xoshiro256** reference seed -- PHILOX_M0=0xD2511F53U is the
// CORRECTED multiplier; do NOT copy from benchmarks/comparisons/, those
// still carry the Philox2x32-constant bug documented in that file's header).
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

// ---- CLI dispatch, CORES[]/find_core pattern from ra_core.c -------------
typedef uint32_t (*core_fn_t)(uint32_t key, size_t rng, FILE *raw_stream);
typedef struct { const char *name; core_fn_t fn; long max_rng; } core_entry_t;

static const core_entry_t CORES[] = {
    { "orbit_baseline",       ra_core_orbit_baseline,       -1 },
    { "orbit_accum",          ra_core_orbit_accum,          -1 },
    { "singleblock_baseline", ra_core_singleblock_baseline, 255 },
    { "singleblock_accum",    ra_core_singleblock_accum,    255 },
    { "singleblock_oldera",   ra_core_singleblock_oldera,   255 },
    { "philox",               ra_core_philox,               -1 },
    { "xoshiro256",           ra_core_xoshiro256,           -1 },
};

static const core_entry_t *find_core(const char *name) {
    for (size_t i = 0; i < sizeof(CORES) / sizeof(CORES[0]); ++i) {
        if (strcmp(CORES[i].name, name) == 0) return &CORES[i];
    }
    fprintf(stderr, "Unknown core '%s'. Known: ", name);
    for (size_t i = 0; i < sizeof(CORES) / sizeof(CORES[0]); ++i) {
        fprintf(stderr, "%s ", CORES[i].name);
    }
    fprintf(stderr, "\n");
    exit(1);
}

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

int main(int argc, char **argv) {
    if (argc >= 5 && strcmp(argv[1], "--stream") == 0) {
        const core_entry_t *core = find_core(argv[2]);
        uint32_t key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t rng = (size_t)strtoull(argv[4], NULL, 0);

        if (core->max_rng >= 0 && (long)rng > core->max_rng) {
            fprintf(stderr, "--stream: n=%zu exceeds core '%s' max_rng=%ld.\n",
                    rng, core->name, core->max_rng);
            return 1;
        }

        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        uint32_t last_cons = core->fn(key, rng, stdout);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

        fprintf(stderr, "Streamed %zu pseudorandom outputs in %.3f seconds\n", rng, elapsed);
        fprintf(stderr, "Last cons from RNGing: %u\n", last_cons);
        return 0;
    }

    if (argc >= 5 && strcmp(argv[1], "--bench") == 0) {
        const core_entry_t *core = find_core(argv[2]);
        long long cycles_target_words = atoll(argv[3]);
        long long k = atoll(argv[4]);
        if (k <= 0) { fprintf(stderr, "K must be > 0\n"); return 1; }
        if (core->max_rng >= 0 && k > core->max_rng) {
            fprintf(stderr, "--bench: K=%lld exceeds core '%s' max_rng=%ld.\n",
                    k, core->name, core->max_rng);
            return 1;
        }

        long long cycles = cycles_target_words / k;
        if (cycles < 10) cycles = 10;
        if (cycles > 200000) cycles = 200000;

        const int TRIALS = 3;
        double best = -1.0;
        volatile uint32_t sink = 0;
        for (int trial = 0; trial < TRIALS; ++trial) {
            double t0 = now_seconds();
            for (long long c = 0; c < cycles; ++c) {
                uint32_t key = (uint32_t)(c * 2654435761u + 1u);
                sink ^= core->fn(key, (size_t)k, NULL);
            }
            double t1 = now_seconds();
            double dt = t1 - t0;
            if (best < 0 || dt < best) best = dt;
        }

        double ns_per_word = best * 1e9 / (double)(cycles * k);
        fprintf(stderr, "sink=%u\n", sink);
        printf("%s K %lld cycles %lld ns_per_word %.4f\n",
               core->name, k, cycles, ns_per_word);
        return 0;
    }

    // --bench-init: isolate the init-loop-only cost (no permutation cycle,
    // no wrapper call), to tell apart "the loop itself got slower" from
    // "something about the surrounding call sequence changed" when
    // diagnosing the K=255 singleblock_accum regression seen in --bench.
    if (argc >= 4 && strcmp(argv[1], "--bench-init") == 0) {
        long long iterations = atoll(argv[3]);
        if (iterations <= 0) { fprintf(stderr, "iterations must be > 0\n"); return 1; }

        const int TRIALS = 5;
        double best = -1.0;
        alignas(64) uint32_t L[256], M[256];
        volatile uint32_t sink = 0;

        if (strcmp(argv[2], "orbit_baseline") == 0) {
            for (int trial = 0; trial < TRIALS; ++trial) {
                double t0 = now_seconds();
                for (long long c = 0; c < iterations; ++c) {
                    ra_init_state_orbit_baseline(L, M, (uint32_t)(c * 2654435761u + 1u));
                    sink ^= M[0] ^ L[0];
                }
                double t1 = now_seconds();
                if (best < 0 || t1 - t0 < best) best = t1 - t0;
            }
        } else if (strcmp(argv[2], "orbit_accum") == 0) {
            for (int trial = 0; trial < TRIALS; ++trial) {
                double t0 = now_seconds();
                for (long long c = 0; c < iterations; ++c) {
                    ra_init_state_orbit_accum(L, M, (uint32_t)(c * 2654435761u + 1u));
                    sink ^= M[0] ^ L[0];
                }
                double t1 = now_seconds();
                if (best < 0 || t1 - t0 < best) best = t1 - t0;
            }
        } else if (strcmp(argv[2], "singleblock_baseline") == 0) {
            for (int trial = 0; trial < TRIALS; ++trial) {
                double t0 = now_seconds();
                for (long long c = 0; c < iterations; ++c) {
                    ra_init_state_singleblock_baseline(M, (uint32_t)(c * 2654435761u + 1u));
                    sink ^= M[0];
                }
                double t1 = now_seconds();
                if (best < 0 || t1 - t0 < best) best = t1 - t0;
            }
        } else if (strcmp(argv[2], "singleblock_accum") == 0) {
            for (int trial = 0; trial < TRIALS; ++trial) {
                double t0 = now_seconds();
                for (long long c = 0; c < iterations; ++c) {
                    ra_init_state_singleblock_accum(M, (uint32_t)(c * 2654435761u + 1u));
                    sink ^= M[0];
                }
                double t1 = now_seconds();
                if (best < 0 || t1 - t0 < best) best = t1 - t0;
            }
        } else if (strcmp(argv[2], "singleblock_oldera") == 0) {
            for (int trial = 0; trial < TRIALS; ++trial) {
                double t0 = now_seconds();
                for (long long c = 0; c < iterations; ++c) {
                    ra_init_state_singleblock_oldera(M, (uint32_t)(c * 2654435761u + 1u));
                    sink ^= M[0];
                }
                double t1 = now_seconds();
                if (best < 0 || t1 - t0 < best) best = t1 - t0;
            }
        } else {
            fprintf(stderr, "unknown core '%s' for --bench-init\n", argv[2]);
            return 1;
        }

        double ns_per_call = best * 1e9 / (double)iterations;
        double ns_per_elem = ns_per_call / 256.0;
        fprintf(stderr, "sink=%u\n", sink);
        printf("%s iterations %lld ns_per_call %.4f ns_per_elem(/256) %.4f\n",
               argv[2], iterations, ns_per_call, ns_per_elem);
        return 0;
    }

    fprintf(stderr,
        "usage:\n"
        "  %s --stream <core> <key> <n>\n"
        "  %s --bench <core> <cycles_target_words> <K>\n"
        "  %s --bench-init <core> <iterations>\n"
        "cores: orbit_baseline orbit_accum singleblock_baseline singleblock_accum\n",
        argv[0], argv[0], argv[0]);
    return 1;
}
