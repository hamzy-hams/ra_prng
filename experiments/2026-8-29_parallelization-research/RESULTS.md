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
`RNG_test stdin32 -tlmin <L> -tlmax <L>`. Tahapan berurutan dengan
checkpoint (semua tier harus bersih sebelum naik):

| Tier | K | Total data | Waktu | Hasil |
|---|---|---|---|---|
| smoke | 4 | 64MB | 0.7s | bersih, 142 test, no anomalies |
| medium | 8 | 1GB | 11.7s | bersih, 194 test, no anomalies |
| **full** | **8** | **16GB** | **196s (~3.3 menit)** | **bersih, 240 test, no anomalies** |

**Kesimpulan Q1 Method B**: interleave 8 stream (seed 0-7) sampai 16GB
gabungan tidak menghasilkan satu pun FAIL/SUSPICIOUS di PractRand (240 test
result). Kalau stream-stream ini saling berkorelasi, menggabungkannya
sebagai "satu RNG besar" seharusnya membuat PractRand mendeteksi pola jauh
lebih cepat dibanding baseline single-stream 128GB-clean yang sudah
diverifikasi di eksperimen 2026-8-27 — tidak terjadi di sini. Ini bukti
tambahan (menangkap dependensi nonlinear, tidak seperti Method A) yang
konsisten dengan hasil Method A: tidak ada sinyal dependensi antar-stream
pada skala uji ini.

**Catatan skala**: 16GB dianggap cukup informatif untuk keputusan go/no-go
riset paralelisasi ini (bukan validasi produksi final skala 128GB/1TB —
itu follow-up terpisah kalau ada sinyal mencurigakan, yang tidak ada di
sini).

## 2. Probabilitas collision/overlap antar-stream (`collision_scan.py`)

Satu `dict` hash global lintas SEMUA seed (bukan per-seed) — collision
dilaporkan hanya kalau seed pemilik hash pertama BERBEDA dari seed yang
menghasilkan hash duplikat (collision dalam satu stream sudah dicover
eksperimen periodisitas terpisah, bukan cakupan di sini).

| Mode | Skala full | Unit diperiksa | Collision ditemukan | Probabilitas digest-collision murni (pembanding) |
|---|---|---|---|---|
| `prefix` (64-word fingerprint/seed) | M=10,000 seed | 10,000 fingerprint | **0** | ≈2.71×10⁻¹² |
| `blocksweep` (blok 255-word) | M=10,000 seed, V=250,000/seed | 9,800,000 blok | **0** | ≈2.60×10⁻⁶ |

**Kesimpulan Q2**: nol collision terdeteksi di kedua mode pada skala uji
10.000 seed / 9,8 juta blok. Karena probabilitas digest-collision murni
(blake2b 64-bit) di skala ini diabaikan (≈10⁻⁶ hingga 10⁻¹²), hasil nol-hit
ini bukan artefak hash — ini batas bawah confidence empiris untuk skala
yang diuji: **tidak ada overlap antar-stream terdeteksi di antara 10.000
seed pertama, masing-masing diperiksa sampai 250.000 nilai (≈1MB) pertama**.
Ini bukan klaim atas seluruh ruang seed 2³², hanya atas skala yang benar-
benar diuji di sini.

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

- Kalau ingin validasi lebih dalam Q1: naikkan Method B ke 128GB/1TB
  (seperti validasi single-stream 2026-8-27), atau perbesar K di Method A
  untuk cakupan pasangan lebih luas.
- Kalau ingin validasi lebih dalam Q2: sampling seed acak dari seluruh
  ruang 2³² (bukan berurutan dari 0), atau naikkan M/V lebih jauh dengan
  paralelisasi hashing lintas-core (`multiprocessing.Pool`, disebut sebagai
  opsi di rencana riset).
- Kalau ingin serius mengejar SIMD lintas-stream: implementasikan swap
  `L[i]↔L[d]` via AVX-512 gather/scatter (`_mm512_i32gather_epi32`/
  `_mm512_i32scatter_epi32`) dan validasi ulang lewat PractRand — bagian
  yang belum disentuh di sesi ini karena time-box.
- (Opsional, tidak dikerjakan) Perbandingan dengan `src/ra_prng2/c/ra_prng2.c`
  (versi paper-exact) untuk melihat apakah pruning+wiring di
  `winner_wired_v2.c` mengubah perilaku independensi antar-stream — helper
  `ensure_ra_prng2_cli()` di `common.py` sudah disiapkan untuk ini kalau
  dibutuhkan di sesi berikutnya.
