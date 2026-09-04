# Handover: uji BCFN pasca-batas-reseed untuk `ra_reseed`/`ra_hash`

**Status folder ini: BELUM DIKERJAKAN — dirancang siap-eksekusi, ditulis
supaya sesi lain (atau user) bisa langsung jalan tanpa perlu merancang
ulang dari nol.** Tidak ada kode/hasil di folder ini sama sekali sampai
tulisan ini.

## 1. Kenapa test ini perlu (ringkasan, baca sumber untuk detail lengkap)

`PRODUCTION_READINESS_HANDOVER.md` (`../2026-9-1_family-productionization/`)
mencatat satu item terbuka: **`ra_reseed`/`ra_hash` orbit BCFN pra-reseed**.
Setelah ditelusuri (2026-09-04, sesi diskusi dengan user), ini **bukan
defect yang sudah terbukti gagal** — statusnya "belum pernah diuji dengan
metodologi yang tepat", bukan "sudah terbukti rusak". Jangan mulai sesi ini
dengan asumsi ada bug; mulai dengan asumsi netral dan biarkan datanya
bicara.

Latar belakang singkat (baca sumber untuk rantai diagnosis lengkap):

- 2026-09-01, `../2026-9-1_multikey-remix-search/HANDOVER.md`: PractRand
  menemukan BCFN FAIL nyata (escalating, `R=+40.7` di 4GB) pada pola
  "banyak key beda-beda, tiap key hasilkan blok pendek ≤255 kata,
  disambung" — root cause terdiagnosis lewat rantai eliminasi yang disiplin
  (setiap kandidat penyebab diuji dan disingkirkan satu per satu, BUKAN
  ditebak): korelasi terkonsentrasi di **siklus PERTAMA, sebelum
  `ra_reseed` pernah dipanggil**, karena `M[]`/`L[]` adalah fungsi affine
  (linear) langsung dari `key`. Bukti kunci: menyisipkan satu siklus
  255-langkah penuh yang HASILNYA DIBUANG plus satu `ra_reseed` sebelum
  output pertama → bersih sampai 8GB, tanpa ubah formula apa pun. Uji
  128GB single-key lama (`winner_wired_addressable.c`) tidak pernah
  menangkap ini karena didominasi milyaran siklus PASCA-reseed —
  "buta secara statistik" terhadap pola ini.
- Fix diterapkan dua bagian: formula init (multiply-combine keyterm,
  2026-09-01) + transform cycle (wide 8-tap `o` + finalizer XORSHIFT(17),
  `w8_f10_i0`, singleblock 2026-09-03 lalu orbit 2026-09-04). Keduanya
  sudah tervalidasi bersih (dieharder+PractRand 16GB+avalanche+
  collision-scan) untuk pola **pra-reseed** — lihat
  `../2026-9-4_orbit-kmin-battery/RESULTS.md`.
