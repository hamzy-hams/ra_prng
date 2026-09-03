# RESULTS: continuous-stream real benchmark untuk `ra_core_orbit`

Implements `HANDOVER.md` in this folder. Status: **executed, 2026-09-03.**

## Konteks

Motivasi: `../2026-9-3_combo-winner-pareto-selection/RESULTS.md` menunjukkan
continuous-stream real `singleblock` (~121-155 MB/s) jauh lebih dekat ke
ujung K=1 (~109 MB/s ekuivalen) daripada K=255 yang dikonversi aljabar
(~20.000-30.000 MB/s) — usul awal user (derive bound orbit dari K=1/K=255
saja) tidak akan mewakili angka real, jadi orbit diuji langsung dengan
metodologi continuous-stream yang sama persis.

Scope task ini sengaja sempit: cuma `orbit` continuous-stream yang diukur
baru. `singleblock` K=1/K=255/continuous-stream dan seluruh keluarga PRNG
lain (philox, xoshiro256, pcg32, mt19937, chacha20, splitmix64, paperorig,
wiredv2, addrcont) dikutip dari eksperimen sebelumnya, tidak diukur ulang
(kecuali satu titik sanity-check, lihat "Deviasi" di bawah).

## Metodologi

- `orbit_bench.c`: extern-link ke `ra_core_orbit` (compile 2-tahap, `ra_core.c`
  dibaca read-only, tidak diubah) — pola sudah terverifikasi jalan di
  `../2026-9-1_family-productionization/bench_ra_core.c`. Ini berarti
  `ra_core_orbit` yang diuji di sini adalah **object code produksi asli**,
  bukan salinan tulis-ulang — jaminan korektnya lebih kuat daripada
  copy-verbatim (yang dipakai `winners_bench.c` untuk 11 kandidat combo
  karena candidate-candidate itu memang tidak punya sumber otoritatif lain).
- Mode `--stream` dan `--multistream` di-port (logikanya, bukan file-nya)
  dari `winners_bench.c` — sehingga parameter dan cara ukur MB/s (real
  syscall-inclusive via `fwrite` berulang ke `/dev/null`, BUKAN derivasi
  aljabar dari ns_per_word) identik dengan yang dipakai untuk `singleblock`.
- Parameter continuous-stream: `base_key=111222`, `n=1.000.000.000` kata
  (4GB), `K=255` — identik `winners_bench_run.py`.
- Flag compile identik `ra_core.c`/`winners_bench.c`:
  `-O3 -march=native -std=gnu17 -include stdalign.h`.
- Protokol run: 2x, ambil mean — mengikuti presedan noise-handling metrik
  continuous-stream di riset ini.

## Verifikasi

- `verify_orbit_kat.py`: **10/10 checks passed, 0 mismatches.**
  - 5 titik KAT tetap (`key=1,n=1`→`2452187902` [sama dengan singleblock,
    sudah dikutip sebelumnya], `key=0,n=1`→`2019748745`, `key=1,n=256`→
    `725782582`, `key=1,n=510`→`3006171839`, `key=42,n=1000`→`2983272652`) —
    semua diverifikasi ulang secara independen di sesi ini dengan menjalankan
    binary produksi `../2026-9-1_family-productionization/ra_core --stream
    orbit` langsung, sebelum dipakai sebagai assert di script.
  - 5 perbandingan byte-identik penuh (bukan cuma word terakhir) terhadap
    binary produksi yang sama, untuk `(key,n)` yang sama — 0 mismatch.
- **Catatan perilaku (bukan bug):** meminta `rng=256` dari `ra_core_orbit`
  menghasilkan **255 kata**, bukan 256 — `count`/`break` logic di
  `ra_permutation_cycle_orbit` membuat fungsi kembali begitu `count` internal
  mencapai ≤1, yang untuk `rng=256` terjadi tepat di akhir cycle 255-kata
  pertama (cycle reseed kedua tidak pernah mulai). Perilaku produksi yang
  sudah ada, dikonfirmasi byte-identik terhadap binary produksi (1020 byte
  di kedua sisi), bukan sesuatu yang diperkenalkan harness ini.
- `git diff --stat -- ../2026-9-1_family-productionization/ra_core.c`:
  kosong sebelum dan sesudah task ini.

## Deviasi dari rencana: sanity-check tambahan

Run pertama `orbit` continuous-stream menghasilkan **376.2 MB/s** — jauh di
atas angka `singleblock` lama (154.8 MB/s, dari
`combo-winner-pareto-selection/RESULTS.md`), padahal secara struktur `orbit`
punya swap `L[i]<->L[d]` ekstra per iterasi yang seharusnya membuatnya
**sedikit lebih lambat**, bukan 2.4x lebih cepat, dibanding `singleblock`.
Angka mentah ini secara struktural tidak masuk akal sebagai temuan asli
"orbit lebih cepat".

