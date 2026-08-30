# Handover: mekanisme update state `L`/`M` (swap vs. maximal bit-change)

Written for a fresh Claude Code session (or the user) picking this up cold
in a later session. This is a **spec for a not-yet-started research task**
— nothing has been implemented yet, no candidate C files, no toy-model
extension, no search/measurement code exists here. Only this document.

## Context: why this idea exists

Three prior experiments have all touched `ra_permutation_cycle` from a
different angle, but none has touched **how the `L`/`M` state arrays
themselves are updated**:

- `2026-8-25_periodicity-heuristic-validation/` — built a scaled-down toy
  PRNG (`toy_prng.py`, parameterized by array length `n` and word width
  `w`) to make cycle-length measurement (Brent's algorithm) tractable,
  since the real 256×32-bit state space is computationally infeasible to
  cycle-close directly. The toy model's `L[i], L[d] = L[d], L[i]` swap
  (`toy_prng.py:189`) was kept **fixed** throughout — never varied. Key
  conclusions (`RESULTS.md:130-176`): the `0.7824√|S|` cycle-length
  heuristic doesn't hold cleanly or monotonically; internal hash-fold
  depth (`rows`) dominates over `|S|` as a λ predictor; functional graphs
  collapse onto a tiny number of giant dominant cycles.
- `2026-8-26_operation-pruning-research/` — greedy ablation search over 18
  discrete on/off operation flags (`operations.py`) in
  `ra_permutation_cycle`/`ra_hash`. **The `L`/`M` swap was never one of
  the ablatable flags** — it's part of the fixed skeleton in both
  `pruned_prng.py:138` and `pruned_prng.c:221`. The only adjacent toggle,
  `MULT_REDUCE` (`operations.py:47-52`), only changes how the swap
  *index* `d` is derived (full Lemire multiply-reduce vs. `d = c & 0xFF`)
  — the swap operation itself always executes either way, never disabled.
- `2026-8-27_operand-position-search/` — exhaustive search over which
  variable feeds which slot in the `a`/`b`/`c` update chain (operand
  wiring), fixed op set. Found and fixed a real defect (seed bits 5/6
  nearly avalanche-dead in the pruning experiment's winner) purely by
  rewiring, without touching `L`/`M` at all. Also found the search's own
  small PractRand gate (2MB) let 3 of 13 "winners" through that failed
  hard at 16GB (`v01`/`v02`/`v09` — see that folder's `RESULTS.md`
  "Survey of the other 12 winners" section) — a concrete, repo-local
  lesson this experiment must not repeat.

**So the `L[i]/L[d]` swap itself has never been varied, ablated, or
independently measured for periodicity impact anywhere in this repo — it
is genuinely new territory.** The user's question: is this swap
statistically *necessary*, and can periodicity be pushed higher with a
different, cleaner state-update mechanism?

### The two reference mechanisms already in this codebase

**`ra_prng2` (the shipped, paper-exact algorithm)** — pure permutation,
zero bit mutation of `L`:

```c
// src/ra_prng2/c/ra_prng2.c:75-77 (ra_permutation_cycle, inner loop)
uint32_t tmp = L[i];
L[i] = L[d];
L[d] = tmp;
```

`L` is initialized once (`ra_init_state`, `L[i] = i*0x9e3779b7 +
0x9e3779b7`) and from then on **only ever permuted**, never XORed/added/
overwritten with new bits. Its contribution to state-space size is purely
combinatorial: `log2(256!) ≈ 1684` bits (vs. `M`'s `256*32 = 8192` fully-
mutable bits). Crucially, the swap happens **after** `c` (the emitted
stream value) is computed each step — `c` does not depend on the
post-swap `L[i]`/`L[d]` values at all. `L`'s only causal effect on future
output is indirect and delayed: at the end of the 255-step cycle,
whatever permuted arrangement `L` has reached gets folded into `M` via
`M[i] ^= L[i]` (`ra_prng2.c:87-89`, inside `ra_reseed`), which then feeds
`ra_hash` → the next `cons`. So today's swap is a **pure side-effect
generator that accumulates permutation-state entropy for the reseed
step** — not an in-loop shuffle that's read back mid-cycle.

`M` itself is even simpler: read every step to build the tap value `o`,
and mutated **once per outer iteration** (255 inner steps), via that same
`M[i] ^= L[i]` fold, then hashed by `ra_hash` (`ra_prng2.c:21-31`).

(`ra_prng2_struct.c`/`ra_prng2_thread.c` and the Python reference
`src/ra_prng2/python/ra_prng2.py:57-62` all match this exactly — no
variant changes swap semantics.)

**`ra_prng3` (experimental, `src/ra_prng3/c/ra_prng3.c`, "do not use" per
README.md and docs/STRUCTURE.md)** — a **64-bit, larger-state, unrelated
in-development algorithm**, not a drop-in variant of `ra_prng2`. Its
analogous state-churn step (`ZepFold`, lines 57-78) does something
structurally different from a swap:

```c
// src/ra_prng3/c/ra_prng3.c:69-70 (inside ZepFold's inner loop)
L[i] = L[e];
L[e] = d;
```

This is a **one-directional overwrite**, not a swap: slot `i` gets the
*old* value that was sitting in slot `e`, and slot `e` gets clobbered with
`d` — a freshly computed, fully-mixed value (derived through `ZepFold`'s
own rotate/XOR/add chain, analogous to `ra_prng2`'s `c`). This breaks
bijectivity (some old state can be permanently lost, some slots can be
hit by injection more than once per pass while others are hit zero times)
but injects fresh full-width entropy directly into `L` every step, not
just relocates existing entropy. `ra_prng3` also reseeds via a more
aggressive full-state hash (`ZepXORhash`, lines 17-28: each `M[i]` XORed
with a self-indexed `M[M[i]]`-style lookup plus a full column-XOR over
32 rows × 8) rather than `ra_prng2`'s simpler sequential 32-deep fold.

**Use `ra_prng3` only as a source of design ideas for the "injection"
pole of the mechanism spectrum below — it is not a dependency, not a
target to fix or validate, and nothing here should make it any less
"experimental, do not use."**

## Scope decisions already made (do not re-litigate these)

The user was asked and explicitly decided the following — treat as fixed
constraints, not open questions:

1. **Breadth of variants: a broad spectrum, not just two pure poles.**
   Build and compare more than just "permutation-only" vs. "full
   injection" — see "The idea" below for a starting catalog (not
   exhaustive; narrow or extend it as evidence comes in, but don't
   collapse back down to just 2 variants without a reason).
2. **Periodicity methodology: both of the following, not just one.**
   - (a) Extend `toy_prng.py`/`cycle_measure.py`'s existing tooling with a
     new pluggable state-update-mechanism parameter (same spirit as its
     existing `n`/`w` parameterization), to get rigorous λ/μ (Brent)
     statistics per mechanism at tractable toy scale.
   - (b) A full-scale (real 256×32-bit) **practical sanity check**: verify
     the real-scale candidates don't collapse into a short cycle within a
     practically reachable number of iterations (billions), since the true
     astronomical period can't be computed directly (same conclusion the
     2026-8-25 experiment already reached). Do NOT skip this and rely on
     toy-model results alone — the user explicitly rejected that shortcut.
3. **This handover's own scope: documentation only.** Nothing beyond this
   `HANDOVER.md` has been created. Folder generation, toy-model extension,
   candidate C files, perf/avalanche/PractRand scripts — all of it is for
   the executing session to build, starting from this spec.

## Target algorithms (two, per the user's instruction)

Apply the state-update-mechanism spectrum to **both**:

1. **"Original"** = `src/ra_prng2/c/ra_prng2.c`, paper-exact, unpruned,
   all operations active. This is the term "original" has consistently
   meant across all three prior experiments' `RESULTS.md` files (e.g.
   `ra_prng2_orig_local`, "the original, unpruned algorithm") — not
   `baseline.c` (which is the *pruned* candidate from the 2026-8-26
   experiment, called "baseline" there, not "original").