- **Yang BELUM pernah diuji**: apakah `ra_reseed`/`ra_hash` — mekanisme
  yang menghasilkan `cons`/state BARU setiap kali `ra_core_orbit` melewati
  batas 255 kata — punya kelemahan SERUPA (bocor struktur) di siklus
  PERTAMA SETELAH satu reseed? Semua validasi yang ada sejauh ini:
  - Pola multistream K≤255 di atas (yang sudah bersih) **tidak pernah
    memicu `ra_reseed` sama sekali** — `ra_core_orbit`'s iterasi = `rng/255
    + 1` = 1 untuk `rng≤255`, loop cuma jalan sekali, return sebelum
    `ra_reseed` terpanggil (lihat `ra_core.c` baris ~254-270).
  - Uji 128GB single-key teragregasi milyaran reseed — sama seperti
    alasan defect pra-reseed asli tidak tertangkap di sana, uji ini
    kemungkinan juga "buta" terhadap bocor-satu-siklus-tepat-setelah-satu-
    reseed.
  - `../2026-9-2_singleblock-prereseed-experiment/RESULTS.md` mencoba
    pre-mix `M[]` pakai `ra_hash` SEBELUM siklus pertama — **gagal total**
    menutup defect. Tapi itu menyasar defect cycle-structural (yang sudah
    diperbaiki lewat `w8_f10_i0`), BUKAN menguji `ra_reseed`/`ra_hash` itu
    sendiri. Kegagalan itu tidak membuktikan apa pun soal mekanisme
    reseed — jangan jadikan itu alasan untuk skip test ini.

## 2. Desain test — analog 1-ke-1 dari test yang menemukan defect asli

Ide inti: test asli mengisolasi "siklus pertama SEBELUM reseed manapun,
untuk banyak key independen". Test ini menggeser jendela isolasi tepat
satu siklus ke kanan: "siklus pertama SETELAH satu reseed, untuk banyak
key independen".

### 2.1 Cara memaksa tepat satu reseed lalu mengisolasi hasilnya

Panggil `ra_core_orbit(key, rng=255+K, raw_stream)` untuk banyak key
berbeda. 255 kata pertama = siklus pra-reseed (formula yang SUDAH terbukti
bersih pasca-fix — lihat §1), sisanya `K` kata berasal dari siklus
pasca-reseed pertama (`ra_reseed` terpanggil tepat sekali di antaranya,
lihat `ra_core.c`'s `ra_core_orbit`: loop `it=0` jalan 255 langkah lalu
panggil `ra_reseed(M, L)` sebelum `it=1`). **Buang 255 kata pertama, simpan
HANYA `K` kata sisanya** ke stream yang diumpankan ke PractRand — ini
mengisolasi persis siklus pasca-reseed-pertama itu, tanpa campur sinyal
dari siklus pra-reseed yang sudah diketahui bersih.

### 2.2 Kode yang harus dibuat

**Jangan verbatim-copy fungsi `static` apa pun dari `ra_core.c`** (beda
dari `bench_release.c`'s kebutuhan isolasi init) — cukup extern-link ke
`ra_core_orbit` (API publik), sama seperti driver-driver multikey yang
sudah ada. Link terhadap **`ra_core.c` KANONIK**
(`../2026-9-1_family-productionization/ra_core.c`) — BUKAN `ra_core_v2.c`
(itu sudah dipromosikan/usang, lihat
`../2026-9-4_orbit-fix-and-wideo-rolling-optimization/HANDOVER.md`) —
karena fix `w8_f10_i0` + rolling-`o` sudah ada di kanonik sejak
2026-09-04.

Tulis dua file baru di folder ini, mencontoh persis pola
`../2026-9-4_orbit-kmin-battery/multikey_stream_orbit_k1.c`
(baca file itu dulu sebagai starting point literal — struktur CLI-nya,
`--stream <base_key> <n>`, mau dipertahankan sama):

- `multikey_stream_orbit_postreseed_k1.c` — untuk tiap output word yang
  diminta (`n` total), panggil `ra_core_orbit(key, 255+1, tmp_buffer_atau_tmpfile)`
  dengan key baru (Weyl-stepped, `base_key + i*0x9E3779B9u`, konvensi yang
  sama persis dengan semua driver multikey lain di repo ini), lalu tulis
  KE STDOUT hanya word ke-256 (index 255, 0-based) dari 256 word yang
  dihasilkan panggilan itu. Cara termudah: panggil dengan `raw_stream=NULL`
  tidak bisa dipakai (butuh akses ke word individual) — pakai
  `fmemopen()` buffer `256*sizeof(uint32_t) + 1` byte (**PENTING**: +1
  byte, bukan pas — lihat catatan bug glibc `fmemopen` di §4 di bawah,
  ini WAJIB dihindari ulang), lalu `fseek`/baca offset `255*4` untuk
  ambil word terakhir, tulis 4 byte itu ke `stdout` via `fwrite`.
- `multikey_stream_orbit_postreseed_k255.c` — sama, tapi tiap key
  memanggil `ra_core_orbit(key, 255+255, tmp_buffer)` (510 kata per
  panggilan) dan menulis 255 kata TERAKHIR (offset `255*4` sampai akhir
  buffer) ke stdout, bukan cuma satu word. Buffer `510*sizeof(uint32_t)+1`
  byte.

**Kontrol/pembanding wajib dalam kode yang sama atau file terpisah kecil**:
sertakan juga mode yang menjalankan pola pra-reseed LAMA (persis
`multikey_stream_orbit_k1.c`/`_k255.c` yang sudah ada di
`../2026-9-4_orbit-kmin-battery/`, `rng=K` langsung tanpa buang apa pun) —
supaya kontrol sanity di §2.3 bisa dijalankan dari binary yang sama,
linked terhadap `ra_core.c` kanonik yang sama, di sesi yang sama, tanpa
harus rebuild folder lain.

**Dua urutan key** (Weyl-linear DAN SplitMix32-hashed) — diagnosis 2026-09-01
membuktikan keduanya gagal identik pada defect asli, jadi keduanya WAJIB
dicoba lagi di sini untuk parity metodologi (jangan cuma satu, itu bisa
menyembunyikan pola yang cuma muncul di salah satu urutan). Contoh SplitMix32
hashed-key generator ada di `../2026-9-1_multikey-remix-search/`'s
`diag_hashed_init.c` atau `../2026-9-1_family-productionization/multikey_stream_hashed.c`
— salin pola pembangkitan key-nya, bukan seluruh filenya.

Compile (pola sama seperti `bench_ra_core.c`/`bench_release.c`):
```bash
gcc -O3 -march=native -std=gnu17 -include stdalign.h \
    -Dmain=ra_core_unused_main -c ../2026-9-1_family-productionization/ra_core.c \
    -o ra_core_nomain.o -Wall -Wextra
