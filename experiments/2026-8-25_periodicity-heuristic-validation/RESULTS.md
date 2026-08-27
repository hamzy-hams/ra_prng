# Results: validating the ICCS2026 paper's √|S| periodicity heuristic

Companion to `HANDOVER.md` (read that first for full context). This
experiment shrinks `ra_prng2` to a new, separate `(n, w)`-parameterized toy
generator (`toy_prng.py`) small enough to measure real cycle lengths, and
checks whether they track the paper's `λ ≈ 0.7824·√|S|` prediction.

## Design decisions (documented, not silently picked)

The handover named two scaling traps and asked that the fixes be made
explicit; implementing the toy model surfaced **two more**, found by
running the code and observing degenerate output, not by inspection alone.
All four:

1. **Shift amounts** (`<<9`, `>>18`, `>>13`, `<<14` at `w=32`): rescaled as
   `s_w = round(s·w/32)`, clamped to `[1, w-1]`. Computed values:
   `w=8 → (S9,S18,S13,S14) = (2,4,3,4)`; `w=4 → (1,2,2,2)`.
2. **Golden-ratio-derived init constants** (`0x06a0dd9b` for `M`,
   `0x9e3779b7` for `L`): top-`w`-bits truncation. **Trap found**: at
   `w=4`, `0x06a0dd9b`'s top 4 bits are exactly `0x0` - naive truncation
   zeroes the entire `M` array at init. Fixed by forcing nonzero:
   `c_w = c_w or 1`. Computed values: `w=8 → c_m=0x06, c_l=0x9e`;
   `w=4 → c_m=0x1, c_l=0x9`.
3. **The `ra_hash`/`o`-mixing-loop "8" and "32" structural constants**
   (tied to the original's `256 = 8×32` factorization, not addressed by the
   handover at all): generalized as a group size `G(n)`, with
   `rows = n/G` inner reduction terms. **Two further traps found by
   running the code**:
   - `G = min(n, w)` → `rows = 1` for 4 of 5 configs. `ra_hash`'s first
     reduction term is always a self-XOR (`N[i] ^= N[i] = 0` when `j=0`) -
     harmless in the original because 31 more terms follow it, but with
     `rows=1` there is nothing after it: `M` collapsed to all-zero from
     step 0 onward (confirmed empirically).
   - `G = n/2` → `rows = 2`: the one surviving term after the self-cancel
     is a raw copy, not a mix - confirmed `M[i] == M[i + n/2]` identically
     at every step, an artificial correlation absent from the real
     algorithm.
   - **Fix used**: `G = max(1, n/4)`, giving `rows = 4` for every `n ≥ 4`
     in the sweep (3 real terms after the self-cancel - no raw-copy
     pathology) and `rows = 2` for `n = 2` (unavoidable; the handover
     already flags `n=2` as inherently "very degenerate").
4. **Lemire reduction** (`d = (c·(i+1)) >> 32`): generalizes with no
   change in shape, `d = (c·(i+1)) >> w`. No trap - matches the handover's
   claim.

`toy_prng.py`'s docstrings carry the same rationale inline, in particular
`group_size()`.

## Avalanche sanity check (handover step 2 gate)

`avalanche_check.py`: single outer iteration, no rehash, flip each seed
bit, measure mean Hamming distance across the captured inner-loop `c`
sequence vs. the unflipped baseline. Target: convergence toward ~50% of
`w` bits differing.

| n | w | mean Hamming distance | fraction of w |
|---|---|---|---|
| 2 | 8 | 2.88 / 8 | 36% |
| 4 | 8 | 3.71 / 8 | 46% |
| 2 | 4 | 2.00 / 4 | 50% |
| 4 | 4 | 2.33 / 4 | 58% |
| 8 | 4 | 2.04 / 4 | 51% |

All five configs land within a reasonable band around 50%. **Gate passed**
- proceeded to cycle measurement.

## Cycle length measurements

Brent's algorithm (`cycle_measure.py`), run over independent seeds per
config - Python for the four small/fast configs, the C port
(`cycle_measure.c`, cross-validated bit-for-bit against `toy_prng.py` on
`n=4,w=8` before use - see the rounding-tie note below) for `(8,4)`, which
was too slow to reach its predicted scale in pure Python (measured
throughput ≈92k steps/s; at `λ_pred≈1.6e8` that's hours per seed in
Python vs. ≈3.3s/seed in C). Actual seed counts used (not the full
100-1000 the handover suggested "budget permitting" - see the timing note
below):

| n | w | log2\|S\| | λ_pred | seeds | λ mean | λ median | λ min | λ max | mean/pred |
|---|---|---|---|---|---|---|---|---|---|
| 2 | 8 | 33.0 | 7.251e4 | 500 | 5.550e2 | 7.680e2 | 256 | 768 | 0.0077 |
| 4 | 8 | 52.6 | 6.431e7 | 100 | 1.542e5 | 2.004e5 | 4352 | 200448 | 0.0024 |
| 2 | 4 | 17.0 | 2.833e2 | 500 | 1.600e1 | 1.600e1 | 16 | 16 | 0.0565 |
| 4 | 4 | 28.6 | 1.570e4 | 500 | 8.284e4 | 1.073e5 | 15232 | 107328 | **5.2764** |
| 8 | 4 | 55.3 | 1.647e8 | 100 | 6.319e6 | 6.319e6 | 6318640 | 6318640 | 0.0384 |

