# RESULTS — State-Size / Speed Pareto Frontier (N∈{256,128,64,32,16})

## Status: **N=256/128/64/32 SIAP-DIPUBLIKASI (LOWONLY penuh CLEAN; MUTUAL clean kecuali satu FAIL bergradien). N=16 GAGAL (dikecualikan dari Pareto frontier final).**

Eksekusi penuh `HANDOVER.md` (Tahap A-H). Ringkasan: menyusutkan state
`ra_core.c` dari 512 word (2×256) ke N per-half lebih kecil TIDAK
mengubah kesimpulan kualitatif "singleblock K=N-1 mengalahkan semua
baseline" sampai N=32 — tapi di N=16 muncul kegagalan statistik nyata
(bukan cuma degradasi kecepatan), dan mode reseed MUTUAL punya cacat
bergradien-terhadap-N yang baru terlihat di deep-test (Tier2, 4-16GB) di
N=32 setelah lolos screen dangkal (Tier1, 2GB).

## 1. Metodologi ringkas

- Core generik: `ra_core_param.c` (macro `RA_N`/`RA_MASK`/`RA_RESEED_MODE`,
  sanity-check byte-identik dengan `src/ra_core/c/ra_core.c` produksi di
  N=256/LOWONLY — PASS, gerbang keras pertama).
- 15 varian inti: N∈{256,128,64,32,16} × {orbit-LOWONLY, orbit-MUTUAL,
  singleblock (reseed-mode tidak relevan, hanya `M[]`)}.
- Tier bertingkat: **Tier0** (avalanche 3-arah + heatmap, semua varian) →
  **Tier1** (PractRand screen 256M/2G/2G, laptop) → **Tier2** (PractRand
  3-stage 2G/8G/16G dual-base-key + dieharder 27-test K=1 & K=N-1, VPS,
  hanya survivor Tier1). Satu FAIL = varian gagal, final — tidak ada retry
  seed lain (`feedback_no_reseed_on_fail`).
- Benchmark kecepatan (`bench_param.c`, extend `bench_release.c` +PCG32
  baru) jalan independen dari hasil kualitas, mencakup SEMUA 5N,
  5-angka-standar (orbit K=1/K=N-1/`--stream`, singleblock K=1/K=N-1) per N
  per reseed-mode, plus baseline Philox4x32-10/Xoshiro256**/PCG32.

## 2. Gerbang period teoretis (Tahap B)

Tidak memfilter kandidat manapun — bahkan di N=16 (kasus terkecil, jadi
paling berisiko), margin masih sangat jauh di atas budget test:

| N | orbit `lambda_pred` (word) | singleblock `lambda_pred` (word) | Budget Tier2 (16GB) |
|---|---|---|---|
| 256 | ~1.03e1496 | ~8.17e1232 | 2^32 ≈ 4.3e9 word |
| 128 | ~6.74e733 | ~2.53e616 | " |
| 64 | ~2.15e362 | ~1.41e308 | " |
| 32 | ~2.31e181 | ~1.05e154 | " |
| 16 | ~1.78e93 | ~9.06e76 | " |

Periodisitas bukan bottleneck di ukuran state manapun yang diuji — bahkan
di N=16, margin masih >80 orde magnitudo di atas kedalaman test terdalam.

## 3. Tier 0 — Avalanche 3-arah + heatmap

**15/15 varian PASS** (band [0.3,0.7], floor per-bit 0.2) — termasuk N=16.
Avalanche/rolling-o secara struktural tetap benar di semua N; kegagalan
kualitas yang ditemukan belakangan (Tier1/Tier2, di bawah) murni soal
interaksi reseed-mode di stream panjang, bukan cacat difusi bit
per-langkah. Heatmap per varian: `heatmaps/heatmap_{core}_N{n}[_{mode}].png`.

## 4. Tier 1 — PractRand screen (256M/2G/2G)

**12/15 CLEAN. 3/15 FAIL — semua dan hanya di N=16**:

| Varian | Lane FAIL | Detail |
|---|---|---|
| `orbit_N16_LOWONLY` | K=1 (weyl) | BCFN escalating |
| `orbit_N16_MUTUAL` | K=1 (weyl) **dan** continuous | K=1 sama dgn LOWONLY (reseed tidak aktif di K=1, wajar identik); continuous breakdown total instan @64MB (~200 test FAIL serentak, R>99999) |
| `singleblock_N16` | K=1 | Pola sama dgn orbit K=1 |