gcc -O3 -march=native -std=gnu17 -include stdalign.h \
    multikey_stream_orbit_postreseed_k1.c ra_core_nomain.o \
    -o multikey_stream_orbit_postreseed_k1 -Wall -Wextra
# ulangi untuk _k255 dan varian hashed-key
```

### 2.3 Kontrol sanity — WAJIB dijalankan SEBELUM percaya hasil apa pun

Jalankan ulang pola pra-reseed yang SUDAH diketahui bersih (mode kontrol
di §2.2, atau langsung reuse binary `multikey_stream_orbit_k1`/`_k255` di
`../2026-9-4_orbit-kmin-battery/` kalau masih ter-compile) sampai skala
kecil (mis. 1-2GB cukup untuk kontrol, tidak perlu ulang 16GB penuh) di
mesin/sesi yang sama. **Kalau kontrol ini sendiri gagal atau menunjukkan
anomali, harness barunya yang salah — jangan lanjut ke test pasca-reseed
sampai kontrol ini bersih.** Ini bukan formalitas: kalau ada bug di
`fmemopen`/offset/buffer-size pada kode baru §2.2, kontrol yang seharusnya
bersih akan menunjukkan itu duluan, sebelum hasil pasca-reseed
disalahartikan.

### 2.4 Skala dan command PractRand

Sama persis dengan yang dipakai di seluruh battery K-small-defect
sebelumnya (`../2026-9-4_orbit-kmin-battery/RESULTS.md`, konsisten dengan
gate singleblock 2026-09-03):

```bash
./multikey_stream_orbit_postreseed_k1 --stream <base_key> <n> | \
    RNG_test stdin32 -tlmin 256MB -tlmax 8GB -multithreaded > \
    practrand_orbit_postreseed_K1.log
