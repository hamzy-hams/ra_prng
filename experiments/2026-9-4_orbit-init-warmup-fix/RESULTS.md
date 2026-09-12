# Hasil: uji 2 kandidat fix tertarget (seed `d0` di fase init) untuk defect K=1+hashed pada `ra_core_orbit`

**Status: SELESAI (pipeline §7 langkah 1-5 semua dijalankan).** Kedua
kandidat gagal menutup defect target (K=1+hashed). Sesuai HANDOVER.md §0.4,
laporan ini berhenti di sini dengan data lengkap -- TIDAK lanjut otomatis ke
"cari operasi finalizer baru" atau opsi berat lainnya. `ra_core.c` kanonik
TIDAK disentuh sama sekali oleh sesi ini.

## 1. Konteks

Defect: `ra_core_orbit` K=1 dengan key ordering hashed (SplitMix32) FAIL
PractRand (signature `FPF-14+6/16`), sedangkan K=255 dan K=1+Weyl-ordering
tetap bersih. Diagnosis sebelumnya
(`../2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md`) melokalisasi
defect ke **word pertama** setelah init segar. Sesi combo-search
(`../2026-9-4_orbit-init-cycle-combo-search/RESULTS.md`) sudah membuktikan
0/32 kombinasi formula init/cycle lolos -- BUKAN soal formula mana yang
dipakai. Hipotesis sesi ini: nilai seed `a`/`d` di ronde pertama (`it==0`)
pada `ra_permutation_cycle_orbit`, satu-satunya titik yang belum pernah
divariasikan.

Dua kandidat diuji, HANYA mengubah baris inisialisasi `a`/`d` di `it==0`;
loop body (`ra_core.c` L228-251) verbatim tidak disentuh:
- **Kandidat A (`d0_hash`)**: `d0 = fmix32(GUARD_D ^ key) & 0xFF`, `a0 = key` (tidak berubah).
- **Kandidat B (`d0_realround`)**: satu ronde asli `i=255` dihitung di fase init (dibuang), hasilnya jadi `a0`/`d0`.

## 2. GUARD_D provenance

Dipilih oleh `pick_guard_constant_d.py` (metode sama seperti
`../2026-9-1_keyzero-guard-fix/pick_guard_constants.py`: SHA-256-scan label
deskriptif, ambil 32-bit rendah, cari popcount==16 pertama yang beda dari
konstanta existing):

```
GUARD_D = 0x17067f32  popcount=16  (sha256 attempt #24)
```

Terverifikasi beda dari `GUARD_L=0x38916df4`, `GUARD_M=0x6c26fc92`, dan 4
konstanta pre-existing (`0x9e3779b7`, `0x06a0dd9b`, `0x85ebca6b`,
`0xc2b2ae35`).

## 3. Validasi §7 langkah 1-2: byte-exact (WAJIB lolos sebelum PractRand)

- **Kontrol byte-exact vs produksi**: `control --single 1 1` = `397095967`,
  cocok dengan `ra_core_orbit(key=1, rng=1)` produksi dan oracle Python. **PASS.**
- **Oracle vs codegen** (`--single 12345 37`, dan tambahan `--single 777 2000`
  untuk exercise cabang `it>=1`): `d0_hash` dan `d0_realround` cocok
  byte-for-byte dengan `combo_prng_warmup.py`. **PASS** (lihat juga spot-check
  manual key=1 di §7).

## 4. Tier 0: avalanche gate (K=1, band LOW=0.3/HIGH=0.7/MIN_BIT_FLOOR=0.2)

| Variant | mean_fraction | min_bit_fraction | Verdict |
|---|---|---|---|
| control (referensi) | 0.5137 | 0.2500 | PASS |
| **d0_hash** | 0.5137 | 0.3750 | **PASS** -> lanjut Tier 1 |
| **d0_realround** | 0.2793 | 0.1562 | **REJECT** -> TIDAK lanjut Tier 1 |

`d0_realround` gagal di Tier 0 sendiri: `mean_fraction` di bawah `LOW=0.3`
dan `min_bit_fraction` di bawah `MIN_BIT_FLOOR=0.2`. Sesuai disiplin
pipeline (§7 langkah 3 men-gate langkah 4), kandidat ini TIDAK diuji
PractRand sama sekali.

## 5. Tier 1: PractRand dual-ordering, K=1 dan K=255, skala 2GB (hanya `d0_hash`)

Staged 64MB->256MB (Stage A) -> 2GB (Stage B, `base_key=0x12345`) ->
reproduksi 2GB (Stage C, `base_key=0xABCDEF01`), early-kill on FAIL, tanpa
retry seed lain.

| Variant | K | Ordering | Stage A (256MB) | Stage B (2GB) | Stage C repro (2GB) |
|---|---|---|---|---|---|
| d0_hash | 1 | Weyl | CLEAN | CLEAN | (tidak dijalankan, K=1 gugur di B) |
| d0_hash | 1 | Hashed | CLEAN | **FAIL @512MB** (`FPF-14+6/16`, R=+7.9, "very suspicious") | (tidak dijalankan) |
| d0_hash | 255 | Weyl | CLEAN | CLEAN | CLEAN |
| d0_hash | 255 | Hashed | CLEAN | CLEAN ("unusual" soft-anomaly transien di 512MB & 1GB, hilang di 2GB) | CLEAN ("unusual" transien di 512MB & 2GB) |

