# HANDOVER: perbesar ruang address `ra_core_orbit`/`ra_core_singleblock` ke 64-bit

**Status: BELUM DIKERJAKAN.** Dokumen ini murni scoping/diskusi desain +
rencana pengujian, ditulis atas permintaan eksplisit user. **Tidak ada
kode/formula final ditulis atau dijalankan di sini** -- `ra_core.c` sama
sekali tidak disentuh. Sesi berikutnya yang mengeksekusi bagian ini WAJIB
membaca §6 (larangan/rambu) sebelum menulis satu baris kode pun.

**Batas eksplisit dari user (jangan dilanggar)**: mesin 255-ronde
(`ra_permutation_cycle_orbit`/`ra_permutation_cycle_singleblock`, wide-
8-tap `o`, finalizer XORSHIFT(17)) **TIDAK diredesain sama sekali**.
Perubahan dibatasi ke fungsi init (`ra_init_state_orbit`/
`ra_init_state_singleblock`) yang menerima **dua** bilangan 32-bit
(`key` + satu param baru) alih-alih satu.

## 1. Kenapa ini ada

Rantai temuan hari ini (2026-09-05), semua di
`../2026-9-5_orbit-k1-exhaustive-function-analysis/RESULTS.md`:

- **Analisis 4** membuktikan langsung (dedup test + N-scaling test) bahwa
  defect `ra_core_orbit` K=1 (`FPF-14+6/16` FAIL untuk key hashed/
  urandom, CLEAN untuk Weyl) disebabkan oleh **duplikasi key** dalam satu
  stream. `f(key)` deterministik murni (`ra_init_state_orbit` cuma
  fungsi dari `key` + 2 konstanta tetap), jadi key yang berulang otomatis
  menghasilkan word output identik. Pada domain 32-bit, birthday-paradox
  membuat pengulangan itu signifikan secara statistik jauh sebelum domain
  2^32 habis: N=500 juta sampel -> `~29 juta` pasangan key bertabrakan
  (~5,8%) -> `agg_z=+22,06` di FPF-14+6/16. Dedup total menghilangkan
  bias ini (z turun ke -0,28).
- **§5 (diskusi lanjutan, laporan saja, tanpa kode)** menghitung proyeksi
  kuantitatif: memperbesar domain address dari 2^32 ke 2^64 mendorong
  ambang deteksi birthday-paradox yang sama (~5,8% collision rate) ke
  `N ~= 2,14*10^18` sampel (~8,6 exabyte) -- **~5 orde magnitudo** di atas
  skala uji manapun yang pernah dibahas proyek ini, termasuk rencana
  PractRand 32TB (`project_parallelization_research_1tb.md`, cuma
  `3,2*10^13` byte). Sekaligus menaikkan plafon kapasitas
  addressable-tanpa-reseed (`singleblock`/`orbit` K<=255) dari
  `2^32*255 ~= 1,0952*10^12` word ke `2^64*255 ~= 4,7*10^21` word --
  masalah kedua, independen tapi berakar sama, yang ditemukan user
  sendiri saat diskusi.

