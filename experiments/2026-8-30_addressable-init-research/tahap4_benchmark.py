"""Tahap 4: orchestrator for tahap4_bench (C harness, does the actual timed
work in-process). This script calls the compiled binary ONCE per mode
(subprocess), parses its stdout lines, and writes structured JSON results
into this folder. All precise per-repeat timing happens inside the C
binary via clock_gettime(CLOCK_MONOTONIC, ...) -- this script is pure
orchestration, not a re-timer (subprocess-spawn overhead would corrupt
fine-grained measurements if the repeat loops were driven from Python).

Does not modify winner_wired_addressable.c, winner_wired_v2.c, or any
Philox file in benchmarks/comparisons/ in place -- tahap4_bench.c is a
self-contained new file (see its own header comment for the Philox
PHILOX_M0 correction and citation).
"""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).parent
BENCH_BIN = HERE / "tahap4_bench"

INIT_COST_REPEATS = 5_000_000
THROUGHPUT_N = [1, 2, 4, 8, 16, 24, 32, 48, 64, 80, 96, 128, 192, 256,
                512, 1024, 4096, 65536, 1_000_000, 10_000_000, 100_000_000]
REINIT_SWEEP_TARGET_WORDS = 20_000_000
REINIT_SWEEP_K = [1, 2, 4, 8, 16, 24, 32, 48, 64, 80, 96, 128, 192, 256,
                   512, 1024, 4096, 65536]


def run_verify() -> bool:
    result = subprocess.run([str(BENCH_BIN), "verify"], capture_output=True, text=True)
    print(result.stdout)
    return result.returncode == 0


def run_init_cost() -> dict:
    result = subprocess.run(
        [str(BENCH_BIN), "init-cost", str(INIT_COST_REPEATS)],
        capture_output=True, text=True, check=True,
    )
    print(result.stdout.strip())
    parts = result.stdout.split()
    d = {parts[i]: parts[i + 1] for i in range(0, len(parts) - 1, 2) if i > 0}
    return {
        "repeats": INIT_COST_REPEATS,
        "addr_ns_per_init": float(d["addr_ns_per_init"]),
        "philox_ns_per_call": float(d["philox_ns_per_call"]),
        "ratio": float(d["ratio"]),
    }


def _parse_rows(stdout: str, row_key: str) -> list[dict]:
    rows = []
    for line in stdout.strip().splitlines():
        parts = line.split()
        if not parts or parts[0] != row_key:
            continue
        d = {}
        i = 0
        while i < len(parts):
            if parts[i] == row_key:
                d[row_key] = int(parts[i + 1])
                i += 2
                continue
            key, val = parts[i], parts[i + 1]
            try:
                d[key] = int(val)
            except ValueError:
                d[key] = float(val)
            i += 2
        rows.append(d)
    return rows


def run_throughput() -> list[dict]:
    args = [str(BENCH_BIN), "throughput"] + [str(n) for n in THROUGHPUT_N]
    result = subprocess.run(args, capture_output=True, text=True, check=True)
    print(result.stdout)
    return _parse_rows(result.stdout, "N")


def run_reinit_sweep() -> list[dict]:
    args = [str(BENCH_BIN), "reinit-sweep", str(REINIT_SWEEP_TARGET_WORDS)] + [str(k) for k in REINIT_SWEEP_K]
    result = subprocess.run(args, capture_output=True, text=True, check=True)
    print(result.stdout)
    return _parse_rows(result.stdout, "K")


def find_crossover(rows: list[dict], n_key: str, addr_key: str, philox_key: str):
    """Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word."""
    prev = None
    for row in rows:
        ratio = row[addr_key] / row[philox_key]
        if prev is not None:
            prev_ratio = prev[addr_key] / prev[philox_key]
            if prev_ratio >= 1.0 > ratio:
                # linear interp in log-N space between prev and row
                import math
                x0, x1 = math.log(prev[n_key]), math.log(row[n_key])
                y0, y1 = prev_ratio, ratio
                frac = (1.0 - y0) / (y1 - y0)
                return math.exp(x0 + frac * (x1 - x0))
        prev = row
    return None


def main():
    print("=== verify (Philox KAT check) ===")
    if not run_verify():
        print("KAT verification FAILED -- stopping, do not trust any benchmark numbers.")
        sys.exit(1)

    print("=== init-cost ===")
    init_cost = run_init_cost()
    (HERE / "tahap4_results_init-cost.json").write_text(json.dumps(init_cost, indent=2))

    print("=== throughput ===")
    throughput_rows = run_throughput()
    n_star = find_crossover(throughput_rows, "N", "addr_ns_per_word", "philox_ns_per_word")
    (HERE / "tahap4_results_throughput.json").write_text(json.dumps(
        {"rows": throughput_rows, "n_star_estimate": n_star}, indent=2))
    print(f"N* (throughput break-even) ~= {n_star:.1f}" if n_star else "N* not found in swept range")

    print("=== reinit-sweep ===")
    reinit_rows = run_reinit_sweep()
    k_star = find_crossover(reinit_rows, "K", "addr_ns_per_word_steadystate", "philox_ns_per_word")
    (HERE / "tahap4_results_reinit-sweep.json").write_text(json.dumps(
        {"rows": reinit_rows, "k_star_estimate": k_star}, indent=2))
    print(f"K* (reinit-sweep break-even) ~= {k_star:.1f}" if k_star else "K* not found in swept range")


if __name__ == "__main__":
    main()
