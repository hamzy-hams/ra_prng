# Handover: Addressable-Init Data Shuffling & Speed Investigation

Ditulis untuk sesi Claude Code baru yang melanjutkan topik ini (dipicu frasa
"lanjutkan shuffle addressable" / "kenapa ra_prng lambat buat shuffle" atau
serupa). **Status: investigasi ini SELESAI, termasuk kedua follow-up di §6**
(2026-08-30) — implementasi shuffle CLI, validasi statistik, benchmark
kecepatan, root-cause analysis, optimasi I/O, dan rename `ra_core`→
`ra_shuffle` semuanya tuntas dan terdokumentasi. Kalau tujuan sesi baru
adalah "optimasi lebih lanjut" (sisa gap ~1.6x ke xoshiro/pcg setelah I/O
dibenahi, lihat §6), itu scope baru — baca §6 dulu untuk konteks lengkapnya.

## 1. Ringkasan alur & tujuan

1. **Trigger awal**: user minta implementasi "pengacakan data" (shuffle) buat
   PRNG hasil riset paralelisasi terbaru (`ra_init_state_addressable`,
   "Orbit Addressing", dari `../2026-8-30_addressable-init-research/`,
   Tahap 0-5 selesai), meniru pola shuffle CLI lama ("ZepFold":
   `experiments/2025-10-5_scramble-design/scrambler.c` /
   `benchmarks/comparisons/source/ra_prng2.c`).
2. **Diperluas**: user tanya apakah sudah dibandingkan kecepatannya dengan
   algoritma lain (xoshiro256\*\*, PCG32, Philox4x32, ChaCha20) → dibangun
   `speed_bench/` harness, hasil: shuffle ZepFold (dgn/tanpa addressable-init)
   ~2.2x lebih lambat dari kelompok itu.
3. **Diperluas lagi**: user berhipotesis penyebabnya "swap array dobel" (ZepFold
   nge-swap 2 array per token: data + state internal `L[]`, sementara
   xoshiro/pcg cuma 1). Diuji langsung lewat CLI baru yang HANYA pakai 1
   swap → **hipotesis terbukti salah**. Root cause sebenarnya: implementasi
   I/O (`fscanf`/`fprintf` per token), bukan algoritma PRNG/shuffle sama
   sekali. Detail lengkap di §3.

## 2. File & perannya (peta folder `experiments/2026-8-30_addressable-shuffle/`)

| File | Peran |
|---|---|
| `scrambler_addressable.c` / `.py` | Shuffle CLI utama — turunan `scrambler.c` lama, init `(L,M)` diganti `ra_init_state_addressable(key)`. Shuffle loop (recurrence `a/b/o/c/d` + Lemire reduction + block-reseed `ra_hash`, DUA swap/token: `scrambled_tokens` & `L`) **tidak diubah** dari `scrambler.c`. |
| `verify_parity.py` | Bit-identity check C vs Python — 63/63 PASS (9 ukuran × 7 key). |
| `scc_test.py` | Validasi statistik (entropy/chi2/runs/SCC) + spot-check 200-key distinctness (0 collision). |
| `tokens.txt` | Sample data, 100.000 token, salinan `../2025-10-5_scramble-design/tokens.txt`. |
| `RESULTS.md` | Laporan lengkap implementasi + validasi + ringkasan speed & root-cause (revisi terakhir mencerminkan §3 di bawah). |
| `SPEED_BENCHMARK.md` | Laporan lengkap benchmark kecepatan + bagian "Analisis Lanjutan" (root-cause investigation, tabel dekomposisi I/O vs generate vs swap). **Baca ini duluan** kalau mau angka persisnya. |
| `stream_driven_shuffle.c` | CLI eksperimen: `ra_core` (stream generator, byte-for-byte copy dari `winner_wired_addressable.c`) + Fisher-Yates **satu-swap** (persis pola `xoshiro256.c`). Dibuat khusus buat menguji/membantah hipotesis "swap dobel". |
| `scrambler_wired_addressable.c` / `.py`, `verify_parity_wired.py`, `scc_test_wired.py` | Follow-up (2026-08-31): sama seperti `scrambler_addressable.c` (shuffle-loop shape ZepFold dua-swap + addressable init), TAPI recurrence `a/b/o/c/d`-nya diganti `winner_wired_v2` (pruned+wired) -- kombinasi yang belum pernah diuji sebelumnya. Lihat §8. |
| `speed_bench/` | Semua binary hasil compile (build artifact, **tidak** ditaruh di `benchmarks/comparisons/source/`), `tokens.txt` (1.000.000 token, salinan `benchmarks/comparisons/source/tokens.txt`), `run_speed_benchmark.sh` (otomasi `perf stat -r 30` semua 11 binary), `speed_benchmark_perf.log` (log mentah run terakhir), `io_only_isolation_test.c` (diagnostic: `stream_driven_shuffle.c` dengan panggilan shuffle **dimatikan**, dipakai isolasi biaya I/O murni). |

