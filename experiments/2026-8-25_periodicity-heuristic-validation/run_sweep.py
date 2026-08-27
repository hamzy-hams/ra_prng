"""Drives the full periodicity-heuristic-validation sweep.

For each measurable (n, w) config, runs Brent's cycle detection over many
independent seeds (Python for the small/fast configs, the compiled C port
for (8,4) - see the timing note in RESULTS.md for why), records lambda/mu
per seed, and compares the measured distribution against the paper's
0.7824*sqrt(|S|) heuristic. Also prints the two configs the handover flags
as infeasible to fully measure ((8,8) and (16,4)) as analytic-only rows.

Raw per-seed results are written to results_n{n}_w{w}.csv for each
measured config; the summary table is printed to stdout (redirect to save).
"""

from __future__ import annotations

import csv
import math
import statistics
import subprocess
import sys
import time
from pathlib import Path

from toy_prng import Params
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
    print(proc.stderr, end="", file=sys.stderr)  # config echo from the C binary
    out = []
    reader = csv.DictReader(proc.stdout.splitlines())
    for row in reader:
        out.append((int(row["seed"]), int(row["lambda"]), int(row["mu"])))
    return out


# (n, w, seed_count, backend) - seed counts chosen from measured per-seed
# timing (see RESULTS.md): fast configs get more seeds within a few minutes
# budget, (8,4) is capped by C's ~3.3s/seed.
SWEEP = [
    (2, 8, 500, "python"),
    (4, 8, 100, "python"),
    (2, 4, 500, "python"),
    (4, 4, 500, "python"),
    (8, 4, 100, "c"),
]

INFEASIBLE = [(8, 8), (16, 4)]


def main():
    summary_rows = []

    for n, w, seed_count, backend in SWEEP:
        log2s, lam_pred = predicted(n, w)
        print(f"=== n={n} w={w} ({backend}, {seed_count} seeds) ===", file=sys.stderr)
        start = time.time()
        if backend == "python":
            rows = run_python(n, w, seed_count)
        else:
            rows = run_c(n, w, seed_count)
        elapsed = time.time() - start
        print(f"  done in {elapsed:.1f}s", file=sys.stderr)

        out_csv = HERE / f"results_n{n}_w{w}.csv"
        with open(out_csv, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(["seed", "lambda", "mu"])
            writer.writerows(rows)

        lambdas = [r[1] for r in rows]
        mus = [r[2] for r in rows]
        summary_rows.append({
            "n": n, "w": w, "log2S": log2s, "lambda_pred": lam_pred,
            "seed_count": seed_count,
            "lambda_mean": statistics.mean(lambdas),
            "lambda_median": statistics.median(lambdas),
            "lambda_min": min(lambdas),
            "lambda_max": max(lambdas),
            "mu_mean": statistics.mean(mus),
            "elapsed_s": elapsed,
        })

    print()
    print(f"{'n':>3} {'w':>3} {'log2|S|':>9} {'lambda_pred':>14} {'seeds':>6} "
          f"{'lambda_mean':>14} {'lambda_median':>14} {'lambda_min':>12} {'lambda_max':>12} "
          f"{'ratio(mean/pred)':>17}")
    for r in summary_rows:
        ratio = r["lambda_mean"] / r["lambda_pred"]
        print(f"{r['n']:>3} {r['w']:>3} {r['log2S']:>9.1f} {r['lambda_pred']:>14.3e} "
              f"{r['seed_count']:>6} {r['lambda_mean']:>14.3e} {r['lambda_median']:>14.3e} "
              f"{r['lambda_min']:>12} {r['lambda_max']:>12} {ratio:>17.4f}")

    print()
    print("Infeasible to measure (analytic only):")
    for n, w in INFEASIBLE:
        log2s, lam_pred = predicted(n, w)
        print(f"  n={n:>3} w={w}: log2|S|={log2s:.1f}  lambda_pred={lam_pred:.3e}  (not measured)")

    return summary_rows


if __name__ == "__main__":
    main()
