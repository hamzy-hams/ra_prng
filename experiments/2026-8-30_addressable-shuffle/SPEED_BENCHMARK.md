# Speed Benchmark: `scrambler_addressable` vs Algoritma Lain

## Pertanyaan

Apakah `scrambler_addressable` (addressable-init, `ra_init_state_addressable(key)`)
lebih lambat dari `ra_prng2` lama (ZepFold, formula init tetap
`multiplier_m`/`multiplier_l`) karena biaya inisialisasi tambahan? Dan
bagaimana posisinya dibanding shuffle CLI lain yang didorong PRNG berbeda
(`xoshiro256**`, `PCG32`, `Philox4x32`, `ChaCha20`)?

**Jawaban singkat:** tidak ada bukti `ra_init_state_addressable()` menambah
biaya kecepatan — selisih `scrambler_addressable` vs `ra_prng2` bolak-balik
arah antar-run dan selalu dalam rentang noise (lihat poin 1). Yang **memang**
signifikan: seluruh keluarga shuffle "ZepFold" (dengan/tanpa addressable-init)
~2.2x lebih lambat dari xoshiro256\*\*/PCG32/Philox4x32 — tapi **bukan karena
algoritma PRNG atau swap array-nya** (sudah diuji langsung, lihat bagian
"Analisis Lanjutan"), melainkan karena implementasi I/O baca/tulis file teks
(`fscanf`/`fprintf` per token) yang jauh lebih lambat dari pola `fread`+
tokenizer-manual yang dipakai binary pembanding. Detail dan caveat di bawah.

## Metodologi

- Semua binary di-compile fresh di sesi yang sama (`gcc -std=c11 -O3`),
  di folder `speed_bench/` (build artifact, terpisah dari
  `benchmarks/comparisons/source/` supaya folder referensi itu tetap bersih).
- Input sama untuk semua: `tokens.txt` (1.000.000 token), disalin dari
  `benchmarks/comparisons/source/tokens.txt` — file yang sama yang dipakai
  `benchmarks/comparisons/scrambling_speed/benchmark_perf.txt` (metodologi
  lama), supaya angka bisa dibandingkan langsung.
- Diukur dengan `perf stat -r 30 -- <binary> <flags>` (30 repeat, median/mean
  otomatis dari `perf`) — lihat `run_speed_benchmark.sh` untuk command persis
  tiap binary (dialek CLI beda-beda: `-i/-o`, input positional, atau
  `--input/--output`).
- **Semua 10 binary (termasuk `ra_prng2` dan pembanding xoshiro/pcg/philox/
  chacha20) diukur ulang di sesi/mesin yang sama** — bukan cuma menyalin
  angka lama dari `benchmark_perf.txt`. Ini membuat perbandingan
  `scrambler_addressable` vs `ra_prng2` maupun vs algoritma lain
  terkontrol penuh (mesin, beban sistem, waktu yang sama).
- Catatan kompilasi: `benchmarks/comparisons/source/ra_prng2.c` sudah lama
  punya bug komentar header `/* ... */` yang tidak pernah ditutup, sehingga
  seluruh blok `#include` di bawahnya ikut "termakan" komentar dan gagal
  compile polos. Di-compile di sini dengan `-include stdio.h -include
  stdlib.h -include stdint.h -include string.h -include time.h -include
  stdalign.h` (memaksa header standar masuk lewat command line) **tanpa
  mengubah file sumbernya** — di luar scope pekerjaan ini untuk memperbaiki
  file di `benchmarks/`.

## Hasil

Dua run lengkap dilakukan (run 1: 10 binary; run 2, setelah `stream_driven_shuffle`
ditambahkan: 11 binary). Angka run 2 (terbaru, dipakai sebagai acuan tabel
di bawah) — `perf stat -r 30`:

| Algoritma | Run 1 (s) | Run 2 (s) | Variance (run 2) | vs tercepat (run 2) |
|---|---:|---:|---:|---:|
| pcg | 0.048599 | 0.049688 | ±1.13% | — (tercepat) |
| pcg amortized | 0.050165 | 0.050004 | ±1.10% | +0.6% |
| xoshiro256 | 0.051062 | 0.050420 | ±1.28% | +1.5% |
| xoshiro256 amortized | 0.052174 | 0.052140 | ±1.05% | +4.9% |
| philox amortized | 0.052323 | 0.052720 | ±1.10% | +6.1% |
| philox | 0.055449 | 0.053797 | ±0.76% | +8.3% |
| chacha20 amortized | 0.059098 | 0.059408 | ±0.81% | +19.6% |
| `ra_prng2` (ZepFold lama) | 0.125527 | 0.115629 | ±0.44% | +132.7% |
| **`stream_driven_shuffle`** (baru — lihat "Analisis Lanjutan") | — | **0.120388** | **±1.15%** | **+142.3%** |
| `scrambler_addressable` | 0.115757 | 0.125538 | ±4.29% | +152.7% |
| chacha20 | 0.198200 | 0.199404 | ±0.91% | +301.3% |

Perhatikan: `ra_prng2` dan `scrambler_addressable` **bertukar urutan** antar
run 1 dan run 2 (run 1: `scrambler_addressable` lebih cepat 7.8%; run 2:
`ra_prng2` lebih cepat 8.6%) — variance run-to-run salah satunya cukup besar
di tiap run (±4.29-4.50%) untuk menjelaskan pertukaran ini. Ini konsisten
dengan kesimpulan poin 1 di bawah: selisih keduanya ada di dalam noise, bukan
efek `ra_init_state_addressable()` yang terukur.

## Interpretasi

1. **`scrambler_addressable` vs `ra_prng2`**: **arah selisihnya berbalik**
   antar dua run (run 1: `scrambler_addressable` −7.8%; run 2: +8.6%),
   dengan salah satu dari keduanya selalu punya variance run-to-run tinggi
   (±4.3-4.5%) di tiap run. Kesimpulan yang aman: **tidak ada bukti
   `ra_init_state_addressable()` (loop O(256), sekali per pemanggilan
   `ra_core`) menambah atau mengurangi biaya yang terukur** dibanding formula
   init tetap yang lama — selisihnya sepenuhnya konsisten dengan noise
   pengukuran, bukan efek algoritmik sistematis. Ini juga sesuai ekspektasi
   awal (untuk 1.000.000 token / ~3922 blok 255-elemen, biaya init satu kali
   memang seharusnya diabaikan dibanding total kerja shuffle).

2. **`scrambler_addressable`/`ra_prng2` vs xoshiro/pcg/philox/chacha20-amortized**:
   kedua shuffle CLI berbasis "ZepFold" (dengan/tanpa addressable-init) ini
   ~2.2-2.4x lebih lambat dari kelompok itu. Ini **bukan regresi baru** —
   pola yang sama persis sudah ada di `benchmark_perf.txt` lama
   (`ra_prng2`/`ra_prng3` ~0.12s vs xoshiro/pcg/philox ~0.05-0.06s, rasio
   serupa). Dugaan awal (di sesi ini) adalah penyebabnya struktural — shuffle
   loop ZepFold melakukan **dua** swap array per token (`scrambled_tokens`
   dan `L`, tabel state internal generator) sementara pembanding lain cuma
   satu swap. **Analisis lanjutan di bawah menunjukkan dugaan ini salah** —
   biaya swap ganda ternyata bukan penyebab dominan.

3. **`scrambler_addressable` vs `chacha20` (varian "per call", bukan
   amortized)**: `scrambler_addressable` justru **jauh lebih cepat**
   (0.116s vs 0.198s) — `chacha20` per-call mahal karena overhead
   pemanggilan block cipher penuh per token tanpa buffering.

## Analisis Lanjutan: bottleneck sebenarnya (bukan swap ganda)

Untuk menguji dugaan "swap ganda" di poin 2, dibuat `stream_driven_shuffle.c` —
CLI shuffle baru yang **tidak** memakai loop ZepFold sama sekali. Ia memakai
`ra_core(key, rng, raw_stream)` dari `winner_wired_addressable.c` (byte-for-byte
copy, generator stream mentah) semata-mata sebagai sumber angka acak, lalu
men-drive Fisher-Yates **satu-swap** persis seperti `xoshiro256.c` (baca
`stream_driven_shuffle()` di file itu). Kalau dugaan "swap ganda" benar, CLI
ini seharusnya mendekati kecepatan xoshiro/pcg (~0.05s).

