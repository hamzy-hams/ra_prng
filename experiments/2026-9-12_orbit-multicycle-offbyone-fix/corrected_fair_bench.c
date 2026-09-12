// Corrected re-test: same anti-DCE technique AND same sink-write frequency
// as the official bench_release.c (indirect call via function pointer,
// volatile sink XORed ONCE per outer call -- not once per word). This
// directly checks whether the earlier "fair ~15% gap" microbenchmark
// (which XORed a volatile accumulator on EVERY inner word, 255x more
// often) was itself a confound.
#define RA_CORE_NO_MAIN
#include "/home/zep/Documents/research/ra_prng/src/ra_core/c/ra_core.c"
#include <time.h>

static double now_seconds(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

// Matches bench_release.c's bench_real_reinit4 exactly: indirect call via
// function pointer, sink updated once per call (not per word), TRIALS=5,
// best-of-5 reported.
static double bench_reinit(uint32_t (*fn)(uint32_t, uint32_t, size_t, FILE *),
                            long long k, long long cycles_target_words) {
    const int TRIALS = 5;
    double best = -1.0;
    long long cycles = cycles_target_words / k;
    if (cycles < 1) cycles = 1;
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

// Genuine steady-state: ONE call, N words, direct (not through pointer) --
// orbit supports this natively since it's unbounded. singleblock cannot
// (hard-capped rng<=255), so it's not tested here.
static double bench_stream_direct(long long n) {
    const int TRIALS = 5;
    double best = -1.0;
    volatile uint32_t sink = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        double t0 = now_seconds();
        sink ^= ra_core_orbit(0xC0FFEEu, 0u, (size_t)n, NULL);
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (best < 0 || dt < best) best = dt;
    }
    (void)sink;
    return best * 1e9 / (double)n;
}

int main(void) {
    const long long CYCLES_TARGET = 30000000LL;
    double sb255 = bench_reinit(ra_core_singleblock, 255, CYCLES_TARGET);
    double orbit255 = bench_reinit(ra_core_orbit, 255, CYCLES_TARGET);
    double orbit_stream = bench_stream_direct(20000000LL);
    printf("singleblock K=255 (indirect call, sink/call): %.4f ns/word\n", sb255);
    printf("orbit K=255      (indirect call, sink/call): %.4f ns/word\n", orbit255);
    printf("orbit --stream   (direct call, N=20M):        %.4f ns/word\n", orbit_stream);
    printf("ratio orbit255/singleblock255: %.2fx\n", orbit255/sb255);
    printf("ratio orbit_stream/singleblock255: %.2fx\n", orbit_stream/sb255);
    return 0;
}
