# Hasil: karakterisasi ekshaustif f(key) untuk `ra_core_orbit` K=1

Eksekusi penuh dari `HANDOVER.md`. Semua kode di `exhaustive_analysis.c`
(read-only terhadap `ra_core.c` -- tidak ada satu baris pun di `ra_core.c`
yang diubah).

## 0. Derivasi f(key): byte-verified

`f(key)` (output word tunggal K=1) diimplementasikan ulang dari
`../2026-9-1_family-productionization/ra_core.c` (rot32 L144-147, fmix32
L149-156, GUARD_M L163, `ra_init_state_orbit` L197-207 dibatasi ke 8 entri
`M[]` yang relevan, `ra_permutation_cycle_orbit` ronde i=255 L217-252).
Detail formula persis ada di comment block atas `exhaustive_analysis.c`.

**Verifikasi**: `./exhaustive_analysis --verify 1000000` -> **0 mismatch**
dari 1 juta key acak, dibandingkan byte-exact terhadap `ra_core_orbit(key,
1, stream)` produksi asli (ditangkap lewat `open_memstream` -- catatan
penting: `ra_core_orbit` me-return `cons`/state reseed, BUKAN output word
`c`; hanya `fwrite` ke `raw_stream` yang membawa nilai output sebenarnya).

`f(key)` terbukti benar. Semua analisis di bawah memakai `f(key)` ini
langsung (tanpa overhead array 256-entry penuh / `L[]`).

## 1. Sapuan injektif/collision ekshaustif (full 2^32, 2-bit counter)

Timing sample memproyeksikan seluruh run gabungan (Analisis 1+2+3
sekaligus) < 20 menit (proyeksi murni komputasi ~1.5 menit, plus overhead
cache-miss array 1GiB) -- **full exhaustive 2^32 dijalankan**, selesai
dalam **633.4 detik (~10.5 menit)**.

| bucket | observed | expected (fungsi acak ideal) | z |
|---|---:|---:|---:|
| count=0 (tak pernah muncul) | 1.581.547.084 | 1.580.030.168,7 | +38,16 |
| count=1 | 1.578.543.366 | 1.580.030.168,7 | -37,40 |
| count=2 | 789.227.908 | 790.015.084,4 | -28,01 |
| count>=3 (saturated) | 345.648.938 | 344.891.874,2 | +40,77 |
| distinct value hit >=1x | 2.713.420.212 | 2.714.937.127,3 | -48,00 |

**Interpretasi hati-hati**: ini populasi PENUH (2^32 keys, bukan sampel),
jadi z-score di sini bukan "signifikansi statistik akibat noise sampling"
(tidak ada noise sampling -- kita punya ground truth eksak), melainkan
"seberapa jauh profil occupancy `f` dari profil TIPIKAL yang akan
dihasilkan fungsi acak generik berdomain/rentang sama". Polanya konsisten
dan sistematis (lebih banyak nilai tak-pernah-muncul DAN lebih banyak
nilai bertabrakan >=3x, lebih sedikit nilai hit tepat 1x) -- **bukti
matematis f(key) sedikit lebih clustered/kurang bijektif dibanding fungsi
acak ideal**. TAPI deviasi relatifnya sangat kecil (~0,1% dari total) --
z besar di sini murni produk dari n=4,3 miliar (statistical power
sebanding sqrt(n)), bukan efek besar. **Penting**: sapuan ini
ORDER-INDEPENDEN (bijectivity adalah properti f itu sendiri, tidak
bergantung urutan key), jadi temuan ini TIDAK bisa menjelaskan kenapa
Weyl-ordering CLEAN sementara hashed-ordering FAIL -- keduanya menyapu
domain key yang PERSIS SAMA. Kesimpulan: defect kecil real ini kemungkinan
BUKAN penyebab (utama) FAIL PractRand yang order-dependent.

## 2. Histogram FPF-14+6/16 -- ditemukan mekanisme konkret

