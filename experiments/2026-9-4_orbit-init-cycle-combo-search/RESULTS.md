# Hasil: search init-formula × cycle-transform untuk `ra_core_orbit` (dual key-ordering)

**Status: Stage 1 SELESAI, 0 survivor. Stage 2 (§5 HANDOVER.md) BELUM
dijalankan — menunggu keputusan user per §8 HANDOVER.md.**

## 1. Konteks

Dijalankan sesuai desain lengkap di `HANDOVER.md` (baca file itu untuk
rasional detail). Ringkas: gate "orbit K-small defect CLOSED" ternyata cuma
pernah divalidasi dengan urutan key Weyl-stepped; `ra_core_orbit` K=1 dengan
urutan key hashed (SplitMix32) FAIL PractRand parah dan reproducible
(`../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`). Satu hipotesis
fix (dead M[0]/L[0] tap) sudah ditolak
(`../2026-9-4_orbit-init-zerotap-fix/RESULTS.md`). Search ini meng-cross axis
init-formula (8 kandidat baru) dengan axis cycle-transform (4 kandidat
dikurasi dari combo-search lama), dengan kriteria lolos **wajib
dual-ordering (Weyl DAN hashed) sejak Tier 1**.

**Scope disepakati user 2026-09-04**: bangun infra → validasi → Tier 0 →
**Tier 1 PractRand dual-ordering lokal** → tulis laporan ini → STOP. Promotion
Tier (VPS, 8-16GB) sengaja TIDAK dijalankan sesi ini.

## 2. Validasi infra (langkah 1-2 pipeline)

- **Oracle Python vs produksi**: `combo_prng_orbit.orbit_round1(1,
  "init_linear_shared", "w8_f10_i0") == 397095967` — cocok. Stream 37-word
  (`key=12345`) juga cocok byte-for-byte dengan binary produksi. PASS.
- **Codegen C vs oracle, 32 kombinasi**: kombinasi kontrol
  (`init_linear_shared__w8_f10_i0`) diverifikasi byte-identik dengan
  `ra_core_orbit` produksi (`397095967`). Seluruh 32 kombinasi
  ter-compile dan lolos `sanity_check` (key=12345, rng=37) byte-for-byte vs
  oracle Python. PASS, tidak ada bug codegen.

## 3. Tier 0 — avalanche gate (K=1)

Band `LOW=0.3, HIGH=0.7, MIN_BIT_FLOOR=0.2`. **32/32 kombinasi lolos** — tidak
ada yang dibuang di tahap ini (lihat `tier0_survivors_orbit.jsonl` untuk
angka `mean_fraction`/`min_bit_fraction` lengkap per kombinasi; semua berada
nyaman di dalam band, `mean_fraction` berkisar 0.4658-0.5333, `min_bit_fraction`
0.21875-0.375).

## 4. Tier 1 — PractRand dual-ordering lokal

### Stage A (screen, base_key `0x12345`, 64MB→256MB)

**32/32 kombinasi lolos** dual-ordering (Weyl + hashed) sampai 256MB. Tidak
ada FAIL/anomali di skala ini untuk kombinasi manapun. Waktu run 10-22s per
kombinasi per ordering.

### Stage B (confirm, base_key `0x12345`, 256MB→2GB)

**0/32 kombinasi lolos.** Hasil lengkap:

