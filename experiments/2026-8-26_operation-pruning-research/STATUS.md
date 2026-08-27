# Status: operation-pruning greedy ablation search

Live status file, per the pattern in
`experiments/2026-8-25_periodicity-heuristic-validation/STATUS.md` — rewritten
whenever a session ends with work still in progress, so a later session can
resume without re-deriving what's already settled.

## What's done

- `pruned_prng.py`: full-scale (n=256, w=32) reimplementation of
  `ra_permutation_cycle`/`ra_hash`, parameterized by an `operations.Candidate`
  (flag set + `hash_access` mode). Self-check (`python3 pruned_prng.py`)
  confirms `ALL_OPS` (with the default `hash_access="strided"`) is
  bit-identical to `src/ra_prng2/python/ra_prng2.py` across several
  seed/iteration combos.
- `operations.py`: catalog of 18 boolean ablation flags (8 `TAP*`, 4 `ROT_*`,
  4 `SHL*`/`SHR*`, `MULT_REDUCE`, `HASH_SELFIDX`) plus a `HASH_ACCESS`
  choice (`strided` = original `N[j*8+i]` indexing, `sequential` =
  contiguous-block `N[i*32+j]` indexing) — both preserve the full 32-term
  mixing depth, only the access pattern changes.
- `quality_gate.py`: Tier 0 avalanche gate (pure Python, milliseconds) +
  Tier 1 PractRand-prefix gate (pipes raw `c` bytes into the already-built
  `~/Documents/research/PractRand/RNG_test` via `stdin32`, default 8MB —
  see "traps caught" below for why 8MB, not smaller).
- `ablation_search.py`: Phase A (hash-access check) + Phase B (greedy
  ablation over the 18 flags), logging every trial/accept/converge event to
  `ablation_log.jsonl`.

## Correction (user, mid-session): HASH_DEPTH must stay fixed at 32

The first design let `HASH_DEPTH` (the `ra_hash` reduction's term count,
default 32) be swept down to `{16, 8, 4}` as a search variable, mirroring the
periodicity-heuristic experiment's `rows` parameter. **The user corrected
this**: reducing depth breaks periodicity and is not an acceptable pruning
target — the periodicity experiment's `rows` finding (47.93x cycle-length
change at fixed state size) was a warning against touching this knob, not an
invitation to search over it. This was confirmed empirically before the
correction landed (see next section) — the depth-sweep run had already found
a depth=4 candidate that looked clean at a too-weak gate but was badly broken
at a stronger one, i.e. shrinking depth doesn't just risk periodicity in
theory, it actively hides broken candidates from quality gates in practice.

**New design**: `HASH_DEPTH_FIXED = 32` always, everywhere. The pruning
target for `ra_hash` shifted from "how many terms" to "which access pattern"
— `operations.HASH_ACCESS_CHOICES` (`strided` vs `sequential`), tested in
Phase A, keeping the full 32-term mixing depth in both variants. Code
(`operations.py`, `pruned_prng.py`, `quality_gate.py`, `ablation_search.py`)
has been updated accordingly; the old `hash_depth` field / `HASH_DEPTH_*`
sweep code no longer exists.

## Trap caught by execution, not inspection (before the depth correction)

The now-abandoned depth-sweep run used a 1MB PractRand tier as the loop gate
(chosen from a throughput estimate, not verified against a known-bad
candidate). It converged to a candidate with only 3/18 flags active at
`HASH_DEPTH=4`, passing the 1MB gate cleanly ("no anomalies"). Manually
re-testing that same candidate at 16MB produced multiple hard `FAIL` results
(BCFN, Gap-16 tests). 2MB was the smallest size that already caught this
specific candidate (one `FAIL`) — 1MB was simply too little data for
PractRand's tests to trigger at all. **Fix, still in effect**:
`DEFAULT_PRACTRAND_BYTES` raised to 8MB (4x the empirically found 2MB
minimum) in `quality_gate.py`, documented inline. This finding is unrelated
to the depth correction above and stays valid regardless of what HASH_DEPTH
policy is in effect — it's evidence the *gate itself* was too weak, a
separate lesson from "don't shrink HASH_DEPTH."

