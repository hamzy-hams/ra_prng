# Release benchmark prep for `ra_core.c` — RESULTS

Scope: persiapan dua benchmark yang jadi syarat minimal sebelum promosi
`experiments/2026-9-1_family-productionization/ra_core.c` ke `src/` — (1)
avalanche-effect curve dengan metodologi paper original (CSAI2025/ICCS2026),
dan (2) konfirmasi ulang throughput 5-mode standar. **Tidak** memindahkan
`ra_core.c` ke `src/` (belum di-scope, keputusan terpisah). Periodicity test
dan Fisher-Yates shuffle benchmark (bagian lain metodologi paper) **ditunda**
per keputusan user 2026-09-06.

## 1. Avalanche-effect curve, 3 skema (bit-flip, incremental, power-of-two)

Metodologi persis paper Section III.A.2: base seed `0x00000001`, K=255
(block/cycle pertama), 32 seed diperturbasi per skema, kurva = rata-rata
Hamming distance (bit, 0-32) per posisi output (0-254) lintas 32 seed.
Dijalankan untuk **kedua core** (`orbit`, `singleblock`) via
`avalanche_curve_ra_core.py` (reuse `capture_cycle()` dari
`avalanche_ra_core.py`, subprocess ke `ra_core --stream <mode> <seed> 255`).

| Core | Skema | overall_mean_hamming_fraction | Band [0.3, 0.7] |
|---|---|---:|---|
| orbit | bitflip | 0.498828 | OK |
| orbit | incremental | 0.501463 | OK |
| orbit | powoftwo | 0.499717 | OK |
| singleblock | bitflip | 0.498828 | OK |
| singleblock | incremental | 0.501463 | OK |
| singleblock | powoftwo | 0.499717 | OK |

**Semua 6 kombinasi core×skema masuk band `[0.3,0.7]`** — termasuk
incremental dan power-of-two yang sebelumnya belum pernah diuji terhadap
`ra_core.c` sama sekali (hanya bit-flip yang sudah digate lewat
`avalanche_ra_core.py`). Tidak ada temuan baru yang perlu dieskalasi.

**Catatan penting**: angka `orbit` dan `singleblock` **identik persis**
untuk ketiga skema. Ini dikonfirmasi BUKAN bug — diverifikasi langsung
bahwa `capture_cycle("orbit", seed)` dan `capture_cycle("singleblock",
seed)` menghasilkan array 255-word yang **byte-identik** untuk seed yang
sama. Ini konsisten dengan arsitektur `ra_core.c` yang sudah terdiagnosis
sebelumnya ([[project_singleblock_kmin_defect]]): `singleblock` pada
dasarnya adalah `orbit` yang dibatasi ke 255 kata/key tanpa reseed —
keduanya berbagi init+transform yang sama untuk block pertama, jadi hasil
avalananche di K=255 memang seharusnya sama. File output tetap dipisah per
core (2 CSV set + 2 PNG) untuk kelengkapan dokumentasi, bukan karena
diharapkan berbeda.

Output: `avalanche_curve_{orbit,singleblock}_{bitflip,incremental,powoftwo}.csv`
(6 file, 255 baris data tiap file) + `avalanche_curve_{orbit,singleblock}.png`
(2 file, 3 subplot per skema).

## 2. Throughput 5-mode standar — re-run 2026-09-06

Rebuild `bench_release` dari `ra_core.c`+`bench_release.c` (keduanya
tanpa perubahan sejak commit `be4a14a` 2026-09-04, dikonfirmasi via
`git diff` kosong setelah rebuild), jalankan ulang `taskset -c 0
./bench_release all`. Hasil mentah: `bench_release_rerun_2026-09-06.txt`.
Baseline lama: `../2026-9-1_family-productionization/bench_release_results.txt`.

| Mode | Real MB/s (2026-09-04) | Real MB/s (2026-09-06) | Delta % |
|---|---:|---:|---:|
| singleblock K=1 | 371.59 | 372.14 | +0.15% |
| singleblock K=255 | 8351.05 | 8575.67 | +2.69% |
| orbit multistream K=1 | 66.71 | 69.20 | +3.73% |
| orbit multistream K=255 | 1299.98 | 1364.21 | +4.94% |
| orbit --stream | 1362.38 | 1408.58 | +3.39% |
| philox4x32-10 K=1 (ref) | 310.96 | 312.00 | +0.33% |
| philox4x32-10 K=255 (ref) | 1374.36 | 1434.35 | +4.36% |
| philox4x32-10 stream (ref) | 1378.27 | 1441.18 | +4.56% |
| xoshiro256\*\* K=1 (ref) | 580.22 | 579.90 | -0.06% |
| xoshiro256\*\* K=255 (ref) | 3673.50 | 3800.07 | +3.45% |
| xoshiro256\*\* stream (ref) | 3934.89 | 4049.31 | +2.91% |

**Semua 11 baris dalam toleransi ≤5%** (maks 4.94%, orbit multistream
K=255). Kenaikan ~3-4.5% pada hampir semua baris — **termasuk referensi
Philox/Xoshiro yang tidak disentuh sama sekali** — konsisten dengan
variasi sistem run-to-run (mis. CPU frequency scaling/thermal state saat
run), bukan perubahan spesifik pada `ra_core.c`. Rasio perbandingan
naratif di README (mis. "singleblock K=255 6.1x lebih cepat dari Philox")
tetap valid: 8575.67/1434.35 = 5.98x (vs 6.1x lama, beda sangat kecil,
kesimpulan kualitatif tidak berubah).

**Kesimpulan: README TIDAK diedit** — angka existing tetap representatif
dan reproducible, confirmed 2026-09-06.

## 3. Kesimpulan keseluruhan

Kedua benchmark wajib (avalanche-per-cycle K=255 3-skema, throughput
5-mode standar) sekarang tersedia dan PASS/reproducible untuk `ra_core.c`,
kedua core. Tidak ada blocker baru untuk promosi ke `src/` dari sisi
benchmark. Keputusan promosi fisik itu sendiri (nama modul, API surface,
package layout) tetap di luar scope task ini — lihat
`PRODUCTION_READINESS_HANDOVER.md` §8.
