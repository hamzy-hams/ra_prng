# Temuan metodologi: benchmark reinit-K vs steady-state untuk orbit/singleblock

Status: **SELESAI, DIKOREKSI** (investigasi ad-hoc, dipicu goal sesi
"cari penyebab kenapa orbit stream bisa jauh lebih lambat dari singleblock,
padahal perbedaannya cuma di swap L[]"). Lihat **§"KOREKSI"** di bawah —
kesimpulan awal dokumen ini (ringkasan eksekutif asli) **terbukti salah**,
dikoreksi pada sesi lanjutan yang sama hari.

## KOREKSI (ditulis setelah audit lanjutan, sesi sama)

Kesimpulan awal dokumen ini ("gap 17x itu artefak ILP lintas-panggilan,
gap sebenarnya cuma ~15%") **SALAH**. Root cause kesalahannya ada di
microbenchmark "adil" (`microbench_orbit_vs_singleblock.c`) yang dipakai
untuk klaim ~15% itu sendiri, bukan di metodologi resmi `bench_release.c`.

**Bug di microbenchmark saya sendiri**: harness resmi (`bench_release.c`,
fungsi `bench_real_reinit4`) menulis ke `volatile uint32_t sink` **hanya
1x per panggilan** (`sink ^= fn(key, 0u, k, NULL);`, sekali per batch
K-word) untuk mencegah dead-code-elimination pada return value. Microbench
"adil" saya menulis ke `volatile uint32_t *accum` **di DALAM loop dalam,
255x per panggilan** (`*accum ^= c;` tiap word) — 255x lebih sering.
Overhead paksa-tulis-memori sebesar itu nyaris tidak berpengaruh ke
`orbit` (yang memang sudah banyak akses memori lewat swap `L[i]<->L[d]`,
jadi menambah 1 write lagi per iterasi tidak banyak menambah biaya
marginal), tapi **mendominasi total biaya `singleblock`** (yang aslinya
nyaris murni komputasi register, nyaris tanpa akses memori sama sekali) —
sehingga gap keduanya "menyusut" secara artifisial jadi ~15% di
microbenchmark itu. Ini bukan properti algoritma, melainkan bug
instrumentasi di alat ukur saya sendiri.

**Verifikasi ulang** (`corrected_fair_bench.c`, link langsung ke
`src/ra_core/c/ra_core.c` produksi, teknik anti-DCE dan frekuensi sink
PERSIS meniru `bench_release.c`, plus 1 pengukuran genuine steady-state:
satu panggilan `ra_core_orbit` langsung, N=20 juta word, TANPA reinit sama
sekali):

| Pengukuran | ns/word |
|---|---:|
| singleblock K=255 (indirect call, sink 1x/panggilan) | 0.17–0.19 |
| orbit K=255 reinit (indirect call, sink 1x/panggilan) | 3.28–3.44 |
| **orbit --stream murni (1 panggilan, N=20M, tanpa reinit)** | 3.23–3.56 |
| **Rasio orbit-stream / singleblock K=255** | **17–21x** |

Dikonfirmasi juga dengan `perf stat -e instructions,cycles` (hardware
counter, bukan software timer): 9,119,349,964 instruksi / 3,176,501,476
cycles untuk keseluruhan program — konsisten dengan angka ns/word di atas
pada clock ~3.2-3.5GHz mesin ini.

**Kesimpulan yang benar**: gap ~17-21x antara `singleblock` dan `orbit`
**genuine, bukan artefak benchmark** — bahkan di mode `orbit --stream`
yang sepenuhnya continuous (tanpa reinit apa pun, sehingga efek ILP
lintas-panggilan-independen tidak relevan sama sekali di situ), gapnya
tetap sebesar itu (bandingkan `orbit K=255 reinit` 3.28-3.44 ns/word vs
`orbit --stream` 3.23-3.56 ns/word — beda <10%, bukan beda besar). Jadi
efek ILP lintas-panggilan itu SENDIRI nyata (menjelaskan kenapa
angka `real ns/word` dari `bench_real_reinit4` umumnya lebih baik
daripada model latensi-serial naif), tapi **efek itu memengaruhi
`singleblock` dan `orbit` secara proporsional serupa** (karena `orbit K=255
reinit` ≈ `orbit --stream`) — bukan penjelasan untuk KENAPA `singleblock`
jauh lebih murah. `singleblock` memang jauh lebih murah per-word karena
benar-benar tidak punya swap `L[]` dan tidak pernah lewat jalur reseed —
itu perbedaan biaya algoritma yang nyata, bukan kebetulan cara mengukur.

**Implikasi untuk publikasi (DIREVISI dari versi awal dokumen ini)**:
angka resmi `bench_release.c` (gap ~17x antara `singleblock K=255` dan
`orbit` di mode apa pun) **VALID dan boleh dikutip apa adanya** sebagai
perbandingan biaya algoritma — TIDAK perlu embel-embel "hanya berlaku
untuk workload reinit-heavy" seperti yang direkomendasikan versi awal
dokumen ini. Yang justru perlu diberi catatan adalah histori dokumen ini
sendiri: draft awal (di bawah, dipertahankan untuk jejak audit) sempat
salah menyimpulkan sebaliknya karena bug di microbenchmark ad-hoc, BUKAN
karena metodologi resmi bermasalah.

---

## [ARSIP — kesimpulan awal, SALAH, dipertahankan untuk jejak audit]

Bagian di bawah ini adalah versi asli dokumen sebelum koreksi di atas.
Disimpan apa adanya (bukan dihapus) supaya jejak bagaimana kesalahan
metodologis semacam ini bisa terjadi dan terkoreksi tetap terlihat —
relevan sebagai catatan metodologi tambahan untuk paper (lihat §"Rekomendasi
untuk penulisan paper" di bagian arsip, sebagian masih relevan meski
kesimpulan intinya sudah dikoreksi).

## Ringkasan eksekutif (ASLI, SALAH — lihat KOREKSI di atas)

Gap performa besar (~17x) yang dilaporkan `bench_release.c` antara
`singleblock K=255` (~21000 MB/s) dan `orbit multistream K=255`/`--stream`
(~1200 MB/s) **valid secara pengukuran** (diverifikasi dengan hardware
performance counter, bukan cuma software timer) **tapi TIDAK
merepresentasikan perbedaan biaya intrinsik per-word kedua algoritma**.
Begitu diukur dengan metodologi "satu aliran panjang, call langsung" alih-
alih "ribuan reinit independen lewat pointer-ke-fungsi", gap sebenarnya
menyusut ke **~15%** — jumlah yang konsisten dengan biaya swap `L[i]<->L[d]`
+ `ra_reseed` yang diamortisasi, bukan sesuatu yang butuh penjelasan
arsitektural besar.

**Implikasi untuk publikasi**: angka throughput reinit-K style (`K=1`/`K=255`
ala `bench_release.c`) TIDAK BOLEH dikutip sebagai "algoritma A adalah Nx
lebih cepat dari algoritma B" tanpa embel-embel workload — angka itu
mengukur seberapa baik CPU out-of-order bisa meng-overlap BANYAK PANGGILAN
INDEPENDEN sekaligus (instruction-level parallelism lintas-panggilan), bukan
biaya komputasi satu panggilan. Laporkan kedua gaya pengukuran (reinit-K
DAN steady-state single-stream) dengan label eksplisit apa yang diukur.

## Kronologi investigasi

1. **Awal**: dugaan swap `L[]` adalah biang overhead orbit vs singleblock.
   Benchmark CLI (`--stream` dengan `fwrite` per word ke pipe) menunjukkan
   gap ekstrem tapi ternyata didominasi overhead I/O stdio, bukan
   komputasi — metodologi salah.
2. **Koreksi 1**: ditemukan `bench_release.c` (metodologi resmi project)
   memakai `raw_stream=NULL` (tanpa I/O sama sekali), memberi angka resmi
   `orbit --stream = 3.128 ns/word`.
3. **Ablation `always_inline`**: hipotesis bahwa `ra_permutation_cycle_orbit`
   yang tidak `always_inline` (beda dari singleblock yang sudah) adalah
   biang gap. **Hasil: HIPOTESIS GUGUR** — `always_inline` justru membuat
   orbit ~10-15% LEBIH LAMBAT (2.88-3.09s vs 3.30-3.67s untuk 200M word),
   diuji dengan kontrol yang benar (fix off-by-one tetap ada, cuma atribut
   inline yang divariasikan). Perubahan direvert.
4. **Ditemukan bug DCE (di microbenchmark SAYA, bukan project)**: microbench
   ad-hoc pertama yang manggil `ra_core_singleblock` LANGSUNG (bukan lewat
   pointer) menunjukkan angka fisik mustahil (0.007 ns/word, <0.1 siklus
   CPU/word). Dikonfirmasi via disassembly: karena `cons` singleblock TIDAK
   PERNAH di-reassign untuk `rng<=255` (tidak ada `ra_reseed` di jalur itu),
   compiler membuktikan `ra_core_singleblock(key,addr_hi,rng<=255,NULL)`
   SELALU `== key`, lalu membuang SELURUH loop hash 255-word sebagai dead
   code — loop reinit yang diukur ternyata cuma 4 instruksi
   (`xor`/`sub`/`cmp`/`jne`), tidak ada `call` ke fungsi singleblock sama
   sekali.
5. **Investigasi meluas ke binary resmi**: dicek apakah `bench_release.c`
   (yang memanggil lewat pointer-ke-fungsi `fn(...)`, bukan langsung) kena
   bug yang sama. Disassembly `bench_real_reinit4.constprop.{0,1}` di binary
   resmi menunjukkan `call *%rbp` (indirect call GENUINE, tidak
   tereliminasi), dan `ra_core_singleblock.part.0` genuinely berisi 9
   instruksi rotate + 4 loop backward — **bukan dead code**.
6. **Kesalahan aritmatika sementara**: sempat salah membagi total
   instruksi/cycles hasil `perf stat` (yang mencakup SEMUA 5 TRIALS di
   `bench_real_reinit4`) dengan word count SATU trial saja, menghasilkan
   kesimpulan sementara yang salah ("IPC >20, mustahil secara fisik").
   Setelah dikoreksi (bagi dengan `5 × word_count`), angka `cycles/word`
   dari hardware counter (`perf stat -e cycles`) **cocok persis** dengan
   `real_ns_per_word` yang dilaporkan `bench_release.c` sendiri — metodologi
   resmi project **valid**, tidak ada bug pengukuran.
7. **Sisa pertanyaan**: kalau bukan bug, kenapa gap 17x muncul di
   `bench_real_reinit4` (reinit-K style) tapi cuma ~15% di microbenchmark
   "adil" (`volatile` accumulator, satu loop kontinu, call langsung)? Lihat
   §"Penjelasan gap" di bawah.

## Bukti kunci (perf stat, isolated per-fungsi)

Diuji dengan harness terisolasi (`gcc -O3 -march=native`, `CYCLES_TARGET=
30_000_000`, 5 trials, sama persis dengan `bench_release.c`):

| Fungsi (K=255, via `bench_real_reinit4`) | instructions (5 trial) | cycles (5 trial) | cycles/word |
|---|---|---|---|
| `ra_core_singleblock` | 342,477,324 | 104,172,039 | 0.69 |
| `ra_core_orbit` | 5,227,181,201 | 1,826,658,227 | 12.18 |

Rasio cycles/word: **~17.6x** — cocok dengan `real_MBps` ratio yang
dilaporkan (~21000 vs ~1200 MB/s). Ini **genuine**, dikonfirmasi hardware
counter, bukan artefak software-timer atau dead-code-elimination.

Microbenchmark "adil" (formula identik, `volatile uint32_t *accum`
menggantikan `fwrite`/return-value, call langsung bukan lewat pointer,
satu loop kontinu memproses banyak block/word berturut-turut dalam SATU
pemanggilan fungsi terluar):

| Varian | ns/word |
|---|---|
| `orbit_true` (swap `L[]` + reseed) | ~3.52-3.86 |
| `singleblock_true` (tanpa swap/reseed) | ~3.07-3.32 |

Rasio: **~1.15-1.16x** — jauh lebih kecil dari 17.6x di atas.

## Penjelasan gap (hipotesis kerja, belum full-proof)

`bench_real_reinit4` memanggil fungsi **lewat pointer, berulang kali,
untuk KEY YANG BERBEDA setiap panggilan** — setiap panggilan sepenuhnya
independen satu sama lain (tidak ada data yang mengalir dari panggilan
ke-N ke panggilan ke-N+1). CPU out-of-order modern bisa mulai mengeksekusi
instruksi dari panggilan berikutnya SEBELUM panggilan sebelumnya selesai,
selama tidak ada dependency — reorder buffer bisa menampung banyak
"panggilan independen" sekaligus dan mengeksekusinya paralel di berbagai
execution port, mendekati throughput yang jauh lebih tinggi daripada
1/latency satu panggilan.

`ra_core_singleblock` (state kecil — cuma `M[256]`, tanpa akses memori
data-dependent apa pun di loop-nya) jauh lebih mudah di-overlap begini oleh
CPU dibanding `ra_core_orbit` (state 2x lebih besar — `L[256]`+`M[256]`,
DAN py swap `L[i]<->L[d]` yang alamatnya data-dependent tiap iterasi,
kemungkinan menciptakan potential-aliasing hazard yang membatasi seberapa
jauh CPU/compiler berani me-reorder eksekusi lintas-panggilan). Ini
BUKAN dugaan final — untuk membuktikannya perlu profiling lebih dalam
(mis. `perf stat -e resource_stalls`, atau membandingkan reorder-window
occupancy) yang di luar scope investigasi ad-hoc ini.

Yang PASTI (dibuktikan langsung): kedua metodologi itu **sama-sama sah**,
tidak ada yang "cacat" — keduanya cuma mengukur skenario beban kerja yang
berbeda:
- `bench_real_reinit4` (reinit-K) = representatif untuk pola pakai
  "generate banyak stream pendek independen berturut-turut, tanpa
  pekerjaan lain di antaranya" (mis. bulk-generate banyak key sekaligus).
- Microbenchmark `_true` (steady-state, call langsung) = representatif
  untuk pola pakai "satu aliran panjang" ATAU "banyak reinit tapi dengan
  pekerjaan lain interleaved yang mematahkan kesempatan CPU meng-overlap
  panggilan independen" (lebih dekat ke penggunaan dunia nyata yang
  jarang murni back-to-back tanpa jeda).

## Rekomendasi untuk penulisan paper

1. **Jangan kutip rasio tunggal** ("singleblock Nx lebih cepat dari
   orbit") tanpa menyebut workload persis yang diukur — sertakan angka
   reinit-K DAN steady-state, dengan definisi eksplisit keduanya.
2. Kalau paper mengklaim keunggulan kecepatan singleblock/no-L design
   dibanding orbit/addressable design, dasarkan pada angka steady-state
   (~15%, biaya swap+reseed yang genuinely intrinsik), bukan angka
   reinit-K (~17x, artefak ILP-lintas-panggilan) — kecuali use-case nyata
   yang dibahas memang persis pola "banyak reinit independen berturut-turut"
   (dalam hal ini boleh dikutip, TAPI harus dijelaskan mengapa angkanya
   sebesar itu, bukan dibiarkan seolah itu murni biaya algoritma).
3. Pertimbangkan mencantumkan temuan metodologi ini sendiri sebagai catatan
   kaki/lampiran — reviewer PRNG/HPC yang familiar dengan efek ILP-lintas-
   panggilan out-of-order akan menghargai kejelasan ini, dan itu mencegah
   klaim performa disanggah setelah publikasi.
4. Periksa apakah perbandingan Philox/Xoshiro di `bench_release.c` (juga
   pakai `bench_real_reinit`/`bench_real_reinit4` style) kena efek serupa
   sebelum dikutip di paper — TIDAK diverifikasi dalam investigasi ini
   (di luar scope), tapi pola strukturalnya identik jadi kemungkinan besar
   berlaku juga untuk keduanya.

## File yang dihasilkan sesi ini

- `microbench_orbit_vs_singleblock.c` — microbenchmark "adil" yang
  **TERNYATA BUGGY** (lihat §KOREKSI di atas): klaim gap ~15%-nya salah
  karena bug frekuensi volatile-write, bukan temuan valid. Dipertahankan
  di repo apa adanya sebagai jejak audit, JANGAN dikutip angkanya.
- Tidak ada perubahan ke `src/ra_core/c/ra_core.c` dari investigasi ini
  (eksperimen `always_inline` sudah di-revert, kode tetap di state fix
  off-by-one saja).

---

## Rekomendasi untuk penulisan paper (REVISI, pasca-koreksi)

Menggantikan rekomendasi di §arsip di atas (poin 1-2 di sana sudah tidak
berlaku karena premisnya salah):

1. **Angka `bench_release.c` boleh dikutip apa adanya** — gap ~17x antara
   `singleblock K=255` dan `orbit` (mode apa pun: reinit-K atau
   `--stream`) sudah diverifikasi genuine lewat 2 jalur independen
   (perf-stat hardware counter session sebelumnya + reproduksi ulang
   dengan `corrected_fair_bench.c` yang link langsung ke `ra_core.c`
   produksi). Tidak perlu embel-embel "hanya berlaku untuk workload
   reinit-heavy" — gap itu bertahan bahkan di continuous stream murni.
2. **Poin metodologi yang TETAP valid dan layak dicatat di paper**: teknik
   anti-dead-code-elimination itu sendiri penting untuk benchmark PRNG
   apa pun yang punya jalur "return value bisa provably invariant" (mis.
   generator apa pun yang punya fast-path tanpa reseed) — pakai
   **indirect call (function pointer)**, bukan direct call, supaya
   compiler tidak bisa membuktikan invariance lintas call-boundary.
   Kalau butuh teknik tambahan (volatile accumulator) untuk kasus lain,
   **jaga frekuensi tulisnya sama dengan pola pemakaian nyata** (1x per
   unit-kerja yang relevan, bukan 1x per elemen internal) — sesi ini
   sendiri jadi contoh kasus nyata kalau ini dilanggar.
3. Poin #4 di rekomendasi arsip (audit Philox/Xoshiro terhadap efek
   serupa) **masih relevan dan belum dikerjakan** — tapi pertanyaannya
   sekarang berbeda: bukan "apakah Philox/Xoshiro juga kena inflasi ILP
   yang harus dikoreksi", melainkan "apakah perbandingan `singleblock` vs
   Philox/Xoshiro di `bench_release.c` (§4 `docs/PITCH.md`) memang
   mengukur ketiganya dengan gaya yang SAMA (semuanya reinit-K)" — ini
   soal konsistensi metodologi antar-baris tabel, bukan soal validitas
   gap itu sendiri. Lihat audit terpisah di
   `experiments/2026-9-1_family-productionization/RESULTS.md:192-204`.
