// driver_main_template.c -- shared CLI driver for tools/battery/.
//
// Reusable `main()` so a new candidate .c file never has to hand-roll argv
// parsing / key-stepping again. Convention lifted verbatim (formulas
// unchanged) from
// ../../experiments/2026-9-5_orbit-d0-raw-cons-fix/candidates/d0_raw_cons.c
// L74-82,163-208 -- that file independently converged on this exact CLI
// shape as the most-refined version of the pattern used across
// experiments/2026-9-4_orbit-*/ and experiments/2026-9-1_family-productionization/
// multikey_stream_orbit_k1.c / multikey_stream_hashed.c.
//
// 2026-09-06 (addr_hi): CORE_FN/CORE_INIT gained a second 32-bit parameter,
// `addr_hi`, mirroring ra_core.c's `ra_core_orbit`/`ra_core_singleblock`
// signature after the Candidate D widening (see
// ../../experiments/2026-9-6_orbit-address-widen-64bit-promotion/RESULTS.md).
// This is a breaking contract change for NEW candidates using this template
// -- existing frozen candidate files under experiments/ that used the old
// 3-arg contract are never rebuilt against this template, so they are
// unaffected. addr_hi must be varied INDEPENDENTLY from key (not left at a
// constant), matching
// ../../experiments/2026-9-5_orbit-address-widen-64bit/multikey_stream_addr_hi.c
// -- a constant addr_hi collapses back to the old 32-bit-key-only domain and
// does not exercise the configuration that closed the K=1+hashed-key gate.
//
// Two supported conventions -- pick ONE per candidate file:
//
// (A) Single-core: define CORE_FN to a function (already declared or
// defined, with this exact signature) before including this file:
//
//   static uint32_t my_candidate_core(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream) {
//       ... your edited ra_permutation_cycle_orbit/singleblock here ...
//   }
//   #define CORE_FN my_candidate_core
//   #include "../../tools/battery/driver_main_template.c"
//
// or, to test the unmodified production core by linking against a
// nomain object of experiments/2026-9-1_family-productionization/ra_core.c
// (compiled with -Dmain=ra_core_unused_main):
//
//   extern uint32_t ra_core_orbit(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream);
//   #define CORE_FN ra_core_orbit
//   #include "../../tools/battery/driver_main_template.c"
//
// CORE_FN must already be visible (declared or defined) with signature
// `uint32_t CORE_FN(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream)`
// at the point of this #include -- this template does not forward-declare
// it, to avoid static/non-static redeclaration edge cases. Binary CLI takes
// no leading core name: `<binary> --single ...` etc.
//
// (B) Dual-core (orbit+singleblock sharing one edited transform): define
// CORE_INIT and CORE_TRANSFORM instead of CORE_FN. This is for the common
// case where you're editing the per-round transform or the init formula and
// want BOTH ra_core_orbit-shape and ra_core_singleblock-shape behavior to
// come from that ONE edit, instead of hand-copying it into two functions
// that can drift apart. Rationale: in ra_core.c, ra_permutation_cycle_orbit
// and ra_permutation_cycle_singleblock are byte-identical except that orbit
// also does an L[i]<->L[d] swap right after the count<=1 check (singleblock
// skips it entirely) -- and L[] is write-only within one block (only
// ra_reseed() reads it, at the NEXT block boundary), which is exactly why
// ORBIT_KAT_CHECKSUMS == SINGLEBLOCK_KAT_CHECKSUMS byte-for-byte in ra_core.c
// for every rng in [1,255]. So one transform, parameterized on L being
// NULL (singleblock) or non-NULL (orbit), covers both:
//
//   static void my_init(uint32_t *L /* NULL for singleblock */,
//                        uint32_t *M, uint32_t key, uint32_t addr_hi) {
//       ... compute M[i] (and L[i] if L != NULL) for i in [0,256) ...
//   }
//   static void my_transform(uint32_t cons, size_t it, const uint32_t *M,
//                             uint32_t *L /* NULL for singleblock */,
//                             uint64_t *count, FILE *raw_stream) {
//       ... a/b/c/d/o computation, fwrite(&c, ...) ...
//       ... if (*count <= 1) break-equivalent (caller loop handles it) ...
//       if (L) { /* swap L[i]/L[d] -- orbit only */ }
//       ... rolling-o update ...
//   }
//   #define CORE_INIT my_init
//   #define CORE_TRANSFORM my_transform
//   #include "../../tools/battery/driver_main_template.c"
//
// Note CORE_TRANSFORM does NOT take addr_hi -- matching ra_core.c, where
// addr_hi only feeds the init-time keyterm and is deliberately excluded from
// the per-round rotation/transform (see the addr_hi promotion RESULTS.md
// above for why: folding it into rotation failed Gap-16 for two earlier
// candidates).
//
// This mode supplies rot32/ra_hash/ra_reseed (canonical, verbatim from
// ra_core.c) and two wrapper functions (dual_core_orbit / dual_core_
// singleblock, mirroring ra_core_orbit/ra_core_singleblock exactly) so you
// never touch the loop-vs-single-shot or reseed-vs-no-reseed logic -- only
// CORE_INIT/CORE_TRANSFORM. Override the canonical helpers by #define-ing
// ROT32_FN/RA_HASH_FN/RA_RESEED_FN to your own function name before the
// #include, if you deliberately want to test a change there too (out of
// scope for most experiments -- ra_reseed/ra_hash were ruled out as the
// K=1 defect's location, see experiments/2026-9-4_orbit-key-avalanche-diagnosis/).
// Binary CLI takes a leading core name: `<binary> orbit --single ...` /
// `<binary> singleblock --stream ...`.
//
// Modes (same three for both conventions, `<binary>` vs `<binary> <core>`):
//   --single <key> <addr_hi> <rng>
//       Continuous: one call, one (key, addr_hi) pair, no periodic reinit.
//       This is what tools/battery/ calls "K=N continuous" -- NOT the same
//       as ra_core.c's own `--stream <core> <key> <addr_hi> <n>` naming
//       (that binary predates this convention; same concept, different flag
//       name). Only meaningful for cores with no rng cap (orbit). A core
//       that hard-aborts above rng=255 (singleblock) is rejected here with
//       a clean error (not a crash) -- and
//       tools/battery/practrand_gate.py skips --continuous for
//       --core singleblock before ever invoking this mode.
//   --stream <base_key> <base_addr_hi> <n> <K>
//       Multistream, Weyl-ordered step: reinit every K words (K in [1,255]),
//       key = base_key + block_idx*0x9E3779B9, addr_hi = base_addr_hi +
//       block_idx*0x85EBCA6B -- an INDEPENDENT delta from key's, so the two
//       never move in lockstep.
//   --stream-hashed <base_key> <base_addr_hi> <n> <K>
//       Same reinit-every-K-words pattern, but key AND addr_hi each come
//       from their own independent splitmix32 chain instead of a linear
//       Weyl step -- this is the ordering that has historically exposed the
//       K=1 structural defect the weyl ordering hides (see
//       ../../experiments/2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md
//       and ../../experiments/2026-9-5_orbit-address-widen-64bit/RESULTS.md
//       for why addr_hi must ALSO be independently hashed here, not held
//       constant). Never assume a candidate is clean from --stream alone.
//
// K is capped at [1,255] for both stream modes, matching the repo-wide
// "one block <= 255" convention (ra_core_singleblock's hard limit, kept
// for orbit too so both cores can share this one driver).

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BATTERY_WEYL_DELTA_KEY 0x9E3779B9u
#define BATTERY_WEYL_DELTA_ADDR 0x85EBCA6Bu

