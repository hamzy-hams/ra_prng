# Tahap 0 Results: Addressable-Init Mechanism Test

Pure-Python prototype (`tahap0_prototype.py`), per HANDOVER.md's Tahap 0 spec.
Never modifies `winner_wired_v2.c`/`ra_prng2.c`; port cross-validated
bit-identical against the compiled `winner_wired_v2` binary before any
candidate result below is trusted (`sanity_check()`, seeds 1/42/12345, n=2000
-- PASSED).

## Method recap

For each candidate, sweep a set of addresses, build `(L, M)`, and check three
things:
1. **multiset digest** of `L` (sorted, order-independent) -- catches two
   addresses landing on the same *set* of L values.
2. **stream digest** of the generated output after several reseed cycles --
   catches actual long-run behavioral convergence (the real concern from
   HANDOVER.md), not just a static-state coincidence.
3. **avalanche** (`avalanche_stats()`, reused unmodified from
   `quality_gate.py`) -- confirms the addressable variant doesn't regress the
   diffusion property `winner_wired_v2.c` was chosen for.

Collisions are counted via a shared dict of 64-bit blake2b digests (same
technique as `collision_scan.py`), with `digest_collision_prob()` giving the
birthday-bound false-positive rate for that scale (negligible at every scale
run below -- collisions found are real, not hash noise).

## Tiers run

| tier  | addresses/candidate (K1/K2/K4) | K5 (×4 multipliers) | K3 (counter sweep) | stream words/address |
|-------|-------------------------------:|---------------------:|--------------------:|----------------------:|
| smoke | 50                              | 200                   | 50                   | 500 (~2 reseed cycles) |
| full  | 2,000                           | 8,000                 | 2,000                | 5,000 (~19 reseed cycles) |

These tiers are deliberately far below `TIERS_Q2`/`TIERS_Q2_RANDOM`
(`common.py`) -- that scale assumes a compiled C binary; this is pure Python.
Tahap 0's job is only to cheaply prove/disprove the mechanism before
committing to a C port. Production-scale collision/PractRand validation is
Tahap 3's job, on the real C implementation.

## Results (full tier)

| candidate            | multiset collisions | stream collisions | min distinct L | avalanche mean | avalanche min-bit | avalanche max-bit |
|-----------------------|---------------------:|--------------------:|----------------:|-----------------:|--------------------:|--------------------:|
| Kandidat 1             | 0 / 2,000            | 0 / 2,000            | 256/256          | 0.4874           | 0.4730               | 0.5012               |
| Kandidat 2             | 0 / 2,000            | 0 / 2,000            | 256/256          | 0.4994           | 0.4874               | 0.5092               |
| **Kandidat 3 (control)** | **1,999 / 2,000**  | **1,744 / 2,000**    | 256/256          | 0.4874           | 0.4730               | 0.5012               |
| Kandidat 4             | 0 / 2,000            | 0 / 2,000            | 256/256          | 0.4874           | 0.4730               | 0.5012               |
| **Kandidat 5 (user)**  | 0 / 8,000             | 0 / 8,000             | 256/256          | 0.4989           | 0.4859               | 0.5116               |
| Kandidat 5, low-5-bit-shared-key stress (64 addresses) | 0 / 64 | 0 / 64 | 256/256 | -- | -- | -- |

Min-bit and max-bit both sit close to the 0.5 ideal for every candidate (tightest
band: Kandidat 1/3/4 at [0.4730, 0.5012], widest: Kandidat 5 at [0.4859,
0.5116]) -- no single seed bit is systematically weak *or* over-amplified,
consistent with a healthy diffusion property across the whole 32-bit key, not
just on average. Numbers regenerated via `python3 tahap0_prototype.py
avalanche` (fast path, `tahap0_avalanche.json` -- reruns only the 32-bit-flip
avalanche check, not the full distinctness sweep, since that part is
deterministic and already recorded above).

Full numbers: `tahap0_results_full.json` (also `tahap0_results_smoke.json` for
the smoke tier).

## Interpretation

**Core hypothesis holds at this scale.** Kandidat 1, 2, 4, and 5 all produced
zero multiset collisions and zero stream collisions across their full-tier
address sweeps, with every array staying fully rich (256/256 distinct values,
no collapse toward degenerate 0/1-heavy states) and avalanche well within
`operand_search.py`'s passing band (`[0.3, 0.7]` mean, `>= 0.2` min-bit floor
-- these candidates clear both comfortably, ~0.47-0.50 mean and ~0.47-0.49
min-bit).

