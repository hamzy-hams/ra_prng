# RESULTS: Pareto selection over the 11 combo-search winners

Implements `HANDOVER.md` in this folder. Status: **executed, 2026-09-03.**

## Konteks

`../2026-9-2_singleblock-cycle-combo-search/RESULTS.md` found **11 candidates
FULLY CLEAN** PractRand (16GB, K∈{1,2,4,8,16,32,64,96}) out of 756 DSL
combinations for `ra_core_singleblock`'s per-round transform (tap width=8,
various finalizers on `c`). All 11 still tied at that point — this task adds
two stricter axes (speed, multi-seed avalanche + normality) to narrow them
further, and positions the survivor(s) against the other PRNG families
already benchmarked in `../2026-8-31_prng-family-benchmark/`.

**Non-goal (unchanged from the combo-search HANDOVER):** nothing here is
promoted to `ra_core.c`. `git diff --stat -- ../2026-9-1_family-productionization/ra_core.c`
stayed empty throughout (verified at the end, see Verifikasi).

## Deviations from HANDOVER.md (documented, both investigated before deviating)

1. **Baseline source.** §7 says copy the baseline `singleblock` row from
   `../2026-8-31_prng-family-benchmark/benchmark_all.c`. That file
   (dated 2026-8-31) predates `../2026-9-1_keyzero-guard-fix/`, which changed
   `ra_init_state_singleblock`'s formula (additive `i*C + C*key` → the
   current `fmix32(GUARD_M ^ (C*key))` keyterm multiply). benchmark_all.c's
   `ra_init_state_full` still has the *old* formula — copying it would have
   silently benchmarked a stale, non-production init as "baseline" for the
   before/after comparison §5 asks for. `winners_bench.c`'s baseline is
   instead copied verbatim from `ra_core.c` L205-260 directly. Verified
   correct: `winners_bench --stream singleblock 1 1` reproduces
   `2452187902`, the exact KAT value the combo-search RESULTS.md's own
   Verifikasi section cites for production `ra_core_singleblock(key=1,rng=1)`.
2. **K=1 measurement robustness.** A single 3-trial `mode_reinit_sweep`
   invocation (as ported verbatim from `benchmark_all.c`) turned out to be
   noisy on this machine — two independent full runs of all 12 cores gave
   K=1 numbers differing by up to ~2x for the same candidate (e.g.
   `w8_f22_i0`: 76.1 vs 41.2 ns/word), while K=255 and continuous-stream
   MB/s stayed stable (~10-15% spread) across the same two runs. This points
   to whole-process-level VM contention, not a per-candidate effect — every
   core showed the same pattern. Fix: K=1 was re-measured as **7 independent
   `winners_bench reinit-sweep` invocations per core** (each already
   min-of-3-trials internally), and the **minimum across the 7** is reported
   below as `ns_per_word` at K=1 (least-contended sample, most representative
   of true steady-state cost). Full spread is in `k1_repeats.jsonl`. K=255
   and continuous-stream are the mean of the 2 original runs
   (`axis_a_results_run1.jsonl`, `axis_a_results.jsonl`).

## Axis A — Speed

`winners_bench.c`/`winners_bench_run.py`, methodology per HANDOVER §2:
`mode_reinit_sweep` ported verbatim from `benchmark_all.c` for K=1/K=255;
continuous-stream is a real `--multistream` run (chunked multikey, Weyl key
increment between K=255 blocks, `base_key=111222`) to `/dev/null`,
1,000,000,000 words (4GB), wall-clock timed outside the process.

| candidate | K=1 ns/word | K=255 ns/word (diagnostic) | continuous-stream MB/s |
|---|---:|---:|---:|
| `singleblock` (baseline) | 39.063 | 0.1915 | 154.8 |
| w8_f8_i0 | 37.014 | 0.1625 | 127.4 |
| w8_f9_i0 | 38.765 | 0.1800 | 124.7 |
| **w8_f10_i0** | **36.692** | 0.1395 | **134.1** |
| w8_f22_i0 | 38.675 | 0.1870 | 124.4 |
| w8_f24_i0 | 46.025 | 0.1485 | 127.6 |
| w8_f25_i0 | 38.576 | 0.1810 | 125.4 |
| w8_f26_i0 | 38.746 | 0.1855 | 123.9 |
| w8_f27_i0 | 38.742 | 0.1695 | 121.1 |
| **w8_f28_i0** | 37.405 | 0.2260 | 128.0 |
| w8_f29_i0 | 40.691 | 0.2250 | 123.1 |
| w8_f33_i0 | 41.798 | 0.1525 | 121.7 |

