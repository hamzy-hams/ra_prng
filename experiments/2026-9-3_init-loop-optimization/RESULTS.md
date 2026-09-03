# RESULTS: Priority 1 — Init-loop speed optimization

Status: **Priority 1 executed and complete.** Priority 2/3 NOT started —
this document ends at the checkpoint `HANDOVER.md` itself requires
("significant" speedup must be pinned down with the user before deciding
whether to proceed).

`git diff --stat -- ../2026-9-1_family-productionization/ra_core.c` was
empty at the start of this session and remains empty now — `ra_core.c` was
never modified, per the hard constraint in `HANDOVER.md`. All work happened
in this folder's own files.

## Candidate implemented

One candidate, per the "concrete starting hypothesis" in `HANDOVER.md`:
replace the per-iteration `l_val = (uint32_t)(i*A) * keyterm` (two
multiplies per iteration: one to scale `i` by `A`, one to combine with the
per-call `keyterm`) with a precomputed `step = A*keyterm` and a running
accumulator `l_val`, updated by a single `+= step` per iteration instead.
This is an exact ring-arithmetic identity in `Z/2^32Z`
(`(i*A mod 2^32)*B mod 2^32 == i*(A*B mod 2^32) mod 2^32`, multiplication
associativity), valid as long as `i` increases strictly by 1 every
iteration (unchanged from the original loop). Applied to both `l_val`
(orbit only) and `m_val` (orbit + singleblock). `rot32(val, r)` itself is
untouched — this candidate only changes how the pre-rotate value is
produced, not the rotate.

Implementation: `diag_init_candidates.c` (self-contained diagnostic, not
extern-linked — `ra_permutation_cycle_orbit`/`ra_permutation_cycle_singleblock`/
`ra_hash`/`ra_reseed`/`rot32`/`fmix32`/`GUARD_L`/`GUARD_M` are `static` in
`ra_core.c`, so they're verbatim-copied here, same convention as
`../2026-9-2_singleblock-o-width-fix/diag_wideo_singleblock.c`). Four
cores: `orbit_baseline`, `orbit_accum`, `singleblock_baseline`,
`singleblock_accum`.

No second candidate (reorder-by-shift-amount) was attempted — `HANDOVER.md`
itself flags that direction as "not yet designed," and the accum candidate
already produced a clear, measurable result (see below) that's worth
checking in on before spending more time on a less-defined direction.

## Verification (mandatory, done before any speed measurement)

`verify_init_candidates.py`: full-stream byte-identical diff against the
**unmodified production `ra_core` binary's** `--stream` output, for all
four cores, across 6 key/n pairs each (key=0, key=0xFFFFFFFF, small edge,
orbit reseed-crossing cases, singleblock upper bound n=255) — 24 checks
total.

**Result: 24/24 passed, 0 mismatches.** Both `orbit_accum` and
`singleblock_accum` are bit-identical to production. No mismatch occurred,
so there was nothing to retry with a different key — consistent with the
standing rule that a failure at any single key/seed is decisive and is
never re-run to explain away (this rule didn't end up being invoked here,
since nothing failed).

## Assembly diff (`gcc -O3 -march=native -S`, extracted per-function)

| | orbit loop body | singleblock loop body |
|---|---|---|
| baseline `vpmulld` | 2 | 1 |
| accum `vpmulld` | **0** | **0** |
| baseline `vpaddd` | 1 (i-increment only) | 1 (i-increment only) |
| accum `vpaddd` | 3 (i-increment + 2 accumulators) | 2 (i-increment + 1 accumulator) |
| `vpsrlvd`/`vpsllvd` (rotate), baseline vs accum | 2+2 → 2+2 (**unchanged**) | 1+1 → 1+1 (**unchanged**) |
| total instructions in loop body, baseline vs accum | 17 vs 17 | 12 vs 12 |

The accum candidate does exactly what the algebra predicted: it eliminates
every `vpmulld` in the init loop, replacing each with a `vpaddd` of equal
count — same total instruction count, but multiply traded for add.
Confirms two things from `HANDOVER.md`'s own caveats:
1. GCC did **not** already auto-strength-reduce this pattern on its own —
   the baseline loop still issues `vpmulld` every iteration even though
   `keyterm` is loop-invariant, so writing the accumulation explicitly was
   a real transformation, not a no-op.
