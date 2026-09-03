# Production Readiness Handover — `ra_core_orbit` / `ra_core_singleblock`

**Tujuan dokumen ini**: satu tempat terpusat untuk memantau semua yang harus
selesai sebelum `experiments/2026-9-1_family-productionization/ra_core.c`
(dua entry point publik: `ra_core_orbit`, `ra_core_singleblock`) bisa disebut
siap produksi. **Dimaksudkan untuk terus di-update tiap sesi** sampai battery
gate lolos — bukan laporan sekali-jalan seperti `HANDOVER.md`/`RESULTS_TAHAP2.md`
di folder ini (yang tetap riwayat Tahap 1/2, jangan diubah).

Scope pelacakan (protokol benchmark/status standar, lihat memory
`feedback_orbit_singleblock_benchmark_standard`): `singleblock` K=1..K=255
(seluruh rentang valid), `orbit --multistream` (reinit tiap K kata), dan
`orbit --stream` (kontinu, tanpa reinit paksa).

Ditulis 2026-09-03, setelah sesi investigasi kecepatan (kenapa lebih lambat
dari versi lama, perbandingan vs Philox/Xoshiro) dan setelah commit `d2f1675`
(combo-winner Pareto-selection, hari yang sama).

---

## 1. Status matrix ringkas

| mode | PractRand | dieharder | cross-corr/collision-scan | weak-key (key=0) | status |
|---|---|---|---|---|---|
| `ra_core_orbit` | 128GB clean | 0 FAILED | PASS | fixed (GUARD_L/GUARD_M) | **tidak ada blocker diketahui** |
| `ra_core_singleblock` | **FIXED 2026-09-03 — `w8_f10_i0` dipromosikan ke `ra_core.c`, battery gate Step 0-8 CLOSED** | 0 FAILED (K=1/K=96 standalone, K=1/K=255 battery gate) | PASS (K=1/K=255, battery gate Step 4) | fixed (GUARD_M) | **TIDAK ADA BLOCKER TERSISA — production-candidate-battery gate PASS, lihat `../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md`** |

---

## 2. Blocker utama: singleblock K-small structural defect

`ra_core_singleblock` (formula saat ini di `ra_core.c`, dengan GUARD_M +
fmix32 + keyterm-multiply-combine) punya defect PractRand BCFN-style pada K
kecil. Status resmi di `RESULTS_TAHAP2.md`: **"DEFECT NYATA DITEMUKAN, root
cause terdiagnosis, fix BELUM diimplementasikan ke `ra_core.c`."**
Konsekuensinya, battery gate produksi
(`experiments/2026-9-1_production-candidate-battery/RESULTS.md`) berstatus:
**"key=0 defect FIXED dan Step 0-3 RECONFIRMED clean, tapi defect TERPISAH
yang lebih parah (pola K=1) sekarang memblokir progres lebih lanjut — gate
STILL NOT PASSED."** Dikonfirmasi ulang di
`experiments/2026-9-2_singleblock-o-width-fix/RESULTS.md:173`: gate **tetap
PAUSED** sampai ada kandidat fix K-kecil yang terbukti bersih di seluruh
rentang K relevan pada skala 16GB+.

### Safe-K floor formula SAAT INI (belum di-fix)

Dari `experiments/2026-9-2_singleblock-k-threshold-characterization/RESULTS.md`:

> "Ambang minimum aman: K ≈ 80-96 (batas bawah pasti tidak aman di ≤72, batas
> atas pasti aman di ≥96, K=80 kemungkinan besar sudah aman tapi satu noise
> ringan @32GB belum sepenuhnya menghilangkan keraguan). **Rekomendasi
> konservatif: K≥96.**"

Crossover kecepatan singleblock vs orbit (di mana keduanya sama cepat, ~2.2
ns/word): **K≈20-21**. Caveat: ambang K=80 hanya diuji **satu** `base_key`
dan **satu** seed — belum di-reproduksi lintas seed.

### 2 kandidat fix co-frontier (commit `d2f1675`, hari ini)

Dari `experiments/2026-9-3_combo-winner-pareto-selection/RESULTS.md`. Dari 11
kandidat cycle-op combo-search yang tadinya "fully clean" di K∈{1,2,4,8,16,32,
64,96}@16GB, **8/11 gagal** uji normalitas avalanche multi-seed (kemungkinan
artefak diskretisasi test, belum tentu defect asli) — tersisa 2 kandidat
non-dominated:

| kandidat | finalizer | K=1 (ns/word) | continuous-stream (MB/s) | avalanche worst-case margin |
|---|---|---:|---:|---:|
| `w8_f10_i0` (tercepat) | `c ^= c >> 17;` | 36.7 | 134.1 | 0.4810 |
| `w8_f28_i0` (margin terbaik) | `MUL(0xc2b2ae35)->XORSHIFT(13)` | 37.4 | 128.0 | 0.4827 |

