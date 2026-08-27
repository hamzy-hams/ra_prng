# Status / handover for resuming this experiment in a new session

Written 2026-08-26 so a **fresh Claude Code session** (no memory of the
conversation that did this work) can pick up exactly where things stand,
without the user having to re-explain anything. Read `HANDOVER.md` first for
the original task spec, and `RESULTS.md` for everything already written up.
This file is just "what's live right now and what to do next."

## PAUSED 2026-08-27 ~10:00 WIB - handoff for continuing on ANOTHER DEVICE

The user intentionally stopped the search here to move to more
cost-effective research. The process was **not** killed abruptly - it was
sent `SIGTERM`, which triggers the checkpoint-then-exit handler cleanly
(confirmed via the `interrupted at seed=0 - checkpoint saved to
.cycle_measure_ckpt_n8_w4_rows8_seed0.bin` line in
`run_n8_w4_rows8.err`). **No progress was lost.** This section is a
self-contained snapshot so someone with *no context on this conversation*,
possibly on a completely different machine, can resume it.

### Final state snapshot (last checkpoint before pausing)

Decoded from `.cycle_measure_ckpt_n8_w4_rows8_seed0.bin` (64-byte header,
format `'<IIiiiIi4xQQQQ'`, fields `magic,version,n,w,rows,seed,phase,lam,power,k,mu`):

```
phase=0, lam=307,244,786,114, power=549,755,813,888 (2^39), k=0, mu=0
```

- `total_steps = (power-1) + lam = 857,000,600,001` (cumulative Brent-search
  steps across all epoch-doublings so far - see `speed_log.csv` for the
  full timestamped history and the "total_steps" note further down).
- Already **~1865x above** `λ_pred = 0.7824*sqrt(|S|) = 1.647e8` for this
  config, with no cycle closure yet - see "Why this run exists" below and
  the conversation's periodicity-heuristic discussion for what that means.
- Net compute time invested so far: **~22.7 CPU-hours** (estimated from
  `total_steps / ~10.5M steps/sec` average throughput; actual wall-clock
  span is longer because the process was paused/resumed several times
  during this session without losing progress).
- `run_n8_w4_rows8.out` still has **no CSV result line** (`seed,lambda,mu`)
  - seed 0 has not finished. Seeds 1-2 (of the `seed_start=0 seed_count=3`
    invocation) haven't started yet.

### Files needed to continue on a different device

Copy this entire directory (`experiments/2026-8-25_periodicity-heuristic-validation/`),
or at minimum these files:

