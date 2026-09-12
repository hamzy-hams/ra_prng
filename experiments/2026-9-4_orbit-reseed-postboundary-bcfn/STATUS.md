# STATUS (2026-09-06, sesi lanjutan) — RESOLVED, PASS

**Item ini sekarang CLOSED.** Lihat `RESULTS.md` untuk log lengkap dan
analisis. Ringkasan: kontrol sanity WAJIB (`HANDOVER.md` §2.3) dan 4 run
utama post-reseed semuanya bersih sampai target 8GB — tidak ditemukan
defect BCFN-style pada `ra_reseed`/`ra_hash`.

## Apa yang berubah sejak status BLOCKED (2026-09-04)

Status lama di file ini (2026-09-04) melaporkan BLOCKED: kontrol sanity
`--control-hashed` K=1 FAIL parah (`FPF-14+6/16`, eskalasi sampai R=+132.3
@8GB kalau dipaksa), ditemukan bukan bug harness tapi defect nyata di
`ra_permutation_cycle_orbit` untuk urutan key hashed dengan `key` 32-bit —
root cause: birthday-paradox key-repetition (lihat
`../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md` dan
`../2026-9-5_orbit-k1-exhaustive-function-analysis/`).

Defect itu **sudah diperbaiki** 2026-09-06 lewat widening `addr_hi`
(Kandidat D, `../2026-9-5_orbit-address-widen-64bit/`, dipromosikan ke
`ra_core.c` kanonik di `../2026-9-6_orbit-address-widen-64bit-promotion/`).
Sesi ini (2026-09-06, lanjutan) menemukan satu hal penting yang belum
tercatat di mana pun: **driver post-reseed di folder ini (`multikey_stream_
orbit_postreseed_k1.c`/`_k255.c`) masih hardcode `addr_hi=0u` di semua
mode** — konfigurasi itu TIDAK mengaktifkan fix (lihat detail teknis di
`RESULTS.md`). Driver diperbaiki untuk memvariasikan `addr_hi` secara
independen dari `key` (pola dua-chain, sama seperti
`../2026-9-5_orbit-address-widen-64bit/multikey_stream_addr_hi.c`), lalu
seluruh kontrol + run utama dijalankan ULANG dari nol dengan konfigurasi
yang benar itu.

## Hasil akhir (2026-09-06, VPS)

- **Kontrol sanity (2GB, 4 kombinasi K×urutan-key)**: SEMUA bersih. Dua
  blip "unusual" terisolasi (`control_k1_hashed`@256MB `BCFN(2+2,13-4,T)`
  R=-6.9; keduanya hilang di checkpoint berikutnya) — pola noise yang sama
  seperti yang sudah tercatat historis di folder lain, bukan defect.
- **4 run utama (8GB, K∈{1,255} × urutan-key∈{weyl,hashed})**: SEMUA bersih
  sampai 8GB penuh. Satu blip "unusual" terisolasi (`main_k255_weyl`@256MB
  dua flag `BCFN`/`Gap-16:B`, hilang di 512MB; `main_k1_weyl`@4GB satu flag
  `mod3n(5)`, hilang di 8GB) — tidak eskalasi, sesuai kriteria PASS di
  `HANDOVER.md` §3.

**Verdict: PASS.** `ra_reseed`/`ra_hash` tidak membocorkan struktur
terdeteksi ke siklus pasca-reseed, untuk K=1 dan K=255, urutan key Weyl
maupun hashed, sampai 8GB. Tidak ada blocker gate tersisa untuk item ini.

## File di folder ini

- `HANDOVER.md` — desain asli (tidak diubah).
- `multikey_stream_orbit_postreseed_k1.c` / `_k255.c` / `BUILD.sh` — driver,
  DIUPDATE 2026-09-06 untuk memvariasikan `addr_hi` (lihat komentar header
  file untuk detail).
- `control_*.log`, `main_*.log` — hasil final 2026-09-06 (dijalankan di
  VPS, lihat `RESULTS.md`). File lama dari 2026-09-04 (kontrol pra-fix yang
  FAIL) TIDAK ada di folder ini sebelumnya (hanya dicatat di
  `ADDENDUM_HASHED_KEY_GAP.md` folder tetangga).
- `vps_logs_20260906/` — salinan mentah 8 log dari VPS (4 kontrol + 4 run
  utama).
- `RESULTS.md` — laporan lengkap sesi ini.
