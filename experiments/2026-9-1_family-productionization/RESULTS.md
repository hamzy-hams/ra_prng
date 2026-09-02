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
