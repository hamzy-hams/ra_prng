# Hasil riset paralelisasi: independensi, collision, dan skalabilitas multi-stream

Sesuai `HANDOVER.md`. Subjek uji: `winner_wired_v2` (binary di
`experiments/2026-8-27_operand-position-search/winner_wired_v2`, CLI
`--stream <seed> <n>`). Tidak ada file source (`winner_wired_v2.c`,
`src/ra_prng2/*`, `src/ra_prng3/*`) yang dimodifikasi in-place — semua
harness di folder ini (`common.py`, `cross_correlation.py`,
`interleave_practrand.py`, `collision_scan.py`, `wall_time_scaling.py`,
`perf_scaling.sh`, `simd_prototype.c`) hanya memanggil CLI biner yang
sudah ada.

## Setup

- **Binary**: `winner_wired_v2`, compile flags `gcc -O3 -march=native
  -std=gnu17 -include stdalign.h` (sudah ter-compile sejak eksperimen
  2026-8-27).
- **Hardware**: Intel i3-1115G4 (Tiger Lake mobile), `nproc`=4 (2 core
  fisik, 4 thread via hyperthreading). AVX2 dan AVX-512F/VL/BW/DQ tersedia.
  RAM 7.5GB.
- **Baseline throughput** (re-verifikasi): `--stream 0 50000000` →
  0.491s ⇒ ~102M nilai/detik/proses (single-thread, uncontended).
- **Fakta arsitektural yang relevan** (dari pembacaan `winner_wired_v2.c`,
  belum tercatat di HANDOVER.md sebelumnya): `ra_init_state(L, M)` mengisi
  `L[256]` dan `M[256]` dengan formula tetap yang **identik untuk semua
  seed** (`M[i] = i*0x06a0dd9b + 0x06a0dd9b`, `L[i] = i*0x9e3779b7 +
  0x9e3779b7`). Satu-satunya jalur masuk seed ke sistem adalah skalar
  `cons = seed`. Jadi dua seed berdekatan (mis. 0 vs 1) mulai dari state
  array yang **benar-benar identik**, hanya beda 1 pada satu skalar `cons`
  — independensi antar-stream murni bergantung pada seberapa cepat
  perbedaan 1-bit di `cons` menyebar (avalanche) ke seluruh 256-elemen
  `L`/`M` di 255 langkah pertama, sebelum divergensi "cukup acak" untuk
  output. Ini konteks penting kenapa riset ini relevan, bukan kekhawatiran
  generik.

## 1. Independensi antar-stream

### Method A: cross-correlation langsung (`cross_correlation.py`)

Dua grup seed dibandingkan: **adjacent** (seed `0..K-1`, sesuai kekhawatiran
HANDOVER) vs **control** (`K` seed random tersebar via
`random.Random(42).sample(range(0,2**32), K)`). Untuk tiap grup:
`R = numpy.corrcoef(data)` (matrix Pearson `K×K`), `z_ij = R[i,j]*sqrt(n)`
(asimtotik `N(0,1)` di bawah H0 karena output dibatasi `[0,2^32)`, CLT
berlaku), threshold Bonferroni untuk `m=C(K,2)` pengujian ganda, family-wise
alpha 0.01.

| Tier | K per grup | n per stream | m_pairs | z_crit | flagged (adjacent) | flagged (control) |
|---|---|---|---|---|---|---|
| smoke | 8 | 200,000 | 28 | 3.570 | 0 | 0 |
| **full** | **128** | **1,000,000** | **8,128** | **4.851** | **0** | **0** |

**Kesimpulan Q1 Method A**: nol pasangan flagged di kedua grup pada skala
full (8,128 pasangan diuji per grup, threshold Bonferroni alpha=0.01). Tidak
ada sinyal korelasi linear lag-0 antara stream mana pun yang diuji, baik
seed berdekatan maupun seed tersebar acak — tidak ada efek adjacency
terdeteksi pada skala ini. **Batasan eksplisit**: metode ini hanya
menangkap korelasi linear pada lag 0, elemen-sejajar; dependensi
nonlinear/bit-level di luar cakupan metode ini (lihat Method B).

### Method B: interleaved PractRand (`interleave_practrand.py`)

K stream di-interleave word-level round-robin, dipipe langsung ke
`RNG_test stdin32 -tlmin <Lmin> -tlmax <Lmax>`. Tahapan berurutan dengan
checkpoint (semua tier harus bersih sebelum naik):

| Tier | K | Total data | Waktu | Hasil |
|---|---|---|---|---|
| smoke | 4 | 64MB | 0.7s | bersih, 142 test, no anomalies |
| medium | 8 | 1GB | 11.7s | bersih, 194 test, no anomalies |
| full | 8 | 16GB | 204s (~3.4 menit) | bersih, 240 test, no anomalies |
| **xlarge** | **8** | **128GB** | **1699s (~28,3 menit)** | **bersih, 273 test, no anomalies** |

