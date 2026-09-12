// weyl_triple32.c -- Candidate B, orbit-splitmix-finalizer-redesign experiment
//
// Same architecture as weyl_fmix32.c (Weyl-sequence 32-bit-ADD state advance
// + finalizer applied fresh to every state), swapping the finalizer for
// `triple32` (Chris Wellons, hash-prospector project -- 4 multiplies,
// stronger avalanche per publication: exact bias 0.020888578919738908 vs
// fmix32/MurmurHash3's, indistinguishable from a random 32-bit permutation).
// Only run because Candidate A (weyl_fmix32) FAILED Tier 1 decisively (see
// ../RESULTS.md): FAIL at K=1+Weyl-ordering @1GB (FPF-14+6/16) AND at
// K=255 both orderings @512MB (DC6-9x1Bytes-1, a NEW failure signature
// never seen in any of the 8 prior orbit K-small fix attempts) -- pointing
// at fmix32's 3-multiply finalizer being insufficiently strong for this
// purely-linear Weyl-sequence input at 32-bit width, exactly the risk
// flagged in HANDOVER.md section 0.2.
//
// triple32 constants VERIFIED 2026-09-05 against the authoritative source
// (raw.githubusercontent.com/skeeto/hash-prospector/master/README.md,
// L160-169) before use, per HANDOVER.md section 4's explicit warning and
// feedback_verify_external_baselines memory discipline -- NOT copied from
// memory/the handover's own transcription without re-checking.
//
// WEYL32, GUARD_KEY, CLI structure (--single/--stream/--stream-hashed),
// and splitmix32() test-harness helper: identical rationale to
// weyl_fmix32.c, see that file's header comment for full provenance.
//
// ra_core.c is NOT modified by this file.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define WEYL32 0x9e3779b7u
#define GUARD_KEY 0x17067f32u  // = GUARD_D, ../../2026-9-4_orbit-init-warmup-fix/candidates/d0_hash.c

// triple32 -- verbatim from hash-prospector README.md (verified 2026-09-05,
// see header comment). triple32(0) == 0, same weak-key shape as fmix32(0)
// == 0, so GUARD_KEY is applied here too (see weyl_fmix32.c header comment).
static inline uint32_t triple32(uint32_t x) {
    x ^= x >> 17;
    x *= 0xed5ad4bbu;
    x ^= x >> 11;
    x *= 0xac4c1b51u;
    x ^= x >> 15;
    x *= 0x31848babu;
    x ^= x >> 14;
    return x;
}

static void weyl_triple32_core(uint32_t key, size_t rng, FILE *raw_stream) {
    uint32_t state0 = triple32(GUARD_KEY ^ key);
    for (size_t j = 0; j < rng; ++j) {
        uint32_t state_j = state0 + (uint32_t)((uint32_t)j * WEYL32);
        uint32_t output_j = triple32(state_j);
        if (raw_stream) fwrite(&output_j, sizeof(uint32_t), 1, raw_stream);
    }
}

/* splitmix32 -- copied verbatim from
 * ../../2026-9-1_family-productionization/multikey_stream_hashed.c L16-22.
 * NOTE: this is actually SplitMix64 truncated to its high 32 bits (see
 * HANDOVER.md section 0.2) -- kept ONLY as the --stream-hashed
 * key-ordering test harness, NOT part of the weyl_triple32 candidate. */
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
        weyl_triple32_core(key, rng, stdout);
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
            weyl_triple32_core(key, block_len, stdout);
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
            weyl_triple32_core(key, block_len, stdout);
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
