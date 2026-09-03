"""Pareto frontier selection (HANDOVER.md sect 4) over the 11 combo-search
winners, combining Axis A (speed: winners_bench_run.py) and Axis B
(avalanche safety: avalanche_multiseed.py).

Three objectives (all must be simultaneously undominated):
  1. ns_per_word at K=1 (min better) -- worst-case reinit cost.
  2. continuous-stream MB/s (max better) -- best-case amortized throughput.
  3. min(min_bit_fraction) across the 64 Axis B seeds (max better) --
     worst-case avalanche safety across seeds, not an average.

Candidates that failed Axis B criterion 1 (normality) or criterion 3
(weak-bit concentration) are excluded from the frontier outright per
HANDOVER sect 4, even if their raw Axis A numbers would otherwise put them
on it. A criterion-2-only failure (an outlier seed below the 0.2 floor)
would NOT be a hard exclusion by itself -- it instead shows up directly in
objective 3 (min_bit_fraction_worst) and is judged by ordinary Pareto
dominance (no such candidate exists in this run: 0/11 breached the floor).
"""
from __future__ import annotations

import json
from pathlib import Path

HERE = Path(__file__).parent


def load_jsonl(path: Path) -> list[dict]:
    with open(path) as f:
        return [json.loads(line) for line in f]


def dominates(a: dict, b: dict) -> bool:
    """True if candidate a dominates candidate b: a is >= b on all 3
    objectives (in their better-direction) and strictly better on >= 1."""
    a_ge = (
        a["k1_ns_per_word"] <= b["k1_ns_per_word"]
        and a["stream_mb_per_s"] >= b["stream_mb_per_s"]
        and a["min_bit_fraction_worst"] >= b["min_bit_fraction_worst"]
    )
    a_gt = (
        a["k1_ns_per_word"] < b["k1_ns_per_word"]
        or a["stream_mb_per_s"] > b["stream_mb_per_s"]
        or a["min_bit_fraction_worst"] > b["min_bit_fraction_worst"]
    )
    return a_ge and a_gt


def main():
    axis_a = {r["core"]: r for r in load_jsonl(HERE / "axis_a_final.jsonl")}
    axis_b = {r["candidate"]: r for r in load_jsonl(HERE / "avalanche_multiseed_results.jsonl")}

    winners = [c for c in axis_a if c != "singleblock"]

    combined = []
    for cid in winners:
        a = axis_a[cid]
        b = axis_b[cid]
        combined.append({
            "candidate": cid,
            "k1_ns_per_word": a["k1_ns_per_word_min_of_7"],
            "k255_ns_per_word": a["k255_ns_per_word_avg2"],
            "stream_mb_per_s": a["stream_mb_per_s_avg2"],
            "min_bit_fraction_worst": b["min_bit_fraction_worst"],
            "failed_axis_b_hard": b["failed_axis_b_hard"],
            "axis_b_reasons": b["reasons"],
        })

    eligible = [c for c in combined if not c["failed_axis_b_hard"]]
    excluded = [c for c in combined if c["failed_axis_b_hard"]]

    frontier = []
    for cand in eligible:
        if not any(dominates(other, cand) for other in eligible if other is not cand):
            frontier.append(cand)

    print(f"Axis B eligible (crit1+crit3 pass): {len(eligible)}/{len(combined)}")
    for c in excluded:
        print(f"  EXCLUDED {c['candidate']}: {'; '.join(c['axis_b_reasons'])}")
    print()
    print(f"Pareto frontier: {len(frontier)} candidate(s)")
    for c in sorted(frontier, key=lambda c: c["k1_ns_per_word"]):
        print(f"  {c['candidate']:12s} K=1={c['k1_ns_per_word']:.3f} ns/word  "
              f"stream={c['stream_mb_per_s']:.1f} MB/s  "
              f"min_bit_worst={c['min_bit_fraction_worst']:.4f}")

    baseline = axis_a["singleblock"]
    print(f"\nbaseline (singleblock): K=1={baseline['k1_ns_per_word_min_of_7']:.3f} ns/word  "
          f"stream={baseline['stream_mb_per_s_avg2']:.1f} MB/s")

    out = {
        "eligible": eligible,
        "excluded": excluded,
        "frontier": frontier,
        "baseline": baseline,
    }
    out_path = HERE / "pareto_result.json"
    with open(out_path, "w") as f:
        json.dump(out, f, indent=2)
    print(f"\nwrote {out_path}")


if __name__ == "__main__":
    main()