**The negative control (Kandidat 3) validates the methodology, and does so
more sharply than expected.** At smoke tier (500 words, ~2 reseed cycles),
Kandidat 3 already showed the predicted multiset collision (49/50) but its
*stream* digest was still collision-free -- reordering alone still produces
locally distinct short runs. Only at full tier (5,000 words, ~19 reseed
cycles) did the stream digest also start colliding (1,744/2,000 pairs). This
is itself a finding worth keeping: **a short prefix is not enough to catch
long-run convergence** -- consistent with `quality_gate.py`'s documented
lesson about no fixed prefix size being safely candidate-agnostic. It means
Tahap 3's real validation must run enough reseed cycles (not just a prefix)
before trusting a "no collision" result, and that Tahap 0's own full-tier
result for Kandidat 1/2/4/5 (zero collisions after ~19 cycles, not just at
init) is a meaningfully stronger signal than the smoke-tier result alone.

**Kandidat 5's specific flagged risk did not materialize at this scale.**
HANDOVER.md's Lampiran noted that `rot32`'s `r &= 31` masking makes `L[i]`'s
effective rotation depend only on `seed`'s low 5 bits, so seeds sharing those
bits get an identical rotation-amount sequence. The dedicated stress test (64
seeds, all sharing the same low 5 bits, only varying in higher bits) still
produced zero multiset and zero stream collisions -- the seed-dependent
additive term (`0x9e3779b7 * seed`, which depends on all 32 bits) was enough
to keep `L` distinct even when the rotation pattern repeats. This is not
proof the risk is absent at larger scale (64 addresses is still small), just
that it's not the dominant failure mode at Tahap 0 scale.

## Open items for Tahap 1

- **`counter` role for Kandidat 5 is still an assumption, not a decision.**
  Tahap 0 tested Kandidat 5 as address = `seed` alone (per HANDOVER.md's
  Lampiran). If `counter` needs to be a real independent dimension, that
  needs to be folded in (e.g. `seed_effective = mix(seed, counter)`) and
  re-tested before Tahap 1 freezes the spec.
- **Kandidat 5's `multiplier`** was swept over 4 values
  (`0x9e3779b7`, its 2^32-complement, `0x2545F4914F6CDD1D`'s low 32 bits, and
  `3`) with no distinctness difference observed between them at this scale --
  Tahap 1 can pick any of these, or keep it configurable, without a
  distinctness tradeoff visible so far.
- No candidate needed Tahap 0b (the `operand_search.py`-style fallback
  search) -- all of Kandidat 1/2/4/5 passed cleanly, so it was not run.

## Fallback search (Tahap 0b): not needed

Not run -- Kandidat 1, 2, 4, and 5 all passed Tahap 0 at full-tier scale, so
the `operand_search.py`/`wiring.py`-style brute-force fallback (searching
init-formula operation configs for a non-degenerate winner) was not
triggered. If a future re-test at larger scale (or in Tahap 3's C
implementation) does find a failure, that fallback is still the planned next
step -- see HANDOVER.md's Tahap 0b note.

## Tahap 2: C Implementation

New file `winner_wired_addressable.c` (does not modify `winner_wired_v2.c`
or any `winner_wired*` variant in place). `ra_permutation_cycle`,
`ra_reseed`, `ra_core`, `rot32`, `ra_hash` are byte-for-byte copies of
`winner_wired_v2.c`'s; the only change is `ra_init_state` ->
`ra_init_state_addressable(L, M, key)` per the Tahap 1 frozen formula.
Compiled clean (`gcc -O3 -march=native -std=gnu17 -include stdalign.h`, no
warnings with `-Wall -Wextra`).

**Validated against the Tahap 0/1 Python reference** (`init_kandidat5()` +
`ra_core_from_state()` in `tahap0_prototype.py`, which was itself validated
bit-identical to the *original* `winner_wired_v2.c` before any candidate was
tested):

| key | n words | match |
|---|---|---|
| 1 | 3,000 | yes |
| 42 | 3,000 | yes |
| 12,345 | 3,000 | yes |
| 0 | 3,000 | yes |
| 4,294,967,295 (2^32-1) | 3,000 | yes |
| 999,999,937 | 3,000 | yes |

