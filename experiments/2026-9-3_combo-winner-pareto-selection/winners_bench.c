// winners_bench.c
// Axis A speed harness (HANDOVER.md sect 2) for the 11 fully-clean
// singleblock cycle-op combo winners
// (../2026-9-2_singleblock-cycle-combo-search/RESULTS.md) plus the
// unmodified production ra_core_singleblock baseline.
//
// Ported/copied verbatim from:
//   ../2026-8-31_prng-family-benchmark/benchmark_all.c
//     -> core_fn_t / core_entry_t / now_seconds() / mode_reinit_sweep()
//        (byte-for-byte, only generalized to loop the local CORES[] table)
//   ../2026-9-2_singleblock-cycle-combo-search/candidates/w8_f*_i0.c
//     -> rot32/fmix32/GUARD_M/ra_init_state_singleblock (identical across
//        all 11 winner files) and each winner's per-round finalizer body
//   ../2026-9-1_family-productionization/ra_core.c L86-98,205-260
//     -> the BASELINE row (ra_init_state_singleblock/
//        ra_permutation_cycle_singleblock/ra_core_singleblock), copied
//        from ra_core.c directly rather than from benchmark_all.c's
//        `ra_init_state_full`/`ra_core_singleblock`.
//
// DEVIATION FROM HANDOVER.md sect 7 (documented, see RESULTS.md "Deviations
// from HANDOVER"): the handover says to copy the baseline from
// benchmark_all.c. That file is dated 2026-8-31 and predates the
// 2026-9-1_keyzero-guard-fix experiment, which changed
// ra_init_state_singleblock's formula (additive `i*C + C*key` ->
// `fmix32(GUARD_M ^ (C*key))` keyterm multiply). benchmark_all.c's
// `ra_init_state_full` still has the OLD formula, so copying it here would
// silently benchmark a stale/wrong "baseline" for the before/after
// comparison HANDOVER sect 5 point 3 asks for. Using ra_core.c's actual
// current baseline instead keeps the comparison meaningful; the harness
// ported from benchmark_all.c (core_fn_t/mode_reinit_sweep/now_seconds) is
// unaffected by this and is still copied verbatim as instructed.
//
// ra_core.c is NOT modified. None of the 11 candidates/*.c files are
// modified -- their transform bodies are copied byte-for-byte below,
// renamed only to avoid symbol collision in this single translation unit.
//
// Compile:
//   gcc -O3 -march=native -std=gnu17 -include stdalign.h winners_bench.c \
//       -o winners_bench -Wall -Wextra

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

static inline uint32_t fmix32(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6bu;
    h ^= h >> 13;
    h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}

#define GUARD_M 0x6c26fc92u

// Shared by all 12 entries -- identical in every candidates/*.c file and in
// ra_core.c's own ra_init_state_singleblock (L205-212).
static void ra_init_state_singleblock(uint32_t *M, uint32_t key) {
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32(m_val, r);
    }
}

typedef uint32_t (*core_fn_t)(uint32_t key, size_t rng, FILE *raw_stream);
typedef struct { const char *name; core_fn_t fn; long max_rng; } core_entry_t;

// Wide-o tap (width=8), identical across all 11 winners -- extracted once
// since it's a verbatim-identical subexpression in every candidates/*.c
// file, not an algorithmic reinterpretation.
#define WIDE_O(M, i) \
    ((M[(uint8_t)((i) + 0)] << 0) ^ (M[(uint8_t)((i) + 1)] << 1) ^ \
     (M[(uint8_t)((i) + 2)] << 2) ^ (M[(uint8_t)((i) + 3)] << 3) ^ \
     (M[(uint8_t)((i) + 4)] << 4) ^ (M[(uint8_t)((i) + 5)] << 5) ^ \
     (M[(uint8_t)((i) + 6)] << 6) ^ (M[(uint8_t)((i) + 7)] << 7))

// ---------------------------------------------------------------------
// baseline -- unmodified production ra_core_singleblock. Verbatim from
// ../2026-9-1_family-productionization/ra_core.c L214-260 (tap width=2,
// no finalizer). See file header for why this is sourced from ra_core.c
// rather than benchmark_all.c.
// ---------------------------------------------------------------------

static void ra_permutation_cycle_baseline(uint32_t cons, size_t it,
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

static uint32_t ra_core_baseline(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_baseline(cons, 0, M, &count, raw_stream);
    return cons;
}

// ---------------------------------------------------------------------
// 11 combo-search winners. Each pair below is a byte-for-byte copy of the
// corresponding candidates/w8_f*_i0.c's ra_permutation_cycle_singleblock_combo
// / ra_core_singleblock_combo, renamed to avoid symbol collision. Only the
// finalizer applied to `c` after rot32 differs per candidate -- see each
// comment for the exact recipe (matches ../2026-9-2_.../RESULTS.md's table).
// ---------------------------------------------------------------------

// w8_f8_i0: XORSHIFT(15)
static void ra_permutation_cycle_w8_f8_i0(uint32_t cons, size_t it, const uint32_t *M, uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = WIDE_O(M, i);
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 15u;
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) break;
        --(*count);
    }
}
static uint32_t ra_core_w8_f8_i0(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_w8_f8_i0(cons, 0, M, &count, raw_stream);
    return cons;
}