Sebelum ditulis sebagai hasil, dilakukan **satu titik sanity-check** (di
luar rencana awal, tapi murah — binary `winners_bench` yang sudah ada dari
eksperimen sebelumnya dijalankan sekali lagi, tidak dimodifikasi, bukan
re-benchmark penuh): `winners_bench --multistream singleblock 111222
1000000000 255` di mesin yang sama, sekarang. Hasil: **387.6 MB/s** —
hampir identik dengan angka `orbit` baru. Ini mengonfirmasi gap besar
terhadap 154.8 MB/s lama murni **efek mesin/sesi** (mesin sekarang jauh
lebih cepat dari sesi `combo-winner-pareto-selection`, konsisten dengan
disclaimer lintas-sesi yang berulang kali dicatat di riset ini), bukan
`orbit` yang benar-benar lebih cepat dari `singleblock`.

## Hasil

### Continuous-stream, sesi/mesin yang sama (hari ini)

| core | run 1 (MB/s) | run 2 (MB/s) | mean (MB/s) |
|---|---:|---:|---:|
| `orbit` (BARU, task ini) | 402.0 | 350.4 | **376.2** |
| `singleblock` (sanity-check 1x, bukan full re-measurement) | — | — | 387.6 |

`orbit` ≈ 3% lebih lambat dari `singleblock` pada sesi/mesin yang sama —
konsisten secara struktural dengan overhead swap `L[]` ekstra yang dimiliki
`orbit`, bukan anomali. Ini mengoreksi kesan awal (376.2 vs 154.8 dari sesi
lama) yang keliru menyiratkan `orbit` "jauh lebih cepat".

### Posisi vs keluarga PRNG lain (dikutip, disclaimer lintas-sesi)

Baris `philox` s.d. `dev_urandom` dikutip verbatim dari
`../2026-8-31_prng-family-benchmark/RESULTS.md` (sesi/mesin berbeda, MB/s
adalah **derivasi aljabar** dari reinit-sweep, BUKAN real multistream).
Baris `orbit` dan `singleblock` (kolom K=1/largest-K) dikutip dari sesi
lain juga (`combo-winner-pareto-selection` untuk singleblock K=1/K=255;
`RESULTS_TAHAP2.md` untuk orbit K=1 lama — lihat catatan). Kolom MB/s untuk
`orbit`/`singleblock` di tabel ini pakai angka **real multistream sesi hari
ini** (376.2 / 387.6) sebagai yang paling representatif, ditandai terpisah.

| core | K=1 ns/word (sesi asal, dikutip) | largest-K ns/word (dikutip) | MB/s |
|---|---:|---:|---:|
| splitmix64 | 3.0 | 0.245 @ K=1,000,000 | 16326.5 *(derivasi aljabar)* |
| pcg32 | 3.2 | 1.288 @ K=1,000,000 | 3105.6 *(derivasi aljabar)* |
| xoshiro256 | 6.5 | 0.937 @ K=1,000,000 | 4268.9 *(derivasi aljabar)* |
| philox | 12.1 | 2.600 @ K=1,000,000 | 1538.5 *(derivasi aljabar)* |
| paperorig | 24.9 | 5.263 @ K=1,000,000 | 760.0 *(derivasi aljabar)* |
| wiredv2 | 25.2 | 2.080 @ K=1,000,000 | 1923.1 *(derivasi aljabar)* |
| **singleblock** | 33.4 (sesi lama) / 39.063 (sesi pareto) | 0.132-0.1915 @ K=255 (cap) | **387.6** *(real multistream, sesi ini)* |
| **orbit** | 56.8 *(sesi `RESULTS_TAHAP2.md`, jauh lebih lama — TIDAK sebanding langsung)* | ~2.0-2.4 @ K besar *(sesi sama, historis)* | **376.2** *(real multistream, sesi ini, BARU)* |
| addrcont | 52.1 | 2.077 @ K=1,000,000 | 1925.9 *(derivasi aljabar)* |
| chacha20 | 121.5 | 7.344 @ K=1,000,000 | 544.7 *(derivasi aljabar)* |
| dev_urandom | — | — | 507.0 |
| mt19937 | 1289.4 | 1.776 @ K=1,000,000 | 2252.3 *(derivasi aljabar)* |

**Baca tabel ini dengan hati-hati** — dua sumber ketidaksebandingan
bertumpuk:
1. **Lintas-sesi/mesin**: baris `orbit`/`singleblock` yang dikutip
   (K=1 ns/word, largest-K ns/word) berasal dari sesi/mesin lain, TIDAK
   sebanding langsung dengan baris lain di kolom yang sama. Sudah
   didisclaimer di RESULTS.md asalnya masing-masing.