2. The variable-shift rotate (`vpsrlvd`/`vpsllvd`) is completely untouched
   in instruction count, as expected — this candidate only ever targeted
   the multiply chain, never claimed to touch the rotate.

## Benchmark (K=1, `--bench <core> 50000000 1`, 3 runs each, min/best-of-3 internal to each run)

| core | run1 | run2 | run3 | avg ns/word | ratio vs own baseline | ratio vs paperorig (24.9) |
|---|---|---|---|---|---|---|
| orbit_baseline | 54.43 | 54.16 | 54.17 | **54.25** | 1.00 | 2.18x |
| orbit_accum | 46.46 | 46.60 | 46.58 | **46.55** | **0.858** (−14.2%) | 1.87x |
| singleblock_baseline | 35.35 | 37.13 | 35.19 | **35.89** | 1.00 | 1.44x |
| singleblock_accum | 28.76 | 28.39 | 28.69 | **28.61** | **0.797** (−20.3%) | 1.15x |

(Baseline numbers here are consistent with the historical measurements
cited in `HANDOVER.md` — orbit 56.8, singleblock 33.4-39.1 — same
machine/session lineage, within normal run-to-run variance.)

## Benchmark (K=255, `--bench <core> 50000000 255`, 3 runs each) — the candidate is NOT a strict win across all K

Requested as a follow-up after the K=1 result above, to check the other
end of the K range (near-max block size, reinit rare, vs K=1's
reinit-every-call). Same method as K=1.

| core | run1 | run2 | run3 | avg ns/word | ratio vs own baseline |
|---|---|---|---|---|---|
| orbit_baseline | 2.2281 | 2.2425 | 2.2059 | **2.2255** | 1.00 |
| orbit_accum | 2.1730 | 2.1577 | 2.1445 | **2.1584** | **0.970** (−3.0%) |
| singleblock_baseline | 0.5868 | 0.5518 | 0.5461 | **0.5616** | 1.00 |
| singleblock_accum | 0.8401 | 0.7415 | 0.7307 | **0.7708** | **1.373** (**+37.3%, SLOWER**) |

Consistent across all 3 runs each (not noise). At K=255 the once-per-call
256-element init cost is amortized over 255 output words instead of 1, so
it contributes only a thin slice of total ns/word — dominated instead by
`ra_permutation_cycle_orbit`/`ra_permutation_cycle_singleblock`, which are
byte-identical, verbatim, untouched between baseline and accum. That
explains why orbit's gain shrinks from −14.2% (K=1) to −3.0% (K=255,
still a real if small win — orbit's init is heavier, L[]+M[], so it still
registers). It does **not** explain why singleblock flips to a 37.3%
*regression* — the accum init loop itself is not doing more work (same
instruction count per the assembly diff above), so this is most likely a
secondary codegen effect (the accumulator turns `m_val` itself into a
loop-carried dependency, vs. baseline where `m_val` is recomputed fresh
from the `i` register each iteration and only `i`'s increment is
loop-carried — plus possible register-allocation/code-layout differences
between the two variants sharing one translation unit). Root cause **not
diagnosed further** in this session — flagged as an open item, not
explained away.