Awalnya dijalankan pada key berurutan (`0,1,2,...,2^32-1`) sesuai bacaan
literal HANDOVER ("reuse sapuan ekshaustif"). Hasil: **CLEAN** (semua
z-eksponen <1.3, tidak ada bin individual >z=4.5 di luar yang wajar untuk
~490rb bin yang di-scan). **Tapi ini salah sasaran** -- urutan sekuensial
integer adalah urutan ke-4 yang belum pernah diuji PractRand sama sekali,
BUKAN urutan yang benar-benar FAIL (hashed) atau CLEAN (Weyl) di axis asli.
Dikoreksi dengan mode `--fpf-order {hashed,weyl,...}` yang men-generate
key PERSIS sesuai ordering asli (splitmix32-chain utk hashed, konvensi
`splitmix32` disamakan ke `diag_orbit_k1_hashed.c` -- ambil UPPER 32-bit,
bukan lower).

FPF-14+6/16 diporting **persis** dari
`~/Documents/research/PractRand/src/tests.cpp` L4084-4235 (cabang "large
footprint, short stride, STRIDE_BITS=16" -- cabang yang benar-benar dipakai
untuk parameter sig_bits=14, exp_bits=6, stride_bits_L2=4; `max_exp`
ter-clamp ke 51, bukan 63 mentah, karena footprint>64 di L3943-3944).

Statistik yang dipakai: **agregat chi-square intra-eksponen** (untuk tiap
level eksponen `e` dengan `expected/bin>=20`, hitung chi-square distribusi
`sig` [16384 nilai] vs uniform, jumlahkan semua level `e` -- ini
mendekati semangat statistik `:all` PractRand yang menjumlahkan banyak
bias kecil, bukan mencari satu bin ekstrem).

500 juta key/ordering (~1 miliar sampel, comparable skala dengan
512MB-1GB tempat PractRand asli FAIL untuk hashed):

| ordering | agg_chi2 | df | agg_z | catatan |
|---|---:|---:|---:|---|
| **hashed** (splitmix32-chain, seed A) | 193.457,8 | 180.213 | **+22,06** | **ANOMALI KUAT** |
| **hashed** (splitmix32-chain, seed B beda) | 192.782,1 | 180.213 | **+20,94** | reproducible lintas seed |
| weyl (`key += 0x9E3779B9`) | 180.216,2 | 180.213 | +0,005 | nyaris sempurna uniform |
| sequential (`key += 1`) | 179.647,3 | 180.213 | -0,942 | clean |
| step lain (`key += 0x12345679`) | 179.244,7 | 180.213 | -1,613 | clean |
| fmixhash (`key = fmix32(i)`) | 180.131,4 | 180.213 | -0,136 | clean |

Breakdown per-eksponen untuk **hashed** (satu-satunya yang anomali; semua
11 level `e` yang diuji SEMUA positif, tidak seperti ordering lain yang
berfluktuasi acak di kedua arah):

```
e= 0 z=+12.516   e= 4 z=+6.276   e= 8 z=+5.022
e= 1 z= +9.029   e= 5 z=+5.120   e= 9 z=+4.836
e= 2 z= +6.900   e= 6 z=+5.782   e=10 z=+6.376
e= 3 z= +6.363   e= 7 z=+4.951
```

**Ini bukti matematis langsung dan definitif**: given eksponen `e`
(jumlah trailing-zero pada window bit 32-bit dari stream round-1
berturutan), distribusi bit-signifikan (`sig`) untuk ordering **hashed**
TIDAK uniform -- pola ini KONSISTEN di semua level `e` yang diuji (semua
positif, bukan campur acak), dan TIDAK ADA untuk ordering lain manapun
yang diuji (weyl/sequential/step lain/fmixhash, semua z dalam rentang
noise -2 s.d. +2.3). Ini pertama kalinya sesi mana pun di riwayat defect
ini mengukur pola konkret alih-alih hanya menerima verdict FAIL/CLEAN
PractRand sebagai kotak hitam.

