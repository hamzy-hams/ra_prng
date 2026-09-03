"""Tier 0: avalanche gate, min-per-bit variant (see
../2026-8-26_operation-pruning-research/quality_gate.py's avalanche_gate_min_bit
for the reused threshold band and the rationale for min-bit over mean-only).

Departure from that reused pattern, made explicit (see HANDOVER.md and
recipes.py's module docstring): HANDOVER specifies capturing "output K=1" --
a single word from the single i=255 round -- not a full 255-word cycle. With
a single-word capture there is no list of positions to average over; each of
the 32 seed-bit flips produces exactly one Hamming distance value against the
baseline single word.

Only the 108 (width, finalizer) combos are evaluated -- slot 3 (extra
inject) is provably inert at K=1, see recipes.py. Pure Python is fast enough
for the whole space (108 x 33 cheap 32-bit-arithmetic calls).
"""

from __future__ import annotations

import json
from pathlib import Path

from recipes import all_tier01_candidates, candidate_id, candidate_describe
from combo_prng import round1

LOW, HIGH, MIN_BIT_FLOOR = 0.3, 0.7, 0.2
BASE_SEED = 1

HERE = Path(__file__).parent
OUT_PATH = HERE / "tier0_survivors.jsonl"


def hamming(x: int, y: int) -> int:
    return bin(x ^ y).count("1")


def avalanche_k1(width: int, finalizer_idx: int, base_seed: int = BASE_SEED) -> dict:
    baseline = round1(base_seed, width, finalizer_idx)
    fracs = []
    for bit in range(32):
        mutated = round1(base_seed ^ (1 << bit), width, finalizer_idx)
        fracs.append(hamming(baseline, mutated) / 32.0)
    mean_fraction = sum(fracs) / 32.0
    min_bit_fraction = min(fracs)
    passed = (LOW <= mean_fraction <= HIGH) and (min_bit_fraction >= MIN_BIT_FLOOR)
    return {
        "mean_fraction": mean_fraction,
        "min_bit_fraction": min_bit_fraction,
        "passed": passed,
    }


def main():
    all_results = []
    survivors = []
    for width, fidx in all_tier01_candidates():
        stats = avalanche_k1(width, fidx)
        cid = candidate_id(width, fidx, 0)
        entry = {
            "id": cid,
            "width": width,
            "finalizer_idx": fidx,
            "description": candidate_describe(width, fidx, 0),
            **stats,
        }
        all_results.append(entry)
        if stats["passed"]:
            survivors.append(entry)

    with OUT_PATH.open("w") as f:
        for entry in survivors:
            f.write(json.dumps(entry) + "\n")

    n_total = len(all_results)
    n_pass = len(survivors)
    print(f"Tier 0: {n_pass}/{n_total} candidates passed avalanche gate "
          f"(low={LOW} high={HIGH} min_bit_floor={MIN_BIT_FLOOR})")
    print(f"Survivors written to {OUT_PATH}")

    rejected = [e for e in all_results if not e["passed"]]
    print(f"Rejected: {len(rejected)}")
    for e in rejected[:10]:
        print(f"  REJECT {e['id']}: mean={e['mean_fraction']:.4f} min_bit={e['min_bit_fraction']:.4f} -- {e['description']}")
    if len(rejected) > 10:
        print(f"  ... and {len(rejected) - 10} more")

    return all_results, survivors


if __name__ == "__main__":
    main()
