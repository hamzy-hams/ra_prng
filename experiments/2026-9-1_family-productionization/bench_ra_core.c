// bench_ra_core.c
// Tahap 2 (family-productionization): confirm the Tahap 1 unification/
// rename (winner_wired_v2.c + winner_wired_addressable.c + tahap6_bench.c's
// ra_core_singleblock -> ra_core.c's ra_core_orbit/ra_core_singleblock) did
// not regress speed -- reuses tahap6_bench.c's throughput/reinit-sweep
// methodology (min-of-trials, CLOCK_MONOTONIC, volatile checksum sink,
// Philox4x32-10 comparison, corrected PHILOX_M0) verbatim, pointed at the
// new ra_core.c entry points via extern declarations. Does not modify
// ra_core.c or tahap6_bench.c -- links against ra_core.c as an object file
// (built with -Dmain=<other name> to avoid a duplicate main() symbol).
//
// Compile:
//   gcc -O3 -march=native -std=gnu17 -include stdalign.h \
//       -Dmain=ra_core_unused_main -c ra_core.c -o ra_core_nomain.o -Wall -Wextra
//   gcc -O3 -march=native -std=gnu17 bench_ra_core.c ra_core_nomain.o \
//       -o bench_ra_core -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

extern uint32_t ra_core_orbit(uint32_t key, size_t rng, FILE *raw_stream);
extern uint32_t ra_core_singleblock(uint32_t key, size_t rng, FILE *raw_stream);

typedef uint32_t (*core_fn_t)(uint32_t key, size_t rng, FILE *raw_stream);
typedef struct { const char *name; core_fn_t fn; long max_rng; } core_entry_t;

static const core_entry_t CORES[] = {
    { "orbit", ra_core_orbit, -1 },
    { "singleblock", ra_core_singleblock, 255 },
};

static const core_entry_t *find_core(const char *name) {
    for (size_t i = 0; i < sizeof(CORES) / sizeof(CORES[0]); ++i) {
        if (strcmp(CORES[i].name, name) == 0) return &CORES[i];
    }
    fprintf(stderr, "Unknown core '%s'. Known: orbit singleblock\n", name);
    exit(1);
}

// ----------------------------------------------------------------------
// Philox4x32-10 -- byte-for-byte copy of tahap6_bench.c's (corrected
// PHILOX_M0). See tahap4_bench.c header comment for the Random123
// citation/rationale.
// ----------------------------------------------------------------------

#define PHILOX_M0 0xD2511F53U
#define PHILOX_M1 0xCD9E8D57U
#define PHILOX_W0 0x9E3779B9U
#define PHILOX_W1 0xBB67AE85U

static inline void philox4x32_round(uint32_t ctr[4], uint32_t key[2]) {
    uint64_t p0 = (uint64_t)ctr[0] * PHILOX_M0;
    uint64_t p1 = (uint64_t)ctr[2] * PHILOX_M1;
    uint32_t hi0 = (uint32_t)(p0 >> 32);
    uint32_t lo0 = (uint32_t)p0;
    uint32_t hi1 = (uint32_t)(p1 >> 32);
    uint32_t lo1 = (uint32_t)p1;
    uint32_t c1 = ctr[1];
    uint32_t c3 = ctr[3];
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
    if (++ctr[0] == 0) { if (++ctr[1] == 0) { if (++ctr[2] == 0) { ++ctr[3]; } } }
}

// ----------------------------------------------------------------------
// benchmark modes -- verbatim methodology from tahap6_bench.c.
// ----------------------------------------------------------------------

static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

static long trials_for_n(long long n) {
    if (n < 1000) return 200;
    if (n < 100000) return 30;
    if (n < 10000000) return 5;
    return 1;
}