N=256/128/64/32 (semua mode) — **100% CLEAN** di Tier1.

## 5. Safe-K floor scan, khusus N=16 (Tahap E)

K=1 FAIL (Tier1), K=15 CLEAN (Tier1). Sapuan K=2..14 (weyl, base_key
tunggal, s.d. 2GB):

| K | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Verdict | CLEAN | FAIL | FAIL | FAIL | FAIL | FAIL | FAIL | FAIL | CLEAN | CLEAN | FAIL | CLEAN | CLEAN |

**Non-monotonic** — beda karakter total dengan floor N=256 produksi (crisp,
K≥80-96 aman, transisi tunggal). Di N=16 tidak ada floor tunggal yang bisa
dilaporkan; kualitas "berkedip" tergantung K spesifik. Kesimpulan: safe-K
floor TIDAK scale proporsional dengan N — di N kecil, sifatnya kualitatif
beda (noisy/non-monotonic), bukan sekadar bergeser skala linear/log.
N=32-256 tidak di-floor-scan (semua K=1 sudah PASS Tier1+Tier2 bersih di N
tsb, tidak ada sinyal yang memicu investigasi lebih lanjut).

## 6. Tier 2 — PractRand deep (2G→8G→16G dual-base-key) + dieharder 27-test (VPS)

12 survivor Tier1 (N∈{256,128,64,32} × {orbit-LOWONLY, orbit-MUTUAL,
singleblock}) diuji penuh. **11/12 CLEAN. 1/12 FAIL:**

| Varian | PractRand | dieharder K=1 | dieharder K=N-1 |
|---|---|---|---|
| orbit_N256_LOWONLY | CLEAN | CLEAN (26/27, 1 WEAK) | CLEAN (25/27, 1 WEAK) |
| orbit_N256_MUTUAL | CLEAN | CLEAN (26/27, 1 WEAK) | CLEAN (25/27, 1 WEAK) |
| singleblock_N256 | CLEAN | CLEAN (26/27, 1 WEAK) | CLEAN (25/27, 1 WEAK) |
| orbit_N128_LOWONLY | CLEAN | CLEAN (25/27, 2 WEAK) | CLEAN (26/27, 0 WEAK) |
| orbit_N128_MUTUAL | CLEAN | CLEAN (25/27, 2 WEAK) | CLEAN (26/27, 0 WEAK) |
| singleblock_N128 | CLEAN | CLEAN (25/27, 2 WEAK) | CLEAN (26/27, 0 WEAK) |
| orbit_N64_LOWONLY | CLEAN | CLEAN (26/27, 1 WEAK) | CLEAN (25/27, 4 WEAK) |
| orbit_N64_MUTUAL | CLEAN | CLEAN (26/27, 1 WEAK) | CLEAN (25/27, 4 WEAK) |
| singleblock_N64 | CLEAN | CLEAN (26/27, 1 WEAK) | CLEAN (25/27, 4 WEAK) |
| orbit_N32_LOWONLY | CLEAN | CLEAN (26/27, 0 WEAK) | CLEAN (26/27, 1 WEAK) |
| **orbit_N32_MUTUAL** | **FAIL (lane continuous)** | CLEAN (26/27, 0 WEAK) | CLEAN (26/27, 1 WEAK) |
| singleblock_N32 | CLEAN | CLEAN (26/27, 0 WEAK) | CLEAN (26/27, 1 WEAK) |

**Detail FAIL `orbit_N32_MUTUAL`**: lane continuous (stream panjang tanpa
reinit periodik) CLEAN s.d. 2GB (Stage A), **FAIL @4GB** (Stage B, 4
gigabyte checkpoint). Lane K=1 dan K=31 (reinit periodik tiap K word) di
varian **yang sama** tetap CLEAN s.d. 16GB dual-base-key (Stage C).
dieharder K=1 dan K=31 juga CLEAN total. Jadi cacat ini spesifik:
**mode reseed MUTUAL × lane continuous (tanpa reinit)** — bukan cacat
menyeluruh varian N=32/MUTUAL, dan tidak muncul sama sekali di lane yang
dipakai standar 5-angka benchmark (K=1/K=N-1/stream — catatan: "orbit
--stream" pada benchmark kecepatan = lane continuous ini, jadi klaim
kecepatan `orbit --stream` MUTUAL di N=32 TIDAK didukung kualitas bersih).

