#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// ----------------- ChaCha20 Core -----------------

static inline uint32_t rotl32(uint32_t x, int r) {
    return (x << r) | (x >> (32 - r));
}

#define QUARTERROUND(a, b, c, d) \
    a += b; d ^= a; d = rotl32(d,16); \
    c += d; b ^= c; b = rotl32(b,12); \
    a += b; d ^= a; d = rotl32(d, 8); \
    c += d; b ^= c; b = rotl32(b, 7);

// ChaCha20 block function: input key (8×32b), counter, nonce (3×32b)
void chacha20_block(uint32_t out[16],
                    const uint32_t key[8],
                    uint32_t counter,
                    const uint32_t nonce[3])
{
    // constants "expand 32-byte k"
    const uint32_t constants[4] = {
        0x61707865, 0x3320646e, 0x79622d32, 0x6b206574
    };

    uint32_t state[16];
    // initialize state
    memcpy(state+0,  constants, 16);
    memcpy(state+4,  key,       32);
    state[12] = counter;
    memcpy(state+13, nonce,     12);

    // working copy
    uint32_t x[16];
    memcpy(x, state, sizeof x);

    // 20 rounds: 10 column + 10 diagonal
    for (int i = 0; i < 10; i++) {
        // column rounds
        QUARTERROUND(x[0], x[4], x[ 8], x[12]);
        QUARTERROUND(x[1], x[5], x[ 9], x[13]);
        QUARTERROUND(x[2], x[6], x[10], x[14]);
        QUARTERROUND(x[3], x[7], x[11], x[15]);
        // diagonal rounds
        QUARTERROUND(x[0], x[5], x[10], x[15]);
        QUARTERROUND(x[1], x[6], x[11], x[12]);
        QUARTERROUND(x[2], x[7], x[ 8], x[13]);
        QUARTERROUND(x[3], x[4], x[ 9], x[14]);
    }

    // add original state
    for (int i = 0; i < 16; i++) {
        out[i] = x[i] + state[i];
    }
}

// ----------------- ChaCha20 PRNG & Benchmark -----------------

int main() {
    const size_t N = 255000000;      // total 32-bit numbers
    uint32_t key[8]   = {
        0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
        0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c
    };
    uint32_t nonce[3] = { 0x00000009, 0x0000004a, 0x00000000 };
    uint32_t counter  = 1;

    uint32_t block[16];
    size_t produced = 0;

    clock_t t0 = clock();
    while (produced < N) {
        chacha20_block(block, key, counter++, nonce);
        // this block generating 16 x 32-bit words
        for (int i = 0; i < 16 && produced < N; i++, produced++) {
            volatile uint32_t v = block[i];
            (void)v;
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
