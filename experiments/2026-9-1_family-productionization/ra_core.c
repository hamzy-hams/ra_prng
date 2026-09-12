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
//   - ra_permutation_cycle_orbit K-small defect fix (2026-09-04,
//     ../2026-9-4_orbit-fix-and-wideo-rolling-optimization/HANDOVER.md +
//     ../2026-9-4_orbit-kmin-battery/RESULTS.md): `ra_permutation_cycle_orbit`
//     still had the narrow 2-tap `o` with no finalizer -- byte-identical to
//     the PRE-fix singleblock formula above, and because `ra_init_state_orbit`'s
//     M[] path is byte-identical to singleblock's and L[] never influences
//     output for rng<=255 in one call, `ra_core_orbit(key, rng<=255, ...)`
//     reproduced that exact defective formula for anyone using orbit in a
//     "multistream" pattern (many short calls, fresh key each time) instead
//     of its intended long continuous stream. Ported the same `w8_f10_i0`
//     fix (wide 8-tap `o` + `c ^= c >> 17u` finalizer) to
//     `ra_permutation_cycle_orbit`. Statistically re-validated in the
//     multistream pattern specifically (the gap this closes): dieharder 0
//     FAILED (K=1/K=255), PractRand 16GB no anomalies (K=1/K=255),
//     avalanche PASS, collision-scan PASS -- see
//     ../2026-9-4_orbit-kmin-battery/RESULTS.md. `ra_init_state_orbit`
//     itself is UNCHANGED.
//   - Rolling-register computation of wide-`o` (2026-09-04, same HANDOVER.md
//     as above), applied to BOTH ra_permutation_cycle_singleblock AND
//     ra_permutation_cycle_orbit: `o` was recomputed from all 8 M[] taps
//     from scratch every iteration, even though consecutive iterations'
//     8-element windows share 7 elements. Because XOR and constant
//     left-shift are linear over GF(2), `o(i-1)` is derivable from `o(i)`
//     by an exact algebraic identity (XOR out the term leaving the window,
//     shift left by 1, XOR in the new term), not an approximation. ~30-34%
//     speedup at K=255/long streams, no regression at K=1 (structurally
//     inert there -- only one cycle iteration runs). Bit-exact: `validate`
//     below reproduces the SAME checksums as the naive formula for both
//     cores, 0 mismatches -- this needed no separate statistical
//     re-validation of its own, only the KAT-match proof.
//   - Variable-length M[] init for ra_init_state_singleblock (2026-09-04,
//     RESULTS.md's "Variable-length M[] init" section): ra_permutation_cycle_singleblock
//     indexes M[] only via the loop variable `i` (never via `d`, unlike
//     orbit's data-dependent L[i]<->L[d] swap), so for `rng` rounds the
//     cycle only ever touches a wrapped, K-predictable index range of size
//     min(rng+7,256) -- entries outside it are provably dead and no longer
//     initialized. Bit-exact: `validate` below still matches
//     SINGLEBLOCK_KAT_CHECKSUMS, 0 mismatches. First attempt (single masked
//     `(start+j)&0xFF` loop) measured a real REGRESSION at rng>=~96 (defeats
//     auto-vectorization vs. the old plain 0..255 loop) -- fixed by splitting
//     into up to 2 non-wrapping ascending-index sub-loops instead; see
//     RESULTS.md for the full before/after benchmark table. `ra_core_orbit`
//     is NOT touched by this (its L[]<->L[d] swap makes the equivalent trick
//     unsafe for rng>=2; orbit's K=1 case shares the same dead-window
//     property but was left as a documented, unimplemented backlog item,
//     not applied here).
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

