# Handover: operand-position rewiring untuk `ra_permutation_cycle`

Written for a fresh Claude Code session (or the user) picking this up cold
in a later session. This is a **spec for a not-yet-started research task**
— nothing has been implemented yet, no search/generation code exists here.
Direct continuation of `experiments/2026-8-26_operation-pruning-research/`
(operation-pruning search), whose `RESULTS.md` explicitly flagged this idea
as out of scope for that round:

> **Operand-position changes** (which state variable feeds which slot in
> the `a/b/o/c` update chain) — a structurally larger search space,
> deliberately deferred to a separate round per the plan.

## Context: why this idea exists

The operation-pruning experiment found a winning candidate — `ops =
{TAP6, TAP7, ROT_C, SHR13}` (4 of 18 tracked flags active, 14 removed),
`hash_access = sequential`, original shift widths untouched — delivering a
reproducible ~2.4-2.5x speedup (`perf stat`, instructions/cycles) while
passing PractRand up to 128GB (0 anomalies) and all 27 "Good"-reliability
dieharder tests. That search worked by **removing** operations (discrete
on/off flags). It never considered **rewiring** — keeping every operation,
but changing which state variable feeds which slot in the update chain.

The user confirmed the key distinction driving this handover: swapping
operand positions does **not** change the operation count (same number of
rotates/XORs/adds/shifts) — so it gives **no direct speedup**. What makes
it worth a separate round anyway: if some other wiring diffuses better
(stronger avalanche/statistical margin) than the original, that new wiring
could become a more favorable starting point for a *subsequent* pruning
round — i.e. any speedup this idea produces is indirect, arriving only if
combined with pruning afterward, not from the rewiring itself.

## Baseline to preserve / use as reference

This folder's own baseline is the operation-pruning experiment's winning
candidate, **not** the original paper-exact algorithm — copied here
unmodified (verified `diff`-clean against the source) and renamed for
brevity:

- `baseline.c` (was `pruned_winner.c` in the pruning experiment) — the
  winning candidate, specialized/hardcoded C mirror of
  `src/ra_prng2/c/ra_prng2.c`'s structure with the pruned op set baked in.
  Self-contained (`stdio.h`/`stdlib.h`/`stdint.h`/`string.h`/`time.h`
  only), compiles standalone: `gcc -O3 -march=native baseline.c -o
  baseline`. Same CLI as the original (`--stream <seed> <n>` for a raw
  pipeable stream, or default `TOTAL_RNG` run).
- `baseline_refactored.c` (was `pruned_winner_refactored.c`) — same
  candidate with the sliding-window TAP6/TAP7 load-reuse micro-refactor
  (bit-identical output, ~3.2% fewer instructions, perf-neutral in
  practice since the loop is latency- not throughput-bound). Also
  self-contained, same build command.

Numbers any new wiring should be measured against (from
`experiments/2026-8-26_operation-pruning-research/RESULTS.md`):

| metric | value |
|---|---|
| speed vs. original algorithm (`perf stat`, instructions/cycles) | ~2.4-2.5x fewer/faster |
| PractRand | 0 anomalies up to 128GB |
| dieharder | all 27 "Good"-reliability tests PASSED |

`src/ra_prng2/c/*` remains the paper-exact reference and is **not** touched
by this line of research either — same constraint as the pruning
experiment.

## The idea (as described by the user, not yet a plan)

`ra_permutation_cycle`'s per-step churn (see `baseline.c`) updates five
variables through a fixed chain of expressions:

```c
a = (rot32(b ^ o, d) ^ (cons + a));
b = (rot32(cons + a, i) ^ (o + d));
o = (rot32(a ^ o, i) << 9 ^ (b >> 18));
c = rot32((o + c << 14) ^ (b >> 13) ^ a, b);
d = (uint32_t)(((uint64_t)c * (i + 1)) >> 32);
```

"Operand position" means: which of `a`, `b`, `o`, `c`, `d` (and `cons`,
`i`) occupies which slot in these five expressions — e.g. swapping which
variable feeds the XOR term in `b`'s update vs. `a`'s. **The set of
operations stays identical**; only the data-dependency wiring changes.
This is a rewiring axis, orthogonal to the pruning experiment's on/off
axis — it changes output/statistical character, not instruction count.

The point of exploring it: find out whether some alternative wiring has
better diffusion properties than the original (which was hand-designed,
not searched), and if so, whether starting a *second* pruning round from
that better-diffusing wiring can remove more operations than pruning
managed starting from the original wiring — i.e., this experiment's
success condition is about unlocking a better pruning ceiling later, not
about anything measured directly in this experiment.

## Open questions for the future planning session

These are unresolved on purpose — real design decisions for that session,
not to be guessed at now:

- **How to represent and bound the swap search space.** Five variables
  across many slots is combinatorially large; not every swap is
  semantically valid (types/roles differ — e.g. `d` is only ever used as a
  rotate amount, `cons` is read-only within the step). Needs a concrete
  definition of which slots are swappable before any search method can be
  chosen.
- **Starting point: `baseline.c` (4/18-op pruned candidate) or the
  original, unpruned algorithm?** Rewiring a heavily-pruned candidate vs.
  the full operation set are different search spaces with different
  interpretations of "better diffusion." Not decided.