**Hipotesis yang REFUTED oleh data ini**: "step aritmetik tetap = clean,
key independen = dirty" -- REFUTED oleh `fmixhash` (key independen via
hash BUKAN splitmix32, tetap CLEAN, z=-0.136). Jadi defect ini tampaknya
**spesifik ke interaksi antara konstruksi splitmix32-chain (dgn truncation
upper-32-bit, seperti dipakai di `diag_orbit_k1_hashed.c`) dan formula
`f(key)`**, bukan properti umum "independensi key". Mekanisme presisnya
(kenapa splitmix32 spesifik, bukan fmix32 atau step aritmetik apa pun)
**belum diketahui** -- ini pertanyaan terbuka baru, lebih sempit dan lebih
terarah dari sebelumnya, tapi TIDAK diselidiki lebih lanjut di sesi ini
(di luar cakupan yang disepakati; lihat larangan HANDOVER soal fix
formula baru).

## 3. Korelasi pembatalan-Weyl (kontingensi 256x256 + chi-square)

Diuji PERSIS sesuai spek HANDOVER: `(f(key), f(key+0x9E3779B9))` vs
`(f(key), f(key+delta_random))`, delta_random=0x3ef20a84 (dipilih acak
sekali di awal run, full 2^32 pasangan):

| pasangan | chi2 | df | z |
|---|---:|---:|---:|
| Weyl (key, key+0x9E3779B9) | 64.750,60 | 65.025 | -0,761 |
| kontrol (key, key+0x3ef20a84) | 64.719,14 | 65.025 | -0,848 |

**Hipotesis "pembatalan-Weyl sistematis pada level pairwise (key,
key+delta)" REFUTED secara definitif** oleh pengukuran full-population:
chi2 Weyl vs kontrol praktis identik (selisih <0,05% dari df), keduanya
sedikit di bawah ekspektasi chi-square murni acak (z negatif kecil,
kemungkinan artefak kuantisasi top-8-bit, bukan sinyal). Tidak ada
perbedaan bermakna antara step Weyl dan step acak lain pada level korelasi
pairwise top-8-bit ini.

**Catatan penting**: hasil ini TIDAK bertentangan dengan temuan Analisis 2
di atas -- Analisis 3 menguji hubungan ADITIF TETAP (`key+delta` konstan),
sedangkan ordering "hashed" yang FAIL di Analisis 2 memakai CHAIN
splitmix32 (`key_{i+1} = splitmix32(state)`), hubungan non-aritmetik yang
sama sekali berbeda antar key berurutan. Analisis 3 me-refute hipotesis
spesifik "step Weyl vs step acak lain", bukan hipotesis umum tentang
hashed-chain.

## 4. Uji hipotesis duplikasi key (susulan -- SplitMix32-chain BUKAN faktor kunci)

**Koreksi atas kesimpulan Analisis 2**: kesimpulan "mekanismenya spesifik
ke splitmix32-chain" di atas KELIRU. User menunjukkan bahwa
`/dev/urandom`-key sudah dibuktikan FAIL juga di sesi lain
(`../2026-9-5_orbit-k1-urandom-key-diagnosis/RESULTS.md`, PractRand asli:
R=+5,8@512MB -> +28,4@2GB FAIL, eskalasi identik dengan hashed) --
`/dev/urandom` sama sekali tidak berhubungan aljabar dengan SplitMix32.
Jadi bukan soal algoritma generator key.

**Hipotesis baru**: bukan soal *algoritma* key, tapi soal apakah key
**dijamin tidak pernah berulang** dalam satu run. `weyl`/`sequential`/
`fmixhash` semuanya PERMUTASI (fungsi bijektif) atas domain 32-bit --
key tidak pernah berulang. `hashed` (splitmix32-chain, truncated) dan
`/dev/urandom` berperilaku statistik seperti draw *dengan pengembalian*
-- bisa berulang. Karena `f(key)` deterministik, key yang berulang
otomatis menghasilkan **word output identik persis, dua kali**. Pada
N=500 juta sampel dari domain ~4,3 miliar, ekspektasi jumlah key
berulang (birthday paradox) `~= N^2/(2*2^32) ~= 29 juta (~5,8%)` --
cukup besar untuk membiaskan statistik window-lokal seperti FPF.

