#include <stdint.h>
#include <stdio.h>
#include <time.h>

// ----------------- xoshiro256** Core -----------------

static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

// STATE: 4×64-bit
static uint64_t s[4];

// State initiation, must not be zero
void xoshiro256_init(uint64_t seed0, uint64_t seed1, uint64_t seed2, uint64_t seed3) {
    s[0] = seed0;
    s[1] = seed1;
    s[2] = seed2;
    s[3] = seed3;
}

// xoshiro256** next, generating 64bit number
uint64_t xoshiro256_next(void) {
    const uint64_t result = rotl(s[1] * 5, 7) * 9;
    const uint64_t t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];
    s[2] ^= t;
    s[3] = rotl(s[3], 45);

    return result;
}

// ----------------- Benchmark -----------------

int main() {
    const size_t N = 255000000;    // total 32-bit numbers
    // Seed, must not be zero
    xoshiro256_init(0x0123456789ABCDEFULL,
                    0xFEDCBA9876543210ULL,
                    0xF0E1D2C3B4A59687ULL,
                    0x1234567890ABCDEFULL);

    clock_t t0 = clock();
    for (size_t i = 0; i < N; i++) {
        // call next, take last 32 significant bit
        volatile uint32_t v = (uint32_t)xoshiro256_next();
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
