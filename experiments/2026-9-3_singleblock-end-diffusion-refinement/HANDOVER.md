# HANDOVER — Refinement akhir `ra_core_singleblock`: difusi-di-akhir, optimasi, eksperimen kecil

**Status folder ini: langkah TERAKHIR yang direncanakan untuk riset
singleblock K-small-defect-fix.** Setelah pekerjaan di folder ini selesai,
tidak ada iterasi lanjutan yang dijadwalkan untuk formula
`ra_core_singleblock` sebelum keputusan promosi ke `src/` (lihat §5).
Kalau ternyata muncul kebutuhan baru setelah ini, itu keputusan baru, bukan
lanjutan otomatis dari rencana yang ada.

## 1. Baseline: `ra_core_baseline.c` (REFERENSI BEKU, jangan diedit)

Salinan bit-identik dari
`../2026-9-1_family-productionization/ra_core.c` per 2026-09-03 (verified
`diff` kosong). Ini state yang SUDAH:
- Dipromosikan: `w8_f10_i0` (8-tap `o` + finalizer `c ^= c >> 17u`) resmi
  jadi formula `ra_permutation_cycle_singleblock` (commit `89ffc95`).
- Lolos penuh `production-candidate-battery` gate Step 0-8 (commit
  `e7628c1`, `2bbdce9`) — collision-scan, dieharder, PractRand 16GB
  (K=1..255), shuffle-implementation. Satu catatan diketahui: bias kecil
  di runs-test untuk pola ekstraksi-bit-rendah di K=1 (mis. Fisher-Yates),
  diterima sebagai limitasi, bukan blocker.
- Detail lengkap: `../2026-9-1_family-productionization/PRODUCTION_READINESS_HANDOVER.md`
  dan `../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md`.

**Cara mulai kerja**: copy `ra_core_baseline.c` ke nama baru (mis.
`ra_core_v2.c`), edit salinan itu. Jangan timpa `ra_core_baseline.c` —
fungsinya sebagai titik pembanding/regresi untuk apa pun yang dicoba di
sini.

## 2. Struktur round saat ini — di mana "difusi" terjadi

Dari `ra_permutation_cycle_singleblock` (baseline, per-iterasi `i` dari
255 turun ke 1):

```c
uint32_t o = (M[i+0]<<0) ^ (M[i+1]<<1) ^ ... ^ (M[i+7]<<7);  // 8-tap dari M[]

a = (d ^ o) ^ (cons + a);      // <-- o (entropi dari M[]) masuk DI SINI, AWAL round
b = (cons + a) ^ (o + d);      // <-- o dipakai LAGI di sini, masih awal
c = rot32((a >> 13) ^ a, b);   // mixing tengah (rotasi tergantung-data)
c ^= c >> 17u;                 // <-- SATU-SATUNYA langkah "di akhir" saat ini:
                                //     XORSHIFT sempit, self-mixing pada c
                                //     sendiri. TIDAK menyerap entropi M[]
                                //     baru di titik ini -- semua entropi M[]
                                //     sudah "dipakai habis" di awal (lewat o).

if (raw_stream) fwrite(&c, ...);   // c inilah yang jadi output
d = c & 0xFFu;                     // umpan untuk iterasi berikutnya
```

**Observasi yang melatari ide user**: difusi utama (kontribusi `o` dari
M[]) terjadi di AWAL round, sebelum `c` dihitung. Bagian "akhir" round
(finalizer) cuma XORSHIFT internal pada `c` — tidak ada titik di mana
entropi BARU (dari M[] atau dari state lain) sengaja disuntikkan tepat
sebelum output selesai dihitung. Ini kebalikan dari sebagian desain
hash/PRNG lain yang justru menaruh mixing terkuat paling dekat dengan
titik output (karena itu yang paling langsung menentukan statistik
output), bukan di awal round.

## 3. Arah kerja (dari user, DICATAT SEBAGAI ARAH — bukan spek final)

Tiga hal yang disebutkan, sengaja tidak dipersempit jadi spek detail
karena ini kerja mandiri, bukan hasil riset sesi ini:

1. **Tambah difusi di akhir round, bukan di awal.** Bukan berarti
   menghapus `o`/difusi-awal yang sudah tervalidasi — melainkan menambah
   satu tahap difusi baru dekat titik output (setelah/menggantikan
   finalizer sempit `c ^= c>>17u`), kemungkinan menyerap entropi tambahan
   di titik itu (bukan cuma self-XORSHIFT `c`). Bentuk konkretnya
   (operasi apa, sumber entropi tambahan dari mana) belum ditentukan —
   ini bagian yang mau dieksplorasi.
2. **Sedikit optimasi.** Tidak dispesifikasi user lebih lanjut sesi ini —
   dibiarkan terbuka. Kalau relevan sebagai referensi: sesi
   `../2026-9-3_init-loop-optimization/` sempat coba optimasi loop init
   (bukan cycle) dan menemukan hasil tidak strict-win (menang di K=1,
   regresi di K=255) — worth dibaca sebelum mengulang jalur yang sama.
3. **Eksperimen kecil.** Tidak dispesifikasi — terbuka juga.

## 4. Reminder metodologi (supaya tidak mengulang jebakan yang sudah ketemu)

- **Benchmark 5-angka standar** (memory `feedback_orbit_singleblock_benchmark_standard`):
  kalau mengukur kecepatan, selalu laporkan orbit `--multistream` K=1/K=255/
  `--stream`, DAN singleblock K=1/K=255 — jangan cuma K=1 (pernah menyesatkan).
- **No re-seed retry on FAIL** (memory `feedback_no_reseed_on_fail`): kalau
  PractRand/dieharder FAIL di satu seed, itu final — jangan coba seed lain
  untuk "menjelaskan" kebetulan.
- **Bug `fmemopen` exact-buffer-size** (`../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md`):
  kalau bikin tooling C baru yang capture output via `fmemopen(buf, n*sizeof(T), "wb")`,
  buffer HARUS 1 byte lebih besar dari data yang ditulis — glibc menaruh
  NUL-terminator yang menimpa byte terakhir kalau buffer pas ukurannya
  (klobber MSB word terakhir di little-endian). Ini nyata, sudah pernah
  bikin hasil test kelihatan katastropik padahal RNG-nya baik-baik saja.
- **Step 7 (shuffle-implementation) key-reuse note** (`../2026-9-3_combo-winner-pareto-selection/RUNNER_UP_BATTERY_COMPARISON.md`):
  driver shuffle K=1 yang ada menurunkan key sebagai
  `base_key + (rep_index + step_index) * GOLDEN` — pola aditif ini
  menyebabkan reuse key antar repetisi berbeda, kemungkinan sumber bias
  runs-test kecil yang sudah tercatat. Kalau eksperimen di sini menyentuh
  shuffle-implementation lagi, pertimbangkan skema indexing yang tidak
  reuse (mis. hash gabungan `(rep, step)` alih-alih penjumlahan linear).

## 5. Di luar scope folder ini

- Keputusan promosi `ra_core.c` ke `src/` (paket resmi, lihat
  `src/ra_prng2/`, `src/ra_prng3/` untuk konvensi yang ada) — masih
  terbuka, TIDAK bagian dari langkah terakhir ini kecuali user
  menyatakan lain.
- Mengubah `ra_core_orbit`/jalur orbit — baseline ini fokus
  `ra_core_singleblock` saja, ikuti scope yang sama kecuali diminta lain.