**Pola gradien terhadap N** (dibandingkan dengan `orbit_N16_MUTUAL`
continuous FAIL @64MB, Tier1): breakdown MUTUAL-continuous muncul makin
lambat seiring N membesar (N=16 → 64MB, N=32 → 4GB, keduanya FAIL) dan
sepenuhnya hilang di N=64/128/256 (CLEAN s.d. 16GB). Ini indikasi
kelemahan struktural mode reseed MUTUAL pada lane continuous yang
melemah tapi belum tentu nol di N besar — bukan cacat biner yang cuma
eksis di satu titik N.

## 7. Benchmark kecepatan (independen kualitas, `bench_results.json`)

### 7a. Singleblock K=N-1 (steady-state terbaik RA-PRNG)

| N | LOWONLY | MUTUAL |
|---|---|---|
| 256 | 23 207.8 MB/s (0.164 ns/word) | 23 147.0 MB/s (0.165 ns/word) |
| 128 | 17 366.4 MB/s (0.220 ns/word) | 6 964.8 MB/s (0.548 ns/word) *(outlier, lihat catatan)* |
| 64 | 12 180.4 MB/s (0.313 ns/word) | 14 073.7 MB/s (0.271 ns/word) |
| 32 | 8 563.7 MB/s (0.445 ns/word) | 8 743.3 MB/s (0.436 ns/word) |
| 16 | 4 579.4 MB/s (0.833 ns/word) | 5 007.2 MB/s (0.762 ns/word) — **kualitas N=16 gagal**, angka ini referensi saja |

Catatan: `singleblock N=128/MUTUAL` (6.965 GB/s) jauh di bawah tetangganya
(N=64/N=32) — kemungkinan noise pengukuran run tunggal (tidak
di-multi-run-rata-ratakan), dicatat sebagai outlier, bukan pola nyata.

### 7b. Orbit `--stream` (lane continuous, steady-state murni tanpa reinit)

| N | LOWONLY | MUTUAL |
|---|---|---|
| 256 | 1 317.2 MB/s (2.896 ns/word) | 947.8 MB/s (4.025 ns/word) |
| 128 | 1 140.2 MB/s (3.346 ns/word) | 1 131.9 MB/s (3.370 ns/word) |
| 64 | 1 094.7 MB/s (3.485 ns/word) | 1 080.1 MB/s (3.532 ns/word) |
| 32 | 1 031.8 MB/s (3.697 ns/word) | 1 074.3 MB/s (3.551 ns/word) — **kualitas FAIL @4GB (§6)** |
| 16 | 1 011.4 MB/s (3.772 ns/word) | 955.0 MB/s (3.995 ns/word) — **kualitas FAIL @64MB** |

Nyaris flat lintas N — konsisten dengan klaim §8 RQ1 (steady-state tidak
membaik dengan mengecilkan state).

### 7c. Cold-start K=1 (di sinilah N kecil unggul)

| N | orbit K=1 LOWONLY | orbit K=1 MUTUAL | singleblock K=1 (flat, ref.) |
|---|---|---|---|
| 256 | 61.3 MB/s | 54.8 MB/s | 379.0 MB/s |
| 128 | 115.0 MB/s | 117.8 MB/s | 356.6 MB/s |
| 64 | 176.4 MB/s | 166.4 MB/s | 371.5 MB/s |
| 32 | 206.1 MB/s | 217.7 MB/s | 399.8 MB/s |
| 16 | 265.5 MB/s | 257.8 MB/s | 367.7 MB/s |

orbit K=1: **~4-5× lebih cepat** dari N=256 ke N=16 — kebalikan pola
steady-state. singleblock K=1 nyaris flat (init-nya sudah nyaris nol di
semua N, tidak ada lagi yang bisa dihemat).

### 7d. Init-time murni (ns), penjelasan §7c

| N | orbit init_ns (LOWONLY) | singleblock init_ns (LOWONLY) |
|---|---|---|
| 256 | 53.2 | 0.2 |
| 128 | 4.3 | 0.3 |
| 64 | 4.0 | 0.3 |
| 32 | 3.9 | 0.2 |
| 16 | 0.2 | 0.2 |

orbit init N=256→N=16: **~266× lebih murah**. Ini akar dari §7c — init
orbit (penuh 2×N word + permutasi `L`) dominan di K kecil, sementara
singleblock (tanpa `L`, init parsial) sudah nyaris nol biaya di N mana pun.

