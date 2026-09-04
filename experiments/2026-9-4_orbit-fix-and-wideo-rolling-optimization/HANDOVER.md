# HANDOVER — Orbit K-kecil defect fix + rolling-register optimasi wide-`o`

**Status (2026-09-04, update): SELESAI DIPROMOSIKAN ke
`../2026-9-1_family-productionization/ra_core.c` kanonik.** Battery
statistik yang di-defer di §6 (di bawah) sudah dijalankan di
`../2026-9-4_orbit-kmin-battery/` (dieharder K=1/K=255 0 FAILED, PractRand
16GB K=1/K=255 no anomalies, avalanche + collision-scan PASS — lihat
`RESULTS.md` di folder itu) — **PASS**, jadi syarat promosi di §6 terpenuhi.
Kedua perubahan (fix formula K-kecil orbit + rolling-`o` untuk KEDUA core)
sudah di-port ke `ra_core.c` kanonik: `./ra_core validate` 0 mismatch untuk
singleblock DAN orbit. Masih **BELUM** dipromosikan ke `src/` resmi — itu
keputusan repo-level terpisah yang tidak pernah di-scope, di luar sesi ini
juga.

Isi asli HANDOVER ini (di bawah) dipertahankan sebagai catatan
riset/derivasi historis dari sesi ketika perubahan pertama kali diterapkan
ke `ra_core_v2.c` (sebelum promosi).

## 1. Temuan awal: `ra_core_orbit` rentan defek K-kecil

Dipicu pertanyaan user: "apa yang terjadi kalau ada orang mengambil orbit
K=1 sebagai input?"

Investigasi (2 subagent Explore, membaca ulang seluruh riset K-threshold-
characterization dan production-candidate-battery) mengonfirmasi celah
nyata yang belum pernah ditandai:

- `ra_permutation_cycle_orbit()` (di
  `../2026-9-1_family-productionization/ra_core.c`) masih pakai `o` 2-tap
  sempit (`M[i+6]<<6 ^ M[i+7]<<7`) **tanpa finalizer** — persis formula
  LAMA yang terbukti gagal PractRand (defek BCFN) di bawah K=96, sebelum
  fix `w8_f10_i0` (2026-09-03,
  `../2026-9-3_dieharder-inject-crossing/RESULTS.md` +
  `../2026-9-2_singleblock-cycle-combo-search/RESULTS.md` +
  `../2026-9-3_combo-winner-pareto-selection/RESULTS.md`) diterapkan ke
  `ra_permutation_cycle_singleblock`.
- `ra_init_state_orbit`'s bagian M[] byte-identik dengan
  `ra_init_state_singleblock`; L[] tidak berpengaruh ke output untuk
  `rng<=255` dalam satu panggilan (swap L hanya relevan setelah
  `ra_reseed`, yang tak pernah terpanggil sebelum 255 kata).
- Konsekuensi: `ra_core_orbit(key, rng<=255, ...)` menghasilkan output
  **byte-for-byte identik** dengan formula singleblock versi PRA-fix — jadi
  siapa pun yang memakai orbit dengan pola "multistream" (banyak panggilan
  pendek, key baru tiap panggilan, K kecil) alih-alih pola yang dimaksud
  (satu stream panjang kontinu) akan mereplikasi persis defek yang sudah
  terbukti gagal.
- Fix `w8_f10_i0` 2026-09-03 secara eksplisit hanya menyasar
  `ra_permutation_cycle_singleblock` (provenance comment di `ra_core.c`
  baris 41-57) — tidak ada guard minimum-K atau peringatan apa pun di
  `ra_core_orbit` untuk pola pemakaian ini.
- Semua uji K-kecil yang sudah ada (K-threshold-characterization,
  production-candidate-battery Step 0-8) hanya menguji
  `ra_core_singleblock`. `ra_core_orbit` tidak pernah diuji ulang di K
  kecil pasca-fix — klaim "128GB clean" untuk orbit di
  `PRODUCTION_READINESS_HANDOVER.md` mewarisi validasi stream panjang
  kontinu (`winner_wired_addressable.c`), bukan pola multistream.
- Petunjuk penguat (bukan penutup celah): versi *init* orbit pernah gagal
  BCFN serupa di masa lalu
  (`../2026-9-1_multikey-remix-search/HANDOVER.md`), tapi itu di-root-cause
  ke formula init (affine-in-key L[]/M[]) yang sudah diperbaiki di kedua
  mode sejak 2026-09-01 — bukan ke cycle transform, jadi tidak menutup
  celah K-kecil di atas.

