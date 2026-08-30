# STATUS: periodicity-heuristic-validation2

**Completed, with one caveat (2026-08-28, corrected 2026-08-29)** -- see
`RESULTS.md` for full report and findings, and the "2026-08-29 correction"
entry in `HANDOVER.md` for what changed and why.

6 of 7 sweep configs completed cleanly via the automated `run_sweep.py`
pipeline. The 7th, `(n=8, w=4, rows=4)`, made `run_sweep.py` **crash**
(`subprocess.CalledProcessError`, exit 42) instead of completing -- root cause:
a since-fixed process-matching bug in `auto_stop_power38.py` (its `pgrep`
pattern was too broad and killed the wrong `cycle_measure` process, including
this sweep's own subprocess for that config). This config was instead measured
out-of-band via a direct, checkpoint-resumable `cycle_measure` invocation
babysat by the (now-fixed) watchdog script -- not a completed cycle
measurement, but a strict lower bound on `lambda`. The originally-published
lower bound for this config (`> 151 Billion steps`, written before that
out-of-band process had actually stopped) also undercounted the checkpoint's
true final state by roughly 1.8x. Corrected, re-measured lower bound (from a
clean resume under the fixed watchdog, manually stopped 2026-08-29):
`lambda > 3.045 × 10^11` (`> 304 Billion steps`). See `HANDOVER.md` for the
full derivation.

## Completed Tasks

1. **`pruned_wired_toy_prng.py`** -- core module, written and smoke-tested.
2. **`self_check_full_scale.py`** -- bit-identical gate at `Params(n=256,w=32,rows=32)` vs `winner_wired_v2.c` PASSED (5/5).
3. **`avalanche_check.py`** -- PASSED across all 7 configs (30-70% Hamming band).
4. **`cycle_measure.py` / `cycle_measure.c`** -- compiled and cross-validated bit-identical against Python backend.
5. **`enumerate_n2w4.py`** -- $N = 131,072$ brute force enumeration complete, saved to `enumerate_n2w4_result.json`.
6. **`enumerate_n4w4.py`** -- $N = 402,653,184$ vectorized enumeration complete, saved to `enumerate_n4w4_result.json`.
7. **Sweep CSVs generated via `run_sweep.py`** (6 of 7 configs; see caveat above for the 7th):
   - `results_n2_w8.csv` (500 seeds)
   - `results_n4_w8.csv` (100 seeds)
   - `results_n2_w4.csv` (500 seeds)
   - `results_n4_w4.csv` (500 seeds)
   - `results_n4_w4_rows1.csv` (500 seeds)
   - `results_n8_w4_rows2.csv` (10 seeds, $\lambda_{\text{mean}} \approx 5.06\text{ Billion}$)
8. **`(n=8, w=4, rows=4)`** -- measured manually (not via `run_sweep.py`, see
   caveat above); corrected lower bound in `RESULTS.md` / `HANDOVER.md`.
9. **`RESULTS.md`** -- comprehensive final report written and documented.