All 6 keys (including the 0 and 2^32-1 edge cases) produced byte-identical
output between the C binary's `--stream key n` and the Python reference.

**"Continue" mode identical to `winner_wired_v2.c`** (HANDOVER.md's Tahap 2
requirement): confirmed by prefix consistency -- `--stream 777 2000`'s
output is byte-identical to the first 2,000 words of `--stream 777 6000`
(spans multiple 255-word reseed cycles), i.e. generating more output never
re-initializes or otherwise perturbs the already-generated prefix. This
follows from `ra_permutation_cycle`/`ra_reseed`/`ra_core`'s loop structure
being an untouched copy of the original, but was verified empirically
rather than assumed.

**Status: Tahap 2 complete.** Next: Tahap 3 (full statistical re-validation
-- collision-scan in `(key, ...)` space, PractRand on cross-key output,
using the existing `experiments/2026-8-29_parallelization-research/`
infrastructure re-pointed at `winner_wired_addressable`).

## Tahap 3: Validasi Statistik

Full re-validation of `winner_wired_addressable` (Kandidat 5, frozen Tahap 1
formula) in `key`-space, reusing the existing Q1/Q2 infrastructure from
`experiments/2026-8-29_parallelization-research/` re-pointed at the new
binary (never modified in place -- see `tahap3_*.py`'s own docstrings for
the exact reuse technique per file). Same interpretation convention as that
prior validation: only `FAIL`/`SUSPICIOUS` PractRand tags are blocking;
isolated `unusual` tags that don't persist across checkpoints are routine
noise.

### Q1 Method A: cross-correlation (`tahap3_cross_correlation.py`)

Pearson cross-correlation (Bonferroni-corrected z-test, `analyze_group()`
reused unmodified) between pairs of independent `--stream <key> n` outputs,
adjacent keys (0..K-1) vs. control keys (K random keys sampled from the
full 2^32 space).

| tier  | K   | n words   | pairs | adjacent flagged | control flagged |
|-------|----:|----------:|------:|------------------:|------------------:|
| smoke | 8   | 200,000   | 28    | 0/28               | 0/28               |
| full  | 128 | 1,000,000 | 8,128 | 0/8,128            | 0/8,128            |

Zero flagged pairs at both tiers, adjacent and control alike -- no
detectable cross-key correlation.

### Q2: collision scan (`tahap3_collision_scan.py`)

Prefix (64-word digest) and blocksweep (interleaved-block digest) collision
scans, sequential keys (0..M-1) and random keys, via the shared blake2b
64-bit digest + birthday-bound methodology from Tahap 0/`collision_scan.py`.

| mode       | tier  | M (prefix / blocksweep) | prefix collisions | blocksweep collisions | max digest-collision-prob |
|------------|-------|--------------------------|--------------------:|--------------------------:|-----------------------------:|
| sequential | smoke | 100 / 100                 | 0                    | 0                          | ~4.12e-13                    |
| random     | smoke | 2,000 / 500               | 0                    | 0                          | ~1.03e-11                    |
| sequential | full  | 10,000 / 10,000           | 0                    | 0                          | ~2.60e-06                    |
| random     | full  | 500,000 / 25,000          | 0                    | 0                          | ~1.63e-05                    |

Zero collisions across all four scans, both sequential and random key
sampling, with negligible birthday-bound false-positive probability at
every scale (max ~1.63e-05) -- collisions, had any occurred, would have been
real, not digest noise.

### Q1 Method B: interleaved PractRand (`tahap3_interleave_practrand.py`)

K independent `--stream <key> n` outputs interleaved word-by-word and piped
into PractRand (`RNG_stdin32`), gated smoke -> medium -> full -> xlarge
(each tier only run if the previous was clean).

| tier   | K | total bytes | result | anomalies |
|--------|--:|-------------:|--------|-----------|
| smoke  | 4 | 64 MB         | PASSED | 1 isolated `unusual` at 4MB checkpoint (`[Low1/32]DC6-9x1Bytes-1`), resolved by 8MB+ |
| medium | 8 | 1,024 MB (1GB)| PASSED | 1 isolated `unusual` at the final 1GB checkpoint (`Gap-16:A`) |
| full   | 8 | 16,384 MB (16GB) | PASSED | same `Gap-16:A` unusual carried at the 1GB checkpoint (cumulative run); clean (`no anomalies`) at every checkpoint from 2GB through 16GB |
| xlarge | 8 | 131,072 MB (128GB) | PASSED | see below |