// addr_hi guard constants (2026-09-06, 64-bit address widening promotion,
// ../2026-9-5_orbit-address-widen-64bit/RESULTS.md "Kandidat D"): second
// independent guard pair for the addr_hi-derived hash term, distinct from
// GUARD_L/GUARD_M so the two 32-bit inputs (key, addr_hi) never collide on
// the same guard constant.
#define GUARD_L2 0x3eb13b90u
#define GUARD_M2 0x392456deu

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
// 64-bit address widening (2026-09-06, Kandidat D promotion -- see
// ../2026-9-5_orbit-address-widen-64bit/RESULTS.md sec 9.1): addr_hi is a
// second 32-bit input, independent of key, giving 64-bit effective address
// space (mitigates the birthday-paradox key-repetition defect found for
// K=1+hashed-key, see ../2026-9-5_orbit-k1-exhaustive-function-analysis/).
// keyterm_l/keyterm_m are now the PRODUCT of two independently-hashed terms
// (one from key, one from addr_hi). Rotation stays `r = key ^ i` -- addr_hi
// deliberately does NOT enter the rotation; folding it in there (Kandidat
// A/B) XORed two independent Weyl-stepped sequences into rot32's 5-bit
// residue and failed PractRand Gap-16 at 4-8GB under K=1+Weyl reinit.
static void ra_init_state_orbit(uint32_t *L, uint32_t *M, uint32_t key,
                                 uint32_t addr_hi) {
    uint32_t keyterm_l_key   = fmix32(GUARD_L  ^ (0x9e3779b7u * key));
    uint32_t keyterm_l_param = fmix32(GUARD_L2 ^ (0x85ebca77u * addr_hi));
    uint32_t keyterm_l = keyterm_l_key * keyterm_l_param;

    uint32_t keyterm_m_key   = fmix32(GUARD_M  ^ (0x06a0dd9bu * key));
    uint32_t keyterm_m_param = fmix32(GUARD_M2 ^ (0x9e3779b1u * addr_hi));
    uint32_t keyterm_m = keyterm_m_key * keyterm_m_param;

    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i; // addr_hi deliberately excluded, see above
        uint32_t l_val = (uint32_t)(i * 0x9e3779b7u) * keyterm_l;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

// One full 255-step permutation cycle, orbit mode (includes the L[i]<->L[d]
// swap that feeds ra_reseed on the next cycle).
//
// Wide 8-tap `o` + XORSHIFT(17) finalizer on `c`: K-small defect fix
// ported from ra_permutation_cycle_singleblock, candidate w8_f10_i0.
// `o` itself computed via rolling register (o_term7 tracks the term
// leaving the window) instead of recomputing all 8 taps from scratch each
// iteration -- bit-exact identity. See header comment provenance block.
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

uint32_t ra_core_orbit(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_state_orbit(L, M, key, addr_hi);

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
//
// Variable-length init (2026-09-04): ra_permutation_cycle_singleblock below
// indexes M[] ONLY via the loop variable `i` (the rolling-o window), NEVER
// via `d` (the output-derived byte) -- unlike ra_core_orbit's L[i]<->L[d]
// swap, M[] access here is fully static/predictable from `rng` alone. For
// `rng` rounds the cycle only ever touches the wrapped index range
// [(256-rng)&0xFF .. 6] (size min(rng+7,256)), so entries outside that range
// are provably never read and are safely left uninitialized. IMPORTANT: if
// ra_permutation_cycle_singleblock is ever changed to read M[] via a
// data-dependent index (e.g. M[d]), this function MUST be revisited -- the
// partial-init range below would then be unsafe.
// Split into up to 2 non-wrapping ascending-index sub-loops rather than one
// masked `(start+j)&0xFF` loop: a per-iteration wrap mask defeats
// auto-vectorization at larger `rng` (measured regression vs. the old
// full-256 loop at rng>=~96 before this split -- see "Variable-length M[]
// init" section in RESULTS.md), while each plain ascending-index sub-loop
// here vectorizes the same way the original 256-iteration loop did.
// addr_hi widening (2026-09-06, Kandidat D): same product-of-two-hashes
// keyterm_m as ra_init_state_orbit, see that function's header comment.
static void ra_init_state_singleblock(uint32_t *M, uint32_t key,
                                       uint32_t addr_hi, size_t rng) {
    uint32_t keyterm_m_key   = fmix32(GUARD_M  ^ (0x06a0dd9bu * key));
    uint32_t keyterm_m_param = fmix32(GUARD_M2 ^ (0x9e3779b1u * addr_hi));
    uint32_t keyterm_m = keyterm_m_key * keyterm_m_param;
    int len = (rng + 7 > 256) ? 256 : (int)(rng + 7);
    int start = (256 - (int)rng) & 0xFF;
    int end = start + len; // may exceed 256 (wraps past index 255)

    int first_end = (end <= 256) ? end : 256;
    for (int i = start; i < first_end; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32(m_val, r);
    }
    if (end > 256) {
        for (int i = 0; i < end - 256; ++i) {
            uint32_t r = key ^ (uint32_t)i;
            uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
            M[i] = rot32(m_val, r);
        }
    }
}

// Wide 8-tap `o` (was 2-tap M[i+6]/M[i+7]) + XORSHIFT(17) finalizer on `c`:
// K-small defect fix, candidate w8_f10_i0 -- see header comment provenance
// block above. `o` computed via rolling register (o_term7 tracks the term
// leaving the window) instead of recomputing all 8 taps from scratch each
// iteration -- bit-exact identity, see header comment.
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

uint32_t ra_core_singleblock(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream) {
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

    ra_init_state_singleblock(M, key, addr_hi, rng);
    ra_permutation_cycle_singleblock(cons, /*it=*/0, M, &count, raw_stream);
    return cons; // == key always for rng<=255 (cons is never reassigned
                 // without ra_reseed).
}

// ----------------------------------------------------------------------
// CLI dispatch -- pola CORES[]/find_core dari tahap6_bench.c.
// ----------------------------------------------------------------------

typedef uint32_t (*core_fn_t)(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream);

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
// validate: known-answer-test (KAT) checksum check, now for BOTH cores,
// for every key in {0, 0xFFFFFFFF, 5 fixed keys, 0..31} x every rng in
// 1..255. In-process via fmemopen (no subprocess/cmp needed at this scale).
//
// UPDATED 2026-09-03 (K-small structural defect fix, see the provenance
// comment near ra_permutation_cycle_singleblock above): this used to compare
// ra_core_singleblock against ra_core_orbit directly (precedent:
// tahap6_bench.c's run_validate_singleblock, valid back when the two cores
// were bit-identical for rng<=255 by design). That equivalence broke once
// singleblock got its K-small fix first (2026-09-03) while orbit hadn't yet
// -- comparing against orbit then would have reported a false "FAIL".
// Replaced with fixed KAT checksum tables (SINGLEBLOCK_KAT_CHECKSUMS /
// ORBIT_KAT_CHECKSUMS below), so this only catches a future silent change to
// either cycle function's output, not cross-core equivalence.
//
// UPDATED 2026-09-04 (orbit K-small fix + rolling-o promoted, see header
// provenance block): now that BOTH cores share the same fixed cycle
// transform again for the K<=255 pre-reseed window, their KAT tables are
// (expectedly) identical -- see ORBIT_KAT_CHECKSUMS's comment.
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
 * Generalized (2026-09-04) to take any core_fn_t, so the same KAT machinery
 * covers both singleblock and orbit (the K<=255 pre-reseed window, so this
 * exercises the same per-round transform for both). Rotate+add avalanches
 * any single-word change into the whole checksum.
 *
 * `buf` is deliberately 1 word larger than the max write (255 words):
 * glibc's fmemopen("wb") writes a trailing NUL right after the last byte
 * written, and if the buffer is exactly write-sized that NUL overwrites the
 * last byte instead of landing past it -- on little-endian this would
 * clobber the MSB of the last uint32_t written every time. The spare word
 * gives that NUL somewhere harmless to land (found 2026-09-03 debugging
 * production-candidate-battery's Step 7, see PRODUCTION_READINESS_HANDOVER.md). */
static uint32_t checksum_key_core(core_fn_t fn, uint32_t key) {
    uint32_t chk = 0;
    uint32_t buf[256];
    for (size_t n = 1; n <= 255; ++n) {
        FILE *fs = fmemopen(buf, n * sizeof(uint32_t) + 1, "wb");
        fn(key, 0u, n, fs); // addr_hi=0: KAT baseline == "single global address"
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
 * UNCHANGED by the 2026-09-04 rolling-o change -- that table match is the
 * bit-exactness proof, see header comment.
 *
 * REGENERATED 2026-09-06 (Kandidat D / 64-bit address widening promotion,
 * ../2026-9-5_orbit-address-widen-64bit/RESULTS.md): ra_init_state_orbit/
 * _singleblock's keyterm is now a product of two independently-hashed
 * terms (key, addr_hi) instead of a single key-hashed term -- Kandidat D
 * is NOT byte-identical to the pre-promotion formula even at addr_hi=0
 * (verified: RESULTS.md sec 1), so this table intentionally supersedes the
 * old one. `checksum_key_core` always calls with addr_hi=0u ("single
 * global address" baseline), captured via `./ra_core checksum-gen
 * singleblock` post-promotion. */
static const uint32_t SINGLEBLOCK_KAT_CHECKSUMS[39] = {
    0x060c638au, 0x6461ba2cu, 0x83901f29u, 0x37de0899u, 0x9b173a43u, 0x4a7cb273u,
    0x03935ef3u, 0x060c638au, 0xa4c84e66u, 0xdb3b9c7au, 0xda7b73b6u, 0x50267095u,
    0x8ba36778u, 0xc1ffd246u, 0xc0735137u, 0x721baf2bu, 0xe67e8d67u, 0xa6f5216du,
    0xed673689u, 0xbb50e7dbu, 0xcb6461c6u, 0x5482fab3u, 0x3175065fu, 0xad39e0f9u,
    0x92449c76u, 0xa52622acu, 0xf52ff02au, 0xf1614af4u, 0x4fac6af6u, 0x0fe7aa28u,
    0xc2ea7666u, 0xa7ce7279u, 0x53655745u, 0xf39c3777u, 0xab9a5443u, 0x68b6bdc2u,
    0xa25ee6c0u, 0x99f4cbf0u, 0x61d79605u,
};

/* Golden checksums for orbit, K<=255 pre-reseed window, same 39 keys and
 * same order as SINGLEBLOCK_KAT_CHECKSUMS. Captured from
 * ../2026-9-4_orbit-fix-and-wideo-rolling-optimization/ra_core_v2.c's
 * `checksum-gen orbit` and independently cross-checked there against a
 * standalone non-rolling reference build -- 0 mismatches across 39 keys.
 * Statistically re-validated in the multistream K=1/K=255 pattern by
 * ../2026-9-4_orbit-kmin-battery/RESULTS.md (dieharder 0 FAILED, PractRand
 * 16GB no anomalies, avalanche + collision-scan PASS) before promotion here.
 *
 * These values are IDENTICAL to SINGLEBLOCK_KAT_CHECKSUMS above. This is
 * expected, not a copy-paste bug: orbit's K<=255 window uses the same
 * M[]-init path, the same `cons=key`/`it=0` start state, and (after the
 * K-small fix) the same per-round cycle transform as singleblock -- L[]
 * never influences output before the first reseed.
 *
 * REGENERATED 2026-09-06 alongside SINGLEBLOCK_KAT_CHECKSUMS above (see
 * that table's comment) -- still identical to it for the same reason,
 * addr_hi=0u baseline, `./ra_core checksum-gen orbit` post-promotion. */
static const uint32_t ORBIT_KAT_CHECKSUMS[39] = {
    0x060c638au, 0x6461ba2cu, 0x83901f29u, 0x37de0899u, 0x9b173a43u, 0x4a7cb273u,
    0x03935ef3u, 0x060c638au, 0xa4c84e66u, 0xdb3b9c7au, 0xda7b73b6u, 0x50267095u,
    0x8ba36778u, 0xc1ffd246u, 0xc0735137u, 0x721baf2bu, 0xe67e8d67u, 0xa6f5216du,
    0xed673689u, 0xbb50e7dbu, 0xcb6461c6u, 0x5482fab3u, 0x3175065fu, 0xad39e0f9u,
    0x92449c76u, 0xa52622acu, 0xf52ff02au, 0xf1614af4u, 0x4fac6af6u, 0x0fe7aa28u,
    0xc2ea7666u, 0xa7ce7279u, 0x53655745u, 0xf39c3777u, 0xab9a5443u, 0x68b6bdc2u,
    0xa25ee6c0u, 0x99f4cbf0u, 0x61d79605u,
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
        printf("validate: PASS -- singleblock and orbit both match their "
               "known-answer checksums (w8_f10_i0 fix formula, both cores).\n");
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
            "  %s --stream <core> <key> <n>              (addr_hi implied 0)\n"
            "  %s --stream <core> <key> <addr_hi> <n>     (explicit addr_hi)\n"
            "Cores: orbit singleblock (singleblock valid only for rng in [1,255])\n",
            argv[0], argv[0], argv[0], argv[0]);
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
        // Backward-compatible: argc==5 is the old 4-arg form (key n),
        // addr_hi implied 0. argc>=6 is the new 5-arg form (key addr_hi n).
        uint32_t addr_hi = 0u;
        size_t rng;
        if (argc == 5) {
            rng = (size_t)strtoull(argv[4], NULL, 0);
        } else {
            addr_hi = (uint32_t)strtoul(argv[4], NULL, 0);
            rng = (size_t)strtoull(argv[5], NULL, 0);
        }

        if (core->max_rng >= 0 && (long)rng > core->max_rng) {
            fprintf(stderr,
                "--stream: n=%zu exceeds core '%s' max_rng=%ld.\n",
                rng, core->name, core->max_rng);
            return 1;
        }

        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        uint32_t last_cons = core->fn(key, addr_hi, rng, stdout);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

        fprintf(stderr, "Streamed %zu pseudorandom outputs in %.3f seconds\n", rng, elapsed);
        fprintf(stderr, "Last cons from RNGing: %u\n", last_cons);
        return 0;
    }

    fprintf(stderr, "Unknown mode or missing args: %s\n", argv[1]);
    return 1;
}
