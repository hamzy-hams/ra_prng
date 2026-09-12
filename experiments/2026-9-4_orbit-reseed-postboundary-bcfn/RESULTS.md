# RESULTS (2026-09-06) — post-reseed BCFN test: PASS

## Tujuan

Menguji apakah `ra_reseed`/`ra_hash` (dipanggil `ra_core_orbit` tiap 255
kata) membocorkan struktur yang terdeteksi PractRand ke siklus PERTAMA
setelah satu reseed — pertanyaan yang belum pernah terjawab sejak
`HANDOVER.md` didesain 2026-09-04 (lihat file itu untuk metodologi
lengkap: kenapa `fmemopen`+offset dipakai, kenapa buffer harus
`+1` byte, kenapa `rng=257` diperlukan untuk memaksa satu reseed).

## Kenapa baru selesai sekarang (bukan 2026-09-04)

Sesi 2026-09-04 BLOCKED sebelum sempat menjalankan test intinya: kontrol
sanity wajib (`HANDOVER.md` §2.3) untuk pola pra-reseed K=1+hashed-key
sendiri FAIL parah — ternyata defect BERBEDA (birthday-paradox key
32-bit, bukan `ra_reseed`/`ra_hash`), didiagnosis penuh di sesi-sesi
setelahnya dan akhirnya diperbaiki 2026-09-06 lewat widening `addr_hi`
(`../2026-9-5_orbit-address-widen-64bit/` Kandidat D, dipromosikan ke
`ra_core.c` di `../2026-9-6_orbit-address-widen-64bit-promotion/`).

## Temuan baru sesi ini: driver lama tidak mengaktifkan fix `addr_hi`

Sebelum menjalankan ulang, ditemukan bahwa `multikey_stream_orbit_
postreseed_k1.c`/`_k255.c` (dibuat 2026-09-04, sebelum `addr_hi` ada)
memanggil `ra_core_orbit(key, 0u, rng, ...)` — `addr_hi` HARDCODE 0 di
semua 4 mode (`--stream`, `--stream-hashed`, `--control`,
`--control-hashed`). Battery yang benar-benar menutup gap K=1+hashed
(`../2026-9-5_orbit-address-widen-64bit/multikey_stream_addr_hi.c`)
memvariasikan **key DAN addr_hi lewat dua splitmix32-chain independen** —
`addr_hi` konstan 0 sepanjang run membuat `keyterm_l`/`keyterm_m` cuma
dikalikan konstanta tetap (`fmix32(GUARD_L2^0)`, dst.), jadi collision
antar-key tetap murni ditentukan oleh 32-bit `key` saja — TIDAK
mengaktifkan perbaikan 64-bit-effective-address sama sekali.

**Fix**: kedua driver diupdate (2026-09-06) untuk memvariasikan `addr_hi`
independen dari `key`, pola sama seperti `multikey_stream_addr_hi.c`:
- mode weyl: `addr_hi = base_addr_hi + block_idx*0x85EBCA6Bu` (delta beda
  dari delta key `0x9E3779B9u`)
- mode hashed: `addr_hi` dari chain `splitmix32` terpisah (state awal
  `0xDEADBEEFCAFEULL ^ base_addr_hi`, beda dari chain key `0xC0FFEE1234ULL
  ^ base_key`)

CLI kedua driver berubah dari `{mode} <base_key> <n>` jadi
`{mode} <base_key> <base_addr_hi> <n>`.

## Metodologi eksekusi

Dijalankan di VPS (`103.93.160.34`, lihat `VPS_ACCESS.md` di root repo),
bukan laptop, karena run 8GB K=1 di sini jauh lebih mahal dari biasanya:
tiap kata post-reseed yang disimpan butuh `ra_core_orbit(key, addr_hi,
257, ...)` penuh (255 kata siklus pra-reseed yang dibuang + reseed + 2
kata siklus baru, disimpan cuma 1) — throughput efektif turun ~257x
dibanding K=1 biasa. K=1 8GB makan waktu ~21500 detik (~6 jam) per run;
K=255 jauh lebih murah (buang rasio ~2x, bukan ~257x) sehingga selesai
dalam ~214-218 detik.

Build fresh di VPS dari `ra_core.c` kanonik saat ini (`BUILD.sh`,
`-Dmain=ra_core_unused_main` lalu link), diverifikasi **byte-identik**
dengan build lokal (gcc 16.1.1 laptop vs gcc 14.2.0 VPS) via `md5sum`
sebelum dipercaya. PractRand dijalankan `stdin32 -tlmin <awal> -tlmax
<akhir> -multithreaded`, sama seperti standar battery lain di repo ini.