No `FAIL`/`SUSPICIOUS` at any tier or checkpoint. `full` (16GB) ran clean
end-to-end except the one carried-over `unusual` at the smallest (1GB)
checkpoint, consistent with the smoke/medium pattern of isolated,
non-persistent noise.

`xlarge` (128GB) was run as a stretch goal (per HANDOVER.md/plan: only
after `full` came back clean, matching the depth `winner_wired_v2`'s own
baseline was validated to). Result: **PASSED, fully clean** -- `no anomalies`
at every checkpoint from 8GB through 128GB (2^33 through 2^37 bytes), no
`unusual`/`FAIL`/`SUSPICIOUS` tags anywhere in this run. Total wall time
~3,467s (~58 min) for the cumulative 8GB->128GB portion.

### Status: Tahap 3 complete

All three methods (cross-correlation, collision-scan, interleaved
PractRand up to 128GB) passed with zero blocking anomalies, at both smoke
and full/xlarge tiers, in both sequential and random key sampling where
applicable. **Kandidat 5's addressable-init variant is now validated to the
same depth as the `winner_wired_v2` baseline it was derived from.**

## Tahap 4: Speed Benchmark vs Philox

New self-contained harness `tahap4_bench.c` (does not modify
`winner_wired_addressable.c` or any Philox file in `benchmarks/comparisons/`
in place). Addressable core (`rot32`, `ra_hash`, `ra_permutation_cycle`,
`ra_reseed`, `ra_init_state_addressable`, `ra_core`) is a byte-for-byte copy
of `winner_wired_addressable.c`'s, re-validated bit-identical against that
binary's `--stream` output for keys 1/42/999999937 at n=3000 before any
timing number was trusted.

