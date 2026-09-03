# Handover: eliminasi 11 pemenang combo-search via Pareto (speed x avalanche), lalu posisikan vs keluarga PRNG lain

Ditulis untuk sesi Claude Code baru yang mengimplementasikan dan menjalankan
eksperimen ini. **Status: BELUM DIKERJAKAN** — dokumen ini murni spesifikasi
(handover), ditulis di sesi yang tidak mengeksekusi kodenya sendiri. Baca
sampai selesai sebelum menulis kode — semua keputusan desain open-ended
(axis definitions, N seed, ambang normalitas, cara masuk ke tabel lama)
sudah diputuskan di sini supaya sesi eksekusi tidak perlu riset ulang.

## 1. Konteks & tujuan

`experiments/2026-9-2_singleblock-cycle-combo-search/RESULTS.md` sudah
menghasilkan **11 kandidat FULLY CLEAN** PractRand (16GB, K∈{1,2,4,8,16,32,
64,96}) dari pencarian 756-kombinasi DSL (tap width x finalizer x inject).
Sebelas ini semuanya width=8, cuma beda finalizer:

| id | finalizer | file sumber |
|---|---|---|
| w8_f8_i0  | `XORSHIFT(15)` | `../2026-9-2_singleblock-cycle-combo-search/candidates/w8_f8_i0.c` |
| w8_f9_i0  | `XORSHIFT(16)` | `.../candidates/w8_f9_i0.c` |
| w8_f10_i0 | `XORSHIFT(17)` | `.../candidates/w8_f10_i0.c` |
| w8_f22_i0 | `XORSHIFT(16)->MUL(0x85ebca6b)` | `.../candidates/w8_f22_i0.c` |
| w8_f24_i0 | `XORSHIFT(16)->MUL(0x9e3779b7)` | `.../candidates/w8_f24_i0.c` |
| w8_f25_i0 | `XORSHIFT(16)->MUL(0x06a0dd9b)` | `.../candidates/w8_f25_i0.c` |
| w8_f26_i0 | `MUL(0x85ebca6b)->XORSHIFT(13)` | `.../candidates/w8_f26_i0.c` |
| w8_f27_i0 | `MUL(0x85ebca6b)->XORSHIFT(16)` | `.../candidates/w8_f27_i0.c` |
| w8_f28_i0 | `MUL(0xc2b2ae35)->XORSHIFT(13)` | `.../candidates/w8_f28_i0.c` |
| w8_f29_i0 | `MUL(0xc2b2ae35)->XORSHIFT(16)` | `.../candidates/w8_f29_i0.c` |
| w8_f33_i0 | `ADD(b)->ROT(cons)` | `.../candidates/w8_f33_i0.c` |

Semua sudah lolos avalanche K=1 (Tier 0), speed K=1 (microbench,
0.90x-1.06x overhead), dan avalanche K=255 satu-seed
(`avalanche_heatmap_winners_k255.py`, `base_seed=1`, tidak ada bit mati —
lihat RESULTS.md eksperimen itu). **Masih 11 kandidat, belum ada yang
tereliminasi** — task ini menambah dua axis baru yang lebih ketat untuk
mempersempit ke satu (atau beberapa) kandidat pemenang final, lalu
memposisikannya terhadap PRNG lain (xoshiro, philox, pcg32, chacha20,
mt19937, splitmix64) yang sudah dibandingkan di
`experiments/2026-8-31_prng-family-benchmark/`.

**Non-goal eksplisit**: TIDAK mempromosikan apa pun ke `ra_core.c` — itu
tetap butuh konfirmasi eksplisit user terpisah, di luar scope task ini
(sama seperti batasan HANDOVER combo-search asal).

## 2. Axis A — Speed: K=1, K=255, continuous-stream

Reuse metodologi `../2026-8-31_prng-family-benchmark/benchmark_all.c`'s
`mode_reinit_sweep()` (CLOCK_MONOTONIC, min-of-3-trials, volatile checksum
sink, `cycles = clamp(cycles_target_words/K, 10, 200000)`) — **jangan
menulis ulang metodologi timing baru**, port fungsi itu verbatim ke harness
baru di folder ini (lihat §5 struktur file).

Definisi 3 titik/axis (keputusan desain, ikuti persis — jangan
interpretasi ulang):