// w8_f9_i0: XORSHIFT(16)
static void ra_permutation_cycle_w8_f9_i0(uint32_t cons, size_t it, const uint32_t *M, uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = WIDE_O(M, i);
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 16u;
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) break;
        --(*count);
    }
}
static uint32_t ra_core_w8_f9_i0(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_w8_f9_i0(cons, 0, M, &count, raw_stream);
    return cons;
}

// w8_f10_i0: XORSHIFT(17)
static void ra_permutation_cycle_w8_f10_i0(uint32_t cons, size_t it, const uint32_t *M, uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = WIDE_O(M, i);
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 17u;
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) break;
        --(*count);
    }
}
static uint32_t ra_core_w8_f10_i0(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_w8_f10_i0(cons, 0, M, &count, raw_stream);
    return cons;
}

// w8_f22_i0: XORSHIFT(16)->MUL(0x85ebca6b)
static void ra_permutation_cycle_w8_f22_i0(uint32_t cons, size_t it, const uint32_t *M, uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = WIDE_O(M, i);
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 16u;
        c = (uint32_t)(c * 0x85ebca6bu);
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) break;
        --(*count);
    }
}
static uint32_t ra_core_w8_f22_i0(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_w8_f22_i0(cons, 0, M, &count, raw_stream);
    return cons;
}

// w8_f24_i0: XORSHIFT(16)->MUL(0x9e3779b7)
static void ra_permutation_cycle_w8_f24_i0(uint32_t cons, size_t it, const uint32_t *M, uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = WIDE_O(M, i);
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 16u;
        c = (uint32_t)(c * 0x9e3779b7u);
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) break;
        --(*count);
    }
}
static uint32_t ra_core_w8_f24_i0(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_w8_f24_i0(cons, 0, M, &count, raw_stream);
    return cons;
}

// w8_f25_i0: XORSHIFT(16)->MUL(0x06a0dd9b)
static void ra_permutation_cycle_w8_f25_i0(uint32_t cons, size_t it, const uint32_t *M, uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = WIDE_O(M, i);
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 16u;
        c = (uint32_t)(c * 0x06a0dd9bu);
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) break;
        --(*count);
    }
}
static uint32_t ra_core_w8_f25_i0(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_w8_f25_i0(cons, 0, M, &count, raw_stream);
    return cons;
}

// w8_f26_i0: MUL(0x85ebca6b)->XORSHIFT(13)
static void ra_permutation_cycle_w8_f26_i0(uint32_t cons, size_t it, const uint32_t *M, uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = WIDE_O(M, i);
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c = (uint32_t)(c * 0x85ebca6bu);
        c ^= c >> 13u;
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) break;
        --(*count);
    }
}
static uint32_t ra_core_w8_f26_i0(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_w8_f26_i0(cons, 0, M, &count, raw_stream);
    return cons;
}

// w8_f27_i0: MUL(0x85ebca6b)->XORSHIFT(16)
static void ra_permutation_cycle_w8_f27_i0(uint32_t cons, size_t it, const uint32_t *M, uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = WIDE_O(M, i);
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c = (uint32_t)(c * 0x85ebca6bu);
        c ^= c >> 16u;
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) break;
        --(*count);
    }
}
static uint32_t ra_core_w8_f27_i0(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_w8_f27_i0(cons, 0, M, &count, raw_stream);
    return cons;
}

// w8_f28_i0: MUL(0xc2b2ae35)->XORSHIFT(13)
static void ra_permutation_cycle_w8_f28_i0(uint32_t cons, size_t it, const uint32_t *M, uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = WIDE_O(M, i);
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c = (uint32_t)(c * 0xc2b2ae35u);
        c ^= c >> 13u;
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) break;
        --(*count);
    }
}
static uint32_t ra_core_w8_f28_i0(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_w8_f28_i0(cons, 0, M, &count, raw_stream);
    return cons;
}

// w8_f29_i0: MUL(0xc2b2ae35)->XORSHIFT(16)
static void ra_permutation_cycle_w8_f29_i0(uint32_t cons, size_t it, const uint32_t *M, uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = WIDE_O(M, i);
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c = (uint32_t)(c * 0xc2b2ae35u);
        c ^= c >> 16u;
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) break;
        --(*count);
    }
}
static uint32_t ra_core_w8_f29_i0(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_w8_f29_i0(cons, 0, M, &count, raw_stream);
    return cons;
}