**Insiden teknis sepanjang jalan** (dicatat untuk transparansi, tidak
mempengaruhi validitas hasil akhir): percobaan pertama 4 run utama pakai
`n=999999999` (~3,72 GiB output) — kurang dari target 8GB, jadi keempat
run mati sendiri (EOF bersih, "error reading standard input") begitu
mendekati 2GB. Diperbaiki dengan `n=6000000000` (~22 GiB, margin aman) dan
dijalankan ulang dari nol. Terpisah, satu percobaan `setsid`+`</dev/null`
untuk "mengamankan" proses dari koneksi SSH yang terputus malah bikin
generator mati instan — dibatalkan, kembali ke pola `nohup ... | nohup
... & disown` yang sudah terbukti jalan di run-run lain di VPS ini.

## Hasil: kontrol sanity (2GB, wajib per HANDOVER.md §2.3)

Dengan `addr_hi` divariasikan dengan benar, kontrol pra-reseed (pola yang
identik dengan `../2026-9-4_orbit-kmin-battery/`, sudah terbukti bersih)
**bersih di keempat kombinasi**:

| Run | Checkpoint akhir | Waktu | Anomali |
|---|---|---:|---|
| `control_k1_weyl` | 2GB | 91.3s | tidak ada |
| `control_k1_hashed` | 2GB | 97.1s | 1 blip terisolasi @256MB (`BCFN(2+2,13-4,T)` R=-6.9), hilang di 512MB |
| `control_k255_weyl` | 2GB | 36.7s | tidak ada |
| `control_k255_hashed` | 2GB | 36.8s | tidak ada |

Ini mengonfirmasi: dengan `addr_hi` divariasikan dengan benar, gap
K=1+hashed-key yang FAIL parah di 2026-09-04 **tidak lagi muncul** —
konsisten dengan klaim "gate closed" dari promosi `addr_hi` 2026-09-06.
Kontrol bersih -> boleh lanjut ke run utama (per aturan HANDOVER.md §2.3).

## Hasil: 4 run utama post-reseed (8GB, target HANDOVER.md §2.4)

| Run | Checkpoint akhir | Waktu total | Anomali |
|---|---|---:|---|
| `main_k1_weyl` | **8GB** | 21530s (~5,98j) | 1 blip terisolasi @4GB (`mod3n(5):(2,9-6)` R=-4.3), **hilang di 8GB** |
| `main_k1_hashed` | **8GB** | 21518s (~5,98j) | tidak ada, di semua checkpoint |
| `main_k255_weyl` | **8GB** | 218s | 1 blip terisolasi @256MB (`BCFN(2+1,13-5,T)` R=+9.5, `Gap-16:B` R=-4.3), **hilang di 512MB** |
| `main_k255_hashed` | **8GB** | 214s | tidak ada, di semua checkpoint |

Total 3 blip "unusual" terisolasi ditemukan lintas 8 run (kontrol+utama) —
semuanya di SATU checkpoint, semuanya hilang total di checkpoint
berikutnya, tidak satu pun eskalasi. Ini pola noise-satu-titik yang sudah
dikenal di repo ini (lihat `control_k255_hashed` blip di
`experiments/2026-9-4_orbit-reseed-postboundary-bcfn/STATUS.md` versi lama
untuk contoh serupa sebelumnya), BUKAN pola BCFN eskalatif (severity naik
tajam antar-checkpoint) yang jadi kriteria FAIL di `HANDOVER.md` §3.

Log mentah lengkap kedelapan run: `vps_logs_20260906/*.log`.

## Verdict

**PASS.** Kriteria `HANDOVER.md` §3 terpenuhi: bersih (tidak ada anomali
eskalatif) sampai minimal 8GB di semua 4 run utama (K=1 dan K=255, kedua
urutan key). `ra_reseed`/`ra_hash` **tidak** membocorkan struktur
terdeteksi ke siklus pasca-reseed pada skala yang diuji.

Item ini ditutup sebagai **resolved** di
`PRODUCTION_READINESS_HANDOVER.md` §2b/§6 — tidak ada tindakan lanjutan
yang diperlukan untuk `ra_reseed`/`ra_hash` sebelum promosi `src/`.

## Stretch goal (§5 HANDOVER.md, opsional, TIDAK dikerjakan sesi ini)

Pertanyaan reseed KEDUA/KETIGA berturut-turut (menggeser jendela buang ke
`rng=510+K` dst.) tetap belum diuji — dicatat sebagai follow-up independen
opsional, bukan blocker, sesuai HANDOVER.md §5.
