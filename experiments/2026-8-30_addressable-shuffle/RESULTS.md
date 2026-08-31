# Addressable-Init Data Shuffling — RESULTS

## Apa ini

Turunan dari `experiments/2025-10-5_scramble-design/scrambler.c`/`.py`
("ZepFold" token-shuffling CLI), dengan **satu** perubahan: inisialisasi
state `(L, M)` diganti dari formula tetap yang diparameterisasi
`--multiplier_m`/`--multiplier_l` (tidak bergantung pada seed sama sekali)
menjadi `ra_init_state_addressable(L, M, key)` — formula "Orbit Addressing"
yang dibekukan dan divalidasi penuh di
`../2026-8-30_addressable-init-research/` (Tahap 0-5, selesai 2026-08-30).

Shuffle loop inti (recurrence `a/b/o/c/d`, Lemire's fast reduction untuk
index swap, block-reseed 255-elemen via `ra_hash`) **tidak diubah** — itu
murni application logic milik shuffle, ortogonal dari pertanyaan
"bagaimana state awal dibentuk dari key" yang jadi topik riset
addressable-init.

File:
- `scrambler_addressable.c` — CLI utama (`--input`, `--output`, `--key`).
- `scrambler_addressable.py` — port Python referensi (untuk parity check).
- `verify_parity.py` — bit-identity check C vs Python.
- `scc_test.py` — validasi kualitas statistik + spot-check multi-key.
- `tokens.txt` — sample data (100.000 token, salinan dari
  `../2025-10-5_scramble-design/tokens.txt`).

## Build & run

```
gcc -std=c11 -O3 -o scrambler_addressable scrambler_addressable.c
./scrambler_addressable --input tokens.txt --output shuffled.txt --key 1
```

Catatan build: `alignas(64)` butuh `#include <stdalign.h>` — header ini
**hilang** di `scrambler.c` lama (baru terlihat saat `-std=c11` tanpa
`-include stdalign.h`); ditambahkan di sini agar `gcc -std=c11 -O3` polos
bisa compile bersih.

## Hasil validasi

### 1. Smoke test
`./scrambler_addressable --input tokens.txt --output shuffled.txt --key 1`
→ 100.000 token in, 100.000 token out, multiset token identik (hanya urutan
berubah). PASS.

### 2. Parity check (`verify_parity.py`)
63 kombinasi (9 ukuran input: 0, 1, 2, 254, 255, 256, 511, 1000, 12345 ×
7 key: 0, 1, 42, 12345, 0x7FFFFFFF, 0xFFFFFFFF, 0xDEADBEEF) — **63/63 PASS**,
`scrambler_addressable.c` bit-identik dengan `scrambler_addressable.py`.

### 3. Statistik kualitas shuffle (`scc_test.py`, key=1, n=100.000)

| Metrik | Baru (addressable-init) | Lama (`2025-10-5_scramble-design/scrambled.txt`, seed=1) |
|---|---|---|
| Entropy (bit) | 16.609640 | 16.609640 |
| Chi-Square | 0.000000 | 0.000000 |
| Runs Test z | 0.1100 (runs=66681, exp=66666.33) | -0.6850 (runs=66575, exp=66666.33) |
| Serial Correlation | 0.001644 | -0.003633 |

Entropy/Chi-Square identik karena kedua run memakai multiset token yang
sama (hanya urutan berbeda — metrik ini tidak sensitif terhadap
permutasi). Runs-test `z` dan serial-correlation sama-sama kecil/tidak
signifikan (`|z| < 2`, `|r| ≈ 0`) di kedua versi — **tidak ada regresi
kualitas statistik**.

### 4. Multi-key distinctness spot-check (`scc_test.py`)
200 key acak diterapkan ke 2.000 token pertama dari `tokens.txt` →
**0 collision permutasi**. Ini bukan pengganti collision-scan skala riset
(Tahap 3 addressable-init sudah menguji 500.000 key acak pada level state
init) — sekadar sanity check ringan bahwa sifat addressable ikut terbawa
ke konteks shuffle.

## Benchmark kecepatan

Sudah dilakukan (2026-08-30) — lihat `SPEED_BENCHMARK.md` untuk detail
lengkap (metodologi `perf stat -r 30`, tabel dua run, script
`speed_bench/run_speed_benchmark.sh`).

