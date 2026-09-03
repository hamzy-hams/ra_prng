# Runner-up comparison: `production-candidate-battery` Step 4-8 for `w8_f28_i0`

Purely a reference/curiosity comparison, requested after `w8_f10_i0` (the
faster of the 2 Pareto co-frontier candidates, see `RESULTS.md`) was
promoted to `ra_core.c` and cleared the full `production-candidate-battery`
gate (`../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md`).
**Does not change the promotion decision** -- `w8_f10_i0` remains the
promoted candidate. This just runs the same Step 4-8 battery against
`w8_f28_i0` (best avalanche margin, ~1ns/word slower, NOT selected) to see
how the runner-up would have fared.

Run directly against the already-validated standalone candidate binary
(`../2026-9-2_singleblock-cycle-combo-search/candidates/w8_f28_i0`, which
has a built-in `--stream <base_key> <n> <K>` parametrized CLI) via
subprocess -- `ra_core.c` and the `production-candidate-battery` folder's
own binaries were **not touched or rebuilt** for this comparison.

## Result table (vs. the promoted `w8_f10_i0`)

| Test | `w8_f10_i0` (promoted) | `w8_f28_i0` (runner-up) |
|---|---|---|
| Step 4: collision-scan K=1 (50,000 keys) | 0 collisions, z=-0.54 | 0 collisions, z=-0.54 |
| Step 5: dieharder K=1 | 25/27 PASSED, 2 WEAK, 0 FAILED | 26/27 PASSED, 1 WEAK, 0 FAILED |
| Step 5: dieharder K=96 | 26/27 PASSED, 1 WEAK, 0 FAILED | 26/27 PASSED, 0 WEAK, 0 FAILED |
| Step 5: dieharder K=255 (new) | 25/27 PASSED, 1 WEAK, 0 FAILED | 25/27 PASSED, 2 WEAK, 0 FAILED |
| Step 6: PractRand 16GB, K=1..96 | clean (prior combo-search runs) | clean (prior combo-search runs) |
| Step 6: PractRand 16GB, K=255 (new) | **fully clean, 0 anomalies at any checkpoint** | 2 "unusual" flags at one intermediate checkpoint (BCFN R=+8.6, DC6-9x1Bytes-1 R=+5.9), both gone by the final 16GB checkpoint ("no anomalies in 240 test results") |
| Step 7: shuffle K=255, runs-test z | -0.35 | -1.20 |
| Step 7: shuffle K=1, runs-test z (global) | +9.25 | +5.69 |
| Step 7: shuffle K=1, per-rep mean z (t-stat) | 0.242 (t=11.0) | 0.309 (t=12.8) |

(K=1/K=96 dieharder and K=1..96 PractRand numbers for both candidates are
from the earlier `../2026-9-3_dieharder-inject-crossing/RESULTS.md` and
`../2026-9-2_singleblock-cycle-combo-search/` sessions, cited here for a
complete side-by-side -- not re-run.)

## Observations

- **Both candidates are essentially equivalent** on Step 4 (collision-scan)
  and Step 5 (dieharder) -- no FAILED anywhere, WEAK counts within the
  same noise band both directions.
- **Step 6 (PractRand K=255) mildly favors `w8_f10_i0`**: it stayed
  completely anomaly-free at every checkpoint, while `w8_f28_i0` picked up
  2 "unusual" (the mildest PractRand flag) at one intermediate checkpoint
  that cleared by the final 16GB target. Neither is a FAIL; this is a
  minor data point, not a reversal-worthy signal on its own.
- **Step 7's K=1 runs-test bias appears in BOTH candidates, similar
  magnitude** (per-rep mean z 0.242 vs 0.309, same sign, both highly
  significant vs. their own noise floor). This is evidence the bias found
  in `w8_f10_i0`'s Step 7 (see `ADDENDUM_POST_FIX_STATUS.md`) is more
  likely a **shared methodology property of the shuffle driver** than a
  defect specific to `w8_f10_i0`'s formula: `scrambler_ra_core_
  singleblock.c`'s K=1 driver derives each swap-step's key as
  `base_key + (rep_index + step_index) * GOLDEN` (additive Weyl increment
  reused for both the per-repetition and per-swap-step chaining) --
  meaning many different (rep, step) pairs across a long run share the
  exact same underlying key whenever `rep_index + step_index` coincides.
  That's a real structural property of the test harness's key derivation,
  not something either candidate's cycle formula can avoid. **Not fixed
  here** (out of scope for a look-only comparison) -- flagged for whoever
  next touches Step 7's methodology.

Raw output for the two new (K=255) runs: `dieharder_w8_f28_i0_K255_piped.txt`,
`practrand_w8_f28_i0_K255_16GB.log`. Step 7's throwaway driver
(`scramble_w8_f28_i0_lookonly.py`, kept for reference/reproducibility only
-- calls the standalone candidate binary via subprocess, does not touch
`ra_core.c` or `scrambler_ra_core_singleblock.c`) reused
`../2026-8-30_addressable-shuffle/scc_test.py`'s scoring functions, same as
`ADDENDUM_POST_FIX_STATUS.md`'s Step 7.

## Conclusion

No new information here argues for revisiting the `w8_f10_i0` promotion.
The runner-up performs comparably, with `w8_f10_i0` slightly cleaner on
PractRand K=255 specifically -- consistent with it already being the
selected candidate. This file is a reference snapshot only.
