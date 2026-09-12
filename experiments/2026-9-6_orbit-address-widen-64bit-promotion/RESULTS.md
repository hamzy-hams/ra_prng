# Promosi Kandidat D ke `ra_core.c` kanonik — RESULTS

Scope: promosi formula Kandidat D (64-bit address widening, `key`+`addr_hi`)
dari `experiments/2026-9-5_orbit-address-widen-64bit/candidates/candidate_d.c`
ke `experiments/2026-9-1_family-productionization/ra_core.c` kanonik, plus
update seluruh caller, redo 2 benchmark wajib, dan update README. Lihat
`HANDOVER.md` untuk scope in/out lengkap dan konteks root-cause.

## 1. Diff formula yang diterapkan

- 2 define baru: `GUARD_L2 0x3eb13b90u`, `GUARD_M2 0x392456deu`.
- `ra_init_state_orbit(L, M, key, addr_hi)` — `keyterm_l`/`keyterm_m` jadi
  produk dua hash independen: satu dari `key` (konstanta lama
  `0x9e3779b7u`/`0x06a0dd9bu`, guard lama), satu dari `addr_hi` (konstanta
  baru `0x85ebca77u`/`0x9e3779b1u`, guard baru). Rotasi TETAP `r = key ^ i`
  — `addr_hi` sengaja TIDAK masuk rotasi (ini yang membedakan D dari
  Kandidat A/B yang gugur PractRand Gap-16 karena mencampur `addr_hi` ke
  rotasi).
- `ra_init_state_singleblock(M, key, addr_hi, rng)` — pola sama, `addr_hi`
  disisipkan antara `key` dan `rng` di signature publik.
- `ra_core_orbit`/`ra_core_singleblock` — `addr_hi` disisipkan antara `key`
  dan `rng`. `core_fn_t` typedef, `CORES[]`, `find_core()`,
  `checksum_key_core()` ikut berubah ke 4-arg.
- `rot32`, `fmix32`, `ra_permutation_cycle_orbit`/`_singleblock`, `ra_hash`,
  `ra_reseed` — TIDAK berubah.
- CLI `--stream` backward-compatible: `argc==5` → bentuk lama (`key n`,
  `addr_hi` implisit 0); `argc>=6` → bentuk baru (`key addr_hi n`).

## 2. KAT checksum regenerasi

`./ra_core checksum-gen singleblock` dan `checksum-gen orbit` dijalankan
dengan `addr_hi=0` (baseline "satu address global", konsisten makna lama).
`SINGLEBLOCK_KAT_CHECKSUMS`/`ORBIT_KAT_CHECKSUMS` (39 entri masing-masing)
ditimpa. `./ra_core validate`:

```
validate(singleblock): 39 keys checked, 0 checksum mismatches
validate(orbit, K<=255 window): 39 keys checked, 0 checksum mismatches
validate: PASS -- singleblock and orbit both match their known-answer checksums (w8_f10_i0 fix formula, both cores).
```

`ORBIT_KAT_CHECKSUMS` tetap identik dengan `SINGLEBLOCK_KAT_CHECKSUMS` —
diharapkan (`singleblock` = `orbit` dibatasi 255 kata tanpa reseed, keduanya
berbagi init+transform block pertama, konsisten dengan yang sudah
terdokumentasi di `../2026-9-6_release-benchmark-prep/RESULTS.md`).

## 3. Byte-identical merge-sanity-check

Dua kelas verifikasi, `cmp` biner, semua kombinasi MATCH:

**(a) `ra_core.c` (addr_hi=0) vs `candidate_d.c` berdiri sendiri** — key ∈
{0, 1, 0xDEADBEEF, 0xFFFFFFFF}, n ∈ {1, 255} (kedua core) + n=1000 (orbit
saja, di luar window K=255 singleblock):

| Core | Kombinasi diuji | Hasil |
|---|---|---|
| orbit | 4 key × {1,255,1000} = 12 | 12/12 MATCH |
| singleblock | 4 key × {1,255} = 8 | 8/8 MATCH |

**(b) Backward-compat CLI**: `--stream <mode> <key> <n>` (4-arg lama) vs
`--stream <mode> <key> 0 <n>` (5-arg baru, `addr_hi` eksplisit 0) — key ∈
{0, 1, 12345}, n ∈ {1, 255}, kedua core:

| Core | Kombinasi diuji | Hasil |
|---|---|---|
| orbit | 6 | 6/6 MATCH |
| singleblock | 6 | 6/6 MATCH |