Ringkasan: selisih `scrambler_addressable` vs `ra_prng2` lama **berbalik arah
antar dua run pengukuran** (run 1: baru −7.8%; run 2: lama −8.6%) — dalam
rentang noise, jadi **`ra_init_state_addressable()` tidak menambah/mengurangi
biaya kecepatan yang terukur**.

Yang signifikan: seluruh keluarga shuffle "ZepFold" ~2.2x lebih lambat dari
xoshiro256\*\*/PCG32/Philox4x32 — awalnya diduga karena shuffle loop ZepFold
melakukan swap array dua kali per token (data + state internal `L[]`), tapi
dugaan ini **diuji langsung dan terbukti salah**: `stream_driven_shuffle.c`
(CLI baru, pakai `ra_core` stream generator murni + Fisher-Yates satu-swap
seperti xoshiro256.c) tetap sama lambatnya (~0.12s), bukan mendekati
xoshiro/pcg (~0.05s). Dekomposisi `perf stat` membuktikan **~85% dari total
waktu adalah I/O teks** (`fscanf`/`fprintf` per token di
`load_tokens_from_file`/`save_tokens_to_file`, dipakai semua varian ZepFold)
— generate stream murni cuma 0.011s untuk ~1 juta angka. `xoshiro256.c` dkk.
pakai `fread()` sekali + tokenizer pointer manual, jauh lebih cepat dari
`fscanf`/`fprintf` per-token. Detail lengkap di bagian "Analisis Lanjutan"
`SPEED_BENCHMARK.md`. Ini bukan regresi baru (pola sama sudah ada di
`benchmarks/comparisons/scrambling_speed/benchmark_perf.txt` lama), tapi
penyebabnya ternyata implementasi I/O, bukan algoritma PRNG/shuffle-nya.

## Follow-up: SELESAI (2026-08-30, sesi lanjutan)

Kedua follow-up di bawah ini sudah dikerjakan dan diverifikasi. Sub-bagian
sebelumnya (naming + I/O, tercatat "belum dilakukan") dipertahankan sebagai
riwayat, ditandai selesai:

**Naming — DONE.** Fungsi shuffle `ra_core` di `scrambler_addressable.c`/
`.py` di-rename jadi **`ra_shuffle`** (definisi + semua pemanggilan,
termasuk import di `verify_parity.py`/`scc_test.py`), supaya tidak lagi
bentrok konsep dengan `ra_core` stream-generator di
`stream_driven_shuffle.c`/`winner_wired_addressable.c`/
`src/ra_prng2/c/ra_prng2.c` (signature beda, fungsi beda — nama itu sendiri
sengaja dipertahankan di file-file itu karena mereka byte-for-byte copy dari
sumbernya). `verify_parity.py` tetap 63/63 PASS dan `scc_test.py` tetap PASS
setelah rename ini.

**I/O — DONE, hipotesis terbukti benar arahnya (gap berkurang ~separuh,
tidak tertutup penuh).** `load_tokens_from_file`/`save_tokens_to_file` di
`scrambler_addressable.c`, `stream_driven_shuffle.c`, dan
`speed_bench/io_only_isolation_test.c` diganti dari `fscanf`/`fprintf`
per-token ke `fread()` sekali + tokenizer pointer manual (`strtol`) + satu
`fwrite()` untuk output, meniru `xoshiro256.c`. Representasi token tetap
`uint32_t` (bukan raw-string passthrough) supaya format output identik.
Validasi: `verify_parity.py` 63/63 PASS, `scc_test.py` PASS (statistik +
0/200 collision), dan diff byte-per-byte terhadap `shuffled.txt`/
`stream_shuffled.txt` lama (`tokens.txt` sama) — **0 beda**.

Hasil benchmark ulang (`perf stat -r 30`, detail lengkap di
`SPEED_BENCHMARK.md` bagian "Follow-up: optimasi I/O terverifikasi"):
`scrambler_addressable` **0.1255s → 0.0771s (-38.5%)**, rasio ke `pcg`
tercepat turun dari 2.53x → **1.62x**; I/O murni terisolasi turun
**0.0937s → 0.0584s (-37.7%)**. `stream_driven_shuffle` **0.1204s → 0.0893s
(-25.9%, variance run ini tinggi ±8.27%)**. `ra_prng2` (kontrol, read-only,
I/O sengaja tidak disentuh) tetap ~0.116-0.118s di kedua run — membuktikan
penurunan besar `scrambler_addressable` benar berasal dari perubahan I/O,
bukan drift mesin/sesi.