## 3. Hasil kunci (sudah final, jangan diulang)

**Implementasi & validasi** (lihat `RESULTS.md`):
- Parity C vs Python: **63/63 PASS** bit-identik.
- Statistik shuffle: entropy 16.6096 / chi2 0.0 / runs-test z≈0.1-0.16 / SCC≈0.0006-0.0016 — setara baseline lama, tidak ada regresi.
- 0 collision permutasi di antara 200 key acak (spot-check, bukan riset collision-scan skala penuh — itu sudah dilakukan di level state-init pada Tahap 3 addressable-init research, 500.000 key).

**Speed benchmark** (lihat `SPEED_BENCHMARK.md`, tabel lengkap dua run):
- `scrambler_addressable` vs `ra_prng2` lama: **selisihnya berbalik arah antar run** (run 1: baru −7.8%; run 2: lama −8.6%), keduanya dalam rentang noise. **Tidak ada bukti `ra_init_state_addressable()` menambah/mengurangi biaya kecepatan.**
- Seluruh keluarga shuffle ZepFold (`ra_prng2`, `scrambler_addressable`) ~2.2x lebih lambat dari xoshiro256\*\*/PCG32/Philox4x32 (bukan regresi baru — pola sama sudah ada di `benchmarks/comparisons/scrambling_speed/benchmark_perf.txt` lama).

**Root-cause investigation** (lihat SPEED_BENCHMARK.md § "Analisis Lanjutan" — **ini bagian paling penting untuk sesi lanjutan**):
- Dugaan "swap dobel" **dibantah**: `stream_driven_shuffle` (1 swap/token, sumber entropi `ra_core` yang sama) tetap ~0.11-0.12s, TIDAK mendekati xoshiro/pcg (~0.05s).
- Dekomposisi `perf stat -r 10` (1.000.000 token/angka):
  | Komponen | Elapsed (s) |
  |---|---:|
  | I/O murni (load `fscanf` + save `fprintf`, shuffle dimatikan) | **0.0937** |
  | Generate stream murni (`ra_core --stream`, tanpa file I/O) | **0.0113** |
  | Total `stream_driven_shuffle` | 0.1098 |
- **Kesimpulan: ~85% waktu eksekusi adalah I/O teks**, bukan generator PRNG
  (cuma 0.011s untuk ~1 juta angka) atau jumlah swap. `load_tokens_from_file`/
  `save_tokens_to_file` (dipakai semua varian ZepFold: `scrambler_addressable.c`,
  `ra_prng2.c`, `stream_driven_shuffle.c`) manggil `fscanf(fp, "%ld", &value)`/
  `fprintf(fp, "%u", ...)` **satu kali per token**. `xoshiro256.c` dkk pakai
  `fread()` sekali + tokenizer pointer manual + `fwrite()` mentah — lihat
  `benchmarks/comparisons/source/xoshiro256.c` baris 53-103.

## 4. Reproduksi cepat