Infeasible, analytic-only (per the handover, not attempted):
`n=8,w=8` (`log2|S|=95.3`, `λ_pred≈1.727e14`), `n=16,w=4`
(`log2|S|=116.3`, `λ_pred≈2.459e17`).

**A striking pattern independent of the λ/λ_pred ratio**: across every
config, the number of *distinct* λ values seen across hundreds of
independent seeds is tiny - the state space's functional graph collapses
onto a handful of giant cycles that almost every trajectory falls into,
not a spread of many different cycle lengths:

| n | w | seeds | distinct λ values | λ (count) |
|---|---|---|---|---|
| 2 | 8 | 500 | 2 | 768 (292), 256 (208) |
| 4 | 8 | 100 | 5 | 200448 (65), 33536 (17), 137984 (13), 5632 (4), 4352 (1) |
| 2 | 4 | 500 | **1** | 16 (500/500) |
| 4 | 4 | 500 | 3 | 107328 (313), 15232 (94), 68752 (93) |
| 8 | 4 | 100 | **1** | 6318640 (100/100) |

`μ` (tail length before entering the cycle) is likewise informative: it's
tiny (mean 1-2 steps) for every config except `(8,4)`, where the mean tail
(≈4.48e6) is comparable in magnitude to the cycle itself (6.32e6) - seeds
there spend a long time in transient states before locking onto the one
dominant cycle.

**Timing note**: `n=4,w=4` (994.9s for 500 seeds) and `n=8,w=4` (345.8s
for 100 seeds via C) both ran considerably slower than the handover's
"trivial" / "seconds-to-minutes" estimates - `n=4,w=4`'s measured λ turned
out ~5x *larger* than predicted (see below), not smaller, which is what
made it slow in Python despite its tiny nominal state space. `n=4,w=8`
(435.5s for 100 seeds) was in line with the handover's "low minutes"
estimate. All timings are on this session's hardware, single-threaded,
`-O3 -march=native` for the C port.

**A rounding-tie bug caught during implementation, not by the handover**:
the shift-rescale formula `s_w = round(s·w/32)` hits an exact `.5` tie for
`(s=18, w=8)`. Python's `round()` is round-half-to-even (→4); a first C
implementation used round-half-up (→5), silently producing a *different*
algorithm for every `w=8` config until caught by cross-validating the C
port's `(λ, μ)` output against Python's on `n=4,w=8` before trusting any
C-derived numbers (both now agree bit-for-bit). Anyone re-deriving this in
another language should match round-half-to-even specifically at this tie,
not "the obvious" rounding.

## Verdict

**The heuristic does not hold cleanly at this measurable toy scale, and the
measured trend does not even track `√|S|` monotonically across the sweep -
consistent with the handover's own framing of this as the paper's
weakest-supported claim, though the specific failure mode found here is
more informative than a simple "off by a constant factor".**

- **Magnitude**: measured mean `λ` is off from `λ_pred` by roughly
  20x-400x *smaller* in four of five configs, but **~5.3x *larger*** in
  `n=4,w=4` - the direction of the error isn't even consistent, let alone
  the size.
- **Trend**: ordering the configs by `log2|S|` (17.0, 28.6, 33.0, 52.6,
  55.3) against measured `log2(mean λ)` (4.0, 16.3, 9.1, 17.2, 22.6) is
  **not monotonic** - `n=2,w=8` (`log2|S|=33.0`) measured a *smaller*
  cycle than `n=4,w=4` (`log2|S|=28.6`, smaller state space). If the
  `√|S|` scaling held even approximately, larger `|S|` should not produce
  a smaller typical cycle. `n` and `w` independently appear to matter more
  than the combined `|S|` figure the heuristic reduces everything to.
- **Partial qualitative support**: the *shape* of a random-mapping
  functional graph - a small number of giant, dominant cycles that nearly
  all trajectories converge into, rather than a wide spread of cycle
  lengths - did show up clearly (1-5 distinct λ values across 100-500
  seeds per config, see table above). That part of the random-mapping
  analogy the paper leans on is qualitatively visible even here.
- **Confound worth flagging explicitly**: this toy model's `ra_hash`
  reduction is necessarily far shallower than the original's (`rows=4` or
  `rows=2` here vs. `rows=32` in the real algorithm - design decision 3
  above), and `n=2` configs are forced to `G=1` (the thinnest possible
  reduction). It is plausible that this shallow-hash artifact, not a
  genuine property of the full-scale algorithm's mixing, is what's
  driving the non-monotonic trend and the giant-cycle collapse - the toy
  model may be *structurally weaker* at diffusing/reseeding than
  `ra_prng2` really is at `n=256`, which would bias measured `λ` down
  (and toward fewer distinct values) independent of whether the `√|S|`
  heuristic itself is right or wrong for the real algorithm.

**Bottom line**: this experiment does not confirm the paper's
`0.7824·√|S|` heuristic at measurable scale, but it also cannot cleanly
refute it for the real `n=256` algorithm - the toy model's necessary
structural simplifications (particularly the shallow `ra_hash` reduction)
are a plausible confound distinct from the heuristic's own validity. A
follow-up that varies the `ra_hash` depth (`rows`) independently of `(n,w)`
- to see whether the trend improves as `rows` grows back toward the
original's 32 - would separate "the heuristic is wrong" from "this toy
model's hash is too shallow to be a fair test," and is the natural next
step this experiment sets up but does not itself answer.
