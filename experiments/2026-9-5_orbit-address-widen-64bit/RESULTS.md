# RESULTS: Perbesar ruang address ra_core_orbit/singleblock ke 64-bit

Status: **Kandidat D dipilih sebagai pemenang final** (tie-break
efisiensi GPU atas Kandidat E, keduanya lolos semua gate statistik
secara identik — lihat §9), TAPI **BELUM dipromosikan ke `ra_core.c`
kanonik** (promosi jadi task terpisah, lihat §9).

## 0. Ringkasan eksekutif

Masalah asli: `ra_core_orbit`/`ra_core_singleblock` K=1 memakai `key`
32-bit untuk re-init; kalau `key` bisa berulang (hashed-chain,
`/dev/urandom` -- bukan permutasi repeat-free seperti Weyl), birthday
paradox pada domain 2^32 membuat pengulangan itu signifikan secara
statistik jauh sebelum 2^32 draw habis (root cause asli, lihat
`../2026-9-5_orbit-k1-exhaustive-function-analysis/RESULTS.md` Analisis
4: N=500 juta -> `agg_z=+22.06`).

Solusi yang disetujui: perbesar input fungsi init dari satu `key` 32-bit
menjadi dua bilangan 32-bit (`key` + `addr_hi`), TANPA meredesain mesin
255-ronde (`ra_permutation_cycle_orbit`/`_singleblock` disalin verbatim
di semua kandidat). `cons` tetap murni `key` (keputusan user 2026-09-05).

**5 kandidat dicoba, 3 gugur, 2 lolos semua gate:**

| Kandidat | Deskripsi | Hasil |
|---|---|---|
| C | addr_hi hanya di rotasi (`r=key^addr_hi^i`), keyterm murni key | **GUGUR** -- avalanche addr_hi_only FAIL (bit 5 mati total, min-bit=0.0) |
| A | dual keyterm, kombinasi multiplikatif (key dan addr_hi masing2 di-hash lalu dikali), rotasi `r=key^addr_hi^i` | **GUGUR** -- PractRand staged K=1+Weyl FAIL (Gap-16, 4GB) |
| B | pre-mix 64-bit gaya splitmix64 sebelum derivasi per-index, rotasi `r=key^addr_hi^i` | **GUGUR** -- PractRand staged K=1+Weyl FAIL (Gap-16, 8GB) |
| **D** | = A, TAPI rotasi diperbaiki jadi `r=key^i` (addr_hi dihapus dari rotasi) | **LOLOS SEMUA GATE — PEMENANG FINAL** |
| E | = B, TAPI rotasi diperbaiki jadi `r=key^i` (addr_hi dihapus dari rotasi) | LOLOS SEMUA GATE — ditolak karena tie-break efisiensi GPU (§9), bukan cacat statistik |

Root cause kegagalan A/B (lihat §3): rumus rotasi bersama
`r = key ^ addr_hi ^ i` meng-XOR **dua** deret Weyl-stepped independen
(delta dari keluarga konstanta golden-ratio yang saling berkerabat)
langsung ke residu mod-32 (satu-satunya bit yang dipakai `rot32`).
Kandidat D/E mengembalikan rotasi ke `key ^ i` (persis formula lama,
battle-tested puluhan run PractRand sebelumnya) dan memindahkan **seluruh**
kontribusi `addr_hi` ke sisi keyterm saja.

## 1. Tahap 1 -- Implementasi & verifikasi kompilasi

Semua kandidat: turunan penuh `ra_core.c` kanonik
(`../2026-9-1_family-productionization/ra_core.c`), hanya
`ra_init_state_orbit`/`ra_init_state_singleblock` + signature
`ra_core_orbit`/`ra_core_singleblock` yang berubah (+`addr_hi`).
`ra_permutation_cycle_*` disalin VERBATIM di semua kandidat (0 baris
berubah). File di `candidates/`: `candidate_{a,b,c,d,e}.c`.

