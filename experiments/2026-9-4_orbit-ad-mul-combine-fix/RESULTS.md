# orbit-ad-mul-combine-fix -- hasil

Kandidat tunggal (`candidates/ad_mul_combine.c`): satu perubahan yang
diminta user, `a = (d ^ o) ^ (cons + a)` -> `a = (d * o) ^ (cons + a)`,
sebagai axis baru distinct dari `orbit-o-fmix-fix` (mixing pada input `o`)
dan 32-combo search (mixing/varying pada output `c`) yang sudah terbukti
tidak mengubah signature defect. Motivasi: teknik multiply-combine
terbukti sukses menutup defect afin-dalam-key yang berbeda di
`multikey_remix_search` (2026-09-01) saat `+`/`^` combine gagal.

`o` (8-tap dari `M[]`) TIDAK dimixing (tetap formula produksi mentah,
rolling-register asli) -- berbeda dari eksperimen `o-fmix-fix` sebelumnya.
`ra_init_state_orbit`, `ra_hash`, `ra_reseed` semua verbatim produksi.
`ra_core.c` kanonik TIDAK disentuh.

## 1. Byte-exact vs oracle Python (`combo_prng_admul.py`) -- WAJIB sebelum statistik

| Kasus | Hasil |
|---|---|
| `--single 1 1` (1 kata) | C=16, oracle `orbit_round1(1)`=16 -- **MATCH** |
| `--single 12345 300` (300 kata, lintas 1x reseed karena rng>255) | 300/300 kata **MATCH** |
| `--single 999 2000000` (2 juta kata, banyak siklus reseed) | 2,000,000/2,000,000 kata **MATCH** |

**PASS** penuh -- kode C dan oracle Python identik byte-for-byte di semua
skala yang diuji, termasuk lintas reseed berkali-kali. Implementasi
`ad_mul_combine.c` sudah benar merepresentasikan formula yang diminta.

## 2. Tier 0: avalanche gate (K=1, band LOW=0.3/HIGH=0.7/MIN_BIT_FLOOR=0.2, BASE_SEED=1)

| Kandidat | mean_fraction | min_bit_fraction | Verdict |
|---|---|---|---|
| `ad_mul_combine` | 0.1201 | 0.0312 | **REJECT** |

Jauh di bawah kedua ambang (`mean_fraction` 0.12 vs minimum 0.3;
`min_bit_fraction` 0.03 vs minimum 0.2). Sesuai disiplin pipeline yang
sudah dipakai konsisten di eksperimen sejenis (`orbit-init-warmup-fix`
kandidat `d0_realround`, `orbit-o-fmix-fix`): kandidat yang REJECT di
Tier 0 **tidak lanjut ke Tier 1 PractRand sama sekali**, dan overhead
tidak diukur.

## 3. Kenapa REJECT: konsekuensi struktural dari `d=0` di ronde pertama

Ini BUKAN bug implementasi (byte-exact PASS membuktikan kode benar sesuai
formula yang diminta) -- ini konsekuensi matematis langsung dari formula
itu sendiri:

- Ronde pertama tiap panggilan (`i=255`, sebelum `c` manapun dihitung)
  selalu mulai dengan `d=0`.
- Dengan combine XOR produksi, `d=0` tetap membawa `o` penuh ke `a`:
  `a = (0 ^ o) ^ (cons+a) = o ^ (cons+a)`.
- Dengan combine MUL kandidat ini, `d=0` MEMBUNUH kontribusi `o` ke `a`
  sepenuhnya: `a = (0*o) ^ (cons+a) = cons+a` -- `o` (satu-satunya sumber
  keragaman dari `M[]`/key selain `cons` sendiri) hilang total dari `a`
  di ronde 1.
- `b` masih mengandung `o` lewat `(cons+a) ^ (o+d)`, tapi `a` sendiri
  (yang lewat rotasi `rot32((a>>13)^a, b)` sebagai basis `c`) kehilangan
  setengah sumber difusinya justru di ronde yang paling kritis -- ronde
  yang sama yang sudah terdiagnosis sebagai lokasi defect
  ([[project_orbit_key_avalanche_diagnosis]]).

Ini menjelaskan avalanche key->output round1 yang jauh lebih lemah
(0.12 vs baseline produksi ~0.51 dari eksperimen sebelumnya): mengganti
`^` jadi `*` pada titik kombinasi ini melemahkan diffusi pada kasus umum,
bukan hanya di skenario K=1+hashed yang jadi target -- efek sampingnya
lebih luas dari yang dimaksud.

## 4. Kesimpulan

`a=(d*o)^(cons+a)` **ditolak di Tier 0**, tidak sampai diuji PractRand.
Kandidat ini tidak memperbaiki defect K=1+hashed dan sekaligus
memperkenalkan kelemahan avalanche baru yang lebih parah dari masalah
yang coba diatasi. Tidak direkomendasikan untuk axis lanjutan tanpa
modifikasi (mis. menghindari titik kombinasi saat `d=0`, atau
menggabungkan MUL dengan sumber lain yang tidak nol di ronde 1) --
belum didiskusikan/diputuskan dengan user, tidak dikerjakan sesi ini.

`ra_core.c` tidak diubah.

## Ringkasan axis yang sudah dicoba untuk defect K=1+hashed (kumulatif)

| Axis | Hasil |
|---|---|
| Varying/mixing `c` (finalizer, 32-combo search) | 0/32 survivor |
| Zero-tap, init x cycle 8x4 combo search | 0/32 survivor |
| `d0_hash`/`d0_realround` (init d dari hash/ronde nyata) | `d0_realround` REJECT Tier0; `d0_hash` FAIL PractRand K=1+hashed |
| Weyl-constant-resonance (ganti step constant) | Hipotesis REFUTED, tidak relevan sebagai fix |
| `fmix32(o)` init-only / every-round | Byte-exact+Tier0 PASS, tapi FAIL PractRand K=1+hashed (identik) |
| **`a=(d*o)^(cons+a)` (MUL-combine)** | **REJECT Tier 0** -- avalanche rusak sebelum sempat diuji PractRand |

Titik kombinasi `a=(d^o)^(cons+a)` / `b=(cons+a)^(o+d)` tetap kandidat
kuat sebagai lokasi defect, tapi penggantian sederhana (XOR->MUL) di
titik ini terbukti bukan solusi -- perlu direction lain kalau mau
dilanjutkan.
