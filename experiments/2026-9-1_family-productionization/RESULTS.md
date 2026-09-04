# RESULTS: Tahap 1 — Unifikasi `ra_core.c` (orbit + singleblock)

## Status

**Tahap 1 SELESAI** (2026-09-01). Track ini sebelumnya PAUSED
(`HANDOVER.md`) menunggu penamaan diluruskan — semua open decision di
HANDOVER §4 sudah dikonfirmasi user lewat diskusi (bukan diasumsikan
sepihak), sebelum satu baris kode pun ditulis.

## Keputusan penamaan & scope (dikonfirmasi user, sesi ini)

1. **Anchor nama**: `ra_core`, buang label "winner_wired" sepenuhnya (itu
   cuma nama file, artefak "pemenang survei wiring", bukan identitas
   algoritma — fungsi ekspornya sendiri sudah konsisten `ra_core` di semua
   varian sebelumnya: `src/ra_prng2`, `winner_wired.c`, `winner_wired_v2.c`,
   `winner_wired_addressable.c`).
2. **Tanpa suffix versi** — ini varian produksi/default, bukan salah satu
   dari beberapa iterasi bertanggal.
3. **Bentuk API**: dua fungsi publik terpisah (`ra_core_orbit` /
   `ra_core_singleblock`) memanggil helper statis bersama, bukan satu fungsi
   dengan parameter mode.
4. **Hanya 2 mode, bukan 3**: mode "stream kontinu non-addressable"
   (`winner_wired_v2.c`'s `ra_init_state`, fixed tanpa key sama sekali)
   **sengaja tidak dibawa** ke `ra_core.c`. Stream kontinu cukup dilayani
   mode orbit dengan satu key/address tetap. Precedent implisit sudah ada
   di `tahap6_bench.c`'s `CORES[]` (cuma `baseline`+`singleblock`, tidak ada
   entry fixed-init ketiga).
5. **Nama mode**: `orbit` (addressable, unbounded) dan `singleblock` (≤255
   kata/init, tanpa `L[]`).
   **Catatan kebijakan**: memory `project_ra_prng_terminology.md`
   sebelumnya mencatat "Orbit Addressing" sebagai istilah dokumentasi/paper
   SAJA, eksplisit bukan untuk rename kode (keputusan 2026-08-30). Sesi ini
   secara sadar membalik itu atas permintaan eksplisit user — `orbit`
   sekarang identifier kode resmi. Memory terkait sudah diperbarui.
6. **`ADDR_L_MIX_CONST` dibuang** — nilainya (`0x9e3779b7u`) identik dengan
   literal yang sudah dipakai di baris yang sama; ditulis sebagai literal
   langsung, tanpa macro.

Tabel penamaan lengkap ada di `ra_core.c`'s header comment.

## Struktur `ra_core.c`

- Shared tanpa suffix mode: `rot32`, `ra_hash`, `ra_reseed`.
- Mode orbit: `ra_init_state_orbit`, `ra_permutation_cycle_orbit`,
  `ra_core_orbit(key, rng, raw_stream)` — sama persis dengan
  `winner_wired_addressable.c`'s formula addressable (Tahap 1
  addressable-init, Kandidat 5).
- Mode singleblock: `ra_init_state_singleblock`,
  `ra_permutation_cycle_singleblock`, `ra_core_singleblock(key, rng,
  raw_stream)` — sama persis dengan `tahap6_bench.c`'s no-`L` fast path,
  termasuk hard guard `rng>255` (fprintf + abort).
- CLI: `--stream <orbit|singleblock> <key> <n>`, `validate`. Infrastruktur
  benchmark (throughput/reinit-sweep/Philox) `tahap6_bench.c` sengaja tidak
  dibawa — itu scope Tahap 2 (battery validasi produksi), bukan Tahap 1.

## Validasi

