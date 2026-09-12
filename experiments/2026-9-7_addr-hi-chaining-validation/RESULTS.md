# Validasi pola addr_hi-increment chaining untuk fitur shuffle pip-ra-core

## Konteks

Saat merancang plan library pip `ra-core` (lihat
`~/.claude/plans/aku-mau-bikin-library-smooth-dawn.md`), fitur shuffle
deterministik untuk N>255 diusulkan pakai `ra_core_singleblock` berturut-turut
dengan `key` TETAP dan `addr_hi` di-increment tiap block baru
(`addr_hi = block_idx`). Battery yang sudah closed-gate sebelumnya
(`../2026-9-2_singleblock-k-threshold-characterization/`,
`../2026-9-1_production-candidate-battery/`) semuanya menguji pola
KEBALIKANNYA: `key` yang increment (Weyl `0x9E3779B9`), `addr_hi` tetap 0.
Pola addr_hi-increment ini belum pernah jadi target PractRand secara
spesifik sebelum sesi ini.

## Metodologi

`multikey_stream_addrhi_k.c` (folder ini) — identik strukturnya dengan
`multikey_stream_k.c` tapi peran key/addr_hi ditukar. Link ke
`src/ra_core/c/ra_core.c` (canonical, promoted, sama persis yang akan
dibungkus pip-ra-core). `base_key=111222` (konsisten dengan konvensi
eksperimen K-threshold sebelumnya). `RNG_test stdin32 -tlmin 256MB -tlmax
8GB -multithreaded`, dua titik K: K=1 (simulasi block sisa/tail terkecil
yang mungkin muncul untuk N sembarang) dan K=255 (block penuh, kasus utama
shuffle).

## Hasil

| K | Hasil | Detail |
|---|---|---|
| 1 | **FAIL katastrofik** @256MB (checkpoint pertama) | `FPF-14+6/16:all` R=+47.8, p=2.3e-44, FAIL!!!; juga `FPF-14+6/16:(0,14-0)` R=+42.5 FAIL!!!, `DC6-9x1Bytes-1` R=+32.3 FAIL |
| 255 | **FAIL katastrofik** @256MB (checkpoint pertama) | `BCFN(2+0,13-2,T)` R=+66.1, p=2.1e-33, FAIL!!! |

Kedua titik K FAIL parah di checkpoint PALING KECIL yang diuji (256MB) —
tidak ada ambiguitas, tidak perlu run lebih besar untuk konfirmasi
(fail-fast, konsisten dengan metodologi
`k-threshold-characterization/RESULTS.md`).

Log lengkap: `practrand_addrhi_K1_8GB.log`, `practrand_addrhi_K255_8GB.log`.

## Kesimpulan

**Pola addr_hi-increment chaining (key tetap, addr_hi increment per block)
TIDAK AMAN dipakai** untuk fitur shuffle N>255 di pip-ra-core, pada K
manapun yang diuji (kecil maupun besar). Sesuai aturan
[[feedback_no_reseed_on_fail]]: FAIL = algoritma/pola gagal, full stop,
tidak dicoba-coba seed lain.

Ini BUKAN regresi pada `ra_core_singleblock`/`ra_core_orbit` itu sendiri —
`addr_hi` sebagai parameter tunggal (misal `generate(key, addr_hi, n)` untuk
random-access satu panggilan) tetap valid dan sudah tervalidasi terpisah
(lihat addr_hi 64-bit widening promotion). Yang FAIL secara spesifik adalah
pola **chaining berurutan** lewat addr_hi (0,1,2,3,...) untuk key yang sama
— pola ini tampaknya mengekspos korelasi struktural antar-block yang mirip
BCFN, sama seperti pola key-increment lama sebelum fix K-threshold
diterapkan.

## Dampak ke plan pip-ra-core

Fitur shuffle N>255 di Fase 1/2 **harus direvisi**: pakai chaining
KEY-increment (Weyl `0x9E3779B9`, `addr_hi` tetap 0) — pola yang SUDAH
tervalidasi bersih sampai K=1 pada formula `ra_core.c` saat ini (lihat
`PRODUCTION_READINESS_HANDOVER.md` baris status matrix: "0 FAILED (K=1/K=96
standalone, K=1/K=255 battery gate)" untuk `ra_core_singleblock`, dan
addr_hi-widening promotion terverifikasi byte-identik terhadap versi
pre-widening saat `addr_hi=0` — jadi validasi lama itu masih berlaku untuk
`ra_core.c` yang sekarang).