```bash
cd experiments/2026-8-30_addressable-shuffle
gcc -std=c11 -O3 -o scrambler_addressable scrambler_addressable.c
./scrambler_addressable --input tokens.txt --output shuffled.txt --key 1
python3 verify_parity.py
python3 scc_test.py

cd speed_bench
./run_speed_benchmark.sh                                   # semua 11 binary, perf stat -r 30
perf stat -r 10 -- ./io_only_isolation_test --input tokens.txt --output /tmp/x.txt --key 1
perf stat -r 10 -- ./winner_wired_addressable --stream 1 999999 > /dev/null
```

## 5. Constraint yang diwarisi

- **Read-only**: `benchmarks/comparisons/source/*.c` (termasuk `ra_prng2.c`)
  tidak boleh diubah in-place — semua binary pembanding di-compile ke
  `speed_bench/` (build artifact terpisah), bukan di folder sumber itu.
- `benchmarks/comparisons/source/ra_prng2.c` punya bug lama: komentar header
  `/* ... */` tidak pernah ditutup, jadi seluruh blok `#include` di bawahnya
  ikut "termakan" komentar. Workaround compile (tanpa ubah file):
  `gcc -std=c11 -O3 -include stdio.h -include stdlib.h -include stdint.h
  -include string.h -include time.h -include stdalign.h -o ra_prng2 ra_prng2.c`.
- `winner_wired_addressable.c`/`scrambler_addressable.c` pakai `alignas(64)`
  tapi lupa `#include <stdalign.h>` di kode aslinya (sudah ditambahkan di
  `scrambler_addressable.c`/`stream_driven_shuffle.c` versi folder ini).
- Semua artefak addressable-init (termasuk shuffle turunannya) tetap di
  `experiments/`, bukan `benchmarks/`, karena PRNG addressable-init sendiri
  belum dipromosikan ke `src/` (lihat memori/keputusan sesi sebelumnya).

## 6. Follow-up: SELESAI (2026-08-30, sesi lanjutan)