- **K=1**: satu titik sweep `mode_reinit_sweep`, K=1. Worst-case reinit
  (skenario "addressable agresif" yang jadi motivasi seluruh riset ini).
  Comparable langsung ke tabel lama's kolom "ns/word at K=1"
  (`../2026-8-31_prng-family-benchmark/RESULTS.md` baris `singleblock`:
  33.4 ns/word).
- **K=255**: satu titik sweep `mode_reinit_sweep`, K=255 — batas maksimum
  `ra_core_singleblock`/varian combo (fast-path tanpa reseed, valid untuk
  `rng<=255` saja, lihat guard `core->max_rng` di harness lama). Comparable
  ke tabel lama's baris `singleblock` di kolom "largest K measured=255"
  (0.132 ns/word) — **catatan tabel lama eksplisit bilang ini BUKAN rezim
  asimtotik flat yang sama dengan kandidat tak-terbatas** (xoshiro dst. diuji
  sampai K=1,000,000); jangan bandingkan langsung sebagai "throughput flat"
  tanpa catatan itu.
- **continuous-stream** (axis BARU, tidak ada di tabel lama — didesain di
  sini karena family `singleblock` capped di K=255 per-call, jadi tidak
  bisa ikut sweep sampai K=1,000,000 seperti kandidat lain): ukur MB/s
  sungguhan lewat CLI `--stream <base_key> <n> 255` masing-masing kandidat
  (bukan microbench loop `core->fn(...,NULL)` — pakai jalur nyata yang
  sesungguhnya dipakai riset ini untuk PractRand, termasuk overhead
  `fwrite`/syscall write yang microbench K=255 di atas sengaja tidak
  hitung). Generate `n` >= 1,000,000,000 word (4GB) via `--stream` ke
  `/dev/null` (bukan pipe ke proses lain, supaya waktu murni generator +
  write syscall, tanpa overhead PractRand), ukur wall-clock dengan
  `time`/`clock_gettime` di sekeliling proses, hitung
  `MB/s = (n*4 bytes) / elapsed_s / 1e6`. Ini axis yang mensimulasikan
  penggunaan riil (banyak reinit berturut-turut, K=255 tiap blok, terus
  menerus) — beda dari K=255 di atas yang cuma satu blok per pengukuran.

Jalankan ketiganya untuk **seluruh 11 kandidat** (bukan cuma yang lolos
axis lain — semua axis dikumpulkan dulu sebelum Pareto §4).

## 3. Axis B — Avalanche multi-seed + uji normalitas

`avalanche_heatmap_winners_k255.py` (eksperimen combo-search) cuma
mengukur SATU seed (`base_seed=1`) — cukup untuk menyingkirkan defect
"bit mati" kelas `pruned_winner` (lihat `quality_gate.py`'s
`avalanche_stats()` docstring), TAPI tidak menjamin bit terlemah/terkuat
stabil di seed lain. Axis ini menutup celah itu.

**N seed**: 64 seed, deterministik dan reproducible: `seed = 1..64`
(bukan seed acak — supaya sesi lain bisa reproduce angka yang sama persis
tanpa RNG-seed-generator terpisah, dan supaya "seed=1" yang sudah pernah
diuji tetap salah satu titik data).

**Untuk tiap kandidat, tiap seed**: capture K=255 (satu cycle penuh, pakai
`--single <seed> 255`, port `compute_matrix()` dari
`avalanche_heatmap_winners_k255.py`), hitung 32 `per_bit_fractions` (reuse
`quality_gate.py`'s `avalanche_stats()` — JANGAN tulis ulang implementasi
Hamming/fraction, sudah ada dan sudah tervalidasi). Simpan per seed:
`min_bit_fraction` (+ index bit terlemah), `max_bit_fraction` (+ index bit
terkuat).

**Uji normalitas** (pakai `scipy.stats.shapiro`, sudah terinstal — versi
1.17.1 dicek di sesi ini): untuk tiap kandidat, kumpulkan array 64 nilai
`min_bit_fraction` dan array 64 nilai `max_bit_fraction` lintas seed,
jalankan Shapiro-Wilk pada masing-masing. Kriteria lolos axis ini (checklist
eksplisit, bukan "kira-kira normal"):

1. Shapiro-Wilk p-value > 0.05 untuk kedua array (`min_bit_fraction` dan
   `max_bit_fraction`) — gagal tolak H0 normalitas.