(threshold avalanche margin: 0.2 — keduanya jauh di atas ambang)

**DIPROMOSIKAN 2026-09-03**: user memilih `w8_f10_i0` (tercepat) secara
eksplisit ("tidak perlu, langsung saja aku pilih yang tercepat w8_f10_i0"),
tanpa menunggu speed/avalanche test tambahan untuk 11 varian inject baru
(sudah cukup dari dieharder+inject-crossing di atas). Diterapkan ke
`ra_permutation_cycle_singleblock` di `ra_core.c`: `o` diperlebar dari 2-tap
jadi 8-tap penuh (`M[i+0..7]`), ditambah finalizer `c ^= c >> 17u;` persis
formula `w8_f10_i0`. `ra_init_state_singleblock`, `ra_core_singleblock`,
dan seluruh jalur `ra_core_orbit` TIDAK disentuh. Diverifikasi: compile
bersih (`gcc -O3 -march=native -std=gnu17 -include stdalign.h ra_core.c -o
ra_core -Wall -Wextra`), cross-check 70 vektor (10 key x 7 panjang) vs
binary `w8_f10_i0` tervalidasi (`../2026-9-2_singleblock-cycle-combo-search/
candidates/w8_f10_i0`) — 0 mismatch.

**Efek samping penting**: `./ra_core validate` yang tadinya membuktikan
`ra_core_singleblock` bit-identik dengan `ra_core_orbit` (bukti L[] dead-code,
Tahap 1) SEKARANG SENGAJA TIDAK LAGI BERLAKU — fix ini justru membuat kedua
core berbeda formula per-round di K kecil. `run_validate_singleblock()` di
`ra_core.c` diganti jadi known-answer-test (checksum tetap terhadap formula
`w8_f10_i0` yang sudah tervalidasi), bukan lagi perbandingan vs orbit —
lihat komentar di `ra_core.c` dekat `SINGLEBLOCK_KAT_CHECKSUMS`.

### Battery gate Step 4-8 dibuka lagi dan CLOSED (2026-09-03)

Setelah promosi, `../2026-9-1_production-candidate-battery/` (yang berhenti
di Step 3 karena defect K-kecil) dilanjutkan penuh Step 4-8 — detail lengkap
di `ADDENDUM_POST_FIX_STATUS.md` folder itu. Ringkas: collision-scan K=1
PASS (0/50.000 collision, sesuai ekspektasi ~0,29 di ruang 32-bit),
dieharder K=1/K=255 PASS (0 FAILED), PractRand 16GB K=1/K=255 di VPS PASS
(bersih total, "no anomalies" di setiap checkpoint).

**Bug ditemukan+diperbaiki di tengah jalan (bukan defect RNG)**:
`scrambler_ra_core_singleblock.c` (Step 7) awalnya melaporkan K=1
katastropik (rotasi trivial, runs-test z=-3528) — ternyata bug `fmemopen()`
glibc: kalau buffer PAS ukuran tulisan, NUL-terminator menimpa byte
terakhir (byte teratas word ter-nol-kan tiap panggilan). Diperbaiki
(buffer +1 byte) di file itu DAN di `checksum_key()` `ra_core.c` (bug sama,
tapi tautologis-aman karena golden checksum & verifikasi sama-sama pakai
fungsi yang sama — checksum sudah di-regenerate). Semua `fmemopen` lain di
repo membandingkan 2 buffer yang sama-sama kena bug ini secara identik,
jadi hasil PASS historis lain TIDAK perlu diragukan.

Setelah fix: K=255 bersih total, K=1 masih ada bias kecil-tapi-nyata di
runs-test (z=+9,25 vs K=255's -0,35; per-repetisi mean z=0,242 vs 0,155,
beda ~2,8 sigma) — kemungkinan sebagian artefak rumus runs-test untuk data
permutasi (bukan i.i.d. kontinu), tapi K=1 punya lebih banyak dari itu.
**User memutuskan: PASS dengan catatan** — bias ini jauh lebih kecil dari
defect pra-fix, tidak terlihat di PractRand 16GB, dicatat sebagai limitasi
diketahui untuk use-case ekstraksi-bit-rendah (mis. Fisher-Yates) di K=1,
BUKAN alasan membatalkan promosi `w8_f10_i0`.

**Verdict gate: PASS keseluruhan. Tidak ada blocker tersisa.**

### Validasi dieharder + inject-crossing (2026-09-03, `../2026-9-3_dieharder-inject-crossing/RESULTS.md`)

