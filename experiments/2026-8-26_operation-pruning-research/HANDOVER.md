# Handover: automated operation-pruning search for ra_core, trading speed for a controlled quality cost

Written for a fresh Claude Code session (or the user) picking this up cold
in a later session. This is a **spec for a not-yet-started research task**
— nothing has been implemented yet, no code exists here. The user's own
words when requesting this write-up: "aku akan lanjut research khusus ini
di sesi lain... nanti aku akan mulai plan dari sana" (continuing this
research in another session, will start planning from this document).

## Context: why this idea exists

`ra_prng` (this repo) is a solo-researcher project: an array-based
deterministic PRNG (`ra_prng2`, 32-bit, stable/canonical; `ra_prng3`,
experimental). Published at CSAI2025; a follow-up targets ICCS2026
(`research/iccs2026_ra_prng/`).

A prior session (same day, 2026-08-26) spent significant effort trying to
make `ra_core` (`src/ra_prng2/c/ra_prng2.c`, `ra_prng2_thread.c`) faster
**without changing any output at all** (a hard constraint for that work,
since those files must stay bit-for-bit faithful to the CSAI2025/ICCS2026
papers). Three separate micro-optimization attempts were tried and
measured with `perf stat -e instructions,cycles` (never trust wall-clock
alone on this machine — it's noisy under load):

1. Reordering `ra_hash`'s inner loop for cache locality → **~3-7% slower**
   (reverted).
2. Splitting `ra_core` into `ra_init_state`/`ra_permutation_cycle`/
   `ra_reseed` for readability (commit `c4fc35c`) → perf-neutral (GCC
   fully re-inlines everything; a tiny ~1.3% instruction-count change
   turned out to be an incidental compiler-codegen side-effect, not a real
   speedup — cycles/wall-time were identical within noise).
3. A "fast path" that skips the per-step `count` truncation check when a
   full 255-step cycle is guaranteed not to truncate (the same pattern
   that measurably helped `ra_prng2_struct.c`'s `ra_prng_next()` by
   ~5-10%) → **also failed here**: instructions barely moved, and cycles
   were consistently *worse* across every repeated run, because the
   static code size grew ~19% (two inlined loop bodies instead of one),
   and that I-cache cost outweighed the tiny branch/decrement savings.
   Reverted; nothing was committed for this attempt.

Conclusion from that session: at the current algorithm's operation set,
the hot loop (an 8-wide shift-XOR read of `M`, a handful of `rot32` calls,
one 64-bit-multiply-based reduction) is already close to what `-O3
-march=native` can extract — there is no more free lunch from
*reorganizing* the existing operations. The only way left to get
meaningfully faster is to **do fewer operations** — which necessarily
means the output changes, so it cannot happen inside `ra_core` itself
(that file's whole job is to stay paper-faithful).

## Where this PRNG actually stands (baseline to beat / preserve)

Also gathered in that session, from files already in this repo — use these
as the quality/speed floor, don't re-derive them from scratch:

**Statistical quality** (`benchmarks/results/ra_prng2_original/`):
- Dieharder: 113 PASSED / 2 WEAK / **0 FAILED** (`dieharder_test.txt`)
- NIST STS: every category at/above the minimum pass rate (`NIST_STS_test.txt`)
- PractRand: **zero anomalies up to 128 GB** (`PractRand.txt`) — note this
  took ~14,586 seconds (~4 hours) of wall-clock for the 128GB tier alone;
  a search loop cannot afford to run this at full scale per candidate.
- TestU01 BigCrush (the hardest standard battery, ~160 tests): **"All
  tests were passed"** (`TestU01.txt` line 3779) — same tier as
  xoshiro256\*\*/PCG, which are the modern reference generators for
  "passes everything cleanly."

**Speed** (`benchmarks/comparisons/RNGing_speed/*.txt`, all `perf stat`,
same machine/flags):

| Generator | Throughput |
|---|---|
| xoshiro256** | 3574 MB/s |
| pcg32 | 3065 MB/s |
| philox | 1304 MB/s |
| **ra_prng2 (ours)** | **745.6 MB/s** |
| chacha20 (CSPRNG) | 555.5 MB/s |
| /dev/urandom | 389.6 MB/s |

So: quality is already at the same empirical tier as the best
non-cryptographic PRNGs, but speed is ~4-5x behind them. The margin on the
quality tests is *wide*, not razor-thin (clean passes, not marginal
p-values) — that margin is the budget this research is meant to spend.

## The idea (as the user described it, lightly organized — not yet a plan)

Build a **new, separate research pipeline** (not touching `src/ra_prng2/*`,
which must stay paper-exact) that:

1. Takes the current permutation-cycle math as a starting point (the
   `a`/`b`/`o`/`c`/`d` churn inside `ra_permutation_cycle` in the current
   `src/ra_prng2/c/ra_prng2.c`, post-`c4fc35c` — read that file for the
   exact current operation list: the 8-iteration shift-XOR loop building
   `o` from `M`, four `rot32` calls, one 64-bit multiply-based reduction
   for `d`, plus `ra_hash`'s own structure for reseeding).
2. Generates **candidate variants** that remove, merge, or cheapen some of
   those operations (e.g., shrink the 8-wide `o` loop to fewer taps,
   replace a `rot32` with a cheaper shift, drop terms from the XOR chain,
   simplify `ra_hash`).
3. For each candidate, **streams its `c` output** directly into a quality
   check — reuse the pattern (not necessarily the literal code) of the
   `FILE *raw_stream` mechanism just added to `ra_core` in
   `src/ra_prng2/c/ra_prng2.c` / `ra_prng2_thread.c` (commit `5ea01d9`):
   write raw `uint32_t` `c` values to a pipe/file so they can feed a
   statistical test tool without buffering the whole run in memory.
4. Uses **some kind of search/scoring procedure** to find the "lightest"
   (fewest operations / fastest) variant that still clears a chosen
   randomness-quality bar. The user explicitly was not sure what this
   procedure should be — they floated "regresi linear?" as a guess but
   framed it as an open question, not a decision. **Do not assume linear
   regression is the answer when planning starts** — treat this as the
   first real design question to resolve, e.g.:
   - Greedy/ablation search (try removing one operation at a time, keep
     the removal if quality survives, repeat) — simplest, likely first
     baseline.
   - Model operation-presence (or operation "dosage", e.g. shift-width) as
     features and a quality score as the target, e.g. linear regression
     or another statistical model, to *predict* which operations matter
     most before running expensive tests on every combination.
   - A proper multi-objective search (genetic algorithm / Pareto frontier
     between throughput and quality score) if the combinatorial space
     turns out too big for greedy ablation.

## Key open questions for the future planning session

These are unresolved on purpose — they're real design decisions, not
things to guess at now:

- **What's the cheap quality proxy for the search loop?** Full BigCrush or
  128GB PractRand per candidate is far too slow (hours per run). Likely
  needs something like: a small PractRand prefix (e.g. up to 1-2 GB, which
  only took tens of seconds per `PractRand.txt`), a fast subset of
  Dieharder, or a custom fast statistic — with the full suites reserved
  only for validating whatever the search converges on.
- **What quality bar counts as "still good enough"?** Not defined yet.
  Given the current margin is "passes everything cleanly," there's room to
  degrade somewhat, but by how much before it stops being trustworthy is
  a judgment call for that session.
- **Search space size / representation** — is an "operation" a discrete
  on/off choice (include the multiply-reduction or not), a continuous
  parameter (shift/rotate amounts, loop width), or both? This shapes which
  search method is even applicable.
- **Where does this code live?** Should be its own experiment directory
  (this one, or a sibling), producing new/separate C (or Python prototype)
  files — never edit `src/ra_prng2/c/ra_prng2.c` or `ra_prng2_thread.c` in
  place for this, since those must remain byte-for-byte paper-faithful
  (see the "no output change" constraint that governed the entire prior
  session). Mirror the structure of
  `experiments/2026-8-25_periodicity-heuristic-validation/` (its
  `HANDOVER.md` + `RESULTS.md` + throwaway driver scripts pattern) if a
  similar exploratory-with-checkpoints shape fits.

## What NOT to do

- Don't touch `src/ra_prng2/c/ra_prng2.c`, `ra_prng2_thread.c`, or
  `ra_prng2_struct.c` for this — those are the paper-exact, zero-output
  drift reference implementations validated all through the prior session
  (commits `5ea01d9`, `c4fc35c`). This research explicitly *will* change
  output — it needs its own sandbox.
- Don't re-attempt the three micro-optimizations listed above under
  "reorganize without changing operations" — already measured, already
  failed, don't redo the benchmarking from zero.
- Don't run full-scale PractRand/BigCrush inside a search loop — use them
  only to validate a final candidate, per the open question above.