**Hasil: tidak.** `stream_driven_shuffle` tetap ~0.110-0.120s — sama-sama
di kelas `scrambler_addressable`/`ra_prng2`, jauh dari xoshiro/pcg. Statistik
outputnya sehat (entropy 16.6096, chi2 0, runs-test z=0.155, SCC=0.0006 —
setara varian lain).

Supaya jelas biaya ada di mana, dipecah jadi tiga komponen (`perf stat -r 10`,
1.000.000 token/angka, semua di sesi yang sama):

| Komponen | Cara ukur | Elapsed (s) |
|---|---|---:|
| **I/O murni** (load `fscanf` + save `fprintf`, shuffle **dimatikan**) | `io_only_isolation_test` (derivasi `stream_driven_shuffle.c`, cross-check dgn varian dari `scrambler_addressable.c` beda >1%: 0.0937s vs 0.0924s) | **0.0937** |
| **Generate stream murni** (999.999 angka, tanpa file I/O, `fwrite` biner ke stdout) | `winner_wired_addressable --stream 1 999999 > /dev/null` | **0.0113** |
| **Total `stream_driven_shuffle`** (I/O + generate + 1 swap/token) | `perf stat -r 10 ./stream_driven_shuffle ...` | 0.1098 |

**I/O teks (`fscanf`/`fprintf` per token) memakan ~85% dari total waktu** —
bukan generator, bukan swap. `load_tokens_from_file`/`save_tokens_to_file`
(dipakai semua varian ZepFold: `scrambler_addressable.c`, `ra_prng2.c`,
`stream_driven_shuffle.c`) memanggil `fscanf(fp, "%ld", &value)` dan
`fprintf(fp, "%u", ...)` **sekali per token** (1 juta pemanggilan fungsi
format-parsing masing-masing). `xoshiro256.c`/`pcg.c`/`philox.c`/`chacha20.c`
sebaliknya baca seluruh file sekali lewat `fread()`, tokenize manual pakai
pointer scan (tanpa `scanf`), dan tulis balik dengan `fwrite()` byte mentah
dari string yang sudah ada (tanpa `fprintf` per token) — lihat
`benchmarks/comparisons/source/xoshiro256.c` baris 53-103.

**Revisi kesimpulan poin 2 di atas:** perbedaan swap-ganda vs swap-tunggal
sebenarnya kecil sekali (generate 0.0113s + 1 swap ≈ 0.016s vs generate+2-swap
ZepFold ≈ 0.014-0.032s — dalam rentang noise satu sama lain, lihat variance
run-to-run `ra_prng2`/`scrambler_addressable` yang bertukar urutan antar-run).
**Penyebab dominan gap ~2.2x terhadap xoshiro/pcg/philox adalah pilihan
implementasi I/O yang sepenuhnya lepas dari algoritma PRNG-nya** — bukan sifat
inheren "array-based PRNG" seperti hipotesis awal. Kalau `load_tokens_from_file`/
`save_tokens_to_file` diganti pola `fread`+tokenizer-manual seperti xoshiro,
seluruh keluarga ZepFold (termasuk `scrambler_addressable`) kemungkinan besar
akan mendekati kecepatan xoshiro/pcg — ini **belum diverifikasi**, di luar
scope sesi ini (mengubah I/O helper akan mengubah 3 file sekaligus:
`scrambler_addressable.c`, `stream_driven_shuffle.c`, dan/atau `ra_prng2.c`
di `benchmarks/comparisons/source/` yang di luar folder eksperimen ini).

## Follow-up: optimasi I/O terverifikasi (2026-08-30, sesi lanjutan)

