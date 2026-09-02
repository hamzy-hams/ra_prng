# Karakterisasi ambang K minimum aman — `ra_core_singleblock`

## Konteks

`ra_permutation_cycle_singleblock` (`../2026-9-1_family-productionization/ra_core.c:214-236`)
selalu mulai loop dari `i=255` dan berhenti setelah `count` (=K) output —
K kecil berarti sedikit putaran mixing sebelum output pertama, dan pola
multikey (banyak key pendek berturut-turut) mengekspos korelasi struktural
antar-key lewat PractRand BCFN. K=1 terbukti katastrofik (R=+1028-1069 di
1GB). Eksperimen pre-mix/reseed-sebelum-cycle (`../2026-9-2_singleblock-prereseed-experiment/`)
sudah dicoba dan GAGAL menutup ini — root cause ada di *bentuk transformasi
satu-putaran*, bukan kualitas seed masuknya.

Tujuan folder ini: cari ambang K minimum secara empiris sebagai dasar
keputusan (redesign cycle vs restrict rentang K valid), bukan menebak.
Semua uji memakai formula `ra_core_singleblock` SEKARANG (dengan guard-fix
`key=0` sudah diterapkan, `GUARD_L=0x38916df4`/`GUARD_M=0x6c26fc92`).

## Metodologi

`multikey_stream_k.c` (baru, folder ini) — sama seperti
`../2026-9-1_family-productionization/multikey_stream.c` tapi `block_len`
(K) jadi argumen CLI (`--stream <base_key> <n> <K>`), key derivation Weyl
increment `0x9E3779B9` identik. Dibangun dari `ra_core_nomain.o` (compile
ulang dari `ra_core.c` yang sama, path relatif, source tidak disalin).
`RNG_test stdin32 -tlmin ... -tlmax ... -multithreaded`, base_key=111222
tetap di semua run untuk konsistensi. Fail-fast: kalau checkpoint kecil
sudah FAIL katastrofik, tidak lanjut ke skala lebih besar untuk titik K
itu; kalau bersih, diperpanjang sampai skala yang meyakinkan (minimal
8GB, kandidat ambang diperdalam sampai 32GB — menyamai bar yang sudah
dipakai untuk memvalidasi K=255).

## Hasil sweep

| K | Hasil | Detail |
|---|---|---|
| 1 | FAIL katastrofik | BCFN R=+1028-1069 @1GB (diketahui dari fork sebelumnya) |
| 2 | FAIL katastrofik | BCFN R=+255 @1GB, **BRank(12):2K(2) R=+6000** @1GB — lebih parah dari K=1 di beberapa test |
| 4 | FAIL katastrofik | BCFN R=+116.8 @1GB |
| 8 | FAIL katastrofik | BCFN R=+41.7 @1GB |
| 16 | FAIL, tumbuh tajam | BCFN R: +10.1(1GB, mild)→+18.8(2GB, very susp.)→+34.1(4GB, **FAIL**) |
| 32 | FAIL, tumbuh lambat | bersih ≤2GB, BCFN R=+10.7(4GB, mild)→+17.1(8GB, very susp.) — masih naik |
| 64 | FAIL, tumbuh sangat lambat | bersih ≤8GB, BCFN R=+10.1(16GB, mild)→+17.1(32GB, very susp.) — pola sama persis dengan K=32, cuma bergeser ke skala 2x lebih besar |
| 72 | FAIL, tumbuh lambat | bersih ≤8GB, BCFN R=+7.9(16GB, unusual)→+13.4(32GB, **very suspicious**) — masih pola sama, sedikit lebih lambat dari K=64 |
| 80 | Bersih (1 noise ringan) | bersih ≤16GB; @32GB: `[Low1/32]TMFn(2+3):wl` R=+18.8 (unusual) — **test BEDA, bukan BCFN**, tidak ada pola tumbuh; ciri statistik noise biasa bukan defect nyata |
| 96 | **Bersih total** | 0 anomali di SEMUA checkpoint 1GB→32GB (194→251 test, semua lolos) |
| 128 | Bersih (1 noise, self-resolve) | bersih ≤8GB; @16GB: BCFN R=+7.7 (unusual) tunggal, **hilang lagi @32GB** (0 anomali) — pola self-resolving yang sudah dikenal jadi artefak normal di repo ini |
| 255 | Bersih total (dari fork sebelumnya) | 0 anomali sampai 32GB, sudah diverifikasi terpisah dengan guard-fix |

## Ambang