| kombinasi | Weyl @2GB | hashed @2GB |
|---|---|---|
| `init_linear_shared__w8_f10_i0` (**kontrol/produksi**) | CLEAN | **FAIL @1GB** |
| `init_linear_shared__w8_f9_i0` | CLEAN | **FAIL @512MB** |
| `init_linear_shared__w8_f28_i0` | CLEAN | **FAIL @1GB** |
| `init_linear_shared__w8_f33_i0` | CLEAN | **FAIL @512MB** |
| `init_linear_per_index__w8_f10_i0` | CLEAN | **FAIL @1GB** |
| `init_linear_per_index__w8_f9_i0` | CLEAN | **FAIL @1GB** |
| `init_linear_per_index__w8_f28_i0` | CLEAN | **FAIL @1GB** |
| `init_linear_per_index__w8_f33_i0` | CLEAN | **FAIL @1GB** |
| `init_linear_p1_shared__w8_f10_i0` | **FAIL @2GB** | **FAIL @1GB** |
| `init_linear_p1_shared__w8_f9_i0` | **FAIL @2GB** | **FAIL @512MB** |
| `init_linear_p1_shared__w8_f28_i0` | CLEAN | **FAIL @1GB** |
| `init_linear_p1_shared__w8_f33_i0` | CLEAN | **FAIL @512MB** |
| `init_linear_p1_per_index__w8_f10_i0` | CLEAN | **FAIL @512MB** |
| `init_linear_p1_per_index__w8_f9_i0` | CLEAN | **FAIL @1GB** |
| `init_linear_p1_per_index__w8_f28_i0` | CLEAN | **FAIL @1GB** |
| `init_linear_p1_per_index__w8_f33_i0` | CLEAN | **FAIL @512MB** |
| `init_odd_shared__w8_f10_i0` | CLEAN | **FAIL @1GB** |
| `init_odd_shared__w8_f9_i0` | CLEAN | **FAIL @1GB** |
| `init_odd_shared__w8_f28_i0` | CLEAN | **FAIL @1GB** |
| `init_odd_shared__w8_f33_i0` | CLEAN | **FAIL @1GB** |
| `init_odd_per_index__w8_f10_i0` | CLEAN | **FAIL @512MB** |
| `init_odd_per_index__w8_f9_i0` | CLEAN | **FAIL @1GB** |
| `init_odd_per_index__w8_f28_i0` | CLEAN | **FAIL @512MB** |
| `init_odd_per_index__w8_f33_i0` | CLEAN | **FAIL @1GB** |
| `init_hashed_shared__w8_f10_i0` | CLEAN | **FAIL @512MB** |
| `init_hashed_shared__w8_f9_i0` | CLEAN | **FAIL @1GB** |
| `init_hashed_shared__w8_f28_i0` | CLEAN | **FAIL @512MB** |
| `init_hashed_shared__w8_f33_i0` | CLEAN | **FAIL @1GB** |
| `init_hashed_per_index__w8_f10_i0` | CLEAN | **FAIL @2GB** |
| `init_hashed_per_index__w8_f9_i0` | CLEAN | **FAIL @512MB** |
| `init_hashed_per_index__w8_f28_i0` | CLEAN | **FAIL @512MB** |
| `init_hashed_per_index__w8_f33_i0` | CLEAN | **FAIL @1GB** |

Log mentah lengkap per (kombinasi, ordering, base_key) ada di `tier1_logs/`;
ringkasan terstruktur di `tier1_log_orbit.jsonl` dan `tier1_summary_orbit.json`.

### Stage C (reproducibility, base_key `0xABCDEF01`)

**Tidak dijalankan** — 0 survivor dari Stage B berarti tidak ada kandidat
untuk dites (sesuai desain, bukan diskip).

## 5. Analisis pola kegagalan

**Temuan utama: seluruh 32 kombinasi (8 init-formula × 4 cycle-transform)
FAIL pada urutan key hashed, di rentang 512MB-2GB — 100% tanpa kecuali.**
`init_hashed_per_index__w8_f10_i0` bertahan paling lama (FAIL tepat @2GB,
setelah "hampir" CLEAN), tapi tetap FAIL, bukan CLEAN.

Ini adalah sinyal kuat bahwa akar masalah **bukan** di `ra_init_state_orbit`
maupun `ra_permutation_cycle_orbit` — kedua fungsi itulah yang divariasikan
di sini, secara agresif (termasuk kandidat `hashed_per_index` yang mengganti
total mekanisme index-to-state), dan tidak satupun berhasil menutup gap.

