// diag_prereseed.c
//
// Experiment (user-requested, 2026-09-02): ra_core_singleblock's cycle
// (ra_permutation_cycle_singleblock in ../2026-9-1_family-productionization/
// ra_core.c, lines 214-236) always starts its internal loop at i=255. For
// K=1 (rng=1) the loop runs exactly ONE iteration (i=255, reads M[5]/M[6]
// only via M[(uint8_t)(i+6)]/M[(uint8_t)(i+7)]) then breaks -- output is a
// single-round function of (key, M[5], M[6]) with no accumulated mixing.
// PractRand BCFN fails catastrophically (R=+1069 at 1GB) on multikey K=1
// streams -- see ../2026-9-1_production-candidate-battery/RESULTS.md.
// Proven NOT caused by the key=0 guard fix (control without guard fails
// identically) -- this is a pre-existing cycle-design defect, first
// exercised now because K=1 was never tested before this battery.
//
// User's requested experiment: reverse the order -- reseed/mix M[] BEFORE
// the cycle starts, so cons and M[]'s entropy is already "raised" at the
// very first output word, instead of only at the end of a 255-word block
// like ra_core_orbit does. Does NOT reintroduce L[] (that would defeat the
// whole point of the no-L singleblock fast path) -- the pre-mix pass only
// touches M[] (self-hash), mirroring how ra_reseed folds L into M but with
// no L to fold, and derives a new `cons` the same way ra_reseed derives
// new_cons.
//
// Self-contained diagnostic: copies (does not modify) rot32/fmix32/
// GUARD_L/GUARD_M/ra_hash/ra_init_state_singleblock/
// ra_permutation_cycle_singleblock byte-for-byte from ra_core.c (post
// guard-fix, verified 2026-09-01) so this file can be built and run
// standalone. ra_core.c itself is NOT touched by this experiment.
//
// Two candidate cores in this binary, selectable via CLI, for same-run A/B:
//   - "baseline"   : byte-for-byte ra_core_singleblock as it is in ra_core.c
//                    right now (cons = key, no pre-mix). Included as a
//                    sanity control -- known to fail PractRand BCFN
//                    catastrophically at K=1 (already established).
//   - "prereseed"  : the experiment candidate (cons = pre-mixed hash of M,
//                    computed BEFORE the cycle runs).
//
// CLI:
//   diag_prereseed --stream <baseline|prereseed> <base_key> <n> <block_len>
//       Chains ra_core_singleblock_<variant>(key, block_len, stdout) calls,
//       key derived via the same Weyl/golden-ratio increment (0x9E3779B9)
//       as multikey_stream.c, writes raw uint32 stream to stdout (for
//       piping into PractRand's RNG_test stdin32).
//   diag_prereseed --single <baseline|prereseed> <key> <rng>
//       Single call, prints each output word as decimal to stdout (for
//       edge-case / degenerate-state checks).
//   diag_prereseed --bench <baseline|prereseed> <block_len> <iters>
//       Timing loop: <iters> reinit-and-generate-<block_len>-words cycles,
//       prints ns/word.
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h
//          diag_prereseed.c -o diag_prereseed -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

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

static void ra_hash(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}

static void ra_init_state_singleblock(uint32_t *M, uint32_t key) {
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32(m_val, r);
    }
}

static void ra_permutation_cycle_singleblock(uint32_t cons, size_t it,
                                              const uint32_t *M,
                                              uint64_t *count,
                                              FILE *raw_stream) {
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
    }
}

// ---- baseline (= production ra_core_singleblock, unchanged) ----
static uint32_t ra_core_singleblock_baseline(uint32_t key, size_t rng,
                                              FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_singleblock(cons, 0, M, &count, raw_stream);
    return cons;
}

// ---- experiment candidate: pre-mix M[] and derive cons BEFORE the cycle
// runs, instead of using raw key as cons with no prior mixing. Mirrors
// ra_reseed's tmp8-fold-and-derive-new_cons shape, but self-hash only (no
// L[] to fold in -- singleblock stays L-free by design). ----
static uint32_t ra_prereseed_singleblock(uint32_t *M) {
    uint32_t tmp8[8];
    ra_hash(M, tmp8);
    for (int e = 0; e < 8; ++e) {
        int base = e * 32;
        for (int j = 0; j < 32; ++j) M[base + j] ^= tmp8[e];
    }
    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

static uint32_t ra_core_singleblock_prereseed(uint32_t key, size_t rng,
                                               FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) { fprintf(stderr, "rng>255 out of scope\n"); abort(); }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    ra_init_state_singleblock(M, key);
    uint32_t cons = ra_prereseed_singleblock(M);
    ra_permutation_cycle_singleblock(cons, 0, M, &count, raw_stream);
    return cons;
}

typedef uint32_t (*core_fn_t)(uint32_t key, size_t rng, FILE *raw_stream);

static core_fn_t pick_variant(const char *name) {
    if (strcmp(name, "baseline") == 0) return ra_core_singleblock_baseline;
    if (strcmp(name, "prereseed") == 0) return ra_core_singleblock_prereseed;
    fprintf(stderr, "unknown variant '%s' (want baseline|prereseed)\n", name);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--stream") == 0 && argc >= 6) {
        core_fn_t fn = pick_variant(argv[2]);
        uint32_t base_key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t n = (size_t)strtoull(argv[4], NULL, 0);
        size_t block_len = (size_t)strtoull(argv[5], NULL, 0);
        size_t emitted = 0;
        uint64_t ki = 0;
        while (emitted < n) {
            uint32_t key = base_key + (uint32_t)(ki * 0x9E3779B9u);
            size_t this_block = block_len;
            if (emitted + this_block > n) this_block = n - emitted;
            fn(key, this_block, stdout);
            emitted += this_block;
            ++ki;
        }
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--single") == 0 && argc >= 5) {
        core_fn_t fn = pick_variant(argv[2]);
        uint32_t key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t rng = (size_t)strtoull(argv[4], NULL, 0);
        fn(key, rng, stdout);
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--bench") == 0 && argc >= 5) {
        core_fn_t fn = pick_variant(argv[2]);
        size_t block_len = (size_t)strtoull(argv[3], NULL, 0);
        size_t iters = (size_t)strtoull(argv[4], NULL, 0);
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        volatile uint32_t sink = 0;
        for (size_t k = 0; k < iters; ++k) {
            uint32_t key = 0x12345678u + (uint32_t)(k * 0x9E3779B9u);
            sink ^= fn(key, block_len, NULL);
        }
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double secs = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
        double words = (double)iters * (double)block_len;
        fprintf(stderr, "sink=%u\n", sink);
        printf("%s block_len=%zu iters=%zu total_words=%.0f secs=%.6f ns_per_word=%.4f\n",
               argv[2], block_len, iters, words, secs, secs * 1e9 / words);
        return 0;
    }
    fprintf(stderr,
        "usage:\n"
        "  %s --stream <baseline|prereseed> <base_key> <n> <block_len>\n"
        "  %s --single <baseline|prereseed> <key> <rng>\n"
        "  %s --bench <baseline|prereseed> <block_len> <iters>\n",
        argv[0], argv[0], argv[0]);
    return 1;
}
