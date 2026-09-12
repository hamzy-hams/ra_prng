// bench_init_only_baseline.c -- sama seperti bench_init_only.c, TAPI
// untuk ra_core.c kanonik (signature init 1-param: key saja, tanpa
// addr_hi) -- baseline pembanding biaya inisiasi murni.
//
// Compile:
//   gcc -O3 -march=native -std=gnu17 -include stdalign.h \
//       bench_init_only_baseline.c -o bench_init_baseline

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define main ra_core_unused_main_include
#include "../2026-9-1_family-productionization/ra_core.c"
#undef main

static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage:\n"
            "  %s init-orbit <iterations>\n"
            "  %s init-singleblock <rng> <iterations>\n",
            argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "init-orbit") == 0 && argc >= 3) {
        long long iterations = atoll(argv[2]);
        alignas(64) uint32_t L[256], M[256];
        volatile uint32_t sink = 0;

        double best = -1.0;
        for (int trial = 0; trial < 5; ++trial) {
            double t0 = now_seconds();
            for (long long i = 0; i < iterations; ++i) {
                uint32_t key = (uint32_t)(i * 2654435761u + 1u);
                ra_init_state_orbit(L, M, key);
                sink ^= L[128] ^ M[128];
            }
            double t1 = now_seconds();
            double dt = t1 - t0;
            if (best < 0 || dt < best) best = dt;
        }
        printf("init-orbit iterations %lld best_seconds %.6f ns_per_init %.3f checksum %u\n",
               iterations, best, best * 1e9 / (double)iterations, sink);
        return 0;
    }

    if (strcmp(argv[1], "init-singleblock") == 0 && argc >= 4) {
        size_t rng = (size_t)strtoull(argv[2], NULL, 0);
        long long iterations = atoll(argv[3]);
        alignas(64) uint32_t M[256];
        volatile uint32_t sink = 0;

        double best = -1.0;
        for (int trial = 0; trial < 5; ++trial) {
            double t0 = now_seconds();
            for (long long i = 0; i < iterations; ++i) {
                uint32_t key = (uint32_t)(i * 2654435761u + 1u);
                ra_init_state_singleblock(M, key, rng);
                sink ^= M[(256 - (int)rng) & 0xFF];
            }
            double t1 = now_seconds();
            double dt = t1 - t0;
            if (best < 0 || dt < best) best = dt;
        }
        printf("init-singleblock rng %zu iterations %lld best_seconds %.6f ns_per_init %.3f checksum %u\n",
               rng, iterations, best, best * 1e9 / (double)iterations, sink);
        return 0;
    }

    fprintf(stderr, "Unknown mode or missing args: %s\n", argv[1]);
    return 1;
}
