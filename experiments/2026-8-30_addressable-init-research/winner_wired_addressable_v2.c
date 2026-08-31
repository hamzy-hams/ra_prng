// winner_wired_addressable_v2.c
// Tahap 5 (HANDOVER_TAHAP5.md): implementation-level speed optimization of
// winner_wired_addressable.c's ra_init_state_addressable(). NEW FILE --
// does not modify winner_wired_addressable.c, winner_wired_v2.c, or any
// other winner_wired* variant in place, per this research's read-only
// constraint (see HANDOVER_TAHAP5.md section 7).
//
// ONLY CHANGE vs winner_wired_addressable.c: ra_init_state_addressable()'s
// rotate is now an explicit AVX-512VL `_mm256_rolv_epi32` intrinsic instead
// of relying on GCC's auto-vectorizer, which (confirmed via objdump both in
// the Tahap 5 prep session and independently reproduced in this session,
// 2026-08-30) emits a 3-instruction sllv+srlv+or pattern for rot32 inside
// the auto-vectorized loop instead of the single-instruction hardware
// rotate this CPU (Intel Tiger Lake, AVX-512F/VL) actually has available.
// `rot32`, `ra_hash`, `ra_permutation_cycle`, `ra_reseed`, `ra_core`'s
// control flow are BYTE-FOR-BYTE identical to winner_wired_addressable.c --
// ra_core still calls a function producing the exact same (L, M) values,
// just computed with a different (faster) instruction sequence.
//
// FORMULA-PRESERVING, NOT a redesign: output is bit-identical to
// winner_wired_addressable.c for every key tested (0, 0xFFFFFFFF, several
// random keys, and keys 0..31 covering every rotate-amount residue --
// verified via `cmp` in this Tahap 5 session, see RESULTS.md). Per
// HANDOVER_TAHAP5.md section 6, a bit-identical formula-preserving change
// automatically inherits winner_wired_addressable.c's Tahap 3 statistical
// guarantees (128GB PractRand clean, 0 collision to 500k keys) -- Tahap 3
// was NOT rerun for this file, by design.
//
// Measured effect (this machine, GCC 16.1.1, Intel i3-1115G4; see
// RESULTS.md for full methodology and repeated-trial numbers): init cost
// ~55-58ns -> ~37-39ns (~30-35% reduction), throughput break-even N*
// ~50 -> ~26, reinit-frequency break-even K* (median-of-3, robust to the
// run-to-run noise this microbenchmark shows) ~112 -> ~57 -- roughly
// halving both break-even points vs winner_wired_addressable.c.
//
// Rank 2 in HANDOVER_TAHAP5.md section 4's candidate ranking. Rank 1
// (-mprefer-vector-width=512, compile flag only, no code change) measured
// a comparable win on the ORIGINAL formula's auto-vectorized code path but
// was not adopted here: it has no effect on this file's explicit __m256i
// intrinsics (they are already fixed at 256-bit regardless of that flag --
// verified empirically this session), and depending on a compile flag is
// less robust than an explicit, portable intrinsic choice. See RESULTS.md
// "Tahap 5" section for the flag-only numbers if that path is preferred
// instead of this file.
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h
//          winner_wired_addressable_v2.c -o winner_wired_addressable_v2
// (same flags as winner_wired_addressable.c; -march=native must resolve to
// a target with AVX-512F/VL for _mm256_rolv_epi32 to be available -- this
// is NOT a portability regression vs the original file, since GCC's own
// auto-vectorizer already required AVX-512VL to produce the sllv/srlv/or
// sequence it emits for winner_wired_addressable.c's rot32 on this class of
// CPU; a non-AVX-512VL target falls back to scalar code for BOTH files.)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <immintrin.h>

#define TOTAL_RNG 200000000UL // matches winner_wired_v2.c's benchmark constant
#define ADDR_L_MIX_CONST 0x9e3779b7u // Tahap 1: frozen, see HANDOVER.md

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

// hash_access=sequential, HASH_SELFIDX off, HASH_DEPTH fixed at 32.
// Byte-for-byte copy of winner_wired_v2.c's ra_hash().
static void ra_hash(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}