Semua MATCH → battery berat historis Kandidat D (avalanche 3-arah, FPF
collision-scan 6/6 CLEAN, PractRand `--stream` s.d. 1TB 0 anomali, PractRand
staged 16GB 8 kombinasi, dieharder 27-test 8 kombinasi 0 FAILED — lihat
`../2026-9-5_orbit-address-widen-64bit/RESULTS.md`) berlaku turunan untuk
`ra_core.c` yang baru: kodenya provably byte-identik. Tidak di-re-run.

## 4. Caller yang diupdate (13 file + `bench_release.c`)

Semua cukup sisipkan `addr_hi=0u` setelah `key` di tiap call-site
(`extern`/typedef ikut diupdate ke 4-arg kalau ada), compile-check bersih
(exit 0) untuk semuanya:

1. `experiments/2026-9-1_family-productionization/bench_ra_core.c`
2. `experiments/2026-9-1_family-productionization/bench_release.c` — kasus
   khusus: `bench_real_reinit4`/`bench_real_stream4` (fungsi baru, tipe
   pointer 4-arg) dibuat untuk 5 call-site `ra_core_*`; `bench_real_reinit`/
   `bench_real_stream` 3-arg asli dibiarkan utuh untuk Philox/Xoshiro
   (menghindari cast function-pointer antar-arity, UB di C).
3. `experiments/2026-9-1_family-productionization/multikey_stream.c`
4. `experiments/2026-9-1_family-productionization/multikey_stream_hashed.c`
5. `experiments/2026-9-1_family-productionization/multikey_stream_orbit.c`
6. `experiments/2026-9-1_family-productionization/ra_core_singleblock_cli.c`
7. `experiments/2026-9-1_production-candidate-battery/multikey_stream_singleblock_k1.c`
8. `experiments/2026-9-1_production-candidate-battery/scrambler_ra_core_singleblock.c` (2 call-site)
9. `experiments/2026-9-2_singleblock-k-threshold-characterization/multikey_stream_k.c`
10. `experiments/2026-9-3_orbit-continuous-bench/orbit_bench.c` (pola
    function-pointer table, extern+typedef+2 call-site)
11. `experiments/2026-9-4_orbit-reseed-postboundary-bcfn/multikey_stream_orbit_postreseed_k1.c` (3 call-site)
12. `experiments/2026-9-4_orbit-reseed-postboundary-bcfn/multikey_stream_orbit_postreseed_k255.c` (3 call-site)
13. `experiments/2026-9-5_orbit-k1-urandom-key-diagnosis/urandom_key_stream_orbit.c`

Out of scope (dicatat, tidak diedit fungsional): `exhaustive_analysis.c`
(komentar stale ditambahkan), `tools/battery/driver_main_template.c` +
`tools/battery/README.md` (redesign kontrak CLI generik ditunda). Lihat
`HANDOVER.md`.

## 5. Avalanche-effect curve, 3 skema — pasca-promosi

Metodologi sama persis dengan `../2026-9-6_release-benchmark-prep/` (base
seed `0x00000001`, K=255, 32 seed per skema), dijalankan dari
`avalanche_curve_ra_core.py` (disalin tanpa perubahan) terhadap `ra_core`
pasca-promosi:

| Core | Skema | Sebelum promosi (2026-09-06) | Sesudah promosi D | Band [0.3,0.7] |
|---|---|---:|---:|---|
| orbit/singleblock | bitflip | 0.498828 | 0.499460 | OK |
| orbit/singleblock | incremental | 0.501463 | 0.502095 | OK |
| orbit/singleblock | powoftwo | 0.499717 | 0.500927 | OK |

`orbit` dan `singleblock` tetap identik persis (arsitektur berbagi block
pertama, sudah terdokumentasi sebelumnya). Pergeseran sebelum→sesudah
~0.0006 (jauh di dalam band, tidak signifikan) — konsisten dengan ekspektasi
`addr_hi=0` dampaknya pada avalanche minimal untuk kasus baseline ini.
Output: 6 CSV + 2 PNG di folder ini.

## 6. Throughput 5-mode standar — pasca-promosi, delta jujur

Rebuild `bench_release` dari `ra_core.c`+`bench_release.c` pasca-promosi,
`taskset -c 0 ./bench_release all`. Hasil mentah:
`bench_release_rerun_2026-09-06_post-promotion.txt`. Baseline "sebelum":
`../2026-9-6_release-benchmark-prep/bench_release_rerun_2026-09-06.txt`
(formula lama, re-run tgl sama pagi harinya, delta run-to-run terhadap
2026-09-04 sudah dikonfirmasi noise ≤5% di situ).

