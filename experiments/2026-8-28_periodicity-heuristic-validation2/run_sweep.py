"""Drives the pruned/wired periodicity-heuristic sweep.

Same lambda_pred formula as ../2026-8-25_periodicity-heuristic-validation/
run_sweep.py, reused verbatim (state shape -- hence |S| -- is unchanged by
op-pruning/operand-rewiring, only the update function differs). For each
config, runs Brent's cycle detection over many seeds and compares against
0.7824*sqrt(|S|).

SWEEP = 4 of the original 5 configs (default rows, for direct comparability
against the sibling experiment's own results; (n=8,w=4,rows=4) is
DELIBERATELY EXCLUDED -- see MANUAL_ONLY below) + 2 new configs
(n=4,w=4,rows=1) and (n=8,w=4,rows=2), both G=4, chosen so tap-pruning
(top-2-of-G survivors) is non-vacuous -- see pruned_wired_toy_prng.py's
module docstring for why a G=8 config (exact TAP6/TAP7 structural match)
is infeasible at any measurable (n,w): it either collapses to zero
(w<=7, e.g. n=8,w=4,rows=1: taps 6,7 >= w=4) or pushes |S| into the
already-documented-infeasible range (w>=8, e.g. n=8,w=8 -> log2|S|=95.3,
far beyond the original experiment's own INFEASIBLE list).
"""

from __future__ import annotations

import csv
import math
import statistics
import subprocess
import sys
import time
from pathlib import Path

from pruned_wired_toy_prng import Params
from cycle_measure import brent

HERE = Path(__file__).parent
C_BINARY = HERE / "cycle_measure"

LAMBDA_CONST = 0.7824


def log2_fact(n: int) -> float:
    return math.lgamma(n + 1) / math.log(2)


def predicted(n: int, w: int) -> tuple[float, float]:
    bmut = w * (n + 2)
    log2s = bmut + log2_fact(n)
    lam_pred = LAMBDA_CONST * (2 ** (log2s / 2))
    return log2s, lam_pred


def run_python(n: int, w: int, seed_count: int, rows: int | None = None) -> list[tuple[int, int, int]]:
    p = Params(n=n, w=w, rows=rows)
    out = []
    for seed in range(seed_count):
        lam, mu = brent(p, seed)
        out.append((seed, lam, mu))
    return out


def run_c(n: int, w: int, seed_count: int, rows: int | None = None) -> list[tuple[int, int, int]]:
    if not C_BINARY.exists():
        raise RuntimeError(f"{C_BINARY} not built - run: gcc -O3 -march=native -std=gnu17 "
                            f"cycle_measure.c -o cycle_measure -lm")
    rows_arg = rows if rows is not None else min(4, n)
    proc = subprocess.run(
        [str(C_BINARY), str(n), str(w), str(rows_arg), "0", str(seed_count)],
        capture_output=True, text=True, check=True,
    )
    print(proc.stderr, end="", file=sys.stderr)
    out = []
    reader = csv.DictReader(proc.stdout.splitlines())
    for row in reader:
        out.append((int(row["seed"]), int(row["lambda"]), int(row["mu"])))
    return out


# (n, w, rows, seed_count, backend) -- rows=None means "use the config's
# default min(4,n)", matching the sibling experiment's own convention.
#
# (8, 4, rows=2, seed_count=10): reduced from the originally-planned 100 --
# each seed at this scale takes long enough (lambda in the billions) that
# 100 seeds was infeasible in reasonable wall-clock time. results_n8_w4_rows2.csv
# and RESULTS.md's table both reflect 10 seeds; keep this constant in sync
# with what's actually run, or re-running this SWEEP won't reproduce the
# published table.
#
# (8, 4, rows=4) is INTENTIONALLY ABSENT from this list -- see MANUAL_ONLY
# below. It crashed this sweep on 2026-08-28 (subprocess.CalledProcessError,
# exit 42) because a since-fixed bug in auto_stop_power38.py's process-matching
# pattern killed the wrong `cycle_measure` process while this sweep's own
# (8,4,rows=4) subprocess happened to be running concurrently (see
# HANDOVER.md's "2026-08-29 correction" entry). Re-adding it here would risk
# the same crash again if a manual out-of-band measurement is running at the
# same time.
SWEEP = [
    (2, 8, None, 500, "c"),
    (4, 8, None, 100, "c"),
    (2, 4, None, 500, "c"),
    (4, 4, None, 500, "c"),
    (4, 4, 1, 500, "c"),   # NEW: G=4, taps={2,3} non-vacuous
    (8, 4, 2, 10, "c"),    # NEW: G=4, taps={2,3} non-vacuous
]

