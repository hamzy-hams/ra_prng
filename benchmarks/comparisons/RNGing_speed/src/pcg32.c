#include <stdint.h>
#include <stdio.h>
#include <time.h>

// ----------------- PCG32 Implementation -----------------

typedef struct {
    uint64_t state;
    uint64_t inc;
} pcg32_random_t;

// PCG seed and sequence initiation using odd increment
void pcg32_srandom(pcg32_random_t* rng, uint64_t initstate, uint64_t initseq) {
    rng->state = 0U;
    rng->inc   = (initseq << 1u) | 1u;
    // Put through generator once to mix initial state
    pcg32_random_t tmp = *rng;
    // advance state
    rng->state = tmp.state * 6364136223846793005ULL + rng->inc;
    rng->state += initstate;
    // one more step
    rng->state = rng->state * 6364136223846793005ULL + rng->inc;
}

// PCG32
uint32_t pcg32_random_r(pcg32_random_t* rng) {
    uint64_t oldstate = rng->state;
    // advance RNG state
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    // calculate output function (xorshifted + rotate)
    uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32_t rot        = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

// ----------------- Benchmark Code -----------------

int main() {
    const size_t N = 255000000;      // total 32-bit number
    pcg32_random_t rng;
    pcg32_srandom(&rng, 42u, 54u);   // seed "42", sequence "54"

    // Benchmark
    clock_t t0 = clock();
    for (size_t i = 0; i < N; i++) {
        volatile uint32_t v = pcg32_random_r(&rng);
        (void)v;  // preventing elide optimization
    }
    clock_t t1 = clock();
    double elapsed = (double)(t1 - t0) / CLOCKS_PER_SEC;

    double total_mb = (double)N * sizeof(uint32_t) / (1024.0 * 1024.0);
    double mb_per_s = total_mb / elapsed;

    printf("Generated %zu 32-bit values in %.3f seconds\n", N, elapsed);
    printf("Throughput: %.2f MB/s\n", mb_per_s);

    return 0;
}