2. **Tidak ada outlier ekstrem**: tidak ada satu pun dari 64 seed yang
   `min_bit_fraction < 0.2` (ambang lama dari `tier0_avalanche.py`'s
   `MIN_BIT_FLOOR`) — walau distribusinya "normal" secara statistik,
   ekor bawah yang menyentuh ambang gagal tetap harus dilaporkan sebagai
   kegagalan axis ini, bukan diabaikan.
3. **Sebaran index bit terlemah**: histogram index bit (0-31) yang jadi
   `argmin` di tiap seed. Sehat = tersebar (tidak ada satu index yang jadi
   argmin di >50% dari 64 seed — kalau ada, itu tanda structural weakness
   pada bit spesifik itu, mirip pola bit 5/6 `pruned_winner`, walau tidak
   se-ekstrem 0.0078). Laporkan histogram penuh di RESULTS.md, bukan cuma
   pass/fail biner.

Kandidat yang gagal kriteria manapun di atas TETAP dilaporkan (bukan
dihapus diam-diam dari tabel) — tandai FAILED_AXIS_B dengan alasan spesifik
yang gagal (mana dari 3 kriteria).

## 4. Pareto frontier selection

Dua sumbu speed dipakai untuk Pareto (bukan tiga — K=255 dilaporkan sebagai
kolom diagnostik tapi TIDAK jadi sumbu Pareto terpisah, supaya ruang
objektif tidak terlalu terfragmentasi oleh 3 metrik speed yang berkorelasi
tinggi satu sama lain untuk keluarga yang sama):

- **Speed 1**: `ns_per_word` di K=1 (lebih kecil lebih baik) — worst-case.
- **Speed 2**: `MB/s` continuous-stream (lebih besar lebih baik) — best-case
  amortized.
- **Avalanche**: `min(min_bit_fraction across 64 seeds)` (lebih besar lebih
  baik) — metrik keselamatan paling ketat (kasus terburuk lintas seed, BUKAN
  rata-rata, supaya satu seed jelek tidak "ditutupi" seed lain).

Kandidat gagal Axis B (§3 kriteria 1 atau 3) dikeluarkan dari Pareto
frontier walau lolos kriteria 2 (ambang absolut) — kegagalan normalitas
statistik atau konsentrasi bit lemah adalah sinyal struktural, bukan cuma
soal skala.

Definisi dominasi standar (3 objektif di atas): kandidat A didominasi
kandidat B kalau B sama-atau-lebih-baik di ketiga objektif DAN
strictly-lebih-baik di minimal satu. Set Pareto = kandidat yang tidak
didominasi kandidat manapun.

**Kalau Pareto frontier > 1 kandidat**: laporkan semua sebagai
co-frontier, JANGAN paksa satu pemenang tunggal — user yang putuskan
trade-off mana yang dipentingkan (echo keputusan HANDOVER combo-search
asal: "no acceptable candidate" adalah outcome valid, di sini analognya
"lebih dari satu pemenang co-optimal" juga valid, jangan dipaksa jadi satu).

## 5. Perbandingan vs keluarga PRNG lain — REUSE tabel lama, JANGAN tes ulang

`../2026-8-31_prng-family-benchmark/RESULTS.md` **tidak diedit** (read-only
precedent, sama seperti semua eksperimen lain di repo ini). Sebagai
gantinya, buat `RESULTS.md` BARU di folder ini yang:

1. **Mengutip verbatim** tiga tabel dari `RESULTS.md` lama (seeding cost
   OLS, K=1 vs largest-K, MB/s sanity check) — copy-paste tabel markdown-nya
   apa adanya, JANGAN generate ulang angkanya (tidak perlu rebuild
   `benchmark_all.c`, tidak perlu re-run `xoshiro`/`philox`/dst).
2. **Menambahkan baris baru** untuk kandidat Pareto-frontier hasil §4 (nama
   baris: id kandidat combo-search, mis. `w8_f9_i0 (combo-winner)`) berisi
   angka dari axis §2 (K=1 ns/word masuk ke tabel "K=1 vs largest-K", K=255
   ns/word bisa jadi catatan tambahan, continuous-stream MB/s masuk ke
   tabel "MB/s sanity check").
3. Baris lama `singleblock` (baseline tanpa finalizer, wide-o width=2) di
   ketiga tabel **tetap ada apa adanya** sebagai pembanding sebelum-sesudah
   — supaya terlihat jelas biaya tambahan combo-winner vs baseline asli
   (finalizer nambah instruksi, tap width 8 vs 2 juga nambah), bukan cuma
   vs PRNG luar.