/* splitmix32 -- verbatim from
 * ../../experiments/2026-9-1_family-productionization/multikey_stream_hashed.c
 * L16-22, also used by d0_raw_cons.c L74-82. */
static uint32_t battery_splitmix32(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    z = z ^ (z >> 31);
    return (uint32_t)(z >> 32);
}

/* Shared argv parser for --single/--stream/--stream-hashed, used by both
 * conventions. `argv[off]` is the first mode flag (offset 1 for single-core:
 * `<binary> --single ...`; offset 2 for dual-core: `<binary> <core> --single ...`).
 * `max_rng` < 0 means no cap (orbit); >= 0 rejects --single/--stream block
 * lengths above it with a clean error instead of calling core_fn at all. */
static int battery_run_cli(uint32_t (*core_fn)(uint32_t, uint32_t, size_t, FILE *),
                            long max_rng, int argc, char **argv, int off) {
    if (argc >= off + 4 && strcmp(argv[off], "--single") == 0) {
        uint32_t key = (uint32_t)strtoul(argv[off + 1], NULL, 0);
        uint32_t addr_hi = (uint32_t)strtoul(argv[off + 2], NULL, 0);
        size_t rng = (size_t)strtoull(argv[off + 3], NULL, 0);
        if (max_rng >= 0 && (long)rng > max_rng) {
            fprintf(stderr, "--single: rng=%zu exceeds this core's max_rng=%ld\n", rng, max_rng);
            return 1;
        }
        core_fn(key, addr_hi, rng, stdout);
        return 0;
    }

    if (argc >= off + 5 && (strcmp(argv[off], "--stream") == 0 || strcmp(argv[off], "--stream-hashed") == 0)) {
        int hashed = strcmp(argv[off], "--stream-hashed") == 0;
        uint32_t base_key = (uint32_t)strtoul(argv[off + 1], NULL, 0);
        uint32_t base_addr = (uint32_t)strtoul(argv[off + 2], NULL, 0);
        size_t n = (size_t)strtoull(argv[off + 3], NULL, 0);
        size_t K = (size_t)strtoull(argv[off + 4], NULL, 0);
        if (K == 0 || K > 255) { fprintf(stderr, "K must be in [1,255]\n"); return 1; }
        if (max_rng >= 0 && (long)K > max_rng) {
            fprintf(stderr, "--stream: K=%zu exceeds this core's max_rng=%ld\n", K, max_rng);
            return 1;
        }

        uint64_t block_idx = 0;
        uint64_t sm_key = 0xC0FFEE1234ULL ^ base_key;
        uint64_t sm_addr = 0xDEADBEEFCAFEULL ^ base_addr;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < K ? remaining : K;
            uint32_t key, addr_hi;
            if (hashed) {
                key = battery_splitmix32(&sm_key);
                addr_hi = battery_splitmix32(&sm_addr);
            } else {
                key = base_key + (uint32_t)(block_idx * BATTERY_WEYL_DELTA_KEY);
                addr_hi = base_addr + (uint32_t)(block_idx * BATTERY_WEYL_DELTA_ADDR);
            }
            core_fn(key, addr_hi, block_len, stdout);
            remaining -= block_len;
            ++block_idx;
        }
        return 0;
    }

    fprintf(stderr,
        "usage (offset %d):\n"
        "  ... --single <key> <addr_hi> <rng>\n"
        "  ... --stream <base_key> <base_addr_hi> <n> <K>\n"
        "  ... --stream-hashed <base_key> <base_addr_hi> <n> <K>\n",
        off);
    return 1;
}

