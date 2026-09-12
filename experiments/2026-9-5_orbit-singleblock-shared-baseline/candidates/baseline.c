// baseline.c -- byte-identical starting point for editing the orbit/
// singleblock shared transform, using driver_main_template.c's dual-core
// (CORE_INIT/CORE_TRANSFORM) convention. See HANDOVER.md in this folder for
// why this exists and what it already proved.
//
// my_init/my_transform below are copied verbatim from
// ../../2026-9-1_family-productionization/ra_core.c:
//   - ra_init_state_orbit (L144-207, specifically L197-207)
//   - ra_permutation_cycle_orbit (L217-252), with the unconditional
//     L[i]<->L[d] swap (L241-243) replaced by `if (L) { ... }` -- this is
//     the ONLY textual change from ra_core.c, and it is behavior-preserving:
//     L[] is write-only within one block (only ra_reseed() at the NEXT
//     block boundary reads it), so orbit (L non-NULL) is unaffected and
//     singleblock (L NULL) just skips writes nothing would have read yet
//     anyway -- matching ra_permutation_cycle_singleblock's own "No L[]
//     swap here" comment (ra_core.c:348-349) exactly.
//
// my_init always computes the full M[0..256) range for both cores, unlike
// production's ra_init_state_singleblock which skips indices proven
// unreachable for a given `rng` (pure speed optimization, ra_core.c:280-317).
// That optimization is intentionally NOT ported here: it depends on
// my_transform indexing M[] ONLY via the loop variable `i`, never via `d`
// (ra_core.c:288-291's own warning) -- exactly the kind of invariant an
// experimental edit is likely to break. Always-full-range init is slower
// but can never silently go out of sync with an edited transform.
//
// EDIT my_init / my_transform to try a new formula. Both cores (orbit via
// `./baseline orbit ...`, singleblock via `./baseline singleblock ...`)
// come from these same two functions -- no separate singleblock copy to
// remember to keep in sync.
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h baseline.c -o baseline
// Test:    python ../../../tools/battery/run_battery.py --binary ./baseline \
//              --dual-core --k 1,255 --continuous --quick

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

// L == NULL selects singleblock mode (no L[] computed/written); non-NULL
// selects orbit mode. Verbatim ra_init_state_orbit formula for both L[] and
// M[] -- see file header for why M[] is always computed for the full range.
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

// L == NULL selects singleblock mode (the `if (L)` swap below is skipped
// entirely, matching ra_permutation_cycle_singleblock's own logic).
static void my_transform(uint32_t cons, size_t it, const uint32_t *M, uint32_t *L,
                          uint64_t *count, FILE *raw_stream) {

    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    uint32_t o = (M[(uint8_t)(255 + 0)] << 0) ^ (M[(uint8_t)(255 + 1)] << 1) ^
                 (M[(uint8_t)(255 + 2)] << 2) ^ (M[(uint8_t)(255 + 3)] << 3) ^
                 (M[(uint8_t)(255 + 4)] << 4) ^ (M[(uint8_t)(255 + 5)] << 5) ^
                 (M[(uint8_t)(255 + 6)] << 6) ^ (M[(uint8_t)(255 + 7)] << 7);
    uint32_t o_term7 = M[(uint8_t)(255 + 7)] << 7;    
    for (uint32_t i = 255; i > 0; --i) {
        // >>> EDIT HERE: the per-round transform <<<
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
            uint32_t new_term = M[(uint8_t)(i - 1)];
            uint32_t o_next = new_term ^ ((o ^ o_term7) << 1);
            o_term7 = M[(uint8_t)(i + 6)] << 7;
            o = o_next;
        }
    }
}

#define CORE_INIT my_init
#define CORE_TRANSFORM my_transform
#include "../../../tools/battery/driver_main_template.c"