## 2. Keputusan user (AskUserQuestion + pesan lanjutan)

1. **Terapkan** fix cycle `w8_f10_i0` ke `ra_permutation_cycle_orbit`.
2. **Terapkan rolling-register optimasi wide-`o` di KEDUA**
   `ra_permutation_cycle_singleblock` DAN `ra_permutation_cycle_orbit`
   (insight/instruksi eksplisit user — lihat §3).
3. **Re-validasi statistik penuh (PractRand/dieharder) untuk formula orbit
   K-kecil-fix di SESI LAIN** — di luar scope kerja ini.
4. **Perbaikan `ra_reseed`/`ra_hash` orbit di-skip untuk sekarang** — user
   eksplisit mau lihat hasil re-validasi cycle fix dulu. `ra_reseed`/
   `ra_hash` TIDAK disentuh sama sekali di sini.
5. Perubahan pada `ra_permutation_cycle_singleblock` (kode sudah
   dipromosikan di `ra_core.c`) dibatasi HANYA pada cara hitung `o`
   (rolling vs naif) — dikerjakan di `ra_core_v2.c` (salinan), tidak
   menimpa `ra_core.c` kanonik. Promosi rolling-singleblock ke `ra_core.c`
   kanonik adalah keputusan terpisah di masa depan.

## 3. Insight user: rolling-register untuk wide-`o`

`o` di-recompute dari 8 pembacaan M[] penuh tiap iterasi
(`M[i+0]<<0 ^ ... ^ M[i+7]<<7`), padahal window `o(i)` dan `o(i-1)`
beririsan 7 dari 8 elemen (indeks `i` turun 1 tiap iterasi). Karena XOR dan
left-shift-konstan itu linear atas GF(2) (`(a^b)<<k == (a<<k)^(b<<k)`),
berlaku identitas aljabar (BUKAN aproksimasi):

```
o(i-1) = M[(i-1) mod 256] ^ ( (o(i) ^ (M[(i+7) mod 256] << 7)) << 1 )
```

Derivasi: pisahkan `o(i-1)`'s 8 term jadi term `k=0` baru (`M[i-1]`) plus 7
term sisa yang masing-masing punya shift `+1` dibanding term sepadan di
`o(i)`. 7 term sisa itu = `o(i) minus term k=7` (dibuang via XOR), lalu
di-geser `<<1` sekaligus (sah karena shift distributif atas XOR). Diverifikasi:

- Manual dengan window 3-tap kecil (aljabar, exact match).
- Program differential standalone: 20 trial × 255 nilai `i` × M[] acak,
  membandingkan rolling vs hitung-ulang-dari-nol — **0 mismatch**.
- KAT `ra_core_v2.c`'s `validate` command: checksum singleblock rolling
  MATCH PERSIS dengan `SINGLEBLOCK_KAT_CHECKSUMS` yang sudah ada di
  `ra_core.c` kanonik (tidak diubah sedikit pun) — bukti bit-exact
  terhadap formula w8_f10_i0 yang sudah lolos PractRand/dieharder/
  collision-scan penuh.
- KAT orbit (`ORBIT_KAT_CHECKSUMS`, baru): di-generate dari
  `ra_core_v2.c` (`checksum-gen orbit`) DAN disilangkan dengan build
  referensi naif independen (kode terpisah, tidak berbagi apa pun dengan
  `ra_core_v2.c`) — 0 mismatch di 39 key.

Karena ini identitas aljabar (bukan formula baru), kebenarannya cukup
dibuktikan lewat KAT-match di atas — **tidak perlu re-run battery
statistik** untuk bagian optimasi ini. Battery yang di-defer ke sesi lain
(§6) itu untuk memvalidasi formula K-kecil-fix `w8_f10_i0` di ORBIT secara
statistik (baru, belum pernah diuji di K kecil) — bukan untuk rolling.

Menariknya: `ORBIT_KAT_CHECKSUMS` ternyata **identik** dengan
`SINGLEBLOCK_KAT_CHECKSUMS` — bukan bug, memang diharapkan: untuk window
K<=255 pra-reseed, orbit dan singleblock sekarang punya M[]-init, start
state (`cons=key`, `it=0`), dan cycle transform yang sama persis (L[]
tidak berpengaruh sebelum reseed).

## 4. Perubahan di `ra_core_v2.c`

