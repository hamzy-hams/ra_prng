# HANDOVER: State-Size / Speed Pareto Frontier (K[low,high] merged, N∈{256,128,64,32,16})

Status: **BELUM DIMULAI** — ini rencana siap-eksekusi hasil sesi perencanaan
2026-09-11, dilanjutkan di sesi lain. Jangan diedit lagi setelah eksekusi
dimulai (ikuti konvensi repo: `HANDOVER.md` = rencana beku, `RESULTS.md` =
hasil final ditulis nanti).

## 1. Latar belakang & tujuan riset

`ra_core.c` produksi saat ini (`src/ra_core/c/ra_core.c`) punya state total
512 word (`M[256]` + `L[256]`, masing-masing `uint32_t`) — total
state-space jauh melampaui apa pun yang relevan untuk periodisitas praktis
(superkomputer mana pun tidak akan pernah mendekati exhaustive search di
ruang sebesar itu), sehingga sebagian dari ukuran state itu adalah
*overhead* murni, bukan kebutuhan keamanan statistik.

Di sisi lain, biaya `singleblock` (init murah/parsial, tanpa `L`, tanpa
reseed) dan `orbit` (init selalu penuh 2×256 word, plus swap
`L[i]<->L[d]` dan `ra_reseed` tiap 255 word) sangat timpang — makin besar
state, makin mahal initnya tapi makin baik amortisasinya untuk stream
panjang; makin kecil state, makin murah initnya tapi kontribusi biaya
per-cycle jadi makin dominan secara relatif.

**Pertanyaan riset (dari user):**
1. Menyusutkan state ke deret power-of-two lebih kecil — di titik mana
   (jika ada) kecepatan `singleblock` dan `orbit-stream` konvergen, atau
   malah bertukar peringkat, atau keduanya naik?
2. Apakah keduanya bisa mengalahkan Philox4x32-10 / Xoshiro256** / PCG32?
3. Apakah optimasi *rolling-o* yang ada sekarang tetap valid (tidak rusak)
   pada N kecil?
4. Apakah kualitas statistik tetap baik?
5. Di mana letak bottleneck terbesar yang bikin mahal?

**Hasil yang diharapkan**: grafik waktu generate-word & init untuk
singleblock dan orbit-stream per ukuran state K[] dan model reseed,
rangkuman jawaban tiap pertanyaan di atas, dan temuan menarik (bug, masalah
kualitas, atau hasil di luar dugaan).

## 2. Keputusan desain — SUDAH DIKONFIRMASI USER (AskUserQuestion, 2026-09-11)

Semua pertanyaan di bawah dijawab dengan opsi "Recommended":

1. **Semantik ukuran N**: kandidat power-of-two (256 baseline, 128, 64, 32,
   16) berlaku **per-half** — `K[low]` dan `K[high]` masing-masing menyusut
   ke nilai itu (total `K[]` = 2×N), analog langsung dari "M dan L
   masing-masing 256 word saat ini". BUKAN total gabungan K[] dibagi dua.
2. **Reseed "whole-K"** = **mutual XOR kedua half**: `K_low[i] ^=
   K_high[i]` DAN `K_high[i] ^= K_low_lama[i]` sekaligus (dua-duanya
   berubah). Reseed **"K[low] doang"** = persis `ra_reseed` produksi saat
   ini dipetakan ke nama baru (`K_low[i] ^= K_high[i]`, `K_high` tak
   disentuh langsung oleh reseed — hanya berubah lewat swap cycle).
3. **Kesetiaan formula**: tap-offset (window 8-tap, offset `+6`/`-1`,
   `rot32`, `fmix32` keyterm) **ditranslasi mekanis**: `256`→`N`, cast
   `uint8_t`→`mask=(N-1)`, TANPA retuning offset di ronde ini. Kegagalan
   kualitas di N kecil = TEMUAN penelitian, bukan alasan retuning sekarang.
4. **Batas K per-N**: sweep `K` mengikuti `[1, N-1]` per kandidat N (bukan
   tetap absolut 1..255) — konsisten dengan hard-cap `singleblock` saat
   ini (`rng<=255=N-1`).