**Kesimpulan Q1 Method B**: interleave 8 stream (seed 0-7) sampai 128GB
gabungan tidak menghasilkan satu pun FAIL/SUSPICIOUS di PractRand (checkpoint
bertahap 8GB→16GB→32GB→64GB→128GB, 273 test result di titik akhir). Kalau
stream-stream ini saling berkorelasi, menggabungkannya sebagai "satu RNG
besar" seharusnya membuat PractRand mendeteksi pola jauh lebih cepat
dibanding baseline single-stream 128GB-clean yang sudah diverifikasi di
eksperimen 2026-8-27 — tidak terjadi di sini. Ini bukti tambahan (menangkap
dependensi nonlinear, tidak seperti Method A) yang konsisten dengan hasil
Method A: tidak ada sinyal dependensi antar-stream, sekarang divalidasi pada
skala yang sama dengan baseline single-stream 128GB-clean, bukan cuma 16GB.

**1TB (checkpoint follow-up, 2026-08-31)**: tier `1tb` ditambahkan ke
`TIERS_Q1B` di `common.py` (data tier saja, `interleave_practrand.py`
sendiri tidak diedit), dijalankan via `python3 interleave_practrand.py 1tb`
di background (~3h52m wall-clock: 1319s→2434s→4157s→7545s→13907s kumulatif
untuk checkpoint 64GB/128GB/256GB/512GB/1TB). Hasil mentah, apa adanya:

| Checkpoint | Test result | Anomali |
|---|---|---|
| 64GB | 263 | tidak ada |
| 128GB | 273 | tidak ada |
| 256GB | 284 | tidak ada |
| 512GB | 295 | tidak ada |
| **1TB** | 304 | **3 flagged**: `BCFN(2+0,13-0,T)` R=+14.4 p=3.0e-7 **very suspicious**; `FPF-14+6/16:(5,14-0)` R=+9.2 p=3.7e-8 **suspicious**; `FPF-14+6/16:all` R=+7.9 p=7.0e-7 **suspicious** (301 test lain di checkpoint ini bersih) |

