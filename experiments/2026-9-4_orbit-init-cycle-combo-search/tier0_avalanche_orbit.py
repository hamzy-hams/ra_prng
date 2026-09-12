"""Tier 0: avalanche gate for the orbit init x cycle combo search, min-per-bit
variant -- port of
../2026-9-2_singleblock-cycle-combo-search/tier0_avalanche.py to the 32
orbit combos (recipes_orbit.py). Same threshold band, same rationale
(../2026-8-26_operation-pruning-research/quality_gate.py's
avalanche_gate_min_bit).

Captures orbit_round1 (single K=1 word) for base_seed and each of its 32
single-bit-flipped neighbors, and requires the Hamming-distance fraction to
stay in [LOW, HIGH] on average and never drop below MIN_BIT_FLOOR for any
single bit. All 32 combos are evaluated directly (no upfront filtering
needed, unlike singleblock's 108-candidate space).
"""

from __future__ import annotations

import json
from pathlib import Path

from recipes_orbit import all_combos, combo_id, combo_describe
from combo_prng_orbit import orbit_round1

LOW, HIGH, MIN_BIT_FLOOR = 0.3, 0.7, 0.2
BASE_SEED = 1

HERE = Path(__file__).parent
OUT_PATH = HERE / "tier0_survivors_orbit.jsonl"


def hamming(x: int, y: int) -> int:
    return bin(x ^ y).count("1")


def avalanche_k1(init_id: str, cycle_id: str, base_seed: int = BASE_SEED) -> dict:
    baseline = orbit_round1(base_seed, init_id, cycle_id)
    fracs = []
    for bit in range(32):
        mutated = orbit_round1(base_seed ^ (1 << bit), init_id, cycle_id)
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
    for init_id, cycle_id in all_combos():
        stats = avalanche_k1(init_id, cycle_id)
        cid = combo_id(init_id, cycle_id)
        entry = {
            "id": cid,
            "init_id": init_id,
            "cycle_id": cycle_id,
            "description": combo_describe(init_id, cycle_id),
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
    for e in rejected:
        print(f"  REJECT {e['id']}: mean={e['mean_fraction']:.4f} min_bit={e['min_bit_fraction']:.4f} -- {e['description']}")

    return all_results, survivors


if __name__ == "__main__":
    main()