**Uji 1 -- dedup langsung** (mode `--fpf-order {urandom,dedup-hashed,dedup-urandom}`
baru, ditambahkan ke `exhaustive_analysis.c`; `urandom` baca langsung dari
`/dev/urandom` lewat tool yang sama untuk perbandingan apple-to-apple;
`dedup-*` pakai bitmap "seen" 512MiB, redraw kalau key sudah pernah
dipakai di run ini). Semua di N=500 juta:

| ordering | agg_chi2 | agg_z | keterangan |
|---|---:|---:|---|
| hashed (splitmix32-chain) | 193.457,8 | **+22,06** | rusak (baseline, sama seperti Analisis 2) |
| **urandom** (tool sama, bukan PractRand terpisah) | 193.294,6 | **+21,79** | rusak, **nyaris identik** dengan hashed |
| **dedup-hashed** | 180.046,1 | **-0,28** | **BERSIH** |
| **dedup-urandom** | 181.067,0 | **+1,42** | **BERSIH** |

Redraw attempts dedup: 31.568.030 (hashed) dan 31.585.971 (urandom) dari
500 juta draw -- sangat dekat dengan prediksi teoretis birthday-paradox
(~29,1 juta), memverifikasi mekanisme dedup bekerja sesuai desain
(key unik terpakai == n, dicek eksplisit di kode).

**Hasil: cukup dengan membuang duplikat key, KEDUA sumber yang tadinya
rusak parah (hashed DAN urandom, sama-sama z~+22) langsung jadi bersih
total (z antara -0,3 dan +1,4, dalam rentang noise).** Ini konfirmasi
langsung, bukan lagi dugaan.

**Uji 2 -- scaling N** (ordering `hashed`, tanpa dedup, N bervariasi):

| N | agg_chi2 | agg_df | agg_z | ekspektasi jumlah key berulang (teori) |
|---:|---:|---:|---:|---:|
| 10.000.000 | 81.628,5 | 81.915 | -0,71 | ~11.600 (0,12%) |
| 50.000.000 | 131.677,0 | 131.064 | +1,20 | ~290.700 (0,58%) |
| 100.000.000 | 150.532,7 | 147.447 | +5,68 | ~1.162.800 (1,16%) |
| 200.000.000 | 168.754,8 | 163.830 | +8,60 | ~4.651.200 (2,33%) |
| 500.000.000 | 193.457,8 | 180.213 | +22,06 | ~29.069.800 (5,81%) |

Pola yang muncul: **nyaris nol/noise di N kecil** (di mana tabrakan key
langka), lalu **tumbuh jauh lebih cepat dari N** begitu tabrakan key mulai
signifikan -- persis pola yang diprediksi mekanisme birthday-paradox
(`~N^2`), BUKAN pola linear-per-sampel yang diharapkan dari "bias
per-sampel independen dari algoritma tertentu" (kalau itu penyebabnya,
N=10 juta harusnya sudah menunjukkan sinyal proporsional, bukan noise
murni). Catatan kejujuran: rasio pasti excess-chi2/jumlah-tabrakan tidak
konstan sempurna di seluruh rentang N (bervariasi ~0,0005-0,0027,
kemungkinan karena agregasi chi-square yang dipakai di sini kasar, bukan
model per-duplikat yang presisi) -- tapi arah dan orde-magnitudo
pertumbuhannya jelas konsisten dengan hipotesis, dan Uji 1 (dedup) di
atas sudah memberi bukti yang jauh lebih definitif dan langsung.

### Kesimpulan Analisis 4

**Hipotesis duplikasi key TERKONFIRMASI, mengganti kesimpulan
"spesifik-splitmix32" di Analisis 2.** Mekanisme yang sebenarnya:

> `ra_core_orbit` K=1, ketika di-reinit dengan key yang sama dua kali
> dalam satu stream, secara deterministik menghasilkan word output yang
> identik persis dua kali. Sumber key APA PUN yang bisa menghasilkan key
> berulang dalam jendela pengamatan (SplitMix32-chain, `/dev/urandom`,
> dan kemungkinan besar CSPRNG lain seperti ChaCha20 -- belum diuji
> langsung, tapi prediksi hipotesis ini) akan memicu bias FPF-14+6/16
> begitu tingkat pengulangan key jadi signifikan secara statistik (skala
> ~ratusan juta key dari domain 2^32). Sumber key APA PUN yang berupa
> permutasi/bijeksi atas domain key (Weyl, sequential, fmix32-of-sequential,
> atau step aritmetik tetap lain) TIDAK PERNAH mengulang key, sehingga
> tidak pernah memicu defect ini, terlepas dari algoritma spesifiknya.

Ini juga menjawab pertanyaan lama "kenapa Weyl istimewa": **Weyl tidak
istimewa secara aljabar** -- ia cuma kebetulan (seperti sequential/
fmixhash) merupakan salah satu dari banyak cara membuat urutan key tanpa
pengulangan. Pertanyaan yang tersisa bukan lagi "kenapa Weyl", tapi
**"kenapa `ra_core_orbit` K=1 sensitif terhadap key berulang sama
sekali"** -- ini pertanyaan tentang perilaku `f(key)`/formula init itu
sendiri terhadap re-entry dengan key identik, bukan tentang sumber key.
Masih belum diselidiki di sesi ini (di luar cakupan yang disepakati).

## 5. Rekomendasi desain: perbesar ruang address ke 64-bit (diskusi, BELUM diimplementasikan)

**Status: laporan alasan/analisis kuantitatif saja.** Ini muncul dari
diskusi lanjutan dengan user setelah Analisis 4 di atas, BUKAN dari
eksekusi kode baru. `ra_core.c` tetap tidak disentuh; tidak ada formula
fix yang ditulis di bagian ini. Tujuannya cuma mendokumentasikan alasan
kuantitatif kenapa memperbesar input address dari 32-bit ke 64-bit adalah
arah yang secara matematis solid untuk menutup gate K=1, sebagai rujukan
kalau/ketika user memutuskan melanjutkan ke tahap desain resmi.

### 5.1 Kenapa 32-bit tidak cukup (ringkasan Analisis 4)

`ra_init_state_orbit`/`ra_init_state_singleblock` (`ra_core.c:197-207` dan
`:297-311`) menerima **satu** input 32-bit, `key` -- sesuai keputusan
desain "Orbit Addressing" yang sengaja dipilih tanpa dimensi counter
terpisah (`address = key saja`, kontras dengan `f(counter, key)` Philox --
lihat memory `project_ra_prng_terminology.md`). Karena `f(key)`
deterministik murni, ini membuat K=1 setara "satu alamat = satu output
word" dengan domain alamat cuma `2^32 ~= 4,29 miliar`.

Analisis 4 di atas membuktikan langsung: pada domain 2^32, tabrakan key
(birthday paradox, `~= N^2 / (2*2^32)`) sudah signifikan secara statistik
jauh sebelum domain itu habis -- pada N=500 juta sampel saja
(~11,6% dari domain), `~29 juta` pasangan key bertabrakan (~5,8% dari
sampel) sudah cukup memicu `agg_z=+22,06` di FPF-14+6/16 (Uji 1: dedup
total menghilangkan bias ini, z turun ke -0,28; Uji 2: pertumbuhan
`agg_z` terhadap N mengikuti pola `~N^2`, bukan linear -- lihat tabel di
atas). Ambang deteksi statistik jauh di bawah 2^32 penuh, bukan baru
muncul mendekati batas domain.

Konsekuensi kedua dari domain 32-bit yang sama (bukan soal K=1 saja):
kapasitas total addressable-tanpa-reseed untuk `singleblock`/`orbit`
K<=255 dibatasi `2^32 * 255 ~= 1,0952 * 10^12` word (~4,38 TB) -- besar,
tapi untuk pola pemakaian skala masif (mis. setiap entitas/record dalam
sistem besar diberi address unik sendiri) bisa realistis habis, bukan
cuma teoretis.

### 5.2 Proyeksi kuantitatif: kalau domain diperbesar ke 64-bit