- `cycle_measure.c` - the source (**do not** copy the compiled `cycle_measure`
  binary across devices - see "Compiling" below, it won't portably run).
- `.cycle_measure_ckpt_n8_w4_rows8_seed0.bin` - the checkpoint (208 bytes:
  64-byte header + tortoise state + hare state). **This is the only file
  that actually matters for not losing progress** - everything else is
  regenerable or just logs/history.
- `run_n8_w4_rows8.out` / `run_n8_w4_rows8.err` - historical logs (not
  required for resuming, just useful context/continuity).
- `speed_log.csv` - throughput history (not required, just continuity).
- `toy_prng.py` - the Python reference implementation, needed if
  re-running any of the enumeration/analysis scripts (not needed just to
  resume `cycle_measure.c`, which is a standalone C program).

### Compiling on the new device

```bash
cd experiments/2026-8-25_periodicity-heuristic-validation
gcc -O3 -march=native -std=gnu17 cycle_measure.c -o cycle_measure
```

(Built and tested with `gcc 16.1.1` on Arch Linux; any reasonably recent
gcc/clang with C17 support should work.) **`-march=native` means the
binary is NOT portable across CPUs** - always recompile from source on
whichever machine will run it, never copy the `cycle_measure` binary
itself between devices (it may `SIGILL` on a CPU lacking the instructions
the original machine's `-march=native` selected). If unsure the target
CPU supports whatever `-march=native` picks, drop that flag (`gcc -O3
-std=gnu17 cycle_measure.c -o cycle_measure`) for a safe generic build -
slower, but correctness is unaffected either way (checkpoint format and
`next_state()` logic don't depend on `-march`).

### Resuming the search

Identical command line to the original invocation - it auto-detects the
checkpoint and picks up mid-phase, no new flags needed:

```bash
cd experiments/2026-8-25_periodicity-heuristic-validation
nohup ./cycle_measure 8 4 8 0 3 >> run_n8_w4_rows8.out 2>> run_n8_w4_rows8.err &
disown
```

(`>>` appends rather than truncates, preserving history if the old log
files were copied over too; use `>` instead if starting fresh logs.)
`run_n8_w4_rows8.err` will show a `resumed seed=0 from checkpoint:
phase=0 lam=307244786114 power=549755813888 ...` line confirming it
picked up exactly where it left off. A plain `kill <pid>` (SIGTERM, **not**
`-9`) is safe to pause it again at any point - checkpoints within a
fraction of a second, or at most ~30s via the periodic autosave.

### Stop criterion already agreed with the user (still applies)

Don't wait for exact cycle closure - it's not a bounded wait (see the
image-fraction extrapolation discussion: even an optimistic ceiling
estimate implies **1.7-4+ years** of continued compute at current speed,
and there's no guarantee λ approaches that ceiling at all). Instead:

1. Let it run until `power` reaches **`2^40 = 1,099,511,627,776`** without
   a closed cycle (currently at `2^39`, ~55.9% through as of the pause
   point above - roughly another ~865 billion steps, ~22-24 CPU-hours at
   current throughput, to reach that epoch boundary).
2. At that point, **stop and treat `λ > 2^40` as a lower-bound result**,
   not a final measurement.
3. Audit `next_state()` / `cycle_measure.c`'s Brent-search logic
   independently before trusting the deviation as a genuine finding
   (cross-check against `enumerate_n2w4.py` / `enumerate_n4w4.py`'s
   exhaustive in-degree results, which already show the same qualitative
   pattern - see below - so this is likely a real structural property, not
   a measurement bug, but hasn't been independently re-verified by a fresh
   pair of eyes yet).
4. If the audit comes back clean, write up the `rows=8` section in
   `RESULTS.md` per "What to do once rows=8 finishes" below - this was
   explicitly **not** done automatically and still needs to happen with
   (or be reviewed by) a human.

## Why this run exists / what it's testing

Quick version (full reasoning is in `RESULTS.md`'s "Verdict" section and
the conversation this file can't see): the initial sweep measured `n=8,w=4`
only at the toy model's default hash-reduction depth (`rows=4`), where
measured `λ=6318640` came out ~4300x *smaller* than the paper's predicted
`λ_pred≈1.647e8`. The suspicion was that this toy model's necessarily
shallow `ra_hash` reduction (`rows=4` here vs. `rows=32` in the real
256-word algorithm) is a confound, not evidence against the heuristic
itself. So a follow-up varies `rows` independently of `(n,w)` for a couple
of configs to see whether deepening it moves measured `λ` toward (or past)
`λ_pred`.

Data gathered so far for this follow-up (all in this directory, already
final, don't re-run):

| config | rows | file | result |
|---|---|---|---|
| n=8,w=4 | 2 | `results_n8_w4_rows2.csv` | λ=131824 constant, 300 seeds |
| n=8,w=4 | 4 (baseline) | `results_n8_w4.csv` | λ=6318640 constant, 100 seeds |
| n=8,w=4 | 8 | **pending - the run described above** | - |
| n=4,w=8 | 2 | `results_n4_w8_rows2.csv` | 500 seeds |
| n=4,w=8 | 4 (baseline) | `results_n4_w8.csv` | 100 seeds |
| n=4,w=4 | 2 | `results_n4_w4_rows2.csv` | 500 seeds |
| n=4,w=4 | 4 (baseline) | `results_n4_w4.csv` | 500 seeds |

For `n=8,w=4`: `log2|S|=55.3` (identical across all three `rows` values -
only the internal mixing structure changes, not the state space size), so
`λ_pred=1.647e8` is the same target for all three rows. The `rows=2→4`
jump alone moved measured λ by a **47.93x** ratio (131824 → 6318640). A
naive geometric extrapolation of that same ratio to `rows=4→8` predicts
`λ(rows=8) ≈ 3.0×10^8` - notably *close to and slightly above* `λ_pred`
(ratio 1.84x), which would be a striking result if it holds: it would mean
the shallow-hash confound really was suppressing λ, and a properly deep
hash pushes this deterministic generator's cycle length *past* the
"random mapping" baseline, not just toward it.

**This extrapolation is a guess, not a result** - the actual `rows=8`
measurement is the whole point of the pending run.

## What to do once rows=8 finishes

1. Confirm the CSV line(s) in `run_n8_w4_rows8.out`, save them to
   `results_n8_w4_rows8.csv` (same `seed,lambda,mu` format as the other
   `results_*.csv` files - just copy the header + lines over).
2. Compare the actual λ against both `λ_pred=1.647e8` and the `~3.0e8`
   extrapolation above.
3. Add a new section to `RESULTS.md`, after the current "Verdict" (rename
   that one to "Verdict (initial sweep)" to preserve it as a record - don't
   delete it). The new section should cover:
   - A comparison table for `n=8,w=4` across `rows=2/4/8` (same `|S|`, very
     different λ - the headline finding: internal hash depth dominates
     over state-space size alone as a predictor of λ).
   - Why this matters conceptually: an avalanche/diffusion test (as used
     in the initial sweep's sanity check) only measures *local* bit
     sensitivity from one evaluation of `F` - it says nothing about `F`'s
     *global* behavior as a mapping over the whole state space (in-degree/
     collision structure), which is what the `√|S|` heuristic actually
     assumes. The extreme λ concentration at `rows=2` (every seed landing
     on the *same* λ=131824) is a symptom of structural collision, not of
     randomness - despite the toy model passing its avalanche check.
   - Frame "pure random mapping" as a **middle baseline**, not an upper
     bound: by the birthday-paradox reasoning the `√|S|` heuristic itself
     is built on, a genuinely random function collides (and thus cycles)
     faster than naive intuition suggests. A shallow/poor deterministic
     hash can do *worse* than that baseline (collapse into small loops -
     what `rows=2` shows), while a sufficiently deep/good one can do
     *better* than it (explore closer to the full state space before
     repeating) - which is the interesting possibility `rows=8` is
     testing.
   - Also fold in the `n=4,w=8,rows=2` vs. baseline and `n=4,w=4,rows=2`
     vs. baseline comparisons (data already collected, listed in the table
     above) as corroborating evidence for the same "hash depth matters
     independently of |S|" point.
   - A short methodological note that `cycle_measure.c` now supports
     checkpoint/resume (see below) - relevant because this specific run
     needed many hours and the machine isn't always kept running.
   - An updated final verdict along these lines (adjust to what the actual
     number says): the `√|S|` heuristic is conceptually defensible as a
     *middle baseline* (shallow hashing measurably falls below it, deep
     hashing plausibly approaches or exceeds it), but it **fails as a
     standalone predictor from `|S|` alone**, since at this scale the
     internal mixing/hash structure (`rows`) changes measured λ by 4+
     orders of magnitude while `|S|` stays fixed.
4. Report the finished numbers to the user (in Indonesian, per their
   working language for this project) and offer the next step that was
   proposed but not yet started: full enumeration of the `n=2,w=4` state
   space (`|S|≈131072`, small enough to brute-force completely) to compute
   `F`'s true in-degree/preimage-count distribution and chi-square-test it
   against the Poisson(1) distribution the random-mapping heuristic
   assumes - a more direct test of the heuristic's core assumption than
   sampling cycle lengths. Don't start it unprompted; it was offered as a
   question, not agreed to yet.

## Checkpoint/resume (added this session, why it matters here)

`cycle_measure.c` did **not** originally support checkpointing. The first
attempt to measure `n=8,w=4,rows=8` ran for ~12 CPU-hours with zero output
(Brent's algorithm gives no intermediate result until the cycle closes),
had to be killed so the machine could sleep, and **all progress was lost**
- there was no way to resume, only restart from zero.

Checkpoint/resume was then added specifically so this wouldn't happen
again:

- `SIGTERM`/`SIGINT` (a plain `kill`, or Ctrl-C) now trigger an immediate
  checkpoint + clean exit (exit code 42). An unconditional autosave every
  ~30 wall-clock seconds also protects against `SIGKILL`/power loss, which
  can't be caught.
- Checkpoints are per-seed:
  `.cycle_measure_ckpt_n{n}_w{w}_rows{rows}_seed{seed}.bin` in the working
  directory, covering all three phases of Brent's algorithm (search,
  reset, final `mu`-search).
- **Resuming is just re-running the identical command line** - no new
  flags. It auto-detects a matching checkpoint and picks up mid-phase.
- Scope note: only the *in-progress* seed's checkpoint is tracked. In a
  multi-seed invocation, if seed 0 already finished and seed 1 gets
  interrupted, resuming will recompute seed 0 from scratch (fast, since it
  already succeeded once) rather than skip it. This was a deliberate,
  user-confirmed scope choice, not an oversight.
- A real bug was found and fixed during verification: the reset phase
  (phase 1 of 3) originally saved its loop counter to the checkpoint
  *before* the `for`-loop's implicit increment ran, causing an off-by-one
  that made the resumed computation's tail-search (`mu`) loop **never
  terminate**. Fixed and re-verified (interrupt+resume tested in all three
  Brent phases, plus a multi-seed interruption scenario, all matching
  known-correct results bit-for-bit). If you're auditing `cycle_measure.c`
  and see the comment above the phase-1 loop explaining this, that's why
  it's there - don't "simplify" it back to a `for`-loop without re-reading
  that comment.

## Live checkpoint note (added 2026-08-26 afternoon, mid-run of rows=8)

Written while `n=8,w=4,rows=8,seed=0` was still running (not finished) so
this run can keep going unattended while other research work is paused.
Snapshot as of **2026-08-26 15:56 WIB**:

- **Process**: PID 31795, `./cycle_measure 8 4 8 0 3`, elapsed 5h15m,
  state `RN` (actively running, not hung).
- **Live checkpoint state** (`.cycle_measure_ckpt_n8_w4_rows8_seed0.bin`,
  decode with `python3 -c "import struct; ..."` using format
  `'<IIiiiIi4xQQQQ'` on the first 64 bytes - fields
  `magic,version,n,w,rows,seed,phase,lam,power,k,mu`, note the `4x`
  padding before the `uint64_t`s, easy to get wrong):
  `phase=0, lam=76093446530, power=137438953472 (2^37), k=0, mu=0`.
  Still in Brent's doubling-search phase, 55% through the `2^37` epoch,
  no cycle found yet.
- **Proven lower bound on λ so far**: `> 2^36 ≈ 6.87e10` (search has
  passed that epoch without a match; will keep ratcheting up as `power`
  doubles again - `2^38` next, etc). This is already **~417x above**
  `λ_pred=1.647e8` for this config and climbing.
- **Build**: binary was recompiled after two small changes this
  session, both already applied to `cycle_measure.c` in the working
  tree (uncommitted, like everything else in this experiment dir):
  1. Added a progress log line to stderr every 30 min
     (`progress seed=... phase=... lam=... power=... k=... mu=...
     elapsed_this_run=...`) inside `MAYBE_CHECKPOINT()` - purely
     additive, does not touch the checkpoint format or `next_state()`.
  2. Cleaned two compiler warnings (`-Wall -Wextra` now clean): removed
     unused `#include <math.h>`, reworded a comment that had `/*`
     inside a block comment. Also cosmetic, no logic change.
  Compiled with `gcc -O3 -march=native -std=gnu17 cycle_measure.c -o
  cycle_measure` (gcc 16.1.1). Binary and source both currently
  untracked in git (`?? cycle_measure`, `?? cycle_measure.c`) - **do
  not `git add`/commit the binary**, only the `.c` if/when this work
  gets committed.
  **The process was killed (SIGTERM) and resumed from checkpoint twice
  during this session to apply these two changes** - both times
  verified clean via the `resumed seed=0 from checkpoint: ...` log line
  showing `lam`/`power` continuing forward, not resetting. Checkpoint
  format itself is unchanged since the version that's already running,
  so no further rebuild/restart is needed for the run to keep going -
  it can now just be left alone (or killed/resumed again safely if
  needed) until it finishes.
- **Throughput**: steady in a **~9.7-11.3 million lam-steps/sec** band
  (noisy - depends on how much of the host CPU this process actually
  gets; there was an earlier period, before this note, where it dropped
  to ~470K/s for over an hour with no explanation found, then recovered
  - see `speed_log.csv` for the full timestamped history, columns
  `unix_ts,human_time,phase,lam,power,total_steps`, where `total_steps
  = (power-1)+lam` is the cumulative Brent-search step count, robust
  across `power`-doubling resets of raw `lam`). A 30-min cron-style
  polling loop (re-armed manually each session since `CronCreate` jobs
  are session-only, not persisted) has been appending to this file.
- **Sanity ceiling, for context**: `|S| = 2^55.3 ≈ 4.44e16` states is
  the absolute structural upper bound on λ for this config (the whole
  reachable state space). At current speed that would take
  `4.44e16 / 1e7 ≈ 4.44e9 s ≈ 140 years` to reach if λ ever got that
  close - which it structurally cannot, because the update map is
  non-invertible (the `cons_{t+1}=H(M_t)` hash-fold is many-to-one per
  the paper's own "Non-invertible structure" section), so a full-space
  single cycle is impossible. This number is a worst-case sanity bound,
  not a projected finish time - don't panic if it's still running next
  time this is picked up, and don't extrapolate a completion ETA from
  it.
- **f(2)/f(4)/f(8) pattern observed this session** (worth carrying into
  the eventual `RESULTS.md` rows=8 writeup): `f(2)=131824≈2^17.0`,
  `f(4)=6318640≈2^22.6` (+5.6 bits over f(2)), and a *naive* linear
  extrapolation (+5.6 bits again) would predict `f(8)≈2^28.2≈3e8` -
  **already disproven**, actual is `>2^36`, i.e. **>13.4 bits** of
  jump, more than double the previous bit-jump. Working theory: `rows`
  for `n=8` is gated by `G=n/rows`, so `rows=8` forces `G=1` (single
  fully-mixed hash accumulator) vs `rows=4`'s `G=2` (two partial,
  lossier lanes) - `rows=4→8` isn't "a bit deeper," it's crossing from
  partial/lossy mixing to (near-)total mixing in one jump, plausibly a
  genuine phase transition in the functional graph's collision
  structure rather than a point on a smooth curve. **Do not
  interpolate f(8) from f(2)/f(4) with a power law** - it's already
  shown to be wrong by over an order of magnitude in exponent terms.

## Everything else already done (don't redo)

- `toy_prng.py`, `cycle_measure.py`, `cycle_measure.c`, `run_sweep.py`,
  `avalanche_check.py` - all implemented, tested, and described in
  `RESULTS.md`'s "Design decisions" and "Avalanche sanity check" sections.
- The full initial sweep (5 configs, default `rows`) - done, written up.
- The `rows` follow-up's fast configs (everything in the table above
  except `n=8,w=4,rows=8`) - done. Only the one slow run is outstanding.
- **Full state-space enumeration + in-degree/Poisson(1) chi-square test**
  (added 2026-08-27, not yet folded into `RESULTS.md` - do that alongside
  the `rows=8` writeup, not separately): `enumerate_n2w4.py` and
  `enumerate_n4w4.py` brute-force every reachable state for `n=2,w=4`
  (|S|=131,072) and `n=4,w=4` (|S|=402,653,184, default `rows` -> `G=1`
  for both, same "shallow fold" regime as the running `n=8,rows=8`) and
  compute `next_state`'s *exact* in-degree (preimage-count) distribution,
  chi-square-tested against Poisson(1) - the distribution a true random
  self-map would produce, and the assumption the paper's
  `λ≈0.7824√|S|` heuristic implicitly relies on. Results (saved in
  `enumerate_n2w4_result.json` / `enumerate_n4w4_result.json`):
  - n=2: only 3.32% of `S` is ever reached as output; in-degree takes only
    3 distinct values (16, 128, 144); chi-square vs Poisson(1):
    statistic=445,142, p≈0 - rejects decisively.
  - n=4: only 2.39% of `S` is ever reached; in-degree takes 8 distinct
    values (16-208); chi-square statistic=1,026,542,729, p≈0 - rejects
    even harder than n=2.
  - Extra finding, independently verified (not a bug - `cons` does affect
    individual outputs, checked separately): at n=4, the *image set* of
    `F` restricted to any single `cons` value is **exactly identical**
    across every `cons` tested (0, 7, 15, and per the full run log,
    `cons=1..15` contribute zero new distinct points beyond `cons=0`) -
    F collapses onto the same fixed ~9.6M-point attractor regardless of
    starting `cons`.
  - `enumerate_n4w4.py`'s vectorized `next_state` (numpy) was cross-checked
    against the scalar `toy_prng.py` reference on thousands of random
    samples before trusting the full run - see `validate()` in that file
    if extending it further (e.g. to test whether a *deeper* fold, more
    representative of the real `ra_prng2`'s `G=8,rows=32`, converges
    toward Poisson(1) - this hasn't been tested at any `n` yet, since no
    toy `n` in this experiment reaches `rows=32`; `n=32` would be the
    smallest that could).
  - Bottom line these results support: passing local avalanche/diffusion
    tests (`avalanche_check.py` passes for this toy model) does **not**
    imply `F` behaves like a random mapping globally - the paper's
    Section III-A1 uses "passes NIST/Dieharder/BigCrush" as evidence for
    "consistent with the random mapping theorem," and this is now a
    concrete counterexample to that specific inferential step, in a
    structurally faithful (if smaller) replica of the same non-invertible
    hash-fold design.
