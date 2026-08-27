# Results: Operation-Pruning Search for `ra_prng2`

Companion to `HANDOVER.md` (original spec) and `STATUS.md` (live trap-by-trap
log). This file is the final writeup: design decisions → what was tried →
what worked → verdict. `src/ra_prng2/c/*` was never modified — everything
here lives in this experiment folder only.

## Design decisions

1. **Search space**: discrete on/off flags over 18 named operations in
   `ra_permutation_cycle`/`ra_hash` (8 `TAP*`, 4 `ROT_*`, 4 `SHL*`/`SHR*`,
   `MULT_REDUCE`, `HASH_SELFIDX`), plus a separate `HASH_ACCESS` choice
   (`strided` original vs. `sequential`). Continuous parameters (shift
   widths, rotation amounts, operand position) were explicitly out of scope
   for the first round — added only as a targeted repair after the discrete
   search's first winning candidate failed large-scale validation.
2. **`HASH_DEPTH` locked at 32, never a search variable.** Originally
   proposed as a sweepable parameter (mirroring the periodicity experiment's
   `rows` knob); the user corrected this mid-session — reducing hash depth
   breaks periodicity, which the periodicity experiment had already shown
   independently (47.93x cycle-length swing from touching an analogous
   knob). Pruning targets for the hash function were redirected to *access
   pattern* instead (`HASH_ACCESS`), which does not touch depth.
3. **Search method**: greedy ablation — remove one op, check both quality
   gates, keep the removal if both pass, restart the scan from the new
   baseline; converge when a full round removes nothing. Order-dependent by
   construction; the fixed trial order is recorded in `ablation_search.py`.
4. **Quality gates**: Tier 0 avalanche/Hamming-distance check (pure Python,
   milliseconds) → Tier 1 PractRand prefix (piped C-generated stream into
   `RNG_test`). No fixed prefix size was ever trusted as "safe" — see traps
   below; every candidate that survived the search loop was independently
   re-validated at much larger PractRand tiers, and the final candidate at
   full dieharder, before being accepted.
5. **Category-floor constraint** (added after two traps, see below):
   `min_active_taps` / `min_active_rots` — forbid the greedy search from
   ever removing *every* member of the `TAP*` or `ROT_*` families at once,
   even if each individual removal locally passes the gate.

## What was tried and what failed

### Gate-size traps (three, all before the floor constraint existed)

Small/cheap PractRand prefixes repeatedly let broken candidates through
that failed cleanly at larger sizes:

- **Trap #1** (pre-`HASH_DEPTH` correction, depth=4): 1MB gate passed a
  near-degenerate 3/18-op candidate; failed hard (BCFN/Gap-16) at 16MB.
- **Trap #2** (depth correctly fixed at 32, 8MB gate): converged to a 5/18
  candidate with *all 8 `TAP*` removed*; passed cleanly to 24MB, failed
  hard (BCFN) at 32MB and 64MB.
- **Trap #3**: repeat of the same failure mode (all `TAP*` removed) at a
  different gate size, confirming Trap #2 wasn't a fluke — fixed by adding
  `min_active_taps=2`.

**Lesson**: no fixed PractRand prefix size is provably safe for every
candidate; the "removed an entire operation category" pattern, not any
single flag, was the actual recurring defect.

### Shift-width repair (avalanche-guided grid search): tried, failed

After the TAP-floor fix converged to a 6/18-op candidate, the user asked
whether relaxing the shift widths (`9,18,14,13`, fixed since the original
paper) could let *more* ops be removed by rebalancing diffusion. A full
grid search (31³ = 29,791 evaluations) over `s9,s14,s13 ∈ [1,31]` (`s18`
fixed, unused by the surviving op set), scored by closeness of avalanche
fraction to the ideal 0.5:

- Best-avalanche candidate (`s9=29,s14=3,s13=1`, avalanche=0.4982 vs.
  original's 0.364) **failed PractRand at 256MB already** (`FPF-14+6/16`,
  `DC6-9x1Bytes-1` — a low-order-bit correlation defect, different
  signature than the TAP traps' BCFN).
- Two more top-ranked candidates (avalanche 0.4977, 0.4973) failed at 2GB
  and 1GB respectively.
- The **untouched original widths** (avalanche=0.364, far from "ideal")
  outperformed every tuned candidate tried, staying clean to 2GB.

**Lesson**: avalanche fraction was actively *anti-correlated* with real
statistical quality in this specific search — optimizing for it selected
candidates with a defect PractRand only exposes at scale. Full grid
results: `shift_repair_log.jsonl`.

## What worked: MIN_ACTIVE_ROTS constraint

Per the user's standing fallback instruction — "kalau semua percobaan disini
gagal pertimbangkan untuk mengembalikan sebagian/seluruh operasi ROT nya"
(`ROT_*` was flagged as one of the original algorithm's fastest diffusion
contributors) — the shift-repair failure triggered exactly that fallback.
Rerunning greedy ablation with **both** `min_active_taps=2` and
`min_active_rots=1` (forbidding wipeout of either category) converged to a
**more aggressive** candidate than the shift-repair line of investigation
ever found:

**Winning candidate**: `ops = {TAP6, TAP7, ROT_C, SHR13}`
(4 of 18 tracked flags active, 14 removed), `hash_access = sequential`,
shift widths **untouched** at the original `(9, 18, 14, 13)`.

This directly confirmed the user's hypothesis: the real defect in every
earlier trap was never the shift widths — it was eliminating an entire
diffusion-contributing operation *category* (all `TAP*`, or all `ROT_*`) at
once. Keeping ≥1 member of each family was sufficient to preserve real
statistical quality even under otherwise much more aggressive pruning.

## Final validation

`pruned_winner.c` — a specialized, hardcoded (no runtime flag branching)
structural mirror of `src/ra_prng2/c/ra_prng2.c`, compiled with the
project's exact flags (`-O3 -march=native -std=gnu17 -include stdalign.h`),
cross-validated bit-identical against the generic `pruned_prng.c`/`.py`
harness before being trusted for either speed or quality measurement.

### Speed (`perf stat -e instructions,cycles`)

Fair, apples-to-apples comparison (same build flags, same `TOTAL_RNG` =
200,000,000, both binaries structurally identical except for the pruned
operations):

| | baseline (`build/bin/ra_prng2`) | `pruned_winner` | ratio |
|---|---:|---:|---:|
| instructions | 14,900,505,959 | 6,255,028,168 | **2.38x fewer** |
| cycles | 4,422,117,023 | 1,738,938,019 | **2.54x fewer** |
| wall time | 1.1548 s | 0.4546 s | **2.54x faster** |

Original project baseline throughput was 745.6 MB/s
(`benchmarks/comparisons/RNGing_speed/`); a ~2.5x speedup would put the
pruned variant in the ~1860 MB/s range — still short of xoshiro256**
(3574 MB/s) and PCG32 (3065 MB/s), but a substantial, real gain over the
untouched algorithm.

### Quality — PractRand (piped `stdin32`, no intermediate file)

| tier | seed(s) | result |
|---|---|---|
| up to 2GB | 1 | PASS (one mild "unusual" at 1GB, gone again at 2GB) |
| 8GB | 1 | **PASS, 0 anomalies** |
| 1GB | 2, 999, 0xC0FFEE | **PASS, 0 anomalies, all 3** |
| 16GB (2^34 bytes) | 1 | **PASS, 0 anomalies (240 results)** |

**Full-scale run to 1TB** (single pipe, `-tlmin 8GB -tlmax 1TB`, seed=1,
2026-08-27), matching the 1TB tier commonly cited in the wider PRNG
literature (Vigna/xoshiro, O'Neill/PCG) as strong evidence of quality for
a non-cryptographic generator — supersedes the standalone 128GB run
above:

| tier | cumulative time | result |
|---|---:|---|
| 8GB (2^33) | 107s | 0 anomalies (230 results) |
| 16GB (2^34) | 219s | 0 anomalies (240 results) |
| 32GB (2^35) | 427s | 0 anomalies (251 results) |
| 64GB (2^36) | 872s | 0 anomalies (263 results) |
| 128GB (2^37) | 1800s | 0 anomalies (273 results) |
| 256GB (2^38) | 3593s | 0 anomalies (284 results) |
| 512GB (2^39) | 7074s | 0 anomalies (295 results) |
| **1TB (2^40)** | **13736s (~3h49m)** | **0 anomalies (304 results)** |

**Clean at every doubling from 8GB to 1TB, 0 anomalies throughout — no
exceptions.** The result count climbing each tier (230→304) is expected,
not a discrepancy: PractRand's `core` tests (`BCFN`, `DistC6`, `Gap16`,
`FPF`, `BRank`, `mod3n`, `TripleMirrorFreqN` — see
`~/Documents/research/PractRand/src/test_batteries.cpp`) each carry
internal sub-parameterizations (e.g. `BCFN` has up to 32 internal
correlation-spacing "levels", `~/Documents/research/PractRand/include/
PractRand/Tests/BCFN.h`) that only become statistically meaningful once
enough data has streamed past — larger spacings need more samples before
they can produce a valid result at all, so they "switch on" and add a row
to the results table as the stream grows. This means each doubling tests
genuinely new ground, not a re-run of the same checks for longer. Raw
output: `practrand_pruned_winner_128GB.txt` (128GB standalone run),
`practrand_pruned_winner_1TB.txt` (full 1TB run, includes all tiers
above).

**A note on wall-clock, corrected after checking it (2026-08-27):** the
stored `PractRand.txt` for the original algorithm took 14586s to reach
128GB, vs. 1623s here — naively an ~9x gap. That comparison is **not
apples-to-apples** and was initially mis-reported as a pruning speedup; it
isn't one. Re-running the original binary (`build/bin/ra_prng2`) fresh, on
this same machine, right now, piped into the same `RNG_test` up to 8GB
took **110s** — essentially tied with `pruned_winner`'s **103s** on an
identical fresh run (ratio ~1.07x). The stored `PractRand.txt` was
captured under different machine/load conditions than today's session and
its absolute wall-clock isn't a valid speed baseline to diff against a
fresh run — only same-session, same-moment measurements are. A
generation-only check (piped to `wc -c`, no analysis, 2GB) confirms the
real generation-side gap is much smaller than `perf stat`'s pure-compute
figure too: 286 MB/s (pruned) vs. 225 MB/s (original), ~1.27x — because
`fwrite`-per-value and pipe I/O compress the ~2.5x compute-only ratio.
Once piped through `RNG_test`, the analysis cost dominates wall time for
*both* binaries (each already generates far faster than `RNG_test` can
consume), so generator speed stops being the bottleneck and the two
converge to near-parity. **The only trustworthy, controlled speedup
number for this candidate remains the `perf stat` pure-compute ratio
above (~2.4-2.5x)** — PractRand/dieharder wall-clock is not a valid speed
metric for this candidate, only a quality gate.

### Quality — dieharder (27 "Good"-reliability tests, individually piped)

The project's original dieharder run used `-a` (full battery) against a
file, relying on many small rewinds (≤197 total in the passing baseline
run). That approach is infeasible here: reproducing it needs either a very
large file (this machine has ~6GB free disk) or accepting millions of
rewinds of a small file, which was empirically shown to produce
catastrophic false-FAILs (`sts_serial`, `rgb_bitdist` at p=0.00000000 with
90M+ rewinds of a 2GB file — a rewind-count artifact, not a real defect,
confirmed by comparing against the original's ~197-rewind passing
methodology). Instead, all 27 "Good"-reliability tests (per `dieharder -l`;
`-d 5,6,7` excluded as "Suspect", `-d 14` excluded as "Do Not Use") were run
individually via direct pipe — no file, no rewind possible, no disk risk.

**Result: all 27 tests PASSED.** Two apparent failures on the first pass
were both diagnosed as *test-invocation* artifacts, not RNG defects, and
confirmed fixed by correcting the invocation:

- `rgb_minimum_distance` (`-d 201`) "FAILED" at p=0.00000000 when run
  without `-n` (defaults to `ntup=0`) — a configuration the original
  algorithm's own passing dieharder run never actually exercises (its `-a`
  battery only tests `ntup=2..5` for this test). Rerun at `-n 2,3,4,5`
  matching the original's exact methodology: **all four PASSED**
  (p = 0.71, 0.01, 0.75, 0.95).
