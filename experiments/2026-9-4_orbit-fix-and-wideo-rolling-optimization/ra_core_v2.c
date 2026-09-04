// ra_core_v2.c
// Base: byte-for-byte copy of ../2026-9-1_family-productionization/ra_core.c
// (which is itself byte-identical to
// ../2026-9-3_singleblock-end-diffusion-refinement/ra_core_baseline.c's body).
// Two changes on top of that base, both scoped to the two
// ra_permutation_cycle_* functions ONLY -- ra_init_state_orbit,
// ra_init_state_singleblock, ra_reseed, ra_hash, guard constants, and CLI
// dispatch are untouched. See HANDOVER.md in this folder for full
// provenance/derivation/decision context.
//
// CHANGE (a) -- K-small defect fix ported to orbit (2026-09-04):
//   `ra_permutation_cycle_orbit` still had the narrow 2-tap `o`
//   (M[i+6]<<6 ^ M[i+7]<<7) with no finalizer -- byte-identical to the
//   PRE-fix `ra_permutation_cycle_singleblock` formula that was proven to
//   fail PractRand (BCFN-style defect) below K=96
//   (../2026-9-2_singleblock-k-threshold-characterization/RESULTS.md).
//   Because `ra_init_state_orbit`'s M[] path is byte-identical to
//   `ra_init_state_singleblock`'s, and L[] never influences output for
//   rng<=255 in a single call (the L-swap's only reader, ra_reseed, is
//   unreachable before 255 words), `ra_core_orbit(key, rng<=255, ...)`
//   reproduces that exact pre-fix defective formula -- a real, previously
//   unflagged gap for anyone using orbit in a "multistream" pattern
//   (many short/K=1 calls with fresh keys) instead of its intended long
//   continuous stream usage. Fixed here the same way singleblock was
//   fixed: widen `o` to all 8 M[] taps and add the `c ^= c >> 17u`
//   finalizer (candidate `w8_f10_i0`).
//   STATUS: NOT YET STATISTICALLY RE-VALIDATED. PractRand/dieharder for
//   this new orbit formula at small K is deferred to a separate session.
//   Do not promote to src/ or to the canonical
//   ../2026-9-1_family-productionization/ra_core.c until that battery is
//   done and PASSes.
//
// CHANGE (b) -- rolling-register computation of wide-`o` (2026-09-04),
//   applied to BOTH ra_permutation_cycle_singleblock AND
//   ra_permutation_cycle_orbit:
//   `o` was recomputed from all 8 M[] taps from scratch every iteration,
//   even though consecutive iterations' 8-element windows share 7
//   elements (i decreases by 1 each step). Because XOR and constant
//   left-shift are both linear over GF(2) ((a^b)<<k == (a<<k)^(b<<k)),
//   the following is an exact algebraic IDENTITY, not an approximation:
//     o(i-1) = M[(i-1) mod 256] ^ ( (o(i) ^ (M[(i+7) mod 256] << 7)) << 1 )
//   i.e. o(i-1) is derivable from o(i) by: XOR out the term leaving the
//   window (M[(i+7) mod 256]<<7, cached from the previous step), shift
//   left by 1, XOR in the one new term entering the window
//   (M[(i-1) mod 256]<<0). This replaces 8 array reads + 8 shifts + 7 XORs
//   per iteration with 2 reads + 2 shifts + 2 XORs. The first iteration
//   (i=255) still computes `o` naively once as the rolling basis.
//   VERIFIED: (1) algebraically by hand with a reduced-width window: (2)
//   with a standalone differential test (20 trials x 255 i-values x
//   random M[], comparing rolling vs from-scratch -- 0 mismatches, see
//   HANDOVER.md); (3) by this file's `validate` command reproducing the
//   EXISTING SINGLEBLOCK_KAT_CHECKSUMS table (from
//   ../2026-9-1_family-productionization/ra_core.c, unmodified) with 0
//   mismatches -- proof the rolling singleblock is bit-exact against the
//   already fully-validated (PractRand/dieharder/collision-scan) naive
//   w8_f10_i0 formula. Being a bit-exact identity, this change needs no
//   separate statistical re-validation of its own.
//   Orbit's own KAT (ORBIT_KAT_CHECKSUMS below) was captured from THIS
//   file's rolling implementation and cross-checked against a naive
//   (non-rolling) reference build during development -- see HANDOVER.md.
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h
//          ra_core_v2.c -o ra_core_v2 -Wall -Wextra
//
// Copyright (c) 2025 Hamas A. Rahman (derivative research variant)
// Licensed under CC BY-NC-SA 4.0, matching the original this derives from.

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

