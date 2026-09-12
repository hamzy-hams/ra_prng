# Hasil: kandidat fix `fmix32(o)` untuk defect `ra_core_orbit` K=1+hashed

**Status: SELESAI (pipeline byte-exact → Tier 0 → Tier 1 dual-ordering
staged semua dijalankan).** Kedua kandidat GAGAL menutup defect target
(K=1+hashed). Sesuai disiplin repo, laporan ini berhenti di sini dengan
data lengkap -- TIDAK lanjut otomatis ke ide lain tanpa diskusi user.
`ra_core.c` kanonik TIDAK disentuh sama sekali oleh sesi ini.

## 1. Konteks dan motivasi

Defect: `ra_core_orbit` K=1 + urutan key hashed (SplitMix32) FAIL PractRand
(`FPF-14+6/16`), sementara K=255 dan K=1+Weyl tetap bersih. Empat percobaan
fix sebelumnya (zero-tap, 32-combo init×cycle search 0/32, `d0_hash`,
`d0_realround`) semuanya gagal. Sesi hari ini yang sama juga menggugurkan
hipotesis "resonansi konstanta Weyl"
(`../2026-9-4_orbit-weyl-constant-resonance-diagnosis/RESULTS.md`).

Investigasi lanjutan membaca source PractRand langsung menemukan mekanisme
test `FPF-14+6/16`: subsampling stride-16 yang jendelanya menyeberangi
batas word 32-bit, sensitif terhadap korelasi ekor-word/kepala-word
berikutnya -- cocok dengan lokasi defect yang sudah terdiagnosis ("word
pertama setelah init segar").

User mengusulkan axis baru (dikonfirmasi belum pernah dicoba di manapun di
repo lewat grep menyeluruh + baca `singleblock-o-width-fix` dan
`singleblock-cycle-combo-search`): terapkan `fmix32()` pada variabel `o`
(8-tap XOR-shift dari `M[]`) SEBELUM `o` dipakai untuk update `a`/`b` --
bukan ke `c` (output/finalizer, axis yang sudah 32× dicoba dan gagal semua).

## 2. Kandidat yang diuji

- **Kandidat A `o_fmix_init_only`**: `fmix32` HANYA pada `o` awal (sebelum
  loop, round `i=255`). Round `i<255` (`i>1`) recompute `o` via naive 8-tap
  TANPA mixing, persis nilai produksi.
- **Kandidat B `o_fmix_every_round`**: sama seperti A, PLUS setiap `o`
  dihitung ulang di dalam loop (`i>1`) JUGA di-`fmix32`.

**Catatan implementasi penting (bug ditemukan+diperbaiki sesi ini,
sebelum PractRand dijalankan)**: draft awal kedua kandidat memakai
rolling-register update produksi (`o_next = new_term ^ ((o^o_term7)<<1)`)
apa adanya. Ternyata trik XOR-cancel `o_term7` itu HANYA valid secara
aljabar kalau `o` masih bentuk XOR-tap linear murni -- begitu `o` sudah
di-`fmix32` (nonlinear), trik itu diam-diam rusak dan menghasilkan nilai
yang BUKAN `tap8(M,i)` yang dimaksud. Ketahuan lewat byte-exact check wajib
(`combo_prng_ofmix.py`) yang mendeteksi mismatch mulai word kedua. Fix:
kedua kandidat direvisi untuk recompute `o` via naive 8-tap fresh setiap
ronde (bukan rolling register) -- byte-exact PASS setelah revisi (lihat §3).

## 3. Byte-exact C vs oracle (WAJIB lolos sebelum PractRand)

- `--single 12345 300` (exercise reseed loop pertama): **PASS**, 300 word
  identik C vs `combo_prng_ofmix.py` untuk kedua kandidat.
- `--single 777 2000000` (2 juta word, ±7843 siklus reseed): **PASS**,
  identik byte-for-byte untuk kedua kandidat.
- `--single 1 1` = `2548037914` untuk kedua kandidat (BUKAN `397095967`
  produksi -- ekspektasi normal karena formula berubah sejak round pertama).

## 4. Tier 0: avalanche gate (K=1, band LOW=0.3/HIGH=0.7/MIN_BIT_FLOOR=0.2)

| Variant | mean_fraction | min_bit_fraction | Verdict |
|---|---|---|---|
| `init_only` | 0.5117 | 0.3125 | **PASS** -> lanjut Tier 1 |
| `every_round` | 0.5117 | 0.3125 | **PASS** -> lanjut Tier 1 |

Kedua kandidat menghasilkan avalanche K=1 identik (diharapkan -- K=1 hanya
pernah mengeksekusi round pertama, dan kedua kandidat mixing `o` awal
dengan cara yang sama persis).

## 5. Tier 1: PractRand dual-ordering staged, K=1 dan K=255, skala 2GB

Staged 64MB->256MB (Stage A) -> 2GB (Stage B, `base_key=0x12345`) ->
reproduksi 2GB (Stage C, `base_key=0xABCDEF01`), early-kill on FAIL, tanpa
retry seed lain.

| Variant | K | Ordering | Stage A (256MB) | Stage B (2GB) | Stage C repro (2GB) |
|---|---|---|---|---|---|
| `init_only` | 1 | Weyl | CLEAN | CLEAN | (tidak dijalankan, K=1 gugur di B) |
| `init_only` | 1 | Hashed | CLEAN | **FAIL @1GB** (`FPF-14+6/16:all` R=+15.7, p=3.3e-14) | (tidak dijalankan) |
| `init_only` | 255 | Weyl | CLEAN | CLEAN | CLEAN |
| `init_only` | 255 | Hashed | CLEAN | CLEAN | CLEAN |
| `every_round` | 1 | Weyl | CLEAN | CLEAN | (tidak dijalankan, K=1 gugur di B) |
| `every_round` | 1 | Hashed | CLEAN | **FAIL @1GB** (`FPF-14+6/16:all` R=+15.7, p=3.3e-14) | (tidak dijalankan) |
| `every_round` | 255 | Weyl | CLEAN | CLEAN | CLEAN |
| `every_round` | 255 | Hashed | CLEAN | CLEAN | CLEAN |

Log lengkap: `tier1_logs/`, ringkasan mesin: `tier1_summary_ofmix.json`.

**Detail FAIL K=1+hashed** (identik byte-for-byte antar kedua kandidat,
termasuk p-value ke digit terakhir -- lihat §6 untuk kenapa):
```
length= 256 megabytes (2^28 bytes): FPF-14+6/16:all  R=+4.8   p=5.7e-4   unusual
length= 512 megabytes (2^29 bytes): FPF-14+6/16:all  R=+7.8   p=7.7e-7   suspicious
length= 1 gigabyte  (2^30 bytes):   FPF-14+6/16:(0,14-0) R=+8.4  p=2.4e-7  mildly suspicious
                                     FPF-14+6/16:(1,14-0) R=+10.0 p=7.2e-9  very suspicious
                                     FPF-14+6/16:(2,14-0) R=+7.1  p=3.2e-6  unusual
                                     FPF-14+6/16:all      R=+15.7 p=3.3e-14 FAIL
```
Signature sama (`FPF-14+6/16`) dengan defect asli, muncul di skala serupa
dengan sesi-sesi kandidat sebelumnya (@512MB-2GB).

## 6. Overhead (ringan, timing tunggal K=255 stream 20M word)

| Variant | Waktu | Overhead vs produksi |
|---|---|---|
| Produksi (`ra_core_orbit`) | 0.247s | 1.00x |
| `init_only` | 0.337s | ~1.36x |
| `every_round` | 0.274s | ~1.11x |

Angka kasar (timing tunggal, bukan rata-rata banyak run) -- tidak
relevan lebih jauh karena kedua kandidat sudah gagal di Tier 1.

## 7. Verdict dan insight penting

**Kedua kandidat GAGAL menutup defect K=1+hashed** -- `fmix32(o)` sebelum
`a`/`b` TIDAK cukup untuk mematahkan korelasi yang terdeteksi
`FPF-14+6/16`, meski axis ini genuinely baru dan tidak pernah dicoba
sebelumnya.

**Insight penting (bukan cuma "gagal lagi")**: kedua kandidat menghasilkan
hasil PractRand K=1 **identik byte-for-byte** (bukan cuma mirip -- p-value
sama persis ke digit terakhir). Ini karena K=1 (`rng=1`) membuat
`ra_core_orbit` berhenti SETELAH SATU ronde (`count<=1` trigger break
sebelum pernah mencapai update `o` ronde ke-2) -- jadi perbedaan Kandidat A
vs B (yang hanya menyentuh `o` ronde ke-2 dst) **tidak pernah teruji sama
sekali di K=1**. Ini bukan kebetulan, tapi bukti struktural: **defect K=1
100% ditentukan oleh SATU nilai `o` awal (sudah di-fmix32) dikombinasikan
dengan `a`/`b`/`c`/`d` di ronde pertama** -- mixing `o` sendiri (di titik
manapun sebelum masuk ke `a`) TIDAK cukup untuk membersihkan korelasi ini.

Ini menyempitkan ruang hipotesis lebih jauh: bukan cuma "formula
init/cycle/finalizer/o mana yang dipakai" (sudah 32+2=34 kombinasi dicoba,
semua gagal), tapi kemungkinan besar korelasinya ada di **CARA `o` (baik
mentah maupun mixed) dikombinasikan dengan `cons`/`a`/`d` lewat
`a=(d^o)^(cons+a)` dan `b=(cons+a)^(o+d)`** -- yaitu titik gabungan XOR+ADD
itu sendiri, bukan nilai `o` atau `c` secara terpisah.