**Implication:** the accum candidate is not a universal replacement for
the baseline init. It wins in the K-small/frequent-reinit regime that
motivates this whole experiment (HANDOVER's "addressable, few words per
key" use case) — confirmed strongly for singleblock at K=1, more modestly
for orbit — but singleblock specifically loses at K=255 (large blocks,
infrequent reinit). Any decision to apply this to `ra_core.c` needs to
account for which K range the real workload actually uses, not just the
K=1 number.

## Benchmark (orbit `--stream`, single continuous call, no forced reinit)

Requested as a third data point for orbit specifically, per the user's new
standing 5-measurement benchmark standard (see
[[feedback_orbit_singleblock_benchmark_standard]] in memory): orbit
`--multistream` K=1, orbit `--multistream` K=255 (both above, via
`--bench`, same reinit-per-block semantics), orbit `--stream` (this
section), singleblock K=1, singleblock K=255 (both above).

`--stream <core> <key> <n>` makes ONE call to `ra_core_*` with `n=50000000`
— `ra_init_state_orbit` runs exactly once (confirmed in `ra_core.c`:
`ra_permutation_cycle_orbit`'s per-255-word reseed calls `ra_reseed`, which
only XORs/hashes `M`/`L`, it never re-runs the init loop), so the init
cost is amortized over the entire 50M-word stream instead of just 255
words. 8 runs each (`diag_init_candidates --stream <core> 12345 50000000`,
timed via the binary's own `CLOCK_MONOTONIC` report, output redirected to
`/dev/null`), same `last_cons` every run confirming bit-identical output
again:

| core | min (best-of-8) | avg (8 runs) |
|---|---|---|
| orbit_baseline | 9.52 ns/word | 9.81 ns/word |
| orbit_accum | 9.52 ns/word | 9.73 ns/word |

**Ratio: 1.000 min-of-N (identical), 0.992 avg (within run-to-run noise).**
Confirms the mechanism directly: as the amortization length grows
(K=1 → K=255 → full 50M-word stream), the accum candidate's advantage for
orbit shrinks monotonically (−14.2% → −3.0% → ~0%), because
`ra_init_state_orbit` runs once per call regardless of call length. At true
continuous-stream scale the candidate makes no measurable difference for
orbit — it only matters when calls are short and frequent (small K).

## Root-cause investigation: why does singleblock_accum regress at K=255?

Requested by the user as a follow-up. Method: added a `--bench-init <core>
<iterations>` mode to `diag_init_candidates.c` (isolates the init loop
alone — no wrapper, no permutation-cycle call, no `--bench` reinit-sweep
harness), plus `perf stat` (including Intel topdown microarchitecture
counters) on the existing `--bench` K=255 path. All perf runs are `:u`
(userspace only).

**Step 1 — is the init loop itself slower for accum, even in isolation?**
No. `--bench-init` (2,000,000 direct calls to just the init function, no
surrounding wrapper):

| core | ns/call | ns/elem (÷256) |
|---|---|---|
| singleblock_baseline | 33.66 | 0.1315 |
| singleblock_accum | **28.78** (−14.5%) | 0.1124 |

Confirmed with `perf stat`: accum has *lower* cycles (1.21B vs 1.44B) and
*higher* IPC (3.76 vs 3.15) than baseline for the same ~4.55B instructions
— matches the K=1 result and the earlier assembly diff (fewer costly
multiplies). **This rules out "the accum loop itself got slower."** The
regression only appears once the init call is followed by the
`ra_permutation_cycle_singleblock` call in the full wrapper.

**Step 2 — `perf stat` topdown breakdown, full `--bench` K=255 path:**

| | baseline | accum |
|---|---|---|
| retiring | 58.9% | 49.4% |
| bad speculation (branch mispredict etc.) | 12.5% | 7.5% |
| frontend-bound | 7.5% | **1.2%** |
| **backend-bound** | 22.5% | **42.0%** |

Instructions retired are nearly identical between the two (1.489B vs
1.491B, <0.2% difference) but accum burns **~22% more total cycles**
(428.8M vs 525.2M) — all of the extra cost lands in **backend-bound**
stalls; frontend and branch-misprediction actually improved for accum
(fewer branch-misses too: 618K vs 1.18M). So this is not a
fetch/decode/prediction problem — execution/memory backend resources are
the bottleneck specifically in the combined (init + permutation-cycle
call) path.

**Step 3 — ruled out specific backend causes:** cache misses are
negligible and near-identical for both (~5.3-5.4K out of the whole 50M-word
run). `ld_blocks.store_forward` (the classic store→load-forwarding-stall
counter, tested because the init loop writes `M[]` via wide 256-bit vector
stores immediately before the scalar permutation-cycle function reads it
back 4 bytes at a time) is **2 events for both variants** — negligible,
ruling out store-to-load forwarding stalls as the mechanism.
`l1d_pend_miss.fb_full`/`ld_blocks_partial.address_alias` were also small
and inconsistent between runs (in one single perf session under this
specific event group, accum even came out faster — see below).

**Not fully pinned down:** the exact backend resource being contended
(most likely candidate, not confirmed: execution-port contention from the
extra loop-carried add chains the accumulator introduces — baseline has
one loop-carried recurrence (`i`), accum has three (`i`, `l_val`/`m_val`
accumulators) all needing to resolve every iteration, versus baseline's
`l_val`/`m_val` multiplies which are off the critical path and can be
freely reordered/pipelined by the out-of-order engine). Confirming this
specifically would need per-port execution-unit profiling (e.g. Intel
VTune or `perf record` with uarch-specific sampling) not attempted here —
flagged as an open item, not resolved.

**Reproducibility check:** one `perf stat` session (measuring
`ld_blocks.*` specifically) showed accum *faster* than baseline for that
single run (0.641 vs 0.660 ns/word) — the opposite of every other
measurement. Re-ran 5 more `--bench` K=255 trials interleaved
(baseline/accum/baseline/accum/...) with **no** perf instrumentation:
accum was slower in **5/5**. Treating that one perf-instrumented run as
noise from the profiler's own counter-programming overhead, not evidence
against the regression — the regression itself is real and highly
consistent (8/8 non-perf-instrumented runs across this session show it).

## Against the HANDOVER talking-point thresholds

`HANDOVER.md` offered (explicitly as a talking point, not a mandated
threshold) "closing at least half the gap to paperorig" as a rough
definition of "significant": singleblock noticeably under ~29 ns/word,
orbit under ~41 ns/word.

- **singleblock_accum: 28.61 ns/word — crosses that talking-point
  threshold** (under 29), and lands within 15% of paperorig's 24.9.
- **orbit_accum: 46.55 ns/word — does not cross** its talking-point
  threshold (41); it closes part of the gap (54.25→46.55 of the 54.25→24.9
  span, i.e. ~26% of the total gap) but not half.

These are raw numbers against an informally-offered reference point, not a
decision — "significant" was never formally pinned down, per `HANDOVER.md`
itself. That pin-down is exactly the checkpoint this document stops at.

## `ra_hash`/`ra_reseed`

Not touched, not measured separately in this session. `HANDOVER.md` flags
these as lower priority (amortized over 255 words, only called once per
reseed cycle in orbit mode, never called at all in singleblock mode for
`rng<=255`). Left for a future pass if Priority 1 continues.

## What was NOT done

- Reorder-by-shift-amount direction: not attempted (see above).
- No candidate was applied to `ra_core.c` — that requires a separate,
  explicit user confirmation per `HANDOVER.md`'s hard constraint, even
  though both accum candidates are proven bit-identical. Nothing in this
  session applied anything to production.
- No re-run with a different key/seed to explain away a failure — moot
  here since nothing failed, but noted per the standing rule.
- Priority 2 (new init formula, output allowed to change, full
  avalanche/PractRand/anti-weak-key battery) was not started. It requires
  its own DSL design and VPS-scale rigor per `HANDOVER.md`, and the
  HANDOVER's own exit criterion says not to start it until "significant"
  is pinned down with the user — which is exactly the open question this
  document raises rather than answers.

## Open question for the user

Both accum candidates are bit-identical, portable C, zero SIMD intrinsics.
At K=1, singleblock crosses the informal "significant" talking-point
threshold and orbit doesn't quite — but at K=255 the picture is different:
orbit still wins narrowly (−3.0%), singleblock **regresses** (+37.3%
slower). Four things need a decision:

1. Is singleblock's K=1 speedup (35.9→28.6 ns/word, −20%) still worth
   pursuing given it flips to a regression at K=255 — i.e. does the real
   use case actually live in the small-K regime this candidate targets, or
   does it need to be fast across the full K range?
2. Is orbit's more modest but K-range-consistent win (−14% at K=1, −3% at
   K=255, no regression seen) more attractive than singleblock's on that
   basis, even though it doesn't cross its own "significant" threshold?
3. Do you want either candidate applied to `ra_core.c` now (separate
   explicit confirmation, as required), or held pending a further look —
   e.g. diagnosing the K=255 singleblock regression's root cause (not done
   this session), the reorder-by-shift-amount direction, or the
   `ra_hash`/`ra_reseed` amortized cost?
4. Should this proceed to Priority 2 (new formula search, bigger
   undertaking, full battery + VPS)?
