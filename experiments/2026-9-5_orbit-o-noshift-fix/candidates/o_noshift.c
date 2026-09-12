// o_noshift.c -- fix candidate: `o` computed as raw XOR of its 8-word
// window (no per-tap <<0..7 bit-shift), maintained via a rolling update
// derived and verified separately (see HANDOVER note / session log):
//
//   o_new = o_old ^ M[(uint8_t)(i+7)] ^ M[(uint8_t)(i-1)]
//
// (XOR is self-inverse: remove the term that leaves the 8-word window
// when it slides from root i to root i-1, add the term that enters.)
// Verified against brute-force recomputation for all 255 rounds before
// being used here.
//
// Hypothesis under test: production's per-tap shift (<<0..7) staggers the
// 8 M[] taps before XOR-ing them together, specifically to avoid identical
// bit positions across taps cancelling in a correlated way. Removing the
// shift (this candidate) tests whether that staggering is load-bearing for
// the K=1+hashed defect, or irrelevant.
//
// my_init is UNCHANGED from
// ../../2026-9-5_orbit-singleblock-shared-baseline/candidates/baseline.c
// (verbatim ra_init_state_orbit) -- only my_transform's `o` computation
// differs. Same dual-core (CORE_INIT/CORE_TRANSFORM) convention.
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h o_noshift.c -o o_noshift
// Test (per this session's scope decision): K=1 only, hashed + Weyl ordering.
//   python ../../../tools/battery/run_battery.py --binary ./o_noshift \
//       --dual-core --k 1 --orderings weyl,hashed

#include <stdio.h>
#include <stdint.h>

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

#define GUARD_L 0x38916df4u
#define GUARD_M 0x6c26fc92u

static void my_init(uint32_t *L, uint32_t *M, uint32_t key) {
    uint32_t keyterm_l = fmix32(GUARD_L ^ (0x9e3779b7u * key));
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32(m_val, r);
        if (L) {
            uint32_t l_val = (uint32_t)(i * 0x9e3779b7u) * keyterm_l;
            L[i] = rot32(l_val, r);
        }
    }
}

static void my_transform(uint32_t cons, size_t it, const uint32_t *M, uint32_t *L,
                          uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    // >>> EDIT vs baseline.c: raw XOR, no per-tap shift <<<
    uint32_t o = M[(uint8_t)(255 + 0)] ^ M[(uint8_t)(255 + 1)] ^
                 M[(uint8_t)(255 + 2)] ^ M[(uint8_t)(255 + 3)] ^
                 M[(uint8_t)(255 + 4)] ^ M[(uint8_t)(255 + 5)] ^
                 M[(uint8_t)(255 + 6)] ^ M[(uint8_t)(255 + 7)];

    for (uint32_t i = 255; i > 0; --i) {
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 17u;

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & 0xFFu;

        if (*count <= 1) break;
        --(*count);

        if (L) {
            uint32_t tmp = L[i];
            L[i] = L[d];
            L[d] = tmp;
        }

        if (i > 1) {
            // rolling no-shift window update, verified against brute-force
            // recomputation for all 255 rounds before use here.
            uint32_t exiting = M[(uint8_t)(i + 7)];
            uint32_t entering = M[(uint8_t)(i - 1)];
            o = o ^ exiting ^ entering;
        }
    }
}

#define CORE_INIT my_init
#define CORE_TRANSFORM my_transform
#include "../../../tools/battery/driver_main_template.c"