Kalau input address diperluas jadi 64-bit efektif (dua bilangan 32-bit,
`key` + satu dimensi baru), memakai model birthday-paradox yang sudah
diverifikasi empiris di atas (`ekspektasi_tabrakan ~= N^2 / (2*D)`,
`D` = ukuran domain):

- **Ambang deteksi FPF bergeser jauh**: untuk mencapai tingkat pengulangan
  yang sama (~5,8% sampel, level yang menghasilkan `agg_z=+22,06` di
  D=2^32) di D=2^64, dibutuhkan `N ~= 2 * 0,058 * 2^64 ~= 2,14 * 10^18`
  sampel -- setara **~8,6 exabyte** output. Ini jauh di atas skala
  pengujian statistik apa pun yang pernah/akan dijalankan pada proyek ini
  (termasuk rencana PractRand 32TB yang pernah dibahas di
  `project_parallelization_research_1tb.md` -- itu cuma `3,2*10^13` byte,
  **~5 orde magnitudo** di bawah ambang 8,6 exabyte).
- **Kapasitas addressable-tanpa-reseed** naik jadi `2^64 * 255 ~=
  4,7 * 10^21` word -- praktis tak terbatas untuk skenario pemakaian
  massal yang masuk akal.

Analogi yang relevan: ini pola yang sama seperti alasan UUID128 dianggap
"aman dari tabrakan praktis" meski secara teori terbatas -- bukan karena
tabrakan mustahil, tapi karena ambang birthday-paradoxnya didorong jauh
melewati skala penggunaan nyata mana pun.

### 5.3 Dua jebakan teknis yang harus dihindari kalau arah ini dieksekusi

1. **Jangan meringkas dua input 32-bit jadi satu "effective key" 32-bit
   sebelum disuapkan ke formula lama** (mis. `key_eff =
   fmix32(key) ^ fmix32(other)` lalu pakai `ra_init_state_orbit` apa
   adanya). Ini cuma memindahkan titik tabrakan, bukan menghilangkannya --
   dua pasangan `(key, other)` yang berbeda tetap bisa menghasilkan
   `key_eff` 32-bit yang sama, dan ambang birthday-paradox-nya kembali ke
   level 2^32 seperti sekarang. Kedua bilangan 32-bit itu harus benar-benar
   mengalir penuh ke derivasi per-index `L[i]`/`M[i]`, bukan diringkas ke
   32-bit dulu di titik manapun.
2. **Cara menggabungkan dua sumber entropi itu sendiri perlu verifikasi
   avalanche baru**, bukan diasumsikan benar dari desain. Riwayat repo ini
   sudah pernah kena masalah kombinasi serupa di jalur lain: hipotesis
   "Weyl-cancellation" REFUTED tapi kandidat "MUL-combine" DITOLAK karena
   `d=0` di ronde awal membunuh kontribusi salah satu operand di bawah
   perkalian (`project_orbit_ad_mul_combine_fix.md`). Menambah dimensi
   kedua ke `keyterm_l`/`keyterm_m` perlu diverifikasi independen di tiap
   dimensi DAN gabungannya -- skala riset sebanding dengan yang sudah
   dipakai untuk menstabilkan formula `key` tunggal saat ini
   (`project_orbit_key_avalanche_diagnosis.md`, 10 axis gagal sebelum
   formula sekarang).

### 5.4 Cakupan perubahan (kalau dieksekusi)

Blast radius diperkirakan **kecil dibanding redesign penuh**: mesin
255-ronde (`ra_permutation_cycle_orbit`/`_singleblock`, wide-8-tap `o`,
finalizer XORSHIFT) TIDAK perlu diubah sama sekali -- hanya signature dan
derivasi internal `ra_init_state_orbit`/`_singleblock` yang berubah
(tambah satu parameter 32-bit, threaded penuh ke `keyterm_l`/`keyterm_m`).
Tetap ada keputusan terbuka yang belum diputuskan: nama parameter kedua
(`counter` -- menyiratkan pemakaian berurutan ala Philox, vs `salt`/
`addr_hi` -- menyiratkan bebas/tidak harus berurutan), dan spesifikasi
mixing-formula-nya sendiri.

