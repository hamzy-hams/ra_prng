# HANDOVER: karakterisasi ekshaustif fungsi round-1 K=1 (`ra_core_orbit`)

## Untuk sesi baru yang menjalankan ini

Dokumen ini murni handover -- **tidak ada kode yang ditulis atau dijalankan
di sesi yang menulis dokumen ini.** Baca seluruh dokumen dulu, lalu jalankan
sesuai urutan prioritas di bagian "Tiga analisis" sebelum menulis kode
apa pun.

## Kenapa ini ada (jangan ulangi metode lama)

Defect: `ra_core_orbit` K=1 (reinit tiap word) FAIL PractRand (`FPF-14+6/16`)
untuk key hashed (SplitMix32) DAN key `/dev/urandom` (murni acak, tidak
berhubungan aljabar dengan apa pun), tapi tetap CLEAN untuk key Weyl-ordering
(`key = base + idx*0x9E3779B9`).

**10 axis sudah dicoba, SEMUA memakai metode yang sama** ("tebak formula
baru → jalankan PractRand → gagal → ulang"), dan semua gagal menutup
defect ini:
1. Finalizer-on-`c` 32-combo search (8 init x 4 finalizer)
2. Init x cycle 8x4 combo search (varian lain)
3. `d0_hash` (seed `d` dihash dari key)
4. `d0_realround` (buang 1 ronde asli sebagai warmup)
5. Weyl-constant-resonance (ganti konstanta Weyl test-harness)
6. `fmix32(o)` init-only
7. `fmix32(o)` every-round
8. MUL-combine (`a=(d*o)^(cons+a)`, REJECT di Tier 0 karena `d=0` melenyapkan `o`)
9. `d0_raw_cons` (d dari cons mentah, tanpa hash)
10. Full architecture redesign (Weyl-add state + fmix32/triple32 finalizer) -- REGRESI baru di tempat lain
11. `o` sebagai raw XOR 8 word tanpa bit-shift (rolling-window diverifikasi brute-force, tetap FAIL)

Detail lengkap tiap axis: memory `project_orbit_key_avalanche_diagnosis.md`
dan `project_singleblock_kmin_defect.md`, plus file `RESULTS.md` di folder
eksperimen masing-masing (lihat daftar referensi di bagian akhir).

**User secara eksplisit frustrasi dengan pola ini** -- benar, karena kita
tidak pernah benar-benar MENGUKUR objek matematisnya, cuma menebak lalu
menunggu PractRand sebagai kotak hitam bilang lolos/gagal.

## Ide baru: `f(key)` bisa dikarakterisasi ekshaustif, bukan ditebak

Untuk K=1 (`rng=1`), SELURUH output word pertama adalah **fungsi
deterministik murni dari key 32-bit**: `f(key) -> word`. Domainnya cukup
kecil (2^32 ≈ 4,3 miliar) untuk dihitung ekshaustif atau nyaris ekshaustif
secara langsung -- bukan ditebak-tebak lagi dan divalidasi lewat statistik
PractRand yang berat.

### 1. Derivasi persis `f(key)` (WAJIB diverifikasi byte-exact dulu)

Dari `../2026-9-1_family-productionization/ra_core.c` (baca file ini
langsung untuk konfirmasi, jangan percaya transkripsi di bawah tanpa cek):

- `ra_core_orbit` (L254-270): `cons=key` (L261), lalu panggil pertama
  `ra_permutation_cycle_orbit(cons=key, it=0, M, L, &count, raw_stream)` (L265).
- `ra_permutation_cycle_orbit` (L217-252), state awal (L220):
  `a=cons=key`, `b=(uint32_t)it=0`, `c=0`, `d=0`.
- `o` di ronde `i=255` (L222-225) cuma butuh
  `M[(uint8_t)(255+0)]` s.d. `M[(uint8_t)(255+7)]` = `M[255],M[0],M[1],...,M[6]`
  (8 word, mod 256 via cast `uint8_t`). **`L[]` TIDAK dibutuhkan sama
  sekali** untuk output ronde-1 (baru dibaca `ra_reseed` di batas block
  berikutnya, dead code untuk pertanyaan ini).
- `M[idx]` per `ra_init_state_orbit` (L197-207):
  `keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key))` (L199, `GUARD_M=0x6c26fc92u`),
  `M[idx] = rot32((uint32_t)(idx * 0x06a0dd9bu) * keyterm_m, key ^ (uint32_t)idx)` (L201-205).
- Body ronde-1 (L228-232):
  ```
  a = (d ^ o) ^ (cons + a);      // = (0^o) ^ (key+key) = o ^ (2*key)
  b = (cons + a) ^ (o + d);      // = (key + a) ^ o
  c = rot32((a >> 13) ^ a, b);
  c ^= c >> 17u;
  ```
  `c` inilah output K=1 (`fwrite`, L234, lalu `count<=1` langsung `break`/return
  di `ra_core_orbit` L266 -- tidak ada ronde ke-2 untuk `rng=1`).

**Sebelum menjalankan analisis skala besar apa pun**: implementasikan
`f(key)` ini sebagai fungsi C mandiri (cuma hitung 8 index `M[]` yang
dibutuhkan, TIDAK perlu array 256 word penuh, TIDAK perlu `L[]` sama
sekali), lalu **byte-verify terhadap `ra_core_orbit(key, 1, ...)` produksi
untuk minimal beberapa ratus key acak** sebelum dipercaya (sesuai
`feedback_verify_external_baselines` di memory) -- bug derivasi di sini
akan diam-diam merusak semua hasil analisis besar di bawah.

Rot32/fmix32 (perlu di-copy verbatim juga): `ra_core.c` L108-118 (cek baris
persis saat mengerjakan; jangan retik dari ingatan, ambil dari file
langsung).

### 2. Tiga analisis, urutan prioritas ini

1. **Sapuan injektif/collision ekshaustif** (PALING PRIORITAS, paling cepat,
   paling definitif): sapu praktis semua `key` di `[0, 2^32)` (atau subset
   sangat besar acak-seragam kalau full sweep ternyata kelamaan), tandai
   tiap output word di bitmap 512MB (1 bit per kemungkinan nilai 32-bit).
   Laporkan: berapa banyak nilai output yang TIDAK PERNAH muncul, berapa
   yang muncul, apakah ini bijeksi persis atau tidak. Non-bijeksi = bukti
   **matematis langsung**, bukan statistik -- entropi hilang secara
   terbukti kalau ada nilai yang tidak pernah/lebih sering muncul.
   Estimasi: ~4,3 miliar evaluasi fungsi ~10 instruksi, perkirakan 1-3
   menit single-thread di CPU modern (konfirmasi dulu throughput aktual
   lewat sampel kecil yang di-timing sebelum commit ke run penuh).
2. **Histogram frekuensi byte-pattern** pada sampel besar (mis. 1-10 miliar
   key, atau reuse sapuan ekshaustif di atas): hitung langsung frekuensi
   pola byte/tuple spesifik yang ditarget test `FPF-14+6/16` PractRand
   (fixed-pattern frequency, panjang tuple ada di nama testnya -- cek
   sumber/dokumentasi PractRand langsung untuk definisi persis test ini,
   jangan menebak), supaya tahu PERSIS pola mana yang over/under-represented
   -- bukan cuma percaya PractRand sebagai oracle pass/fail buta.
3. **Uji korelasi pembatalan-Weyl**: hitung langsung korelasi/distribusi
   gabungan `(f(key), f(key + 0x9E3779B9))` di sampel besar, untuk menguji
   hipotesis yang masih berdiri (langkah linear Weyl bikin bias ronde-1
   saling membatalkan sistematis antar-blok berurutan, sesuatu yang key
   independen -- hashed ATAU `/dev/urandom` -- tidak bisa lakukan).
   Bandingkan dengan `(f(key), f(key + delta_acak))` sebagai kontrol.

### 3. Larangan eksplisit / rambu untuk sesi baru

- **JANGAN langsung mengusulkan/menguji kandidat fix formula baru** sebagai
  langkah pertama -- itu metode yang sudah terbukti gagal 10-11 kali.
  Baru pertimbangkan fix TARGETED setelah analisis ini menghasilkan
  mekanisme yang konkret dan terukur, dan itu pun **tanya dulu ke user**
  soal cakupan/pendekatan SEBELUM menulis kode (lihat memory
  `feedback_ask_more_before_implementing` dan
  `feedback_ask_more_on_design_decisions` -- tanya sebelum implementasi,
  bukan cuma setelah hasil keluar).
- Jangan mengulang salah satu dari 11 axis yang sudah exhausted di atas.
- `ra_core.c` TIDAK BOLEH diubah oleh analisis ini (murni karakterisasi
  read-only, sama disiplinnya dengan semua sesi diagnostik sebelumnya).
- Kutip file:line PERSIS dari `ra_core.c` untuk tiap formula yang
  direproduksi (baca ulang dari source, jangan percaya transkripsi di
  dokumen ini tanpa verifikasi -- nomor baris bisa saja sudah bergeser).

### 4. Referensi

- `../2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md` -- temuan dilution-curve awal.
- `../2026-9-5_orbit-k1-urandom-key-diagnosis/RESULTS.md` -- bukti defect intrinsik, bukan spesifik hash-chain.
- `../2026-9-5_orbit-o-noshift-fix/RESULTS.md` -- axis ke-10/11 (terbaru) yang gagal.
- Memory: `project_orbit_key_avalanche_diagnosis.md` (riwayat axis lengkap), `project_singleblock_kmin_defect.md`.
- `../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md` -- angka defect asli untuk perbandingan.
- `../../tools/battery/` -- tooling Tier0/Tier1 generik kalau setelah analisis ini ternyata perlu uji kandidat fix (dual-core convention, baca `README.md`-nya).

## Status

Belum ada kode ditulis/dijalankan untuk analisis ini. Murni dokumen
perencanaan untuk dieksekusi di sesi terpisah.
