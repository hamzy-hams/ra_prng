# Promosi Kandidat D (64-bit address widening) ke `ra_core.c` kanonik — HANDOVER

## Tujuan

Mempromosikan formula Kandidat D — hasil riset
`experiments/2026-9-5_orbit-address-widen-64bit/` yang memperbaiki defect
birthday-paradox pada key 32-bit (root cause didiagnosis di
`experiments/2026-9-5_orbit-k1-exhaustive-function-analysis/`) — dari kode
eksperimental (`candidate_d.c`) menjadi bagian dari `ra_core.c` kanonik yang
dipakai semua tooling produksi/battery di repo ini.

## Scope IN

1. Edit `ra_core.c`: tambah parameter `addr_hi` ke `ra_core_orbit`/
   `ra_core_singleblock` dan fungsi init internalnya, ikuti formula
   Kandidat D persis (lihat `../2026-9-5_orbit-address-widen-64bit/candidates/candidate_d.c`).
2. Regenerasi KAT checksum (`SINGLEBLOCK_KAT_CHECKSUMS`/`ORBIT_KAT_CHECKSUMS`)
   dengan baseline `addr_hi=0`.
3. Byte-identical merge-sanity-check terhadap `candidate_d.c` berdiri
   sendiri.
4. Update SEMUA (13) file caller supaya tetap kompilasi terhadap signature
   4-arg baru, plus `bench_release.c` (kasus khusus: dipakai juga oleh
   Philox/Xoshiro yang harus tetap 3-arg).
5. Redo 2 benchmark wajib (avalanche 3-skema, throughput 5-mode) terhadap
   `ra_core.c` PASCA-promosi, bandingkan jujur ke baseline
   `experiments/2026-9-6_release-benchmark-prep/` (pra-promosi).
6. Update `README.md` bagian `### ra_core` — sekarang WAJIB diedit (bukan
   kondisional) karena formula/signature publik betul-betul berubah.
7. Koreksi memory yang sudah usang (`project_orbit_k1_exhaustive_function_analysis.md`
   masih bilang "zero code written yet").

## Scope OUT (dicatat, tidak dikerjakan sesi ini)

- **`experiments/2026-9-5_orbit-k1-exhaustive-function-analysis/exhaustive_analysis.c`**
  — `f_key()` mereimplementasi formula lama (pra-D) secara manual untuk
  analisis defect lama. Tidak di-extern-link ke `ra_core.c` jadi tidak akan
  gagal kompilasi meski sudah stale — hanya ditambah komentar peringatan
  stale di atas file (2026-09-06), TIDAK di-re-derive ke formula D karena
  analisis itu spesifik soal defect key-repetition lama, bukan reimplementasi
  yang harus selalu sinkron.
- **`tools/battery/driver_main_template.c` + `tools/battery/README.md`** —
  kontrak CLI generik dipakai `avalanche.py`/`practrand_gate.py`/
  `run_battery.py`, belum punya slot `addr_hi`. Perlu redesign substansial
  di luar scope task promosi ini — item tertunda untuk sesi berikutnya kalau
  ada kebutuhan menjalankan battery generik dengan `addr_hi` non-default.
- Re-run PractRand 1TB / dieharder 8-kombinasi / FPF collision-scan skala
  penuh pasca-merge — dilewati per keputusan user (re-validasi "ringan"):
  battery berat sudah dijalankan exhaustive terhadap `candidate_d.c` yang
  struktur formulanya identik byte-for-byte (diverifikasi Fase 3), jadi
  hasil itu berlaku turunan untuk `ra_core.c` yang baru.

## Keputusan desain kunci

- **CLI backward-compatible**: `--stream <mode> <seed> <n>` (4 argumen)
  tetap jalan, `addr_hi` implisit 0; bentuk baru `--stream <mode> <seed>
  <addr_hi> <n>` (5 argumen) untuk eksplisit. Alasan: `avalanche_ra_core.py`/
  `avalanche_curve_ra_core.py` yang sudah ada tidak perlu diubah kodenya.
- **`bench_release.c`**: karena C tidak punya default-arg/overload dan
  casting function-pointer antar-arity berbeda adalah undefined behavior,
  dibuat fungsi baru `bench_real_reinit4`/`bench_real_stream4` (salinan
  identik `bench_real_reinit`/`bench_real_stream` dengan tipe pointer 4-arg)
  khusus untuk 5 call-site `ra_core_*`. Fungsi 3-arg asli dibiarkan utuh
  untuk Philox/Xoshiro.
- **`addr_hi=0` sebagai baseline KAT**: konsisten dengan makna lama "satu
  address global" — tidak mengubah interpretasi checksum yang sudah ada di
  seluruh tooling `--verify`/`validate`.

## Status akhir sesi ini

Semua 8 fase (lihat plan Sesi Plan Mode) DONE. Lihat `RESULTS.md` di folder
ini untuk bukti tiap fase, tabel benchmark before/after, dan daftar lengkap
file yang diubah.