User memutuskan lanjut ke arah redesign terbatas ini (bukan sekadar
mendokumentasikan kontrak pemakaian "key harus unik" di README -- itu
dipertimbangkan tapi ditolak karena "meskipun pengguna pakai key dengan
hati-hati, kalau dia mengambil K=1 untuk beberapa miliar angka dia bakal
dapat output yang sama lagi").

## 2. Ruang lingkup perubahan

**Berubah:**
- Signature `ra_init_state_orbit(uint32_t *L, uint32_t *M, uint32_t key)`
  (`ra_core.c:197`) -> `ra_init_state_orbit(uint32_t *L, uint32_t *M,
  uint32_t key, uint32_t <param2>)`. Sama untuk
  `ra_init_state_singleblock(uint32_t *M, uint32_t key, size_t rng)`
  (`ra_core.c:297`) -> tambah `uint32_t <param2>`.
- Ini menjalar ke signature publik `ra_core_orbit(uint32_t key, size_t
  rng, FILE *raw_stream)` dan `ra_core_singleblock(...)` (tambah
  parameter), dan ke **semua titik panggil**: CLI driver orbit/
  singleblock, `bench_ra_core.c` (`throughput`/`reinit-sweep`),
  `multikey_stream_orbit_k1.c`/`_k255.c` dan sejenisnya, harness
  avalanche/collision-scan. Ini bukan cuma perubahan `ra_core.c` --
  daftar titik panggil yang perlu diupdate harus diinventarisasi ulang
  (`grep -rn "ra_core_orbit\|ra_core_singleblock\|ra_init_state_orbit\|ra_init_state_singleblock"`)
  sebelum eksekusi dimulai, karena scope-nya lebih luas dari yang
  RESULTS.md §5.4 sebutkan ("blast radius kecil" itu benar untuk MESIN
  255-ronde, tapi tidak otomatis benar untuk seluruh permukaan API).

**Tidak berubah:**
- `ra_permutation_cycle_orbit`/`ra_permutation_cycle_singleblock` --
  loop 255-ronde, wide-8-tap `o`, finalizer `c ^= c >> 17u`, swap
  `L[i]<->L[d]` di orbit. Nol baris berubah di sini.

**Satu titik yang WAJIB didiskusikan, bukan diasumsikan sepihak:**
`ra_core_orbit` saat ini set `cons = key;` sebelum loop ronde
(`ra_core.c:261`), dan `cons` inilah yang jadi seed `a`/`b` tiap ronde
(`a = cons` di awal `ra_permutation_cycle_orbit`, lalu `b = (cons+a)^...`
tiap iterasi). Kalau `cons` TETAP murni dari `key` (tidak menyerap
`param2`), maka `param2` cuma masuk lewat `L[]`/`M[]` init -- pengaruhnya
ke ronde-ronde awal tergantung sepenuhnya pada seberapa cepat `o` (yang
dibaca dari `M[]`) mendifusikan `param2` ke `a`/`b`/`c`. Kalau `cons`
JUGA menyerap `param2` (mis. `cons = combine(key, param2)`), pengaruhnya
langsung sejak ronde pertama, tapi ini secara teknis berarti `cons`
(dan turunannya lewat `ra_reseed` untuk `rng>255`) tidak lagi "murni
key" -- perlu dicek apakah ini mengubah semantik `ra_reseed`/`ra_hash`
untuk mode `orbit` yang `rng` besar (belum diuji, masih dead-code untuk
K kecil per `project_singleblock_kmin_defect`, tapi TIDAK dead-code untuk
`rng>255`). **Tandai sebagai keputusan desain terbuka**, jangan pilih
sepihak di sesi eksekusi.

## 3. Diskusi penamaan param kedua

Belum ada nama yang dipakai di `ra_core.c` manapun untuk dimensi ini.
Grep repo penuh: `counter`/`nonce`/`salt`/`addr_hi`/`addr_lo` tidak
pernah dipakai sebagai identifier di jalur `ra_init_state_*`/
`ra_core_*`. Dua kandidat nama, dengan trade-off yang perlu didiskusikan
user (bukan diputuskan AI):

- **`counter`** -- preseden lemah: `winner_wired_addressable.c` (baris
  ~17/51) punya komentar `// address = key only (counter intentionally
  unused)`, menyiratkan nama ini pernah dicadangkan untuk kemungkinan ini.
  Tapi nama ini menyiratkan semantik "index berurutan" ala Philox
  `f(counter, key)` -- dan "Orbit Addressing" (nama resmi mekanisme ini
  di paper, `project_ra_prng_terminology.md`) sengaja dipilih SEBAGAI
  KONTRAS dari pendekatan Philox: "address = key saja, TANPA dimensi
  counter terpisah". Memakai nama `counter` bisa membingungkan identitas
  konseptual "Orbit Addressing" vs "point addressing" Philox yang sudah
  dibekukan di paper.
- **`salt`** atau **`addr_hi`** -- menyiratkan bebas/tidak harus
  berurutan, mempertahankan gagasan "satu address = satu alamat", bukan
  "generation index". Tidak beririsan dengan istilah Philox. Tapi kalau
  pemakaian nyatanya memang berupa counter berurutan (mis. shard ID,
  timestamp), nama ini kurang deskriptif.

**Pertanyaan untuk user sebelum eksekusi**: apakah "Orbit Addressing"
sebagai identitas paper perlu tetap dijaga eksplisit berbeda dari Philox
(-> pilih `salt`/`addr_hi`), atau karena mekanismenya sekarang memang
konvergen ke gaya point-addressing Philox di K=1, penyelarasan istilah
(`counter`) sudah bisa diterima? Ini murni keputusan terminologi/branding,
tidak memengaruhi hasil statistik.

## 4. Menu kandidat formula mixing (diskusi, BUKAN kode final)

Gaya sama seperti `../2026-8-30_addressable-init-research/HANDOVER.md`
(menu "Kandidat 1-5" dengan kode draft sebagai titik-mulai diskusi, bukan
formula final). Semua kandidat di bawah untuk `ra_init_state_orbit`;
analog untuk `_singleblock`. **Belum ada yang diverifikasi avalanche --
itu langkah pertama eksekusi (§5.1), bukan bagian dari dokumen ini.**

**Kandidat A -- keyterm ganda, kombinasi multiplikatif:**
```c
uint32_t keyterm_m_key   = fmix32(GUARD_M  ^ (0x06a0dd9bu * key));
uint32_t keyterm_m_param = fmix32(GUARD_M2 ^ (0x9e3779b1u * param2)); // konstanta baru, TBD
uint32_t keyterm_m = keyterm_m_key * keyterm_m_param; // kombinasi via multiply
uint32_t r = key ^ param2 ^ (uint32_t)i;
M[i] = rot32((i * 0x06a0dd9bu) * keyterm_m, r);
```
Analog struktur dengan formula `key`-tunggal saat ini, cuma dobel
keyterm-nya. Risiko: perlu GUARD_M2 baru (popcount/jarak dari konstanta
lain, ikuti disiplin `GUARD_L`/`GUARD_M` yang sudah ada di
`ra_core.c:158-163`) supaya tidak collision dengan weak-key guard yang
sudah ada.

**Kandidat B -- pre-mix 64-bit sebelum derivasi per-index:**
```c
// treat (key, param2) sebagai satu state 64-bit, mix 2 ronde gaya splitmix64
uint64_t s = ((uint64_t)param2 << 32) | key;
s ^= s >> 33; s *= 0xff51afd7ed558ccdULL;
s ^= s >> 33; s *= 0xc4ceb9fe1a85ec53ULL;
s ^= s >> 33;
uint32_t mixed_lo = (uint32_t)s, mixed_hi = (uint32_t)(s >> 32);
uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * mixed_lo) ^ mixed_hi);
```
**SYARAT KERAS**: hasil pre-mix (`mixed_lo`/`mixed_hi`, atau `s` penuh)
harus tetap membawa PENUH 64 bit entropi ke `keyterm_m`/`M[i]` --
**JANGAN** diringkas ke satu nilai 32-bit dulu sebelum dipakai (contoh
salah: `key_eff = (uint32_t)(s ^ (s>>32))` lalu suapkan `key_eff` sendirian
ke formula lama tanpa jejak `param2` lain). Itu cuma memindahkan ambang
tabrakan kembali ke level 32-bit -- persis jebakan yang diidentifikasi di
RESULTS.md §5.3, diulang di sini sebagai guardrail keras (§6).

**Kandidat C -- `param2` cuma di rotasi, keyterm murni dari `key`:**
```c
uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key)); // TIDAK berubah dari sekarang
uint32_t r = key ^ param2 ^ (uint32_t)i; // param2 masuk di sini saja
M[i] = rot32((i * 0x06a0dd9bu) * keyterm_m, r);
```
Paling murah (hampir tidak menambah biaya komputasi per index), tapi
**paling berisiko**: `M[i]`-nya sendiri (nilai sebelum rotasi) tetap
murni fungsi `key`, cuma ARAH rotasinya yang berubah per `param2`. Perlu
diverifikasi eksplisit apakah dua `param2` berbeda dengan `key` sama
menghasilkan `M[]` yang cukup berbeda (rotasi ulang M[] yang sama, dalam
kasus terburuk, bisa jadi kombinasi linear yang lebih predictable
dibanding mixing penuh) -- **jangan diasumsikan otomatis benar**, ini
alasan avalanche 3-arah di §5.1 wajib, bukan opsional.

Pemilihan akhir antar kandidat (atau kombinasi/variasi lain yang muncul
saat diskusi) **harus lewat bake-off avalanche dulu** (§5.1) sebelum
lanjut ke PractRand -- disiplin yang sama seperti Tahap 0 riset
addressable-init 2026-08-30, dan pelajaran langsung dari
`../2026-9-4_orbit-ad-mul-combine-fix/` (kandidat MUL-combine gagal murni
di avalanche, tidak pernah sampai PractRand sama sekali).

## 5. Rencana pengujian -- battery standar

Standar yang diminta user, dipetakan ke infrastruktur yang **sudah ada**
di repo (jangan tulis ulang dari nol):

### 5.1 Avalanche effect
Reuse `../2026-9-4_orbit-kmin-battery/avalanche_orbit_kmin.py` (orbit
K=1/K=255) + `../2026-9-1_production-candidate-battery/avalanche_ra_core_singleblock_k1.py`
dan `avalanche_orbit_singleblock_k255.py` (singleblock). Gate yang sudah
dipakai proyek ini: mean Hamming-fraction di [0,3, 0,7], per-bit floor
>=0,2 (referensi hasil PASS terakhir: orbit K=1 mean 0,513672/min-bit
0,250000; K=255 mean 0,498828/min-bit 0,481005).

**Perluasan wajib untuk kandidat 2-param**: jalankan avalanche **3 arah**
per kandidat mixing:
1. Flip 1 bit `key`, `param2` tetap.
2. Flip 1 bit `param2`, `key` tetap.
3. Flip 1 bit gabungan (`key` DAN `param2` sekaligus, gaya avalanche
   64-bit penuh).

Kandidat yang lolos arah (1) tapi gagal arah (2) -- ini persis risiko
Kandidat C di §4 -- harus GUGUR di sini, sebelum sampai ke collision-scan
atau PractRand manapun.

### 5.2 Korelasi / collision-scan singleblock K=1 & K=255
Reuse `../2026-9-4_orbit-kmin-battery/collision_scan_orbit_k1.py` dan
`../2026-9-1_production-candidate-battery/collision_scan_singleblock_k1.py`
(model `digest_collision_prob`/Poisson-z, tier `smoke`/`full`), diadaptasi
untuk sampel pasangan `(key, param2)` langsung (bukan `key` saja) di
domain 64-bit.

**Uji akseptansi utama** (bukan cuma regression check): re-run skenario
"address berulang" dari Analisis 4 (`--fpf-order`-style hashed/urandom,
lihat `../2026-9-5_orbit-k1-exhaustive-function-analysis/exhaustive_analysis.c`
sebagai referensi metodologi, bukan dipakai langsung karena itu spesifik
K=1 32-bit) pada domain 64-bit penuh, N besar (mis. 500 juta - 1 miliar
pasangan `(key,param2)`, comparable dengan skala yang bikin FAIL di
32-bit) -- ini validasi EMPIRIS bahwa ambang birthday-paradox memang
bergeser sesuai proyeksi §5.2 RESULTS.md lama, bukan cuma diasumsikan
dari matematika teoretis.

### 5.3 `orbit --stream` (continuous single-stream)
Regression check PractRand gaya `-tlmin 8GB -tlmax 1TB` (konvensi
`../2026-8-29_parallelization-research/RESULTS.md`), pastikan gate lama
(128GB/1TB clean, lihat `project_parallelization_research_1tb.md`) TIDAK
regresi akibat perubahan init. Ini murni regression, bukan discovery baru
-- kalau FAIL di sini, itu bug baru yang harus diperbaiki sebelum lanjut,
bukan trade-off yang diterima.

### 5.4 Kecepatan
Reuse `../2026-9-1_family-productionization/bench_ra_core.c` (mode
`throughput` dan `reinit-sweep`), format tabel seperti
`../2026-9-1_production-candidate-battery/bench_ra_core_results.txt`.
Laporkan **5 angka standar** proyek ini
(`feedback_orbit_singleblock_benchmark_standard`): orbit --multistream
K=1, orbit --multistream K=255, orbit --stream, singleblock K=1,
singleblock K=255 -- untuk kandidat mixing terbaik (hasil §5.1-5.2),
dibandingkan terhadap baseline `ra_core.c` saat ini. **Kuantifikasi biaya
riil** (param tambahan + mixing lebih berat kemungkinan menambah beberapa
ns/word di jalur init) -- jangan asumsikan negligible tanpa diukur.

### 5.5 Kualitas keacakan penuh (dieharder + PractRand)
Reuse `../2026-9-4_orbit-kmin-battery/run_dieharder_battery.py` (27 test
"Good"-reliability, `BASE_KEY=111222` standar repo) + PractRand staged
(`RNG_test stdin32 -tlmax 16GB -multithreaded`, checkpoint
64MB->256MB->...->16GB, driver mengikuti pola
`multikey_stream_orbit_k1.c`/`_k255.c` + `BUILD.sh` yang sudah ada,
diadaptasi untuk signature 2-param) untuk K=1 dan K=255.

**Wajib dua ordering address**, ini acceptance criterion utama untuk
menutup gate:
- **Weyl-stepped** (regression check) -- harus tetap CLEAN, sama seperti
  status quo.
- **Hashed/urandom** (acceptance test utama) -- axis inilah yang FAIL di
  32-bit; gate ini baru boleh dianggap CLOSED kalau axis ini CLEAN di
  64-bit pada skala yang sepadan/lebih besar dari skala yang dulu bikin
  FAIL di 32-bit (N=500 juta di FPF sintetis, atau minimal 8-16GB di
  PractRand asli, mengikuti skala historis
  `../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`).

## 6. Larangan / rambu

1. **`ra_core.c` kanonik TIDAK diedit langsung.** Kandidat dikembangkan
   di file/candidate terpisah dulu (folder eksperimen baru di bawah
   `experiments/`), sama seperti pola promosi `w8_f10_i0`/Kandidat-5
   sebelumnya. Promosi ke `ra_core.c` hanya setelah SELURUH battery §5
   PASS dan user mengonfirmasi.
2. **Jangan meringkas `(key, param2)` jadi satu nilai 32-bit** sebelum
   masuk formula per-index derivation (`keyterm_l`/`keyterm_m`/`L[i]`/
   `M[i]`) -- ini guardrail KERAS, bukan cuma catatan desain, karena
   pelanggaran ini secara diam-diam mengembalikan defect yang sedang
   diperbaiki (lihat Kandidat B di §4).
3. **Urutan gate wajib**: avalanche 3-arah (§5.1) dulu, baru lanjut ke
   collision-scan (§5.2), baru PractRand (§5.3/5.5). Jangan lompat tahap
   -- preseden `../2026-9-4_orbit-ad-mul-combine-fix/` menunjukkan
   kandidat bisa gugur murni di avalanche tanpa perlu buang waktu di
   PractRand.
4. **Tidak retry-seed kalau PractRand FAIL** (`feedback_no_reseed_on_fail`)
   -- FAIL di satu seed = kandidat gagal, titik; jangan coba seed lain
   untuk "menjelaskan" FAIL itu.
5. **Checkpoint ke user setelah avalanche bake-off** (§5.1, menentukan
   kandidat mana yang lanjut) SEBELUM menjalankan battery besar §5.2-5.5
   (`feedback_ask_more_on_design_decisions`) -- jangan jalankan seluruh
   battery untuk semua kandidat sekaligus tanpa penyaringan awal.
6. **Penamaan param kedua (§3) dan pilihan kandidat mixing final (§4)
   HARUS dikonfirmasi user dulu**, tidak diputuskan sepihak oleh sesi
   eksekusi. Kalau ada opsi baru yang muncul saat diskusi/eksperimen di
   luar A/B/C di atas, perlakukan sama -- tanya dulu sebelum ditulis ke
   `ra_core.c` (`feedback_ask_more_before_implementing`).
7. **Inventarisasi ulang seluruh titik panggil** (§2) sebelum mulai edit
   -- jangan asumsikan "cuma `ra_core.c` yang berubah" dari klaim blast-
   radius di RESULTS.md §5.4 (klaim itu soal mesin 255-ronde, bukan soal
   seluruh permukaan API).
8. Jalankan `/graphify --update` di akhir SETIAP sesi (baik sesi
   penulisan HANDOVER ini maupun sesi eksekusi berikutnya) sesuai
   `CLAUDE.md` project ini.

## 7. Referensi

- `../2026-9-5_orbit-k1-exhaustive-function-analysis/RESULTS.md` --
  Analisis 4 (mekanisme duplikasi key) dan §5 (proyeksi kuantitatif
  64-bit, jebakan §5.3, blast-radius §5.4).
- `../2026-9-1_family-productionization/PRODUCTION_READINESS_HANDOVER.md`
  -- status gate resmi (orbit CLOSED Weyl-only, TERBUKA untuk
  hashed-key K=1).
- `../2026-8-30_addressable-init-research/HANDOVER.md` -- preseden format
  "menu Kandidat 1-5" + seksi Terminologi ("Orbit Addressing" vs
  "point addressing" Philox).
- Memory: `project_ra_prng_terminology.md`, `project_orbit_key_avalanche_diagnosis.md`,
  `project_orbit_ad_mul_combine_fix.md`, `project_singleblock_kmin_defect.md`,
  `feedback_orbit_singleblock_benchmark_standard.md`,
  `feedback_ask_more_before_implementing.md`,
  `feedback_ask_more_on_design_decisions.md`, `feedback_no_reseed_on_fail.md`.
- Skrip battery (semua sudah diverifikasi ada di repo per tanggal
  dokumen ini): `avalanche_orbit_kmin.py`, `avalanche_ra_core_singleblock_k1.py`,
  `avalanche_orbit_singleblock_k255.py`, `collision_scan_orbit_k1.py`,
  `collision_scan_singleblock_k1.py`, `multikey_stream_orbit_k1.c`,
  `multikey_stream_orbit_k255.c`, `bench_ra_core.c`,
  `run_dieharder_battery.py` -- lokasi persis di §5 masing-masing.