**Bersih sampai 512GB, TIDAK bersih di checkpoint akhir 1TB** — 3 dari 304
test di checkpoint 1TB kena tag "suspicious"/"very suspicious". Per konvensi
interpretasi yang sudah dipakai di `2026-8-30_addressable-init-research/HANDOVER.md`
("hanya tag FAIL/SUSPICIOUS yang blocking; tag `unusual` terisolasi yang
tidak persisten antar-checkpoint adalah noise rutin"), tag "suspicious"/"very
suspicious" **termasuk blocking**, jadi ini bukan noise rutin dan tidak boleh
dilaporkan sebagai "PASSED" begitu saja.

**Bug harness ditemukan (dilaporkan, TIDAK diperbaiki in-place)**: field
`passed` di JSON hasil (dan skrip `interleave_practrand.py` `main()`) salah
melaporkan tier `1tb` ini sebagai `PASSED`. Sebabnya:
`run_interleave_practrand()`'s pengecekan anomali
(`any(kw in stdout_s for kw in ("FAIL", "SUSPICIOUS"))`) mem-match string
persis huruf besar `"SUSPICIOUS"`, padahal PractRand menulis tag ini huruf
kecil (`suspicious`, `very suspicious`) di kolom Evaluation — jadi
pengecekan itu **tidak pernah bisa match** tag suspicious yang sesungguhnya,
hanya `FAIL` (yang memang selalu huruf besar). Verdict tier smoke/medium/
full/xlarge sebelumnya (di folder ini maupun turunannya di
`2026-8-30_addressable-init-research/tahap3_interleave_practrand.py`) TETAP
valid -- semuanya secara eksplisit menulis "no anomalies in N test result(s)"
di stdout mentah, jadi PractRand sendiri (bukan boolean yang buggy) yang
mengonfirmasi nol anomali di skala itu. Bug ini baru kena dampak sekarang
karena baru run 1TB inilah yang untuk pertama kalinya benar-benar
menghasilkan tabel Evaluation berisi anomali. **Tidak diperbaiki di
`interleave_practrand.py`** (dipakai ulang oleh eksperimen lain sebagai
read-only precedent) -- dicatat di sini sebagai temuan eksplisit supaya
sesi mendatang membaca `stdout` mentah, bukan cuma field `passed`, untuk run
skala besar.

**Interpretasi & batasan**: ini SATU run, tidak diulang untuk konfirmasi
(ulang penuh ke 1TB ≈ 3h52m lagi). p-value BCFN (~3e-7) cukup kecil untuk
jadi sinyal asli, bukan cuma kebisingan multiple-testing dari ratusan test
kumulatif -- tapi karena hanya muncul di checkpoint TERAKHIR (tidak ada
checkpoint lebih besar untuk melihat apakah persisten, beda dari pola
"unusual" 1-checkpoint yang biasanya "resolve" di checkpoint berikutnya yang
sudah terlihat di riset ini sebelumnya), status faktualnya adalah **flagged,
belum dikonfirmasi** -- bukan "PASSED bersih ke 1TB" (klaim lama di seksi
"Next steps" di bawah) dan bukan juga "FAIL terbukti". Kalau klaim 1TB-clean
dibutuhkan untuk paper, run konfirmasi ulang (idealnya 2-3x independent run)
diperlukan sebelum ditulis sebagai hasil final.

**Cross-reference (Langkah 3, 2026-08-31)**: uji interleaved 1TB yang sama
dijalankan ulang terhadap `winner_wired_addressable` (varian "Orbit
Addressing", berbagi core loop byte-for-byte dengan `winner_wired_v2`) untuk
mengecek apakah anomali di atas berasal dari core loop bersama atau spesifik
ke formula init `winner_wired_v2`. Hasil lengkap +tabel di
`../2026-8-30_addressable-init-research/RESULTS.md` ("Q1 Method B, 1TB
checkpoint follow-up") -- ringkas: dua test family yang sama (`BCFN(2+0,13-0,T)`,
`FPF-14+6/16`) muncul lagi tapi lebih lemah (turun ke `unusual` untuk 3 dari 4
flag), plus satu test baru (`DC6-9x1Bytes-1`) yang tidak flagged di v2.
Sebagian mendukung hipotesis korelasi di core loop bersama, tapi belum
konklusif -- Langkah 1 (single-stream `winner_wired_v2` 1TB) dan Langkah 2
(re-run interleaved v2 dengan seed lain) di `HANDOVER_1TB_FOLLOWUP.md` masih
belum dikerjakan, jadi sintesis Langkah 4 belum bisa final.

### Langkah 1, 2, dan sintesis Langkah 4 (2026-09-01) -- SELESAI

Dijalankan di VPS (lihat memory `reference_vps_long_running_jobs`), paralel
dengan Track B (`ra_prng2.c`, subseksi baru di bawah) dan dieharder battery
(`../2026-9-1_dieharder-battery/RESULTS.md`).

**Langkah 1 -- single-stream `winner_wired_v2` ke 1TB** (`stdbuf -oL RNG_test
stdin32 -tlmin 8GB -tlmax 1TB < ./winner_wired_v2 --stream 0 274877906944`,
log `langkah1_practrand_winner_wired_v2_1TB_singlestream.log`). Bersih
sampai 256GB, satu flag transien `[Low8/32]BCFN(2+6,13-1,T)` "unusual" di
checkpoint 64GB yang hilang lagi di 128GB/256GB, lalu:

| checkpoint | waktu (s) | hasil |
|---|---:|---|
| 512GB | 13,019 | `FPF-14+6/16:all` R=+5.7 p=7.1e-5 **unusual**, 294 lainnya bersih |
| **1TB** | **26,878** | **4/304 flagged**, lihat di bawah |

```
BCFN(2+0,13-0,T)                  R=  +8.4  p =  5.1e-4   unusual
FPF-14+6/16:(5,14-0)              R=  +7.3  p =  2.0e-6   unusual
FPF-14+6/16:(8,14-0)              R=  +7.0  p =  4.6e-6   unusual
FPF-14+6/16:all                   R=  +8.4  p =  2.1e-7   very suspicious
...and 300 test result(s) without anomalies
```

**Langkah 2 -- interleaved `winner_wired_v2` 1TB, seed set berbeda**
(`interleave_practrand_1tb_confirm_live.py`, seeds `range(1000, 1008)`
menggantikan default `range(8)`, log
`interleave_practrand_1tb_confirm_live.log`). Bersih sampai 512GB, lalu:

```
length= 1 terabyte (2^40 bytes), time= 30140 seconds
FPF-14+6/16:(5,14-0)              R=  +9.2  p =  3.8e-8   suspicious
FPF-14+6/16:all                   R=  +7.1  p =  3.7e-6   suspicious
...and 302 test result(s) without anomalies
```

`BCFN(2+0,13-0,T)` **tidak muncul lagi** dengan seed set ini -- berbeda dari
ketiga run 1TB lainnya (interleaved v2 seeds 0-7, addressable Langkah 3,
single-stream Langkah 1) yang semuanya memflag test itu.

**Matriks lengkap (4 konfigurasi 1TB, semua checkpoint sebelumnya bersih)**:

| konfigurasi | BCFN(2+0,13-0,T) | FPF-14+6/16 family |
|---|---|---|
| Interleaved v2, seed 0-7 (2026-08-31) | very suspicious (p=3.0e-7) | suspicious x2 (p=3.7e-8, 7.0e-7) |
| Interleaved addressable, seed 0-7 (Langkah 3) | unusual (p=4.2e-4) | unusual x1 + suspicious x1 (p=9.2e-6, 3.9e-6) |
| Single-stream v2, seed 0 (Langkah 1) | unusual (p=5.1e-4) | unusual x2 + very suspicious x1 (p=2.0e-6, 4.6e-6, 2.1e-7) |
| Interleaved v2, seed 1000-1007 (Langkah 2) | **tidak flagged** | suspicious x2 (p=3.8e-8, 3.7e-6) |

**Sintesis Langkah 4** (per hipotesis a/a'/b/c, `HANDOVER_1TB_FOLLOWUP.md` §3):

- **(a) spesifik ke formula init `winner_wired_v2`, harus bersih di
  single-stream dan di addressable**: **DITOLAK** -- single-stream v2
  (Langkah 1) justru flagged dengan severity tertinggi dari seluruh run
  (`very suspicious`, p=2.1e-7), dan addressable (Langkah 3) juga tidak
  bersih.
- **(a') di core loop bersama, harus bersih di single-stream**:
  **DITOLAK dalam bentuk murni** -- syarat "bersih di single-stream" tidak
  terpenuhi.
- **(b) kelemahan level single-stream, state-init tidak relevan**:
  **PALING DIDUKUNG** untuk keluarga `FPF-14+6/16`. Test family ini muncul
  di SEMUA EMPAT konfigurasi yang diuji -- interleaved v2 (2 seed set
  berbeda), interleaved addressable (init formula berbeda), DAN
  single-stream murni (tanpa interleaving sama sekali) -- dengan magnitude
  serupa (p berkisar 3.7e-8 sampai 4.6e-6, R=+6.6 sampai +9.2). Ini pola
  yang konsisten dengan kelemahan statistik di `ra_permutation_cycle`/
  `ra_reseed`/`ra_core` (core loop yang dipakai bersama v2 dan
  addressable) yang muncul di skala 1TB terlepas dari cross-stream mixing
  ATAU formula init -- **BUKAN** sesuatu yang diperbaiki lewat perbaikan
  state-init/avalanche seperti dugaan awal user di §2 handover.
- **(c) noise run tunggal**: **DITOLAK untuk `FPF-14+6/16`** (reproduksi 4x
  independen di atas terlalu konsisten untuk kebisingan multiple-testing
  murni), tapi **cocok untuk `BCFN(2+0,13-0,T)`** secara terpisah -- test
  ini muncul di 3 dari 4 run tapi hilang total di Langkah 2 (seed set
  berbeda, run paling baru), pola yang lebih konsisten dengan artefak
  marjinal/sensitif-seed daripada sinyal struktural yang solid seperti
  `FPF-14+6/16`.

**Kesimpulan praktis**: `FPF-14+6/16` adalah sinyal statistik nyata dan
reproducible di skala 1TB pada core loop bersama `winner_wired_v2`/
`winner_wired_addressable` -- bukan bug harness, bukan noise, dan bukan
spesifik ke satu formula init. Tapi juga bukan kegagalan fatal: dari 4 run
independen tidak ada satupun "FAIL", cuma `unusual`/`suspicious`/`very
suspicious` (PractRand's tier di bawah FAIL), dan seluruh checkpoint sampai
512GB gabungan tetap bersih di semua konfigurasi. Kalau paper butuh klaim
soal independensi/kualitas antar-stream, laporkan apa adanya: "cross-stream
independence dan kualitas single-stream tervalidasi bersih sampai 512GB
gabungan; checkpoint 1TB menunjukkan sinyal `FPF-14+6/16` yang reproducible
di core loop bersama (bukan spesifik cross-stream atau spesifik init),
dengan severity di bawah ambang FAIL." Perbaikan (kalau dianggap perlu)
berada di scope `ra_core`/`ra_permutation_cycle`, bukan di `ra_init_state*`
-- di luar cakupan riset addressable-init yang sudah selesai (Tahap 0-6).

**Catatan implementasi (lesson learned, Fase 2)**: percobaan pertama ke
128GB gagal dua kali secara berurutan sebelum berhasil, murni karena bug di
*harness*-nya (bukan di generator, yang tidak pernah diubah):
1. Skrip awal memakai `-tlmin <L> -tlmax <L>` dengan `L` sama persis —
   cocok sampai 16GB tapi PractRand gagal ("error reading standard input",
   tanpa baris hasil tes sama sekali) di 128GB. Deteksi pass/fail yang lama
   (`"FAIL"/"SUSPICIOUS" not in stdout`) salah melaporkan ini sebagai
   "PASSED" karena tidak memeriksa apakah baris hasil tes yang asli memang
   ada. Diperbaiki dengan pola rentang (`-tlmin 8GB -tlmax 128GB`, meniru
   konvensi yang sudah terbukti di `2026-8-27_operand-position-search/`) dan
   memperketat deteksi pass/fail agar mensyaratkan baris `"no anomalies in N
   test result(s)"` benar-benar ada.
2. Percobaan kedua (dengan fix di atas) sudah dapat checkpoint bersih sampai
   64GB, tapi tetap gagal tepat sebelum 128GB — total data yang terkirim ke
   PractRand kurang 8 word (dari ~34,4 miliar word) dari target persis.
   Diperbaiki dengan pola "over-supply": tiap stream generator diminta
   sedikit lebih banyak data dari kebutuhan, dan PractRand sendiri yang
   menghentikan pembacaan (`BrokenPipeError` di sisi penulis) begitu
   `-tlmax` tercapai — pola yang sama seperti yang sudah dipakai
   `other_winners_practrand.sh` di riset sebelumnya.
Kedua fix ini murni perbaikan alat ukur; binary yang diuji (`winner_wired_v2`)
tidak pernah disentuh.

### Pembanding: `ra_prng2.c` (paper-exact) vs `winner_wired_v2.c` (checkpoint, 2026-08-31)

Menjawab pertanyaan "Next steps (opsional)" lama: apakah pruning+wiring
(`operand-position-search`) mengubah independensi antar-stream dibanding
versi paper-exact asli? File baru `cross_correlation_ra_prng2.py` dan
`interleave_practrand_ra_prng2.py` (Method A dan B, tidak mengedit
`cross_correlation.py`/`interleave_practrand.py` in place -- reuse
`analyze_group()`/`run_interleave_practrand()` dengan `binary=RA_PRNG2_BIN`
di-repoint eksplisit, pola sama seperti
`2026-8-30_addressable-init-research/tahap3_cross_correlation.py`), memakai
`ensure_ra_prng2_cli()` yang sudah disiapkan di `common.py` untuk
meng-compile `src/ra_prng2/c/ra_prng2.c` (read-only, tidak pernah diedit)
ke binary lokal folder ini.

| Method | Tier | Hasil |
|---|---|---|
| A (cross-correlation) | smoke (K=8, n=200k) | 0/28 flagged (adjacent & control) |
| A (cross-correlation) | full (K=128, n=1M) | 0/8,128 flagged (adjacent & control) -- identik dengan hasil `winner_wired_v2` |
| B (interleaved PractRand) | smoke (K=4, 64MB) | bersih, 142 test, no anomalies |
| B (interleaved PractRand) | medium (K=8, 1GB) | bersih, 194 test, no anomalies |

**Kesimpulan**: `ra_prng2.c` menunjukkan pola independensi yang SAMA dengan
`winner_wired_v2.c` pada skala yang diuji (0 pasangan flagged, PractRand
bersih) -- memperkuat klaim bahwa pruning+wiring `winner_wired_v2.c` tidak
mengubah independensi antar-stream dibanding versi paper-exact. **Skala
terbatas**: tidak dijalankan sampai full/xlarge/1TB untuk `ra_prng2.c`
(di luar scope pertanyaan independence ini -- lihat catatan 1TB `winner_wired_v2`
di atas soal kenapa skala besar perlu hati-hati sebelum diklaim "clean").

### Validasi kualitas skala besar single-stream `ra_prng2.c` (2026-09-01) -- SELESAI, bersih total

Pertanyaan independence di atas sengaja dibatasi skala kecil (smoke/medium).
`ra_prng2.c` -- algoritma paper-exact asli, bukan `winner_wired_v2`/
`winner_wired_addressable` yang sedang dalam investigasi anomali 1TB (lihat
subseksi "Langkah 1, 2, dan sintesis Langkah 4" di atas) -- **belum pernah
diuji PractRand single-stream sama sekali di skala berapapun** sebelum sesi
ini. Dijalankan di VPS, paralel dengan Track A/Langkah 1/2 (idle-capacity
use), staged satu invocation `-tlmin 8GB -tlmax 1TB` (pola sama seperti
Langkah 1 `winner_wired_v2`):

```bash
stdbuf -oL ~/Documents/research/PractRand/RNG_test stdin32 -tlmin 8GB -tlmax 1TB \
  < <(./ra_prng2_cli --stream 0 274877906944) \
  > trackB_practrand_ra_prng2_1TB_singlestream.log 2>&1
```

| checkpoint | waktu (s) | hasil |
|---|---:|---|
| 8GB   | 234    | no anomalies in 230 test result(s) |
| 16GB  | 482    | no anomalies in 240 test result(s) |
| 32GB  | 969    | no anomalies in 251 test result(s) |
| 64GB  | 1,929  | no anomalies in 263 test result(s) |
| 128GB | 3,835  | no anomalies in 273 test result(s) |
| 256GB | 7,678  | no anomalies in 284 test result(s) |
| 512GB | 15,412 | no anomalies in 295 test result(s) |
| **1TB** | **30,782** | **no anomalies in 304 test result(s)** |

**Bersih total di semua 8 checkpoint, tidak ada satupun flag** (bukan cuma
di bawah ambang FAIL seperti `winner_wired_v2`/`winner_wired_addressable`
-- benar-benar nol anomali). Ini validasi kualitas skala besar PERTAMA
untuk `ra_prng2.c`, terpisah dari pertanyaan independence di atas yang
sudah settled sebelumnya (skala kecil).

**Catatan interpretasi**: hasil ini TIDAK secara langsung menyimpulkan
bahwa pruning+wiring `winner_wired_v2.c` "merusak" kualitas dibanding
`ra_prng2.c` -- baru satu run non-repeated per generator di skala 1TB,
dan `FPF-14+6/16` yang muncul di `winner_wired_v2`/`addressable` levelnya
`unusual`/`suspicious`, bukan `FAIL`, jadi masih dalam batas yang PractRand
sendiri anggap bisa jadi kebisingan multiple-testing pada satu run.
Menariknya, kontras ini konsisten dengan kemungkinan bahwa proses
pruning+wiring (`operand-position-search`) sedikit menggeser statistik
di skala sangat besar meski identik di skala kecil-menengah dan identik
soal independensi antar-stream -- **worth flagging as a paper caveat**,
tapi butuh run terulang (repeated, seed berbeda) di kedua generator untuk
klaim kausal yang solid, bukan hanya satu run masing-masing seperti di
sini. Raw log lengkap: `trackB_practrand_ra_prng2_1TB_singlestream.log`.

## 2. Probabilitas collision/overlap antar-stream (`collision_scan.py`)

Satu `dict` hash global lintas SEMUA seed (bukan per-seed) — collision
dilaporkan hanya kalau seed pemilik hash pertama BERBEDA dari seed yang
menghasilkan hash duplikat (collision dalam satu stream sudah dicover
eksperimen periodisitas terpisah, bukan cakupan di sini).

| Mode | Skala full | Unit diperiksa | Collision ditemukan | Probabilitas digest-collision murni (pembanding) |
|---|---|---|---|---|
| `prefix` (64-word fingerprint/seed) | M=10,000 seed | 10,000 fingerprint | **0** | ≈2.71×10⁻¹² |
| `blocksweep` (blok 255-word) | M=10,000 seed, V=250,000/seed | 9,800,000 blok | **0** | ≈2.60×10⁻⁶ |

**Kesimpulan Q2 (baseline sekuensial)**: nol collision terdeteksi di kedua
mode pada skala uji 10.000 seed / 9,8 juta blok. Karena probabilitas
digest-collision murni (blake2b 64-bit) di skala ini diabaikan (≈10⁻⁶
hingga 10⁻¹²), hasil nol-hit ini bukan artefak hash — ini batas bawah
confidence empiris untuk skala yang diuji: **tidak ada overlap antar-stream
terdeteksi di antara 10.000 seed pertama, masing-masing diperiksa sampai
250.000 nilai (≈1MB) pertama**. Ini bukan klaim atas seluruh ruang seed
2³², hanya atas skala yang benar-benar diuji di sini — lihat sub-bagian
berikut untuk validasi lanjutan yang menutup gap ini.

### Validasi lanjutan: seed acak dari ruang 2³² penuh

Skala sekuensial (`0..M-1`) di atas hanya menguji seed yang berdekatan.
`collision_scan.py --seeds random` menyampel seed via `common.random_seeds()`
(`random.Random(rng_seed).sample(range(2**32), m)`, tanpa replacement,
reproducible) — tidak berurutan, tersebar di seluruh ruang 2³², jauh lebih
representatif untuk skenario produksi (mis. worker terdistribusi yang
menurunkan seed dari hash job-id, bukan counter berurutan).

| Mode | Skala full (seed acak 2³²) | Unit diperiksa | Collision ditemukan | Probabilitas digest-collision murni (pembanding) |
|---|---|---|---|---|
| `prefix` | M=500.000 seed | 500.000 fingerprint | **0** | ≈6,78×10⁻⁹ |
| `blocksweep` | M=25.000 seed, V=250.000/seed | 24.500.000 blok | **0** | ≈1,63×10⁻⁵ |

Waktu tempuh gabungan (kedua mode, 4 worker paralel): 10 menit 25 detik —
50x lebih banyak seed (prefix) dan 2,5x lebih banyak seed (blocksweep)
dibanding baseline sekuensial, dari ruang 2³² penuh bukan `0..M-1`.

**Kesimpulan Q2 (validasi lanjutan)**: nol collision di 500.000 seed acak
(prefix) dan di 25.000 seed acak × 24,5 juta blok (blocksweep) dari seluruh
ruang 2³². Digabung dengan hasil sekuensial di atas, ini adalah bukti
empiris paling kuat yang dikumpulkan riset ini bahwa collision antar-stream
tidak teramati baik untuk seed berdekatan maupun seed tersebar acak, pada
skala uji gabungan >500.000 seed berbeda. Tetap bukan bukti matematis untuk
seluruh 2³² — lihat Keterbatasan.

**Catatan implementasi (lesson learned)**: paralelisasi awal memakai
`ProcessPoolExecutor` dengan pola map-reduce (tiap worker proses bangun
dict lokal, dikirim balik ke proses utama untuk di-merge). Ini bekerja di
skala sedang (M≤20.000) tapi **deadlock** di skala penuh (prefix
M=500.000/blocksweep M=25.000) — worker berhenti mengonsumsi CPU tanpa
pernah selesai, kemungkinan besar karena mem-pickle dict berukuran
ratusan-MB per worker lewat pipe IPC pool melebihi kapasitas praktis, atau
efek fork-safety dari memanggil `subprocess.Popen` di dalam worker pool di
bawah beban tinggi. Diganti ke `ThreadPoolExecutor` dengan dict bersama +
`threading.Lock` (tanpa serialisasi antar-proses sama sekali, memakai
memori bersama) — pola yang sama seperti konkurensi subprocess di Q3
(`wall_time_scaling.py`) yang sudah terbukti stabil. Setelah diganti, run
penuh selesai bersih tanpa hang.

## 3. Skalabilitas throughput (`wall_time_scaling.py` + `perf_scaling.sh`)

Dijalankan paling awal (mesin idle), sweep N proses `--stream` konkuren
(`n_per_proc=200,000,000`, seed `0..N-1`).

| N | Wall-time (s) | Throughput (val/s) | Speedup | Efficiency | Instructions/elemen |
|---|---|---|---|---|---|
| 1 | 2.111 | 94,758,304 | 1.000 | 1.000 | 176.51 |
| 2 | 2.047 | 195,389,428 | 2.062 | 1.031 | 176.50 |
| 3 | 3.162 | 189,751,331 | 2.002 | 0.667 | 176.50 |
| 4 | 4.409 | 181,448,350 | 1.915 | 0.479 | 176.50 |
| 8 | 9.163 | 174,609,651 | 1.843 | 0.230 | 176.50 |

**Kesimpulan Q3**: `instructions_per_element` **konstan** (176.50±0.01) di
semua N — bukti langsung tidak ada interferensi algoritmik antar-proses,
konsisten dengan desain state-per-proses tanpa lock/shared memory.
`efficiency` wall-time, sebaliknya, turun bertahap: ≈1.0 di N=1-2 (memetakan
ke 2 core fisik), turun ke 0.67 di N=3 dan 0.48 di N=4 (mulai memakai
hyperthreading + kontensi cache/memory-bandwidth bersama), turun lebih jauh
ke 0.23 di N=8 (oversubscription 2x di atas 4 logical CPU, overhead
scheduler OS). **Ini karakterisasi hardware yang jujur, bukan kegagalan
desain** — spesifik untuk laptop 2C4T ini, tidak boleh digeneralisasi ke
server multi-core besar tanpa re-test di sana. Kombinasi kedua metrik
menjawab pertanyaan riset dengan presisi: turunnya efficiency murni
hardware-level (memory bandwidth/cache/scheduling), bukan desain algoritma
generator yang cacat.

## 4. SIMD (opsional) — dikerjakan sebagai stretch goal time-boxed

**Status**: dikerjakan (Q1-Q3 selesai jauh lebih cepat dari estimasi
~25-35 menit, jadi waktu tersisa dipakai untuk mikrobenchmark ini,
~20 menit).

`simd_prototype.c` mengimplementasikan **hanya kernel aritmetika**
(`o`, `a`, `b`, `c`, `d`) untuk 8 lane/stream independen, dalam dua mode:
- `scalar`: loop 8x-unrolled, satu lane per iterasi inner-loop scalar.
- `simd`: AVX2 intrinsics, 8 lane sekaligus dalam satu register `__m256i`,
  layout `M` sebagai SoA (`M[256][8]`) supaya baca `M[(i+6)&0xFF]` jadi
  vector load kontigu (bukan gather) — indeks `i` sama untuk semua lane.

**Swap `L[i]↔L[d]` sengaja DIHILANGKAN** dari kedua mode: `d = c & 0xFF`
data-dependent per-lane, sehingga vektorisasinya butuh gather (untuk baca)
dan AVX2 **tidak punya instruksi scatter sama sekali** untuk tulisnya
(hanya AVX-512 punya). Ini bukan generator yang divalidasi correctness-nya
(tidak dicek terhadap `winner_wired_v2` sesungguhnya) — murni untuk
instruction-count bagian yang genuinely vectorizable secara terisolasi.

| Mode | Instructions (total) | Instructions/elemen/lane | Cycles (total) | Wall-time (3 run) |
|---|---|---|---|---|
| scalar (8x-unrolled) | 4,007,190,066 | 2.504 | 1,460,931,763 | 0.365s ±0.002 |
| simd (AVX2 intrinsics) | 4,803,268,460 | 3.002 | 1,555,076,776 | 0.419s ±0.002 |

**Temuan tak terduga (diverifikasi via `objdump`)**: mode `scalar` bukan
benar-benar scalar — GCC `-O3 -march=native` di CPU ini (AVX-512-capable)
**meng-auto-vectorize** loop 8-lane itu sendiri ke instruksi AVX-512VL
(`vpternlogq`, `vpaddd`, dll., dengan encoding EVEX prefix `62`), tanpa
intrinsics manual apa pun. Kode intrinsics AVX2 tulisan tangan (mode
`simd`) justru **20% lebih banyak instruksi dan ~15% lebih lambat**
dibanding auto-vectorization compiler — kemungkinan karena compiler
memakai `vpternlogq` (fusi 3-input boolean AVX-512VL, tidak tersedia di
AVX2) untuk memadatkan rangkaian XOR yang kode AVX2 manual harus lakukan
sebagai instruksi terpisah.

**Kesimpulan Q4**: pada mesin uji ini, menulis intrinsics SIMD manual untuk
kernel aritmetika TIDAK memberi keuntungan atas auto-vectorization compiler
— bahkan lebih lambat. Bagian yang genuinely vectorizable (aritmetika
`a,b,c,d`, baca `M` dengan indeks seragam) sudah ditangani compiler dengan
baik tanpa campur tangan manual. Swap `L[i]↔L[d]` (satu-satunya bagian yang
BUTUH kerja SIMD manual karena data-dependent per-lane) tetap tidak
diimplementasikan di sini — itulah bottleneck sesungguhnya untuk
vektorisasi lintas-stream penuh, dan analisis kelayakannya
(gather tersedia di AVX2, scatter hanya di AVX-512, downclocking AVX-512 di
mobile Tiger Lake) tetap seperti dianalisis di rencana riset: potensi
untung tidak pasti tanpa implementasi penuh + validasi PractRand ulang,
di luar scope time-box sesi ini.

**Perbandingan dengan Q3**: multi-proses (Q3) sudah memberi paralelisme
lintas-stream dengan korektnes gratis (generator asli dipakai apa adanya,
tidak perlu ditulis ulang) dan skalanya sudah terukur. SIMD lintas-stream
hanya berpotensi bermanfaat untuk use-case "banyak stream dari SATU core"
(throughput per-core), bukan pertanyaan utama HANDOVER (yang fokus
multi-core/multi-proses) — dan hasil di atas menunjukkan potensi itu pun
tidak otomatis terwujud tanpa kerja tambahan signifikan pada bagian
`L`-swap.

## Keterbatasan

- Q1 Method A hanya menangkap korelasi linear lag-0; Method B (PractRand
  interleaved) menutup sebagian dependensi nonlinear tapi tetap terbatas
  pada K=8 stream dan 16GB — bukan pembuktian formal independensi untuk
  semua kombinasi seed di ruang 2³².
- Q2 hanya menguji 10.000 seed pertama (berurutan dari 0) sampai ~1MB/seed
  — bukan sampling acak dari seluruh ruang seed 2³², dan bukan bukti
  matematis atas seluruh ruang tsb.
- Q3 (efficiency wall-time) spesifik untuk hardware 2C4T yang dipakai —
  hasil `instructions_per_element` konstan lebih general (murni algoritmik),
  tapi angka efficiency perlu re-test di hardware server multi-core besar
  sebelum digeneralisasi.
- Q4 adalah mikrobenchmark arsitektural terisolasi (kernel aritmetika saja,
  tanpa `L`-swap, tanpa reseed) — bukan implementasi produksi, hasilnya
  directional saja ("compiler auto-vectorization sudah cukup baik di sini"),
  bukan kesimpulan umum untuk semua compiler/hardware/kompilasi flag.

## Next steps

- ~~Kalau ingin validasi lebih dalam Q1: naikkan Method B ke 128GB/1TB~~ —
  **128GB (tier `xlarge`) dikerjakan di Fase 2, 1TB dikerjakan di checkpoint
  follow-up 2026-08-31** — lihat tabel Method B "1TB" di atas. **Bukan hasil
  bersih**: 3 test flagged (suspicious/very suspicious) di checkpoint akhir
  1TB, belum dikonfirmasi ulang (1 run saja). Follow-up yang benar-benar
  masih terbuka: jalankan ulang 2-3x independent run ke 1TB untuk
  konfirmasi apakah flag ini persisten (real) atau kebisingan run tunggal,
  sebelum menulis klaim "clean to 1TB" di paper.
- ~~Kalau ingin validasi lebih dalam Q2: sampling seed acak dari seluruh
  ruang 2³²~~ — **dikerjakan di Fase 2** (prefix M=500.000, blocksweep
  M=25.000×V=250.000 dari `common.random_seeds()`), lihat sub-bagian
  "Validasi lanjutan" di bagian Q2. Kalau ingin naik lebih jauh lagi:
  perbesar M, atau uji beberapa `rng_seed` berbeda untuk `random_seeds()`
  supaya tidak bergantung pada satu sampel acak tetap.
- Kalau ingin serius mengejar SIMD lintas-stream: implementasikan swap
  `L[i]↔L[d]` via AVX-512 gather/scatter (`_mm512_i32gather_epi32`/
  `_mm512_i32scatter_epi32`) dan validasi ulang lewat PractRand — bagian
  yang belum disentuh di sesi ini karena time-box.
- ~~Perbandingan dengan `src/ra_prng2/c/ra_prng2.c` (versi paper-exact)~~ —
  **dikerjakan di checkpoint 2026-08-31**, lihat sub-bagian "Pembanding:
  `ra_prng2.c`..." di bagian Q1 di atas. Hasil: independensi setara
  `winner_wired_v2.c` (0 flagged, PractRand bersih) sampai skala smoke/
  medium/full (belum full/xlarge/1TB — di luar scope pertanyaan ini).
- **BARU (checkpoint 2026-08-31)**: follow-up diagnostik untuk anomali 1TB
  di atas (single-stream `winner_wired_v2` ke 1TB + re-run interleaved
  dengan seed set berbeda) — lihat `HANDOVER_1TB_FOLLOWUP.md` untuk rencana
  lengkap, TIDAK dikerjakan sesi ini atas permintaan user (~7-8 jam,
  didelegasikan ke sesi berikutnya).