// Tahap 1 frozen formula (HANDOVER.md "Tahap 1: Spesifikasi Final"), same
// math as winner_wired_addressable.c's ra_init_state_addressable(). Tahap 5
// Rank 2: explicit AVX-512VL rotate (_mm256_rolv_epi32) replacing the
// sllv+srlv+or pattern the auto-vectorizer emits for rot32, 8 lanes
// (i..i+7) per iteration -- same vector width GCC itself chose for the
// original file, so this isolates the rotate-instruction change alone.
// l_val/m_val factored as C*(i+key) (single add + one mullo per array),
// matching the algebraic factorization GCC's auto-vectorizer already uses
// for the original file -- HANDOVER_TAHAP5.md section 3 confirmed there is
// no remaining win available there. Explicit `&31` mask kept (matches
// rot32's semantics exactly; not proven redundant for vprolvd, so not
// removed -- see HANDOVER_TAHAP5.md section 4 Rank 2 note).
static void ra_init_state_addressable(uint32_t *L, uint32_t *M, uint32_t key) {
    const __m256i key_v = _mm256_set1_epi32((int)key);
    const __m256i cL_v = _mm256_set1_epi32((int)ADDR_L_MIX_CONST);
    const __m256i cM_v = _mm256_set1_epi32((int)0x06a0dd9bu);
    const __m256i mask31 = _mm256_set1_epi32(31);
    const __m256i inc8 = _mm256_set1_epi32(8);
    __m256i i_v = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);

    for (int i = 0; i < 256; i += 8) {
        __m256i sum = _mm256_add_epi32(i_v, key_v);       // i + key
        __m256i l_val = _mm256_mullo_epi32(sum, cL_v);    // C_L*(i+key)
        __m256i m_val = _mm256_mullo_epi32(sum, cM_v);    // C_M*(i+key)
        __m256i r_v = _mm256_and_si256(_mm256_xor_si256(key_v, i_v), mask31);

        __m256i l_rot = _mm256_rolv_epi32(l_val, r_v);
        __m256i m_rot = _mm256_rolv_epi32(m_val, r_v);

        _mm256_store_si256((__m256i *)&L[i], l_rot);
        _mm256_store_si256((__m256i *)&M[i], m_rot);

        i_v = _mm256_add_epi32(i_v, inc8);
    }
}

// One full 255-step permutation cycle. Byte-for-byte copy of
// winner_wired_v2.c's/winner_wired_addressable.c's ra_permutation_cycle()
// -- untouched, per this file's header comment.
static void ra_permutation_cycle(uint32_t cons, size_t it,
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

// Byte-for-byte copy of winner_wired_v2.c's/winner_wired_addressable.c's
// ra_reseed().
static uint32_t ra_reseed(uint32_t *M, const uint32_t *L) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];

    uint32_t tmp8[8];
    ra_hash(M, tmp8);

    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

// Same structure as winner_wired_addressable.c's ra_core() -- untouched
// except it now calls the AVX-512VL-intrinsics ra_init_state_addressable()
// defined above.
uint32_t ra_core(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_state_addressable(L, M, key);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

int main(int argc, char **argv) {
    uint32_t last_cons;
    uint32_t key;

    if (argc >= 4 && strcmp(argv[1], "--stream") == 0) {
        key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t rng = (size_t)strtoull(argv[3], NULL, 0);

        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        last_cons = ra_core(key, rng, stdout);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

        fprintf(stderr, "Streamed %zu pseudorandom outputs in %.3f seconds\n", rng, elapsed);
        fprintf(stderr, "Last cons from RNGing: %u\n", last_cons);
        return 0;
    }

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    key = 1;
    last_cons = ra_core(key, TOTAL_RNG, NULL);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    printf("Generated %lu pseudorandom updates in %.3f seconds\n",
           (unsigned long)TOTAL_RNG, elapsed);
    printf("Last cons from RNGing: %lu\n", (unsigned long)last_cons);

    return 0;
}