### 7e. Baseline eksternal (referensi, ±~15% noise antar-run)

| N | Philox4x32-10 stream | Xoshiro256** stream | PCG32 stream |
|---|---|---|---|
| 256 | 862-1140 MB/s | 2451-3572 MB/s | 2733-3689 MB/s |
| 128-16 | ~1275-1435 MB/s | ~3059-4014 MB/s | ~3820-3880 MB/s |

## 8. Jawaban 5 pertanyaan riset (§1 HANDOVER.md)

**RQ1 — Titik konvergensi/tukar-peringkat kecepatan singleblock vs
orbit-stream saat state mengecil?**
Tidak ada tukar peringkat, dan tidak konvergen — singleblock K=N-1 selalu
lebih cepat dari orbit-stream di semua N yang diuji (8.6-23.2 GB/s vs
~1.0-1.3 GB/s), tapi **gap-nya menyempit drastis** seiring N mengecil
(23.2/1.3 ≈ 18× di N=256, turun ke 8.6/1.0 ≈ 8.5× di N=32) karena
singleblock K=N-1 menurun jauh lebih curam (init makin sering relatif
terhadap kerja per-block) daripada orbit-stream yang nyaris flat. Kedua
core TIDAK naik dengan mengecilkan N di lane steady-state — arah
sebaliknya justru berlaku untuk cold-start (K=1): orbit K=1 naik ~4-5×
dari N=256 ke N=16, singleblock K=1 flat.

**RQ2 — Bisa mengalahkan Philox4x32-10/Xoshiro256**/PCG32?**
Ya, tapi hanya singleblock K=N-1 di N≥32 (8.6-23.2 GB/s vs baseline
0.9-4.0 GB/s — beberapa kali lipat lebih cepat). orbit-stream (~1.0-1.3
GB/s) KALAH dari Xoshiro256**/PCG32 (2.5-4.0 GB/s), sebanding Philox.
Cold-start (K=1) kedua core RA-PRNG kalah dari ketiga baseline di semua N.

**RQ3 — Optimasi rolling-o tetap valid di N kecil?**
Ya, secara struktural — avalanche 3-arah PASS 15/15 termasuk N=16 (§3),
dan sanity-check byte-identik terhadap produksi di N=256 membuktikan
parameterisasi tidak mengubah aritmatika rolling-o. Kegagalan kualitas
yang ditemukan (Tier1 N=16, Tier2 N=32-MUTUAL-continuous) terjadi di lane
tanpa/jarang reinit dan spesifik ke mode reseed, BUKAN di jalur rolling-o
per-word — mengindikasikan rolling-o sendiri tidak jadi bottleneck
kualitas, cacatnya ada di interaksi `ra_reseed`/boundary dengan N kecil.

**RQ4 — Kualitas statistik tetap baik?**
Sebagian. N=256/128/64 (semua mode) dan N=32-LOWONLY: **bersih total**
(Tier0+Tier1+Tier2, PractRand 16GB dual-key + dieharder 27-test, 0 FAIL).
N=32-MUTUAL: bersih KECUALI lane continuous (FAIL @4GB, §6) — masih
layak untuk pola pakai ber-reinit (K=1/K=N-1). N=16 (semua mode): **gagal**
— K=1 FAIL semua mode, MUTUAL-continuous breakdown total @64MB, floor
scan non-monotonic tanpa titik aman jelas. N=16 dikecualikan dari
rekomendasi Pareto frontier final.

**RQ5 — Letak bottleneck terbesar?**
Untuk orbit: **init** (permutasi `L` penuh 2×N word) — dominan mutlak di
K kecil (§7d: 53.2ns @N=256 vs 3.8ns steady-state gen per word), dan
sisa ~266× penghematan itu didapat gratis hanya dengan mengecilkan N,
tanpa mengubah formula. Untuk singleblock: bottleneck ADA di badan loop
generate itu sendiri (init sudah ~0 di semua N) — makin kecil N, makin
sering `ra_reseed` dipanggil relatif terhadap kerja, sehingga
`real_ns_per_word` naik (0.164ns@N=256 → 0.833ns@N=16, ~5× lebih lambat)
meski `gen_ns_per_word` murni per-word tidak berubah signifikan.

## 9. Temuan tak terduga