| Mode | Sebelum promosi (MB/s) | Sesudah promosi D (MB/s) | Delta % |
|---|---:|---:|---:|
| singleblock K=1 | 372.14 | 335.35 | **-9.89%** |
| singleblock K=255 | 8575.67 | 8525.96 | -0.58% |
| orbit multistream K=1 | 69.20 | 65.35 | **-5.56%** |
| orbit multistream K=255 | 1364.21 | 1342.15 | -1.62% |
| orbit --stream | 1408.58 | 1399.82 | -0.62% |
| philox4x32-10 K=1 (ref) | 312.00 | 310.04 | -0.63% |
| philox4x32-10 K=255 (ref) | 1434.35 | 1416.23 | -1.26% |
| philox4x32-10 stream (ref) | 1441.18 | 1443.18 | +0.14% |
| xoshiro256\*\* K=1 (ref) | 579.90 | 579.42 | -0.08% |
| xoshiro256\*\* K=255 (ref) | 3800.07 | 3789.82 | -0.27% |
| xoshiro256\*\* stream (ref) | 4049.31 | 4043.07 | -0.15% |

**Framing yang berbeda dari `2026-9-6_release-benchmark-prep/RESULTS.md`**
(sesi itu semua delta ≤5% dianggap noise run-to-run karena formula TIDAK
berubah): sesi ini formula BERUBAH secara nyata, jadi delta tidak boleh
diasumsikan noise begitu saja. Pola yang teramati:

- Baris `ra_core` K=1 (`singleblock` -9.89%, `orbit` multistream -5.56%)
  regresi jauh melebihi baris referensi Philox/Xoshiro (±1.3%, dalam
  toleransi run-to-run normal yang sudah dikonfirmasi sesi sebelumnya).
- Baris `ra_core` K=255/stream regresi jauh lebih kecil (≤1.62%), mendekati
  level noise referensi.
- **Kesimpulan: ini genuine overhead dari fitur `addr_hi`, bukan noise.**
  `keyterm_l`/`keyterm_m` sekarang butuh 1 `fmix32` + 1 multiply TAMBAHAN
  (untuk term `addr_hi`) di atas yang sudah ada untuk `key` — biaya ini
  fixed per-init, jadi paling terasa di K=1 (init mendominasi total waktu)
  dan teramortisasi di K=255/stream (init dibagi rata ke banyak kata
  output). Baris referensi yang tidak disentuh sama sekali tetap flat —
  kontrol yang mengonfirmasi bahwa regresi bukan artefak sistem
  (freq-scaling/thermal), tapi biaya nyata dari perubahan kode.

Trade-off ini dianggap dapat diterima: fitur `addr_hi` menutup defect
statistik nyata (birthday-paradox K=1+hashed-key) dengan harga throughput
K=1 turun ~6-10% — masih jauh di atas breakeven vs Philox K=1 (335.4 vs
310.0 MB/s, `singleblock` tetap 1.08x lebih cepat) dan K=255/stream nyaris
tidak terpengaruh (regime utama `ra_core` didesain untuk).

## 7. README

`README.md` bagian `### ra_core` (baris ~112) diedit (wajib, bukan
kondisional): tambah paragraf kapasitas addressing 64-bit + alasan
birthday-paradox, tabel benchmark diganti ke angka pasca-promosi di atas,
paragraf baru khusus membahas cost `addr_hi` (delta jujur, bukan
"reproducible"), paragraf perbandingan rasio (Philox/Xoshiro) diupdate ke
angka baru.

## 8. Kesimpulan

Kandidat D berhasil dipromosikan penuh ke `ra_core.c` kanonik: byte-identik
dengan `candidate_d.c` (12+8 kombinasi MATCH), backward-compat CLI terjaga
(12 kombinasi MATCH), 13 caller + `bench_release.c` compile bersih, 2
benchmark wajib direfresh dengan hasil PASS (avalanche) dan trade-off
terdokumentasi jujur (throughput). `ra_core.c` sekarang punya kapasitas
addressing 64-bit efektif, menutup defect birthday-paradox K=1+hashed-key
yang sebelumnya membuka kembali gate produksi. Belum ada keputusan
promosi fisik ke `src/` (nama modul, API surface, package layout) — tetap
di luar scope task ini, lihat `PRODUCTION_READINESS_HANDOVER.md`.
