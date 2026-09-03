# Dieharder + inject-crossing validation for singleblock K-small-defect fix candidates

Continues `PRODUCTION_READINESS_HANDOVER.md` §3 action items 1 & 2 for the
two Pareto-selection co-frontier candidates (`experiments/2026-9-3_combo-winner-pareto-selection/RESULTS.md`):
`w8_f10_i0` (fastest) and `w8_f28_i0` (best avalanche margin).

## 1. Dieharder (action item 1)

Adapted `../2026-9-1_dieharder-battery/run_dieharder_battery.py` for the
combo-search candidates' 4-arg CLI (`--stream base_key n K`, vs. 3-arg for
`winner_wired_v2`/`winner_wired_addressable`). Same 27 "Good"-reliability
tests, same piped-no-file-no-rewind methodology, `BASE_KEY=111222`
(consistent with the PractRand Promotion Tier that already validated these
candidates). Run locally (laptop), K=1 (worst case) and K=96 (recommended
safe-K floor per `../2026-9-2_singleblock-k-threshold-characterization/RESULTS.md`).

| Candidate | K | PASSED | WEAK | FAILED |
|---|---|---|---|---|
| `w8_f10_i0` | 1 | 25/27 | 2 | 0 |
| `w8_f10_i0` | 96 | 26/27 | 1 | 0 |
| `w8_f28_i0` | 1 | 26/27 | 1 | 0 |
| `w8_f28_i0` | 96 | 26/27 | 0 | 0 |

**0 FAILED across all 4 runs.** Comparable to the already-production-approved
baselines: `winner_wired_v2` (26/27 PASSED, 2 WEAK) and
`winner_wired_addressable` (25/27 PASSED, 1 WEAK), see
`../2026-9-1_dieharder-battery/RESULTS.md`. WEAK sub-results are isolated
single p-value flags within a multi-sample test (e.g. `diehard_count_1s_str`,
`dab_filltree`) -- not FAIL, and this rate is in line with prior clean runs
in this repo (dieharder's own convention: an occasional WEAK per battery is
expected, not a red flag on its own).

Raw output: `dieharder_{w8_f10_i0,w8_f28_i0}_K{1,96}_piped.txt` (4 files, this folder).

**Conclusion: action item 1 (dieharder) is CLEAR for both candidates.**

## 2. Inject-crossing (action item 2)

Generated the 6 extra-inject variants (`i1`..`i6`) for each base candidate
(12 new binaries: `w8_f10_i1`..`i6`, `w8_f28_i1`..`i6`), via
`combo_gen.write_and_compile` reused verbatim from
`../2026-9-2_singleblock-cycle-combo-search/`. All 12 passed
`sanity_check`/`verify_control_matches_production` (byte-identical to the
Python oracle) before any PractRand run.

### Methodology correction found this session

The original Tier 0 (avalanche gate) and Tier 1 (PractRand K=1) both
operate at K=1, where `recipes.py`/`combo_prng.py` **prove** extra-inject is
byte-identical to "off" (the inject write happens after the single round's
output is emitted, and the K=1 loop breaks immediately after). Re-running
those two tiers against the 12 new candidates would just reproduce `i0`'s
already-known-clean K=1 results -- not a real test. `recipes.py`'s own
`all_promotion_candidates()` is designed for exactly this: cross all 7
inject variants directly at the Promotion Tier (K>=2), skipping Tier 0/1
for inject variants specifically.

### Staged screening at K=96 (adapted quick-reject, local)