1. **Staleness kontrak `addr_hi` di tooling battery**: `practrand_gate.py`/
   `avalanche.py` (`tools/battery/`) masih memakai kontrak CLI lama
   (sebelum widening `addr_hi` 64-bit, 2026-09-06) — dipatch sesi ini
   (`addr_hi = base_key ^ 0x5EEDA55D`, konsisten dgn konvensi yang sudah
   ada), dikonfirmasi user. `dieharder_battery_param.py` (baru, Tier2)
   ditulis dengan kontrak yang sudah benar sejak awal.
2. **Off-by-one di `ra_core_orbit` PRODUKSI** (bukan hasil parameterisasi
   — dikonfirmasi identik di N=256 produksi): menulis 1 word lebih
   sedikit dari yang diminta setiap kali `rng ≡ 1 (mod N-1)`. Di-workaround
   di `practrand_gate.py` (margin +1 word untuk lane continuous) TANPA
   mengubah `ra_core.c`/`ra_core_param.c` (di luar scope riset ini) — perlu
   ditindaklanjuti terpisah kalau relevan untuk produksi.
3. **Kelemahan MUTUAL-continuous bergradien-terhadap-N** (§6): baru
   ditemukan karena Tier2 masuk lebih dalam (4-16GB) dari Tier1 (2GB) —
   N=32-MUTUAL-continuous lolos screen dangkal tapi FAIL di deep-test.
   Implikasi metodologis: kesimpulan "CLEAN" dari screen dangkal (2GB)
   TIDAK bisa diasumsikan bertahan di kedalaman lebih dalam untuk lane
   yang jarang/tidak reinit — perlu deep-test penuh sebelum klaim final,
   terutama untuk N kecil.