Base: salinan utuh `../2026-9-1_family-productionization/ra_core.c`.
Fungsi yang diubah HANYA `ra_permutation_cycle_orbit` dan
`ra_permutation_cycle_singleblock`, plus mesin `validate`/checksum
digeneralisasi untuk menguji kedua core (ditambah subcommand dev
`checksum-gen <core>` untuk regenerasi golden checksum). Tidak ada
perubahan di `ra_init_state_orbit`, `ra_init_state_singleblock`,
`ra_reseed`, `ra_hash`, guard constants, atau `CORES[]`/`find_core`
(dicek via `diff` terhadap `ra_core.c` kanonik — scope sesuai).

## 5. Verifikasi yang sudah dijalankan

- Compile bersih: `gcc -O3 -march=native -std=gnu17 -include stdalign.h ra_core_v2.c -o ra_core_v2 -Wall -Wextra` — 0 warning.
- `./ra_core_v2 validate`:
  ```
  validate(singleblock): 39 keys checked, 0 checksum mismatches
  validate(orbit, K<=255 window): 39 keys checked, 0 checksum mismatches
  validate: PASS
  ```
- `diff` terhadap `ra_core.c` kanonik: scope perubahan sesuai (dua cycle
  function + provenance comment + mesin validate/checksum), tidak ada
  perubahan tak sengaja di fungsi lain.
- Benchmark informational (5-angka standar, `feedback_orbit_singleblock_benchmark_standard`),
  dibandingkan bersih terhadap variant "formula baru TANPA rolling"
  (mengisolasi efek rolling dari efek fix K-kecil):

  | | K=1 (ns/word) | K=255 (ns/word) | --stream N=5M (ns/word) |
  |---|---|---|---|
  | orbit, naif (formula baru, no rolling) | 68.8 | 5.03 | 4.90 |
  | orbit, rolling (`ra_core_v2.c`) | 72.9 | 3.50 | 3.22 |
  | singleblock, naif (formula baru, no rolling) | 45.8 | 0.79 | — |
  | singleblock, rolling (`ra_core_v2.c`) | 47.2 | 0.55 | — |

  Pola konsisten dan masuk akal: rolling memberi speedup **~30-34% di
  K=255/stream panjang** (loop benar-benar berulang berkali-kali, di situ
  rolling menghemat kerja). Di K=1 hasilnya wash/dalam noise (~5-6% lebih
  lambat) — **diharapkan**, karena di K=1 hanya SATU iterasi cycle yang
  jalan (`count<=1` langsung break sebelum blok update rolling pernah
  tereksekusi), jadi rolling secara struktural tidak sempat berkontribusi
  apa pun di K=1; ini bukan pola regresi K=255 yang pernah ditemukan di
  `../2026-9-3_init-loop-optimization/` (situ sebaliknya: menang K=1,
  regresi K=255) — di sini justru konsisten menang di beban kerja yang
  lebih besar, tidak ada regresi di K manapun.

## 6. Yang BELUM dikerjakan / sengaja di-skip

- ~~Re-validasi statistik (PractRand/dieharder) untuk formula orbit
  K-kecil-fix~~ — **SELESAI 2026-09-04**, PASS. Lihat
  `../2026-9-4_orbit-kmin-battery/RESULTS.md` dan status update di
  puncak file ini. Fix + rolling-`o` (kedua core) sudah dipromosikan ke
  `ra_core.c` kanonik.
- **Perbaikan `ra_reseed`/`ra_hash` orbit** — MASIH di-skip/terbuka (di
  luar scope battery K-kecil di atas). Catatan riset yang relevan kalau
  nanti dilanjut: korelasi BCFN yang pernah ditemukan terkonsentrasi di
  siklus PERTAMA SEBELUM reseed
  (`../2026-9-1_multikey-remix-search/HANDOVER.md`,
  `../2026-9-1_family-productionization/RESULTS_TAHAP2.md` baris
  130-132), dan mencoba `ra_hash` sebagai pre-mix M[] di awal siklus
  **gagal total** memperbaiki defek itu
  (`../2026-9-2_singleblock-prereseed-experiment/RESULTS.md`) — jadi
  arah "perbaiki reseed" perlu ide baru, bukan sekadar pakai `ra_hash`
  lebih sering/lebih awal.
- ~~Promosi rolling-`o` untuk singleblock ke `ra_core.c` kanonik~~ —
  **SELESAI 2026-09-04**, sekaligus dengan promosi fix orbit di atas.
- Promosi `ra_core.c` kanonik → `src/` resmi — MASIH belum diputuskan/
  di-scope, terlepas dari semua yang di atas.