static void mode_throughput(const core_entry_t *core, int argc, char **argv, int start) {
    volatile uint32_t sink = 0;
    for (int a = start; a < argc; ++a) {
        long long n = atoll(argv[a]);
        if (n <= 0) continue;
        if (core->max_rng >= 0 && n > core->max_rng) {
            fprintf(stderr, "throughput: N=%lld exceeds core '%s' max_rng=%ld -- skipping.\n",
                    n, core->name, core->max_rng);
            continue;
        }
        long trials = trials_for_n(n);

        double addr_best = -1.0;
        for (long t = 0; t < trials; ++t) {
            double t0 = now_seconds();
            uint32_t last_cons = core->fn(12345u, (size_t)n, NULL);
            double t1 = now_seconds();
            sink ^= last_cons;
            double dt = t1 - t0;
            if (addr_best < 0 || dt < addr_best) addr_best = dt;
        }

        long long blocks = (n + 3) / 4;
        double philox_best = -1.0;
        for (long t = 0; t < trials; ++t) {
            uint32_t ctr[4] = {0,0,0,0};
            uint32_t key[2] = {0xDEADBEEFu, 0xFEEDC0DEu};
            double t2 = now_seconds();
            for (long long b = 0; b < blocks; ++b) {
                uint32_t out[4];
                philox4x32_10(out, ctr, key);
                philox_ctr_increment(ctr);
                sink ^= out[0] ^ out[1] ^ out[2] ^ out[3];
            }
            double t3 = now_seconds();
            double dt = t3 - t2;
            if (philox_best < 0 || dt < philox_best) philox_best = dt;
        }

        printf("N %lld addr_seconds %.6f addr_ns_per_word %.3f philox_seconds %.6f philox_ns_per_word %.3f trials %ld checksum %u\n",
               n, addr_best, addr_best * 1e9 / (double)n,
               philox_best, philox_best * 1e9 / (double)n, trials, sink);
    }
}

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
        double addr_total = -1.0, philox_total = -1.0;

        for (int trial = 0; trial < TRIALS; ++trial) {
            double t0 = now_seconds();
            for (long long c = 0; c < cycles; ++c) {
                uint32_t key = (uint32_t)(c * 2654435761u + 1u);
                uint32_t last = core->fn(key, (size_t)k, NULL);
                sink ^= last;
            }
            double t1 = now_seconds();
            double dt = t1 - t0;
            if (addr_total < 0 || dt < addr_total) addr_total = dt;
        }

        long long blocks_per_cycle = (k + 3) / 4;
        for (int trial = 0; trial < TRIALS; ++trial) {
            double t2 = now_seconds();
            for (long long c = 0; c < cycles; ++c) {
                uint32_t ctr[4] = {0,0,0,0};
                uint32_t key[2] = { (uint32_t)(c * 40503u + 1u), 0xFEEDC0DEu };
                for (long long b = 0; b < blocks_per_cycle; ++b) {
                    uint32_t out[4];
                    philox4x32_10(out, ctr, key);
                    philox_ctr_increment(ctr);
                    sink ^= out[0] ^ out[1] ^ out[2] ^ out[3];
                }
            }
            double t3 = now_seconds();
            double dt = t3 - t2;
            if (philox_total < 0 || dt < philox_total) philox_total = dt;
        }

        double addr_ns_per_word = addr_total * 1e9 / (double)(cycles * k);
        double philox_ns_per_word = philox_total * 1e9 / (double)(cycles * k);

        printf("K %lld cycles %lld addr_ns_per_word_steadystate %.3f philox_ns_per_word %.3f ratio %.3f checksum %u\n",
               k, cycles, addr_ns_per_word, philox_ns_per_word,
               addr_ns_per_word / philox_ns_per_word, sink);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage:\n"
            "  %s throughput <core> <n1> <n2> ...\n"
            "  %s reinit-sweep <core> <cycles_target_words> <k1> <k2> ...\n"
            "Cores: orbit singleblock (singleblock valid only for rng in [1,255])\n",
            argv[0], argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "throughput") == 0 && argc >= 4) {
        const core_entry_t *core = find_core(argv[2]);
        mode_throughput(core, argc, argv, 3);
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