## In progress

Full Phase A + Phase B run at the corrected design (HASH_DEPTH fixed at 32,
HASH_ACCESS Phase A check, 8MB PractRand tier), launched via background shell
task. See `ablation_log.jsonl` for the live trial-by-trial log (one JSON
object per line, append-only, safe to tail while running).

**To resume if interrupted**: `ablation_log.jsonl` has every decision made so
far (`"event": "accepted"` lines show the running candidate). Re-run
`ablation_search.py` — it does NOT currently auto-resume from the log (this
is a known gap, not implemented since Phase B for 18 flags is short enough
that a from-scratch rerun is cheap; add log-replay to `phase_b_greedy_ablation`
if a future run needs to survive an interruption mid-search).

## Step 1 diagnostic result: discrete-only ablation is NOT sufficient (confirmed)

Reran Phase A + Phase B from scratch with the corrected 64MB C-backed gate
(`quality_gate.py` piping `pruned_prng.c` -> `RNG_test`). Converged to a more
conservative candidate than before: **7/18 ops active** (kept `ROT_A`,
`ROT_B`, `ROT_C`, `SHR18`, `SHL14`, `SHR13`, `HASH_SELFIDX`; removed all 8
`TAP*`, `ROT_O`, `SHL9`, `MULT_REDUCE`). Passed the 64MB loop gate cleanly.

**Trap #3**: independently validated at 128MB and 256MB (per the plan's
mandatory post-Step-1 check) -- **failed at both**, same signature as traps
#1 and #2: multiple hard `FAIL`s on `BCFN` (low-order bit correlation) and
`Gap-16`. 64MB was, once again, not enough data to expose this candidate's
weakness.

**Pattern across all three traps, worth flagging explicitly**: every
candidate greedy ablation has ever converged to -- regardless of gate
strength or which HASH_DEPTH/HASH_ACCESS was in effect -- removed **all 8
TAP* flags**, i.e. eliminated `M`'s contribution to the inner 255-step loop
entirely (`M` then only re-enters via `ra_hash` once per full outer
iteration, not per step). This is a strong candidate explanation for the
recurring BCFN/Gap-16 failures: greedy ablation is locally myopic -- each
individual TAP removal passes the gate on its own, but the *cumulative*
effect of removing all 8 together isn't visible to a one-at-a-time search,
and no gate size tried so far (1MB through 256MB) fully exposes it at the
point the loop makes each individual decision (64MB *did* expose it, but
only after the fact, on the fully-converged candidate).

**Conclusion**: per the user's own branching instruction, this is the
"kelihatannya semuanya buruk" case -- proceed to Step 2b (repair/rebalancing
phase) rather than accepting a discrete-only candidate. Pending: user
confirmation on Step 2b's scope (shift-amount tuning only, vs. also folding
in operand-position search).

## In progress: shift-width repair search (before Step 2a)

Winning candidate from Step 1 + min_active_taps constraint (6/18 ops:
`TAP6`, `TAP7`, `SHL9`, `SHL14`, `SHR13`, `MULT_REDUCE`, hash_access=
sequential) validated clean up to 2GB PractRand (one mild "unusual" at 8GB,
consistent across 3 extra seeds at 1GB) -- see the trap history above for
why this is a genuinely strong result compared to the 3 earlier failed
candidates.

Before moving to Step 2a (speed measurement + full validation battery), per
user request: check whether the *inherited* shift widths (SHL9=9, SHL14=14,
SHR13=13 -- SHR18 is OFF in this candidate) are actually well-suited to this
reduced op set, or whether retuning them finds a more diffusive ("chaos")
point. Motivating signal: this candidate's avalanche fraction at the
*original* widths is only ~36% (target ~50%), noticeably worse than every
full-op-set candidate's ~50%.