// key=0 weak-key guard (2026-09-01, ../2026-9-1_keyzero-guard-fix/):
// popcount==16 each, distinct from each other and from the 4 existing
// constants in this init path (0x9e3779b7 popcount=21, 0x06a0dd9b
// popcount=15, fmix32's 0x85ebca6b popcount=18, 0xc2b2ae35 popcount=16).
#define GUARD_L 0x38916df4u
#define GUARD_M 0x6c26fc92u

// hash_access=sequential, HASH_SELFIDX off, HASH_DEPTH fixed at 32.
static void ra_hash(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}

static uint32_t ra_reseed(uint32_t *M, const uint32_t *L) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];

    uint32_t tmp8[8];
    ra_hash(M, tmp8);

    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

// ----------------------------------------------------------------------
// Mode orbit: addressable, unbounded stream (any rng length, reseeds every
// 255 words). Init formula identical to winner_wired_addressable.c.
// ----------------------------------------------------------------------

// Mix-init fix candidate 4 (multiply-combined per-key hashed term): hash
// only the key-dependent term once per reinit (key/constant don't change
// within a reinit), combined with the per-index step by MULTIPLY so the
// per-key step size itself becomes pseudorandom/key-unique, not just a
// shared shape shifted by an offset. Rotation (r = key^i, raw key not
// hashed) unchanged from the previous formula. See header comment for
// provenance/verification.
static void ra_init_state_orbit(uint32_t *L, uint32_t *M, uint32_t key) {
    uint32_t keyterm_l = fmix32(GUARD_L ^ (0x9e3779b7u * key));
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * 0x9e3779b7u) * keyterm_l;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

// One full 255-step permutation cycle, orbit mode (includes the L[i]<->L[d]
// swap that feeds ra_reseed on the next cycle).
//
// CHANGE (a): wide 8-tap `o` + XORSHIFT(17) finalizer on `c` -- K-small
// defect fix ported from ra_permutation_cycle_singleblock, candidate
// w8_f10_i0. See file header. NOT YET statistically re-validated at small
// K for orbit specifically -- see file header STATUS note.
//
// CHANGE (b): `o` computed via rolling register (o_term7 tracks the term
// leaving the window) instead of recomputing all 8 taps from scratch each
// iteration. Bit-exact identity, see file header CHANGE (b) note.
static void ra_permutation_cycle_orbit(uint32_t cons, size_t it,
                                        const uint32_t *M, uint32_t *L,
                                        uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    uint32_t o = (M[(uint8_t)(255 + 0)] << 0) ^ (M[(uint8_t)(255 + 1)] << 1) ^
                 (M[(uint8_t)(255 + 2)] << 2) ^ (M[(uint8_t)(255 + 3)] << 3) ^
                 (M[(uint8_t)(255 + 4)] << 4) ^ (M[(uint8_t)(255 + 5)] << 5) ^
                 (M[(uint8_t)(255 + 6)] << 6) ^ (M[(uint8_t)(255 + 7)] << 7);
    uint32_t o_term7 = M[(uint8_t)(255 + 7)] << 7;

    for (uint32_t i = 255; i > 0; --i) {
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 17u;

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & 0xFFu;

        if (*count <= 1) break;
        --(*count);

        uint32_t tmp = L[i];
        L[i] = L[d];
        L[d] = tmp;

        if (i > 1) {
            uint32_t new_term = M[(uint8_t)(i - 1)];
            uint32_t o_next = new_term ^ ((o ^ o_term7) << 1);
            o_term7 = M[(uint8_t)(i + 6)] << 7;
            o = o_next;
        }
    }
}