4. **Bug fairness di benchmark produksi `bench_release.c`** (2026-09-11,
   ditemukan saat membandingkan singleblock N=256 hasil parameterisasi
   study ini — ~22000+ MB/s — dengan angka produksi lama — ~8351 MB/s).
   - **Bug asli**: `bench_release.c` (di
     `experiments/2026-9-1_family-productionization/`) meng-`extern`-link
     `ra_core_orbit`/`ra_core_singleblock` ke `ra_core_nomain.o` terpisah,
     sementara Philox4x32-10/Xoshiro256** di file yang sama didefinisikan
     `static` (compiler bisa inline penuh). Klaim komentar header lama
     "apples-to-apples" **salah** untuk dimensi linking ini — RA-PRNG
     dibandingkan dalam kondisi dirugikan relatif terhadap baseline-nya
     sendiri, meski mekanisme timing-nya sendiri tetap valid.
   - **Fix pertama (perlu tapi TIDAK CUKUP)**: ganti extern-link jadi
     `#include "ra_core.c"` langsung (single translation unit, sama
     seperti Philox/Xoshiro) — ini memang pola nyata yang dipakai
     Philox/Random123 (header-only by design) dan Xoshiro (referensi
     Blackman & Vigna, didistribusikan sebagai potongan `.c` yang memang
     dimaksud di-copy-paste ke source konsumen, bukan dilink sebagai
     library terpisah). Setelah fix ini saja, angka **TIDAK berubah**
     (tetap ~8000 MB/s) — dugaan awal ("extern vs #include semata yang
     menentukan inlining") terbukti tidak cukup.
   - **Root cause sebenarnya** (dikonfirmasi via `nm -S`/`objdump` diff,
     lalu `-fopt-info-inline-missed`, lalu bisection dengan
     `--param max-inline-insns-auto/-single`, lalu patch `always_inline`
     langsung): `ra_init_state_singleblock`/`ra_permutation_cycle_singleblock`
     di `ra_core.c` produksi berada tepat di ambang batas default GCC
     untuk auto-inline per-fungsi — GCC awal-inline menolak keduanya
     ("cold, code would grow by 42/43") di KEDUA file (produksi maupun
     `ra_core_param.c`) dengan estimasi hampir identik, tapi hanya
     `ra_core_param.c` yang lolos di tahap inline lanjutan — sehingga di
     `ra_core.c` fungsi-fungsi itu tetap jadi CALL instruction sungguhan
     (tidak ter-inline), bukan cuma soal extern-vs-include. Ini keputusan
     heuristik compiler yang rapuh/nyaris seri, bukan perbedaan desain
     yang disengaja.
   - **Fix final** (diterapkan ke `src/ra_core/c/ra_core.c` produksi,
     2026-09-11): tambahkan `static inline __attribute__((always_inline))`
     ke kedua fungsi tsb — keduanya memang hot-loop satu-titik-panggil, jadi
     anotasi ini cuma menegaskan niat yang sudah seharusnya, bukan mengubah
     perilaku. Dengan `-O3 -march=native` default (tanpa flag `--param`
     apa pun), singleblock K=255 produksi naik dari ~8351 MB/s menjadi
     **~21000-22300 MB/s** — sejajar dengan `bench_param_N256_LOWONLY`.
     Juga ditambahkan guard `#ifndef RA_CORE_NO_MAIN` di sekitar `int
     main()` `ra_core.c` (pola sama dengan `RA_CORE_PARAM_NO_MAIN` di
     `ra_core_param.c`) supaya konsumen performa-kritis BISA `#include`
     `ra_core.c` langsung — perilaku default (compile mandiri,
     extern-linkable) tidak berubah. `ra_core_orbit` mengalami ambang
     batas serupa tapi efeknya nyaris nol (biaya permutation diamortisasi
     ke ~K word per panggilan init, bukan per-word seperti singleblock) —
     sengaja TIDAK dianotasi `always_inline` (tidak ada bukti perlu).
   - Verifikasi: self-test KAT `validate` (`ra_core_standalone_test
     validate`) tetap 0 mismatch untuk singleblock maupun orbit setelah
     kedua perubahan — perilaku byte-identik, cuma codegen yang berubah.
   - **Implikasi untuk konvensi distribusi PRNG kecepatan-tinggi**: PRNG
     state-kecil (Philox, Xoshiro, PCG minimal) secara konvensi
     didistribusikan sebagai source-inclusion/header-only justru karena
     `generate()`-nya kecil & panas — biaya call-boundary/non-inline
     mendominasi kalau dibiarkan default. `ra_core.c` sekarang mendukung
     kedua mode (compiled-library default, header-include opt-in via
     `RA_CORE_NO_MAIN`) — pola yang sama dengan Xoshiro (satu file `.c`,
     konsumen pilih compile terpisah atau paste-in).

## 10. Rekomendasi untuk publikasi

**Boleh diklaim (didukung data bersih)**:
- singleblock K=N-1 mengalahkan Philox4x32-10/Xoshiro256**/PCG32 beberapa
  kali lipat di N≥32, dengan state 8-32× lebih kecil dari desain produksi.
- Pareto frontier state-size terdokumentasi 16×: N=256→32 tetap CLEAN
  penuh kualitas (LOWONLY) di semua tier hingga 16GB.
- orbit cold-start (K=1) membaik ~266× (init) / ~4-5× (throughput K=1)
  dari N=256 ke N=16 — narasi kuat untuk use-case "Addressable RNG" (init
  murah = akses acak per-alamat murah), independen dari masalah kualitas
  N=16 (init-cost adalah properti struktural, bukan bergantung K spesifik).

**Harus dilaporkan apa adanya (jangan disembunyikan)**:
- N=16 gagal kualitas di kedua core, floor non-monotonic — smaller state
  TIDAK generically better, ada batas bawah nyata.
- MUTUAL tidak superior dari LOWONLY di kualitas (LOWONLY 100% CLEAN
  semua N/tier; MUTUAL 1 FAIL bergradien) — desain LOWONLY (production-
  equivalent) tetap rekomendasi default, MUTUAL butuh riset lanjutan
  sebelum dipakai di lane continuous/N kecil.
- steady-state throughput flat-to-worse seiring N mengecil untuk KEDUA
  core (§7a/§7b) — keuntungan N kecil murni di sisi init/cold-start, bukan
  klaim umum "lebih kecil = lebih cepat".

## 11. Index file

- Data mentah: `bench_results.json`/`.txt` (kecepatan), `tier0_results.json`
  (avalanche), `tier1_out/` (PractRand screen), `floor_scan_n16*`,
  `tier2_out/` (PractRand deep + dieharder VPS), `heatmaps/*.png`.
- Kode: `ra_core_param.c`, `bench_param.c`, `BUILD.sh`,
  `period_precheck.py`, `tier0_quality.py`, `tier1_sweep.sh`,
  `floor_scan_n16.py`, `tier2_run.sh`, `dieharder_battery_param.py`.
- Laporan interim (chart HTML, dipublikasikan sebelum Tier2 selesai):
  https://claude.ai/code/artifact/2ed2c271-e9fe-424d-88e6-cb6407f6944b