Follow-up yang dicatat "belum diverifikasi, di luar scope" di bagian
"Analisis Lanjutan" di atas sudah dikerjakan: `load_tokens_from_file`/
`save_tokens_to_file` di `scrambler_addressable.c`, `stream_driven_shuffle.c`,
dan `speed_bench/io_only_isolation_test.c` (bukan `ra_prng2.c` di
`benchmarks/` — itu tetap read-only) diganti dari pola `fscanf`/`fprintf`
per-token ke pola `fread()` sekali + tokenizer pointer manual (`strtol`,
buffer di-load penuh via `fseek`/`ftell`/`rewind`/`fread`) + satu `fwrite()`
untuk output (buffer dibangun via `snprintf` per token lalu ditulis sekali),
meniru pola `xoshiro256.c` baris 52-106. Representasi token tetap `uint32_t`
(bukan raw string passthrough) supaya format output (desimal `%u`,
tanpa leading zero) identik dengan sebelumnya — **rename `ra_core` →
`ra_shuffle`** (di `scrambler_addressable.c`/`.py`, HANDOVER.md §6 poin 2)
dikerjakan bersamaan di sesi yang sama.

**Validasi sebelum benchmark ulang**: `verify_parity.py` tetap 63/63 PASS
(bit-identik C vs Python) setelah kedua perubahan; `scc_test.py` tetap PASS
(entropy 16.6096, chi2 0.0, 0/200 collision multi-key) — perubahan I/O dan
rename tidak mengubah hasil numerik. Sanity tambahan: output
`scrambler_addressable`/`stream_driven_shuffle` versi baru di-`diff` terhadap
`shuffled.txt`/`stream_shuffled.txt` lama pada `tokens.txt` yang sama — **0
beda byte** di keduanya.

**Hasil benchmark ulang** (`perf stat -r 30`, `tokens.txt` 1.000.000 token,
sesi/mesin yang sama dengan run 2 di atas):

| Algoritma | Lama (run 2, s) | Baru, I/O-optimized (s) | Perubahan |
|---|---:|---:|---:|
| pcg | 0.049688 | 0.047736 | kontrol, tak diubah |
| xoshiro256 | 0.050420 | 0.049074 | kontrol, tak diubah |
| philox | 0.053797 | 0.054031 | kontrol, tak diubah |
| chacha20 amortized | 0.059408 | 0.058334 | kontrol, tak diubah |
| `ra_prng2` (read-only, `benchmarks/`) | 0.115629 | 0.118078 | kontrol negatif -- I/O-nya sengaja TIDAK diubah, dipakai memastikan lingkungan/mesin sesi ini konsisten dengan run 2 |
| **`scrambler_addressable`** | 0.125538 (±4.29%) | **0.077151 (±1.27%)** | **-38.5%** |
| **`stream_driven_shuffle`** | 0.120388 (±1.15%) | **0.089255 (±8.27%)** | **-25.9%** (variance tinggi, lihat catatan di bawah) |
| chacha20 | 0.199404 | 0.198903 | kontrol, tak diubah |

Rasio terhadap tercepat (`pcg`, 0.047736s): `scrambler_addressable` sekarang
**1.62x** (dari 2.53x sebelumnya), `stream_driven_shuffle` **1.87x** (dari
2.42x). `ra_prng2` (kontrol, I/O tidak disentuh) tetap ~2.47x, sesuai
ekspektasi -- membuktikan gap besar yang berkurang benar-benar berasal dari
perubahan I/O, bukan drift mesin/sesi.

Dekomposisi ulang (`perf stat -r 10`, pola sama seperti tabel dekomposisi
di atas):

| Komponen | Lama (s) | Baru (s) | Perubahan |
|---|---:|---:|---:|
| I/O murni (`io_only_isolation_test`, shuffle dimatikan) | 0.0937 | **0.0584** | **-37.7%** |
| Generate stream murni (`winner_wired_addressable --stream`, tak disentuh) | 0.0113 | 0.0110 | ~0 (dalam noise, kontrol) |
| Total `stream_driven_shuffle` | 0.1098 (run dekomposisi) / 0.120388 (run 2 tabel utama) | 0.089255 | -18.7% s.d. -25.9% |