1. **`./ra_core validate`** (in-process, `fmemopen`, pola
   `tahap6_bench.c`'s `run_validate_singleblock`): `ra_core_orbit` vs
   `ra_core_singleblock` untuk 39 key (`{0, 0xFFFFFFFF, 5 extra, 0..31}`) ×
   255 panjang `rng` (1..255) = 9,945 kombinasi.

   ```
   validate: 9945 combinations checked (39 keys x 255 lengths), 0 mismatches
   validate: PASS -- ra_core_singleblock is bit-identical to ra_core_orbit
   for all rng in [1,255] across all tested keys.
   ```

2. **`verify_unification.py`** (skrip baru, folder ini — compile
   `winner_wired_addressable.c` yang lama read-only, bandingkan byte mentah
   `--stream` lama vs `--stream orbit` baru, 8 key × 8 panjang termasuk
   `n<255`, `n==255`, dan `n` besar yang melewati beberapa siklus reseed):

   ```
   verify_unification: 64 combinations checked (8 keys x 8 lengths), 0 mismatches
   verify_unification: PASS -- ra_core_orbit is bit-identical to
   winner_wired_addressable.c's ra_core for all tested (key, n).
   ```

3. **Hard guard singleblock** — `./ra_core --stream singleblock 42 256`
   ditolak dengan pesan jelas (`n=256 exceeds core 'singleblock'
   max_rng=255`), exit code 1. Guard `rng>255` di dalam
   `ra_core_singleblock` sendiri (fprintf+abort) tetap ada sebagai
   pengaman untuk pemanggil non-CLI.

**Kedua bukti bit-identical PASS 100%.** Karena `ra_core_orbit` bit-identical
terhadap `winner_wired_addressable.c` yang sudah tervalidasi luas (128GB
PractRand clean, 0 collision-scan, 0 cross-correlation, dieharder 26/27
PASSED — lihat `experiments/2026-9-1_dieharder-battery/RESULTS.md`), seluruh
jaminan statistik itu otomatis terwarisi untuk `ra_core_orbit` — **tidak
perlu diulang** di tahap ini.

## Catatan scope eksplisit

`ra_core_orbit` **TIDAK** diuji dan **TIDAK** diharapkan bit-identical
terhadap `winner_wired_v2.c`'s mode fixed-init lama (`ra_init_state` tanpa
key) — mode itu sengaja tidak dibawa ke `ra_core.c` (keputusan 2-mode: orbit
+ singleblock saja, lihat §4 di atas). Jangan berasumsi `ra_core_orbit(key=0,
...)` mereproduksi output `winner_wired_v2.c` — formulanya beda (orbit selalu
mencampur key lewat `rot32`, bahkan saat key=0 hasilnya beda dari init fixed
`winner_wired_v2.c`).

## Non-goals (tidak dikerjakan sesi ini, sesuai HANDOVER §3)

- Tidak mengubah formula/recurrence apa pun — murni rename/merge, dibuktikan
  lewat validasi bit-identical di atas.
- Tidak menyentuh `winner_wired_v2.c`, `winner_wired_addressable.c`,
  `winner_wired_addressable_v2.c`, atau `tahap6_bench.c` in-place.
- Tidak mulai Tahap 2 (folder/battery validasi produksi).
- Tidak mempromosikan apa pun ke `src/` (open decision #3 HANDOVER §4 tetap
  ditangguhkan).
- Tidak mengerjakan item `project_research_backlog_2026_09` lain, tidak
  menyentuh proses 1TB PractRand yang masih jalan di VPS.

## File yang dihasilkan sesi ini

- `ra_core.c` — implementasi unified core (orbit + singleblock).
- `verify_unification.py` — skrip cross-check bit-identical vs
  `winner_wired_addressable.c` (reusable, tidak mengedit source lama).
- `ra_core`, `winner_wired_addressable_ref` — binary hasil compile (untuk
  reproduksi lokal; tidak masuk git kalau `.gitignore` mengecualikan
  binary).

## Variable-length M[] init untuk `ra_core_singleblock` (2026-09-04)

User mengamati bahwa `ra_core_singleblock` di K=1 hanya menghasilkan 1 word
output, sehingga sebagian besar dari 256 entri `M[]` yang di-init tidak
pernah dibaca. Analisis kode (`ra_permutation_cycle_singleblock` mengindeks
`M[]` HANYA lewat loop-variable `i`, tidak pernah lewat `d`/output-derived
byte) membuktikan jendela indeks yang dibaca untuk `rng`=K putaran selalu
`min(K+7, 256)` entri kontigu (wrap-around), diprediksi murni dari K —
sehingga `ra_init_state_singleblock` di-ubah untuk hanya meng-init rentang
itu, bukan 256 entri penuh.

