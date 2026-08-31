# Handover: 1TB interleaved-PractRand anomaly follow-up

Ditulis untuk sesi Claude Code baru yang dipicu frasa "lanjutkan follow-up
1TB" / "lanjutkan diagnostik PractRand" atau serupa. **Baca file ini SEBAGAI
TITIK MASUK** untuk follow-up ini (bukan `HANDOVER.md` utama folder ini —
itu spek riset paralelisasi awal, sudah sepenuhnya dikerjakan/superseded,
baca hanya untuk konteks Q1/Q2/Q3 secara umum).

## 1. Status & apa yang sudah terjadi (2026-08-31)

Checkpoint tambahan sebelum paper baru: tier `1tb` ditambahkan ke
`TIERS_Q1B` di `common.py` (`"1tb": (8, 1024**4)`), dijalankan via
`python3 interleave_practrand.py 1tb` (~3h52m wall-clock). Ini **Method B**
(interleaved cross-stream test, 8 stream `winner_wired_v2 --stream <seed> n`
di-interleave word-by-word, dipipe ke PractRand) — BUKAN uji single-stream.

**Hasil mentah** (detail lengkap + tabel di `RESULTS.md`, bagian "1TB
(checkpoint follow-up, 2026-08-31)"):
- Bersih (0 anomali) di checkpoint 64GB/128GB/256GB/512GB.
- **Checkpoint akhir 1TB: 3 dari 304 test flagged** —
  `BCFN(2+0,13-0,T)` R=+14.4 p=3.0e-7 **very suspicious**;
  `FPF-14+6/16:(5,14-0)` R=+9.2 p=3.7e-8 **suspicious**;
  `FPF-14+6/16:all` R=+7.9 p=7.0e-7 **suspicious**.
- Data mentah lengkap: `interleave_practrand_results_1tb.json` (`stdout_tail`).

**Bug harness ditemukan** (dijelaskan detail di `RESULTS.md`, TIDAK
diperbaiki in-place di `interleave_practrand.py` karena file itu read-only
precedent dipakai ulang eksperimen lain): field `passed` di JSON hasil
salah melaporkan tier `1tb` sebagai `PASSED` -- pengecekan anomali di
`run_interleave_practrand()` mem-match string besar `"SUSPICIOUS"`, padahal
PractRand menulis tag itu huruf kecil (`suspicious`/`very suspicious`).
**Selalu baca `stdout`/`stdout_tail` mentah, jangan percaya field `passed`
untuk run apa pun yang scale-nya lebih besar dari yang sudah terverifikasi
bersih sebelumnya.**

## 2. Pertanyaan yang belum terjawab

User (2026-08-31) menanyakan: "Ini terjadi di tes algoritma yang mana?
Padahal di versi sebelumnya masih aman. Kalau memang gagal disini, kita
perlu memperbaiki state init lagi." Jawaban parsial yang sudah diberikan
(lihat percakapan sesi ini): subjeknya `winner_wired_v2` (pruned+wired,
BUKAN `winner_wired_addressable`/Tahap 0-5, BUKAN `ra_prng2.c`), dan "versi
sebelumnya aman" itu benar tapi menyesatkan -- **1TB belum pernah diuji di
jalur manapun sebelum sesi ini**, baik interleaved maupun single-stream
(percobaan single-stream 1TB di `2026-8-27_operand-position-search/`
sendiri tercatat sebagai "open follow-up, not done" di `RESULTS.md` folder
itu). Jadi ini bukan regresi, tapi checkpoint pertama yang menyentuh skala
ini sama sekali.

**Yang BELUM diketahui, dan menentukan apakah "perbaiki state init" itu
langkah yang tepat**: apakah 3 test flagged ini karena (a) korelasi
antar-stream asli (baru ini yang relevan dengan state-init/avalanche antar
seed), (b) kelemahan kualitas single-stream `winner_wired_v2` sendiri yang
baru kelihatan di 1TB, terlepas dari interleaving (state-init TIDAK relevan
untuk ini -- perbaikannya, kalau ada, di ranah lain), atau (c) noise dari
satu run yang tidak diulang (304 test kumulatif per checkpoint, ribuan
across seluruh staged run -- sebagian flag "suspicious" bisa muncul dari
kebisingan multiple-testing murni). User secara eksplisit memilih untuk
**tidak** menjalankan diagnostik tambahan malam ini (~7-8 jam gabungan,
terlalu lama untuk sesi ini) dan meminta dicatat sebagai rencana untuk sesi
berikutnya.

**Follow-up pertanyaan (sesi lanjutan, sama hari)**: user bertanya apakah
cross-seed test ini sudah pernah dicoba untuk `winner_wired_addressable`
(Tahap 0-5, "Orbit Addressing"). Jawaban: **ya untuk Method A+B sampai
128GB** (`tahap3_cross_correlation.py`/`tahap3_interleave_practrand.py` di
`2026-8-30_addressable-init-research/`, tier `xlarge` -- diverifikasi ulang
raw `stdout` sesi ini, benar-benar bersih, bukan false-positive seperti
bug `passed` di atas), **tapi TIDAK PERNAH ke 1TB** -- tier itu cuma ada di
`common.py` folder ini, tidak pernah ditambahkan ke folder addressable-init.
Ini relevan karena `winner_wired_addressable` **berbagi core generation
loop byte-for-byte** (`ra_permutation_cycle`/`ra_reseed`/`ra_core`) dengan
`winner_wired_v2` -- kalau anomali 1TB di atas berasal dari core loop
(bukan spesifik formula init `winner_wired_v2`), `winner_wired_addressable`
kemungkinan besar akan menunjukkan gejala serupa. User memilih menambahkan
ini ke rencana besok (bukan run sekarang) -- lihat Langkah 3 baru di bawah.

## 3. Rencana diagnostik (belum dikerjakan, urutan disarankan)

### Langkah 1 — Single-stream `winner_wired_v2` ke 1TB (tanpa interleave)

Menjawab (b) vs (a)/(c). Kalau `BCFN`/`FPF-14+6/16` yang SAMA muncul di
sini (satu stream saja, tanpa interleaving), itu kelemahan single-stream,
BUKAN masalah cross-stream/state-init -- perbaikan state-init tidak akan
menyelesaikan apa pun untuk temuan ini. Kalau bersih, itu memperkuat dugaan
(a) atau (c).

Ini sekaligus menyelesaikan open item lama yang sudah dicatat di
`../2026-8-27_operand-position-search/RESULTS.md` (baris ~235-257, "PractRand
8GB -> 1TB... status: the 1TB validation is an open follow-up, not done").
**Baca catatan itu dulu** -- ada precedent command line di sana
(`-tlmin 8GB -tlmax 1TB`, ~2-3 jam perkiraan berdasarkan attempt sebelumnya
yang di-interrupt di 1232s untuk porsi awal) dan dua percobaan sebelumnya
yang gagal/tidak lengkap, supaya tidak mengulang kesalahan yang sama.

```bash
cd experiments/2026-8-27_operand-position-search
~/Documents/research/PractRand/RNG_test stdin32 -tlmin 8GB -tlmax 1TB \
  < <(./winner_wired_v2 --stream 0 274877906944) \
  > practrand_winner_wired_v2_1TB_singlestream.txt 2>&1 &
```
(`274877906944` = 1TB/4 bytes = jumlah word 32-bit yang dibutuhkan; sesuaikan
kalau binary butuh oversupply seperti pola `interleave_practrand.py`'s
`supply_words_per_stream`.) Jalankan di background, cek hasil sebelum
lanjut ke Langkah 2 supaya tidak berebut CPU (mesin 2C/4T).

### Langkah 2 — Re-run interleaved 1TB dengan seed set berbeda

Menjawab (a) vs (c). Kalau `BCFN(2+0,13-0,T)`/`FPF-14+6/16` flagged LAGI
(sama atau mirip) dengan seed set lain -- kemungkinan besar sinyal asli
(cross-stream), bukan noise. Kalau tidak muncul lagi -- kemungkinan besar
noise run tunggal.

Cara termudah: edit sementara `random.Random` seed di sebuah script
turunan kecil (jangan edit `interleave_practrand.py` in place -- buat
`interleave_practrand_1tb_confirm.py` baru yang import
`run_interleave_practrand` lalu panggil dengan 8 seed lain, mis.
`range(1000, 1008)` alih-alih `range(8)` default) -- lihat
`cross_correlation_ra_prng2.py`/`interleave_practrand_ra_prng2.py` di
folder ini sebagai contoh pola "reuse fungsi, jangan edit file asli".
`run_interleave_practrand(k, total_bytes)` sudah generik soal seed -- k
stream diberi seed `range(k)` secara hardcoded di dalam fungsinya
(lihat baris ~48 `interleave_practrand.py`: `for seed in range(k)`),
jadi confirm-run butuh sedikit refactor kecil (parameter `seeds` eksplisit)
kalau mau seed lain -- **jangan lupa cek ini di source sebelum menulis
script baru**, asumsi di atas bisa saja sudah berubah.

```bash
python3 interleave_practrand_1tb_confirm.py   # ~3h52m, jalankan di background
```

### Langkah 3 — Interleaved 1TB untuk `winner_wired_addressable` (cross-seed, core loop yang sama)

Menjawab dimensi baru: apakah anomali 1TB spesifik ke `winner_wired_v2`
(mis. formula init-nya), atau melekat ke core generation loop yang
dipakai BERSAMA oleh kedua varian (`ra_permutation_cycle`/`ra_reseed`/
`ra_core`, byte-for-byte identik antara `winner_wired_v2.c` dan
`winner_wired_addressable.c` -- cuma `ra_init_state`/
`ra_init_state_addressable` yang beda). Kalau `BCFN(2+0,13-0,T)`/
`FPF-14+6/16` (atau tag suspicious lain) muncul lagi di sini, itu bukti
kuat penyebabnya di core loop bersama, BUKAN formula init spesifik --
berarti kalaupun perbaikan diperlukan, itu di luar scope "perbaiki state
init" yang diduga user di §2. Kalau bersih, itu justru mengarah ke formula
init `winner_wired_v2` (bukan addressable) sebagai kandidat penyebab.

Tambahkan tier `1tb` ke `TIERS_Q1B` yang dipakai
`tahap3_interleave_practrand.py` (tier itu di-import dari `common.py`
folder INI, jadi entri `"1tb": (8, 1024**4)` yang sudah ditambahkan sesi
2026-08-31 otomatis kepakai -- tidak perlu tambah lagi) lalu jalankan:

```bash
cd experiments/2026-8-30_addressable-init-research
python3 tahap3_interleave_practrand.py 1tb   # ~3h52m (skala sama dengan winner_wired_v2 1TB run)
```

Catatan: script ini gated -- baca `GATED_TIERS = ["smoke", "medium", "full"]`
di `tahap3_interleave_practrand.py` baris ~36, tier `1tb` (seperti `xlarge`)
harus dipanggil eksplisit sebagai argumen, TIDAK otomatis lewat default
run. **Baca `stdout`/`stdout_tail` mentah** untuk verdict, bukan field
`passed` (bug case-sensitivity yang sama berlaku di sini juga, karena
script ini reuse `run_interleave_practrand()` dari `interleave_practrand.py`
tanpa modifikasi).

### Langkah 4 — Sintesis & keputusan

Setelah Langkah 1, 2, dan 3 selesai, susun matriks (flagged/bersih ×
single-stream/interleaved × winner_wired_v2/winner_wired_addressable) dan
simpulkan hipotesis mana yang paling didukung data:
- **(a) korelasi antar-stream asli, spesifik ke formula init
  `winner_wired_v2`**: flagged di interleaved v2, bersih di single-stream
  v2, DAN bersih di interleaved addressable. Baru kalau ini yang terjadi,
  state-init/avalanche formula `winner_wired_v2` relevan untuk diperbaiki --
  dan itu scope besar (Tahap 0-3 addressable-init penuh diulang kalau
  formulanya berubah, per konvensi `HANDOVER_TAHAP5.md` §6).
- **(a') korelasi antar-stream asli, di core loop bersama**: flagged di
  interleaved v2 DAN interleaved addressable, bersih di single-stream v2.
  Perbaikan (kalau ada) di `ra_permutation_cycle`/`ra_reseed`/`ra_core`
  yang dipakai KEDUA varian -- scope lebih besar lagi (mempengaruhi
  `winner_wired_v2.c` yang sudah jadi baseline banyak riset lain).
- **(b) kelemahan single-stream**: flagged juga di single-stream v2 --
  state-init TIDAK relevan untuk ini, penyebabnya di ranah lain
  (kualitas generator per-stream, bukan cross-stream).
- **(c) noise run tunggal**: tidak flagged lagi di re-run seed lain
  (Langkah 2) -- kemungkinan besar bukan sinyal asli.

Kalau (b) atau (c) yang didukung, laporkan temuan itu apa adanya di paper
sebagai catatan batas skala (mis. "cross-stream independence divalidasi
bersih sampai 512GB gabungan; 1TB menunjukkan tag suspicious yang
[terkonfirmasi/tidak terkonfirmasi] persisten, kemungkinan besar
[terkait/tidak terkait] kualitas generator") -- JANGAN dipaksa jadi
"menang" atau disembunyikan.

## 4. Update sesi lanjutan (2026-08-31 siang) — Langkah 3 dimulai, DI-STOP, belum selesai

User memutuskan **langsung ke Langkah 3** (skip Langkah 1) untuk sesi ini:
Langkah 1 (single-stream `winner_wired_v2`) dianggap kurang beralasan selain
"belum pernah dicoba", sedangkan Langkah 3 (`winner_wired_addressable`
cross-seed 1TB) langsung menjawab dimensi (a) vs (a') karena kedua varian
berbagi core loop byte-for-byte. **Langkah 1 dan 2 masih belum dikerjakan
sama sekali** — status masih seperti tertulis di §3.

**Yang terjadi (urutan kronologis, semua di folder
`2026-8-30_addressable-init-research/`)**:

1. Verifikasi pra-jalan: tidak ada proses lama nyangkut, binary
   `winner_wired_addressable` sudah dikompilasi setelah source terakhir
   diedit (tidak perlu rebuild), `tahap3_interleave_practrand.py` sudah
   re-point ke binary itu dan reuse `TIERS_Q1B["1tb"]` -- tinggal panggil
   `python3 tahap3_interleave_practrand.py 1tb`.
2. **Attempt 1** (`python3 tahap3_interleave_practrand.py 1tb`, dijalankan
   via tool background TANPA `nohup`, mulai 10:50): **kena kill sekitar
   14:17-18** (~3j27m berjalan, diperkirakan dekat checkpoint 512GB->1TB
   berdasarkan skala waktu run `winner_wired_v2` sebelumnya), TIDAK selesai.
   `journalctl -k` untuk seluruh window 10:00-14:20 dicek: **tidak ada
   OOM-killer, tidak ada suspend/resume** di sekitar waktu kill (suspend
   satu-satunya di log terjadi jam 10:22, sebelum job mulai). Kesimpulan
   kerja: kemungkinan besar **SIGHUP** ke shell induk sesi CLI diteruskan
   ke child process (karena tidak dijalankan dengan `nohup`).
   **Kerugian: 0 data parsial tersisa** -- `run_interleave_practrand()` di
   `../2026-8-29_parallelization-research/interleave_practrand.py` cuma
   nge-`print`/simpan hasil SEKALI di akhir lewat `test.communicate()`,
   jadi kill sebelum selesai = kehilangan seluruh run, bukan cuma sisa
   checkpoint terakhir.
3. Untuk itu dibuat script baru (TIDAK edit file asli, sesuai konvensi
   repo): **`tahap3_interleave_practrand_1tb_live.py`** -- varian yang baca
   stdout PractRand per baris di thread terpisah dan langsung flush ke
   file log (`tahap3_interleave_practrand_1tb_live.log`) begitu tiap
   checkpoint muncul, supaya (a) bisa dipantau live per-checkpoint, (b)
   interupsi cuma kehilangan progres sejak checkpoint terakhir, bukan
   seluruh run.
4. **Attempt 2** (`python3 tahap3_interleave_practrand_1tb_live.py 1tb`,
   dijalankan dengan `nohup ... & disown` supaya tahan SIGHUP, mulai
   14:21): mencapai **64GB bersih (914 detik, 0/263 anomali)**, lalu **DI-
   STOP oleh user** jam ~14:51 karena ada urusan mendadak -- BUKAN
   kegagalan test, sama seperti precedent
   `2026-8-27_operand-position-search/RESULTS.md` baris ~235-249 (dua
   attempt 1TB sebelumnya juga di-stop di tengah jalan oleh user, bukan
   gagal). Semua proses (`tahap3_interleave_practrand_1tb_live.py`, 8
   stream `winner_wired_addressable`, `RNG_test`) sudah di-`kill` bersih,
   tidak ada proses nyangkut.
   Log parsial attempt 2 (cuma berisi blok 64GB) sudah di-**rename** ke
   `tahap3_interleave_practrand_1tb_live_attempt1_stopped_at_64GB.log`
   supaya tidak tercampur dengan run lanjutan (script baru akan nulis ke
   `tahap3_interleave_practrand_1tb_live.log` lagi, mode append -- kalau
   file lama TIDAK di-rename, isinya akan nyambung tanpa pemisah dan
   membingungkan pembacaan checkpoint mana milik attempt mana).

**Untuk lanjutkan** (sesi berikutnya): PractRand tidak bisa resume dari
checkpoint manapun -- ulangi dari 0 byte, ~3j52m total (bukan
tinggal ~3j sisa dari 64GB). Jalankan:

```bash
cd experiments/2026-8-30_addressable-init-research
nohup python3 tahap3_interleave_practrand_1tb_live.py 1tb \
  > tahap3_1tb_live_wrapper.log 2>&1 &
disown
```

Pantau progres via `tail -f tahap3_interleave_practrand_1tb_live.log` (baris
checkpoint muncul progresif: ~64GB@~15-22m, ~128GB@~40m, ~256GB@~70m,
~512GB@~126m, ~1TB@~232m kumulatif, berdasarkan skala `winner_wired_v2`
sebelumnya -- run selesai total ketika baris `[done] words_written=...`
muncul di akhir file). **Baca isi log mentah untuk verdict, field
`passed` di JSON hasil (kalau nanti dipakai `tahap3_interleave_practrand.py`
yang asli) SALAH karena bug case-sensitivity `SUSPICIOUS`/`suspicious` --
lihat §1.**

**Dokumentasi akhir untuk Langkah 3 ini** (keputusan user sesi ini, berlaku
juga untuk sesi lanjutan kecuali user bilang lain): **hanya update
`RESULTS.md`** di `2026-8-30_addressable-init-research/` (subseksi baru,
Tahap 3) dan `2026-8-29_parallelization-research/` (cross-reference pendek
di bagian "1TB checkpoint follow-up"). **Skip** `graphify update .`/
`/graphify --update` dan skip update memory untuk temuan Langkah 3 ini.

## 5. Setelah selesai

Update `RESULTS.md` bagian "1TB (checkpoint follow-up, 2026-08-31)" dengan
hasil Langkah 1-4 (jangan edit bagian yang sudah ada, tambahkan subseksi
baru) -- untuk Langkah 3 (winner_wired_addressable), tambahkan juga catatan
di `2026-8-30_addressable-init-research/RESULTS.md` Tahap 3 (jangan edit
bagian lama, subseksi baru), lalu jalankan `graphify update .` / `/graphify --update` (CLI dulu,
fallback manual dengan `scan_corpus` LENGKAP, ikuti
`feedback_graphify_update_workflow` di memory), dan update memory terkait.