**Kesimpulan**: hipotesis "I/O adalah bottleneck dominan" **terbukti benar
arahnya** -- mengganti pola I/O memangkas gap terhadap xoshiro/pcg kira-kira
separuh (dari ~2.2-2.5x ke ~1.6-1.9x), dan I/O-murni sendiri turun ~37.7%.
Tapi gap **tidak tertutup penuh** -- `scrambler_addressable` masih ~1.6x
lebih lambat dari `pcg` setelah I/O dibenahi, jadi ada biaya lain (kemungkinan
dari struktur shuffle loop ZepFold sendiri: reseed `ra_hash` tiap 255 elemen,
8-wide read `M[]` per token, dua swap array) yang belum tereliminasi --
di luar scope follow-up ini, catatan untuk sesi mendatang kalau ingin
mengejar lebih dekat ke xoshiro/pcg.

**Catatan variance `stream_driven_shuffle`**: ±8.27% pada run ini jauh lebih
tinggi dari `scrambler_addressable` (±1.27%) dan run 2 lama untuk binary yang
sama (±1.15%) -- konsisten dengan "pelajaran metodologi" yang sudah dicatat
di riset addressable-init Tahap 5 (mikrobenchmark reinit-sweep single-run
kadang menyesatkan). Angka -25.9% di sini harus dibaca order-of-magnitude,
bukan presisi tinggi; kalau presisi lebih tinggi dibutuhkan, ulangi
`perf stat -r 30` beberapa kali dan ambil median.

## Follow-up: dekomposisi gap 1.62x (checkpoint, 2026-08-31)