uint32_t ra_core_orbit(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_state_orbit(L, M, key);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_orbit(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

// ----------------------------------------------------------------------
// Mode singleblock: no-L fast path, valid ONLY for rng in [1,255]. L[]
// dibuang sepenuhnya (bukan cuma dilewati) karena satu-satunya pembaca
// isi L[] adalah ra_reseed, dan ra_reseed tidak pernah terpanggil untuk
// rng<=255 (iteration==1, loop langsung return sebelum reseed) -- lihat
// HANDOVER_TAHAP6.md untuk dead-code proof lengkapnya.
// ----------------------------------------------------------------------

// M-only analog of ra_init_state_orbit's fix, for the no-L singleblock mode.
static void ra_init_state_singleblock(uint32_t *M, uint32_t key) {
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32(m_val, r);
    }
}

// CHANGE (b) ONLY (this function already had CHANGE (a)'s formula since
// 2026-09-03): `o` computed via rolling register instead of recomputing
// all 8 taps from scratch each iteration. See file header CHANGE (b).
static void ra_permutation_cycle_singleblock(uint32_t cons, size_t it,
                                              const uint32_t *M,
                                              uint64_t *count,
                                              FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    uint32_t o = (M[(uint8_t)(255 + 0)] << 0) ^ (M[(uint8_t)(255 + 1)] << 1) ^
                 (M[(uint8_t)(255 + 2)] << 2) ^ (M[(uint8_t)(255 + 3)] << 3) ^
                 (M[(uint8_t)(255 + 4)] << 4) ^ (M[(uint8_t)(255 + 5)] << 5) ^
                 (M[(uint8_t)(255 + 6)] << 6) ^ (M[(uint8_t)(255 + 7)] << 7);
    uint32_t o_term7 = M[(uint8_t)(255 + 7)] << 7;

    for (uint32_t i = 255; i > 0; --i) {
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 17u;

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & 0xFFu;

        if (*count <= 1) break;
        --(*count);
        // No L[] swap here -- L's only functional reader (ra_reseed) is
        // unreachable for rng<=255, see ra_core_singleblock's guard below.

        if (i > 1) {
            uint32_t new_term = M[(uint8_t)(i - 1)];
            uint32_t o_next = new_term ^ ((o ^ o_term7) << 1);
            o_term7 = M[(uint8_t)(i + 6)] << 7;
            o = o_next;
        }
    }
}

uint32_t ra_core_singleblock(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) {
        fprintf(stderr,
            "ra_core_singleblock: rng=%zu > 255 out of scope (no-reseed "
            "fast path valid only for rng in [1,255]); use ra_core_orbit.\n",
            rng);
        abort();
    }

    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key;

    ra_init_state_singleblock(M, key);
    ra_permutation_cycle_singleblock(cons, /*it=*/0, M, &count, raw_stream);
    return cons; // == key always for rng<=255 (cons is never reassigned
                 // without ra_reseed).
}

// ----------------------------------------------------------------------
// CLI dispatch -- pola CORES[]/find_core dari tahap6_bench.c.
// ----------------------------------------------------------------------

typedef uint32_t (*core_fn_t)(uint32_t key, size_t rng, FILE *raw_stream);

typedef struct { const char *name; core_fn_t fn; long max_rng; } core_entry_t;

static const core_entry_t CORES[] = {
    { "orbit", ra_core_orbit, -1 },        // -1 == no cap
    { "singleblock", ra_core_singleblock, 255 },
};

static const core_entry_t *find_core(const char *name) {
    for (size_t i = 0; i < sizeof(CORES) / sizeof(CORES[0]); ++i) {
        if (strcmp(CORES[i].name, name) == 0) return &CORES[i];
    }
    fprintf(stderr, "Unknown core '%s'. Known: ", name);
    for (size_t i = 0; i < sizeof(CORES) / sizeof(CORES[0]); ++i) {
        fprintf(stderr, "%s ", CORES[i].name);
    }
    fprintf(stderr, "\n");
    exit(1);
}

// ----------------------------------------------------------------------
// validate: known-answer-test (KAT) checksum check, now for BOTH cores.
//
// `buf` is deliberately 1 word larger than the max write (255 words):
// glibc's fmemopen("wb") writes a trailing NUL right after the last byte
// written, and if the buffer is exactly write-sized that NUL overwrites
// the last byte instead of landing past it -- on little-endian this would
// clobber the MSB of the last uint32_t written every time. The spare word
// gives that NUL somewhere harmless to land (found 2026-09-03, see
// ../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md).
// ----------------------------------------------------------------------

static void validate_keys(uint32_t *keys, int *nkeys_out) {
    int nkeys = 0;
    keys[nkeys++] = 0u;
    keys[nkeys++] = 0xFFFFFFFFu;
    static const uint32_t extra_keys[5] = {
        0xDEADBEEFu, 0x12345678u, 0xCAFEBABEu, 0x9E3779B9u, 0x7F4A7C15u,
    };
    for (int i = 0; i < 5; ++i) keys[nkeys++] = extra_keys[i];
    for (uint32_t k = 0; k < 32; ++k) keys[nkeys++] = k;
    *nkeys_out = nkeys;
}