**Philox correctness bug found and fixed (2026-08-30, prompted by explicit
user request to verify the repo's Philox against production Philox before
benchmarking against it).** All three existing Philox copies in the repo
(`benchmarks/comparisons/source/philox.c`, `philox_amortized.c`,
`benchmarks/comparisons/RNGing_speed/src/philox.c`) use
`PHILOX_M0 = 0xD256D193U`. Checked line-by-line against the official
Random123 reference (`DEShawResearch/random123`,
`include/Random123/philox.h`): that value is actually `PHILOX_M2x32_0` (the
**Philox2x32** variant's multiplier), not the correct `PHILOX_M4x32_0 =
0xD2511F53U`. The round-function structure, `PHILOX_M1`, `PHILOX_W0`,
`PHILOX_W1`, and the 10-round default are all otherwise byte-identical to
the reference -- only `M0` is wrong, likely a copy-paste from the wrong
variant. **User decision: the existing repo files are left untouched**
(out of scope for this research; changing them would invalidate their own
stored benchmark results) -- `tahap4_bench.c` uses the corrected
`0xD2511F53U` instead, so Tahap 4 benchmarks against real production
Philox4x32-10, not the repo's buggy variant. Verified at runtime (not just
by code review) against 3 official Random123 known-answer-test vectors
(`random123/tests/kat_vectors`, all-zero / all-`0xffffffff` / digits-of-pi
inputs) via `./tahap4_bench verify` -- **all 3 PASS**.

Method: `ra_core`'s reuse/continue mode vs Philox4x32-10 (stateless,
per-call), timed in-process with `clock_gettime(CLOCK_MONOTONIC, ...)`
around tight repeat loops (no subprocess-spawn overhead in the measured
region). **Each N/K value is measured over several trials and the MINIMUM
is reported** (standard microbenchmark practice -- scheduling/system noise
can only inflate a trial's time, never deflate it below the true cost),
200 trials for N/K<1,000 down to 1 trial for N/K>=10^7 where a single run
already takes long enough to be stable. An earlier single-shot-per-N draft
of this harness produced visibly noisy, non-monotonic small-N numbers
(re-running the identical binary gave N=64 addr_ns_per_word anywhere from
2.9 to 5.8) -- min-of-trials fixed this to clean, monotonic, reproducible
curves (see `tahap4_benchmark.py`). Full tier: `init-cost` 5,000,000
repeats; `throughput`/`reinit-sweep` swept densely around the observed
crossover (1-256 words) plus up to 10^8 words to confirm asymptotic
behavior. Orchestrated by `tahap4_benchmark.py`, raw data in
`tahap4_results_{init-cost,throughput,reinit-sweep}.json`.

### Test 1: pure init cost

| | ns per call |
|---|---:|
| `ra_init_state_addressable()` | 49.87 |
| `philox4x32_10()` (one block) | 10.06 |

**Ratio 4.96x** -- addressable's 256-iteration array fill costs about 5x
one Philox round-function call, as expected (Philox's per-call cost is
fixed/small by construction; addressable's "jump" cost is genuinely
separate from its per-word cost).

### Test 2: throughput sweep (single long run per N, min of up to 200 trials)

| N | addr ns/word | philox ns/word | addr faster? |
|---:|---:|---:|:---:|
| 1 | 58.00 | 29.02 | no |
| 8 | 9.37 | 4.88 | no |
| 32 | 3.84 | 3.37 | no |
| 48 | 3.23 | 3.21 | no (essentially tied) |
| 64 | 2.92 | 3.14 | **yes** |
| 128 | 2.45 | 3.05 | yes |
| 256 | 2.30 | 2.93 | yes |
| 65,536 | 2.27 | 2.90 | yes |
| 10,000,000 | 2.30 | 2.99 | yes |
| 100,000,000 | 2.37 | 3.00 | yes |

**Break-even N\* ~= 49** (linear interpolation in log-N space between the
N=48 and N=64 rows, where the ratio crosses 1.0). Asymptotic steady state
(N >= ~1,000): addressable converges to ~2.27-2.37 ns/word, Philox to
~2.90-3.00 ns/word -- **addressable is ~22% faster per word once past the
break-even point**, sustained out to 10^8 words.

### Test 3: empirical reinit-frequency sweep (repeated `init;generate K;discard` cycles, min of 3 trials)

Run as many independent cycles as fit a ~2*10^7-word budget per K (fresh
key each cycle, Philox counter/key also reset each cycle for symmetry),
measuring realized steady-state throughput rather than computing it from
a formula -- this is the test that answers "stop reusing after how many
generates" directly, and validates whether Test 2's crossover survives
real repeated-landing overhead. (Two harness bugs were caught and fixed
before trusting these numbers: a checksum-accumulator inconsistency
between Test 2/3's Philox loops that inflated Test 3's Philox baseline by
~25%, and the single-shot-per-K noise described above -- both fixed by the
current min-of-trials version.)

| K | cycles | addr ns/word (steady-state) | philox ns/word | ratio |
|---:|---:|---:|---:|---:|
| 1 | 200,000 | 49.85 | 12.04 | 4.14 |
| 16 | 200,000 | 4.85 | 2.89 | 1.68 |
| 32 | 200,000 | 3.70 | 2.86 | 1.30 |
| 48 | 200,000 | 3.23 | 2.84 | 1.14 |
| 64 | 200,000 | 2.94 | 2.84 | 1.03 |
| 80 | 200,000 | 2.79 | 2.89 | **0.96** |
| 128 | 156,250 | 2.57 | 2.82 | 0.91 |
| 256 | 78,125 | 2.39 | 2.88 | 0.83 |
| 4,096 | 4,882 | 2.31 | 2.83 | 0.82 |
| 65,536 | 305 | 2.30 | 2.87 | 0.80 |

**Empirical break-even K\* ~= 71** (interpolated between K=64 and K=80) --
consistent with Test 2's N* (~49), both in the same ~50-70 word
neighborhood, confirming the crossover from Test 2 is not an artifact of
the single-long-run measurement style. Asymptotic ratio ~0.80-0.85
(addressable ~15-20% faster steady-state even counting realistic
per-address landing overhead), slightly less favorable than Test 2's
~0.78 asymptote -- the small residual gap is the real cost of repeatedly
landing on fresh addresses (stack setup, cold state) that a single
uninterrupted run doesn't pay.

### Interpretation

Matches `HANDOVER.md`'s framing exactly: **not "who wins", but a concrete
break-even point.** For any workload generating **fewer than ~50-70 words
per address before moving to a new address, Philox4x32-10 wins** (its
per-call cost has no separate "landing" overhead). For **~70 words per
address or more, `winner_wired_addressable` in reuse/continue mode wins**,
and its advantage grows to a stable ~15-22% per-word speed edge for long
runs. This is a genuinely favorable, low break-even point for the niche
this research targeted (few long-lived addresses, e.g. parallel Monte
Carlo streams each run for a long time) -- reuse only needs to amortize
across roughly 50-70 outputs, not thousands, before it pays for itself.

### Status: Tahap 4 complete

All three tests run, Philox baseline verified against production
Random123 reference (bug found and worked around, not silently trusted --
see the Philox correctness note above), measurement noise brought under
control via min-of-trials (two harness bugs caught and fixed along the
way: the Philox checksum-loop inconsistency and single-shot small-N
noise). Break-even point (~49-71 words/address) is small and consistent
across both the analytical (Test 2) and empirical repeated-cycle (Test 3)
methodologies. Tahap 5 (optional init-cost optimization) is only relevant
if a workload's typical address lifetime is expected to fall below this
break-even point and that's judged worth optimizing for -- not pursued
here, pending explicit go-ahead.

## Tahap 5: Optimasi Biaya Init (implementasi, formula-preserving)

Prep session (2026-08-30, read-only `objdump` on the already-compiled
`tahap4_bench`) produced `HANDOVER_TAHAP5.md`'s ranked candidate list.
This section covers the execution session that followed, on the same
machine (Intel i3-1115G4, AVX-512F/VL, GCC 16.1.1).

New harness `tahap5_bench.c` (does not modify `tahap4_bench.c`,
`winner_wired_addressable.c`, or any other file in place). Forks
`tahap4_bench.c`'s structure; `ra_core()` gained an `init_fn_t` function
pointer parameter so multiple `ra_init_state_addressable` variants share
the exact same permutation/reseed loop and can be benchmarked through the
same modes (`init-cost`, `throughput`, `reinit-sweep`, `--stream`,
`verify`). Compiled three ways to isolate each Rank's variable cleanly:
`tahap5_bench` (default flags), `tahap5_bench_zmm512` (+
`-mprefer-vector-width=512`, Rank 1), `tahap5_bench_unroll` (+
`-funroll-loops`, Rank 3 flag-only). All three pass `verify` (3/3 Philox
KAT).

### Ruled out during planning: manual accumulator (strength reduction)

Before executing the ranked plan, a proposed rewrite of the init loop as a
running accumulator (`l += C` per iteration instead of `i*C`, `r` derived
incrementally) was tested. **Bit-identical to the frozen formula** (PASS,
1,000 keys) but **4.4-5.0x SLOWER** (~56ns -> ~255-300ns/init, `gcc -O3
-march=native`, 3 runs x 5,000,000 repeats). Cause, confirmed via
`objdump`: the accumulator introduces a loop-carried dependency chain
(`l`, `m`, `r` each depend on the prior iteration), which defeats GCC's
auto-vectorization entirely -- the loop falls back to a scalar
`rol edi,cl`-per-element loop instead of the AVX-512VL 8-lane vector loop
the direct (non-recurrence) formula gets. The rotate instruction itself
does improve (1 `rol` vs. the 3-instruction `sllv+srlv+or` pattern), but
losing 8-lane parallelism dominates by a wide margin. **Not implemented
anywhere in `tahap5_bench.c`** -- this is a documented negative result
only, so it isn't retried in a future session.

### Correctness gate (mandatory before trusting any speed number)

`v0_baseline` (exact copy, control) and `v1_rolv` (Rank 2, see below) both
compared byte-for-byte (`cmp`) against `winner_wired_addressable --stream`
for keys `0`, `0xFFFFFFFF`, 4 other scattered keys, and every key `0..31`
(covers every `rot32` amount residue) -- **38/38 PASS for both variants**.
The combined Rank 1 + Rank 2 variant (`v1_rolv` compiled with
`-mprefer-vector-width=512`) was also re-checked the same way -- **PASS**.
Per the mechanical gate in `HANDOVER_TAHAP5.md` section 6, bit-identical
output means these changes automatically inherit `winner_wired_addressable`'s
Tahap 3 statistical guarantees; Tahap 3 was not rerun.

### Measurement-noise lesson (methodological finding, not a candidate result)

The reinit-sweep microbenchmark's break-even point (K\*) is noisy enough
run-to-run that a **single** orchestrated run is not reliable evidence for
a small (<10%) effect: the exact same `v0_baseline` control code produced
K\* estimates of **71** (original Tahap 4 session), **90.5** (first Tahap 5
orchestrated run), and **111.7** (median-of-3 re-run, see below) across
three separate measurement sessions on the same machine -- a >50% spread
with zero code difference. `find_crossover()`'s simple pairwise
threshold-crossing is also fragile to a single noisy point in the sweep
(observed directly: one run's `zmm512` sweep had an anomalous uptick at
K=80 that shifted its reported K\* from ~57 to ~89, i.e. made a real win
look like no win at all). **Fix applied**: rerun `reinit-sweep` 3x per
candidate and take the median `addr_ns_per_word_steadystate`/
`philox_ns_per_word` at each K before computing the crossover
(`robust_reinit.py`, not part of the standard `tahap5_benchmark.py` run --
kept as a follow-up script in this folder). All K\* numbers below are the
median-of-3 estimate unless stated otherwise. **Absolute K\*/N\* values
should be read as "same order of magnitude", not exact** -- the
*within-session, same-run* comparison between a variant and its own
control is far more trustworthy than any single absolute number, since
both are measured under the same system-noise conditions.

### Rank 2: explicit AVX-512VL rotate (`v1_rolv`) -- ADOPTED

`_mm256_rolv_epi32`/8-lane `__m256i` intrinsics replace the compiler's
auto-vectorized `sllv+srlv+or` pattern for `rot32`, same vector width GCC
itself chose (isolates the rotate-instruction change alone). Algebra
(`l_val = C*(i+key)`) matches the compiler's own factorization -- no
change there, per `HANDOVER_TAHAP5.md` section 3's finding that no
factorization win remains.

| metric | v0_baseline (control, same run) | v1_rolv | change |
|---|---:|---:|---:|
| init-cost (ns/call, 5x interleaved runs) | ~54-58 | ~35-40 | ~30-35% lower |
| N\* (throughput break-even) | ~50.5 | ~26.1 | ~48% lower |
| K\* (median-of-3 reinit-sweep break-even) | 111.7 | 57.2 | ~49% lower |

Wins across every metric, in the same run (controls for session-level
noise). This is the strongest, most consistent candidate and does not
depend on any non-default compile flag.

### Rank 1: `-mprefer-vector-width=512` flag only, `v0_baseline` code

No code change -- same formula, same auto-vectorized rot32 pattern, just
GCC choosing 16-lane ZMM instead of 8-lane YMM.

| metric | default v0_baseline | zmm512 v0_baseline | change |
|---|---:|---:|---:|
| init-cost (ns/call) | ~54-58 | ~38-44 | ~25-30% lower |
| N\* | ~50.5 | ~29.8 | ~41% lower |
| K\* (median-of-3) | 111.7 | 55.9 | ~50% lower |

**Comparable magnitude to Rank 2, from a compile flag alone.** The first
(non-median) orchestrated run had suggested K\* barely moved for this
candidate (~89 vs ~90.5) -- that turned out to be exactly the
measurement-noise artifact described above (one anomalous sweep point),
not a real downclocking effect; the median-of-3 re-run and a focused
5-round re-check of the K=48-128 crossover region both confirm a real,
consistent win at every K point. **Not adopted as the primary winner**
here because it has no effect on `v1_rolv`'s explicit `__m256i` code
(confirmed empirically -- see next section) and depends on a compile flag
rather than being self-contained in the source, but it is a legitimate
zero-code-change alternative if a flag change is preferred to a source
change.

### Combining Rank 1 + Rank 2 (`v1_rolv` compiled with `-mprefer-vector-width=512`)

Tested out of curiosity after seeing Rank 1 and Rank 2 each win
independently. **No additional benefit over `v1_rolv` alone**
(init-cost ~35-40ns, N\*~25-27 -- statistically indistinguishable from
Rank 2's own numbers above). Expected in hindsight: `v1_rolv` is written
with explicit `__m256i` (256-bit) intrinsics, which are a fixed width
regardless of `-mprefer-vector-width` -- that flag only changes the width
GCC's *auto-vectorizer* chooses for code it vectorizes itself (like
`v0_baseline`), and has no effect on hand-written intrinsic code. A
genuine further win would require rewriting with `__m512i`/
`_mm512_rolv_epi32` (16 lanes/iteration) -- untested, flagged as a
possible Tahap 6 follow-up, not pursued here (would edge toward the
deprioritized Rank 4 territory of a fuller manual SIMD rewrite).

### Rank 3: `-funroll-loops` flag only, `v0_baseline` code

| metric | default v0_baseline | unroll v0_baseline | change |
|---|---:|---:|---:|
| init-cost (ns/call) | ~54-58 | ~52-57 | negligible (~2-5%) |
| N\* | ~50.5 | ~39.8 | ~21% lower |
| K\* (median-of-3) | 111.7 | 74.0 | ~34% lower |

A real but smaller win than Rank 1/2, concentrated in the throughput/
reinit-sweep numbers rather than isolated init-cost -- consistent with
the flag's mechanism (breaking the loop-carried dependency on the single
`i` accumulator matters more once other code interacts with it than in a
tight isolated repeat loop). Not adopted as the primary winner given
Rank 2's larger and more isolated-init-cost-visible win, but a legitimate
additional flag if squeezing out more sustained-throughput gain is worth
it independent of source changes. Manual 2-chunk unroll (the fallback
described in `HANDOVER_TAHAP5.md` section 4 Rank 3, for if the flag alone
wasn't enough) was **not needed** -- the flag alone already gives a
meaningful K\* reduction.

### Adoption decision

**`winner_wired_addressable_v2.c`** (new file, does not modify
`winner_wired_addressable.c`) adopts **Rank 2** (`v1_rolv`'s explicit
`_mm256_rolv_epi32` intrinsics) as `ra_init_state_addressable()`. Every
other function (`rot32`, `ra_hash`, `ra_permutation_cycle`, `ra_reseed`,
`ra_core`, `main`) is a byte-for-byte copy of `winner_wired_addressable.c`.
Re-verified bit-identical against `winner_wired_addressable --stream` on
the final promoted file itself (not just the `tahap5_bench.c` experimental
copy) -- same 38-key set, 38/38 PASS. Rationale for choosing Rank 2 over
Rank 1 despite similar magnitude: self-contained in source (works under
plain `-march=native`, no dependency on a non-default compile flag someone
building this file later might omit), and it is the more mechanically
understood change (a specific missing hardware instruction, not a
compiler heuristic that could shift across GCC versions).

**Updated break-even estimates**: N\* ~49 -> **~26**, K\* ~71 ->
**~57** (median-of-3; read both old and new numbers as order-of-magnitude
given the measurement-noise finding above, not exact). The addressable
design now wins over production Philox4x32-10 at roughly **half the
address-lifetime threshold** found in Tahap 4, without any change to the
validated formula or its statistical guarantees.

### Status: Tahap 5 complete

All three ranked candidates (Rank 1/2/3) implemented, bit-identical
verified where code changed, and benchmarked with a noise-robust
(median-of-3) methodology after an initial single-run measurement was
caught giving a misleading result for Rank 1. Rank 2 adopted into
`winner_wired_addressable_v2.c`. Rank 4 (full manual SIMD rewrite) and a
512-bit-native version of Rank 2 remain untried, flagged as possible
future work, not pursued here per the original ranking's own guidance to
exhaust cheaper options first.

### Decision: baseline for future experiments stays `winner_wired_addressable.c`, NOT `_v2.c`

User decision (2026-08-30, after Tahap 5 closed): `_mm256_rolv_epi32` is an
AVX-512VL intrinsic -- x86-only, and only on CPUs that implement AVX-512VL
specifically (not all x86 does). It has no meaning on GPU (different
programming/execution model entirely), Android/ARM (no AVX at all), or any
non-x86 target. Baking `_v2.c` in as the default thing future experiments
build on top of would silently carry a non-portable assumption into work
that may target a different architecture. **`winner_wired_addressable.c`
(the Tahap 2-4 formula, no architecture-specific SIMD) is the standing
baseline for whatever comes next**, not `_v2.c`. `_v2.c` is not deprecated
or wrong -- it stays as a validated, opt-in fast path specifically for
x86/AVX-512VL targets, selected deliberately when that's confirmed to be
the deployment target, not picked by default.