```

(ganti `8GB` jadi `16GB` kalau K=1 tetap bersih di 8GB dan mau
menyamakan bar penuh dengan gate lain — lihat kriteria PASS di §3).

Ulangi untuk `_k255`, dan untuk masing-masing varian urutan key (Weyl
linear + SplitMix32-hashed) — total 4 run minimum
(2 K-value × 2 urutan key).

## 3. Kriteria PASS/FAIL

- **PASS**: bersih (tidak ada anomali) sampai minimal **8GB** di SEMUA 4
  run (K=1 dan K=255, kedua urutan key). Defect asli sudah "very
  suspicious" di 2GB dan FAIL jelas di 4GB (severity naik tajam, bukan
  noise satu titik) — kalau pola serupa ada di sini, seharusnya sudah
  kelihatan di rentang yang sama. Kalau ke-4 run bersih ≥8GB, tutup item
  ini di `PRODUCTION_READINESS_HANDOVER.md` §2b/§6 sebagai **resolved**,
  update memory `project_singleblock_kmin_defect`/status produksi, tidak
  ada aksi lanjutan.
- **FAIL**: escalating anomaly dengan pola mirip BCFN (severity naik
  tajam antar-checkpoint, bukan satu p-value nyasar). Kalau ini terjadi:
  1. **JANGAN retry seed/base_key lain** untuk "menjelaskan" — FAIL di
     satu seed sudah final (`feedback_no_reseed_on_fail`, memory repo
     ini).
  2. **JANGAN asumsikan fix `w8_f10_i0` otomatis berlaku di sini** —
     mekanisme yang diuji (`ra_reseed`+`ra_hash`, XOR-fold 256→8 kata,
     `HASH_DEPTH=32`, `hash_access=sequential`) beda total dari
     `ra_permutation_cycle_orbit` yang diperbaiki `w8_f10_i0`. Kalau
     memang gagal, root cause HARUS didiagnosis ulang dari nol dengan
     rantai eliminasi sedisiplin `../2026-9-1_multikey-remix-search/HANDOVER.md`
     (uji urutan key, uji apakah `ra_hash`'s `HASH_DEPTH`/access pattern
     penyebabnya, uji apakah `M[i]^=L[i]` di awal `ra_reseed` penyebabnya,
     dst — satu per satu, bukan tebak-fix-langsung-terap).

## 4. Peringatan teknis yang wajib diwarisi (jangan mengulang bug lama)

- **Bug `fmemopen` glibc** (`../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md`):
  kalau buffer PAS ukuran data yang ditulis, NUL-terminator glibc menimpa
  byte terakhir (byte teratas word terakhir ter-nol-kan tiap panggilan,
  little-endian). Buffer WAJIB `+1` byte dari ukuran data. Ini pernah
  bikin hasil test kelihatan katastropik padahal RNG-nya baik-baik saja —
  jangan sampai mispresentasikan hasil test pasca-reseed karena bug
  harness yang sama.
- **Benchmark 5-angka standar** (`feedback_orbit_singleblock_benchmark_standard`):
  tidak relevan langsung untuk test statistik ini, tapi prinsipnya sama —
  jangan cuma uji K=1, selalu K=1 DAN K=255.

## 5. Stretch goal opsional (TIDAK wajib untuk menutup item ini)

Kalau §3 PASS di reseed pertama, pertanyaan lanjutan yang belum terjawab:
apakah efeknya (kalau nanti ternyata ada di skenario lain) menguat/hilang
di reseed KEDUA/KETIGA berturut-turut? Uji dengan menggeser jendela buang
(`rng=510+K`, buang 510 kata, ambil siklus setelah reseed KEDUA). Tidak
perlu dikerjakan sebelum menutup item utama — catat sebagai follow-up
independen kalau ada waktu/keingintahuan lebih.

## 6. Referensi cepat

- Root cause defect asli: `../2026-9-1_multikey-remix-search/HANDOVER.md`
  (rantai eliminasi lengkap), `../2026-9-1_family-productionization/RESULTS_TAHAP2.md`
  baris 111-140.
- Fix yang sudah diterapkan: `../2026-9-1_family-productionization/ra_core.c`
  header comment (provenance lengkap tiap fix), `../2026-9-4_orbit-kmin-battery/RESULTS.md`
  (validasi pra-reseed K=1/K=255 PASS).
- Percobaan yang TIDAK menjawab pertanyaan ini (jangan diulang tanpa
  modifikasi): `../2026-9-2_singleblock-prereseed-experiment/RESULTS.md`.
- Pola driver/CLI untuk dicontoh: `../2026-9-4_orbit-kmin-battery/multikey_stream_orbit_k1.c`,
  `multikey_stream_orbit_k255.c`, `BUILD.sh`.
- Status/tempat mencatat hasil setelah selesai:
  `../2026-9-1_family-productionization/PRODUCTION_READINESS_HANDOVER.md`
  §2b dan §6 (`ra_reseed`/`ra_hash` orbit BCFN pra-reseed — item ini).