Ran a volume-only staged screen (256MB -> 2GB, same staged sizes as
Tier 1's own reasoning) but at **K=96** instead of K=1, so inject is
actually observable. `staged_inject_screening.py`, `BASE_KEY=111222`.

**12/12 candidates survived** (0 hard FAIL). Soft anomalies noted (not
blocking, logged for review):

| Candidate | Stage | Soft anomaly |
|---|---|---|
| `w8_f10_i4` | Stage 2 (2GB) | 1x "unusual" |
| `w8_f10_i5` | Stage 2 (2GB) | 1x "unusual" |
| `w8_f10_i6` | Stage 2 (2GB) | 1x "unusual" |
| `w8_f28_i2` | Stage 1 (256MB) | 1x "unusual" |

Per repo precedent, a clean small-scale screen is not a final verdict (a
prior triage stage passed 9/9 at 2GB and 3 of those later FAILed at 16GB) --
all 12 candidates proceeded to the full Promotion Tier rather than treating
this screen as sufficient.

### Promotion Tier (16GB x K in {2,4,8,16,32,64,96}, VPS)

K=1 omitted: provably identical to `i0`'s already-clean K=1 Promotion Tier
result (see methodology correction above), so re-running it would spend
compute for a known answer. Run via `promotion_search_inject.py` on the VPS
(`VPS_ACCESS.md`), mirroring `../2026-9-2_singleblock-cycle-combo-search/promotion_search.py`
exactly (early-kill on hard FAIL before the final checkpoint, live
checkpoint logging, `ThreadPoolExecutor(max_workers=2)` for the 8-core VPS,
no re-seed retries).

**Result: 11/12 candidates FULLY CLEAN across all 7 K values to 16GB.**

| Candidate | K=2 | K=4 | K=8 | K=16 | K=32 | K=64 | K=96 |
|---|---|---|---|---|---|---|---|
| `w8_f10_i1` | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN |
| `w8_f10_i2` | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN |
| `w8_f10_i3` | CLEAN | **FAIL** | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN |
| `w8_f10_i4` | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN |
| `w8_f10_i5` | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN |
| `w8_f10_i6` | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN |
| `w8_f28_i1` | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN |
| `w8_f28_i2` | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN |
| `w8_f28_i3` | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN |
| `w8_f28_i4` | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN |
| `w8_f28_i5` | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN |
| `w8_f28_i6` | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN | CLEAN |

`w8_f10_i3` at K=4 shows a genuine, escalating BCFN-family defect (the same
failure family this whole research line has been fighting), not a
discretization artifact:

```
2GB:  BCFN(2+9,13-5,T)   R=+17.4  mildly suspicious
4GB:  BCFN(2+9,13-4,T)   R=+14.9  mildly suspicious
      BCFN(2+10,13-5,T)  R=+15.3  unusual
8GB:  13 BCFN anomalies (several VERY SUSPICIOUS), including:
      BCFN(2+18,13-9,T)  R=+64.4  p=1.3e-15  FAIL
      BCFN(2+19,13-9,T)  R=+72.5  p=2.0e-17  FAIL
```
Early-killed at the 8GB checkpoint (16GB not run for this specific pair --
compute saved per the early-stop design, not a missing data point). Per
repo-standing rule (no re-seed retry), `w8_f10_i3` is a **final reject** --
specifically at K=4; K=2,8,16,32,64,96 for the same candidate all ran clean
to 16GB, so this is a narrow (candidate, K) intersection, not a defect
that generalizes across K or across the `f28` base.

Raw logs: `promotion_logs_inject/{cid}_K{k}_16GB.log` (84 files), aggregate
`promotion_log_inject.jsonl`. Run wall-clock: ~3.3h on the VPS (2x parallel,
8-core), matching the pre-run estimate from the original Promotion Tier's
own timing data.

## 3. Recommendation

Both action items from `PRODUCTION_READINESS_HANDOVER.md` §3 are now CLEAR:

1. **Dieharder**: 0 FAILED for both `w8_f10_i0` and `w8_f28_i0` at K=1 and
   K=96 -- clean, comparable to the production-approved baselines.
2. **Inject-crossing**: extra-inject does NOT generally break the
   cleanliness found by the combo-search -- 11/12 crossed variants (both
   bases, 6 of 6 `f28` inject variants, 5 of 6 `f10` inject variants) stay
   fully clean across the entire K range to 16GB. The one exception
   (`w8_f10_i3` at K=4) is a narrow, real defect, not evidence the whole
   inject mechanism is unsafe -- but it does mean: **if extra-inject is
   ever adopted in production, `i3` (`add_a`, offset 64) must not be paired
   with `w8_f10`, or more conservatively, inject should stay off (`i0`,
   already the actual Pareto-selection candidates) unless a specific inject
   variant is independently validated the way `i0` was.**

Both `w8_f10_i0` and `w8_f28_i0` (the actual Pareto-selection candidates,
inject OFF) remain exactly as clean as they were before this session --
this session found no new problem with the candidates that would actually
be promoted, only characterized what happens if inject were added later.

**Next step per the handover's own gate**: promotion decision to
`ra_core.c` still requires explicit user confirmation (not made in this
session) -- see `PRODUCTION_READINESS_HANDOVER.md` update.