> **RALAT (2026-09-04, sesi lanjutan)**: versi awal §5 di sini menunjuk
> `ra_hash`/`ra_reseed` sebagai arah paling mungkin. **Klaim itu salah** —
> dikonfirmasi dengan membaca `ra_core_orbit`
> (`../2026-9-1_family-productionization/ra_core.c` L254-270): untuk `rng=1`
> (K=1, skenario yang FAIL di sini), `count<=1` setelah satu putaran cycle →
> fungsi **return SEBELUM `ra_reseed` dipanggil**. `ra_hash`/`ra_reseed`
> benar-benar dead code di skenario ini, jadi tidak mungkin jadi penyebab.
>
> Analisis yang benar: K=1 = satu panggilan `ra_init_state_orbit(key)` +
> SATU kali `ra_permutation_cycle_orbit` (tidak ada reseed, tidak ada state
> yang bertahan). Yang sebenarnya diuji Tier 1 bukan "kualitas stream dari
> satu key", tapi **kualitas `orbit_round1` sebagai fungsi hash
> `key → 1 word`**, dipanggil jutaan kali dengan key berbeda-beda. Pola 0/32
> 100% FAIL dengan sinyatur sama di semua kombinasi menunjuk ke defisit
> mixing/diffusion yang bukan spesifik formula tertentu — kemungkinan
> "cold-start" (word pertama dari key segar mixing-nya lebih lemah dari word
> ke-2/3/dst dalam stream panjang), yang ter-dilusi di K=255 tapi 100%
> terekspos di K=1. Diagnosis lanjutan (bukan search kombinasi lagi):
> `../2026-9-4_orbit-key-avalanche-diagnosis/`.

**Temuan sekunder (baru, di luar hipotesis awal)**: dua kombinasi
(`init_linear_p1_shared` × `w8_f10_i0`/`w8_f9_i0`) FAIL bahkan pada urutan
**Weyl** @2GB — padahal `init_linear_p1_shared` standalone (formula
`i+1`) sebelumnya sudah diuji terpisah di
`../2026-9-4_orbit-init-zerotap-fix/` dan tidak menunjukkan regresi Weyl di
situ. Kombinasi spesifik `linear_p1_shared` + finalizer XORSHIFT-tunggal
(f10/f9) tampak jadi kombinasi terburuk di ruang cari ini — informasi
berharga untuk dihindari, tapi bukan pusat perhatian karena masih FAIL hashed
juga dan karenanya tetap tidak lolos gate.

## 6. Kesimpulan dan langkah berikut

- **0 survivor CLEAN-DUAL di Stage 1.** Tidak ada kandidat yang
  direkomendasikan untuk dipromosikan ke `ra_core.c`.
- Per §5/§8 HANDOVER.md, hasil nol-survivor di Stage 1 secara desain memicu
  **Stage 2** (perluas axis cycle ke 11 survivor combo-search lama, silang
  dengan 2-3 kandidat init paling menjanjikan) — **namun mengingat pola 100%
  FAIL yang konsisten di SEMUA 32 kombinasi tanpa kecuali (bukan sekadar
  mayoritas), memperluas axis cycle-transform kemungkinan besar TIDAK akan
  mengubah hasil**, karena kedua fungsi yang divariasikan (init + cycle)
  sudah terbukti bukan penyebab. Stage 2 seperti didesain di HANDOVER.md
  kemungkinan adalah pengeluaran biaya besar untuk peluang perbaikan kecil.
- **Rekomendasi (RALAT 2026-09-04, lihat kotak ralat §5 di atas)**: bukan
  `ra_hash`/`ra_reseed` (dead code untuk K=1, terbukti bukan penyebab) —
  langkah berikutnya yang benar adalah **diagnosis murah** kenapa
  `orbit_round1` lemah sebagai fungsi hash `key → 1 word`, sebelum
  memutuskan arah fix (search operasi baru di luar a,b,c,d,o, atau
  redesain init-state). Dijalankan di
  `../2026-9-4_orbit-key-avalanche-diagnosis/`.
- `ra_core.c` TIDAK disentuh sesi ini.

## 7. File terkait

- `recipes_orbit.py`, `combo_prng_orbit.py`, `combo_gen_orbit.py` — infra
  (oracle, codegen, katalog kombinasi).
- `candidates/` — 32 kandidat `.c` + binary terkompilasi.
- `tier0_avalanche_orbit.py`, `tier0_survivors_orbit.jsonl` — Tier 0.
- `tier1_gate_orbit.py`, `tier1_logs/`, `tier1_log_orbit.jsonl`,
  `tier1_summary_orbit.json`, `tier1_run_stdout.log` — Tier 1 lengkap.
