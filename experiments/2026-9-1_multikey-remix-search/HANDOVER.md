# Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey)

Written for a fresh Claude Code session (or the user) picking this up cold.
This is a **root-cause-diagnosed, fix-not-yet-designed** task. The defect
and its cause are conclusively established (see below); what's open is
finding a *cheap* mechanism that fixes it, not whether it's real.

## Context: where this came from

Direct follow-up from `experiments/2026-9-1_family-productionization/`
Tahap 2 (production validation battery for `ra_core.c`'s `ra_core_orbit` +
`ra_core_singleblock`). While testing `ra_core_singleblock`'s actual use
case — many different keys, each producing a short (<=255-word) block,
concatenated ("call-and-discard" / aggressive reinit) — PractRand found a
genuine, severity-escalating BCFN failure (FAIL at 4GB, `R=+40.7,
p=2.6e-21`) that does **not** appear in `ra_core_orbit`'s validated
single-key/long-stream use (128GB clean). See that folder's
`RESULTS_TAHAP2.md` for the full discovery narrative and the still-valid
Tahap 2 results for everything else (avalanche, speed, cross-correlation,
collision-scan, dieharder all PASS).

## Root cause (conclusively diagnosed this session — do not re-litigate)

Diagnostic chain, each step ruling something out (files still exist in
`../2026-9-1_family-productionization/`: `multikey_stream.c`,
`multikey_stream_hashed.c`, `multikey_stream_orbit.c`, and their
`practrand_*.log` results):

1. Linear key sequence (Weyl step `0x9E3779B9`) as cause → **ruled out**:
   SplitMix32-hashed key sequence (`multikey_stream_hashed.c`) failed
   identically.
2. Singleblock-specific code path (no `L[]`) as cause → **ruled out**:
   `multikey_stream_orbit.c` (same 255-word segments, but via
   `ra_core_orbit`, which DOES have `L[]`) failed with near-identical
   numbers.
