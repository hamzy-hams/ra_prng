# HANDOVER: baseline dual-core (orbit+singleblock) siap-edit

## Kenapa folder ini ada

User mau mulai edit algoritma sendiri, dengan syarat: satu fungsi inti yang
diedit sekali otomatis berlaku untuk mode orbit DAN singleblock, supaya
`run_battery.py` sekali jalan langsung menunjukkan di kombinasi mana yang
gagal — tanpa risiko lupa mengedit salah satu dari dua fungsi terpisah.

## Kenapa ini feasible (dibuktikan dari ra_core.c, bukan asumsi)

`ra_permutation_cycle_orbit` dan `ra_permutation_cycle_singleblock` di
`../2026-9-1_family-productionization/ra_core.c` (L217-252, L324-358)
**identik byte-demi-byte** di badan per-round transform. Satu-satunya beda:
orbit melakukan swap `L[i]<->L[d]` tepat setelah cek `count<=1`;
singleblock melewatinya (komentar eksplisit ra_core.c:348-349). `L[]`
write-only dalam satu block — cuma dibaca `ra_reseed()` di block
berikutnya, tidak pernah memengaruhi `a/b/c` pada block yang sama. Ini
kenapa `ORBIT_KAT_CHECKSUMS == SINGLEBLOCK_KAT_CHECKSUMS` byte-for-byte
untuk semua `rng` 1..255 (ra_core.c:496-497 comment) — jaminan struktural,
bukan kebetulan.

`candidates/baseline.c` mengekspresikan ini sebagai satu `my_init` + satu
`my_transform`, parameter-kan lewat `L` nullable, dipakai lewat
`tools/battery/driver_main_template.c`'s mode dual-core (`CORE_INIT`/
`CORE_TRANSFORM`). Lihat `tools/battery/README.md` bagian "Mode dual-core"
untuk detail kontrak.

## Verifikasi yang sudah dijalankan (sebelum dipakai edit)

1. KAT parity byte-per-byte terhadap `ra_core.c` asli: `orbit --single`,
   `singleblock --single` (rng<=255), `orbit --stream` K=1 weyl (vs
   `multikey_stream_orbit_k1` historis), `singleblock --stream-hashed`
   K=255 (vs `ra_core_singleblock` langsung) — semua MATCH.
2. `singleblock --single rng>255` ditolak bersih (bukan crash),
   `exit=1`.
3. Tier 0 avalanche `--core orbit` vs `--core singleblock` pada baseline
   yang belum diedit: angka identik (`mean_fraction=0.4988,
   min_bit_fraction=0.4810`) — membuktikan desain `L`-nullable tidak diam-
   diam mengubah perilaku K<=255.
4. `run_battery.py --dual-core --k 1 --orderings hashed --sizes
   512M,1G,1G`: **KEDUA** core FAIL di 1GB dengan signature identik
   (`FPF-14+6/16:all R=+18.7`) — reproduksi temuan sesi ini bahwa defect
   K=1+hashed BUKAN eksklusif milik orbit, cuma belum pernah diuji untuk
   singleblock sebelumnya (lihat catatan di bawah).

## Temuan penting yang melatarbelakangi baseline ini

Sebelum sesi ini, status riset bilang "singleblock K=1 bersih, orbit K=1
FAIL (khusus hashed-ordering)" — implikasinya seolah singleblock kebal
terhadap defect ini. Investigasi sesi ini (dipicu pertanyaan user, kenapa
K=1 harus identik secara aljabar antara dua core) menemukan: kombinasi
**singleblock + K=1 + hashed-ordering TIDAK PERNAH DIUJI** di repo ini
sebelumnya (`multikey_stream_singleblock_k1.c` cuma Weyl-ordering;
`multikey_stream_hashed.c` cuma K=255). Setelah diuji manual, singleblock
K=1+hashed FAIL identik dengan orbit (`FPF-14+6/16:all R=+18.7` @1GB, angka
sama persis) — sesuai prediksi aljabar di atas.

Konsekuensi: defect ini bukan milik `ra_core_orbit`, ini defect di formula
transform bersama (`w8_f10_i0`) untuk pola "reinit tiap 1 word dengan key
hasil hash/avalanche". Baseline dual-core ini memastikan setiap kandidat
fix ke depan otomatis diuji di KEDUA core, tidak akan lagi terjadi celah
"satu core belum sempat diuji".

## Status

Baseline belum diedit — murni titik awal. Belum ada kandidat fix baru di
folder ini. Lihat `candidates/baseline.c` untuk mulai edit `my_init`/
`my_transform`.
