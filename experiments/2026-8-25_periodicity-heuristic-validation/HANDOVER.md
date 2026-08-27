# Handover: validate the ICCS2026 paper's √|S| periodicity heuristic

Written for a fresh Claude Code session picking this up cold. Read this whole
file before touching anything - it front-loads the design trap that will
otherwise waste a session.

## Context

`ra_prng` (this repo) is a solo-researcher project: an array-based
deterministic PRNG (`ra_prng2`, 32-bit, stable; `ra_prng3`, 64-bit,
experimental). Published at CSAI2025 (oral presentation); a more rigorous
follow-up paper targeting ICCS2026 was just added at
`research/iccs2026_ra_prng/iccs_ra_prng.pdf`.

The repo just went through an unrelated packaging/build refactor (see
`CHANGELOG.md`, `docs/STRUCTURE.md`) - that work's scope restrictions
("no new PRNG logic/tests") do **not** apply here. This task is explicitly
about writing new experimental code.

Reference implementations (do not modify these - the toy model below must be
a separate, new implementation):
- `src/ra_prng2/c/ra_prng2_struct.c` - clean C API (`ra_prng_init`/`next`/`advance`).
- `src/ra_prng2/python/ra_prng2.py` - pure-Python reference (`ra_core`, `ra_hash`, `rot32`).
- `docs/api/ra_prng2.md` - API summary.

## The claim being tested

Section III-C of `iccs_ra_prng.pdf` derives a **heuristic, not proven**
estimate of `ra_prng2`'s period. Transcribed here so you don't have to
re-read the whole PDF:

- State: two 256-word arrays `L`, `M` (32-bit words each), plus scalars
  `cons` and `it` (32-bit each).
- They split the state into two contribution classes:
  - **Fully mutable** bits (each bit can change via XOR/add/rotate): `M`
    (256×32 bits) + `cons` (32 bits) + `it` (32 bits) → `Bmut = 256·32 + 32 + 32 = 8256`.
  - **Permutation-only** contribution: `L` is only ever rearranged (swapped),
    never bitwise-mutated, so its combinatorial contribution is `256!`, i.e.
    `log2(256!) ≈ 1684.05` (Stirling), **not** `256·32 = 8192` bits. This is
    the paper's sharpest move - don't lose it when generalizing below.
- Raw upper bound on distinct states: `|S| ≈ 2^Bmut × 256! ≈ 2^9940`.
- Because reseeding (`cons_{t+1} = H(M_t)`, a many-to-one hash-like
  reduction) makes the overall update non-invertible, they model long-term
  dynamics as a **random mapping** and apply the standard heuristic for
  expected cycle length of a random function on a set of size `|S|`:
  `λ ≈ 0.7824·√|S|`.
- Result: `λ ≈ 2^4969.65` for `ra_prng2`, `2^16447.65` for the (proposed)
  `ra_prng3` scaling.

**This is the paper's weakest-supported claim.** The `√|S|` heuristic
assumes `F` behaves like a *uniformly random* function on the state space.
`F` is actually a specific, structured, deterministic bitwise function -
the analogy is plausible but **never checked empirically** anywhere in the
paper. That's the gap this experiment fills: shrink the state small enough
to actually measure real cycle lengths, and see whether they track the
`0.7824·√|S|` prediction.

## The trap: shrinking array length alone is NOT enough

The obvious first idea - "just use `n` array elements instead of 256, keep
32-bit words" - does not make this tractable. Measuring a cycle costs
`O(λ)` steps (Floyd's/Brent's algorithm walks the actual trajectory; it does
not need to enumerate the state space, but it does need to *reach* the
cycle). With `Bmut(n) = 32n + 64` and `log2|S(n)| = Bmut(n) + log2(n!)`:

| n (32-bit words) | log2\|S\| | λ_pred = 0.7824·√\|S\| |
|---|---|---|
| 4 | ≈196.6 | ≈2^98 (~3·10^29) - already infeasible |
| 2 | ≈129.0 | ≈2^64 (~2·10^19) - still infeasible |

Even `n=2` with full 32-bit words is untouchable. **You must also shrink the
word width `w`** (bits per array element), not just the array length `n`.
Generalized (all figures below double-checked numerically, not by hand):

```
Bmut(n, w) = w·n + w + w = w·(n + 2)        # M array + cons + it
log2|S(n, w)| = Bmut(n, w) + log2(n!)
λ_pred(n, w) = 0.7824 · 2^( log2|S(n, w)| / 2 )
```

Worked examples:

| n | w | log2\|S\| | λ_pred | feasible to fully measure? |
|---|---|---|---|---|
| 2 | 8 | 33.0 | ≈7.3×10^4 | yes, trivial |
| 4 | 8 | 52.6 | ≈6.4×10^7 | yes, seconds in C / low minutes in Python |
| 8 | 8 | 95.3 | ≈1.7×10^14 | no |
| 2 | 4 | 17.0 | ≈2.8×10^2 | yes, trivial (but very degenerate - see below) |
| 4 | 4 | 28.6 | ≈1.6×10^4 | yes, trivial |
| 8 | 4 | 55.3 | ≈1.6×10^8 | yes, seconds-to-minutes in C |
| 16 | 4 | 116.3 | ≈2.5×10^17 | no |

So a reasonable sweep is `w=8` for `n ∈ {2, 4}` and `w=4` for
`n ∈ {2, 4, 8}` - that covers power-of-two `n` from 2 to 8 while staying
inside a measurable `λ`. Getting `n=16` into range needs `w ≤ 2`, which is
almost certainly too narrow for the algorithm's mixing structure to survive
(see next section) - treat `n=16` as likely out of reach for direct
measurement, and say so plainly in the results rather than forcing it.

**Interpretation of "array size kelipatan 2"**: read as *power-of-two array
lengths* (`n = 2, 4, 8, 16, ...`), which is the natural reading given the
context (shrinking down from 256 = 2^8). If that's not what was meant,
confirm with the user before generalizing the sweep.

## Second trap: the fixed shift amounts don't scale for free

The core loop (see `docs/api/ra_prng2.md` / `ra_prng2_struct.c`) uses
specific *plain* shifts (not rotations) tuned for 32-bit words, e.g.
`<<9`, `>>18`, `>>13`, `<<14`. These are not rotate amounts (`rot32` already
handles wraparound separately) - they're lossy shifts that intentionally
drop bits off one end. Reusing the literal integers `9/18/13/14` at `w=8`
or smaller either saturates (shifts ≥ `w` zero out the whole term,
silently deleting part of the mixing function) or changes the *relative*
proportion of state that gets shifted out, which changes the algorithm's
qualitative behavior, not just its scale.

Proposed fix (a design decision to make explicit and document in the
results, not something to silently pick): rescale each shift amount `s`
proportionally, `s_w = round(s · w / 32)`, and sanity-check none collapse
to 0 or ≥ `w` in a way that deletes a whole mixing term. Same treatment
needed for the two golden-ratio-derived init constants
(`0x06a0dd9b`, `0x9e3779b7`) - use the top `w` bits of the same constants
(standard technique; e.g. the 8-bit "golden ratio" byte is `0x9e`), not an
arbitrary re-roll.

Lemire's fast-reduction step generalizes cleanly with no design decision
needed: `d = floor((c_w · (i+1)) / 2^w)` using a `2w`-bit intermediate
product, directly analogous to the paper's `64→32` reduction.

## Suggested first steps

1. **Pilot run before the real sweep.** Implement the toy generator in
   Python (fast to iterate on correctness; only port to C if a chosen
   `(n, w)` is too slow in Python) parameterized by `(n, w)`. Start with
   `n=4, w=8` (λ_pred ≈ 6.4×10^7, cheap enough to fully measure in minutes)
   purely to validate the toy design isn't degenerate.
2. **Sanity-check before trusting cycle numbers.** Before measuring periods,
   rerun a scaled-down version of the paper's own avalanche test (Section
   III-A2 of the PDF: bit-flip / incremental / power-of-two seed
   perturbation, measure Hamming distance convergence) against the toy
   model. If it doesn't show the same rapid convergence toward ~50% bit
   difference that the full-size algorithm does, the toy model's shift/
   constant rescaling is wrong - fix that before spending compute on cycle
   measurement.
3. **Measure real periods.** Use Brent's cycle-detection algorithm (fewer
   state comparisons than Floyd's) starting from many independent seeds
   (aim for 100-1000, budget permitting) per `(n, w)` config. Record `λ`
   (and `μ`, the tail/pre-period length) per seed - don't just take one
   sample, the non-invertible reseed means different seeds can land on
   different orbits.
4. **Compare trend, not just the constant.** Report, per `(n, w)`:
   `log2|S|` (predicted), `λ_pred`, and measured `λ` (mean/median/min/max
   across seeds). The more informative check is whether measured `λ` scales
   correctly as `(n, w)` change (i.e. tracks `√|S|` growth), not just whether
   the `0.7824` constant matches exactly for one config.
5. Write up as `experiments/2026-8-25_periodicity-heuristic-validation/RESULTS.md`
   with the comparison table and a short verdict (heuristic holds / doesn't
   / holds only in some regime).

## Non-goals

- Don't modify `src/ra_prng2/c/ra_prng2_struct.c` or the Python reference -
  build a new, separate toy implementation in this experiment directory.
- Not trying to prove an exact period for the real 256-word, 32-bit
  algorithm (paper already says that's computationally infeasible) - only
  testing whether the *heuristic reasoning* holds at a scale where ground
  truth is checkable.
- Not a cryptographic security evaluation.