3. Weak/affine-in-key init formula as sole cause → **ruled out** by two
   diagnostics in this folder:
   - `diag_paperlike_cycle.c`: current suspect init
     (`ra_init_state_orbit`'s affine formula) + `src/ra_prng2/c/ra_prng2.c`'s
     ORIGINAL paper-exact cycle (8-fold `o` XOR, extra `rot32` layers,
     multiply-high-bits `d`) → **clean to 8GB**. Same weak init, richer
     cycle, no failure.
   - `diag_hashed_init.c`: SAME init formula but with `key` pre-hashed via
     SplitMix32's finalizer, kept on the CURRENT production wired cycle →
     **still FAILS** at 4GB, nearly identical severity to baseline
     (`R=+41.9, p=6.4e-22` vs `R=+40.7, p=2.6e-21`). Hashing the key before
     the affine formula does nothing — rules out "just hash the key" as a
     fix.
4. **Confirmed mechanism** (`diag_warmup_discard.c` + `diag_warmup_depth.c`,
   user's hypothesis): the correlation is concentrated in the **first**
   255-step cycle, before any `ra_reseed` has run — `a=cons=key`,
   `b=(uint32_t)it=0` at the very start of every key's first cycle, and
   `M[]`/`L[]` are directly, affinely derived from `key`. `ra_core_orbit`'s
   128GB test is one key with billions of *post-reseed* cycles, so this
   first-cycle contamination is statistically invisible there — but
   `singleblock`/multikey concatenation measures *exactly* the first cycle,
   for thousands of different keys back to back, the worst possible angle.
   Proof: running the CURRENT production cycle + CURRENT suspect init
   unchanged, but inserting one extra 255-step cycle (output discarded)
   plus one `ra_reseed` *before* the first real output, per key →
   **clean to 8GB**, no formula change at all.
5. **Minimum sufficient warm-up depth**, scanned in this session
   (`diag_warmup_depth.c`, depth = steps run+discarded before reseed):

   | depth | result (up to 8GB) |
   |---|---|
   | 8, 16, 32 | FAIL |
   | 64 | grows to "very suspicious" at 8GB — not safe |
   | 128 | clean to 8GB |
   | 255 (full cycle) | clean to 8GB |

6. **Why a warm-up cycle is not an acceptable fix as-is** (`bench_warmup_cost.c`,
   `bench_warmup_cost2.c`): the warm-up is a **fixed cost per reinit**,
   independent of how many words are actually wanted — brutal at small K
   (words per key), which is exactly `singleblock`'s headline use case:

   | K | overhead, depth=128 | overhead, depth=255 |
   |---|---|---|
   | 1 | 6.94x | 12.38x |
   | 10 | 5.53x | 9.60x |
   | 100 | 2.15x | 3.19x |
   | 255 | 1.53x | 1.96x |

   User's own diagnosis of *why* it's wasteful: the warm-up cycle's only
   useful side effect is shuffling `L[]` via the `L[i]<->L[d]` swaps (the
   cycle never touches `M[]` at all — `M` is `const` inside
   `ra_permutation_cycle_orbit`). Everything else the warm-up computes
   (`a`,`b`,`c` chain, `rot32`, the `o` term, the discarded output) is pure
   waste — paid for, then thrown away, just to get `d` values to drive the
   `L` swaps.

## Candidates tried so far

1. **Per-element `fmix32` (MurmurHash3 finalizer)** on every `L[i]`/`M[i]`
   after `rot32` (`diag_init_avalanche.c`) — **PASSES**, clean to 8GB.
   Cost: O(256) `fmix32` calls per reinit. Overhead: 2.25x (K=1), 1.89x
   (K=10), 1.20x (K=100), 1.11x (K=255) — see `bench_init_avalanche.c`.
2. **`fmix32` on only the per-key additive term** (`keyterm_l =
   fmix32(0x9e3779b7u*key)`, `keyterm_m = fmix32(0x06a0dd9bu*key)`,
   computed ONCE per reinit and reused across all 256 elements — cheaper,
   only 2 `fmix32` calls total instead of 256) (`diag_init_keyterm_avalanche.c`)
   — **FAILS**: "mildly suspicious" at 4GB (`R=+10.1`), grows to FAIL at
   8GB (`R=+22.5, p=1.5e-11`). Weaker/later-onset than the unfixed baseline
   (FAIL already at 4GB, `R=+40.7`) but does not clear the bar. Confirms
   the mechanism this candidate leaves untouched — the fixed per-step
   increment `i*C` and rotation-by-`key^i` progression, which give every
   key's `M[i]`-to-`M[i+1]` relationship the SAME shape, only shifted by a
   (now-hashed) global offset — is enough on its own to leak through the
   first cycle. Hashing only the global offset (this candidate, and
   `diag_hashed_init.c` before it) delays/weakens the leak but does not
   eliminate it; disrupting the PER-ELEMENT relationship (candidate 1) is
   what actually works. **Do not retry "hash a scalar term once" variants
   without also changing the per-index step/rotation shape** — this
   pattern is now confirmed insufficient twice.
3. **Same per-key hashed term, combined by XOR instead of `+`**
   (`l_val = (i*C1) ^ keyterm_l`) (`diag_init_keyterm_xor.c`) — **FAILS**,
   FAIL at 4GB (`R=+29.5, p=2.6e-15`). Weaker than the `+`-combine variant
   (2) but still fails for the same reason: XOR-ing in a per-key constant
   doesn't touch the per-index step shape either.
4. **Same per-key hashed term, combined by MULTIPLY instead of `+`**
   (`l_val = (i*C1) * keyterm_l`, `m_val = (i*C2) * keyterm_m`)
   (`diag_init_keyterm_mul.c`) — **PASSES, and is the current best
   candidate.** Clean through 32GB (`practrand_init_keyterm_mul_diag.log`
   to 8GB, `_32g.log` to 32GB — 0 anomalies at every tier). Cost: still
   just 2 `fmix32` calls per reinit (same as candidates 2/3) — measured
   overhead is **~1.00-1.01x, i.e. free within noise**
   (`bench_init_keyterm_mul.c`: K=1 1.01x, K=10 0.98x, K=100 0.99x, K=255
   1.01x). Why multiply works where `+`/`^` don't: `l_val(i) = i*C1*keyterm_l
   = i*(C1*keyterm_l)` — algebraically still linear in `i` for a fixed key,
   but the per-key STEP SIZE itself (`C1*keyterm_l`) is now a
   well-distributed pseudorandom value unique to that key, not the same
   fixed `C1` shared by every key (which is what additive/XOR-offset
   variants left unchanged). Different keys' `M[i]`-to-`M[i+1]` deltas no
   longer share a common shape, which appears to be what the first-cycle
   BCFN leak was actually keying off. **This is the current leading fix
   candidate — go here first for anything building on this investigation.**

## The task: find something cheaper than a full/partial warm-up cycle

**User's explicit priority (2026-09-01): fix the INIT formula first** — so
that `L[]`/`M[]` coming out of `ra_init_state_*` are no longer so directly
affine-in-key that the first cycle leaks key structure — rather than adding
a runtime pre-mixing step. **Do not drop cycle-side ideas either**: the user
noted that once the init changes, a lighter/cheaper *cycle* configuration
might become sufficient where it wasn't before (analogous to how the
operand-position-search / operation-pruning line of work searched cycle
configurations) — a combined init+cycle redesign is in scope, not just an
init-only patch.

Concretely, open design questions for this folder (answer via experiment,
not assumption — and see the check-in note below):

- Can `ra_init_state_orbit`/`_singleblock` produce `L[]`/`M[]` that are
  well-mixed per-key (not affine in `key`) using roughly the same or fewer
  arithmetic operations as today's formula (`rot32(i*C + C*key, key^i)`),
  so there's no separate runtime cost at all — the fix is free, baked into
  init?
- If a init-only fix isn't enough on its own, what's the *cheapest*
  additional operation (not a full 255-step cycle) that still adequately
  decorrelates the first cycle's output across many keys? E.g., is a much
  shorter, PURPOSE-BUILT mixing pass (not a reused/discarded real cycle)
  enough — something that costs O(1) or O(small constant), not O(255)?
- Does changing the init formula allow *removing* operations from the
  cycle elsewhere (offsetting any added init cost) — i.e. is there a new,
  cheaper (init, cycle) pair jointly, not just a patch on top of the
  existing pair?

## Verification bar for any candidate

Any candidate must be checked against the exact failure mode that exposed
this, not just avalanche/single-key PractRand (which the current formula
already passes):

1. Multikey/aggressive-reinit PractRand, same shape as
   `../2026-9-1_family-productionization/multikey_stream.c` (many keys,
   Weyl-stepped, <=255-word blocks per key, concatenated) — must be clean
   to at least 8GB (match the depth-128/depth-255/paper-like-cycle bar
   already established), ideally pushed further once a promising candidate
   exists.
2. Must not regress `ra_core_orbit`'s existing single-key guarantees
   (128GB PractRand, cross-correlation, collision-scan, dieharder) if the
   candidate touches shared code (`ra_init_state_orbit`,
   `ra_permutation_cycle_orbit`) — bit-identity or full re-validation,
   whichever applies.
3. Speed: benchmark like `bench_warmup_cost.c`/`bench_warmup_cost2.c`
   (fixed total words, sweep K = 1/10/100/255, compare
   baseline-cost-per-word) — a candidate that "fixes" PractRand but keeps
   the 5-13x fixed-cost overhead at small K has not actually solved the
   problem the user cares about.

## Process note (per explicit user feedback this session)

The user asked to be checked in with more often on design decisions during
this kind of investigation — after each diagnostic result that reveals a
new tradeoff or fork, not only once a full battery of results is ready.
Concretely: before committing time to a specific candidate mechanism
(especially anything that touches the init formula shape or adds new
constants), surface the idea and initial rationale for confirmation before
running the full verification bar above.

## Non-goals

- Not modifying `ra_core.c` (production file) directly until a fix is
  validated here and confirmed with the user — all work in this folder is
  diagnostic/throwaway `.c` files, consistent with repo convention.
- Not touching `src/ra_prng2/c/*` (paper-exact reference, read-only).
- Not re-opening the already-closed Tahap 1/Tahap 2 results in
  `../2026-9-1_family-productionization/` except to read them for context.
- Not resuming the "shuffle implementation test" or `src/` promotion
  questions parked in that folder's `RESULTS_TAHAP2.md` — out of scope
  here.

## Non-regression verification results (2026-09-01) -- verification bar item 2 DONE

Candidate 4 (`ra_init_orbit`, multiply-combined keyterm) checked against
`ra_core_orbit_affine`'s existing single-key/long-stream guarantees, via a
new harness `diag_orbit_regression.c` (unbounded-stream, reseeding wrapper
`ra_core_orbit_stream` -- copy of `ra_core.c`'s `ra_core_orbit_affine` loop
structure, calling `ra_init_orbit` instead of `ra_init_state_orbit_affine`;
`ra_permutation_cycle_orbit`/`rot32`/`fmix32` unchanged). Harness fidelity
sanity-checked first: swapping the OLD affine formula into this same
harness reproduced `ra_core.c --stream orbit <key> <n>` byte-identical for
7 (key, n) pairs including n>255 (exercises the reseed loop) -- confirms
any difference below measures the init formula only, not a harness bug.
Run locally (not VPS -- estimated ~25-30 min from the 32GB multikey log's
scaling, confirmed accurate), key=12345 throughout:

| Axis | Result | vs. baseline (`winner_wired_addressable`/`ra_core_orbit_affine`) |
|---|---|---|
| PractRand, single-key, 128GB | 0 anomalies at every checkpoint (1G/2G/4G/8G/16G/32G/64G/128G) | matches `practrand_winner_wired_128GB.txt` |
| Cross-correlation (`TIERS_Q1A` full: K=128, n=1,000,000) | 0/8128 flagged pairs, both adjacent and control groups | matches `tahap3_cross_correlation_results_full.json` |
| Collision-scan (`TIERS_Q2` full sequential: M=10,000, V=250,000) | 0 collisions, prefix (10,000 fingerprints) and blocksweep (9.8M blocks) | matches `tahap3_collision_scan_results_sequential_full.json` |
| Dieharder, 27-test "Good" battery | 26/27 PASSED, 0 WEAK, 0 FAILED (`-d 200` empty output is a known pre-existing invocation artifact, present identically in every dieharder log this repo has produced this way -- not candidate-specific) | matches/exceeds `winner_wired_addressable`'s own 25/27 PASSED, 1 WEAK, 0 FAILED (RESULTS.md, `2026-9-1_dieharder-battery/`) |

**Conclusion: no regression on any of the 4 axes.** Verification bar item
2 (HANDOVER.md above) is now satisfied, alongside item 1 (multikey PractRand,
already clean to 32GB) and item 3 (speed, already ~1.00-1.01x overhead).
All three verification-bar items are DONE.

Files: `diag_orbit_regression.c` (this folder),
`practrand_orbit_regression_128g.log` (this folder),
`cross_correlation_orbit_regression.py` + `cross_correlation_orbit_regression_results_full.json`
and `collision_scan_orbit_regression.py` + `collision_scan_orbit_regression_results_sequential_full.json`
(both in `../2026-8-29_parallelization-research/`),
`dieharder_orbit_regression_piped.txt` (`../2026-9-1_family-productionization/`,
reusing `run_dieharder_battery_multikey.py` unmodified).

**Not run this pass** (explicitly out of scope, see plan notes): collision-scan's
random-seed 500k-key tier (`TIERS_Q2_RANDOM`) -- that is the deep one-off
validation already done for `winner_wired_addressable` itself, not part of
the minimal verification bar.

**Next (separate session, not started)**: apply `ra_init_orbit`/
`ra_init_singleblock` back into `ra_core.c` (replacing `ra_init_state_orbit_affine`/
`ra_init_state_singleblock_affine`), then run the further battery the user
requested (speed, randomness, avalanche, shuffle-implementation) on three
specific usage patterns: `ra_core_singleblock` at K=1, `ra_core_singleblock`
at K=255, and the orbit stream function. Needs: (a) reconciling the
`ra_core_orbit` (1-block, no reseed) vs `ra_core_orbit_stream` (unbounded,
this session's addition) naming -- which one the promoted `ra_core.c`
should call `ra_core_orbit`; (b) fixing `bench_ra_core.c` and
`ra_core_singleblock_cli.c`, both stale since the `_affine` rename
(`extern ra_core_orbit`/`ra_core_singleblock` no longer exist); (c) a new
`ra_init_singleblock`-based core+cycle wrapper (the M-only init exists,
unwired); (d) designing the shuffle-implementation test for the K=1/K=255
cases specifically, no direct precedent for that combination yet.