## 8. Yang TIDAK dilakukan sesi ini (di luar scope, per persetujuan awal)

- TIDAK ada promosi kandidat manapun ke `ra_core.c` kanonik (keduanya
  gagal, jadi tidak relevan).
- TIDAK melanjutkan ke ide arah baru lain (mis. mixing pada titik
  penggabungan `a`/`b` itu sendiri, lihat §7) tanpa diskusi user.

## 9. Referensi

- Diagnosis yang memicu sesi ini (mekanisme FPF-14+6/16): dibahas di chat
  sesi ini, sumber: `~/Documents/research/PractRand/src/tests.cpp` L3933-4230.
- Defect asli: `../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`.
- Percobaan fix yang sudah gagal sebelumnya (jangan diulang):
  `../2026-9-4_orbit-init-zerotap-fix/RESULTS.md`,
  `../2026-9-4_orbit-init-cycle-combo-search/RESULTS.md`,
  `../2026-9-4_orbit-init-warmup-fix/RESULTS.md`,
  `../2026-9-4_orbit-weyl-constant-resonance-diagnosis/RESULTS.md`.
- File kode: `candidates/o_fmix_init_only.c`, `candidates/o_fmix_every_round.c`,
  `combo_prng_ofmix.py`, `tier0_avalanche_ofmix.py`, `tier1_gate_ofmix.py`.
- Artefak: `tier0_survivors_ofmix.jsonl`, `tier1_log_ofmix.jsonl`,
  `tier1_summary_ofmix.json`, `tier1_logs/`.
