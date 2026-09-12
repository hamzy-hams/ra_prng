# Hasil: K=1 orbit dengan key dari `/dev/urandom` (proxy TRNG)

## Pertanyaan

Defect K=1+hashed-key pada `ra_core_orbit` (`FPF-14+6/16`, terdokumentasi di
`../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`) -- apakah
disebabkan oleh **struktur aljabar rangkaian key** SplitMix32 (input
berurutan `base+0, base+1, ...` yang dihash) beresonansi dengan formula init
`ra_core`, atau **intrinsik ke matematika internal `ra_core_orbit`** sendiri
(muncul untuk hampir semua sumber key acak, bukan cuma hash-chain
tertentu)?

Cara memisahkan: ganti sumber key per-blok (K=1, reinit tiap word) dari
SplitMix32 ke `/dev/urandom` -- CSPRNG kernel yang tidak berhubungan secara
aljabar dengan konstanta `ra_core` maupun SplitMix32 sama sekali. `ra_core.c`
TIDAK diubah; hanya sumber key di driver eksternal (`urandom_key_stream_orbit.c`)
yang diganti.

## Sanity check (sebelum PractRand)

- `n=1` -> tepat 4 byte output, tidak crash.
- Dua run `n=8` menghasilkan output berbeda total (bukti key benar-benar
  diambil dari `/dev/urandom`, bukan cache/tetap).
- Build: `gcc -O3 -march=native -std=gnu17 -include stdalign.h
  urandom_key_stream_orbit.c ra_core_nomain.o -o urandom_key_stream_orbit`
  (linking langsung ke `ra_core.c` produksi via nomain object, pola sama
  seperti `../2026-9-4_orbit-kmin-battery/diag_orbit_k1_hashed.c`).

## Hasil PractRand (`run_urandom_key_A.log`, 1 run sampai 2GB)

| Skala | Evaluation | R (FPF-14+6/16) |
|---|---|---|
| 512MB | mildly suspicious | +5.8 |
| 1GB | VERY SUSPICIOUS | +12.0 |
| 2GB | **FAIL** | +28.4 |

Test family yang muncul (`FPF-14+6/16`) **identik** dengan defect asli
K=1+hashed, dengan bentuk eskalasi yang sama (memburuk seiring skala naik).
Perbandingan langsung dengan `ADDENDUM_HASHED_KEY_GAP.md` (base_key
`0x12345`):

| Skala | urandom-key (sesi ini) | hashed-key (SplitMix32, asli) |
|---|---|---|
| 512MB | R=+5.8 (mildly suspicious) | R=+7.8 (suspicious) |
| 1GB | R=+12.0 (very suspicious) | R=+18.7 (**FAIL**) |
| 2GB | R=+28.4 (**FAIL**) | R=+31.2 |

Sesuai `feedback_no_reseed_on_fail`: FAIL sudah muncul jelas di satu run
(`/dev/urandom` inheren tanpa seed tetap -- setiap run otomatis independen),
tidak perlu run ulang untuk "menjelaskan" FAIL ini.

## Kesimpulan

**BUKAN resonansi SplitMix32 x formula init.** Key dari `/dev/urandom` --
tanpa hubungan aljabar apa pun ke SplitMix32 maupun konstanta `ra_core` --
tetap memicu `FPF-14+6/16` dengan pola eskalasi yang sama dan berujung FAIL
di skala yang sebanding. Ini bukti kuat bahwa formula internal
`ra_core_orbit` (init `M[]`/`L[]` + cycle transform, khususnya pola K=1:
reinit tiap 1 word) **sendiri** menciptakan pola statistik dari input key
acak murni -- bukan menghapus pola, malah menambahkannya.

Implikasi untuk status Weyl-ordering yang selama ini "bersih"
(`ADDENDUM_HASHED_KEY_GAP.md`, `feedback_orbit_singleblock_benchmark_standard`
dkk.): Weyl kemungkinan besar bukan kebal terhadap defect ini, melainkan
langkah linearnya (`key = base_key + idx*0x9E3779B9`) kebetulan tidak (atau
sangat jarang) memicu kombinasi key yang memunculkan pola `FPF-14+6/16` --
sementara hampir semua sumber key lain (hash-chain SplitMix32 ATAU
`/dev/urandom` murni) memicunya. Pertanyaan "kenapa Weyl istimewa" tetap
terbuka, tapi kerangkanya sekarang terbalik: bukan "kenapa hashed rusak",
tapi **"kenapa Weyl -- dari semua kemungkinan sumber key -- yang justru
menghindari mode kegagalan yang tampaknya intrinsik ke `ra_core_orbit`
K=1"**.

`ra_core.c` tidak disentuh sama sekali oleh eksperimen ini.

## File

- `urandom_key_stream_orbit.c` -- driver, linked ke `ra_core.c` produksi
  as-is.
- `run_urandom_key_A.log` -- log PractRand mentah (satu run, sampai 2GB).
