# HANDOVER: `ra_core_singleblock` cycle-operation combo search

Status: **spec only, NOT executed.** Written 2026-09-02, to be picked up in a
future session. Do not assume any candidate has been built or tested — this
file is the plan, not a report.

## Why this experiment exists

`ra_core_singleblock` (`experiments/2026-9-1_family-productionization/ra_core.c`,
`ra_init_state_singleblock` L205-212, `ra_permutation_cycle_singleblock`
L214-236) has a structural BCFN defect at small K (fails through K≈72, clean
only from K≈96 — see
`experiments/2026-9-2_singleblock-k-threshold-characterization/RESULTS.md`).

Three prior experiments narrowed the root cause and ruled out the "fix the
init/seed" direction conclusively:

1. **`experiments/2026-9-2_singleblock-prereseed-experiment/`** — pre-mixed
   `M[]`/`cons` via `ra_hash` before the cycle starts (richer input, same
   single-round transform). **FAILED**: BCFN R≈1028 vs baseline's R≈1033 at
   1GB — statistically no improvement. Conclusion in that RESULTS.md: the
   defect lives in the transform's *shape* (too few mixing rounds), not in
   input/seed quality.
2. **Cross-check against `ra_core_orbit`**: `ra_init_state_orbit`'s M[] init
   is *already* the same formula as singleblock's (`fmix32(GUARD_M ^
   (0x06a0dd9bu*key))` keyterm, same per-index rotate), and orbit's cold start
   (`a=cons=key,b=0,c=0,d=0`) is byte-identical to singleblock's. Orbit is
   validated clean at 128GB only because after its first 255-word cycle it
   calls `ra_reseed` and never returns to that cold state; singleblock
   re-enters it on every new key/block. This is a **usage-pattern exposure of
   a shared weakness**, not evidence that orbit's init is structurally
   different/better.
3. **`experiments/2026-9-2_singleblock-o-width-fix/`** — widened `o` from
   2-tap to 8-tap XOR (Kandidat A), and combined with N discarded warm-up
   rounds (Kandidat B). Both only **relocated** which K FAILs, non-
   monotonically, at 16GB (e.g. Kandidat B fixed K=1 fully but broke K=4/K=8).
   Neither eliminated the defect. Full detail + exact R-values in that
   folder's RESULTS.md.

**Conclusion carried into this experiment (user's explicit decision,
2026-09-02): stop trying to fix this via init/seed changes. The init function
(`ra_init_state_singleblock`) stays untouched. All further effort goes into
searching richer per-round *operation combinations* inside
`ra_permutation_cycle_singleblock` itself** — since at K=1 the cycle runs
exactly one round, and only a full 128-255 round warm-up (rejected earlier for
5.5-13x overhead, see `2026-9-1_multikey-remix-search/HANDOVER.md`) has ever
reliably closed the gap. The bet here is that a *richer single round* — not
more rounds — can get the same effect much cheaper, using primitives (MUL,
XORSHIFT) this codebase already trusts (`fmix32`) but has never used inside
the cycle body itself (only in init).

## Goal

Find an operation-combo variant of `ra_permutation_cycle_singleblock`'s
per-round transform — `ra_init_state_singleblock` unchanged, verbatim from
`ra_core.c` — that clears BCFN/TMFn/FPF anomalies for K ∈ {1,2,4,8,16,32,64,96}
at 16GB PractRand scale, tested via the **multikey** harness (not a long
single-stream), at overhead well under the rejected full-warm-up fix's
5.5-13x (a few-x is acceptable; this is per-round enrichment, not extra
rounds).

A search that converges on **no acceptable candidate** is a valid, complete
outcome — write it up the same as a success (see "Deliverables" below). Do
not force a positive result.

## Search space (DSL) — bounded on purpose

Keep the existing update-chain backbone unchanged. It is already the
exhaustively-verified winner (`experiments/2026-8-27_operand-position-search/`)
for the long-stream case, and operand-position/wiring is not the variable
under test here:

```
o = tap_combo(M, i)                        # enrichment slot 1
a = (d ^ o) ^ (cons + a)
b = (cons + a) ^ (o + d)
c = rot32((a >> 13) ^ a, b)
c = finalizer(c)                            # enrichment slot 2 (NEW)
[extra_inject into a or b]                  # enrichment slot 3 (NEW)
d = c & 0xFFu
```

### Slot 1 — `o` tap width: `{2 (current), 4, 8}`

Same XOR-shift tap construction as `pruned_prng.c`'s `build_o()` /
`diag_wideo_singleblock.c`'s `wide_o()`:
`o = XOR_{e=0..w-1} M[(uint8_t)(i+e)] << e`.

### Slot 2 — `c`-finalizer: 0-2 extra stages applied to `c` after its `rot32`

Modeled explicitly on `fmix32` (already in `ra_core.c`, already trusted at
128GB scale via orbit's keyterm derivation) — this is "how few fmix32-shaped
stages on `c` close the K-small gap," not an unconstrained operator soup.
Primitive stage types:

- `MUL(const)` — `x *= const`. Constant pool, curated (do not invent new
  untested constants): `0x85ebca6bu`, `0xc2b2ae35u` (both already in
  `fmix32`), `0x9e3779b7u`, `0x06a0dd9bu` (already in this codebase's init),
  plus Murmur3 fmix32/fmix64 finalizer families (`0xff51afd7u`, `0xc4ceb9feu`
  and their fmix32-specific counterparts) as fresh-but-well-known options.
- `XORSHIFT(s)` — `x ^= x >> s`, `s ∈ {13, 15, 16, 17}`.
- `ROT(amt_source)` — `x = rot32(x, amt)`, amt source `∈ {b, i, cons}`.
- `ADD(operand)` — `x += operand`, operand `∈ {cons, o, a, b}`.

Hand-curate ~20-50 finalizer recipes of 1-2 stages (e.g. `XORSHIFT(16),
MUL(0x85ebca6b)`; `MUL(0xc2b2ae35), XORSHIFT(13)`; `ADD(o), ROT(b)`; etc.) —
favor sequences shaped like known-good finalizers (xorshift-then-multiply is
the Murmur/fmix pattern) over a full cartesian product of every
stage-type × operand × constant combination. Include the empty recipe
(current behavior, `c` unchanged) as the control.

### Slot 3 — extra inject: `{off (current), +M[(i+k)&0xFF] into a, ^M[(i+k)&0xFF] into b}`

Small set of offsets `k` (e.g. `{16, 32, 64}`) — tests whether feeding more
raw `M[]` entropy into the chain directly (independent of `o`/finalizer)
helps.

**Total candidate count**: tap-width(3) × finalizer-recipes(~20-50) ×
extra-inject(~7-10 incl. off) — low hundreds. Tractable for a greedy or
bounded-exhaustive driver. Do **not** let this grow by cartesian-multiplying
every operand/constant choice within each primitive — the curation step above
is what keeps it bounded, the same way `operations.py` (18 hand-picked flags)
and `wiring.py` (4 hand-picked slots, 108 combos) kept prior search spaces
enumerable rather than combinatorially exploding.

## Search procedure

### Tier 0 — avalanche filter (cheap, Python or fast C)

Flip each seed bit, capture the single K=1 output word, require the
**min-per-bit** avalanche fraction (not scalar average — the average version
missed a near-dead-bit candidate in `2026-8-27_operand-position-search`, see
that folder's `quality_gate.py` `avalanche_gate_min_bit` vs `avalanche_gate`)
in a healthy band. Reuse/port
`experiments/2026-8-27_operand-position-search/quality_gate.py`'s pattern.

### Tier 1 — multikey PractRand search-loop gate (NEW — do not reuse the long-stream gate as-is)

This is the critical departure from prior search experiments. The defect only
manifests via multikey/short-block usage; a long single-stream gate has
already been shown in this codebase's history to miss things a differently-
shaped gate catches (three separate documented "gate size trap" incidents:
1MB and 8MB in `2026-8-26_operation-pruning-research`, 2MB/16MB in
`2026-8-27_operand-position-search`, plus wide-o's own 16MB-clean → 16GB-FAIL
surprise in `2026-9-2_singleblock-o-width-fix`).

- Build on the multikey `--stream` driver pattern from
  `experiments/2026-9-2_singleblock-o-width-fix/diag_wideo_warmup_singleblock.c`
  (Weyl-increment key chaining `key = base_key + block_idx * 0x9E3779B9u`, `K`
  param). Generalize it — or generate one C source per candidate the way
  `experiments/2026-8-27_operand-position-search/other_winners_gen.py` did —
  to accept the o-tap-width, finalizer recipe, and extra-inject choice.
  Per-candidate C generation is likely cleaner than a single runtime-flag
  binary given MUL-heavy candidates need compiled code for affordable
  PractRand throughput (established in `2026-8-26_operation-pruning-research`:
  pure Python was ~0.8 MB/s, too slow).
- Gate at **K=1 specifically** (documented worst case).
- Use a **staged** size, not one fixed small size: quick-reject at a small
  size first, survivors re-checked at a larger size before being called a
  "search winner." Pick concrete sizes when implementing, informed by how
  many candidates survive Tier 0 (balance search-loop cost against escape
  risk) — **state the chosen sizes and reasoning explicitly in RESULTS.md**,
  and treat Tier 1 clean as "did not fail the search filter," never as proof
  of correctness.
- Sanity-check each generated candidate's `--single` output against a
  hand-traced expected value before running PractRand on it.

### Promotion tier

Candidates surviving Tier 1 get tested across
`K ∈ {1,2,4,8,16,32,64,96}` at **16GB** via the multikey harness. Use the VPS
(`VPS_ACCESS.md`, gitignored — check it for any process still running before
starting new ones) for these, per this repo's standing convention for
multi-hour PractRand runs (see `CLAUDE.md`). Only a candidate clean across
that **entire** K range at 16GB is eligible for further scale-up (32GB+) or
dieharder validation before it can even be discussed for `ra_core.c`.

## Non-goals / hard constraints

- Do **not** modify `ra_core.c`. Build every candidate as a self-contained
  diagnostic file (copy `rot32`/`fmix32`/`GUARD_L`/`GUARD_M`/
  `ra_init_state_singleblock` verbatim, same pattern as
  `diag_wideo_singleblock.c`/`diag_wideo_warmup_singleblock.c` — do not
  extern-link, the originals are `static`). Applying a winning candidate to
  `ra_core.c` requires a separate, explicit user confirmation — do not
  auto-promote even a fully-clean result.
- Do **not** touch `ra_init_state_singleblock` — out of scope per the Context
  section above.
- Do **not** resume or touch
  `experiments/2026-9-1_production-candidate-battery/` — stays paused
  pending a validated fix from this line of work.
- **No re-seed retries on FAIL.** Per standing rule (memory
  `feedback_no_reseed_on_fail.md`): a FAIL on one seed at any tier is
  decisive — reject that candidate and move to the next one. Never re-run the
  same candidate/config with a different seed/base_key to see if the FAIL was
  "unlucky."
- Never write VPS credentials into any file tracked by git.
- Do not add any `pruned_prng.c` operations beyond what this search space
  already covers (no scope creep into unrelated ablation flags like
  `MULT_REDUCE`/`HASH_SELFIDX`/`HASH_ACCESS` — those apply to `ra_hash`,
  which singleblock's cycle doesn't call).

## Deliverables

- `RESULTS.md` in this folder, same structure as
  `experiments/2026-9-2_singleblock-o-width-fix/RESULTS.md`: candidate table
  (BCFN/TMFn/FPF R-values per K), overhead numbers (ns/word vs baseline,
  reinit-sweep microbench pattern from `bench_ra_core.c`), explicit
  recommendation section. "No candidate recommended" is a valid, complete
  outcome if the search doesn't converge — write it up with the same rigor as
  a positive result, do not omit or soften a negative outcome.
- Run `/graphify --update` before closing out the task, per this repo's
  `CLAUDE.md` workflow rule.
- Update memory (`project_singleblock_kmin_defect.md` and/or a new memory)
  with the outcome, following the pattern already used for the o-width-fix
  experiment's conclusion.

## Critical files to reference

- `experiments/2026-9-1_family-productionization/ra_core.c` — L86-125
  (`rot32`/`fmix32`/`GUARD_L`/`GUARD_M`/`ra_hash`/`ra_reseed`), L205-236
  (`ra_init_state_singleblock`, `ra_permutation_cycle_singleblock`) — the
  unchanged baseline.
- `experiments/2026-8-26_operation-pruning-research/quality_gate.py`,
  `operations.py`, `ablation_search.py`, `pruned_prng.c` — gate pattern,
  flag-catalog-curation pattern, greedy-search driver shape to extend.
- `experiments/2026-8-27_operand-position-search/wiring.py`,
  `operand_search.py`, `other_winners_gen.py`, `quality_gate.py` — bounded-
  exhaustive-enumeration pattern, min-bit avalanche gate, per-candidate
  C-generation pattern.
- `experiments/2026-9-2_singleblock-o-width-fix/diag_wideo_warmup_singleblock.c`,
  `diag_wideo_singleblock.c` — multikey `--stream` driver pattern (Weyl key
  chaining, K param) and the verbatim-copy-not-extern-link pattern for
  `static` functions.
- `experiments/2026-9-2_singleblock-k-threshold-characterization/RESULTS.md`
  — original monotonic K-threshold data, the comparison baseline for scoring
  how much a candidate actually improves.
- `experiments/2026-9-2_singleblock-prereseed-experiment/RESULTS.md`,
  `experiments/2026-9-2_singleblock-o-width-fix/RESULTS.md` — prior negative
  results this experiment must not blindly repeat (read before designing
  finalizer recipes, to avoid re-testing already-falsified shapes like
  "just widen o with nothing else").

## Verification

- Compile each candidate and sanity-check `--single` output against a
  hand-traced expected value before any PractRand run.
- Confirm `git diff --stat -- experiments/2026-9-1_family-productionization/ra_core.c`
  stays empty throughout the experiment.
- Confirm no writes/timestamp changes under
  `experiments/2026-9-1_production-candidate-battery/`.
- RESULTS.md must explicitly state that no candidate's FAIL was re-run with a
  different seed to explain it away (per the standing methodology rule).