Compile bersih (`gcc -O3 -march=native -std=gnu17 -Wall -Wextra`) untuk
kelima kandidat. Byte-verify `addr_hi=0` vs `ra_core.c` asli: hanya
Kandidat C byte-identik (desain "rotasi-saja" membuat itu mungkin);
A/B/D/E TIDAK byte-identik pada `addr_hi=0` (keyterm mereka selalu
mengalikan/mem-pre-mix, bukan pass-through) -- trade-off yang melekat
pada desainnya, sudah didokumentasikan di header masing-masing file.

## 2. Tahap 2 -- Avalanche 3-arah (checkpoint wajib)

Gate: mean Hamming-fraction dalam [0.3, 0.7], per-bit floor >= 0.2. 3
arah: key_only, addr_hi_only, combined (bit sama dibalik di KEDUA key
DAN addr_hi sekaligus -- blind-spot spesifik untuk desain berbasis XOR).

**Kandidat C -- GUGUR di sini:**
arah `addr_hi_only`: bit 5 mati total (`min_bit_fraction=0.0`), gagal
floor 0.2. Root cause: `keyterm` Kandidat C tidak pernah menyerap
`addr_hi` (murni dari `key`), jadi satu-satunya jalur difusi `addr_hi`
adalah lewat rotasi -- tidak cukup untuk semua posisi bit.

**Kandidat A dan B -- PASSED semua arah, semua mode/K** (mean di
kisaran 0.48-0.52, min_bit_fraction 0.22-0.49, jauh di atas floor).

**Kandidat D dan E -- diulang PASCA root-cause-fix, PASSED semua arah,
semua mode/K** (`avalanche_3way_de_results.log`):

| kandidat | mode | K | arah | mean | min_bit | PASS |
|---|---|---|---|---|---|---|
| D | orbit | 1 | key_only | 0.5029 | 0.3438 | True |
| D | orbit | 1 | addr_hi_only | 0.4971 | 0.2812 | True |
| D | orbit | 1 | combined | 0.5010 | 0.2812 | True |
| D | orbit | 255 | key_only | 0.4994 | 0.4841 | True |
| D | orbit | 255 | addr_hi_only | 0.5007 | 0.4904 | True |
| D | orbit | 255 | combined | 0.5023 | 0.4892 | True |
| D | singleblock | 1/255 | (sama seperti orbit) | | | True |
| E | orbit | 1 | key_only | 0.4834 | 0.3750 | True |
| E | orbit | 1 | addr_hi_only | 0.4922 | 0.2188 | True |
| E | orbit | 1 | combined | 0.5156 | 0.3750 | True |
| E | orbit | 255 | key_only | 0.5004 | 0.4907 | True |
| E | orbit | 255 | addr_hi_only | 0.4996 | 0.4882 | True |
| E | orbit | 255 | combined | 0.5015 | 0.4904 | True |
| E | singleblock | 1/255 | (sama seperti orbit) | | | True |

Kesimpulan penting: menghapus `addr_hi` dari rotasi TIDAK merusak
avalanche -- jalur keyterm saja (kombinasi multiplikatif A, pre-mix
splitmix64 B) sudah cukup kuat berdiri sendiri tanpa bantuan rotasi.
Ini yang membenarkan hipotesis fix D/E.

## 3. Root cause analisis (A & B gugur, sebelum D/E dirancang)

**Kandidat A FAIL** -- PractRand staged, K=1, ordering Weyl, 4GB
(`practrand_staged_candidate_a_K1_weyl.log`):
```
512MB: Gap-16:A   R=+5.1   unusual
2GB:   Gap-16:A   R=+7.3   mildly suspicious
       Gap-16:B   R=+5.9   mildly suspicious
4GB:   Gap-16:A   R=+13.3  VERY SUSPICIOUS
       Gap-16:B   R=+19.7  FAIL !
       FPF-14+6/16:(11,14-3) R=+6.8  unusual
```

