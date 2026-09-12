# orbit-round1-collision-position-isolation -- hasil (diagnosis only)

Uji hipotesis: apakah defect `FPF-14+6/16` K-small terkonsentrasi tepat di
round pertama tiap siklus reseed (`i=255`, di mana `a` diinisiasi `=cons`
sehingga `a = (0^o)^(cons+cons) = o^(2*cons)` -- self-collision karena `a`
dan `cons` mulai identik), bukan tersebar merata di semua posisi round.

**Ini sesi diagnosis murni -- TIDAK ada kandidat fix ditulis, `ra_core.c`
tidak disentuh.**

## 1. Driver `position_split_orbit.c` -- verifikasi wajib

- Mode normal (`--stream-hashed`) byte-exact vs control binary produksi
  yang sudah terverifikasi (`../2026-9-4_orbit-init-warmup-fix/candidates/
  control`): **MATCH** untuk K=2/n=2000 dan K=255/n=5000.
- Mode split (`--stream-hashed-split`): rekonstruksi interleave
  (`round1_path` + `rest_path` disusun ulang sesuai posisi asli) **MATCH**
  byte-exact dengan output mode normal, untuk K=2 dan K=255. Membuktikan
  demuxing cuma routing output ke file berbeda, TIDAK mengubah nilai apa
  pun yang dihitung.

## 2. Stage 1: K=2, hashed, base_key=0x12345, staged 64MB->1GB per substream

K=2 dipilih karena tiap panggilan `ra_core_orbit` (fresh init per key)
cuma menghasilkan 2 kata: round1 (`i=255`, kena collision `a=o^(2*cons)`)
dan round2 (`i=254`, TIDAK pernah kena collision itu -- `a` di round2
sudah bergantung pada `d` dari `c` round1, bukan lagi `cons` murni via
`a` awal). Split 50/50, generation cost murah (536,870,912 kata total,
22 detik).

| Substream | 64MB | 128MB | 256MB | 512MB | 1GB |
|---|---|---|---|---|---|
| **round1-only** (`i=255`) | clean | clean | clean | mildly suspicious (R=+7.8) | **FAIL** (`FPF-14+6/16:all` R=+18.7, p=5.1e-17) |
| **rest-only** (`i=254`, "round2") | clean | clean | clean | mildly suspicious (R=+6.3) | **FAIL** (`FPF-14+6/16:all` R=+17.0, p=2.0e-15) |

Log lengkap: `tier_logs/round1_k2.log`, `tier_logs/rest_k2.log`.

## 3. Interpretasi: hipotesis collision REFUTED sebagai penjelasan tunggal

**Kedua substream FAIL dengan magnitude yang nyaris sama** (R=+18.7 vs
+17.0, selisih dalam rentang noise sampel di skala ini). Ini BUKAN pola
yang diprediksi hipotesis "round1 unik ternoda karena `a=cons` self-
collision": prediksinya adalah round1 FAIL sementara round2 (yang secara
aljabar TIDAK pernah mengalami collision `a=cons` murni -- di round2,
`a` sudah tercampur dengan `d` dari output round1) tetap CLEAN. Observasi
menunjukkan sebaliknya: round2 sama parahnya dengan round1.

**Kesimpulan**: collision `a=o^(2*cons)` di round-1 adalah properti
aljabar yang nyata dan terverifikasi (lihat plan/diskusi sebelumnya), tapi
**TERBUKTI BUKAN satu-satunya sumber** defect statistik ini -- setidaknya
tidak dalam pengertian sempit "isolasi round1 vs round2". Yang lebih tepat
menjelaskan data: defect terkait dengan kedekatan pada BATAS INIT SEGAR
secara umum (baik round1 MAUPUN round2 sama-sama dekat batas itu, cuma
1-2 langkah dari `ra_init_state_orbit`), bukan spesifik ke satu titik
kombinasi `a=cons` tunggal. Ini konsisten dengan (dan memperhalus, bukan
menggantikan) diagnosis lama "defect terkonsentrasi di kata pertama
setelah init segar, dilusi ~1/K" ([[project_orbit_key_avalanche_diagnosis]])
-- cuma sekarang jelas radius "dekat init" itu mencakup minimal 2 posisi
pertama, bukan cuma posisi pertama tunggal.

Catatan: round1 R=+18.7 sedikit lebih tinggi dari round2 R=+17.0 -- tidak
cukup bukti untuk klaim "round1 tetap sedikit lebih parah dari round2"
pada skala 1GB ini (selisih plausibel noise), tapi juga tidak bisa
langsung disingkirkan tanpa data lebih banyak/posisi lebih halus.

## 4. Status Stage 2 (K=255 asli)

**BELUM dijalankan** -- sesuai plan, Stage 2 kondisional pada Stage 1
confirmatory DAN persetujuan user. Karena Stage 1 hasilnya REFUTE
(bukan confirm) hipotesis sempit "round1 unik", pertanyaan untuk
didiskusikan dengan user sebelum lanjut apa pun: apakah perlu peta
per-posisi lebih halus (K=4/K=8, isolasi posisi 1/2/3/4/... satu-satu)
untuk melihat bentuk kurva "seberapa jauh dari init radius defect ini
meluas" -- BUKAN keputusan otomatis sesi ini.

## 5. `ra_core.c`

Tidak disentuh. Tidak ada kandidat fix baru ditulis sesi ini -- murni
data diagnosis posisi.
