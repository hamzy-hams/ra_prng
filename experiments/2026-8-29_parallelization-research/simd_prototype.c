// Q4 stretch-goal microbenchmark (time-boxed, per HANDOVER.md's optional
// question 4). Compares an AVX2 8-lane kernel against an 8x-unrolled scalar
// loop for ONLY the genuinely vectorizable part of ra_permutation_cycle --
// the o/a/b/c/d arithmetic, which reads M[(i+6)&0xFF]/M[(i+7)&0xFF] using
// the SAME index i across all lanes (contiguous SoA load, no gather needed).
//
// Deliberately OMITS the L[i]<->L[d] swap and the ra_reseed fold: d = c &
// 0xFF is data-dependent per-lane, so vectorizing the swap needs an AVX2
// gather for the read, and AVX2 has NO scatter instruction at all for the
// write (only AVX-512 does) -- see RESULTS.md's Q4 section for the full
// argument. This file is NOT a correctness-validated generator and its
// output is never checked against winner_wired_v2.c -- it exists purely to
// instruction-count the arithmetic kernel in isolation, for a directional
// answer ("is the vectorizable part faster, and does L-swap dominate").
//
// Build: gcc -O3 -march=native -mavx2 -std=gnu17 simd_prototype.c -o simd_prototype
// Run:   ./simd_prototype scalar | ./simd_prototype simd

#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define LANES 8
#define TOTAL_RNG 200000000UL  // same constant as winner_wired_v2.c's benchmark

static inline uint32_t rot32_scalar(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

static inline __m256i rot32_avx2(__m256i n, __m256i r) {
    __m256i left = _mm256_sllv_epi32(n, r);
    __m256i right_amt = _mm256_sub_epi32(_mm256_set1_epi32(32), r);
    __m256i right = _mm256_srlv_epi32(n, right_amt);
    return _mm256_or_si256(left, right);
}

// 8x-unrolled scalar loop over independent lanes -- same arithmetic, same
// M-read pattern, same iteration count as the SIMD path, minus vector
// intrinsics. This is the fair baseline: "8 streams processed one at a
// time" vs "8 streams processed together in one register".
static void run_scalar(void) {
    static uint32_t M[LANES][256];
    for (int l = 0; l < LANES; ++l)
        for (int i = 0; i < 256; ++i)
            M[l][i] = (uint32_t)((i + l) * 0x06a0dd9bUL + 0x06a0dd9bUL);

    uint32_t cons[LANES], a[LANES], b[LANES], c[LANES] = {0}, d[LANES] = {0};
    for (int l = 0; l < LANES; ++l) { cons[l] = l + 1; a[l] = cons[l]; b[l] = 0; }

    uint64_t iterations = TOTAL_RNG / 255 + 1;
    for (uint64_t it = 0; it < iterations; ++it) {
        for (uint32_t i = 255; i > 0; --i) {
            uint8_t idx6 = (uint8_t)(i + 6), idx7 = (uint8_t)(i + 7);
            for (int l = 0; l < LANES; ++l) {
                uint32_t o = (M[l][idx6] << 6) ^ (M[l][idx7] << 7);
                a[l] = (d[l] ^ o) ^ (cons[l] + a[l]);
                b[l] = (cons[l] + a[l]) ^ (o + d[l]);
                c[l] = rot32_scalar((a[l] >> 13) ^ a[l], b[l]);
                d[l] = c[l] & 0xFFu;
            }
        }
    }
    printf("scalar sample c[0]=%u\n", c[0]);
}

static void run_simd(void) {
    static uint32_t M[256][LANES];  // SoA: contiguous per-index load across lanes
    for (int i = 0; i < 256; ++i)
        for (int l = 0; l < LANES; ++l)
            M[i][l] = (uint32_t)((i + l) * 0x06a0dd9bUL + 0x06a0dd9bUL);

    __m256i cons = _mm256_setr_epi32(1, 2, 3, 4, 5, 6, 7, 8);
    __m256i a = cons, b = _mm256_setzero_si256();
    __m256i c = _mm256_setzero_si256(), d = _mm256_setzero_si256();
    __m256i mask31 = _mm256_set1_epi32(31);
    __m256i mask255 = _mm256_set1_epi32(0xFF);

    uint64_t iterations = TOTAL_RNG / 255 + 1;
    for (uint64_t it = 0; it < iterations; ++it) {
        for (uint32_t i = 255; i > 0; --i) {
            int idx6 = (uint8_t)(i + 6), idx7 = (uint8_t)(i + 7);
            __m256i m6 = _mm256_loadu_si256((const __m256i *)M[idx6]);
            __m256i m7 = _mm256_loadu_si256((const __m256i *)M[idx7]);
            __m256i o = _mm256_xor_si256(_mm256_slli_epi32(m6, 6), _mm256_slli_epi32(m7, 7));

            a = _mm256_xor_si256(_mm256_xor_si256(d, o), _mm256_add_epi32(cons, a));
            b = _mm256_xor_si256(_mm256_add_epi32(cons, a), _mm256_add_epi32(o, d));
            __m256i shr13 = _mm256_xor_si256(_mm256_srli_epi32(a, 13), a);
            __m256i rot_amt = _mm256_and_si256(b, mask31);
            c = rot32_avx2(shr13, rot_amt);

            d = _mm256_and_si256(c, mask255);
            // L[i]<->L[d] swap intentionally omitted -- see file header.
        }
    }
    uint32_t out[LANES];
    _mm256_storeu_si256((__m256i *)out, c);
    printf("simd sample c[0]=%u\n", out[0]);
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s scalar|simd\n", argv[0]); return 1; }

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    if (strcmp(argv[1], "scalar") == 0) run_scalar();
    else if (strcmp(argv[1], "simd") == 0) run_simd();
    else { fprintf(stderr, "unknown mode %s\n", argv[1]); return 1; }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    fprintf(stderr, "mode=%s: %lu updates x %d lanes in %.3f seconds\n",
            argv[1], TOTAL_RNG, LANES, elapsed);
    return 0;
}