**Kandidat B FAIL** -- pola identik, K=1, ordering Weyl, 8GB
(`practrand_staged_candidate_b_K1_weyl.log`):
```
4GB: Gap-16:A  R=+5.5   unusual
     Gap-16:B  R=+7.6   suspicious
8GB: Gap-16:A  R=+10.7  very suspicious
     Gap-16:B  R=+18.5  FAIL !
     FPF-14+6/16:(8,14-0)  R=+18.6  FAIL !
     FPF-14+6/16:(9,14-1)  R=+11.3  very suspicious
     FPF-14+6/16:all       R=+8.0   very suspicious
```

Dua formula keyterm yang TOTAL BERBEDA (A: kombinasi multiplikatif dua
hash terpisah; B: pre-mix splitmix64 penuh 64-bit) gagal dengan cara
identik: test yang sama (Gap-16), axis yang sama (K=1+Weyl), tren R naik
konsisten tiap level (bukan noise). Elemen yang dibagi rata: rumus
rotasi `r = key ^ addr_hi ^ i` -- satu-satunya bagian formula yang
IDENTIK di A dan B.

Hipotesis akar masalah: `rot32` hanya memakai `r & 31` (5 bit rendah).
Di harness Weyl-stepped (`multikey_stream_addr_hi.c`), `key` dan
`addr_hi` SAMA-SAMA di-Weyl-step tiap re-init dengan delta independen
(`WEYL_DELTA_KEY=0x9E3779B9`, `WEYL_DELTA_ADDR=0x85EBCA6B` -- dua-duanya
dari keluarga konstanta golden-ratio/Murmur yang berkerabat dekat).
Sebelum widening, rotasi cuma bergantung `key ^ i` (satu deret Weyl) dan
itu battle-tested bersih selama proyek ini. Meng-XOR DUA deret Weyl
sekaligus ke residu mod-32 diduga menciptakan resonansi bit-rendah yang
lolos avalanche (rata-rata bit tetap terdifusi baik) tapi terdeteksi
Gap-16 di skala besar (test khusus sensitif terhadap periodisitas
gap-length).

Dieharder (27 test) TIDAK menangkap defect ini sama sekali untuk A
maupun B (26/27 PASSED, 0 FAILED) -- konsisten dengan sejarah proyek:
Gap-16/FPF-14+6/16 di PractRand jauh lebih sensitif untuk kelas defect
ini daripada dieharder.

**Fix**: Kandidat D (=A) dan E (=B) mengembalikan rotasi ke `r = key ^ i`
persis, memindahkan 100% kontribusi `addr_hi` ke sisi keyterm.

## 4. Tahap 3 -- FPF collision-scan (uji akseptansi utama)

Reproduksi persis statistik FPF-14+6/16 yang menemukan defect asli,
dijalankan terhadap D dan E sungguhan (link objek nyata, N=500 juta per
ordering). Threshold proyek: `|agg_z| < 6.0`.

| Kandidat | Ordering | agg_z | Verdict |
|---|---|---|---|
| D | weyl | +1.257 | CLEAN |
| D | hashed | -1.129 | CLEAN |
| D | urandom | +0.298 | CLEAN |
| E | weyl | +0.570 | CLEAN |
| E | hashed | +0.266 | CLEAN |
| E | urandom | -0.324 | CLEAN |

Semua 6 run CLEAN, jauh di bawah threshold. Catatan: run `E-weyl`
mencatat satu baris "ANOMALI" di bucket ekor sangat langka
(`e=36, observed=1, expected=0.01, z=+11.638`) -- wajar secara statistik
untuk kategori dengan expected count sangat kecil di ~1 miliar sampel;
`agg_z` keseluruhan tetap CLEAN.

