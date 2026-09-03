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
| `ra_core_singleblock` | **defect K-kecil, BELUM di-fix (formula produksi saat ini)** | 0 FAILED untuk kandidat fix `w8_f10_i0`/`w8_f28_i0` (2026-09-03) | PASS (formula saat ini) | fixed (GUARD_M) | **BLOCKER UTAMA — lihat §2, action item 1&2 kini CLEAR, tinggal keputusan promosi §3 poin 3** |

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

**Belum ada yang dipromosikan ke `ra_core.c`** — `git diff --stat -- ra_core.c`
kosong sepanjang sesi combo-winner-pareto-selection maupun sesi ini. Promosi
butuh konfirmasi eksplisit user terpisah (per `HANDOVER.md` §8 folder
combo-winner-pareto-selection).

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
3. **Keputusan user** (BELUM diminta/diberikan): kandidat mana yang
   dipromosikan ke `ra_core.c` (atau tetap dua-duanya sebagai opsi
   konfigurasi) — konfirmasi eksplisit diperlukan sebelum `ra_core.c`
   disentuh. Ini satu-satunya blocker yang tersisa sekarang.
4. Re-verifikasi battery gate (`experiments/2026-9-1_production-candidate-battery/`)
   resmi dibuka lagi setelah promosi.

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

**Gate produksi PAUSED, satu blocker tersisa: dieharder + inject-crossing
untuk `w8_f10_i0`/`w8_f28_i0` sudah CLEAR (2026-09-03, 0 FAILED dieharder,
11/12 FULLY CLEAN inject-crossing) — tinggal keputusan eksplisit user
kandidat mana yang dipromosikan ke `ra_core.c` (§3 poin 3), lalu buka lagi
battery gate (§3 poin 4). Kecepatan bukan masalah — desain sudah kompetitif
vs Philox/Xoshiro di regime pemakaian aslinya (K besar).**