**Korektnes**: `./ra_core validate` tetap 0 mismatch (39 key × n=1..255)
terhadap `SINGLEBLOCK_KAT_CHECKSUMS` — bit-exact dengan versi init penuh.
Sanity check independen: differential test `M[]` versi penuh vs
variable-length pada 7 nilai K × 5 key (3095 entri) — 0 mismatch.

**Percobaan pertama (masked wrap, `(start+j)&0xFF` per iterasi) REGRESI di
K besar**, bukan strict win seperti dugaan awal (op-count ratio hanya
proxy, bukan jaminan wall-clock):

| K | before (ns/word) | v1 masked-wrap (ns/word) | v1 speedup |
|---|---|---|---|
| 1 | 34.79 | 7.60 | 4.58x |
| 8 | 4.63 | 1.67 | 2.78x |
| 32 | 1.27 | 0.97 | 1.31x |
| 96 | 0.70 | 0.76 | **0.92x (lebih lambat)** |
| 200 | 0.47 | 0.76 | **0.61x (lebih lambat)** |
| 255 | 0.42 | 0.65 | **0.65x (lebih lambat)** |

Penyebab: mask `&0xFF` per-iterasi pada indeks wrap-around mematikan
auto-vectorization yang dinikmati loop lama (`for(i=0;i<256;i++)`, indeks
naik polos). **Fix**: pecah jadi hingga 2 sub-loop non-wrap (indeks naik
polos `[start..255]` lalu `[0..sisa]` kalau wrap), tiap sub-loop
tervektorisasi sama seperti loop asli. Hasil setelah fix (rata-rata 2 run,
`bench_ra_core.c reinit-sweep singleblock 20000000 <K>`):

| K | before (ns/word) | after split-loop (ns/word) | speedup nyata | rasio op-count teoritis |
|---|---|---|---|---|
| 1 | 34.79 | 10.65 | ~3.3x | 32.0x |
| 8 | 4.78 | 1.65 | ~2.9x | 17.1x |
| 32 | 1.28 | 0.88 | ~1.5x | 6.6x |
| 96 | 0.74 | 0.63 | ~1.2x | 2.5x |
| 200 | 0.47 | 0.48 | ~1.0x (impas) | 1.24x |
| 255 | 0.42 | 0.44 | ~0.97x (impas) | 1.0x |

Speedup nyata jauh di bawah rasio op-count teoritis di K kecil (overhead
tetap per-panggilan seperti alokasi stack `M[256]`/pemanggilan fungsi tidak
ikut menyusut), tapi tidak ada regresi di K besar (selaras titik impas
teoritis di K≈249-255) — checksum identik di semua K yang diuji, konsisten
dengan hasil `./ra_core validate`. Kode: `ra_init_state_singleblock` di
`ra_core.c` (baris ~264-296), dipanggil dengan parameter `rng` tambahan dari
`ra_core_singleblock`.

## Release benchmark (`bench_release.c`, 2026-09-04)

Benchmark resmi untuk README, dijalankan setelah promosi rolling-`o` (kedua
core) + variable-length M[] init (singleblock) ke `ra_core.c` kanonik
(§2b `PRODUCTION_READINESS_HANDOVER.md`). File baru
`bench_release.c` (folder ini) — extern-link ke `ra_core_orbit`/
`ra_core_singleblock` untuk angka real-throughput (tidak menyentuh
`ra_core.c`), plus verbatim-copy `ra_init_state_orbit`/
`ra_init_state_singleblock` (keduanya `static`) untuk isolasi waktu init
murni. Mesin: Intel Core i3-1115G4, `taskset -c 0`, GCC 16.1.1
`-O3 -march=native`, min-of-5..7-trial per angka. 5 mode standar
(`feedback_orbit_singleblock_benchmark_standard`) + Philox4x32-10 +
Xoshiro256\*\* (byte-for-byte copy dari implementasi KAT-verified yang
sudah ada di `bench_ra_core.c`/`../2026-9-3_init-loop-optimization/diag_init_candidates.c`).