/* Folds a core's output stream for n=1..255 into one checksum per key.
 * Generalized from ra_core.c's checksum_key() to take any core_fn_t, so
 * the same KAT machinery covers both singleblock and orbit (the K<=255
 * pre-reseed window, so this exercises the same per-round transform for
 * both). Rotate+add avalanches any single-word change into the whole
 * checksum. */
static uint32_t checksum_key_core(core_fn_t fn, uint32_t key) {
    uint32_t chk = 0;
    uint32_t buf[256];
    for (size_t n = 1; n <= 255; ++n) {
        FILE *fs = fmemopen(buf, n * sizeof(uint32_t) + 1, "wb");
        fn(key, n, fs);
        fclose(fs);
        for (size_t j = 0; j < n; ++j) {
            chk = rot32(chk ^ buf[j], 7) + 0x9E3779B9u;
        }
    }
    return chk;
}

/* Golden checksums for validate_keys()'s 39 keys, in the same order.
 * UNCHANGED from ../2026-9-1_family-productionization/ra_core.c -- this
 * is the regression proof that CHANGE (b)'s rolling `o` for singleblock
 * is bit-exact against the already fully-validated (16GB PractRand across
 * K in {1,2,4,8,16,32,64,96}, dieharder 0 FAILED at K=1/K=96) naive
 * w8_f10_i0 formula. If this table ever needs to change, the rolling
 * change introduced a real behavioral difference -- STOP and investigate,
 * do not just regenerate the table. */
static const uint32_t SINGLEBLOCK_KAT_CHECKSUMS[39] = {
    0xee5a0763u, 0x299f7345u, 0x165b620au, 0x41b9b933u, 0xf6613981u, 0x6a03bbf6u,
    0xf8975230u, 0xee5a0763u, 0x1a78795cu, 0x89b5301cu, 0x787bd47cu, 0x31b981d8u,
    0x0ca2c44au, 0xedc2a549u, 0x509112dau, 0xa1f89116u, 0x274c5a70u, 0x0ba07d7bu,
    0x280d1cb1u, 0xfb4a5ec1u, 0xf7e58e11u, 0x291396c2u, 0x1d380914u, 0x8f21fc57u,
    0xd10f68c3u, 0x0c51c5b0u, 0xb734e7eeu, 0x0f2dbf47u, 0xd8abf450u, 0xbb9422e4u,
    0x795fb18fu, 0xc1bced6eu, 0x34b16a2du, 0x7906dcfbu, 0x5a0c615cu, 0x06dfab09u,
    0xa0cfe6c2u, 0x90cf2a0cu, 0x3cb34dfau,
};

/* Golden checksums for orbit, K<=255 pre-reseed window, same 39 keys and
 * same order as SINGLEBLOCK_KAT_CHECKSUMS. Captured from THIS file's
 * ra_core_orbit (CHANGE (a) formula + CHANGE (b) rolling `o`) via
 * `checksum-gen orbit`, and independently cross-checked against a
 * standalone non-rolling reference build of the same CHANGE (a) formula
 * (built separately, NOT sharing any code with ra_core_v2.c) -- all 39
 * values matched exactly, 0 mismatches. See HANDOVER.md.
 *
 * These values are IDENTICAL to SINGLEBLOCK_KAT_CHECKSUMS above. This is
 * expected, not a copy-paste bug: orbit's K<=255 window uses the same
 * M[]-init path, the same `cons=key`/`it=0` start state, and (after
 * CHANGE (a)) the same per-round cycle transform as singleblock -- L[]
 * never influences output before the first reseed. This is a
 * regression-guard KAT only (catches this file's cycle transform
 * silently changing later); it is NOT a statistical validation --
 * PractRand/dieharder for orbit K-small is deferred, see file header
 * STATUS note. */
