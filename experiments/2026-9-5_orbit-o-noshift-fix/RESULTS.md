# Hasil: `o` sebagai XOR 8 word mentah (tanpa bit-shift per-tap)

## Hipotesis

Production menghitung `o` (window 8-tap dari `M[]`) dengan menggeser tiap
tap sebelum XOR (`M[i+k] << k` untuk k=0..7) -- staggering ini mungkin
sengaja untuk menghindari kanselasi berkorelasi antar posisi bit yang sama
di 8 word. Kandidat ini menghapus shift sepenuhnya: `o` = XOR 8 word mentah
tanpa bobot posisi apa pun, untuk menguji apakah staggering itu justru
sumber (atau penangkal) defect K=1+hashed.

## Implementasi

`my_transform` di `candidates/o_noshift.c` sama persis dengan
`../2026-9-5_orbit-singleblock-shared-baseline/candidates/baseline.c`
kecuali baris `o`:

```c
uint32_t o = M[(uint8_t)(255+0)] ^ M[(uint8_t)(255+1)] ^ ... ^ M[(uint8_t)(255+7)];
```

Rolling update per ronde (bukan rekomputasi penuh, sesuai keputusan cakupan
sesi ini) diturunkan ulang untuk skema tanpa-shift dan **diverifikasi
brute-force untuk 255 ronde sebelum dipakai**:

```c
o_baru = o_lama ^ M[(uint8_t)(i+7)] ^ M[(uint8_t)(i-1)]  // keluar/masuk jendela
```

`my_init` tidak diubah (verbatim `ra_init_state_orbit`).

## Hasil

- **Tier 0 avalanche**: PASS (mean_fraction=0.5002, min_bit_fraction=0.4907
  -- identik dengan baseline, wajar karena avalanche gate ini tidak
  sensitif ke perubahan `o` sekecil ini).
- **Tier 1 PractRand, K=1** (sesuai cakupan yang disepakati: K=1 saja,
  Weyl+hashed):
  - **Weyl**: CLEAN sampai 2GB (kedua core, orbit & singleblock).
  - **hashed**: **FAIL** -- `FPF-14+6/16:all R=+8.7` ("very suspicious",
    early-kill) @512MB, kedua core. Signature dan skala kegagalan sebanding
    dengan defect asli (R=+7.8 @512MB versi produksi).

## Kesimpulan

**Hipotesis staggering-shift REFUTED.** Menghapus bit-shift per-tap pada
`o` tidak mengubah perilaku defect sama sekali -- K=1+hashed tetap FAIL
dengan keluarga anomali dan skala yang sama, K=1+Weyl tetap bersih. Cara
menggabungkan 8 tap `o` (shift vs raw XOR) BUKAN faktor penentu defect ini.
Ini konsisten dengan kesimpulan diagnosis `/dev/urandom`-key sebelumnya
(`../2026-9-5_orbit-k1-urandom-key-diagnosis/RESULTS.md`): akar masalah
kemungkinan besar bukan di formula `o` itu sendiri, melainkan di titik
kombinasi `a=(d^o)^(cons+a)` atau lebih dalam lagi -- catatan yang sama
juga muncul di sesi `orbit-o-fmix-fix` (2026-09-04).

`ra_core.c` tidak disentuh sama sekali oleh eksperimen ini.

## File

- `candidates/o_noshift.c` -- kandidat, konvensi dual-core.
- `candidates/tier1_out/` -- log PractRand mentah + `battery_summary.json`.