**Kalau posisi combo-winner ternyata lebih lambat dari `singleblock` lama
di axis manapun**: laporkan apa adanya (dengan alasan — lebih banyak
instruksi finalizer/tap width lebih lebar), JANGAN sembunyikan atau
bulatkan ke arah yang menguntungkan.

## 6. File yang TIDAK BOLEH diubah (read-only precedent)

- `../2026-9-2_singleblock-cycle-combo-search/candidates/*.c` — sumber
  fungsi kandidat, copy verbatim ke harness baru (pola sama seperti
  `combo_gen.py` sendiri copy verbatim dari `ra_core.c`).
- `../2026-9-2_singleblock-cycle-combo-search/RESULTS.md`,
  `combo_prng.py`, `recipes.py`, `quality_gate.py` (reuse
  `avalanche_stats()` via import, sama pola seperti
  `avalanche_ra_core.py`/`avalanche_heatmap_winners_k255.py`).
- `../2026-8-31_prng-family-benchmark/*` (semua file) — kutip angkanya,
  jangan edit atau re-run untuk "menyegarkan" angka.
- `experiments/2026-9-1_family-productionization/ra_core.c` — tidak
  disentuh sama sekali, tidak relevan untuk task ini (task ini murni
  benchmark/seleksi kandidat, bukan promosi).

## 7. Struktur file baru (folder ini)

- `winners_bench.c` — harness baru, port `core_fn_t`/`mode_reinit_sweep`/
  `now_seconds()` dari `benchmark_all.c` verbatim + copy verbatim 11 fungsi
  `ra_core_singleblock_combo` dari 11 file kandidat (rename tiap satu jadi
  unik, mis. `ra_core_w8_f8_i0` dst., supaya tidak bentrok simbol saat
  digabung satu file) + `ra_core_singleblock` baseline (copy dari
  `benchmark_all.c` juga, sebagai baris pembanding "before" di §5).
  `CORES[]` berisi 12 entri (11 combo-winner + 1 baseline).
- `winners_bench_run.py` — orkestrator: reinit-sweep K=1 & K=255 (semua 12
  entri), continuous-stream mode (baru, generate ke `/dev/null`, lihat §2).
- `avalanche_multiseed.py` — Axis B: loop 11 kandidat x 64 seed, panggil
  binary kandidat (`../2026-9-2_singleblock-cycle-combo-search/candidates/
  w8_f*_i0`, binary yang sudah dikompilasi combo-search TIDAK perlu
  dikompilasi ulang, tinggal dipakai) via `--single <seed> 255`, reuse
  `quality_gate.py`'s `avalanche_stats()`, jalankan `scipy.stats.shapiro`,
  tulis hasil ke `avalanche_multiseed_results.jsonl`.
- `pareto_select.py` — Axis A+B -> filter dominasi §4, cetak Pareto set.
- `RESULTS.md` — struktur: Konteks, Axis A (tabel 11x3 speed), Axis B
  (tabel 11 kandidat x {shapiro p-value min/max, outlier count, histogram
  bit terlemah}), Pareto frontier (kandidat mana yang lolos + alasan
  eliminasi yang gagal), Perbandingan vs PRNG lain (§5), Rekomendasi
  (eksplisit, termasuk kemungkinan >1 co-frontier atau 0 kandidat kalau
  SEMUA gagal Axis B — juga outcome valid, jangan dipaksa).

## 8. Non-goals

- Tidak menyentuh `ra_core.c` atau mempromosikan kandidat manapun ke
  produksi — keputusan itu di luar scope, butuh konfirmasi user terpisah.
- Tidak menambah kandidat baru di luar 11 pemenang combo-search ini (tidak
  membuka lagi ruang DSL 756-kombinasi).
- Tidak menjalankan ulang PractRand — 11 kandidat ini sudah lolos 16GB
  Promotion Tier, task ini murni axis speed+avalanche tambahan untuk
  seleksi, bukan re-validasi statistical randomness.
- Tidak dieharder di sini — itu ada di rekomendasi langkah-selanjutnya
  `../2026-9-2_singleblock-cycle-combo-search/RESULTS.md`, giliran setelah
  Pareto winner dipilih di sini, sesi terpisah.

## 9. Penutup sesi

`CLAUDE.md` proyek ini mewajibkan `/graphify --update` sebelum menutup
tugas riset — jalankan setelah `RESULTS.md` selesai.
