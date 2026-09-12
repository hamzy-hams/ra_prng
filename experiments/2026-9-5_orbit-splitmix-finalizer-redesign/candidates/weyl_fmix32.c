// weyl_fmix32.c -- Candidate A, orbit-splitmix-finalizer-redesign experiment
//
// Full architecture REPLACEMENT candidate for the ra_core_orbit K=1+hashed-
// key defect (FPF-14+6/16, see HANDOVER.md and
// ../../2026-9-1_family-productionization/PRODUCTION_READINESS_HANDOVER.md
// section 2b). 8 prior fix axes patching a single point inside
// ra_permutation_cycle_orbit all failed; this candidate does NOT use
// ra_permutation_cycle_orbit, ra_init_state_orbit, L[]/M[], ra_hash, or
// ra_reseed AT ALL -- a from-scratch generator using a Weyl-sequence
// 32-bit-ADD state advance (no multiply in the advance step, GPU-cheap)
// plus an fmix32 finalizer applied fresh to every state (SplitMix pattern,
// genuinely 32-bit throughout -- unlike the repo's existing splitmix32()
// helper below, which is actually SplitMix64 truncated to its high 32
// bits; see HANDOVER.md section 0.2 and section 8).
//
// fmix32 copied VERBATIM from
// ../../2026-9-1_family-productionization/ra_core.c L149-156.
//
// WEYL32 = 0x9e3779b7u: reuses ra_core.c's existing L-multiplier constant
// (ra_init_state_orbit L198) as the state-advance step, deliberately kept
// distinct from the test harness's own --stream key-stepping constant
// (0x9E3779B9u, used below only to derive successive block keys) so that
// "state-advance step" and "key-ordering step" are never conflated in this
// design's own reasoning (HANDOVER.md section 0.1 item 3 / section 3).
//
// Key->state mapping: option (a) "SplitMix-literal" from HANDOVER.md
// section 3, user-confirmed 2026-09-05 plan session:
//   state0   = fmix32(key)
//   state_j  = state0 + j*WEYL32
//   output_j = fmix32(state_j)
//
// GUARD_KEY addition (not in the handover's literal code block, added
// here): handover section 3's own worked example for option (b) flags
// fmix32(0)==0 as a weak-key edge case requiring a guard-XOR before use,
// same pattern as GUARD_L/GUARD_M in ra_init_state_orbit
// (project_multikey_remix_search memory: this exact bug already bit this
// codebase once). Option (a) as literally written has the SAME edge case
// at key=0: state0=fmix32(0)=0, so state_0=0+0*WEYL32=0, output_0=
// fmix32(0)=0 -- a single all-zero output word at key=0,j=0 (the
// repo-known "key=0 Degenerate-State All-Zero Defect" pattern). Closed
// here with state0=fmix32(GUARD_KEY^key) instead of fmix32(key) bare.
// GUARD_KEY reuses the already-vetted GUARD_D constant from
// ../../2026-9-4_orbit-init-warmup-fix/candidates/d0_hash.c (chosen via
// SHA-256 scan, popcount=16, distinct from GUARD_L/GUARD_M and both
// fmix32 multiply constants) rather than picking a fresh one.
//
// splitmix32() below (64-bit generator, high-32-bit truncated) is copied
// verbatim from
// ../../2026-9-1_family-productionization/multikey_stream_hashed.c L16-22
// and used ONLY as the --stream-hashed key-ordering TEST HARNESS (matching
// every prior K-small experiment's defect-reproduction driver) -- it is
// NOT part of the weyl_fmix32 candidate itself.
//
// ra_core.c is NOT modified by this file.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define WEYL32 0x9e3779b7u
#define GUARD_KEY 0x17067f32u  // = GUARD_D, ../../2026-9-4_orbit-init-warmup-fix/candidates/d0_hash.c

static inline uint32_t fmix32(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6bu;
    h ^= h >> 13;
    h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}

// Candidate core: produces `rng` output words for a single `key`. No L[]/M[]
// init, no per-round loop-carried registers beyond the running `state_j`.
static void weyl_fmix32_core(uint32_t key, size_t rng, FILE *raw_stream) {
    uint32_t state0 = fmix32(GUARD_KEY ^ key);
    for (size_t j = 0; j < rng; ++j) {
        uint32_t state_j = state0 + (uint32_t)((uint32_t)j * WEYL32);
        uint32_t output_j = fmix32(state_j);
        if (raw_stream) fwrite(&output_j, sizeof(uint32_t), 1, raw_stream);
    }
}

/* splitmix32 -- copied verbatim from
 * ../../2026-9-1_family-productionization/multikey_stream_hashed.c L16-22.
 * NOTE: this is actually SplitMix64 truncated to its high 32 bits (see
 * HANDOVER.md section 0.2) -- kept ONLY as the --stream-hashed
 * key-ordering test harness, NOT part of the weyl_fmix32 candidate. */
static uint32_t splitmix32(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    z = z ^ (z >> 31);
    return (uint32_t)(z >> 32);
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--single") == 0 && argc >= 4) {
        uint32_t key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t rng = (size_t)strtoull(argv[3], NULL, 0);
        weyl_fmix32_core(key, rng, stdout);
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--stream") == 0 && argc >= 5) {
        uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t n = (size_t)strtoull(argv[3], NULL, 0);
        size_t K = (size_t)strtoull(argv[4], NULL, 0);
        if (K == 0 || K > 255) { fprintf(stderr, "K must be in [1,255]\n"); return 1; }
        uint64_t block_idx = 0;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < K ? remaining : K;
            uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
            weyl_fmix32_core(key, block_len, stdout);
            remaining -= block_len;
            ++block_idx;
        }
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--stream-hashed") == 0 && argc >= 5) {
        uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t n = (size_t)strtoull(argv[3], NULL, 0);
        size_t K = (size_t)strtoull(argv[4], NULL, 0);
        if (K == 0 || K > 255) { fprintf(stderr, "K must be in [1,255]\n"); return 1; }
        uint64_t state = (uint64_t)(uint32_t)base_key;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < K ? remaining : K;
            uint32_t key = splitmix32(&state);
            weyl_fmix32_core(key, block_len, stdout);
            remaining -= block_len;
        }
        return 0;
    }
    fprintf(stderr,
        "usage:\n"
        "  %s --single <key> <rng>\n"
        "  %s --stream <base_key> <n> <K>\n"
        "  %s --stream-hashed <base_key> <n> <K>\n",
        argv[0], argv[0], argv[0]);
    return 1;
}