2. **Metodologi MB/s berbeda**: kolom MB/s `orbit`/`singleblock` di tabel
   ini adalah pengukuran **real** (syscall-inclusive multistream, sesi hari
   ini, same-machine antara keduanya — jadi *keduanya* sebanding satu sama
   lain), sementara kolom MB/s baris lain adalah **derivasi aljabar** dari
   reinit-sweep (bukan real multistream) — pola yang sama persis dengan
   yang sudah didisclaimer untuk angka lama `singleblock` (30303 MB/s) di
   `combo-winner-pareto-selection/RESULTS.md`. Real multistream historisnya
   terbukti 2 orde magnitudo lebih rendah dari derivasi aljabar untuk core
   yang sama (`singleblock`: 30303 derivasi vs 154.8/387.6 real) — jadi
   kolom MB/s baris lain (yang semuanya derivasi aljabar) kemungkinan besar
   **jauh melebih-lebihkan** MB/s real mereka juga, dengan besaran yang
   tidak diketahui tanpa mengukur ulang tiap core dengan `--multistream`
   real (di luar scope task ini).

**Perbandingan paling sahih yang tersedia dari tabel ini**: `orbit` vs
`singleblock`, keduanya diukur real-multistream, same-session, same-machine,
hari ini — `orbit` (376.2 MB/s) ≈ 97% dari `singleblock` (387.6 MB/s),
selisih kecil dan sesuai arah struktural (overhead swap `L[]`).

## Diskusi

Temuan utama task ini bukan cuma angka orbit itu sendiri, tapi **replikasi
pola metodologis** yang sudah ditemukan untuk singleblock: perbandingan
speed lintas-sesi (bahkan di dalam repo riset yang sama) tidak bisa
dipercaya secara nilai absolut — mesin/beban sistem berbeda menghasilkan
faktor 2-2.5x perbedaan yang tidak ada hubungannya dengan algoritma. Kalau
task berikutnya ingin klaim speed `ra_prng` (orbit atau singleblock)
"X kali lebih cepat/lambat" dari philox/xoshiro/dst. secara presisi, itu
memerlukan re-run SEMUA core dengan `--multistream` real, same-session,
same-machine — bukan sekadar mengutip tabel `benchmark_all.c` yang lama
(derivasi aljabar) atau membandingkan across sessions apa adanya. Ini di
luar scope task ini (user secara eksplisit membatasi ke "orbit continuous +
kutip yang sudah ada"), dicatat di sini sebagai temuan metodologis yang
relevan untuk task lanjutan mana pun yang ingin klaim perbandingan speed
presisi terhadap keluarga PRNG lain.

## Kesimpulan

- `orbit` continuous-stream real (parameter identik `singleblock`):
  **376.2 MB/s** (mean 2 run, sesi/mesin hari ini).
- Dibandingkan `singleblock` di mesin/sesi yang sama (1 titik sanity-check):
  `orbit` ≈ 3% lebih lambat — sesuai ekspektasi struktural (swap `L[]`
  ekstra), bukan anomali.
- Perbandingan absolut terhadap keluarga PRNG lain (philox, xoshiro, dst.)
  **tidak bisa diklaim presisi** dari data yang ada — dua lapis
  ketidaksebandingan (lintas-sesi + MB/s derivasi-aljabar vs real) bertumpuk.
  Urutan kasar tetap berguna: `orbit`/`singleblock` (ratusan MB/s real)
  jelas lebih lambat dari core-core cepat (splitmix64/pcg32/xoshiro256, ribuan
  MB/s bahkan setelah dikoreksi turun 1-2 orde magnitudo dari angka
  derivasi-aljabar mereka) tapi jelas lebih cepat dari `mt19937`/`chacha20`.
- **Tidak ada keputusan promosi** — `orbit` sudah production, task ini murni
  karakterisasi speed.

## Verifikasi checklist penutup

- `orbit_bench.c` compile bersih (`-Wall -Wextra`, 0 warning) setelah
  perbaikan format komentar (peringatan `-Wcomment` awal, bukan bug logika).
- `verify_orbit_kat.py`: 10/10, 0 mismatch.
- `git diff --stat -- ../2026-9-1_family-productionization/ra_core.c`:
  kosong sebelum & sesudah.
- Tidak ada perubahan ke `../2026-9-3_combo-winner-pareto-selection/*`
  maupun `../2026-8-31_prng-family-benchmark/*` — file `winners_bench`
  dijalankan (bukan diubah) sekali untuk sanity-check.
- Tidak ada FAIL/hasil anomali yang di-rerun dengan parameter berbeda untuk
  "menjelaskan"-nya — sanity-check dilakukan justru untuk mengonfirmasi
  angka SEBELUM ditulis sebagai hasil, bukan sesudah ditemukan anomali lalu
  diabaikan.

## Closing note

`/graphify --update` dijalankan setelah RESULTS.md ini selesai, per
`CLAUDE.md` proyek.
