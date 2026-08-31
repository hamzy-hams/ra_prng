"""Tahap 6: orchestrator for tahap6_bench (C harness does the actual timed
work in-process). Reuses tahap4/5_benchmark.py's THROUGHPUT_N/REINIT_SWEEP_K
sweep points, filtered to the <=255 subset that ra_core_singleblock's hard
scope boundary allows (rng<=255, single block, no reseed).

Does NOT modify tahap4_bench.c, tahap4_benchmark.py, tahap5_bench.c,
tahap5_benchmark.py, winner_wired_addressable.c, or tahap6_bench.c in
place -- pure orchestration script, all precise timing happens inside the
C binary via clock_gettime(CLOCK_MONOTONIC, ...).

Runs `tahap6_bench validate` FIRST and refuses to run any benchmark mode
if it fails -- per HANDOVER_TAHAP5.md's established convention, no speed
number is trustworthy without the bit-identical gate passing first.
"""

from __future__ import annotations

import json
import math
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).parent
BIN = HERE / "tahap6_bench"

INIT_COST_REPEATS = 5_000_000
# Identical prefix to tahap4/5_benchmark.py's THROUGHPUT_N/REINIT_SWEEP_K,
# truncated to the <=255 subset ra_core_singleblock's hard guard allows.
THROUGHPUT_N = [1, 2, 4, 8, 16, 24, 32, 48, 64, 80, 96, 128, 192]
REINIT_SWEEP_TARGET_WORDS = 20_000_000
REINIT_SWEEP_K = [1, 2, 4, 8, 16, 24, 32, 48, 64, 80, 96, 128, 192]

CORES = ["baseline", "singleblock"]


def run_validate() -> bool:
    result = subprocess.run([str(BIN), "validate"], capture_output=True, text=True)
    print(result.stdout)
    if result.returncode != 0:
        print(result.stderr, file=sys.stderr)
    return result.returncode == 0


def run_verify() -> bool:
    result = subprocess.run([str(BIN), "verify"], capture_output=True, text=True)
    print(result.stdout)
    return result.returncode == 0