Sebagai kontrol positif (verifikasi harness benar): reproduksi
Kandidat C dengan `addr_hi=0` (K=1, hashed, N=500 juta) menghasilkan
`agg_z=+22.062`, cocok persis dengan nilai historis Analisis 4 --
mengonfirmasi harness mendeteksi defect asli dengan benar sebelum dipakai
menguji D/E.

## 5. Tahap 4 -- Regresi PractRand `orbit --stream` (tanpa reinit, 8GB-1TB)

Axis berbeda dari staged (single init call, stream kontinu 300 miliar
kata, `-tlmin 8GB -tlmax 1TB -multithreaded`).

| Kandidat | Level tertinggi | Waktu | Anomali |
|---|---|---|---|
| A | 128GB | 1948s | 0 (dihentikan setelah B gugul, tidak lanjut) |
| B | 64GB | 937s | 0 (dihentikan) |
| **D** | **1 TB (2^40 byte)** | **14404s** | **0 -- "no anomalies" di SETIAP level (64GB,128GB,256GB,512GB,1TB)** |
| **E** | **1 TB (2^40 byte)** | **14440s** | **0 -- sama, bersih total** |

D dan E CLEAN sampai 1TB penuh, tidak ada satupun anomali di semua level
intermediate. Ini axis yang paling murah untuk mendeteksi defect
struktural umum pada mesin 255-ronde -- hasil bersih total menegaskan
`ra_permutation_cycle_*` yang disalin verbatim memang tidak tersentuh
regresi apapun dari perubahan init.

## 6. Tahap 6 -- PractRand staged (64MB-16GB) + dieharder (27 test), 8 kombinasi

Kombinasi: {D, E} x {K=1, K=255} x {weyl (regresi), hashed (akseptansi
utama)}. `multikey_stream_addr_hi.c`, `run_dieharder_battery_addr_hi.py`.

### PractRand staged -- SEMUA 8 KOMBINASI CLEAN (`ALL_PRACTRAND_STAGED_DE_DONE`)

| Kombinasi | Level | Waktu | Anomali di 16GB |
|---|---|---|---|
| D K1 weyl | 16GB | 2931s | tidak ada (titik yang FAIL di Kandidat A pada 4GB, D bersih total) |
| D K1 hashed | 16GB | 2904s | tidak ada |
| D K255 weyl | 16GB | 280s | tidak ada |
| D K255 hashed | 16GB | 280s | tidak ada |
| E K1 weyl | 16GB | 2885s | tidak ada (titik yang FAIL di Kandidat B pada 8GB, E bersih total) |
| E K1 hashed | 16GB | 2874s | tidak ada |
| E K255 weyl | 16GB | 263s | tidak ada |
| E K255 hashed | 16GB | 265s | 1x "unusual" terisolasi (`DC6-9x1Bytes-1`, R=+5.8, p=3.7e-3) -- tidak berulang, bukan tren, wajar |

Titik kritis yang menjatuhkan A (4GB) dan B (8GB) di axis K=1+Weyl
dikonfirmasi bersih tanpa jejak anomali untuk D maupun E sampai 16GB
penuh.

### Dieharder -- SEMUA 8 KOMBINASI, 0 FAILED (`ALL_DIEHARDER_DE_DONE`)

| Kombinasi | PASSED | WEAK | FAILED |
|---|---|---|---|
| D K1 weyl | 26/27 | 1 | 0 |
| D K1 hashed | 26/27 | 1 | 0 |
| D K255 weyl | 27/27 | 0 | 0 |
| D K255 hashed | 26/27 | 1 | 0 |
| E K1 weyl | 25/27 | 2 | 0 |
| E K1 hashed | 25/27 | 2 | 0 |
| E K255 weyl | 27/27 | 0 | 0 |
| E K255 hashed | 27/27 | 0 | 0 |

0 FAILED di semua 8 kombinasi. Sub-hasil WEAK (1-2 per kombinasi K=1)
adalah noise statistik normal dieharder (p-value ekstrem tunggal di
antara puluhan ribu sub-test, tidak reproduksibel/tidak sistemik per
dokumentasi dieharder sendiri) -- bukan kriteria gate proyek ini
(kriteria gate = 0 FAILED, sudah terpenuhi).