static const uint32_t ORBIT_KAT_CHECKSUMS[39] = {
    0xee5a0763u, 0x299f7345u, 0x165b620au, 0x41b9b933u, 0xf6613981u, 0x6a03bbf6u,
    0xf8975230u, 0xee5a0763u, 0x1a78795cu, 0x89b5301cu, 0x787bd47cu, 0x31b981d8u,
    0x0ca2c44au, 0xedc2a549u, 0x509112dau, 0xa1f89116u, 0x274c5a70u, 0x0ba07d7bu,
    0x280d1cb1u, 0xfb4a5ec1u, 0xf7e58e11u, 0x291396c2u, 0x1d380914u, 0x8f21fc57u,
    0xd10f68c3u, 0x0c51c5b0u, 0xb734e7eeu, 0x0f2dbf47u, 0xd8abf450u, 0xbb9422e4u,
    0x795fb18fu, 0xc1bced6eu, 0x34b16a2du, 0x7906dcfbu, 0x5a0c615cu, 0x06dfab09u,
    0xa0cfe6c2u, 0x90cf2a0cu, 0x3cb34dfau,
};

static int run_validate(void) {
    uint32_t keys[2 + 5 + 32];
    int nkeys = 0;
    validate_keys(keys, &nkeys);

    long sb_mismatches = 0;
    for (int ki = 0; ki < nkeys; ++ki) {
        uint32_t got = checksum_key_core(ra_core_singleblock, keys[ki]);
        uint32_t want = SINGLEBLOCK_KAT_CHECKSUMS[ki];
        if (got != want) {
            ++sb_mismatches;
            fprintf(stderr, "SINGLEBLOCK MISMATCH key=%u checksum=0x%08x want=0x%08x\n",
                    keys[ki], got, want);
        }
    }
    printf("validate(singleblock): %d keys checked, %ld checksum mismatches\n",
           nkeys, sb_mismatches);

    long orbit_mismatches = 0;
    for (int ki = 0; ki < nkeys; ++ki) {
        uint32_t got = checksum_key_core(ra_core_orbit, keys[ki]);
        uint32_t want = ORBIT_KAT_CHECKSUMS[ki];
        if (got != want) {
            ++orbit_mismatches;
            fprintf(stderr, "ORBIT MISMATCH key=%u checksum=0x%08x want=0x%08x\n",
                    keys[ki], got, want);
        }
    }
    printf("validate(orbit, K<=255 window): %d keys checked, %ld checksum mismatches\n",
           nkeys, orbit_mismatches);

    long total = sb_mismatches + orbit_mismatches;
    if (total == 0) {
        printf("validate: PASS -- singleblock matches ra_core.c's existing KAT "
               "(rolling-o bit-exact proof) AND orbit matches its own KAT "
               "(regression guard only, NOT statistical validation).\n");
    } else {
        printf("validate: FAIL -- %ld total checksum mismatches (see above).\n", total);
    }
    return total == 0 ? 0 : 1;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage:\n"
            "  %s validate\n"
            "  %s checksum-gen <core>\n"
            "  %s --stream <core> <key> <n>\n"
            "Cores: orbit singleblock (singleblock valid only for rng in [1,255])\n",
            argv[0], argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "validate") == 0) {
        return run_validate();
    }

    if (strcmp(argv[1], "checksum-gen") == 0 && argc >= 3) {
        const core_entry_t *core = find_core(argv[2]);
        uint32_t keys[2 + 5 + 32];
        int nkeys = 0;
        validate_keys(keys, &nkeys);
        printf("static const uint32_t %s_KAT_CHECKSUMS[%d] = {\n",
               strcmp(core->name, "orbit") == 0 ? "ORBIT" : "SINGLEBLOCK", nkeys);
        for (int ki = 0; ki < nkeys; ++ki) {
            uint32_t chk = checksum_key_core(core->fn, keys[ki]);
            printf("    0x%08xu,%s", chk, (ki % 6 == 5) ? "\n" : " ");
        }
        printf("\n};\n");
        return 0;
    }

    if (strcmp(argv[1], "--stream") == 0 && argc >= 5) {
        const core_entry_t *core = find_core(argv[2]);
        uint32_t key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t rng = (size_t)strtoull(argv[4], NULL, 0);

        if (core->max_rng >= 0 && (long)rng > core->max_rng) {
            fprintf(stderr,
                "--stream: n=%zu exceeds core '%s' max_rng=%ld.\n",
                rng, core->name, core->max_rng);
            return 1;
        }

        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        uint32_t last_cons = core->fn(key, rng, stdout);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

        fprintf(stderr, "Streamed %zu pseudorandom outputs in %.3f seconds\n", rng, elapsed);
        fprintf(stderr, "Last cons from RNGing: %u\n", last_cons);
        return 0;
    }

    fprintf(stderr, "Unknown mode or missing args: %s\n", argv[1]);
    return 1;
}