INFEASIBLE = [(8, 8), (16, 4)]

# (n, w, rows) configs whose lambda is too large to fully close (tortoise/hare
# never meet in reasonable time), but ARE partially measurable via a
# checkpoint-resumable lower bound -- unlike INFEASIBLE, these DO have a real
# (if incomplete) measurement. Not run automatically by this script: use
# `cycle_measure <n> <w> <rows> <seed> 1` directly, babysat by a watchdog
# script (pattern: auto_stop_power38.py) that stops it at a target checkpoint
# `power` and reports `lambda > (power - 1) + lam` as a strict lower bound.
# See RESULTS.md's row for this config and HANDOVER.md for the exact number.
MANUAL_ONLY = [(8, 4, 4)]


def result_csv_name(n: int, w: int, rows: int) -> str:
    default_rows = min(4, n)
    if rows == default_rows:
        return f"results_n{n}_w{w}.csv"
    return f"results_n{n}_w{w}_rows{rows}.csv"


def main():
    summary_rows = []

    for n, w, rows, seed_count, backend in SWEEP:
        p_rows = rows if rows is not None else min(4, n)
        log2s, lam_pred = predicted(n, w)
        print(f"=== n={n} w={w} rows={p_rows} ({backend}, {seed_count} seeds) ===", file=sys.stderr)
        start = time.time()
        if backend == "python":
            rows_out = run_python(n, w, seed_count, rows)
        else:
            rows_out = run_c(n, w, seed_count, rows)
        elapsed = time.time() - start
        print(f"  done in {elapsed:.1f}s", file=sys.stderr)

        out_csv = HERE / result_csv_name(n, w, p_rows)
        with open(out_csv, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(["seed", "lambda", "mu"])
            writer.writerows(rows_out)

        lambdas = [r[1] for r in rows_out]
        mus = [r[2] for r in rows_out]
        summary_rows.append({
            "n": n, "w": w, "rows": p_rows, "log2S": log2s, "lambda_pred": lam_pred,
            "seed_count": seed_count,
            "lambda_mean": statistics.mean(lambdas),
            "lambda_median": statistics.median(lambdas),
            "lambda_min": min(lambdas),
            "lambda_max": max(lambdas),
            "distinct_lambdas": len(set(lambdas)),
            "mu_mean": statistics.mean(mus),
            "elapsed_s": elapsed,
        })

    print()
    print(f"{'n':>3} {'w':>3} {'rows':>4} {'log2|S|':>9} {'lambda_pred':>14} {'seeds':>6} "
          f"{'lambda_mean':>14} {'lambda_median':>14} {'lambda_min':>12} {'lambda_max':>12} "
          f"{'distinct':>8} {'ratio(mean/pred)':>17}")
    for r in summary_rows:
        ratio = r["lambda_mean"] / r["lambda_pred"]
        print(f"{r['n']:>3} {r['w']:>3} {r['rows']:>4} {r['log2S']:>9.1f} {r['lambda_pred']:>14.3e} "
              f"{r['seed_count']:>6} {r['lambda_mean']:>14.3e} {r['lambda_median']:>14.3e} "
              f"{r['lambda_min']:>12} {r['lambda_max']:>12} {r['distinct_lambdas']:>8} {ratio:>17.4f}")

    print()
    print("Infeasible to measure (analytic only):")
    for n, w in INFEASIBLE:
        log2s, lam_pred = predicted(n, w)
        print(f"  n={n:>3} w={w}: log2|S|={log2s:.1f}  lambda_pred={lam_pred:.3e}  (not measured)")

    print()
    print("Manual-only (checkpoint-resumable lower bound, not run by this sweep):")
    for n, w, rows in MANUAL_ONLY:
        log2s, lam_pred = predicted(n, w)
        print(f"  n={n:>3} w={w} rows={rows}: log2|S|={log2s:.1f}  lambda_pred={lam_pred:.3e}  "
              f"(see RESULTS.md / HANDOVER.md for the measured lower bound)")

    return summary_rows


if __name__ == "__main__":
    main()
