"""Tahap 5: orchestrator for tahap5_bench / tahap5_bench_zmm512 /
tahap5_bench_unroll (C harnesses, do the actual timed work in-process). Forks
tahap4_benchmark.py's structure -- reuses its THROUGHPUT_N/REINIT_SWEEP_K
sweep points verbatim and its find_crossover() logic unchanged, so N*/K*
here are apples-to-apples with tahap4_results_*.json (N*~=49.2, K*~=71).

Three binaries, three isolated questions (see HANDOVER_TAHAP5.md section 4):
  tahap5_bench         (-O3 -march=native)                    -- Rank 2 (v1_rolv
                                                                   vs v0_baseline)
  tahap5_bench_zmm512  (-O3 -march=native -mprefer-vector-width=512) -- Rank 1,
                                                                   v0_baseline only
  tahap5_bench_unroll  (-O3 -march=native -funroll-loops)      -- Rank 3 flag-only,
                                                                   v0_baseline only

Does not modify tahap4_bench.c, tahap4_benchmark.py, winner_wired_addressable.c,
or tahap5_bench.c in place -- pure orchestration script, all precise timing
happens inside the C binaries via clock_gettime(CLOCK_MONOTONIC, ...).
"""

from __future__ import annotations

import json
import math
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).parent

INIT_COST_REPEATS = 5_000_000
# Identical to tahap4_benchmark.py -- required for apples-to-apples N*/K*.
THROUGHPUT_N = [1, 2, 4, 8, 16, 24, 32, 48, 64, 80, 96, 128, 192, 256,
                512, 1024, 4096, 65536, 1_000_000, 10_000_000, 100_000_000]
REINIT_SWEEP_TARGET_WORDS = 20_000_000
REINIT_SWEEP_K = [1, 2, 4, 8, 16, 24, 32, 48, 64, 80, 96, 128, 192, 256,
                   512, 1024, 4096, 65536]

# (binary, [variants to run]) -- Rank 1 / Rank 3 binaries isolate the flag
# effect cleanly by only ever running v0_baseline through them.
BINARIES: list[tuple[str, list[str]]] = [
    ("tahap5_bench", ["v0_baseline", "v1_rolv"]),
    ("tahap5_bench_zmm512", ["v0_baseline"]),
    ("tahap5_bench_unroll", ["v0_baseline"]),
]


def run_verify(bin_path: Path) -> bool:
    result = subprocess.run([str(bin_path), "verify"], capture_output=True, text=True)
    print(result.stdout)
    return result.returncode == 0


def run_init_cost(bin_path: Path, variant: str) -> dict:
    result = subprocess.run(
        [str(bin_path), "init-cost", variant, str(INIT_COST_REPEATS)],
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


def run_throughput(bin_path: Path, variant: str) -> list[dict]:
    args = [str(bin_path), "throughput", variant] + [str(n) for n in THROUGHPUT_N]
    result = subprocess.run(args, capture_output=True, text=True, check=True)
    print(result.stdout)
    return _parse_rows(result.stdout, "N")


def run_reinit_sweep(bin_path: Path, variant: str) -> list[dict]:
    args = [str(bin_path), "reinit-sweep", variant, str(REINIT_SWEEP_TARGET_WORDS)] + [str(k) for k in REINIT_SWEEP_K]
    result = subprocess.run(args, capture_output=True, text=True, check=True)
    print(result.stdout)
    return _parse_rows(result.stdout, "K")


def find_crossover(rows: list[dict], n_key: str, addr_key: str, philox_key: str):
    """Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word.
    Identical logic to tahap4_benchmark.py -- unchanged."""
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
    summary = []  # (label, n_star, k_star, init_ns)

    for binary_name, variants in BINARIES:
        bin_path = HERE / binary_name
        if not bin_path.exists():
            print(f"SKIP {binary_name}: binary not found (compile first)")
            continue

        print(f"\n{'=' * 20} {binary_name} {'=' * 20}")
        print(f"=== verify ({binary_name}) ===")
        if not run_verify(bin_path):
            print(f"KAT verification FAILED for {binary_name} -- stopping, do not trust its numbers.")
            continue

        for variant in variants:
            label = f"{binary_name}:{variant}"
            print(f"\n--- {label} ---")

            print(f"=== init-cost ({label}) ===")
            init_cost = run_init_cost(bin_path, variant)
            (HERE / f"tahap5_results_init-cost_{binary_name}_{variant}.json").write_text(
                json.dumps(init_cost, indent=2))

            print(f"=== throughput ({label}) ===")
            throughput_rows = run_throughput(bin_path, variant)
            n_star = find_crossover(throughput_rows, "N", "addr_ns_per_word", "philox_ns_per_word")
            (HERE / f"tahap5_results_throughput_{binary_name}_{variant}.json").write_text(json.dumps(
                {"rows": throughput_rows, "n_star_estimate": n_star}, indent=2))
            print(f"N* ({label}) ~= {n_star:.1f}" if n_star else f"N* not found in swept range ({label})")

            print(f"=== reinit-sweep ({label}) ===")
            reinit_rows = run_reinit_sweep(bin_path, variant)
            k_star = find_crossover(reinit_rows, "K", "addr_ns_per_word_steadystate", "philox_ns_per_word")
            (HERE / f"tahap5_results_reinit-sweep_{binary_name}_{variant}.json").write_text(json.dumps(
                {"rows": reinit_rows, "k_star_estimate": k_star}, indent=2))
            print(f"K* ({label}) ~= {k_star:.1f}" if k_star else f"K* not found in swept range ({label})")

            summary.append((label, n_star, k_star, init_cost["addr_ns_per_init"]))

    print(f"\n{'=' * 20} RINGKASAN {'=' * 20}")
    print(f"Baseline Tahap 4 (winner_wired_addressable, hardcoded init): "
          f"init=49.87ns N*~=49.2 K*~=71")
    for label, n_star, k_star, init_ns in summary:
        n_str = f"{n_star:.1f}" if n_star else "n/a"
        k_str = f"{k_star:.1f}" if k_star else "n/a"
        print(f"{label:40s} init={init_ns:8.3f}ns  N*={n_str:>6s}  K*={k_str:>6s}")


if __name__ == "__main__":
    main()