5. **Gaya implementasi**: **satu source file terparametrisasi** via macro
   compile-time (`RA_N`, dan `RESEED_MODE` untuk varian orbit) — BUKAN 5
   file copy-paste terpisah.
6. **Cakupan benchmark kecepatan**: baik sweep-K (reinit tiap K word)
   MAUPUN mode `orbit --stream` kontinu (tanpa reinit paksa) per N —
   supaya pertanyaan konvergensi speed (poin riset #1) bisa dijawab
   langsung dari steady-state murni, bukan hanya titik-titik K diskrit.
7. **Gerbang period teoretis**: tambahkan pre-check heuristik
   λ≈0.7824·√|S| (dari `experiments/2026-8-25_periodicity-heuristic-validation/toy_prng.py`)
   SEBELUM kompilasi/PractRand — menyaring kandidat yang secara teori
   jelas bermasalah, murah karena cuma hitung-hitungan.
8. **Kedalaman uji kualitas**: **bertingkat/triase** — Tier0 murah di
   laptop untuk SEMUA varian → Tier1 PractRand screen singkat untuk SEMUA
   → Tier2 PractRand dalam + dieharder penuh HANYA untuk yang lolos, di
   VPS. (BUKAN full-depth untuk semua varian sejak awal — kombinatorial
   terlalu besar: ~15 varian core × sweep-K masing-masing.)

## 3. Latar teknis kunci (dari eksplorasi kode, sesi 2026-09-11)

Rujukan file & baris di `src/ra_core/c/ra_core.c` (state saat sesi ini —
verifikasi ulang nomor baris sebelum implementasi, karena file ini terus
berubah):

- `M`/`L`: local stack array `alignas(64) uint32_t[256]` masing-masing —
  `ra_core.c:282` (orbit, ada `L`+`M`), `ra_core.c:400` (singleblock, cuma
  `M`, tanpa `L` sama sekali).
- Init (`ra_init_state_orbit` `ra_core.c:215-232`, `ra_init_state_singleblock`
  `ra_core.c:325-347`): dua hash independen (`fmix32` dari `key` dan dari
  `addr_hi`) DIKALIKAN jadi satu `keyterm`, lalu tiap slot `i` diisi
  `rot32((i*const)*keyterm, r=key^i)`. **`addr_hi` sengaja TIDAK dipakai
  untuk indexing runtime** — ia hanya mengubah isi seluruh array lewat
  init (dikeluarkan dari rotasi karena mencampurnya ke rotasi pernah gagal
  PractRand Gap-16, komentar `ra_core.c:212-214`) — retensi perilaku ini
  WAJIB dipertahankan di versi parametrized.
- `ra_reseed` (`ra_core.c:182-191`): `M[i] ^= L[i]` untuk SEMUA 256 slot,
  lalu di-fold `ra_hash` (8 grup × 32 word XOR-tree, `ra_core.c:174-180`)
  jadi `cons` baru. Hanya dipanggil di `ra_core_orbit`, tiap 255 word.
  `singleblock` tidak pernah memanggilnya (dead-code proof: `rng<=255`
  selalu selesai 1 iterasi, `ra_core.c:298-302`).
- Index wraparound di `ra_permutation_cycle_orbit`/`_singleblock`
  (`ra_core.c:247-273, 360-384`) memakai **cast `(uint8_t)`** implisit
  mod-256 (window 8-tap `M[(uint8_t)(255+k)]`, offset `i-1`/`i+6`) — ini
  titik UTAMA yang harus diganti `& (N-1)` eksplisit supaya generalisasi
  ke N<256 valid. `d = c & 0xFFu` (source index swap orbit / batas
  partial-init singleblock) juga harus jadi `c & (N-1)`.
- `ra_hash` (`ra_core.c:174-180`) mengasumsikan struktur 8 grup × 32 word
  (=256) — generalisasi: 8 grup × `(N/8)` word (valid untuk semua
  N∈{256,128,64,32,16} karena kelipatan 8).
- Signature 4-argumen (`key, addr_hi, rng, raw_stream`) — `ra_core.c:279,390`
  — WAJIB dipertahankan agar kompatibel dengan kontrak `tools/battery/
  driver_main_template.c`.
- `ra_core_engine.c`/`.h`: layer stream stateful (M/L persist lintas
  panggilan `.next()`, dipakai binding Python) — DI LUAR SCOPE riset ini
  kecuali nanti hasil promosi, cukup dicatat sebagai referensi arsitektur.

Harness yang bisa **langsung dipakai ulang tanpa modifikasi** (kontrak
`--single`/`--stream`/`--stream-hashed` dari
`tools/battery/driver_main_template.c`):
- `tools/battery/run_battery.py` + `practrand_gate.py` — PractRand,
  generik/N-agnostic karena cuma pipe stdout binary.
- `tools/battery/avalanche.py` — avalanche generik via CLI binary kontrak
  `--single <seed> <n>`.

Pola yang perlu **digeneralisasi** (masih hardcode 255/32 untuk N=256):
- `experiments/2026-9-5_orbit-address-widen-64bit/avalanche_3way_addr_hi.py`
  — 3-arah key/addr_hi/combined bit-flip, reuse
  `quality_gate.avalanche_stats` dari
  `experiments/2026-8-26_operation-pruning-research/quality_gate.py`.
- `experiments/2026-9-2_singleblock-cycle-combo-search/avalanche_heatmap_winners_k255.py`
  — matrix 32×255 Hamming-distance per cycle-position → generalisasi jadi
  32×(N-1). Fungsi inti `compute_matrix()` baris 61-72 di file itu.
- `experiments/2026-9-4_orbit-kmin-battery/run_dieharder_battery.py` — 27
  GOOD_TESTS, `BASE_KEY=111222`, pola piped-no-rewind (pertahankan
  BASE_KEY yang sama demi komparabilitas historis).
- `experiments/2026-9-2_singleblock-k-threshold-characterization/` —
  metodologi BCFN sweep untuk cari safe-K floor (K≈80-96 di N=256,
  RESULTS.md), digeneralisasi per-N — pertanyaan terbuka: floor ini scale
  proporsional dengan N atau N-independent?
- `experiments/2026-8-25_periodicity-heuristic-validation/toy_prng.py` —
  `Params` dataclass sudah generik untuk `n` sembarang — basis gerbang
  period teoretis (keputusan #7 di atas).

Baseline speed yang sudah ada:
- `experiments/2026-9-1_family-productionization/bench_release.c` — sudah
  punya Philox4x32-10 + Xoshiro256** (harness `clock_gettime
  (CLOCK_MONOTONIC)`, min-of-N-trials, format `init_ns`/
  `gen_ns_per_word`/`real_ns_per_word`/`real_MBps`). Standar pelaporan
  WAJIB: laporkan K=1 DAN K=255(=N-1) (dan orbit --stream) bersama, jangan
  cuma satu titik (README.md:171-174 — pernah menyesatkan sebelumnya).
- **PCG32 belum di-port** ke harness `bench_release.c` — baru ada di
  `benchmarks/comparisons/` versi lama dengan metodologi berbeda
  (`clock()`+`perf stat`, bukan `CLOCK_MONOTONIC`). Perlu ditambahkan baru
  ke harness yang sama demi keterbandingan apples-to-apples.
- Semua benchmark yang ada dikompilasi manual dengan
  `-O3 -march=native -std=gnu17 -include stdalign.h -Wall -Wextra` (bukan
  lewat root `Makefile` yang cuma placeholder untuk target `bench:`).

## 4. Rencana implementasi

### 4.1 Core terparametrisasi — `ra_core_param.c`
Satu file, digeneralisasi dari `src/ra_core/c/ra_core.c`:
- Macro `RA_N` (256/128/64/32/16) mengganti semua literal `256`; turunan
  `RA_MASK = (RA_N-1)` mengganti semua cast `(uint8_t)`; `RA_HASH_GROUPS=8`,
  `RA_HASH_GROUPLEN=(RA_N/8)` menggantikan struktur `ra_hash`.
- Macro `RA_RESEED_MODE` (`LOWONLY` | `MUTUAL`) memilih badan `ra_reseed`
  antara perilaku produksi (`K_low[i]^=K_high[i]`) vs mutual-XOR baru
  (`K_low[i]^=K_high[i]` DAN `K_high[i]^=K_low_lama[i]`).
- Array tunggal `K[2*RA_N]`, dengan `K_low = K`, `K_high = K + RA_N` —
  `singleblock` hanya memakai `K_low` (persis perilaku M-only sekarang,
  `K_high` tidak dialokasikan/dipakai untuk mode ini).
- Pertahankan signature 4-argumen (`key, addr_hi, rng, raw_stream`) dan
  kontrak CLI `driver_main_template.c` (`--single`, `--stream`,
  `--stream-hashed`, `--multistream`) supaya seluruh tooling
  `tools/battery/*` jalan tanpa modifikasi.
- Regenerasi tabel KAT per N (tabel KAT `ra_core.c` khusus N=256 tidak
  valid untuk N lain) — buat `checksum-gen` per binary N seperti pola
  `ra_core.c:482-494`.
- **Sanity check WAJIB sebelum lanjut ke tahap manapun**: varian
  `N=256, RESEED_MODE=LOWONLY, addr_hi=0` harus menghasilkan output
  byte-identik dengan `ra_core_orbit`/`ra_core_singleblock` produksi saat
  ini — bukti generalisasi tidak diam-diam mengubah perilaku di titik
  baseline.
- Build: `-O3 -march=native -std=gnu17 -include stdalign.h`, `BUILD.sh`
  meng-compile 5(N) × up to 2(reseed mode, khusus orbit) binary.

### 4.2 Gerbang period teoretis (pre-check, sebelum PractRand)
`period_precheck.py` — adaptasi model `Params`/heuristik
λ≈0.7824·√|S| dari `toy_prng.py` ke topologi state riil (`2×N` word 32-bit
untuk orbit, `N` word untuk singleblock). Jalankan untuk kelima N SEBELUM
tahap statistik apa pun; flag kandidat yang secara teori period-nya
mencurigakan kecil relatif ke panjang test yang direncanakan.

### 4.3 Validasi kualitas bertingkat
- **Tier 0** (laptop, semua varian): KAT/checksum smoke-test; avalanche
  3-arah generalisasi dari `avalanche_3way_addr_hi.py` (ganti target
  binary jadi `ra_core_param`, `base_seed`/`cycle_words` ikut N); heatmap
  32×(N-1) generalisasi dari `avalanche_heatmap_winners_k255.py` (ganti
  `n=255` hardcode jadi `N-1`).
- **Tier 1** (laptop/VPS ringan, varian yang lolos Tier0): PractRand
  screen singkat (budget kecil, mis. 1-2GB) via
  `tools/battery/run_battery.py` apa adanya (sudah N-agnostic).
- **Tier 2** (VPS, HANYA survivor Tier0-1): PractRand dalam (8-16GB,
  standar repo) + dieharder 27-test penuh (adaptasi
  `run_dieharder_battery.py`, `BASE_KEY=111222` dipertahankan). **Sebelum
  mulai**: cek `VPS_ACCESS.md` (gitignored, root repo) untuk lihat proses
  lain yang masih jalan, sesuai instruksi CLAUDE.md proyek. Jangan
  proaktif polling status begitu run diketahui makan waktu berjam-jam —
  cukup lapor kalau user tanya.

### 4.4 Safe-K floor per-N
Generalisasi metodologi BCFN sweep dari
`experiments/2026-9-2_singleblock-k-threshold-characterization/` (pola
`multikey_stream_k.c`, parametrized K) ke tiap N — jawab pertanyaan riset
terbuka: apakah floor aman (80-96 di N=256) scale proporsional dengan N
atau N-independent.

### 4.5 Benchmark kecepatan
Perluas pola `bench_release.c` (harness sama:
`clock_gettime(CLOCK_MONOTONIC)`, min-of-N-trials, format
`init_ns`/`gen_ns_per_word`/`real_ns_per_word`/`real_MBps`) untuk:
- `singleblock` K-sweep (K∈[1,N-1]) per N.
- `orbit` K-sweep (reinit tiap K word) per N × per reseed-mode.
- `orbit --stream` kontinu (tanpa reinit paksa) per N — mengukur
  steady-state murni tanpa overhead init berulang.
- **Tambahkan baseline PCG32** ke harness yang sama (belum ada),
  bersanding dengan Philox4x32-10 dan Xoshiro256** yang sudah ada.
- Ikuti standar pelaporan repo: laporkan K=1 DAN K=N-1 (dan orbit --stream)
  bersama, jangan hanya satu titik.

### 4.6 Output / deliverable → `RESULTS.md`
- Grafik ns/word vs N untuk singleblock-K-sweep / orbit-K-sweep-di-safeK /
  orbit-stream-kontinu, dengan garis referensi horizontal
  Philox/Xoshiro256**/PCG32.
- Grafik init-time vs N.
- Heatmap 32×(N-1) per kandidat/reseed-mode.
- Tabel avalanche 3-arah per kandidat.
- Tabel safe-K floor per N.
- Hasil gerbang period teoretis.
- Rangkuman jawaban eksplisit untuk tiap pertanyaan riset (lihat §1):
  konvergensi speed? melampaui Philox/Xoshiro/PCG? rolling-o rusak atau
  tetap valid di N kecil? kualitas tetap baik? bottleneck terbesar di
  mana?
- Bagian temuan tak terduga/bug/masalah kualitas yang muncul selama
  eksperimen.

## 5. Verifikasi end-to-end (urutan eksekusi)

1. Compile seluruh varian (`BUILD.sh`), jalankan `checksum-gen` + validasi
   KAT internal per N.
2. Jalankan sanity check byte-identik N=256/LOWONLY vs `ra_core.c`
   produksi (addr_hi=0) — WAJIB pass sebelum lanjut.
3. Jalankan gerbang period teoretis, catat flag per N.
4. Jalankan Tier0 (KAT+avalanche+heatmap) untuk semua varian, filter
   survivor.
5. Jalankan Tier1 PractRand screen untuk survivor Tier0.
6. Cek `VPS_ACCESS.md`, jalankan Tier2 (PractRand dalam + dieharder) di
   VPS untuk survivor Tier1.
7. Jalankan benchmark kecepatan (K-sweep + `--stream` + baseline
   Philox/Xoshiro/PCG32) untuk SEMUA N (independen dari hasil kualitas,
   supaya kurva Pareto tetap lengkap walau ada varian yang gagal kualitas
   — kegagalan kualitas itu sendiri adalah data penting untuk laporan).
8. Susun `RESULTS.md` + seluruh plot.
9. Jalankan `/graphify --update` sesuai instruksi workflow proyek
   (CLAUDE.md repo) sebelum menutup tugas, supaya file baru masuk ke
   knowledge graph.

## 6. Catatan untuk sesi lanjutan

- Sesi perencanaan ini TIDAK membuat kode/binary apa pun — murni riset
  baca kode + desain (plan mode). Verifikasi ulang nomor baris `ra_core.c`
  yang dirujuk di §3 sebelum mulai edit, karena file itu terus berubah
  antar sesi.
- Kombinatorial: 5 ukuran N × 2 mode (orbit/singleblock) × sampai 2
  model-reseed (khusus orbit) ≈ 15 varian core, masing-masing perlu
  disapu K sendiri — perkirakan waktu/compute sesuai skema triase di §4.3,
  jangan langsung full-depth PractRand/dieharder di awal.
- Ingat memory lama: jangan retry-reseed dengan seed lain untuk
  "menjelaskan" FAIL PractRand (satu FAIL = algoritma gagal, full stop);
  jangan re-scope pengukuran yang sudah selesai jadi rerun tanpa
  persetujuan eksplisit; selalu laporkan 5 angka standar (orbit K=1/K=N-1/
  --stream, singleblock K=1/K=N-1) bukan cuma satu titik.