`pruned_prng.py` and `pruned_prng.c` both extended with a `shift_widths`
field/args (default `(9,18,14,13)`, cross-validated bit-identical to the old
hardcoded-width behavior and to each other for non-default widths).
`shift_repair.py` runs a full grid search over `s9,s14,s13 in [1,31]`
(`s18` fixed/unused), scored by avalanche closeness to 50%, ~14 min
(31^3 evals x ~28ms each, pure Python, no subprocess). Running in background
-- results land in `shift_repair_log.jsonl`.

**User guidance to apply after this search returns**: `ROT_*` was one of
the fastest diffusion contributors in the original algorithm. If shift-width
retuning alone can't find a candidate that both (a) reaches good avalanche
balance and (b) survives PractRand validation at the same tiers the winning
candidate passed, the next fallback is NOT more shift tuning -- it's
reintroducing some or all of the 4 `ROT_*` flags (currently all OFF in the
winning candidate) before declaring failure. This mirrors the TAP lesson:
removing an entire operation *category* (all TAPs, or here all ROTs) seems
to be the actual risk factor, not any specific single operation.

## Shift-repair search: completed, FAILED (avalanche proxy is misleading here)

`shift_repair.py`'s full grid (31^3 = 29791 evals, ~18 min) found widths
that push avalanche fraction from the original's 0.364 to 0.4982 (near-
perfect 50%) at `s9=29, s14=3, s13=1`. Validated against real PractRand:

- **Rank #1** (s9=29,s14=3,s13=1, avalanche=0.4982): FAILS at 256MB already
  (`FPF-14+6/16` FAIL, `DC6-9x1Bytes-1` very suspicious), gets markedly
  worse at 1GB and 2GB (many hard FAILs, same low-bit test family).
- **Rank #4** (s9=5,s14=2,s13=2, avalanche=0.4977): passes 256MB and 1GB,
  but **fails at 2GB and worse at 8GB** -- same `FPF-14+6/16` signature.
- **Rank #9** (s9=6,s14=1,s13=1, avalanche=0.4973): fails at 1GB already.

All 3 top-avalanche candidates checked are **worse** than the untouched
original widths (9,18,14,13, avalanche=0.364), which remain clean to 2GB
with only one mild "unusual" at 8GB. **Conclusion: avalanche fraction is an
anti-correlated, misleading proxy for this specific repair search** --
optimizing for it finds candidates with severe low-order-bit correlation
defects (`FPF-14+6/16`/`DC6`, a different failure signature than the
BCFN/Gap-16 seen in the TAP-removal traps) that only PractRand at
sufficient scale exposes. Full results: `shift_repair_log.jsonl`.

**Per user's standing fallback instruction**: shift-width repair has now
been tried and failed (multiple points, not just one). Next step is
reintroducing some/all of the 4 `ROT_*` flags (currently all OFF in the
winning 6/18-op candidate) rather than more shift tuning -- `ROT_*` was
flagged as one of the fastest diffusion contributors in the original
algorithm, and the TAP-removal traps already showed that wiping out an
entire operation *category* is the recurring risk pattern, which this
result reinforces (removing all 4 `ROT_*` may itself be the real defect,
independent of shift widths).

## MIN_ACTIVE_ROTS constraint: SUCCESS -- new best candidate

