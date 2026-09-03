"""Axis B (HANDOVER.md sect 3): multi-seed avalanche + normality test for
the 11 Promotion Tier winners of the singleblock cycle-operation combo
search.

`avalanche_heatmap_winners_k255.py` (combo-search experiment) only measured
ONE seed (base_seed=1) -- enough to rule out the "dead bit" class of defect
documented in quality_gate.py's avalanche_stats() docstring, but not enough
to know whether the weakest/strongest bit is stable across seeds. This
script closes that gap: for each of the 11 winners x seeds 1..64
(deterministic, reproducible), capture one full K=255 cycle via each
already-compiled candidate binary's `--single <seed> 255` mode (no
recompilation), reuse quality_gate.py's avalanche_stats() (imported, not
reimplemented) to get per-seed min_bit_fraction/max_bit_fraction, then run
scipy.stats.shapiro on each 64-sample array and apply the 3 explicit HANDOVER
sect 3 criteria.

Criteria (exact wording from HANDOVER.md sect 3):
  1. Shapiro-Wilk p-value > 0.05 for BOTH the min_bit_fraction and
     max_bit_fraction arrays (fail to reject normality).
  2. No seed with min_bit_fraction < 0.2 (MIN_BIT_FLOOR, same threshold as
     quality_gate.py's avalanche_gate_min_bit default / tier0_avalanche.py).
  3. No single bit index is the argmin (weakest bit) in more than 50% (>32)
     of the 64 seeds.

Per HANDOVER.md sect 4: a candidate failing criterion 1 or 3 is excluded
from the Pareto frontier outright, even if it passes criterion 2. A
criterion-2-only failure (an extreme low outlier) is NOT a hard exclusion by
itself -- it is reported and naturally suppresses that candidate's
min(min_bit_fraction) Pareto objective instead (see pareto_select.py).

Does not touch candidates/*.c, RESULTS.md, combo_prng.py, or recipes.py
under ../2026-9-2_singleblock-cycle-combo-search/ (read-only precedent).
"""
from __future__ import annotations

import json
import struct
import subprocess
import sys
from pathlib import Path

from scipy.stats import shapiro

sys.path.insert(0, str(Path(__file__).parent.parent / "2026-8-26_operation-pruning-research"))
from quality_gate import avalanche_stats  # noqa: E402

HERE = Path(__file__).parent
CAND_DIR = HERE.parent / "2026-9-2_singleblock-cycle-combo-search" / "candidates"
MASK = 0xFFFFFFFF
N_WORDS = 255
N_SEEDS = 64
MIN_BIT_FLOOR = 0.2
ARGMIN_CONCENTRATION_LIMIT = 0.5  # > 50% of seeds sharing one weakest-bit index fails crit 3

WINNERS = [
    "w8_f8_i0", "w8_f9_i0", "w8_f10_i0", "w8_f22_i0", "w8_f24_i0",
    "w8_f25_i0", "w8_f26_i0", "w8_f27_i0", "w8_f28_i0", "w8_f29_i0",
    "w8_f33_i0",
]


def capture_cycle(bin_path: Path, seed: int) -> list[int]:
    out = subprocess.run(
        [str(bin_path), "--single", str(seed & MASK), str(N_WORDS)],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
    ).stdout
    assert len(out) == N_WORDS * 4, f"{bin_path.name}: expected {N_WORDS*4} bytes, got {len(out)}"
    return list(struct.unpack(f"<{N_WORDS}I", out))


def per_seed_stats(bin_path: Path, seed: int) -> dict:
    capture_fn = lambda s: capture_cycle(bin_path, s)  # noqa: E731
    stats = avalanche_stats(capture_fn, base_seed=seed)
    per_bit = stats["per_bit_fractions"]
    argmin_bit = min(range(32), key=lambda b: per_bit[b])
    argmax_bit = max(range(32), key=lambda b: per_bit[b])
    return {
        "seed": seed,
        "min_bit_fraction": stats["min_bit_fraction"],
        "max_bit_fraction": max(per_bit),
        "argmin_bit": argmin_bit,
        "argmax_bit": argmax_bit,
    }


