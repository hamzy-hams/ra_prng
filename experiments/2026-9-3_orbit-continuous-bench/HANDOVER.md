# HANDOVER: Continuous-stream real benchmark untuk `ra_core_orbit`

Status: **executed, 2026-09-03.**

## Konteks

Setelah `../2026-9-3_combo-winner-pareto-selection/RESULTS.md` (Pareto
frontier `w8_f10_i0`/`w8_f28_i0` untuk core `singleblock`), user bertanya
kenapa core `orbit` (varian addressable/unbounded di `ra_core.c`) tidak ikut
diuji speed-nya. Usul awal: cukup ukur K=1 dan K=255 untuk orbit lalu
asumsikan continuous-stream-nya "di tengah" sehingga tidak perlu diuji
langsung.

Riset (sebelum implementasi) menemukan asumsi itu terbantahkan oleh data
`singleblock` yang sudah ada: continuous-stream real singleblock (~121-155
MB/s, session lama) jauh lebih dekat ke ujung K=1 (~109 MB/s ekuivalen)
daripada ke K=255 yang dikonversi aljabar (~20.000-30.000 MB/s) — beda 2
orde magnitudo, karena overhead syscall/fwrite per-blok-reinit dominan dan
tidak tertangkap microbench K=255.

Scope yang disepakati user (lebih sempit dari draft awal): **bangun
benchmark continuous-stream REAL untuk `orbit` saja**. `singleblock` K=1 dan
K=255 sudah ada (dari `../2026-9-3_combo-winner-pareto-selection/RESULTS.md`
Axis A table) — dikutip, tidak diukur ulang. Tujuan: posisikan kecepatan
ra_prng (orbit + singleblock) dibandingkan algoritma lain (philox, xoshiro,
pcg32, mt19937, chacha20, splitmix64, paperorig, wiredv2, addrcont) yang
sudah ada di `../2026-8-31_prng-family-benchmark/RESULTS.md`.

**Non-goal:** tidak mengubah `ra_core.c` produksi. Tidak ada promosi apa pun
ke produksi. Tidak mengukur ulang K=1/K=255 singleblock atau ke-11 kandidat
combo-winner. Tidak menguji K=1/K=255 untuk orbit sendiri.

## Rencana implementasi (plan yang disetujui)

1. Folder baru: `experiments/2026-9-3_orbit-continuous-bench/`.
2. `orbit_bench.c` — harness minimal, **extern-link** ke `ra_core_orbit`
   (pola sudah terverifikasi jalan di
   `../2026-9-1_family-productionization/bench_ra_core.c`: compile 2-tahap,
   `-Dmain=ra_core_unused_main -c ra_core.c -o ra_core_nomain.o`, lalu link).
   Mode `--stream` (KAT) dan `--multistream` (continuous-stream real)
   di-port dari `../2026-9-3_combo-winner-pareto-selection/winners_bench.c`.
   Flag compile identik: `-O3 -march=native -std=gnu17 -include stdalign.h`.
3. Verifikasi KAT sebelum timing run: byte-identik vs binary produksi
   `../2026-9-1_family-productionization/ra_core --stream orbit`, plus KAT
   tetap untuk beberapa `(key,n)`.
4. Ukur `orbit_bench --multistream orbit 111222 1000000000 255` → `/dev/null`
   (parameter identik `winners_bench_run.py`), 2x run, ambil mean.
5. Posisikan hasil terhadap `singleblock` (dikutip) dan keluarga PRNG lain
   (dikutip, dengan disclaimer lintas-sesi).
6. Tidak ada re-run seed untuk "menjelaskan" hasil FAIL/anomali.
7. `/graphify --update` setelah selesai.

## Deviasi dari rencana (didokumentasikan)

Saat run pertama, angka orbit continuous-stream (376.2 MB/s) jauh melampaui
angka `singleblock` lama (154.8 MB/s) — padahal struktur `orbit` (ada swap
`L[]` ekstra per iterasi) seharusnya membuatnya *sedikit lebih lambat*, bukan
2.4x lebih cepat, dibanding `singleblock`. Ini secara struktural tidak masuk
akal sebagai temuan asli. Sebelum menulis hasil, dilakukan **satu sanity-check
tambahan** (di luar scope semula, tapi murah — binary `winners_bench` yang
sudah ada, tidak dimodifikasi, cuma dijalankan sekali lagi): jalankan
`winners_bench --multistream singleblock ...` dengan parameter sama, di
mesin yang sama, sekarang. Hasilnya 387.6 MB/s — mengonfirmasi ini murni
efek mesin/sesi (mesin sekarang ~2.4x lebih cepat dari sesi
`combo-winner-pareto-selection` yang lama), bukan orbit yang secara asli
lebih cepat dari singleblock. Lihat RESULTS.md untuk detail. Ini BUKAN
re-benchmark penuh `singleblock` (cuma 1 titik data tambahan untuk validasi
konsistensi), jadi tidak melanggar batasan scope "tidak mengukur ulang
singleblock".

## Deliverable

- `orbit_bench.c`, `ra_core_nomain.o`, `orbit_bench` (binary)
- `verify_orbit_kat.py` — 10/10 checks pass, 0 mismatch
- `orbit_multistream_run.py`, `orbit_multistream_results.jsonl`
- `RESULTS.md`