## 7. Tahap 5 -- Kecepatan (5 angka standar)

Baseline: `ra_core.c` kanonik (`bench_ra_core`, `-O3 -march=native`).
D dan E diukur ulang di sesi ini dengan flag compile YANG SAMA PERSIS
(`bench_addr_hi.c` + `candidate_{d,e}_nomain.o` di `-O3 -march=native`
-- catatan: percobaan pertama salah pakai `-O2` tanpa `-march=native`
untuk objek kandidat, menghasilkan angka 4x lebih lambat yang MENYESATKAN;
sudah dikoreksi dan diulang dengan flag yang konsisten).

| Metrik | Baseline (ra_core.c) | Kandidat D | Kandidat E |
|---|---|---|---|
| orbit `--stream` (throughput, ns/word) | 2.777 | 2.804 (+1.0%) | 2.783 (+0.2%) |
| orbit `--multistream` K=1 (ns/word steadystate) | 55.935 | 52.894 (-5.4%) | 56.410 (+0.9%) |
| orbit `--multistream` K=255 (ns/word steadystate) | 2.793 | 2.900 (+3.8%) | 2.916 (+4.4%) |
| singleblock K=1 (ns/word steadystate) | 10.924 | 9.720 (-11.0%) | 10.598 (-3.0%) |
| singleblock K=255 (ns/word steadystate) | 0.437 | 0.155 | 0.164 |

Overhead D/E vs baseline pada 4 metrik pertama berada dalam kisaran
+/-11% (sebagian malah lebih cepat) -- jauh lebih kecil dari overhead
historis A/B (+3.8% s/d +16.9%, diukur sebelum sesi ini dengan formula
rotasi lama). Ini masuk akal: D/E menghapus satu operasi XOR dari loop
per-index (256 iterasi), jadi kalau ada beda, seharusnya SEDIKIT lebih
murah, bukan lebih mahal -- cocok dengan hasil di atas.

Metrik singleblock K=255 (0.155/0.164 ns/word) TIDAK dianggap
representatif -- angka sekecil ini di bawah 1 siklus CPU per kata,
konsisten dengan sinyal noise pengukuran/potensi dead-code-elimination
compiler pada beban kerja yang sangat kecil per-cycle, sudah pernah
ditandai sebagai perilaku "quirky" untuk metrik ini di sesi-sesi
sebelumnya (single-trial, bukan rata-rata banyak trial). Tidak
mempengaruhi kesimpulan gate manapun (kecepatan bukan gate PASS/FAIL,
hanya informasi).

## 8. Ringkasan gate akhir

| Tahap | Gate | D | E |
|---|---|---|---|
| 1 | Compile bersih | PASS | PASS |
| 2 | Avalanche 3-arah (checkpoint) | PASS (semua arah/mode/K) | PASS (semua arah/mode/K) |
| 3 | FPF collision-scan (uji akseptansi utama) | PASS (6/6 CLEAN) | PASS (6/6 CLEAN) |
| 4 | Regresi PractRand `--stream` 1TB | PASS (0 anomali) | PASS (0 anomali) |
| 6a | PractRand staged 16GB, 4 kombinasi | PASS (0 anomali signifikan) | PASS (0 anomali signifikan) |
| 6b | Dieharder 27 test, 4 kombinasi | PASS (0 FAILED) | PASS (0 FAILED) |
| 5 | Kecepatan (informasi, bukan gate) | overhead kecil (-11% s/d +3.8%) | overhead kecil (-3.0% s/d +4.4%) |

**Kandidat D dan E berdua LOLOS SEMUA GATE tanpa satu FAIL pun.**

## 9. Keputusan final: D vs E, promosi, dan arsip

### 9.1 Kandidat D dipilih -- tie-break efisiensi GPU

