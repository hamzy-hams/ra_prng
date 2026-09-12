# orbit-d0-raw-cons-fix -- hasil

Kandidat tunggal `d0_raw_cons`: uji narrow-scope dari ide user "bikin
inisiasi awal (`a,b,c,d,o`) nggak ada yang nol, pakai `cons` atau apapun
yang ada". Sebelum implementasi, diverifikasi dulu lewat baca kode
langsung (`ra_core.c` L220-232) bahwa `b` dan `c` inisiasi awal **tidak
pernah dibaca** sebelum ditimpa penuh di round pertama (dead code secara
matematis), `a` sudah `=cons` (tidak nol), dan `o` sudah generik tidak nol
dari 8-tap `M[]` -- satu-satunya lever hidup adalah `d`. Ide "beri `d`
nilai awal tidak nol dari key/cons" sudah dicoba sesi sebelumnya sebagai
`d0_hash` (`fmix32(GUARD_D^key)&0xFF`, `../2026-9-4_orbit-init-warmup-fix/
RESULTS.md`) dan GAGAL. User memilih tetap uji varian yang lebih sempit:
**`d0 = cons & 0xFF`** (byte rendah `cons`/`key` LANGSUNG, TANPA fmix32) --
`a0 = key` tidak berubah. Scope perubahan identik dengan `d0_hash`
(HANYA `it==0`, `it>=1` tetap `d=0` produksi) untuk perbandingan apples-
to-apples.

## 1. Byte-exact vs oracle Python (`combo_prng_d0raw.py`)

| Kasus | Hasil |
|---|---|
| `--single 1 1` | C=1588383838, oracle=1588383838 -- **MATCH** |
| `--single 12345 300` (lintas 1x reseed) | 300/300 kata **MATCH** |
| `--single 999 2000000` (banyak reseed, exercise `it>=1`) | 2,000,000/2,000,000 kata **MATCH** |

**PASS** penuh.

## 2. Tier 0: avalanche gate (K=1, band LOW=0.3/HIGH=0.7/MIN_BIT_FLOOR=0.2)

| Variant | mean_fraction | min_bit_fraction | Verdict |
|---|---|---|---|
| control (referensi) | 0.5137 | 0.2500 | PASS |
| **d0_raw_cons** | 0.4932 | 0.3125 | **PASS** -> lanjut Tier 1 |

## 3. Tier 1: PractRand dual-ordering, K=1 dan K=255, staged 64MB->2GB

| K | Ordering | Stage A (256MB) | Stage B (2GB, base=0x12345) | Stage C repro (2GB, base=0xabcdef01) |
|---|---|---|---|---|
| 1 | Weyl | CLEAN | CLEAN | (tidak dijalankan, pair K=1 gugur di B) |
| 1 | Hashed | CLEAN | **FAIL @512MB** (`FPF-14+6/16:all` R=+9.4, p=2.7e-8, "very suspicious") | (tidak dijalankan) |
| 255 | Weyl | CLEAN | CLEAN | CLEAN |
| 255 | Hashed | CLEAN | CLEAN | CLEAN |

Log lengkap: `tier1_logs/`, ringkasan mesin: `tier1_summary_d0raw.json`.

**Detail FAIL K=1+hashed** (`tier1_logs/d0_raw_cons_K1_hashed_0x00012345_B.log`):
```
length= 256 megabytes: FPF-14+6/16:all  R=+4.7  p=6.4e-4   unusual
length= 512 megabytes: FPF-14+6/16:all  R=+9.4  p=2.7e-8   very suspicious
[EARLY KILL after 512 megabytes]
```
Signature sama persis (`FPF-14+6/16`) dengan defect asli dan dengan
`d0_hash` (yang FAIL @512MB juga, R=+7.9). Angka R sedikit lebih tinggi
di sini (+9.4 vs +7.9) -- perbedaan magnitude kecil, dalam rentang noise
run-to-run, bukan bukti salah satu formula "lebih baik".

## 4. Verdict CLEAN-DUAL

- **`d0_raw_cons`, K=1**: **BUKAN CLEAN-DUAL** -- FAIL di hashed-ordering
  @512MB. Defect target (K=1+hashed) TIDAK teratasi.
- **`d0_raw_cons`, K=255**: **CLEAN-DUAL** (Weyl+hashed, kedua base_key,
  sampai 2GB) -- tidak ada regresi K=255.

## 5. Kesimpulan

**Kandidat GAGAL menutup defect K=1+hashed**, sama seperti `d0_hash`
sebelumnya. ini adalah kandidat KETIGA dari keluarga "beri `d` nilai awal
tidak nol" (`d0_hash`, `d0_realround`, `d0_raw_cons`) yang gagal --
`d0_realround` gagal di Tier 0 sendiri, `d0_hash` dan `d0_raw_cons`
sama-sama lolos Tier 0 tapi FAIL PractRand K=1+hashed dengan signature
identik. Menghilangkan langkah `fmix32` (korelasi langsung ke `cons`,
bukan hash pseudo-random) TIDAK mengubah hasil secara kualitatif --
memperkuat kesimpulan sesi sebelumnya bahwa "`d` nol di round-1" bukan
mekanisme akar tunggal; masalahnya lebih dalam dari sekadar nilai awal
`d` (konsisten dengan temuan
`../2026-9-5_orbit-round1-collision-position-isolation/RESULTS.md`: round2
yang TIDAK pernah mengalami `d=0` sekalipun tetap sama parahnya dengan
round1).

Konfirmasi tambahan (bukan hal baru, tapi divalidasi ulang secara
independen): `b`/`c` inisiasi awal memang benar-benar dead code -- tidak
perlu diuji terpisah, sudah terbukti lewat pembacaan kode langsung
sebelum implementasi.

`ra_core.c` TIDAK disentuh. Tidak ada promosi -- kandidat gagal.

## Ringkasan axis yang sudah dicoba untuk defect K=1+hashed (kumulatif, per sesi ini)

| Axis | Hasil |
|---|---|
| Varying/mixing `c` (finalizer, 32-combo search) | 0/32 survivor |
| Zero-tap, init x cycle 8x4 combo search | 0/32 survivor |
| `d0_hash` (d nonzero, hash dari key) | FAIL PractRand K=1+hashed @512MB |
| `d0_realround` (d dari 1 ronde asli dibuang) | REJECT Tier 0 |
| Weyl-constant-resonance | Hipotesis REFUTED, tidak relevan sebagai fix |
| `fmix32(o)` init-only / every-round | PASS Tier0, FAIL PractRand K=1+hashed |
| `a=(d*o)^(cons+a)` (MUL-combine) | REJECT Tier 0 |
| Position-isolation (round1 vs round2, K=2) | Hipotesis "collision round1 tunggal" REFUTED |
| **`d0_raw_cons` (d nonzero, raw cons, tanpa hash)** | **FAIL PractRand K=1+hashed @512MB** |

8 axis dicoba, semua gagal menutup defect K=1+hashed. Radius yang
tercemar terbukti (dari position-isolation) mencakup >=2 posisi round
dari init segar, dan tidak satu pun modifikasi titik tunggal (nilai `d`,
`o`, `c`, atau operator combine) yang cukup untuk menutupnya sejauh ini.