**Catatan penting**: gap ke xoshiro/pcg **berkurang tapi tidak tertutup
penuh** — `scrambler_addressable` masih ~1.6x lebih lambat setelah I/O
dibenahi (dari ~2.5x sebelumnya). Sisa gap kemungkinan berasal dari struktur
shuffle loop ZepFold sendiri (reseed `ra_hash` tiap 255 elemen, 8-wide read
`M[]` per token, dua swap array) — belum diselidiki lebih lanjut, di luar
scope follow-up ini.

## Follow-up: varian cycle `winner_wired_v2` + addressable init (2026-08-31)

**Motivasi**: `scrambler_addressable.c` di atas sengaja mempertahankan
recurrence `a/b/o/c/d` ORIGINAL (tidak diubah dari `scrambler.c` lama) —
hanya init `(L, M)` yang diganti addressable. Belum pernah ada varian yang
menggabungkan addressable-init DENGAN cycle `winner_wired_v2` (pruned+wired,
~6% instruksi lebih sedikit dari `winner_wired.c`, kandidat tervalidasi dari
`../2026-8-27_operand-position-search/`) dalam shuffle-loop shape ZepFold
yang sama (`stream_driven_shuffle.c` DI file ini memang pakai cycle
`winner_wired_v2`, tapi dengan shuffle-loop shape yang beda -- single-swap
Fisher-Yates, dibuat untuk tujuan lain, lihat `HANDOVER.md`). File baru:
`scrambler_wired_addressable.c`/`.py`.

**Perubahan**: HANYA komputasi `a/b/o/c/d` di dalam loop 255-langkah yang
diganti, byte-for-byte dari `winner_wired_addressable.c`'s
`ra_permutation_cycle()`:
```c
o = (M[(i+6)&0xFF] << 6) ^ (M[(i+7)&0xFF] << 7);
a = (d ^ o) ^ (cons + a);
b = (cons + a) ^ (o + d);
c = rot32((a >> 13) ^ a, b);
d = c & 0xFF;   // menggantikan d = (c*(i+1))>>32 punya scrambler_addressable.c
```
Semuanya yang lain (init `ra_init_state_addressable`, `ra_hash`, block-reseed
255-elemen, Lemire reduction untuk index swap token, dua-swap
`scrambled_tokens[]`/`L[]`, I/O `fread`+`strtol`/`fwrite`) identik dengan
`scrambler_addressable.c`.

**Validasi**:
- Parity C vs Python (`verify_parity_wired.py`): **63/63 PASS** bit-identik
  (matrix key/size sama seperti `verify_parity.py`).
- Statistik (`scc_test_wired.py`, `tokens.txt` 100.000 token, key=1):
  entropy 16.609640, chi2 0.0, runs-test z=-0.97, SCC=0.000911 — setara
  `scrambler_addressable` (16.6096/0.0/~0.1-0.16/~0.0006-0.0016), tidak ada
  regresi kualitas.
- Multi-key distinctness spot-check: 0/200 collision.
- Sanity: output shuffle untuk key sama **beda** dari `scrambler_addressable`
  (dikonfirmasi via diff) — bukti bahwa cycle-nya memang berbeda, bukan
  copy-paste yang salah.

**Speed** (lihat `SPEED_BENCHMARK.md`, subseksi "Follow-up: cycle
`winner_wired_v2` + addressable init"): dalam sesi pengukuran yang sama
(`perf stat -r 30`), `scrambler_wired_addressable` **~4.9% lebih cepat**
dari `scrambler_addressable` (0.0705s vs 0.0742s) — rasio ke `pcg` turun
dari 1.386x ke **1.318x**. Arah konsisten dengan klaim ~6% instruksi lebih
sedikit dari riset operand-position-search, tapi perbaikannya modest, TIDAK
menutup mayoritas gap ke `pcg`/xoshiro. Ini melengkapi (bukan membantah)
interpretasi "biaya struktural inheren" di `SPEED_BENCHMARK.md`: pruning+
wiring recurrence-nya sendiri memang mengurangi biaya (~5%), sesuai prediksi,
tapi porsi terbesar gap tetap ada di luar recurrence murni (I/O, reseed,
Lemire reduction, dua-swap array).