**TIDAK ada kode/formula yang ditulis untuk poin ini.** Ini murni laporan
alasan kuantitatif untuk keputusan desain, ditulis atas permintaan
eksplisit user setelah diskusi -- eksekusi (folder eksperimen baru, spec
mixing-formula, verifikasi bertahap) menunggu konfirmasi cakupan
terpisah dari user.

## Ringkasan & rekomendasi lanjutan

1. **f(key) terbukti byte-exact, hasil di bawahnya bisa dipercaya.**
2. Analisis 1 (bijectivity): defect kecil real (~0,1%) tapi
   order-independen -- bukan penjelasan utama gap Weyl-vs-hashed.
3. Analisis 2 (FPF, direvisi oleh Analisis 4 di bawah): mekanisme konkret
   ditemukan tapi kesimpulan awal "spesifik splitmix32-chain" KELIRU --
   lihat Analisis 4.
4. Analisis 3 (korelasi Weyl): hipotesis pairwise-cancellation REFUTED
   definitif.
5. **Analisis 4 (definitif)**: defect disebabkan oleh **duplikasi key**
   dalam stream (bukan algoritma generator key manapun) -- dibuktikan
   langsung: dedup pada hashed DAN urandom sama-sama menghilangkan defect
   total (z~+22 -> z~0), dan scaling N menunjukkan pertumbuhan anomali
   yang cocok dengan pola birthday-paradox (~N^2), bukan linear. Pertanyaan
   terbuka baru: **kenapa `ra_core_orbit` K=1 sensitif terhadap key yang
   sama dipakai dua kali** (bukan lagi soal sumber key).
6. **Rekomendasi desain (§5, diskusi, belum diimplementasikan)**:
   memperbesar input address dari 32-bit ke 64-bit (dua bilangan 32-bit)
   secara matematis mendorong ambang deteksi birthday-paradox ke skala
   ~8,6 exabyte (jauh di luar skala uji statistik mana pun proyek ini) DAN
   sekaligus menaikkan plafon kapasitas addressable-tanpa-reseed dari
   `~1,0952*10^12` ke `~4,7*10^21` word. Blast radius kecil (mesin
   255-ronde tidak berubah), tapi mixing-formula gabungan-dua-dimensi
   perlu riset avalanche baru, bukan patch instan.
7. **TIDAK ada fix formula yang diusulkan/ditulis di sesi ini** (sesuai
   larangan HANDOVER, termasuk untuk §5). Kalau user ingin lanjut
   menyelidiki *kenapa* K=1 sensitif terhadap key berulang, atau mulai
   eksekusi §5, itu perlu axis/scope baru -- **tanya dulu ke user soal
   cakupan sebelum mulai** (sesuai `feedback_ask_more_before_implementing`).

## File

- `exhaustive_analysis.c` -- tool tunggal (mode `--verify`,
  `--timing-sample`, `--full-run`, `--sample`, `--fpf-order` dengan order
  `sequential|weyl|hashed|fmixhash|urandom|dedup-hashed|dedup-urandom|step:<hex>`).
- `BUILD.sh` -- build script (butuh `ra_core_nomain.o`, dibuat otomatis).
- `full_run_output.log` / `full_run_progress.log` -- hasil `--full-run`
  (Analisis 1+2-sequential+3, full 2^32).
- `fpf_hashed.log`, `fpf_hashed_seed2.log`, `fpf_weyl.log`,
  `fpf_sequential_500M.log`, `fpf_step_other.log`, `fpf_fmixhash.log` --
  hasil `--fpf-order` per ordering (500 juta key masing-masing, Analisis 2).
- `fpf_hashed_500M.log`, `fpf_urandom_500M.log`, `fpf_dedup_hashed_500M.log`,
  `fpf_dedup_urandom_500M.log` -- Uji 1 Analisis 4 (dedup).
- `fpf_scale_*.log` -- Uji 2 Analisis 4 (scaling N: 10M/50M/100M/200M/500M).