2. **"Winner"** = `experiments/2026-8-27_operand-position-search/
   winner_wired_v2.c`, the current recommended fast candidate (superseded
   `winner_wired.c`/v05 as of 2026-08-28 — see that folder's `RESULTS.md`
   "Decision" section for why).

**Hard constraints, same zero-drift rule every prior experiment in this
repo has followed:**

- **Never modify** `src/ra_prng2/*` or `src/ra_prng3/*` — paper-exact /
  experimental references, used read-only.
- **Never modify** `winner_wired_v2.c` in place — it is a validated,
  documented artifact from a completed experiment. Any new mechanism
  applied to it must be a new, separate, self-contained mirror file in
  *this* folder (same pattern every prior experiment used: `baseline.c`
  was "a direct structural mirror of `src/ra_prng2/c/ra_prng2.c`", not a
  copy; `winner_wired.c`/`winner_wired_v2.c` are standalone files, not
  edits to `baseline.c`).
- New candidate C files should follow the established shape: self-
  contained (`stdio.h`/`stdlib.h`/`stdint.h`/`string.h`/`time.h` only, no
  external deps), same CLI (`--stream <seed> <n>` for a raw pipeable
  stream, default run using a `TOTAL_RNG` benchmark constant) — this is
  what makes `perf stat` numbers comparable apples-to-apples across every
  candidate in this repo so far. `gcc -O3 -march=native` is the
  established build invocation (add `-std=gnu17 -include stdalign.h` if
  `alignas` is used, per `winner_wired_v2.c`'s build line).

## The idea: a spectrum of non-hybrid state-update mechanisms

**Hard rule, directly from the user: no mechanism may mix relocation
(swap-style) and fresh-injection (overwrite-style) within a single
per-step state transition.** Each candidate mechanism must be describable
as one clean rule. The *set* of mechanisms compared can and should be
broad (per scope decision #1 above) — breadth lives at the level of
"which distinct clean rule," not inside any one rule.

Starting catalog (narrow or extend as the search progresses — this is a
menu, not a mandate):

1. **Permutation-only** (today's mechanism, unchanged) — reference point,
   both for `L` (the swap) and implicitly for `M` (untouched, just XOR-
   folded once per outer iteration as it is today).
2. **Full injection, ra_prng3-style**: `L[i] = L[e]; L[e] = mixed` — one-
   directional overwrite, `mixed` derived from the step's own fresh
   output (e.g. reuse `c`/`d` the same way `ra_prng2` already computes
   them, or `ra_prng3`'s own `d` — decide which fits the target
   algorithm's existing chain without inventing a new mixing function
   from scratch unless necessary).
3. **Direct overwrite, no relocation at all**: `L[i] = mixed` — the
   purest "maximal bit change" reading of the user's request: don't even
   preserve the slot's old value anywhere. Flag explicitly: this is the
   variant most likely to *hurt* periodicity (destroys state history
   fastest) — measure it, don't assume either way.
4. **Symmetric treatment of `M`** — the user explicitly said "cara state
   L/M diubah" (how L/M state is changed), so `M` is in scope too, not
   just `L`. Candidates: permute `M`'s slots (mirroring mechanism #1 but
   applied to `M`), or replace `M`'s XOR-fold with a different fold/
   injection rule. Don't treat `M` as an afterthought just because the
   swap-necessity question was phrased around `L` specifically.
5. **(Flagged ambiguous — resolve before implementing)** Cadence-mixing:
   e.g. permutation-swap every step, but a full-injection "refresh" every
   N steps. Each *individual* step is still a clean single rule, so this
   might not violate the "no hybrid step" constraint — but it does
   combine two mechanisms within one experiment's timeline, which may or
   may not be what the user meant by "bukan hybrid." **Do not implement
   this without an explicit decision** (ask the user, or treat as
   out-of-scope by default and only add if time/interest permits after
   the core spectrum above is measured).

## Measurement methodology (4 axes, per the user's instruction)

For every mechanism × every target algorithm (original, winner) combination:

**Periodicity** (dual approach, both required — see scope decision #2):
- *(a) Toy-model, rigorous.* Extend
  `../2026-8-25_periodicity-heuristic-validation/toy_prng.py` and
  `cycle_measure.py`/`cycle_measure.c` with a new state-update-mechanism
  parameter, following the same spirit as the existing `n`/`w`
  parameterization. Run Brent's algorithm across seeds, get λ/μ
  distributions per mechanism, at a scale small enough to be tractable
  (same n∈{2,4,8}, w∈{4,8} territory the 2026-8-25 experiment already
  validated as measurable). This is the statistically rigorous piece —
  it can actually answer "is mechanism X's cycle structure different from
  the swap's" with real numbers, unlike the full-scale algorithm.
- *(b) Full-scale sanity check, practical.* On the real 256×32-bit
  candidates (both target algorithms × every mechanism), verify no
  collapse into a short cycle within a practically reachable iteration
  count (billions — reuse the `--stream`/perf infrastructure already
  established; PractRand runs already implicitly exercise billions of
  outputs without ever finding a repeat, which is suggestive but not a
  targeted check — consider a dedicated Floyd/Brent-style check over
  periodic state-hash checkpoints, or at minimum confirm no exact output-
  block repeat across the largest PractRand run performed). This is not
  a proof of astronomical period — it's a guard against a mechanism that
  accidentally collapses to something practically short.

**Speed**: `perf stat -e instructions,cycles`, same methodology as
`2026-8-27_operand-position-search/other_winners_perf.sh` established.
**Use instruction count as the primary comparison axis** — it's
deterministic (0.00% variance across repeats in every prior measurement
in this repo). Cycle counts are useful too but get noisy under any
concurrent CPU load (concretely happened in the 2026-8-27 session when a
PractRand background job contaminated a `perf stat` run) — never run
`perf stat` concurrently with another CPU-heavy job in this repo's history
of doing so, and don't repeat that mistake.

**Avalanche**: reuse `../2026-8-26_operation-pruning-research/
quality_gate.py`'s `avalanche_stats()` and `avalanche_gate_min_bit()` —
already generic over any `capture_fn(seed) -> list[int]`, no need to
rebuild. **Always check the per-bit minimum, never just the scalar
average** — this is the exact lesson from `baseline.c`'s undetected bit
5/6 dead zone (2026-8-27's `RESULTS.md`, "Root cause" section): a healthy-
looking average (0.418) hid two structurally dead seed bits
(0.000245 each).

**Quality (PractRand)**: tiered gate, same lesson repeated twice now in
this repo — a small gate is not sufficient. Concrete evidence:
`operand_search.py`'s original 2MB Tier-1 filter let `v01`/`v02`/`v09`
through as "winners"; all three failed catastrophically at 16GB (one had
p-values down to `3e-2070`) despite passing avalanche cleanly. Minimum
practice for this experiment: **16GB screen for every candidate**
(~130s/candidate at this repo's measured throughput, tractable even
across a wide mechanism spectrum), **128GB+ only for finalists** that
clear every other axis first.

## What NOT to do

- Don't touch `src/ra_prng2/*` or `src/ra_prng3/*` — read-only references.
- Don't modify `winner_wired_v2.c` (or any other prior experiment's
  artifact) in place — new mechanisms are new mirror files in *this*
  folder.
- Don't run full-scale PractRand/dieharder inside a search/sweep loop —
  screen small (but not too small — 16GB minimum, not 2MB) first, full-
  scale only for finalists.
- Don't run `perf stat` concurrently with another CPU-heavy background
  job (e.g. a PractRand run) — it silently produces noisy, misleading
  cycle counts (instruction counts stay reliable even then, but don't
  rely on that as an excuse to ignore the contamination).
- Don't assume toy-model periodicity conclusions transfer to the real
  256×32-bit scale without the full-scale sanity check — this is exactly
  why the user required the dual approach instead of toy-model-only.
- Don't build a mechanism that mixes swap-relocation and fresh-injection
  within a single per-step state transition — that is literally the
  "hybrid" the user ruled out. If a design idea can't be described as one
  clean rule, it doesn't belong in the core spectrum (see the ambiguous
  cadence-mixing idea above — flagged, not included, until resolved).
- Don't treat `ra_prng3` as anything more than a design-idea source. It
  stays "experimental, do not use" regardless of what this experiment
  finds.
- Don't skip the per-bit avalanche floor check in favor of just the
  scalar average — repeats the exact mistake that let `baseline.c`'s
  defect through undetected in an earlier experiment.

## Open questions for the executing session

Left unresolved on purpose — real design decisions for that session, not
guessed at here:

- **Naming/parameterization scheme** for what could be a fairly wide
  mechanism spectrum. Options seen elsewhere in this repo:
  per-mechanism standalone `.c` files (like `baseline.c`/`winner_wired.c`),
  or a template-substitution generator (like `2026-8-27`'s
  `other_winners_gen.py`, which generated 12 variant `.c` files from one
  template + a parameter tuple). The latter scales better if the
  mechanism spectrum ends up being combinatorial (e.g. mechanism choice ×
  target algorithm × L-only/M-only/both) rather than a short enumerated
  list.
- **Relative priority of `L` vs. `M`** if time is limited. The user's
  literal question ("swap L[i]/L[d] perlu atau tidak") is about `L`
  specifically, but they also explicitly scoped `M` in ("cara state L/M
  diubah"). Recommend treating `L` as primary (it's the one with a
  concrete "is this even necessary" question attached) and `M` as a
  secondary axis explored once the `L` spectrum has usable results,
  unless the executing session has reason to do otherwise.
- **The cadence-mixing idea** (catalog item 5 above) — resolve the
  hybrid-or-not ambiguity explicitly (ideally by asking the user) before
  implementing it, rather than assuming either way.
- **Whether the toy model needs both `L`- and `M`-analogous arrays**
  parameterized for the mechanism swap, or just one — `toy_prng.py`'s
  current structure should be read carefully before extending it; don't
  assume its internal shape without checking, since this handover was
  written from a summarized exploration report, not a full line-by-line
  read of that file.
