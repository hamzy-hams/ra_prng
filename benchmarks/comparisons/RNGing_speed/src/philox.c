#include <stdint.h>
#include <stdio.h>
#include <time.h>

// ----------------- Philox4x32-10 Core -----------------

// Constants for Philox4x32-10
#define PHILOX_M0 0xD256D193U
#define PHILOX_M1 0xCD9E8D57U
#define PHILOX_W0 0x9E3779B9U
#define PHILOX_W1 0xBB67AE85U

// Single round of Philox4x32
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

    // bump the key
    key[0] += PHILOX_W0;
    key[1] += PHILOX_W1;
}

// Philox4x32-10 block function: 10 rounds
void philox4x32_10(uint32_t out[4], const uint32_t ctr_in[4], const uint32_t key_in[2]) {
    uint32_t ctr[4] = { ctr_in[0], ctr_in[1], ctr_in[2], ctr_in[3] };
    uint32_t key[2] = { key_in[0], key_in[1] };

    for (int i = 0; i < 10; i++) {
        philox4x32_round(ctr, key);
    }
    // write output
    out[0] = ctr[0];
    out[1] = ctr[1];
    out[2] = ctr[2];
    out[3] = ctr[3];
}

// ----------------- Benchmark -----------------

int main() {
    const size_t N = 255000000;      // total 32-bit number
    uint32_t ctr[4] = {0, 0, 0, 0};  // counter
    uint32_t key[2] = {0xDEADBEEF, 0xFEEDC0DE};  // seed key

    uint32_t block[4];
    size_t produced = 0;

    clock_t t0 = clock();
    while (produced < N) {
        philox4x32_10(block, ctr, key);
        // increment counter (as 128-bit little-endian)
        if (++ctr[0] == 0) {
            if (++ctr[1] == 0) {
                if (++ctr[2] == 0) {
                    ++ctr[3];
                }
            }
        }
        // output 4 × 32-bit per block
        for (int i = 0; i < 4 && produced < N; i++, produced++) {
            volatile uint32_t v = block[i];
            (void)v;  // preventing elide optimization
        }
    }
    clock_t t1 = clock();

    double elapsed = (double)(t1 - t0) / CLOCKS_PER_SEC;
    double total_mb = (double)N * sizeof(uint32_t) / (1024.0 * 1024.0);
    double mb_per_s = total_mb / elapsed;

    printf("Generated %zu 32-bit values in %.3f seconds\n", N, elapsed);
    printf("Throughput: %.2f MB/s\n", mb_per_s);

    return 0;
}
