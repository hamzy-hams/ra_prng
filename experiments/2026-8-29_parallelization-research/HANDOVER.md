# Handover: Riset Paralelisasi RNG (independensi multi-stream)

Ditulis untuk sesi Claude Code baru (atau user) yang mengambil ini nanti.
Ini adalah **spek untuk riset yang belum dimulai** — belum ada file
kode/harness/data apa pun di folder ini, hanya dokumen ini.

## Konteks: kenapa riset ini muncul

Pertanyaan awalnya: karena arsitektur RNG ini berbasis array state mandiri
per instance (`L`, `M`, skalar `cons`) tanpa shared state antar-instance,
apakah ini punya potensi paralelisasi — satu instance generator per
CPU/core/thread, tanpa lock atau sinkronisasi?

Jawabannya secara arsitektural **ya, berpotensi** — ini *embarrassingly
parallel* secara struktural. Tapi seluruh riset yang sudah dijalankan
sejauh ini (`2026-8-25_periodicity-heuristic-validation/`,
`2026-8-26_operation-pruning-research/`,
`2026-8-27_operand-position-search/`,
`2026-8-28_state-update-mechanism-research/`,
`2026-8-28_periodicity-heuristic-validation2/`) **semuanya menguji
satu stream/seed pada satu waktu** — periodicity (toy-model & full-scale
sanity check), PractRand, avalanche, dan speed semuanya axis single-stream.
**Belum ada satupun pengujian independensi antar-stream saat dijalankan
paralel dalam jumlah besar.** Inilah gap yang mau ditutup riset ini.

**Catatan soal subjek uji**: dokumen ini semula mengutip sebuah "instruksi
eksplisit dari user" yang ternyata **tidak akurat** — bukan kalimat yang
pernah ditulis user secara verbatim, hanya parafrasa yang keliru
diatribusikan sebagai kutipan langsung. Rasionalnya tetap valid dan
independen dari kutipan yang sudah dihapus itu: karena seluruh riset
sebelumnya (Fase 1/2 `state-update-mechanism-research`, `operand-position-search`,
`operation-pruning-research`) memvalidasi `winner_wired_v2.c` sebagai
kandidat cepat yang sudah lolos ke-4 axis (periodicity, PractRand, avalanche,
speed), maka **itulah subjek uji paling masuk akal untuk riset paralelisasi
ini** — bukan karena instruksi eksplisit yang pernah diberikan, melainkan
karena ia adalah hasil terbaik yang sudah divalidasi.

## Klarifikasi penting sebelum mulai: toy model ≠ RNG yang bisa dideploy

Jangan salah pakai `toy_prng.py`/`cycle_measure.c` (parameter `n`∈{2,4,8},
`w`∈{4,8,16}) sebagai subjek uji paralelisasi. Toy model itu murni proxy
matematis skala-turun yang dibuat supaya pengukuran periode via Brent's
algorithm jadi tractable secara komputasi — dikutip langsung dari
`experiments/2026-8-28_state-update-mechanism-research/HANDOVER.md`:

> "the real 256×32-bit state space is computationally infeasible to
> cycle-close directly"

Periode toy model sengaja pendek (berkisar 16 sampai puluhan juta,
lihat `2026-8-28_state-update-mechanism-research/RESULTS.md`) supaya bisa
diukur exhaustif — ini bukan indikasi kualitas RNG kalau dipakai apa
adanya, dan toy model **tidak pernah divalidasi atau dimaksudkan sebagai
RNG produksi** untuk use-case apa pun, kecil ataupun besar.

## Target algoritma untuk riset paralelisasi ini

- **Winner (subjek utama uji)**:
  `experiments/2026-8-27_operand-position-search/winner_wired_v2.c` —
  256×32-bit, disebut eksplisit sebagai "current recommended fast
  candidate (superseded `winner_wired.c`/v05 as of 2026-08-28)" di
  `experiments/2026-8-28_state-update-mechanism-research/HANDOVER.md`.
  Riset mekanisme update state L/M (fase 1 & 2, folder yang sama)
  menyimpulkan mempertahankan mekanisme `permute`+`xor_fold` pada
  kandidat ini tanpa perubahan — jadi `winner_wired_v2.c` tetap kandidat
  "terbaik" yang berlaku per tanggal dokumen ini ditulis (2026-08-29).
