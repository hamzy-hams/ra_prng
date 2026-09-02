# RESULTS: Tahap 2 — Battery validasi produksi (`ra_core_orbit` + `ra_core_singleblock`)

## Status

**IN PROGRESS** (2026-09-01), dieksekusi langsung setelah user mengonfirmasi
Tahap 1 dan memberi instruksi eksplisit untuk lanjut ("lanjut jalankan Tahap
2") — ini adalah gerbang konfirmasi yang diminta `HANDOVER.md` sebelum Tahap
2 boleh mulai.

## Ringkasan per axis (HANDOVER.md §2)

### `ra_core_orbit`: SEMUA axis terwarisi, tidak ada pekerjaan baru

`ra_core_orbit` bit-identical terhadap `winner_wired_addressable.c` (bukti:
`verify_unification.py`, 64/64 kombinasi, `RESULTS.md` Tahap 1). Karena
byte outputnya identik, seluruh jaminan berikut otomatis terwarisi tanpa
perlu diulang:

- **Avalanche**: `winner_wired_v2.c`'s wiring survey (`operand-position-search/RESULTS.md`)
  sudah melewati `avalanche_gate_min_bit()`. Diverifikasi ulang langsung di
  binary `ra_core` produksi (lihat bagian "Verifikasi langsung" di bawah) —
  PASS.
- **PractRand**: 128GB clean (Tahap 3, `addressable-init-research/RESULTS.md`).
- **Cross-correlation & collision-scan**: 0/8.128 pairs flagged, 0 collision
  sampai 500k key (Tahap 3, skala produksi).
- **Dieharder "Good" battery**: 26/27 PASSED, 1 WEAK, 0 FAILED
  (`2026-9-1_dieharder-battery/RESULTS.md`).

### `ra_core_singleblock`: pekerjaan baru, "aggressive reinit" skala produksi

Bit-identity terhadap `ra_core_orbit` (untuk `rng<=255`) sudah dibuktikan di
Tahap 1 (9.945/9.945 kombinasi in-process). Tapi bit-identity satu key TIDAK
membuktikan bahwa banyak key BERBEDA yang dipakai berturut-turut (pola
pakai nyata "call-and-discard") tetap independen satu sama lain secara
statistik — itu axis baru yang dikerjakan di sini.

**Alat baru** (tidak mengubah `ra_core.c`):
- `ra_core_singleblock_cli.c` — wrapper CLI `--stream <key> <n>` (tanpa
  argumen mode), supaya skrip `cross_correlation.py`/`collision_scan.py`
  yang sudah ada bisa dipakai ulang lewat re-point `binary=`, pola yang
  sama persis dengan `cross_correlation_ra_prng2.py`.
- `multikey_stream.c` — generator yang merangkai banyak panggilan
  `ra_core_singleblock` (key berbeda tiap blok 255-kata, turunan key via
  Weyl increment `0x9E3779B9`) jadi satu stream kontinu, supaya alat
  streaming standar (dieharder, PractRand) bisa menilai mode ini persis
  seperti dipakai di produksi. Diverifikasi cross-check: output-nya identik
  byte-per-byte dengan memanggil `ra_core --stream singleblock <key> <n>`
  langsung per blok.
- `bench_ra_core.c` — reuse metodologi `tahap6_bench.c` (throughput,
  reinit-sweep, Philox4x32-10 pembanding), dihubungkan ke `ra_core.c` via
  `extern` (tanpa duplikasi source).

#### 1. Avalanche (`avalanche_ra_core.py`, reuse `quality_gate.py`'s `avalanche_gate_min_bit()`)

| Mode | overall_mean_hamming_fraction | min_bit_fraction | PASS |
|---|---|---|---|
| orbit | 0.498943 | 0.485907 | YA (band [0.3,0.7], floor 0.2) |
| singleblock | 0.498943 | 0.485907 | YA |

Identik persis antara kedua mode (konsisten dengan bukti bit-identical
`rng<=255`).

#### 2. Speed (`bench_ra_core.c`, pola `tahap6_bench.c`)

Throughput orbit (N besar) ~2.0-2.4 ns/word, mengalahkan Philox4x32-10 di
semua N yang diuji (1K-100M). Reinit-sweep (pola pakai nyata singleblock):

| K | singleblock ns/word (steady-state) | orbit ns/word | Philox ns/word |
|---|---|---|---|
| 1 | 38.135 | 56.819 | ~15.2-15.5 |
| 10 | 4.659 | 7.893 | ~3.6-3.9 |
| 100 | 0.422 | 2.635 | ~3.0-3.1 |
| 255 | 0.135 | 2.434 | ~2.98-3.15 |

Checksum silang cocok persis antara orbit dan singleblock di K yang sama
(bukti tambahan pasif bit-identity). Arah dan magnitude hasil ini konsisten
dengan angka historis Tahap 6 (`addressable-init-research/RESULTS.md` baris
"K\* reinit-sweep crossover ~60.1 (baseline) vs ~12.3 (singleblock)") —
**tidak ada regresi kecepatan dari unifikasi/rename Tahap 1.**

#### 3. Cross-correlation, multi-key (`cross_correlation_ra_core_singleblock.py`, tier "full": K=512, n=255)

```
adjacent flagged rate=0.0000 vs control flagged rate=0.0000
(m_pairs=130,816 tiap grup, alpha=0.01 Bonferroni-corrected)
```
PASS — tidak ada sinyal korelasi linear lag-0 antar key, baik key
berurutan (adjacent) maupun key acak (control).

#### 4. Collision-scan, multi-key (`collision_scan_ra_core_singleblock.py`, tier "full": M=50.000, satu blok penuh 255-kata per key)

| Seed mode | Fingerprints | Collisions | digest-collision-prob (birthday bound) |
|---|---|---|---|
| sequential (0..49999) | 50.000 | 0 | ~6.78e-11 |
| random (full 2^32 space) | 50.000 | 0 | ~6.78e-11 |

PASS — 0 collision jauh di bawah ambang digest-collision noise floor.

#### 5. Dieharder "Good" battery, stream multi-key (`multikey_stream` + `run_dieharder_battery.py` reused)

```
26/27 test invocations PASSED, 1 WEAK, 0 FAILED
```
Satu WEAK: `sts_serial|ntup=13` p=0.99820300 (high-tail) — satu p-value
di antara puluhan per test, pola sama persis dengan run dieharder
`winner_wired_v2`/`winner_wired_addressable` sebelumnya (1-2 WEAK per
generator, tidak pernah FAILED). `-d 200` kosong di kedua run (precedent
sama di `dieharder_winner_wired_v2_piped.txt`) — kuirk dieharder yang
sudah ada, bukan regresi baru.

#### 6. PractRand bertahap, stream multi-key (`multikey_stream` -> `RNG_test stdin32`)

**STATUS: DEFECT NYATA DITEMUKAN, root cause terdiagnosis, fix BELUM
diimplementasikan ke `ra_core.c`.**

```
1GB: bersih
2GB: BCFN(2+0,13-0,T) very suspicious (R=+14.3, p=3.3e-7)
4GB: BCFN(2+0,13-0,T) FAIL!! (R=+40.7, p=2.6e-21)
     BCFN(2+1,13-0,T) FAIL! (R=+28.9, p=5.8e-15)
```
Severity naik tajam dari 2GB ke 4GB (bukan noise satu titik) -- pola khas
defect nyata. Terjadi identik persis di `ra_core_orbit` kalau dipakai
dengan pola reinit-pendek-banyak-key yang sama (`multikey_stream_orbit.c`),
jadi ini defect pada mekanisme init+cycle bersama, bukan spesifik kode
`singleblock`. Diagnosis lengkap, hasil ukur biaya, dan pencarian fix
dipindah ke folder terpisah:
**`../2026-9-1_multikey-remix-search/HANDOVER.md`** (root cause
terkonfirmasi: korelasi terkonsentrasi di SIKLUS PERTAMA sebelum
`ra_reseed`, karena `M[]`/`L[]` affine-in-key -- `ra_core_orbit`'s 128GB
test aman karena didominasi milyaran siklus PASCA-reseed, bukan karena
formulanya benar).

**Implikasi untuk status Tahap 2**: axis-axis lain (avalanche, speed,
cross-correlation, collision-scan, dieharder) di atas TETAP valid apa
adanya -- tidak satupun dari alat itu menangkap defect ini (butuh skala
PractRand 4GB+ pada pola banyak-key). Tapi `ra_core_singleblock` (dan
`ra_core_orbit` kalau dipakai reinit agresif) **belum boleh dianggap
selesai divalidasi** sampai fix dari folder di atas diimplementasikan dan
lolos verifikasi ulang.

## Belum dikerjakan / perlu keputusan user

- **Shuffle implementation test** (entropy/chi-square/runs-test/serial
  correlation/multi-key distinctness, pola `scc_test.py`/`verify_parity.py`
  di `2026-8-30_addressable-shuffle/`) — HANDOVER.md menghedge ini dengan
  "kalau relevan untuk use-case shuffle". `ra_core_orbit` sudah punya
  padanan teruji (`scrambler_wired_addressable.c`, kombo wired+addressable
  yang sama). Untuk `ra_core_singleblock`: apakah mode ≤255-kata ini
  dimaksudkan untuk use-case shuffle (mis. Fisher-Yates array kecil), atau
  murni untuk addressable-number generation biasa (bukan shuffle)? Belum
  dikerjakan, menunggu klarifikasi.
- Promosi ke `src/` — tetap ditangguhkan (open decision #3 HANDOVER §4),
  di luar cakupan Tahap 2 sampai semua axis di atas + shuffle (kalau
  relevan) tuntas dan dikonfirmasi user.

## File yang dihasilkan sesi ini

- `ra_core_singleblock_cli.c`, `multikey_stream.c`, `bench_ra_core.c` (folder ini)
- `avalanche_ra_core.py`, `run_dieharder_battery_multikey.py` (folder ini)
- `cross_correlation_ra_core_singleblock.py`, `collision_scan_ra_core_singleblock.py`
  (`experiments/2026-8-29_parallelization-research/`, pola re-point binary
  yang sama dengan `cross_correlation_ra_prng2.py`)
- `dieharder_ra_core_singleblock_multikey_piped.txt`, `multikey_dieharder_run.log`,
  `practrand_singleblock_multikey.log`, `cross_correlation_singleblock_results_full.json`,
  `collision_scan_singleblock_results_{sequential,random}_full.json`