Dua dari tiga dugaan penyebab gap residual (§ atas: reseed `ra_hash` tiap
255 elemen, 8-wide read `M[]` per token, dua swap array) diuji lewat harness
diagnostik baru di `speed_bench/`: `no_reseed_isolation_test.c` (reseed
blok-akhir dihilangkan) dan `single_mread_isolation_test.c` (`o` dibaca dari
1 elemen `M[]`, bukan 8). **Keduanya diagnostic-only** -- output PRNG tidak
divalidasi statistik (sama seperti `stream_driven_shuffle.c`'s precedent),
dipakai murni untuk isolasi wall-clock/instruction-count. Dugaan ketiga
(dua swap array) sudah dibantah sebelumnya (`stream_driven_shuffle`,
1-swap, justru LEBIH LAMBAT dari `scrambler_addressable` pasca-fix I/O:
1.87x vs 1.62x) -- tidak diuji ulang di sini.

**Metodologi**: `perf stat -r 20`, `tokens.txt` 1.000.000 token, key=1,
setiap varian dipasangkan (paired) dengan baseline `scrambler_addressable`
dalam batch pengukuran yang sama, dijalankan di mesin yang benar-benar
idle (bukan bersamaan job CPU-heavy lain -- pelajaran dari sesi ini:
`scrambler_addressable` yang diukur bersamaan job PractRand 1TB background
sebelumnya melaporkan 0.077s, ~15% lebih lambat dari kondisi idle ~0.066s,
meski rasio ke `pcg` tetap konsisten ~1.6x di kedua kondisi -- lihat
verifikasi di bawah).

| Varian | Waktu (s) | Delta vs baseline | Instructions | Delta instr. |
|---|---:|---:|---:|---:|
| `scrambler_addressable` (baseline, run A) | 0.066238 ± 0.79% | -- | 827,561,118 | -- |
| `no_reseed_isolation_test` (run A) | 0.065220 ± 0.29% | -1.5% | 820,919,428 | -0.8% |
| `scrambler_addressable` (baseline, run B, paired dg no_reseed run 2) | 0.065972 ± 0.61% | -- | -- | -- |
| `no_reseed_isolation_test` (run B) | 0.065301 ± 0.22% | -1.0% | -- | -- |
| `scrambler_addressable` (baseline, run C, paired dg single_mread) | 0.066136 ± 0.30% | -- | -- | -- |
| `single_mread_isolation_test` (run C) | 0.063812 ± 0.23% | **-3.5%** | 795,850,470 | -3.9% |
| `pcg` (re-measured, mesin idle sama) | 0.040898 ± 0.68% | -- | 299,046,296 | -- |

(`single_mread_isolation_test`'s run pertama melaporkan 0.071043s ±7.73% --
variance terlalu tinggi untuk dipercaya, pola "median-of-3"/pengulangan yang
sama seperti pelajaran Tahap 5 addressable-init; run kedua yang dipasangkan
dengan baseline barulah stabil dan dipakai di tabel di atas.)

**Rasio ke `pcg` tetap ~1.62x** pada pengukuran ulang mesin idle
(0.066136/0.040898 = 1.617x) -- konsisten dengan angka lama (0.077151/
0.047736 = 1.62x, diukur di sesi lain, kondisi mesin berbeda). Rasio ini
robust terhadap kondisi mesin (naik-turun bareng), bukan artefak
kontensi CPU sesi sebelumnya.

**Kesimpulan: kedua dugaan TIDAK menjelaskan sebagian besar gap.** Total
gap absolut `scrambler_addressable` vs `pcg` ≈ 0.0252s. Reseed
menyumbang ≈0.0009s (**~3-4% dari gap**), lebar-baca `M[]` menyumbang
≈0.0023s (**~9% dari gap**) -- gabungan keduanya cuma ~13% dari total gap.
**Sisa ~87% gap tidak teridentifikasi oleh kedua isolasi ini**, dan karena
dugaan ketiga (double-swap) sudah dibantah sebelumnya, ketiga dugaan asli
di §"Kesimpulan" sudah habis tanpa satu pun menjelaskan mayoritas gap.

**Interpretasi**: kemungkinan besar sisa gap adalah biaya struktural
recurrence `a/b/o/c/d` itu sendiri per token (5x `rot32`, beberapa
XOR/shift/add, 2x reduksi Lemire 64-bit) -- inheren lebih mahal secara
aritmetika daripada langkah PCG32 (satu LCG 64-bit + xorshift), bukan
sesuatu yang bisa diisolasi lebih jauh tanpa mendesain ulang recurrence-nya
sendiri (di luar scope diagnostic checkpoint ini -- perubahan recurrence
berarti PRNG baru, butuh validasi statistik penuh, bukan sekadar tweak
implementasi). **Ini temuan yang sah untuk paper**: gap ~1.6x terhadap
xoshiro/pcg adalah batas praktis desain shuffle-loop ZepFold saat ini
(setelah I/O dan tiga dugaan komponen utama dieliminasi satu-per-satu),
bukan inefisiensi implementasi yang tersisa untuk dipangkas murah.

## Follow-up: cycle `winner_wired_v2` + addressable init (checkpoint, 2026-08-31)

Pertanyaan: interpretasi di atas menyimpulkan sisa gap ~1.6x adalah "biaya
struktural recurrence `a/b/o/c/d` itu sendiri... bukan sesuatu yang bisa
diisolasi lebih jauh tanpa mendesain ulang recurrence-nya sendiri". Tapi
angka itu diukur dari `scrambler_addressable`, yang sengaja memakai
recurrence ORIGINAL (tidak pernah diubah dari `scrambler.c` lama) — bukan
recurrence `winner_wired_v2` (pruned+wired, ~6% instruksi lebih sedikit,
`../2026-8-27_operand-position-search/RESULTS.md`) yang jadi kandidat
tercepat/tervalidasi dari riset lain. "Mendesain ulang recurrence" itu
sudah pernah dilakukan (di riset lain) — belum pernah diukur di shuffle-loop
ZepFold yang sama. Varian baru `scrambler_wired_addressable.c` (lihat
`RESULTS.md` untuk detail perubahan + validasi parity/statistik) mengisi
celah itu.

**Metodologi**: `run_speed_benchmark.sh` dijalankan ulang (`perf stat -r 30`,
`tokens.txt` 1.000.000 token, key=1) dengan `scrambler_wired_addressable`
ditambahkan sebagai binary ke-12, dalam sesi/mesin yang sama dengan
`scrambler_addressable`/`pcg` sehingga perbandingan langsung valid meski
kondisi mesin sesi ini bukan idle-terisolasi (lihat catatan variance di
subseksi sebelumnya).

| Binary | Waktu (s) | Rasio ke `pcg` |
|---|---:|---:|
| `pcg` | 0.053511 ± 2.06% | 1.00x |
| `scrambler_addressable` (recurrence original) | 0.074151 ± 0.76% | 1.386x |
| `scrambler_wired_addressable` (recurrence `winner_wired_v2`) | 0.070541 ± 2.21% | 1.318x |
| `stream_driven_shuffle` (recurrence `winner_wired_v2`, single-swap) | 0.078430 ± 0.71% | 1.466x |
| `ra_prng2` (kontrol, recurrence original, tanpa addressable-init) | 0.127373 ± 0.78% | 2.381x |

**Hasil**: `scrambler_wired_addressable` **~4.9% lebih cepat** dari
`scrambler_addressable` dalam sesi/kondisi mesin yang sama (0.070541s vs
0.074151s), delta melebihi gabungan noise kedua pengukuran (±0.76%/±2.21%).
Rasio ke `pcg` turun dari 1.386x ke 1.318x.

**Interpretasi (merevisi bagian atas)**: pruning+wiring recurrence memang
mengurangi biaya wall-clock, arah dan skalanya (~5%) konsisten dengan klaim
"~6% instruksi lebih sedikit" dari riset operand-position-search — jadi
bagian "biaya struktural TIDAK bisa diisolasi lebih jauh tanpa mendesain
ulang recurrence" di atas **tidak sepenuhnya tepat**: mendesain ulang
recurrence (via `winner_wired_v2`, yang sudah tervalidasi PractRand-clean
di riset lain) memang membantu, terukur ~5%. Namun perbaikan ini **modest,
bukan penutup gap** — `scrambler_wired_addressable` masih ~1.32x lebih
lambat dari `pcg`, jauh dari 1.0x. Kesimpulan "gap ~1.6x adalah batas
praktis desain shuffle-loop ZepFold saat ini" tetap berdiri untuk mayoritas
gap (I/O sudah dioptimasi, reseed+lebar-baca-`M[]` cuma ~13% gap per
subseksi sebelumnya, recurrence-pruning cuma ~5% lagi) — porsi terbesar gap
kemungkinan besar bukan di recurrence PRNG sama sekali, melainkan di
struktur shuffle-loop ZepFold sendiri (Lemire reduction ganda, dua-swap
array, overhead loop per-token di luar recurrence) yang tetap ada di kedua
varian recurrence.

Belum dijalankan: benchmark ini di mesin benar-benar idle (seperti
checkpoint "dekomposisi gap 1.62x" di atas) atau dekomposisi
`no_reseed_isolation_test`/`single_mread_isolation_test`-style untuk varian
`winner_wired_v2` — kalau ingin presisi lebih tinggi, itu scope baru.

## Reproduksi

```
cd experiments/2026-8-30_addressable-shuffle/speed_bench
./run_speed_benchmark.sh          # perf stat -r 30, semua 12 binary, ~2-3 menit
perf stat -r 10 -- ./io_only_isolation_test --input tokens.txt --output /tmp/x.txt --key 1
perf stat -r 10 -- ./winner_wired_addressable --stream 1 999999 > /dev/null
```

Log mentah: `speed_bench/speed_benchmark_perf.log`. Semua file output
(`out_*.txt`) diverifikasi berjumlah 1.000.000 token (sama dengan input),
tidak corrupt.

## Konteks tabel lama

Untuk perbandingan lintas-waktu: `benchmarks/comparisons/scrambling_speed/benchmark_perf.txt`
(metodologi historis, mesin/sesi berbeda) mencatat `ra_prng2`/`ra_prng3`
~0.1216s dan xoshiro/pcg/philox ~0.059-0.066s, chacha20-per-call ~0.1965s —
sangat konsisten dengan angka run terkontrol di atas (chacha20-per-call
0.1965s lama vs 0.1982s baru, selisih <1%), yang menguatkan bahwa hasil di
sini bukan kebetulan/noise mesin.