def run_init_cost(core: str) -> dict:
    result = subprocess.run(
        [str(BIN), "init-cost", core, str(INIT_COST_REPEATS)],
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


def run_throughput(core: str) -> list[dict]:
    args = [str(BIN), "throughput", core] + [str(n) for n in THROUGHPUT_N]
    result = subprocess.run(args, capture_output=True, text=True, check=True)
    print(result.stdout)
    return _parse_rows(result.stdout, "N")


def run_reinit_sweep(core: str) -> list[dict]:
    args = [str(BIN), "reinit-sweep", core, str(REINIT_SWEEP_TARGET_WORDS)] + [str(k) for k in REINIT_SWEEP_K]
    result = subprocess.run(args, capture_output=True, text=True, check=True)
    print(result.stdout)
    return _parse_rows(result.stdout, "K")


def find_crossover(rows: list[dict], n_key: str, addr_key: str, philox_key: str):
    """Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word.
    Identical logic to tahap4/5_benchmark.py -- unchanged. Note: for the
    <=255-only sweep here, a crossover vs Philox may simply not exist in
    range (that's a valid, reportable outcome, not a bug)."""
    prev = None
    for row in rows:
        ratio = row[addr_key] / row[philox_key]
        if prev is not None:
            prev_ratio = prev[addr_key] / prev[philox_key]
            if prev_ratio >= 1.0 > ratio:
                x0, x1 = math.log(prev[n_key]), math.log(row[n_key])
                y0, y1 = prev_ratio, ratio
                frac = (1.0 - y0) / (y1 - y0)
                return math.exp(x0 + frac * (x1 - x0))
        prev = row
    return None


def main():
    if not BIN.exists():
        print(f"ERROR: {BIN} not found -- compile tahap6_bench.c first "
              f"(gcc -O3 -march=native -std=gnu17 -include stdalign.h "
              f"tahap6_bench.c -o tahap6_bench -Wall -Wextra)", file=sys.stderr)
        sys.exit(1)

    print("=== verify (Philox KAT) ===")
    if not run_verify():
        print("KAT verification FAILED -- stopping, do not trust any numbers.", file=sys.stderr)
        sys.exit(1)

    print("=== validate (ra_core_singleblock vs ra_core_baseline, rng in [1,255]) ===")
    if not run_validate():
        print("VALIDATION FAILED -- ra_core_singleblock is NOT bit-identical to "
              "ra_core_baseline. Refusing to run benchmarks: treat this as a new, "
              "unvalidated formula (would need full Tahap 0 + Tahap 3 re-validation "
              "before any speed claim means anything). Stopping.", file=sys.stderr)
        sys.exit(1)

    summary = []  # (core, n_star, k_star, init_ns)
    for core in CORES:
        print(f"\n{'=' * 20} core={core} {'=' * 20}")

        print(f"=== init-cost ({core}) ===")
        init_cost = run_init_cost(core)
        (HERE / f"tahap6_results_init-cost_{core}.json").write_text(
            json.dumps(init_cost, indent=2))

        print(f"=== throughput ({core}) ===")
        throughput_rows = run_throughput(core)
        n_star = find_crossover(throughput_rows, "N", "addr_ns_per_word", "philox_ns_per_word")
        (HERE / f"tahap6_results_throughput_{core}.json").write_text(json.dumps(
            {"rows": throughput_rows, "n_star_estimate": n_star}, indent=2))
        print(f"N* ({core}) ~= {n_star:.1f}" if n_star else f"N* not found in swept range ({core})")

        print(f"=== reinit-sweep ({core}) ===")
        reinit_rows = run_reinit_sweep(core)
        k_star = find_crossover(reinit_rows, "K", "addr_ns_per_word_steadystate", "philox_ns_per_word")
        (HERE / f"tahap6_results_reinit-sweep_{core}.json").write_text(json.dumps(
            {"rows": reinit_rows, "k_star_estimate": k_star}, indent=2))
        print(f"K* ({core}) ~= {k_star:.1f}" if k_star else f"K* not found in swept range ({core})")

        summary.append((core, n_star, k_star, init_cost["addr_ns_per_init"]))

    print(f"\n{'=' * 20} RINGKASAN {'=' * 20}")
    for core, n_star, k_star, init_ns in summary:
        n_str = f"{n_star:.1f}" if n_star else "n/a"
        k_str = f"{k_star:.1f}" if k_star else "n/a"
        print(f"{core:15s} init={init_ns:8.3f}ns  N*={n_str:>6s}  K*={k_str:>6s}")

    # Direct baseline-vs-singleblock comparison (same K/N points, both
    # measured in this same run) -- the number that actually answers "how
    # much faster is Tahap 6".
    print(f"\n{'=' * 20} baseline vs singleblock (direct) {'=' * 20}")
    base_reinit = json.loads((HERE / "tahap6_results_reinit-sweep_baseline.json").read_text())["rows"]
    sb_reinit = json.loads((HERE / "tahap6_results_reinit-sweep_singleblock.json").read_text())["rows"]
    base_by_k = {r["K"]: r["addr_ns_per_word_steadystate"] for r in base_reinit}
    sb_by_k = {r["K"]: r["addr_ns_per_word_steadystate"] for r in sb_reinit}
    for k in REINIT_SWEEP_K:
        if k in base_by_k and k in sb_by_k:
            b, s = base_by_k[k], sb_by_k[k]
            pct = (b - s) / b * 100.0
            print(f"K={k:5d}  baseline={b:8.3f}ns/word  singleblock={s:8.3f}ns/word  "
                  f"singleblock is {pct:+.1f}% vs baseline")


if __name__ == "__main__":
    main()