Action item 1 & 2 (lihat §3) sekarang **CLEAR**:

1. **Dieharder**: `w8_f10_i0` dan `w8_f28_i0`, K=1 dan K=96, 27 test
   Good-reliability — **0 FAILED** di keempat run (25-26/27 PASSED, 0-2
   WEAK per run), sebanding dengan baseline produksi `winner_wired_v2`
   (26/27, 2 WEAK) dan `winner_wired_addressable` (25/27, 1 WEAK).
2. **Inject-crossing**: 12 varian extra-inject (`i1`..`i6` untuk kedua
   base) diuji Promotion Tier penuh (16GB, K∈{2,4,8,16,32,64,96}, K=1
   di-skip krn provably identik dgn `i0`) — **11/12 FULLY CLEAN**. Satu
   pengecualian: `w8_f10_i3` FAIL di K=4 (defect BCFN nyata, eskalatif,
   bukan artefak) — K lain untuk kandidat yang sama tetap bersih, jadi ini
   kombinasi (kandidat,K) sempit, bukan pola yang menggeneralisasi. Kandidat
   Pareto-selection asli (`i0`/inject off, keduanya) **tidak terpengaruh**
   temuan ini — tetap sebersih sebelumnya.

Kesimpulan: kalau inject tetap OFF (`i0`, seperti kandidat Pareto-selection
asli), tidak ada temuan baru yang menghalangi promosi. Kalau extra-inject
mau diaktifkan di produksi nanti, `i3` pada `w8_f10` harus dihindari atau
divalidasi ulang independen.

---

## 3. Action item yang belum dikerjakan (blocking promosi)

Dibawa verbatim dari `experiments/2026-9-3_combo-winner-pareto-selection/RESULTS.md`
§Rekomendasi (item 1 dan 2 — item 3 di dokumen asal cuma caveat metodologi,
bukan action item):

1. ~~**Uji dieharder** (bagian dari battery gate yang PAUSED) untuk
   `w8_f10_i0` dan `w8_f28_i0`~~ — **DONE 2026-09-03**, 0 FAILED, lihat di
   atas.
2. ~~**Crossing dengan 7 varian extra-inject** (slot 3 DSL combo-search)
   untuk kedua kandidat~~ — **DONE 2026-09-03**, 11/12 FULLY CLEAN, lihat
   di atas.

Setelah 1 & 2 bersih:
3. ~~**Keputusan user**: kandidat mana yang dipromosikan ke `ra_core.c`~~ —
   **DONE 2026-09-03**, `w8_f10_i0` dipilih user dan diterapkan, lihat §2.
4. ~~Re-verifikasi battery gate~~ — **DONE 2026-09-03**, Step 4-8
   (`../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md`):
   collision-scan K=1 PASS, dieharder K=1/K=255 PASS (0 FAILED), PractRand
   16GB K=1/K=255 PASS (bersih total di VPS), shuffle-implementation PASS
   dengan catatan (lihat §2). **Gate ini sekarang CLOSED — tidak ada action
   item tersisa.**

---

## 4. Selesai/tertutup sesi ini (2026-09-03) — jangan ditelusuri ulang

- **Kekhawatiran "guard/fmix32 bikin ~100x lebih lambat" terbukti salah.**
  Diukur via `diag_init_candidates.c` (kandidat baru `singleblock_oldera`,
  replay byte-for-byte formula lama `winner_wired_addressable.c` di harness
  yang sama): overhead loop init terisolasi cuma **+0.7%**, K=255 full-call
  cuma **+13.9%** — jauh dari skala "berlipat/100x" yang dicurigai.
