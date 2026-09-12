# HANDOVER — release-benchmark-prep (2026-09-06)

## Tujuan

Persiapan dua benchmark wajib sebelum promosi `ra_core.c`
(`experiments/2026-9-1_family-productionization/ra_core.c`) ke `src/`
sebagai source produksi resmi: (1) avalanche-effect curve dengan
metodologi paper original (CSAI2025/ICCS2026), (2) konfirmasi ulang
throughput 5-mode standar.

## Scope

- IN: avalanche curve full 3-skema (bit-flip, incremental, power-of-two),
  K=255, kedua core (orbit, singleblock); re-run `bench_release` untuk
  konfirmasi reproducibility; cek/perbarui README kalau perlu.
- OUT (ditunda per keputusan user): periodicity test, Fisher-Yates shuffle
  benchmark (bagian lain metodologi paper); promosi fisik `ra_core.c` ke
  `src/` itu sendiri (keputusan API/package terpisah, belum di-scope).

## Hasil

Lihat `RESULTS.md` untuk detail lengkap. Ringkas:

- Avalanche 3-skema × 2 core = 6 kombinasi, **semua PASS band [0.3,0.7]**
  — termasuk incremental & power-of-two yang belum pernah diuji terhadap
  `ra_core.c` sebelumnya. Temuan: `orbit` dan `singleblock` menghasilkan
  kurva avalanche **identik** untuk K=255 (byte-identik, dikonfirmasi
  bukan bug — konsekuensi arsitektur, `singleblock` = `orbit` dibatasi
  255 kata/key, berbagi init+transform block pertama).
- Throughput re-run: semua 11 baris (5 mode `ra_core` + Philox + Xoshiro
  referensi) dalam toleransi ≤5% dari baseline 2026-09-04 (maks 4.94%).
  Kenaikan seragam ~3-4.5% termasuk di baris referensi → variasi sistem,
  bukan perubahan `ra_core.c`. **README tidak diedit** — angka existing
  tetap representatif.
- Tidak ada blocker baru untuk promosi ke `src/` dari sisi benchmark.

## File yang dihasilkan

- `avalanche_curve_ra_core.py` — script baru (3 skema × 2 core)
- `avalanche_curve_{orbit,singleblock}_{bitflip,incremental,powoftwo}.csv` (6 file)
- `avalanche_curve_{orbit,singleblock}.png` (2 file)
- `bench_release_rerun_2026-09-06.txt` — stdout mentah re-run
- `RESULTS.md` — tabel banding + kesimpulan

## Next steps (di luar scope task ini)

- Keputusan promosi fisik `ra_core.c` ke `src/`: nama modul, API surface,
  package layout — belum di-scope sama sekali, perlu keputusan user
  terpisah (lihat `PRODUCTION_READINESS_HANDOVER.md` §8).
- Periodicity test dan Fisher-Yates shuffle benchmark dari paper original,
  kalau user mau kesetaraan penuh dengan metodologi paper sebelum rilis.