Bold = Pareto frontier (§4 below). All 11 winners land within noise of the
baseline at K=1 (36.7–46.0 vs baseline 39.1 ns/word) — consistent with the
combo-search RESULTS.md's own microbench finding (0.90x–1.06x overhead via a
different harness/methodology) that the finalizer's 0–2 extra instructions
don't meaningfully move K=1 cost, which is dominated by the 256-word `M[]`
init. `w8_f24_i0` is a mild, reproducible outlier (46.0 min-of-7, all 7 reps
above the rest's floor of ~37–41) — not disqualifying on its own (still only
~18% over baseline), but noted since every other candidate clustered
tighter; no structural reason found (its finalizer, `XORSHIFT(16)->MUL(0x9e3779b7)`,
isn't meaningfully different in cost from `w8_f22_i0`/`w8_f25_i0`'s same
`XORSHIFT(16)->MUL(other-const)` shapes, which don't show the same lift).
Continuous-stream MB/s: baseline is fastest (154.8, no finalizer to run),
all 11 winners cluster 121–134 MB/s (finalizer overhead is a larger fraction
of a *full* 255-word cycle's total than it is of the noise-dominated K=1
number).

## Axis B — Avalanche multi-seed + normalitas

`avalanche_multiseed.py`, methodology per HANDOVER §3: 64 seeds
(deterministic, seed=1..64), each captured as one full K=255 cycle via the
already-compiled `../2026-9-2_.../candidates/<id>` binary's `--single <seed>
255` (no recompilation), `avalanche_stats()` reused (imported) from
`../2026-8-26_operation-pruning-research/quality_gate.py`. 704/704 expected
seed-rows collected (11 × 64).

| candidate | shapiro p (min_bit) | shapiro p (max_bit) | crit1 (both p>0.05) | floor breaches (<0.2) | max argmin concentration | crit3 (≤32/64) | **Axis B result** |
|---|---:|---:|:---:|---:|---:|:---:|:---|
| w8_f8_i0 | 0.00426 | 0.2211 | FAIL | 0 | 6/64 | pass | **FAILED_AXIS_B** |
| w8_f9_i0 | 0.05235 | 0.04802 | FAIL | 0 | 5/64 | pass | **FAILED_AXIS_B** |
| **w8_f10_i0** | 0.382 | 0.1186 | pass | 0 | 5/64 | pass | **OK** |
| w8_f22_i0 | 0.2037 | 0.02208 | FAIL | 0 | 5/64 | pass | **FAILED_AXIS_B** |
| w8_f24_i0 | 0.09614 | 0.000884 | FAIL | 0 | 7/64 | pass | **FAILED_AXIS_B** |
| w8_f25_i0 | 0.8841 | 0.5584 | pass | 0 | 6/64 | pass | OK (not on frontier — dominated, see §4) |
| w8_f26_i0 | 0.00211 | 0.1691 | FAIL | 0 | 7/64 | pass | **FAILED_AXIS_B** |
| w8_f27_i0 | 0.00053 | 0.4092 | FAIL | 0 | 5/64 | pass | **FAILED_AXIS_B** |
| **w8_f28_i0** | 0.548 | 0.8286 | pass | 0 | 5/64 | pass | **OK** |
| w8_f29_i0 | 0.00051 | 0.04252 | FAIL | 0 | 4/64 | pass | **FAILED_AXIS_B** |
| w8_f33_i0 | 0.00017 | 0.02521 | FAIL | 0 | 9/64 | pass | **FAILED_AXIS_B** |

**8/11 candidates fail Axis B criterion 1** (normality of `min_bit_fraction`
and/or `max_bit_fraction` across the 64 seeds) — only `w8_f10_i0`,
`w8_f25_i0`, `w8_f28_i0` pass. Criterion 2 (absolute floor 0.2) and
criterion 3 (no single weakest-bit index dominating >50% of seeds) both pass
for **all 11 candidates** — no evidence of the `pruned_winner`-style
near-dead-bit defect at any seed, and weakest-bit position is well spread
(max concentration 9/64 seeds sharing one index, `w8_f33_i0`). Per HANDOVER
§4, criterion 1 or 3 failure means outright exclusion from the Pareto
frontier even though every candidate's raw `min_bit_fraction` numbers
(worst per candidate: 0.477–0.483, all comfortably above the 0.2 floor and
far from the ~0.008 dead-bit pattern seen historically) look fine in
isolation.

**Methodology caveat, stated explicitly per HANDOVER §3's own
instruction not to soften a failure**: Shapiro-Wilk on 64 samples drawn from
a quantized statistic (`per_bit_fraction` is a mean of 255 Hamming-distance
bits, so it takes a limited set of discrete values, not a truly continuous
one) is known to be sensitive to ties/discreteness and can reject normality
for reasons unrelated to a real structural defect. This experiment does not
have a second, independent signal (e.g. a QQ-plot-based human check, or a
non-parametric alternative) to distinguish "genuine skew/structure" from
"discreteness artifact" for the 8 FAILED_AXIS_B candidates — the criterion
is applied exactly as specified in HANDOVER §3, not reinterpreted, but this
means a real possibility exists that some of the 8 excluded candidates are
false negatives from this specific normality test rather than genuinely
worse than the 3 survivors. Flagged here rather than left implicit, per the
combo-search RESULTS.md's own precedent of stating such limits plainly (see
that file's "Batas metodologi penting").

No FAIL/borderline result in either axis was re-run with a different seed to
explain it away, consistent with this research line's standing rule.

## Pareto frontier

3 objectives (`pareto_select.py`): `ns_per_word`@K=1 (min better),
continuous-stream MB/s (max better), `min(min_bit_fraction)` across the 64
Axis B seeds (max better). Only the 3 Axis-B-`OK` candidates are eligible.

**Result: 2 co-frontier candidates, no single forced winner** (per HANDOVER
§4's explicit allowance):

| candidate | K=1 ns/word | stream MB/s | min_bit_fraction (worst of 64 seeds) |
|---|---:|---:|---:|
| **w8_f10_i0** | **36.692** (best) | **134.1** (best) | 0.4810 |
| **w8_f28_i0** | 37.405 | 128.0 | **0.4827** (best) |
| w8_f25_i0 (Axis-B OK, not on frontier) | 38.576 | 125.4 | 0.4826 |

`w8_f25_i0` is Axis-B eligible but dominated by `w8_f10_i0` (worse on all 3
objectives simultaneously) — excluded from the frontier by ordinary Pareto
dominance, not by an Axis B failure. `w8_f10_i0` and `w8_f28_i0` are mutually
non-dominated: `w8_f10_i0` wins on both speed metrics, `w8_f28_i0` wins on
worst-case avalanche safety — a genuine speed-vs-safety-margin trade-off,
not forced into one winner per HANDOVER §4.

## Perbandingan vs keluarga PRNG lain

Tiga tabel berikut dikutip **verbatim** dari
`../2026-8-31_prng-family-benchmark/RESULTS.md` (baris lama tidak diubah,
tidak dijalankan ulang), dengan baris baru ditambahkan untuk kedua kandidat
Pareto-frontier per HANDOVER §5.

### Seeding cost — OLS fit `call_ns(K) = a + b·K` (quoted verbatim, no new rows — this table's OLS fit wasn't re-run here, see HANDOVER §5 point 2)

| core | `a_ns` (fixed seed cost) | `b_ns_per_word` (steady-state) |
|---|---:|---:|
| philox | −6.1 *(noise around ~0 — Philox's reseed really is just a word assignment)* | 2.600 |
| splitmix64 | 3.6 | 0.245 |
| pcg32 | 4.8 | 1.288 |
| paperorig | 10.4 | 5.263 |
| xoshiro256 | 12.7 | 0.937 |
| singleblock | 33.7 | 0.001 |
| wiredv2 | 33.8 | 2.080 |
| addrcont | 39.4 | 2.077 |
| chacha20 | 47.9 | 7.344 |
| **mt19937** | **1257.2** | 1.775 |

### K=1 (frequent reinit) vs largest-K (steady state)

| core | ns/word at K=1 | largest K measured | ns/word at largest K |
|---|---:|---:|---:|
| splitmix64 | 3.0 | 1,000,000 | 0.245 |
| pcg32 | 3.2 | 1,000,000 | 1.288 |
| xoshiro256 | 6.5 | 1,000,000 | 0.937 |
| philox | 12.1 | 1,000,000 | 2.600 |
| paperorig | 24.9 | 1,000,000 | 5.263 |
| wiredv2 | 25.2 | 1,000,000 | 2.080 |
| singleblock | 33.4 | 255 *(hard cap, see note)* | 0.132 |
| addrcont | 52.1 | 1,000,000 | 2.077 |
| chacha20 | 121.5 | 1,000,000 | 7.344 |
| **mt19937** | **1289.4** | 1,000,000 | 1.776 |
| **w8_f10_i0 (combo-winner, Pareto-frontier)** | **36.7** | 255 *(same hard cap as singleblock)* | 0.1395 |
| **w8_f28_i0 (combo-winner, Pareto-frontier)** | **37.4** | 255 *(same hard cap)* | 0.2260 |

`singleblock`'s "largest K" is capped at 255 by design (no-reseed fast path,
valid only for `rng<=255`), so its steady-state column isn't the same
asymptotic flat-throughput regime the uncapped candidates reach at
K=1,000,000; same caveat applies to both new combo-winner rows (identical
cap, same reason). The new rows' K=1 numbers (36.7/37.4) were measured in
this session on this machine (see "Deviations" above re: noise/methodology)
and are **not directly comparable in absolute terms** to the other rows'
figures, which come from a different session/machine per the original
`prng-family-benchmark` RESULTS.md — only the new rows' relative ordering
against this session's own `singleblock` re-measurement (39.063, see Axis A
table) is a fair like-for-like comparison: both combo-winners are *faster*
than this session's baseline re-measurement at K=1, not slower.

### MB/s sanity check

| core | measured MB/s | HANDOVER.md reference | ratio |
|---|---:|---:|---:|
| singleblock | 30303.0 | n/a *(no reseed within cap — see note above, not directly comparable)* | — |
| splitmix64 | 16326.5 | n/a *(new candidate, no prior literature figure)* | — |
| xoshiro256 | 4268.9 | 3574.0 | 1.19 |
| pcg32 | 3105.6 | 3065.0 | 1.01 |
| mt19937 | 2252.3 | n/a *(new candidate)* | — |
| addrcont | 1925.9 | n/a *(no prior flat-throughput figure)* | — |
| wiredv2 | 1923.1 | n/a | — |
| philox | 1538.5 | 1304.0 | 1.18 |
| paperorig | 760.0 | 745.6 | 1.02 |
| chacha20 | 544.7 | 555.5 | 0.98 |
| dev_urandom | 507.0 | 389.6 | 1.30 |
| **w8_f10_i0 (combo-winner, Pareto-frontier)** | **134.1** | n/a | — |
| **w8_f28_i0 (combo-winner, Pareto-frontier)** | **128.0** | n/a | — |

**The old `singleblock` row's 30303.0 MB/s is NOT the same measurement as
this task's new rows and must not be compared directly.** That old number
was computed from the K=255 `mode_reinit_sweep` `ns_per_word_steadystate`
figure algebraically converted to MB/s (single-block-per-call, no real
syscall/chunking overhead) — explicitly annotated in the original table as
"no reseed within cap, not directly comparable" even to the *other* rows in
that same old table. This task's new rows use the **real, syscall-inclusive
`--multistream` path** HANDOVER §2 specifically asks for ("bukan microbench
loop... pakai jalur nyata yang sesungguhnya dipakai riset ini untuk
PractRand"), which is why they land two orders of magnitude lower (~130
MB/s vs ~30,000 MB/s) despite testing the same family — the two numbers
answer different questions ("cost of one already-warm in-process call" vs.
"real sustained throughput including repeated syscall/process-level
reinit overhead"). For a fair same-methodology comparison, see the new
`singleblock` continuous-stream re-measurement in the Axis A table above
(154.8 MB/s) instead — both combo-winners are within ~13-17% of that number
(slower, as expected: finalizer adds instructions per word).

## Rekomendasi

**2 kandidat co-frontier ditemukan, TIDAK dipaksa jadi satu pemenang**
(outcome valid per HANDOVER §4):

- **`w8_f10_i0`** (finalizer `c ^= c >> 17;` — width=8, 1 instruksi, tanpa
  MUL) untuk prioritas **kecepatan**: tercepat di K=1 (36.7 ns/word) dan
  continuous-stream (134.1 MB/s) di antara kedua kandidat frontier, avalanche
  worst-case sedikit di bawah `w8_f28_i0` (0.4810 vs 0.4827 — keduanya jauh
  di atas ambang 0.2, selisihnya kecil).
- **`w8_f28_i0`** (finalizer `MUL(0xc2b2ae35)->XORSHIFT(13)`) untuk
  prioritas **avalanche worst-case**: marjin keamanan bit-avalanche
  terbaik di antara kedua kandidat frontier, dengan kecepatan yang hampir
  sama (37.4 ns/word, selisih <1ns dari `w8_f10_i0`).
- Kedua kandidat sama-sama lolos kriteria normalitas + konsentrasi bit lemah
  Axis B (§3) yang menggugurkan 8 dari 11 pemenang combo-search lainnya —
  ini adalah sinyal tambahan yang TIDAK diuji di eksperimen combo-search asal
  (yang hanya menguji K=255 satu seed).

**TIDAK direkomendasikan mempromosikan otomatis ke `ra_core.c`** — di luar
scope task ini per HANDOVER §8, butuh konfirmasi eksplisit user terpisah.
Sebelum promosi, per HANDOVER §8 dan rekomendasi
`../2026-9-2_.../RESULTS.md` yang masih berlaku:

1. Uji dieharder (battery gate yang di-PAUSE per
   [[project_ra_prng_family_productionization]]) untuk `w8_f10_i0`/
   `w8_f28_i0` — belum dilakukan di sini maupun di eksperimen combo-search
   asal, PractRand-only sejauh ini.
2. Crossing dengan 7 varian extra-inject (slot 3 DSL combo-search), yang
   sengaja belum diuji di Promotion Tier (semua 29 kandidat pakai
   inject_idx=0/off) — kalau extra-inject akan tetap dipakai di produksi,
   perlu diverifikasi tidak merusak kebersihan yang sudah ditemukan.
3. Mengingat catatan metodologi Axis B di atas (Shapiro-Wilk pada data
   terkuantisasi bisa sensitif terhadap artefak, bukan cuma defect
   sungguhan), sesi lanjutan yang ingin mempertimbangkan kembali 8 kandidat
   FAILED_AXIS_B sebaiknya menjalankan uji normalitas alternatif
   (non-parametrik, atau inspeksi QQ-plot manual) sebelum menggugurkannya
   secara permanen dari pertimbangan — bukan tugas task ini untuk
   memutuskan itu (kriteria HANDOVER diterapkan apa adanya di sini), tapi
   dicatat sebagai batasan yang eksplisit.

## Verifikasi

- Setiap 11 fungsi kandidat di `winners_bench.c` diverifikasi byte-identik
  terhadap binary kandidat asli
  (`../2026-9-2_.../candidates/<id>`) via `--stream <id> <key> <rng>` vs
  `<id> --single <key> <rng>`, untuk 5 key × 4 panjang (`0, 1, 42,
  4294967295, 2654435761` × `1, 2, 8, 255`) — 220 perbandingan, 0 mismatch.
- Baseline `winners_bench --stream singleblock 1 1` = `2452187902`,
  cocok dengan KAT produksi yang dikutip di
  `../2026-9-2_.../RESULTS.md`'s Verifikasi section.
- `avalanche_multiseed_results.jsonl`: 704/704 baris (11 kandidat × 64 seed)
  seperti yang diharapkan.
- `git diff --stat -- ../2026-9-1_family-productionization/ra_core.c`: kosong
  sepanjang task ini.
- Tidak ada tulisan/perubahan ke `../2026-9-2_singleblock-cycle-combo-search/candidates/*.c`,
  `RESULTS.md`, `combo_prng.py`, `recipes.py`, atau
  `../2026-8-31_prng-family-benchmark/*` di manapun dalam task ini.
- Tidak ada FAIL/hasil borderline (Axis A atau Axis B) yang di-rerun dengan
  seed berbeda untuk "menjelaskan" hasilnya.
- Tidak ada PractRand atau dieharder dijalankan di task ini — murni
  benchmark speed + seleksi statistik avalanche di atas 11 kandidat yang
  sudah lolos Promotion Tier 16GB sebelumnya.

## Closing note

`CLAUDE.md` proyek ini mewajibkan `/graphify --update` sebelum menutup tugas
riset. Tidak ada CLI/skill/MCP `graphify` yang benar-benar terpasang di
environment sesi ini (diverifikasi ulang: tidak ada binary, tidak ada skill
Claude Code terdaftar, tidak ada di PATH) — `graphify-out/` hanya berisi
snapshot pre-generated dari environment lain. Konsisten dengan temuan yang
sama persis di `../2026-8-31_prng-family-benchmark/RESULTS.md`'s "Closing
note" dan riset sebelumnya di repo ini — bukan regresi baru, dilaporkan
apa adanya alih-alih dilewati diam-diam.