| Mode | Init (ns) | Generate/word (ns/word) | Real ns/word | Real MB/s |
|---|---:|---:|---:|---:|
| singleblock K=1 | 0.25 | 10.02 | 10.27 | 371.6 |
| singleblock K=255 | 31.69 | 0.33 | 0.457 | 8351.1 |
| orbit --multistream K=1 | 48.54 | 8.64 | 57.18 | 66.7 |
| orbit --multistream K=255 | 48.54 | 2.74 | 2.93 | 1300.0 |
| orbit --stream (N=20M) | 48.54 | 2.80 | 2.80 | 1362.4 |
| philox4x32-10 K=1 | 1.87 | 10.40 | 12.27 | 311.0 |
| philox4x32-10 K=255 | 1.87 | 2.77 | 2.78 | 1374.4 |
| philox4x32-10 stream | 1.87 | 2.77 | 2.77 | 1378.3 |
| xoshiro256\*\* K=1 | 1.79 | 4.78 | 6.58 | 580.2 |
| xoshiro256\*\* K=255 | 1.79 | 1.03 | 1.04 | 3673.5 |
| xoshiro256\*\* stream | 1.79 | 0.97 | 0.97 | 3934.9 |

Metodologi kolom "generate/word": derivasi `(real_total_time(K) −
init_time(K)) / K`, dihitung TERPISAH per K untuk singleblock (bukan
formula 1-titik konstan) karena init singleblock sekarang variable-length
(K=1 hanya init ~8 entri M[], K=255 hampir 256 penuh) — kalau dipakai
formula konstan, gain variable-length-init akan salah teratribusi ke kolom
generate. Untuk orbit, init konstan (selalu L[256]+M[256] penuh, tidak
disentuh optimasi variable-length) sehingga K=1/K=255 saling konsisten
sebagai cross-check internal (diverifikasi lewat aljabar
`real(K)*K = init + K*gen` untuk kedua K, hasil identik).

**Cross-check terhadap RESULTS.md sendiri (PASS)**: singleblock K=1/K=255
real ns/word (10.27 / 0.457) di sini cocok dekat dengan tabel
"variable-length M[] init" di atas (10.65 / 0.44, run independen, mesin
sama tapi tanpa `taskset` pinning) — beda kecil konsisten dengan
run-to-run/CPU-frequency-scaling variance, bukan indikasi bug harness baru.

**Interpretasi**: singleblock K=255 6.1x lebih cepat dari Philox, 2.27x
dari Xoshiro256\*\* (makin unggul dari klaim lama 4.2x/1.5x pre-optimasi
kemarin). Temuan baru: optimasi variable-length M[] init membalik posisi
K=1 vs Philox — singleblock K=1 sekarang 1.19x lebih cepat dari Philox
(dulu kalah), meski masih 1.56x lebih lambat dari Xoshiro256\*\* di K=1.
orbit K=1 masih berat (init tidak variable-length, ~4.7x lebih lambat dari
Philox) karena orbit didesain untuk stream panjang kontinu, bukan pola
reinit-tiap-panggilan K kecil — gunakan singleblock untuk pola itu (dalam
batas 255 kata/key). orbit K=255/stream sekarang mendekati impas dengan
Philox (beda arah dari pengukuran lama yang kurang terkontrol, yang
mengklaim orbit menang), dan tetap kalah ~2.8x dari Xoshiro256\*\*.

**Caveat cakupan klaim**: angka di atas murni throughput CPU single-core.
Belum ada implementasi GPU/SIMD untuk `ra_core` (kandang Philox untuk pola
pemakaian addressable/counter-based yang sama, mis. gaya
`jax.random.split`), dan validasi statistik sejauh ini mencakup kualitas
output single-key, bukan independensi cross-stream skala besar untuk pola
addressable/multikey secara spesifik. Diangkat dari diskusi user
2026-09-04 ("kalau memang nggak ada [workload nyata untuk K=255], berarti
ra_prng belum unggul dimanapun kecuali kecepatan mentah") — jawabannya:
polanya nyata (kelas counter-based/stateless-splittable RNG, sama seperti
yang dilayani Philox), tapi keunggulan yang sudah terverifikasi memang
murni kecepatan mentah, bukan klaim lain yang belum dibuktikan.