Kedua item di bawah ini sudah dikerjakan dan diverifikasi. Detail lengkap di
`RESULTS.md` ("Follow-up: SELESAI") dan `SPEED_BENCHMARK.md` ("Follow-up:
optimasi I/O terverifikasi").

1. **Optimasi I/O — DONE.** `load_tokens_from_file`/`save_tokens_to_file` di
   `scrambler_addressable.c`, `stream_driven_shuffle.c`, dan
   `speed_bench/io_only_isolation_test.c` diganti ke pola `fread()` sekali +
   tokenizer pointer manual (`strtol`) + satu `fwrite()` untuk output, meniru
   `xoshiro256.c`. Hipotesis "I/O adalah bottleneck dominan" **terbukti benar
   arahnya, tapi tidak menutup gap sepenuhnya**: `scrambler_addressable`
   0.1255s → 0.0771s (-38.5%), rasio ke `pcg` turun dari 2.53x → **1.62x**
   (bukan 1.0x — sisa gap kemungkinan dari struktur shuffle loop ZepFold
   sendiri, reseed `ra_hash`/8-wide `M[]` read/dua swap array, belum
   diselidiki). I/O murni terisolasi turun 0.0937s → 0.0584s (-37.7%).
   Divalidasi: `verify_parity.py` tetap 63/63 PASS, `scc_test.py` tetap PASS,
   diff byte-per-byte terhadap output lama = 0 beda.
2. **Naming — DONE.** `ra_core` (fungsi shuffle in-place di
   `scrambler_addressable.c`/`.py`) di-rename jadi **`ra_shuffle`** — tidak
   lagi bentrok konsep dengan `ra_core` stream-generator di
   `stream_driven_shuffle.c`/`winner_wired_addressable.c`/
   `src/ra_prng2/c/ra_prng2.c` (fungsi itu sengaja TIDAK direname, karena
   byte-for-byte copy dari sumbernya masing-masing).

Kalau ingin melanjutkan lebih jauh (sisa gap ~1.6x ke xoshiro/pcg): itu scope
baru, bukan follow-up yang tersisa dari sesi ini.

## 7. Follow-up: cycle `winner_wired_v2` + addressable init — SELESAI (2026-08-31)

User (2026-08-31) mengoreksi asumsi: riset speed di atas ("gap ~1.6x adalah
biaya struktural recurrence inheren") diukur dari `scrambler_addressable`,
yang recurrence-nya **sengaja tetap ORIGINAL** (§2 tabel di atas) — bukan
`winner_wired_v2` (pruned+wired), padahal `winner_wired_v2` itulah kandidat
yang dimaksud dipakai riset-riset lain untuk aplikasi nyata. `stream_driven_shuffle.c`
memang pakai `winner_wired_v2`, tapi shuffle-loop shape-nya beda
(single-swap, dibuat buat tujuan lain — lihat tabel §2), jadi bukan
perbandingan apple-to-apple terhadap `scrambler_addressable`.

**Dikerjakan**: `scrambler_wired_addressable.c`/`.py` — identik dengan
`scrambler_addressable.c` KECUALI recurrence `a/b/o/c/d`-nya diganti
`winner_wired_v2` (byte-for-byte dari `winner_wired_addressable.c`). Detail
perubahan, validasi (parity 63/63 PASS, statistik sehat, 0/200 collision),
dan angka speed lengkap di `RESULTS.md`/`SPEED_BENCHMARK.md` bagian
"Follow-up: cycle `winner_wired_v2` + addressable init".

**Hasil kunci**: `scrambler_wired_addressable` ~4.9% lebih cepat dari
`scrambler_addressable` dalam sesi pengukuran yang sama (rasio ke `pcg`:
1.318x vs 1.386x). Recurrence pruning **membantu, terukur, arah sesuai
prediksi (~6% instruksi)** — tapi TIDAK menutup mayoritas gap ke `pcg`.
Klaim lama "biaya struktural inheren, tidak murah dipangkas" perlu direvisi
sebagian: recurrence-nya SENDIRI memang bisa dipangkas ~5%, tapi porsi
terbesar gap kemungkinan besar ada di struktur shuffle-loop ZepFold
(Lemire reduction ganda, dua-swap array, overhead per-token di luar
recurrence inti), bukan di pilihan recurrence PRNG.

**Rekomendasi untuk aplikasi nyata**: kalau harus memilih satu varian
default untuk "pengacakan data" produksi, data checkpoint ini mendukung
`scrambler_wired_addressable` (addressable-init + `winner_wired_v2`) atas
`scrambler_addressable` (addressable-init + recurrence original) — kualitas
statistik setara, sedikit lebih cepat, dan cycle-nya konsisten dengan
kandidat `winner_wired_v2` yang sudah jadi baseline riset lain (bukan
recurrence lama yang tidak lagi jadi rujukan di tempat lain). Ini bukan
keputusan final/promosi resmi — cuma observasi berbasis data checkpoint ini,
sesi berikutnya bebas menimbang ulang kalau ada temuan baru.

**Belum dikerjakan** (scope baru kalau mau lanjut): re-run di mesin idle
seperti checkpoint "dekomposisi gap 1.62x", atau dekomposisi
`no_reseed_isolation_test`/`single_mread_isolation_test`-style khusus untuk
varian `winner_wired_v2`.

## 8. Setelah mengerjakan follow-up di atas

Jangan lupa jalankan `/graphify --update` sebelum menutup sesi (sesuai
`CLAUDE.md` project ini) — lihat riwayat commit/sesi sebelumnya untuk pola
alurnya (deteksi file, semantic extraction lewat subagent kalau ada
dokumen/gambar baru, `build_merge` additif ke `graph.json` yang sudah ada,
bukan rebuild dari nol — **hati-hati shrink-guard**: kalau me-re-extract
`RESULTS.md`/`SPEED_BENCHMARK.md` lagi, node konsep lintas-file yang
kepemilikannya kebetulan tercatat di file itu — mis. `pcg`, `xoshiro256`,
`orbit_addressing`, `zepfold` — bisa ikut terhapus oleh replace-on-re-extract
kalau chunk baru tidak mendeklarasikan ulang node itu; cek `graphify-out/graph.json`
lama vs node baru sebelum merge kalau menemukan warning shrink).