Log lengkap: `tier1_logs/`, ringkasan mesin: `tier1_summary_warmup.json`.

**Detail FAIL K=1+hashed** (`tier1_logs/d0_hash_K1_hashed_0x00012345_B.log`):
```
length= 512 megabytes (2^29 bytes)
FPF-14+6/16:all   R=+7.9   p=6.5e-7   very suspicious
```
Signature identik dengan defect asli (`FPF-14+6/16`) yang ditemukan di
`orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md` -- Kandidat A hanya
menggeser sedikit skala kegagalan (produksi FAIL parah @8GB dengan R jauh
lebih tinggi; di sini "very suspicious" muncul lebih awal, @512MB), TIDAK
menghilangkan mekanismenya.

## 6. Verdict CLEAN-DUAL per kandidat per K per ordering

- **d0_hash, K=1**: **BUKAN CLEAN-DUAL** -- FAIL di hashed-ordering @512MB.
  Defect target TIDAK teratasi oleh Kandidat A.
- **d0_hash, K=255**: **CLEAN-DUAL** (Weyl+hashed, kedua base_key, sampai
  2GB) -- tidak ada regresi K=255 akibat perubahan seed `a0`/`d0`, meski
  perubahan ini otomatis berlaku juga di K=255 (karena `rng<=255` selalu
  `it==0`, lihat HANDOVER §2). Ada beberapa soft-anomaly "unusual" transien
  di checkpoint tengah yang hilang di checkpoint berikutnya -- pola yang
  sama dengan noise statistik biasa (bukan hard FAIL), tidak menaikkan
  kekhawatiran regresi sendirian.
- **d0_realround**: **REJECT di Tier 0**, tidak sampai diuji K/ordering
  manapun di PractRand.

## 7. Regresi opsional `it>=1` (§7 langkah 5, tidak blocking)

Satu panggilan `--single` per variant dengan `rng=2,000,000` (key=777,
±7843 siklus reseed) untuk ketiga variant (control, d0_hash, d0_realround):
tidak crash, byte-exact cocok dengan oracle Python untuk seluruh 2 juta
word. **PASS**, konsisten dengan desain (`it>=1` tidak disentuh oleh kedua
kandidat).

Spot-check manual tambahan (`--single 1 1`, independen dari `sanity_check`
otomatis): `control`=397095967, `d0_hash`=2901270056, `d0_realround`=112,
ketiganya cocok C vs Python.

## 8. Kesimpulan dan langkah berikutnya

**Kedua kandidat GAGAL menutup defect K=1+hashed.** Kandidat A (`d0_hash`)
lolos Tier 0 tapi FAIL di Tier 1 dengan signature yang sama persis dengan
defect asli -- menunjukkan "nilai `d0` yang key-dependent saja" tidak cukup.
Kandidat B (`d0_realround`) bahkan gagal di avalanche gate paling awal,
menunjukkan "progresi state riil satu ronde" (mereplikasi `i=255` lagi,
bukan maju ke `i=254`) justru MERUSAK avalanche sebelum sempat diuji
terhadap PractRand -- kemungkinan desain "buang satu ronde di `i=255` yang
sama" (bukan mengikuti progresi index asli) adalah masalahnya sendiri (lihat
catatan desain HANDOVER §4).

Per HANDOVER.md §0.4/§8: sesi ini TIDAK lanjut ke arah baru (opsi berat
"cari operasi finalizer baru") tanpa diskusi dengan user. Pertanyaan
terbuka dari sesi diagnosis sebelumnya -- kenapa Weyl-ordering K=1 (struktur
"cold word" yang sama) tetap bersih padahal hashed-ordering K=1 FAIL --
masih belum terjawab dan makin relevan untuk dijawab dulu sebelum mencoba
arah fix lain.

Temuan positif kecil yang tidak diminta tapi dicatat: Kandidat A tidak
meregresi K=255 (tetap CLEAN-DUAL sampai 2GB, dua base_key) -- jadi
mekanisme `d0_hash` sendiri "aman" untuk K besar, meski tidak menyelesaikan
masalah K=1.

## 9. Yang TIDAK dilakukan sesi ini (di luar scope, per persetujuan awal)

- TIDAK ada promosi kandidat manapun ke `ra_core.c` kanonik (lagipula
  keduanya gagal, jadi tidak relevan).
- TIDAK melanjutkan ke Promotion Tier (VPS, 8-16GB) -- tidak ada kandidat
  yang lolos untuk dipromosikan.
- TIDAK memutuskan arah lanjutan (mis. cari operasi finalizer baru, atau
  jawab dulu pertanyaan Weyl-vs-hashed) -- keputusan ini diserahkan ke user.

## 10. Referensi

- Diagnosis yang memicu sesi ini: `../2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md`.
- Search kombinasi sebelumnya (0/32): `../2026-9-4_orbit-init-cycle-combo-search/RESULTS.md`.
- Defect asli: `../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`.
- Spec sesi ini: `HANDOVER.md`.
- File kode: `pick_guard_constant_d.py`, `combo_prng_warmup.py`,
  `combo_gen_warmup.py`, `tier0_avalanche_warmup.py`, `tier1_gate_warmup.py`.
- Artefak: `candidates/`, `tier0_survivors_warmup.jsonl`,
  `tier1_log_warmup.jsonl`, `tier1_summary_warmup.json`, `tier1_logs/`.