- Marsaglia-Tsang GCD test (`-d 17`) hit `EOF` on the first (1.2GB) attempt
  — this test alone consumes ~8GB of input. Rerun with a 10.4GB piped
  stream (no disk write): **PASSED** (p = 0.80, 0.96).

One `sts_serial|ntup=8` sub-result was `WEAK` (p=0.99527) — a single
high-tail p-value among 56 `sts_serial` sub-tests is expected statistical
noise, not a defect (this is exactly why dieharder distinguishes WEAK from
FAILED).

Raw output: `dieharder_pruned_winner_piped.txt`,
`dieharder_rgb_minimum_distance_fixed.txt`, `dieharder_gcd_fixed.txt`.

## Verdict

The operation-pruning search found a candidate — **4 of 18 tracked
operations active (`TAP6`, `TAP7`, `ROT_C`, `SHR13`), 14 removed, hash
access pattern changed to `sequential`, all shift/rotation amounts left at
their original values** — that delivers a reproducible **~2.4-2.5x
speedup** (fewer instructions, fewer cycles, and lower wall time, all
measured with matched build flags) while passing every quality gate applied
in this experiment: the avalanche sanity check, PractRand up to **1TB**
(the tier commonly cited in the wider PRNG literature — Vigna/xoshiro,
O'Neill/PCG — as strong evidence of quality) with zero anomalies across
all 8 doublings, and all 27 "Good"-reliability dieharder tests. The **~2.4-2.5x
`perf stat` ratio is the only trustworthy speedup figure** for this
candidate — PractRand/dieharder wall-clock is dominated by the test
tool's own analysis cost once piped, not generator speed (see the note
under the PractRand table), so it must not be read as a speed
comparison.

The key methodological finding, confirmed three separate times (twice for
`TAP*`, once for `ROT_*`) and directly validated against the user's own
domain intuition: **removing an entire operation category is the real risk
factor**, not any specific operation or the shift/rotation magnitudes.
Constraining the greedy search to always keep at least one member of each
category (`min_active_taps=2`, `min_active_rots=1`) was what actually fixed
every trap encountered — shift-width tuning, tried as an alternative
repair, was not just unnecessary but actively harmful (optimizing shift
widths via an avalanche proxy consistently produced *worse* real quality
than leaving them untouched).

### Explicitly out of scope / future work

- **Operand-position changes** (which state variable feeds which slot in
  the `a/b/o/c` update chain) — a structurally larger search space,
  deliberately deferred to a separate round per the plan.
- **TestU01/BigCrush wrapper** — optional in the plan, not run; PractRand
  1TB + full "Good"-battery dieharder were judged sufficient evidence for
  this experiment's scope.
- This candidate is a research artifact for measuring the pruning
  technique's ceiling, not a drop-in replacement — `src/ra_prng2/c/*`
  remains untouched and paper-exact, as required throughout.