Reran Phase B with `min_active_taps=2` AND `min_active_rots=1` (forbid
wiping out either category entirely -- user's hypothesis after the
shift-repair failure: `ROT_*` was one of the original algorithm's fastest
diffusion contributors, so losing all 4 at once, not the shift widths, was
the likely real defect). Converged to a **more aggressive** candidate than
before: **4/18 ops active** (`TAP6`, `TAP7`, `ROT_C`, `SHR13` -- 14/18
removed, vs. the previous winner's 12/18 removed).

Validated at the same escalating tiers as before, at original (untouched)
shift widths:

| tier | seed=1 | notes |
|---|---|---|
| 64MB-2GB | PASS | 2GB: 0 anomalies (1GB had one mild "unusual", gone again at 2GB) |
| 8GB | **PASS, 0 anomalies** | better than the 6-op candidate's one "unusual" at 8GB |
| 1GB x3 extra seeds (2, 999, 0xC0FFEE) | **PASS, 0 anomalies, all 3** | |

This is the best-validated candidate so far in this experiment: more ops
removed (better speedup potential) AND a cleaner validation record than any
prior candidate. Confirms the user's hypothesis directly -- the earlier
failures (TAP-only-constrained winner, and both shift-repair attempts) were
never really about shift widths; they were about eliminating an entire
diffusion-contributing operation *category* (all TAPs, or all ROTs).
Keeping >=1 from each of TAP and ROT appears to be enough to preserve real
statistical quality even under otherwise-aggressive pruning.

**Current best candidate for Step 2a**: `ops={TAP6,TAP7,ROT_C,SHR13}`,
`hash_access=sequential`, `shift_widths=(9,18,14,13)` (untouched originals
-- shift-repair search is not revisited for this candidate, since it wasn't
needed).

## Step 2a: DONE — final candidate validated, speedup measured, experiment concluded

`pruned_winner.c` (specialized hardcoded C mirror of `src/ra_prng2/c/ra_prng2.c`,
same file shape, built with the project's exact `-std=gnu17 -include stdalign.h`
flags) implements the winning candidate: `ops={TAP6,TAP7,ROT_C,SHR13}`,
`hash_access=sequential`, original shift widths `(9,18,14,13)` untouched.
Cross-validated bit-identical against `pruned_prng.c`/`.py` before trusting it.

**Speed** (`perf stat -e instructions,cycles`, TOTAL_RNG=200,000,000, apples-to-
apples build flags, re-confirmed on a clean rerun):

| | baseline (`build/bin/ra_prng2`) | `pruned_winner` | ratio |
|---|---|---|---|
| instructions | 14,900,505,959 | 6,255,028,168 | **2.38x fewer** |
| cycles | 4,422,117,023 | 1,738,938,019 | **2.54x fewer** |
| wall time | 1.1548s | 0.4546s | **2.54x faster** |

**Quality — PractRand** (piped, no file, `RNG_test stdin32`):
- 16GB (2^34 bytes, seed=1): **"no anomalies in 240 test result(s)"**, 233s.
  (This is on top of the earlier 8GB pass + 1GB x3-extra-seed pass already
  recorded above under "MIN_ACTIVE_ROTS constraint: SUCCESS".)

**Quality — dieharder** (all 27 "Good"-reliability tests, run individually via
direct pipe — `-a`/file-based full battery is infeasible here: it requires
rewinding a fixed-size file millions of times, which produces catastrophic
false-FAILs at any file size we can afford on this disk; see the two traps
below for concrete evidence of this):

- All 27 tests **PASSED** except two apparent failures, both diagnosed as
  **methodology artifacts, not real defects**, and confirmed fixed by rerunning
  correctly:
  1. **`-d 201` (`rgb_minimum_distance`) "FAILED" at p=0.00000000** — my
     invocation omitted `-n`, so dieharder defaulted to `ntup=0`, a
     configuration the *original* algorithm's own passing dieharder run
     (`benchmarks/results/ra_prng2_original/dieharder_test.txt`) never
     actually tests (that run's `-a` battery only exercises `ntup=2..5` for
     this test). Rerun at `-n 2`, `-n 3`, `-n 4`, `-n 5` (matching the
     original's methodology exactly): **all four PASSED**
     (p=0.71, 0.01, 0.75, 0.95).
  2. **`-d 17` (Marsaglia-Tsang GCD test) hit `EOF`** — this test consumes
     ~8GB of input (2×10^7 rands/psample × 100 psamples), far more than the
     1.2GB single-pass stream first piped in. Reran with a 10.4GB stream
     (2.6B rands, ~2min, pure pipe, no disk write): **PASSED** (both
     sub-results p=0.80, 0.96).
  3. One `sts_serial|8` sub-test showed `WEAK` (p=0.99527) — a single WEAK
     among 56 `sts_serial` p-values is ordinary statistical noise (dieharder
     distinguishes WEAK from FAILED for exactly this reason: an isolated
     high-tail p-value among dozens is expected under H0), not evidence of a
     defect.
  4. `-d 7` (`diehard_dna`) was correctly **excluded** from the start — rated
     "Suspect" reliability by dieharder itself, per the plan's methodology
     (only "Good"-reliability tests, `-d 5,6,7` excluded as "Suspect",
     `-d 14` excluded as "Do Not Use").
- Full raw output: `dieharder_pruned_winner_piped.txt`,
  `dieharder_rgb_minimum_distance_fixed.txt`, `dieharder_gcd_fixed.txt`.

**Verdict**: the winning candidate (4/18 tracked ops active, 14/18 removed —
`TAP6`, `TAP7`, `ROT_C`, `SHR13` kept; all shifts/rotations at original
widths) delivers a reproducible **~2.4-2.5x speedup** and passes every
quality gate applied in this experiment: avalanche, PractRand up to 16GB,
and all 27 "Good"-reliability dieharder tests. See `RESULTS.md` for the full
writeup.

## Not started (out of scope for this experiment, noted as future work)

- Rebalancing/repair of shift or rotation *amounts* beyond the untouched
  originals — not needed for this candidate (shift-repair was tried and
  found actively harmful, see above; the min_active_rots/min_active_taps
  floor constraint was what actually fixed the earlier traps).
- Operand-position changes (which state variable feeds which slot in the
  `a/b/o/c` update chain) — explicitly out of scope per the plan, proposed
  as a separate follow-up round.
- Optional TestU01/BigCrush wrapper — not run; PractRand 16GB + full
  dieharder "Good" battery were judged sufficient for this experiment's
  scope, and BigCrush was optional in the plan ("kalau kandidat final
  menjanjikan dan waktu memungkinkan").

## Follow-up micro-refactor: sliding-window TAP6/TAP7 load reuse — bit-identical, perf-neutral

After Step 2a concluded, checked `pruned_winner.c` for redundant ops/vars
that could be merged without changing output. Found one real (not
cosmetic) redundancy: in `ra_permutation_cycle`, `M` is `const` (never
written inside the function) and the loop counter `i` decrements by 1 each
step, so `idx7(i) = (i+7)&0xFF == idx6(i+1)` — the TAP7 array index this
iteration is always the TAP6 index from the iteration immediately before
it. One of the two per-iteration `M[]` reads is therefore provably
redundant and can be replaced by carrying the previous iteration's TAP6
load forward in a register, primed once with `M[6]` before the loop.

Implemented in `pruned_winner_refactored.c` (new file; `pruned_winner.c`
left untouched as the reference). Also dropped the dead `c = 0`
initialization before the loop (never read before being overwritten in the
first iteration) by scoping `c` inside the loop body.

- **Bit-identical**: confirmed via `--stream` output `cmp` across 4 seeds x
  3 lengths (1K, 12345, 300K outputs) + the default `TOTAL_RNG` path's
  `last_cons` — all identical, no exceptions.
- **Speed** (`perf stat -e instructions,cycles`, same build flags, 3 runs
  each):
  - instructions: 6,255,028,xxx → 6,054,243,xxx — a real, reproducible
    **~3.2% fewer instructions** (matches the expected ~1 fewer load per
    255-step cycle).
  - cycles: ~1.75B → ~1.75B — **no measurable difference** (within
    run-to-run noise).
  - wall time: ~0.43s → ~0.42s — **no measurable difference**.

**Conclusion: bit-identical, verified-safe, but perf-neutral in practice.**
The loop is latency-bound by the sequential `a`→`b`→`c`→`d` dependency
chain (each step needs the previous step's `c`/`d` before it can proceed),
not by load throughput — `M` is 1KB and stays L1-resident regardless, so
removing one of two L1-hit loads doesn't shorten the critical path. This
matches the precedent already recorded in `HANDOVER.md`: generic
instruction-count micro-optimizations on this algorithm tend to be
perf-neutral once the dependency chain, not raw instruction count, is the
bottleneck. Kept as a documented, validated finding rather than adopted as
the new "winner" — `pruned_winner.c` remains the reference candidate for
this experiment's verdict in `RESULTS.md` since the refactor changes
nothing measurable there.