#if defined(CORE_FN)
/* ------------------------- (A) single-core mode ------------------------- */

int main(int argc, char **argv) {
    return battery_run_cli(CORE_FN, -1, argc, argv, 1);
}

#elif defined(CORE_INIT) && defined(CORE_TRANSFORM)
/* -------------------------- (B) dual-core mode -------------------------- */

#include <stdalign.h>

#ifndef ROT32_FN
static inline uint32_t battery_rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}
#define ROT32_FN battery_rot32
#endif

#ifndef RA_HASH_FN
static void battery_ra_hash(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}
#define RA_HASH_FN battery_ra_hash
#endif

#ifndef RA_RESEED_FN
static uint32_t battery_ra_reseed(uint32_t *M, const uint32_t *L) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];
    uint32_t tmp8[8];
    RA_HASH_FN(M, tmp8);
    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}
#define RA_RESEED_FN battery_ra_reseed
#endif

/* User-supplied: void CORE_INIT(uint32_t *L, uint32_t *M, uint32_t key, uint32_t addr_hi);
 * void CORE_TRANSFORM(uint32_t cons, size_t it, const uint32_t *M,
 *                      uint32_t *L, uint64_t *count, FILE *raw_stream);
 * Must already be visible (declared/defined) above this #include. */

static uint32_t dual_core_orbit(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;
    uint32_t cons = key;
    CORE_INIT(L, M, key, addr_hi);
    for (size_t it = 0; it < iteration; ++it) {
        CORE_TRANSFORM(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = RA_RESEED_FN(M, L);
    }
    return cons;
}

static uint32_t dual_core_singleblock(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) {
        fprintf(stderr, "singleblock: rng=%zu > 255 out of scope (no-reseed "
                         "fast path valid only for rng in [1,255])\n", rng);
        abort();
    }
    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;
    CORE_INIT(NULL, M, key, addr_hi);
    CORE_TRANSFORM(cons, /*it=*/0, M, NULL, &count, raw_stream);
    return cons;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s {orbit|singleblock} --single|--stream|--stream-hashed ...\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "orbit") == 0) {
        return battery_run_cli(dual_core_orbit, -1, argc, argv, 2);
    }
    if (strcmp(argv[1], "singleblock") == 0) {
        return battery_run_cli(dual_core_singleblock, 255, argc, argv, 2);
    }
    fprintf(stderr, "unknown core '%s', expected orbit|singleblock\n", argv[1]);
    return 1;
}

#else
#error "define CORE_FN (single-core) or both CORE_INIT and CORE_TRANSFORM (dual-core) before including driver_main_template.c"
#endif