**Defect BCFN nyata (pola tumbuh monoton seiring skala) masih terbukti ada
sampai K=72.** K=80 ke atas tidak lagi menunjukkan pola tumbuh BCFN —
anomali yang muncul (kalau ada) berbeda jenis test dan tidak konsisten
dengan sinyal defect (baik isolated-noise di K=80, maupun self-resolving
di K=128). **K=96 bersih sempurna di seluruh rentang uji (1-32GB).**

**Ambang minimum aman: K ≈ 80-96** (batas bawah pasti tidak aman di ≤72;
batas atas pasti aman di ≥96; K=80 kemungkinan besar sudah aman tapi satu
titik data tunggal dengan 1 anomali lemah tidak sepenuhnya meyakinkan
tanpa run ulang/seed lain). **Rekomendasi konservatif: K≥96** sebagai
syarat pemakaian `ra_core_singleblock` yang aman secara statistik,
berdasarkan data yang ada.

## Overlay trade-off performa

Dari tabel bench yang sudah ada (`RESULTS_TAHAP2.md`, reuse, tidak
di-rerun sesuai scope tugas ini):

| K | singleblock ns/word | orbit ns/word |
|---|---|---|
| 1 | 38.135 | ~2.0-2.4 |
| 10 | 4.659 | ~2.0-2.4 |
| 100 | 0.422 | ~2.0-2.4 |
| 255 | 0.135 | ~2.0-2.4 |

Interpolasi log-log antara titik K=10 dan K=100 (data yang ada, bukan run
baru): crossover di mana `singleblock` sama cepatnya dengan `orbit`
(~2.2 ns/word) ada di **K≈20-21**. Untuk K≥~20, `singleblock` sudah lebih
cepat dari `orbit`; makin besar K makin jauh selisihnya.

**Implikasi penting**: ambang aman empiris (K≈80-96) berada JAUH DI ATAS
titik crossover (K≈20) — bukan di bawahnya. Artinya `ra_core_singleblock`
**masih bermakna secara praktis** di rentang aman ini: pada K=96
(interpolasi kasar dari K=10/K=100, ~0.44-0.45 ns/word), `singleblock`
tetap ~5x lebih cepat dari `orbit`. **`ra_core_singleblock` TIDAK
kehilangan alasan keberadaannya** — tapi klaim use-case aslinya ("frequent
init, **few words per address**", eksplisit menyebut K kecil termasuk K=1)
tidak lagi valid untuk K di bawah ~80-96. Use-case yang tetap valid:
"batch sedang-besar per alamat, tetap jauh lebih murah dari orbit" —
bukan lagi "aggressive call-and-discard super agresif" seperti yang
awalnya diiklankan.

## Batasan &amp; catatan

- Angka speed di atas HASIL INTERPOLASI dari tabel lama (K=10/K=100), BUKAN
  benchmark baru — kalau presisi lebih tinggi dibutuhkan di K=80-96 secara
  spesifik, perlu run `bench_ra_core` reinit-sweep di titik itu langsung
  (di luar scope tugas ini).
- Ambang K=80 hanya diuji SATU base_key (111222) dan SATU seed — untuk
  klaim produksi yang lebih kuat, sebaiknya dikonfirmasi dengan base_key
  berbeda sebelum dipakai sebagai keputusan resmi (bukan bagian scope
  karakterisasi awal ini).
- `ra_core.c` TIDAK diubah sama sekali di sesi ini (murni pengukuran,
  sesuai batasan scope).
- Tidak mencoba redesign cycle apa pun (sudah 1x dicoba dan gagal terpisah,
  di luar scope tugas ini).

## Pertanyaan terbuka untuk user

Data ini adalah input untuk keputusan berikutnya, bukan rekomendasi
kebijakan:
1. **Restrict K**: dokumentasikan `ra_core_singleblock` valid resmi hanya
   untuk K≥96 (atau K≥80 kalau dikonfirmasi ulang dulu), guard eksplisit
   di kode (seperti guard `rng>255` yang sudah ada) untuk `rng<96` — cepat,
   tidak butuh riset formula lagi, tapi menyempitkan use-case asli.
2. **Redesign cycle** supaya aman di K berapa pun (termasuk K=1) — perlu
   riset lebih lanjut (percobaan pre-mix sudah gagal 1x; butuh ide baru,
   kemungkinan menyentuh `ra_permutation_cycle_singleblock`'s bentuk
   transformasi per-putaran, bukan cuma input awalnya).
3. Opsi lain yang belum dipertimbangkan di sini.