Karena §8 menunjukkan D dan E lolos SEMUA gate PASS/FAIL secara identik
(tidak ada pembeda dari sisi korektnes statistik), keputusan final
dijatuhkan berdasarkan pertimbangan di luar gate proyek ini: efisiensi
untuk deployment di GPU.

**Fakta kode** (dikonfirmasi baca penuh `candidates/candidate_d.c` dan
`candidates/candidate_e.c`):
- **Kandidat D** -- `ra_init_state_orbit`/`ra_init_state_singleblock`
  HANYA memakai perkalian `uint32_t * uint32_t` (mis.
  `0x9e3779b7u * key`, `0x85ebca77u * addr_hi`,
  `keyterm_l_key * keyterm_l_param`, dan per-index
  `(i * const) * keyterm`). **Nol operasi 64-bit di seluruh file.**
- **Kandidat E** -- melakukan pre-mix gaya splitmix64 sebelum
  menurunkan keyterm: `uint64_t s = ((uint64_t)addr_hi<<32)|key;
  s *= 0xff51afd7ed558ccdULL; ... s *= 0xc4ceb9fe1a85ec53ULL;` --
  **2 perkalian 64x64->64 sungguhan per panggilan init**, di kedua
  varian orbit dan singleblock.

**Alasan teknis**: banyak arsitektur GPU dirancang dengan ALU native
32-bit; perkalian integer 64-bit sering tidak punya unit hardware
native dan diemulasi lewat beberapa instruksi 32-bit berantai, atau
kalaupun didukung native throughput-nya jauh lebih rendah dari 32-bit.
Ini concern nyata untuk portabilitas PRNG ini ke target GPU/SIMT.

**Keputusan** (dikonfirmasi user 2026-09-06): **Kandidat D dipilih
sebagai pemenang final.** Kandidat E TIDAK gugur gate apapun --
didokumentasikan di sini sebagai alternatif yang tetap valid secara
statistik, ditolak murni karena pertimbangan portabilitas GPU, bukan
karena cacat.

### 9.2 Promosi ke `ra_core.c` -- ditunda, task terpisah

Sesi ini **TIDAK mempromosikan** Kandidat D ke `ra_core.c` kanonik.
Promosi berarti mengganti signature publik `ra_core_orbit`/
`ra_core_singleblock` (+`addr_hi`), yang berdampak ke >=13 file
pemanggil teridentifikasi di eksplorasi awal:

- `bench_ra_core.c`, `bench_release.c`
- `multikey_stream.c` / `_hashed.c` / `_orbit.c`
- `ra_core_singleblock_cli.c`, `multikey_stream_singleblock_k1.c`
- `scrambler_ra_core_singleblock.c`, `multikey_stream_k.c`
- `orbit_bench.c`, `multikey_stream_orbit_postreseed_k1.c` / `_k255.c`
- `urandom_key_stream_orbit.c`
- `exhaustive_analysis.c` (via `--verify`; catatan: `f_key()`-nya
  me-reimplementasi formula lama secara manual, TIDAK akan gagal
  compile saat signature berubah -- risiko silent-staleness kalau
  dipakai lagi tanpa re-derive ke formula D)

plus `tools/battery/driver_main_template.c` (kontrak signature generik
dipakai `avalanche.py`/`practrand_gate.py`/`run_battery.py`) dan
`tools/battery/README.md`.

**Keputusan** (2026-09-06): promosi dikerjakan sebagai task/sesi
terpisah, dengan approval eksplisit tersendiri -- bukan bagian sesi
finalisasi ini.

### 9.3 Kandidat A/B/C -- disimpan sebagai arsip

**Keputusan** (2026-09-06): `candidate_a.c`/`candidate_b.c`/
`candidate_c.c` disimpan apa adanya di `candidates/` sebagai arsip
riset gagal (komentar penjelasan root-cause sudah ada di masing-masing
header), bukan dihapus.