- **(Opsional, pembanding)** `src/ra_prng2/c/ra_prng2.c` — versi
  paper-exact/original, kalau ingin tahu apakah pruning+wiring di
  `winner_wired_v2.c` mengubah perilaku independensi antar-stream
  dibanding versi asli.

## Constraint read-only (diwarisi dari riset-riset sebelumnya)

- **Never modify** `src/ra_prng2/*`, `src/ra_prng3/*`, atau
  `winner_wired_v2.c` in place — file harness/instrumentasi baru untuk
  riset ini harus jadi mirror/wrapper terpisah di folder ini, memanggil
  CLI `--stream <seed> <n>` yang sudah ada pada binary tsb, bukan
  mengedit source-nya.

## Pertanyaan riset yang perlu dijawab

1. **Independensi antar-stream**: apakah seed yang berdekatan
   (mis. seed=0,1,2,...) menghasilkan stream yang berkorelasi secara
   statistik? Uji lewat cross-correlation langsung antar pasangan
   stream, atau treat gabungan/interleaved N stream sebagai satu input
   besar dan jalankan lewat PractRand (mirip pendekatan
   `2026-8-28_state-update-mechanism-research/` tapi lintas-seed, bukan
   lintas-mekanisme).
2. **Probabilitas collision/overlap siklus**: kalau menjalankan
   ribuan–jutaan seed paralel, seberapa besar kemungkinan dua seed
   berbeda jatuh ke siklus yang sama atau tumpang-tindih di titik yang
   berdekatan? State space 256×32-bit sangat besar sehingga secara teori
   collision harus sangat jarang, tapi ini **belum diverifikasi empiris**
   untuk arsitektur khusus ini (bandingkan dengan pendekatan
   `full_scale_sanity_check.py` di riset-riset sebelumnya, yang mengecek
   block-repeat *dalam* satu stream, bukan *antar* stream).
3. **Skalabilitas throughput riil**: seberapa dekat generator ini ke
   scaling linear di multi-core (tiap thread/core punya instance state
   independen, tanpa shared state/lock)? Ukur instruction count/wall-time
   per elemen pada 1 thread vs N thread (`perf stat`, konsisten dengan
   convention "instruction count sebagai axis utama" dari riset-riset
   sebelumnya — jangan jalankan bersamaan job CPU-heavy lain).
4. **(Opsional) Potensi vektorisasi SIMD**: apakah operasi
   rotate/add/xor/shift yang seragam per-lane memungkinkan beberapa
   stream independen dijalankan sekaligus dalam register SIMD
   (AVX2/AVX-512), dan apakah itu memberi speedup dibanding menjalankan
   generator satu-per-satu secara scalar.

## Metodologi yang disarankan (belum diimplementasikan)

- Bangun harness yang men-generate K stream dari K seed berbeda
  (mis. seed berurutan 0..K-1, dan/atau seed acak) memakai
  `winner_wired_v2 --stream <seed> <n>` yang sudah ada — jangan
  regenerasi/reimplementasi generator itu sendiri.
- Independensi: cross-correlation antar pasangan stream, dan/atau
  PractRand/TestU01 pada output ter-interleave dari banyak stream.
- Collision: bandingkan prefix N-elemen pertama tiap stream, cari
  duplikat/overlap antar stream (bukan dalam satu stream).
- Throughput: `perf stat -e instructions,cycles` single-thread vs
  multi-thread, bandingkan instruction count per elemen per thread.

## Belum dikerjakan

Dokumen ini murni spesifikasi awal — belum ada file kode, harness,
maupun data pengukuran apa pun di folder ini. Sesi berikutnya yang
mengambil alih riset ini perlu mulai dari menulis harness pembangkit
multi-stream dan skrip analisis independensi/collision/throughput di
atas, memakai `winner_wired_v2.c` sebagai subjek uji utama.

## Setelah selesai

Jalankan `/graphify --update` sebelum menutup tugas — wajib per
`CLAUDE.md` proyek ini untuk memasukkan file-file baru ke knowledge graph.
