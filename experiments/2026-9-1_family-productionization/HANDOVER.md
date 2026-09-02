# Handover: Formalisasi "ra_prng-family" — Unifikasi Kode + Gerbang Validasi Produksi

Ditulis untuk sesi Claude Code yang akan **mengeksekusi** ini (dipicu frasa
"lanjutkan family-productionization" atau serupa). **Status: BELUM
DIKERJAKAN** — dokumen ini murni spesifikasi (handover), ditulis di sesi
yang sengaja tidak mengimplementasikan kodenya sendiri, atas permintaan
eksplisit user ("jangan lanjut dulu karena ini perlu di rename dan
diluruskan kodenya", 2026-09-01). Baca dokumen ini sampai selesai sebelum
menulis kode apa pun.

**Urutan tahap WAJIB dipatuhi — jangan mulai Tahap 2 sebelum Tahap 1 selesai
dan diverifikasi oleh user.** Ini bukan preferensi, ini instruksi eksplisit
dari user saat handover ini ditulis.

## 0. Latar belakang (supaya sesi eksekusi tidak perlu riset ulang)

Motivasi datang dari `experiments/2026-8-31_prng-family-benchmark/` (benchmark
12 kandidat, ra_prng vs PRNG literatur) yang membuktikan `ra_core_singleblock`
(Tahap 6, `experiments/2026-8-30_addressable-init-research/HANDOVER_TAHAP6.md`)
unggul telak di pola pemakaian reinit-agresif (39x lebih murah dari MT19937 di
K=1), sementara `winner_wired_addressable.c` (Tahap 0-5, folder yang sama)
sudah tervalidasi luas untuk pemakaian stream berkelanjutan. Setelah melihat
ini, user ingin memformalkan "keluarga" ra_prng ini **di kode**, bukan cuma
sebagai laporan benchmark — dipetakan langsung ke use-case.

**Peta use-case → algoritma** (keputusan user, dikutip dari diskusi
2026-09-01):

| Use-case | Algoritma | Catatan |
|---|---|---|
| Stream berkelanjutan (single stream ATAU parallel-orbit) | `winner_wired` (unified, lihat Tahap 1) | Ini standar/default keluarga |
| Pemakaian cukup singkat | Varian addressable dari `winner_wired` yang sama | BUKAN implementasi terpisah setelah Tahap 1 selesai |
| Panggil-buang agresif (frequent reinit, sedikit kata per address) | `ra_core_singleblock` | Tetap variant terpisah dan sempit, bukan default |

## 1. Tahap 1 (WAJIB PERTAMA): Unifikasi `winner_wired_v2.c` + `winner_wired_addressable.c`

**Diagnosis user**: kedua file berikut ini sebenarnya algoritma yang sama,
cuma beda nama variabel dan beda cara pakai (init tetap vs init
key-addressable):
- `experiments/2026-8-27_operand-position-search/winner_wired_v2.c` —
  `ra_core`, `ra_init_state(L,M)` tanpa parameter key (init fixed,
  seed-independent, seed cuma masuk lewat `cons=seed` di dalam `ra_core`).
- `experiments/2026-8-30_addressable-init-research/winner_wired_addressable.c`
  — `ra_permutation_cycle`/`ra_reseed`/`ra_core`/`rot32`/`ra_hash` adalah
  copy byte-for-byte dari `winner_wired_v2.c`; **satu-satunya** perbedaan
  adalah `ra_init_state` → `ra_init_state_addressable(L, M, key)` (formula
  "Orbit Addressing" dari Tahap 1 addressable-init).

**Tujuan Tahap 1**: satu algoritma/codebase, dua mode pakai (stream biasa
vs. addressable/parallel-orbit) lewat parameter/entry point pada init —
bukan dua file yang tumbuh divergen dari waktu ke waktu seperti sekarang.

**Langkah yang disarankan** (verifikasi dulu sebelum menulis kode baru):

1. Diff line-by-line kedua file untuk mengonfirmasi diagnosis user (bahwa
   memang HANYA `ra_init_state` yang beda) — jangan asumsikan, verifikasi
   langsung. Precedent: `winner_wired_addressable.c`'s HANDOVER sendiri
   sudah menyatakan ini ("`ra_permutation_cycle`, `ra_reseed`, `ra_core`,
   `rot32`, `ra_hash` are byte-for-byte copies of `winner_wired_v2.c`'s"),
   jadi diff seharusnya cepat/konfirmatif, bukan eksploratif.
2. Rancang satu core dengan init yang bisa pakai formula lama (`ra_init_state`,
   fixed) ATAU formula addressable (`ra_init_state_addressable(L,M,key)`) —
   mis. lewat dua entry point publik yang memanggil inti yang sama, atau satu
   fungsi init dengan parameter yang membedakan mode. Detail desain API ini
   **belum diputuskan user** — lihat §4 "Open decisions" sebelum
   berasumsi.
3. Rekonsiliasi nama variabel (user bilang "beda nama variabel" adalah
   bagian dari masalah) — samakan konvensi penamaan di kedua jalur sebelum/
   selama penggabungan, bukan cuma menyalin satu set nama begitu saja.
4. `ra_core_singleblock` (Tahap 6, di `tahap6_bench.c`) TIDAK ikut
   digabung di tahap ini — ia tetap terpisah, dan setelah unified core
   selesai, `ra_core_singleblock` semestinya dibangun DI ATAS unified core
   itu (bukan diubah sendiri) supaya tidak ada dua sumber kebenaran untuk
   bagian yang mereka bagi (recurrence `a/b/o/c/d`, `ra_hash`).

**Precedent read-only repo ini** (dipatuhi konsisten di setiap eksperimen
sebelumnya): source yang sudah tervalidasi tidak pernah diedit in-place,
selalu dibuat file baru. Rekomendasi: ikuti pola yang sama — hasil
unifikasi jadi **file baru**, `winner_wired_v2.c` dan
`winner_wired_addressable.c` yang lama TIDAK disentuh (tetap ada sebagai
riwayat/rujukan). Ini rekomendasi, bukan keputusan final user — lihat §4.

**Validasi wajib sebelum Tahap 1 dianggap selesai**: hasil unifikasi harus
diverifikasi bit-identical terhadap KEDUA mode lama (`--stream` output
`winner_wired_v2.c` untuk mode fixed-init, dan `winner_wired_addressable.c`
untuk mode addressable) — pola cross-check yang sama persis dipakai di
setiap tahap riset sebelumnya di repo ini (lihat `tahap0_prototype.py`'s
`sanity_check()`, `tahap6_bench.c`'s `validate` mode, dst.). Kalau
bit-identical, seluruh jaminan statistik (PractRand 128GB, cross-correlation,
collision-scan) otomatis terwarisi — tidak perlu diulang di tahap ini.

## 2. Tahap 2 (SETELAH Tahap 1 selesai & dikonfirmasi user): Folder/battery validasi produksi

**Jangan mulai bagian ini sebelum Tahap 1 selesai.** Setelah unified core
(dan `ra_core_singleblock` di atasnya) ada dan bit-identical-validated,
bangun battery validasi berikut untuk **2 kandidat produksi**: unified
`winner_wired` (mode addressable, karena itu superset dari mode stream
biasa) dan `ra_core_singleblock`:

- **Avalanche** — reuse `quality_gate.py`'s `avalanche_gate_min_bit()` (band
  keseluruhan `[0.3,0.7]`, floor per-bit `>=0.2`), bukan gate rata-rata lama
  yang pernah melewatkan cacat bit 5/6 di `operand-position-search`.
- **Kecepatan** — `perf stat -e instructions,cycles`, pola min-of-trials
  seperti `tahap4/5/6_bench.c`.
- **Kualitas statistik** — PractRand bertahap (8GB→…), **dieharder full
  "Good" battery** (27 tes, piped no-file, pola
  `experiments/2026-8-26_operation-pruning-research/`'s
  `dieharder_pruned_winner_piped.txt` — item ini belum pernah dijalankan
  untuk `winner_wired_v2`/`winner_wired_addressable`/`singleblock`, lihat
  memory `project_research_backlog_2026_09`), cross-correlation +
  collision-scan skala produksi (pola `2026-8-29_parallelization-research/`).
- **Implementasi shuffling** — pola `scc_test.py`/`verify_parity.py` di
  `2026-8-30_addressable-shuffle/` (entropy, chi-square, runs-test, serial
  correlation, multi-key distinctness) diterapkan ke unified core +
  `singleblock` kalau relevan untuk use-case shuffle.

**Kalau ada axis yang gagal**: perbaiki operasi internal (`init`/`cycles`)
lalu retest — ulangi sampai lolos semua axis. **Jangan promosikan apa pun ke
`src/` sebelum lolos penuh.**

## 3. Non-goals eksplisit (supaya sesi eksekusi tidak overscope)

- **Tidak** memulai Tahap 2 sebelum Tahap 1 selesai DAN dikonfirmasi user —
  constraint eksplisit, bukan urutan yang bisa ditukar demi kenyamanan.
- **Tidak** mempromosikan apa pun ke `src/` sebelum lolos Tahap 2 penuh.
- **Tidak** mengubah formula/recurrence apa pun selama Tahap 1 — ini murni
  penggabungan/rename kode, bukan riset formula baru. Kalau diff di
  langkah 1 §1 menemukan bahwa kedua file TERNYATA tidak cuma beda nama
  variabel (mis. ada perbedaan operasi lain yang belum tercatat), STOP dan
  laporkan ke user sebelum melanjutkan — jangan diam-diam "meluruskan" jadi
  satu formula pilihan tanpa konfirmasi.
- **Tidak** mengerjakan item-item di `project_research_backlog_2026_09`
  (dieharder mandiri di luar Tahap 2 ini, mekanisme `inject` di w=32,
  `docs/ERRATA.md`, SIMD cross-stream L-swap) — itu track riset terpisah,
  independen dari productionization ini, boleh dikerjakan kapan saja tapi
  tidak termasuk scope folder ini.
- **Tidak** menyentuh anomali 1TB PractRand `winner_wired_v2` yang sedang
  dikonfirmasi ulang di VPS (lihat memory `project_parallelization_research_1tb`)
  — itu proses terpisah yang sedang berjalan, di luar scope handover ini.

## 4. Open decisions untuk sesi eksekusi (BELUM diputuskan user — jangan diasumsikan, tanya dulu)

1. **Skema API unified core**: dua fungsi publik terpisah (mis.
   `ra_core_stream(...)` vs `ra_core_addressable(...)`) yang memanggil satu
   inti bersama, atau satu fungsi dengan parameter mode? Tidak ada preferensi
   user yang tercatat.
2. **Nama file/fungsi hasil unifikasi**: user bilang "perlu di-rename" secara
   eksplisit — artinya nama-nama saat ini (`winner_wired_v2`,
   `winner_wired_addressable`) sendiri dianggap bermasalah, bukan cuma
   soal dua-file-jadi-satu. Jangan asumsikan nama baru (mis. jangan asal
   pilih `winner_wired_unified.c`) — konfirmasi dulu skema penamaan yang
   user mau sebelum menulis file.
3. **Lokasi promosi ke `src/`**: apakah jadi bagian dari folder ini (mis.
   sub-langkah Tahap 2), atau langkah terpisah lagi setelah folder ini
   ditutup? Belum diputuskan.
4. **Apakah mode "stream biasa" (non-addressable) tetap dipertahankan
   sebagai entry point sendiri**, atau addressable dengan address tetap/
   default sudah cukup menggantikannya sepenuhnya? User menyebut keduanya
   sebagai use-case terpisah ("single stream" vs "pemakaian singkat") tapi
   juga bilang harus jadi "algoritma yang sama" — sesi eksekusi perlu
   klarifikasi apakah ini berarti satu fungsi generate dengan dua cara init,
   atau sesuatu yang lebih menyatu lagi.

## 5. Penutup sesi

`CLAUDE.md` proyek ini mewajibkan `/graphify --update` sebelum menutup tugas
riset — jalankan ini setelah menulis handover ini (dan lagi setelah setiap
tahap di atas selesai).

**Memory terkait** (baca sebelum mulai, di
`~/.claude/projects/-home-zep-Documents-research-ra-prng/memory/`):
- `project_ra_prng_family_productionization.md` — keputusan lengkap yang
  melatarbelakangi handover ini.
- `project_addressable_init_research.md` — status Tahap 0-6
  addressable-init (termasuk detail `ra_core_singleblock`).
- `project_research_backlog_2026_09.md` — daftar riset terpisah, non-goal
  untuk folder ini tapi relevan untuk konteks.
- `project_parallelization_research_1tb.md` — status konfirmasi ulang
  anomali 1TB yang sedang berjalan di VPS, tidak terkait langsung tapi
  baik diketahui.