- **Klaim tabel lama 30303.0 MB/s** (`experiments/2026-8-31_prng-family-benchmark/RESULTS.md`)
  **tidak reproducible**, bahkan saat formula lama itu sendiri direplay
  persis di mesin ini (~7019 MB/s, beda 4.3x dari klaim). Indikasi kuat:
  artefak metodologi benchmark lama (kemungkinan OLS-ekstrapolasi "asymptotic
  ns/word" yang tidak valid untuk core yang hard-capped di K≤255 by design),
  bukan regresi algoritma nyata dari fix guard/fmix32.
- **Perbandingan vs Philox4x32-10** (multiplier `0xD2511F53U` terkoreksi,
  KAT-verified) **dan Xoshiro256\*\*** (KAT-verified, seeding SplitMix64):
  di K=255, `singleblock` menang **4.2x** atas Philox, **1.5x** atas
  Xoshiro; `orbit` menang atas Philox tapi kalah atas Xoshiro. Di K=1
  RA-PRNG kalah dari keduanya — ini trade-off desain (init O(256) per
  key/address vs seeding O(1) Philox/Xoshiro), **bukan anomali**.
- Kode diagnostik: `experiments/2026-9-3_init-loop-optimization/diag_init_candidates.c`
  (cores: `orbit_baseline`, `orbit_accum`, `singleblock_baseline`,
  `singleblock_accum`, `singleblock_oldera`, `philox`, `xoshiro256`),
  hasil lengkap di `RESULTS.md` folder yang sama.

---

## 5. Item terbuka lain, prioritas lebih rendah — TIDAK memblokir gate korektnes

Init-loop speed optimization (`experiments/2026-9-3_init-loop-optimization/`,
lihat memory `project_init_loop_speed_optimization`) — ini soal kecepatan,
bukan bagian battery gate korektnes:

- Kandidat `accum` (ring-arithmetic strength-reduction pada init loop):
  menang ~20%/14% di K=1 (singleblock/orbit), tapi **singleblock meregresi
  +37.3% di K=255**. Root cause dipersempit ke backend-bound execution
  stalls (bukan cache miss/store-forwarding/frontend) via `perf stat`
  topdown, tapi mekanisme port-level belum terkonfirmasi (butuh profiling
  VTune-style, belum dicoba). **Belum diterapkan ke `ra_core.c`.**
- 4 keputusan terbuka dikembalikan ke user (lihat memory di atas): (1) K=1
  gain worth K=255 regression?, (2) orbit's modest-but-consistent win lebih
  disukai?, (3) apply sekarang vs tunda sampai root-cause tuntas?, (4)
  lanjut Priority 2 (formula baru, output boleh berubah)?

---

## 6. Eksplisit di luar scope gate ini (track terpisah)

Quote dari `HANDOVER.md:134` folder ini sendiri — item-item ini **sengaja
tidak masuk** scope productionization/gate ini, boleh dikerjakan kapan saja
secara independen:

> "Tidak mengerjakan item-item di `project_research_backlog_2026_09`
> (dieharder mandiri di luar Tahap 2 ini, mekanisme `inject` di w=32,
> `docs/ERRATA.md`, SIMD cross-stream L-swap) — itu track riset terpisah,
> independen dari productionization ini."

Status masing-masing: **semua belum dimulai**. `docs/ERRATA.md` bahkan belum
punya file sama sekali di repo ini (baru rencana).

---

## 7. Referensi cepat

- `ra_core.c` — target akhir, saat ini TIDAK boleh disentuh tanpa konfirmasi
  eksplisit user.
- `../2026-9-1_production-candidate-battery/RESULTS.md` — status battery
  gate (PAUSED).
- `../2026-9-2_singleblock-k-threshold-characterization/RESULTS.md` — safe-K
  floor formula saat ini (K≥80-96).
- `../2026-9-2_singleblock-cycle-combo-search/RESULTS.md` — 11 kandidat
  cycle-op combo-search awal (sebelum Pareto-selection).
- `../2026-9-3_combo-winner-pareto-selection/HANDOVER.md` + `RESULTS.md` — 2
  kandidat co-frontier + 2 action item §3 di atas.
- `../2026-9-3_dieharder-inject-crossing/RESULTS.md` — hasil dieharder +
  inject-crossing (action item 1&2 §3, DONE 2026-09-03).
- `../2026-9-3_init-loop-optimization/RESULTS.md` + `diag_init_candidates.c`
  — investigasi kecepatan sesi ini (§4-5 di atas).
- `../2026-8-31_prng-family-benchmark/RESULTS.md` — tabel lama yang
  diklarifikasi di §4.
- `../2026-8-30_addressable-init-research/winner_wired_addressable.c` —
  formula lama (pre-fix) yang direplay sebagai `singleblock_oldera`.

---

## 8. Ringkasan satu-baris untuk sesi berikutnya

**GATE PRODUKSI CLOSED (2026-09-03): `w8_f10_i0` dipromosikan ke `ra_core.c`
(§2, `ra_permutation_cycle_singleblock` diganti ke 8-tap `o` + XORSHIFT(17)
finalizer) DAN production-candidate-battery Step 0-8 semua PASS (§2,
`../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md`).
`./ra_core validate` sekarang KAT-checksum, bukan lagi vs-orbit. Satu bug
harness (`fmemopen` glibc, bukan defect RNG) ditemukan+diperbaiki di jalan
Step 7; hasil akhirnya PASS-dengan-catatan (bias kecil di K=1 runs-test,
diterima user, tidak menghalangi apapun). **Tidak ada blocker tersisa** —
kecepatan juga bukan masalah, desain sudah kompetitif vs Philox/Xoshiro di
regime pemakaian aslinya (K besar).**