- **What counts as "success" here**, given this experiment produces no
  direct speedup? Candidates: an avalanche/diffusion score improvement
  over the original wiring's ~0.364 avalanche fraction (see the pruning
  experiment's shift-repair section for why avalanche alone was found to
  be a *misleading* proxy there — any metric chosen here needs the same
  large-scale PractRand skepticism); or, more directly, whether a
  rewired variant survives a repeat of the pruning experiment's
  greedy-ablation search with a *smaller* final op count than the current
  4/18 winner.
- **Reuse vs. rebuild search infrastructure.** The pruning experiment's
  `quality_gate.py` (avalanche + PractRand-prefix two-tier gate) and
  `ablation_search.py` (greedy search loop) live in
  `../2026-8-26_operation-pruning-research/`. Whether they can be adapted
  as-is (the gate logic is likely reusable; the search loop is not, since
  it walks on/off flags, not permutations) is a decision for the
  implementation session, not this handover.

## What NOT to do

- Don't touch `src/ra_prng2/c/ra_prng2.c`, `ra_prng2_thread.c`, or
  `ra_prng2_struct.c` — paper-exact, zero-output-drift references, same
  constraint that governed the pruning experiment.
- Don't re-derive the winning pruned candidate from scratch — `baseline.c`
  / `baseline_refactored.c` in this folder already are that candidate,
  copied verbatim (diff-clean) from the pruning experiment's output.
- Don't run full-scale PractRand/dieharder inside a search loop — the
  pruning experiment hit three separate "gate-size traps" (small/cheap
  gates passing candidates that failed hard at larger sizes) before
  landing on an 8MB+ tiered gate; reuse that lesson rather than
  rediscovering it. Full-scale validation is for the final candidate only.
- Don't assume swapping operand positions gives a speedup on its own —
  already confirmed false (see Context above): the op count doesn't
  change, so any measured speedup would have to come from a *second*
  pruning round on top of a rewired candidate, not from rewiring alone.

## Original algorithm avalanche baseline (measured, 2026-08-27)

Added `other/avalanche_heatmap_original.py` (32 seed-bit flips x 255
output-position Hamming-distance heatmap, full ALL_OPS formula, cross-
checked exactly against both `pruned_prng.py`'s `stream()` and
`src/ra_prng2/python/ra_prng2.py`'s paper-exact `ra_core()` before any
number below was trusted). This gives the empirical avalanche baseline
any rewired candidate from this experiment should be measured against —
useful context for the "what counts as success" open question above.

Motivation: the sibling pruning experiment's winning candidate
(`baseline.c` in this folder) has a real, previously-undetected defect —
seed bits 5 and 6 are nearly avalanche-dead (253/255 output positions
show 0 bit difference when either is flipped), invisible to
`quality_gate.py`'s `avalanche_gate()` because it only reports one
number averaged across all 32 bits (0.418, still inside the passing
[0.3, 0.7] band). The user asked whether the *original*, unpruned
algorithm has the same weakness. It does not:

- **Zero dead bits.** All 32 seed-bit rows have 0 exactly-zero-Hamming-
  distance cells (`pruned_winner`'s bits 5/6 each have 253/255). Overall
  avalanche fraction 0.497576 — essentially the 0.5 ideal, vs.
  `pruned_winner`'s 0.418459.
- **Bit-flip axis is tight**: per-bit mean Hamming distance ranges
  15.3-16.2 (out of 32) across all 32 bits, std only 0.195.
- **Output-position axis is even too, except a genuine 2-step warm-up
  region at the very start of the cycle**: output index 0 (the cycle's
  very first step, loop counter `i=255`) is a real outlier (min=1,
  max=18, mean=5.47 across the 32 bit-flip trials) because `d` starts at
  0 for every seed regardless of which bit was flipped, so it hasn't had
  a chance yet to reflect the seed difference; index 1 is a smaller
  version of the same effect (min=3, mean=13.8). From index ~2 onward,
  per-cell Hamming distances settle into a tight band mostly in the
  ~10-22 range (mean ~16) with only a small tail outside it (0.8% of
  cells <=9, 5.3% >=21, out of 8096 cells checked) — consistent with
  ordinary binomial scatter around mean 16 for 32 fair-ish trials
  (expected std ~2.83), not a systematic defect. (Correction on exact
  location: the warm-up outlier is at output index 0-1, i.e. loop
  counter `i=255..254` -- not "around index 6" as first guessed by eye
  from the plot; by index 6 the distribution is already in the same
  settled ~10-22 band as the rest of the cycle.)

**Implication for this experiment's premise**: this is concrete evidence
that the pruning search's 4-op winning candidate destroyed two seed
bits' avalanche contribution entirely, in a way the existing quality
gate cannot see. Any rewiring candidate explored here should be checked
against this per-bit heatmap (not just `avalanche_gate()`'s scalar
average) before being considered a diffusion improvement — a rewiring
that "improves" the average while still killing individual bits would
repeat the same blind spot.

Raw output: `other/avalanche_heatmap_original.png`.