def evaluate_candidate(cid: str) -> dict:
    bin_path = CAND_DIR / cid
    assert bin_path.exists(), f"{bin_path} not found"
    rows = [per_seed_stats(bin_path, seed) for seed in range(1, N_SEEDS + 1)]

    min_arr = [r["min_bit_fraction"] for r in rows]
    max_arr = [r["max_bit_fraction"] for r in rows]

    shapiro_min = shapiro(min_arr)
    shapiro_max = shapiro(max_arr)
    shapiro_min_p = float(shapiro_min.pvalue)
    shapiro_max_p = float(shapiro_max.pvalue)
    crit1_pass = bool(shapiro_min_p > 0.05 and shapiro_max_p > 0.05)

    floor_breaches = [r["seed"] for r in rows if r["min_bit_fraction"] < MIN_BIT_FLOOR]
    crit2_pass = bool(len(floor_breaches) == 0)

    argmin_hist = [0] * 32
    for r in rows:
        argmin_hist[r["argmin_bit"]] += 1
    worst_index_count = max(argmin_hist)
    crit3_pass = bool(worst_index_count <= (N_SEEDS * ARGMIN_CONCENTRATION_LIMIT))

    hard_fail = bool(not (crit1_pass and crit3_pass))  # per sect 4: crit 1 or 3 -> excluded

    reasons = []
    if not crit1_pass:
        reasons.append(
            f"crit1_normality: shapiro p(min)={shapiro_min.pvalue:.4g} "
            f"p(max)={shapiro_max.pvalue:.4g} (need both >0.05)"
        )
    if not crit2_pass:
        reasons.append(f"crit2_floor: {len(floor_breaches)} seed(s) below {MIN_BIT_FLOOR}: {floor_breaches}")
    if not crit3_pass:
        reasons.append(
            f"crit3_concentration: bit index with max argmin count = {worst_index_count}/{N_SEEDS} "
            f"(limit {int(N_SEEDS*ARGMIN_CONCENTRATION_LIMIT)})"
        )

    return {
        "candidate": cid,
        "n_seeds": N_SEEDS,
        "min_bit_fraction_worst": min(min_arr),
        "min_bit_fraction_mean": sum(min_arr) / len(min_arr),
        "max_bit_fraction_best": max(max_arr),
        "shapiro_min_pvalue": shapiro_min_p,
        "shapiro_max_pvalue": shapiro_max_p,
        "floor_breaches": floor_breaches,
        "argmin_histogram": argmin_hist,
        "crit1_normality_pass": crit1_pass,
        "crit2_floor_pass": crit2_pass,
        "crit3_concentration_pass": crit3_pass,
        "failed_axis_b_hard": hard_fail,
        "reasons": reasons,
        "rows": rows,
    }


def main():
    results = []
    for cid in WINNERS:
        res = evaluate_candidate(cid)
        results.append(res)
        status = "FAILED_AXIS_B" if res["failed_axis_b_hard"] else "OK"
        print(f"{cid:12s} [{status}] worst_min_bit={res['min_bit_fraction_worst']:.4f} "
              f"shapiro_min_p={res['shapiro_min_pvalue']:.4g} shapiro_max_p={res['shapiro_max_pvalue']:.4g} "
              f"floor_breaches={len(res['floor_breaches'])} "
              f"max_argmin_count={max(res['argmin_histogram'])}/{N_SEEDS}")
        if res["reasons"]:
            for r in res["reasons"]:
                print(f"    - {r}")

    out_path = HERE / "avalanche_multiseed_results.jsonl"
    with open(out_path, "w") as f:
        for res in results:
            f.write(json.dumps(res) + "\n")
    total_rows = sum(len(r["rows"]) for r in results)
    print(f"\nwrote {out_path} ({len(results)} candidates, {total_rows} total seed-rows, "
          f"expected {len(WINNERS)*N_SEEDS})")


if __name__ == "__main__":
    main()
