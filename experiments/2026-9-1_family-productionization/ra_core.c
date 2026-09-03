// ra_core.c
// Tahap 1 (HANDOVER.md): unifikasi ra_prng-family. Satu file, dua entry
// point publik, satu core recurrence bersama. Precedent read-only repo ini
// dipatuhi: winner_wired_v2.c, winner_wired_addressable.c, dan
// tahap6_bench.c semuanya TIDAK disentuh, tetap ada sebagai riwayat/rujukan
// validasi.
//
// Provenance per fungsi:
//   - rot32, ra_hash, ra_reseed, dan struktur ra_core_orbit/
//     ra_permutation_cycle_orbit: byte-for-byte sama dengan
//     winner_wired_v2.c's rot32/ra_hash/ra_reseed/ra_core/
//     ra_permutation_cycle (yang juga byte-for-byte sama dengan
//     winner_wired_addressable.c's versi, kecuali nama parameter
//     seed->key).
//   - ra_init_state_orbit, ra_init_state_singleblock, fmix32: formula mix-
//     init produksi dasarnya adalah kandidat 4 (multiply-combined per-key
//     hashed term) dari
//     ../2026-9-1_multikey-remix-search/diag_init_keyterm_mul.c
//     (ra_init_orbit/ra_init_singleblock di sana, byte-for-byte sama isinya
//     -- cuma dibersihkan dari sufiks _affine di sini). Ini MENGGANTIKAN
//     formula affine-in-key Tahap 1 addressable-init (Kandidat 5) yang
//     dipakai sebelumnya, setelah terbukti gagal (PractRand BCFN FAIL di
//     4GB) pada pola aggressive-reinit/multikey -- root cause & fix
//     candidate search ada di folder itu, HANDOVER.md. Non-regresi
//     terhadap seluruh jaminan single-key formula lama (128GB PractRand,
//     cross-correlation, collision-scan, dieharder -- semua PASS) dicatat
//     di HANDOVER.md folder yang sama, bagian "Non-regression verification
//     results (2026-09-01)".
//   - GUARD_L/GUARD_M (2026-09-01, ../2026-9-1_keyzero-guard-fix/RESULTS.md):
//     kandidat 4 di atas punya weak-key kritis di key=0 (fmix32(0)==0 dan
//     kombinasi multiply artinya keyterm_l=keyterm_m=0 -> L[]/M[] nol
//     total -> output nol permanen, ditemukan oleh
//     production-candidate-battery/RESULTS.md Step 3). Fix: XOR guard
//     constant (popcount=16, beda dari 4 konstanta lain yang sudah ada di
//     jalur init ini) ke input fmix32 SEBELUM fmix32, jadi key=0 tidak
//     lagi memetakan ke fmix32(0). Diverifikasi: edgecheck 14 key
//     (termasuk key=0 dan 2 key "single-keyterm-zero" turunannya) semua
//     non-degenerate, PractRand multikey K=255 bersih 32GB (tidak
//     meregresi fix BCFN), ./ra_core validate tetap 9945/9945 pasca
//     diterapkan. Detail lengkap + bukti di folder itu.
//   - ra_permutation_cycle_singleblock (K-small structural defect fix,
//     2026-09-03, ../2026-9-3_dieharder-inject-crossing/RESULTS.md +
//     ../2026-9-2_singleblock-cycle-combo-search/RESULTS.md +
//     ../2026-9-3_combo-winner-pareto-selection/RESULTS.md): the narrow
//     2-tap `o` (M[i+6]<<6 ^ M[i+7]<<7, tahap6_bench.c's original) had a
//     BCFN-style PractRand defect at small K, safe only from K>=96
//     (../2026-9-2_singleblock-k-threshold-characterization/RESULTS.md).
//     Fixed by widening the tap to all 8 M[] slots and adding a single
//     XORSHIFT(17) finalizer stage on `c` (candidate id `w8_f10_i0` from a
//     756-combo cycle-operation DSL search) -- the fastest of 2 co-frontier
//     Pareto candidates, user-selected 2026-09-03 for promotion over
//     `w8_f28_i0` (best avalanche margin, <1ns slower). Verified clean:
//     PractRand 16GB across K in {1,2,4,8,16,32,64,96}, dieharder 0 FAILED
//     (K=1/K=96), 11/12 extra-inject variants stay clean when crossed
//     (inject itself stays OFF/unused in production). `ra_init_state_singleblock`
//     and `ra_core_singleblock` themselves are UNCHANGED -- only the
//     per-round transform in the cycle function was in scope for this fix.
//
// Keputusan penamaan/scope (dikonfirmasi user, sesi family-productionization
// 2026-09-01 -- lihat HANDOVER.md, sebelumnya PAUSED menunggu ini):
//   - Nama file/anchor: "winner_wired" (artefak nama "pemenang survei
//     wiring", bukan identitas algoritma) dibuang sepenuhnya; dipakai
//     ra_core apa adanya, karena fungsi ekspornya sendiri sudah konsisten
//     bernama itu di semua varian sebelumnya.
//   - Tanpa suffix versi ("_v2" dst) -- ini varian produksi/default, bukan
//     salah satu dari beberapa iterasi bertanggal.
//   - Bentuk API: dua fungsi publik terpisah (ra_core_orbit /
//     ra_core_singleblock) memanggil helper statis bersama, BUKAN
//     satu fungsi dengan parameter mode.
//   - Hanya 2 mode, bukan 3: mode "stream kontinu non-addressable"
//     (winner_wired_v2.c's ra_init_state, fixed tanpa key sama sekali)
//     SENGAJA TIDAK dibawa ke sini. Stream kontinu cukup dilayani mode
//     orbit dengan satu key/address tetap. ra_core_orbit karena itu
//     TIDAK diharapkan/diuji bit-identical terhadap winner_wired_v2.c's
//     output -- lihat RESULTS.md.
//   - Nama mode: "orbit" (addressable, unbounded -- pengganti "baseline"/
//     "addressable") dan "singleblock" (<=255 kata per init, tanpa L[],
//     nama lama tetap dipakai). CATATAN KEBIJAKAN: "orbit" sebelumnya
//     (memory project_ra_prng_terminology, 2026-08-30) dicatat sebagai
//     istilah dokumentasi/paper SAJA, eksplisit bukan untuk rename kode.
//     Keputusan ini secara sadar membalik itu atas permintaan eksplisit
//     user sesi ini -- "orbit" sekarang identifier kode resmi.
//   - Parameter key (Philox terminology, Tahap 1 addressable-init) tidak
//     diubah.
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h
//          ra_core.c -o ra_core -Wall -Wextra
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
// 255 words). Formula identik dengan winner_wired_addressable.c.
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
static void ra_permutation_cycle_orbit(uint32_t cons, size_t it,
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

static void ra_permutation_cycle_singleblock(uint32_t cons, size_t it,
                                              const uint32_t *M,
                                              uint64_t *count,
                                              FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    for (uint32_t i = 255; i > 0; --i) {
        // Wide 8-tap `o` (was 2-tap M[i+6]/M[i+7]) + XORSHIFT(17) finalizer
        // on `c`: K-small defect fix, candidate w8_f10_i0 -- see header
        // comment provenance block above.
        uint32_t o = (M[(uint8_t)(i + 0)] << 0) ^ (M[(uint8_t)(i + 1)] << 1) ^
                     (M[(uint8_t)(i + 2)] << 2) ^ (M[(uint8_t)(i + 3)] << 3) ^
                     (M[(uint8_t)(i + 4)] << 4) ^ (M[(uint8_t)(i + 5)] << 5) ^
                     (M[(uint8_t)(i + 6)] << 6) ^ (M[(uint8_t)(i + 7)] << 7);

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
// validate: known-answer-test (KAT) checksum check for ra_core_singleblock,
// for every key in {0, 0xFFFFFFFF, 5 fixed keys, 0..31} x every rng in
// 1..255. In-process via fmemopen (no subprocess/cmp needed at this scale).
//
// UPDATED 2026-09-03 (K-small structural defect fix, see the provenance
// comment near ra_permutation_cycle_singleblock above): this used to compare
// ra_core_singleblock against ra_core_orbit directly (precedent:
// tahap6_bench.c's run_validate_singleblock, valid back when the two cores
// were bit-identical for rng<=255 by design). That equivalence is now
// PERMANENTLY GONE ON PURPOSE -- the whole point of the fix is that
// singleblock's cycle transform diverges from orbit's at small K. Comparing
// against orbit here would therefore always report "FAIL" even though the
// formula is correct and independently validated (16GB PractRand, dieharder,
// inject-crossing -- see the header provenance comment). Replaced with a
// fixed KAT checksum table (SINGLEBLOCK_KAT_CHECKSUMS below) captured from
// that validated formula, so this check now only catches a future silent
// change to ra_permutation_cycle_singleblock's output, not
// singleblock-vs-orbit equivalence.
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

/* Folds ra_core_singleblock's output stream for n=1..255 into one checksum
 * per key -- used both to print the golden table (dev-only "checksum-gen"
 * subcommand) and to check against it (below). Rotate+add avalanches any
 * single-word change into the whole checksum, so this is sensitive to the
 * same 9945 (key,n) combinations the old orbit-vs-singleblock check covered.
 *
 * `buf` is deliberately 1 word larger than the max write (255 words):
 * glibc's fmemopen("wb") writes a trailing NUL right after the last byte
 * written, and if the buffer is exactly write-sized that NUL overwrites the
 * last byte instead of landing past it -- on little-endian this would
 * clobber the MSB of the last uint32_t written every time. The spare word
 * gives that NUL somewhere harmless to land (found 2026-09-03 debugging
 * production-candidate-battery's Step 7, see PRODUCTION_READINESS_HANDOVER.md). */
static uint32_t checksum_key(uint32_t key) {
    uint32_t chk = 0;
    uint32_t buf[256];
    for (size_t n = 1; n <= 255; ++n) {
        FILE *fs = fmemopen(buf, n * sizeof(uint32_t) + 1, "wb");
        ra_core_singleblock(key, n, fs);
        fclose(fs);
        for (size_t j = 0; j < n; ++j) {
            chk = rot32(chk ^ buf[j], 7) + 0x9E3779B9u;
        }
    }
    return chk;
}

/* Golden checksums for validate_keys()'s 39 keys, in the same order,
 * computed from the validated w8_f10_i0 fix (8-tap `o` + XORSHIFT(17)
 * finalizer) applied to ra_permutation_cycle_singleblock -- see
 * experiments/2026-9-3_dieharder-inject-crossing/RESULTS.md and
 * experiments/2026-9-3_combo-winner-pareto-selection/RESULTS.md for the
 * validation this formula carries (16GB PractRand across K in
 * {1,2,4,8,16,32,64,96}, dieharder 0 FAILED at K=1/K=96). This is a
 * regression KAT, not a proof of quality by itself -- it only catches
 * ra_permutation_cycle_singleblock silently changing after this point.
 * ra_core_singleblock and ra_core_orbit are now INTENTIONALLY different
 * formulas (that divergence is the fix), so this no longer compares
 * against ra_core_orbit the way the old validate() did. */
static const uint32_t SINGLEBLOCK_KAT_CHECKSUMS[39] = {
    0xee5a0763u, 0x299f7345u, 0x165b620au, 0x41b9b933u, 0xf6613981u, 0x6a03bbf6u,
    0xf8975230u, 0xee5a0763u, 0x1a78795cu, 0x89b5301cu, 0x787bd47cu, 0x31b981d8u,
    0x0ca2c44au, 0xedc2a549u, 0x509112dau, 0xa1f89116u, 0x274c5a70u, 0x0ba07d7bu,
    0x280d1cb1u, 0xfb4a5ec1u, 0xf7e58e11u, 0x291396c2u, 0x1d380914u, 0x8f21fc57u,
    0xd10f68c3u, 0x0c51c5b0u, 0xb734e7eeu, 0x0f2dbf47u, 0xd8abf450u, 0xbb9422e4u,
    0x795fb18fu, 0xc1bced6eu, 0x34b16a2du, 0x7906dcfbu, 0x5a0c615cu, 0x06dfab09u,
    0xa0cfe6c2u, 0x90cf2a0cu, 0x3cb34dfau,
};

static int run_validate_singleblock(void) {
    uint32_t keys[2 + 5 + 32];
    int nkeys = 0;
    validate_keys(keys, &nkeys);

    long mismatches = 0;
    for (int ki = 0; ki < nkeys; ++ki) {
        uint32_t got = checksum_key(keys[ki]);
        uint32_t want = SINGLEBLOCK_KAT_CHECKSUMS[ki];
        if (got != want) {
            ++mismatches;
            fprintf(stderr, "MISMATCH key=%u checksum=0x%08x want=0x%08x\n",
                    keys[ki], got, want);
        }
    }

    printf("validate: %d keys checked (255 lengths each, %d combinations), %ld checksum mismatches\n",
           nkeys, nkeys * 255, mismatches);
    if (mismatches == 0) {
        printf("validate: PASS -- ra_core_singleblock matches its known-answer "
               "checksums (w8_f10_i0 fix formula).\n");
    } else {
        printf("validate: FAIL -- ra_permutation_cycle_singleblock's output "
               "changed since the KAT checksums were captured.\n");
    }
    return mismatches == 0 ? 0 : 1;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage:\n"
            "  %s validate\n"
            "  %s --stream <core> <key> <n>\n"
            "Cores: orbit singleblock (singleblock valid only for rng in [1,255])\n",
            argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "validate") == 0) {
        return run_validate_singleblock();
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