// w8_f33_i0: ADD(b)->ROT(cons)
static void ra_permutation_cycle_w8_f33_i0(uint32_t cons, size_t it, const uint32_t *M, uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = WIDE_O(M, i);
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c = (uint32_t)(c + b);
        c = rot32(c, cons);
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) break;
        --(*count);
    }
}
static uint32_t ra_core_w8_f33_i0(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_w8_f33_i0(cons, 0, M, &count, raw_stream);
    return cons;
}

// ---------------------------------------------------------------------
// CORES[] -- 12 entries (11 combo winners + 1 baseline). All capped at
// rng<=255 (singleblock no-reseed fast path).
// ---------------------------------------------------------------------

static const core_entry_t CORES[] = {
    { "singleblock",  ra_core_baseline,  255 },
    { "w8_f8_i0",     ra_core_w8_f8_i0,  255 },
    { "w8_f9_i0",     ra_core_w8_f9_i0,  255 },
    { "w8_f10_i0",    ra_core_w8_f10_i0, 255 },
    { "w8_f22_i0",    ra_core_w8_f22_i0, 255 },
    { "w8_f24_i0",    ra_core_w8_f24_i0, 255 },
    { "w8_f25_i0",    ra_core_w8_f25_i0, 255 },
    { "w8_f26_i0",    ra_core_w8_f26_i0, 255 },
    { "w8_f27_i0",    ra_core_w8_f27_i0, 255 },
    { "w8_f28_i0",    ra_core_w8_f28_i0, 255 },
    { "w8_f29_i0",    ra_core_w8_f29_i0, 255 },
    { "w8_f33_i0",    ra_core_w8_f33_i0, 255 },
};
#define NUM_CORES (sizeof(CORES) / sizeof(CORES[0]))

static const core_entry_t *find_core(const char *name) {
    for (size_t i = 0; i < NUM_CORES; ++i) if (strcmp(CORES[i].name, name) == 0) return &CORES[i];
    fprintf(stderr, "Unknown core '%s'. Known:", name);
    for (size_t i = 0; i < NUM_CORES; ++i) fprintf(stderr, " %s", CORES[i].name);
    fprintf(stderr, "\n");
    exit(1);
}

// ---------------------------------------------------------------------
// reinit-sweep: verbatim port of benchmark_all.c's mode_reinit_sweep(),
// generalized over the local CORES[] table above.
// ---------------------------------------------------------------------

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

// ---------------------------------------------------------------------
// --multistream: continuous-stream mode for the Axis A MB/s metric
// (HANDOVER.md sect 2). Chunked multikey pattern -- Weyl key increment
// between blocks of K words each -- identical to the pattern already in
// every candidates/*.c file's own --stream mode (block_idx*0x9E3779B9u),
// generalized here over CORES[] so all 12 entries go through the exact
// same compiled binary as the reinit-sweep numbers above.
// ---------------------------------------------------------------------

static void mode_multistream(const core_entry_t *core, uint32_t base_key, size_t n, size_t K, FILE *out) {
    if (K == 0 || (core->max_rng >= 0 && (long)K > core->max_rng)) {
        fprintf(stderr, "--multistream: K=%zu invalid for core '%s' (max_rng=%ld)\n", K, core->name, core->max_rng);
        exit(1);
    }
    uint64_t block_idx = 0;
    size_t remaining = n;
    while (remaining > 0) {
        size_t block_len = remaining < K ? remaining : K;
        uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
        core->fn(key, block_len, out);
        remaining -= block_len;
        ++block_idx;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage:\n"
            "  %s --stream <core> <key> <rng>\n"
            "  %s --multistream <core> <base_key> <n> <K>\n"
            "  %s reinit-sweep <core> <cycles_target_words> <k1> <k2> ...\n",
            argv[0], argv[0], argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "--stream") == 0 && argc >= 5) {
        const core_entry_t *core = find_core(argv[2]);
        uint32_t key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t rng = (size_t)strtoull(argv[4], NULL, 0);
        core->fn(key, rng, stdout);
        return 0;
    }
    if (strcmp(argv[1], "--multistream") == 0 && argc >= 6) {
        const core_entry_t *core = find_core(argv[2]);
        uint32_t base_key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t n = (size_t)strtoull(argv[4], NULL, 0);
        size_t K = (size_t)strtoull(argv[5], NULL, 0);
        mode_multistream(core, base_key, n, K, stdout);
        return 0;
    }
    if (strcmp(argv[1], "reinit-sweep") == 0 && argc >= 5) {
        const core_entry_t *core = find_core(argv[2]);
        long long cycles_target_words = atoll(argv[3]);
        mode_reinit_sweep(core, argc, argv, 4, cycles_target_words);
        return 0;
    }
    fprintf(stderr, "Unknown mode or missing args: %s\n", argv[1]);
    return 1;
}
