"""Tier 0: avalanche gate for the orbit-ad-mul-combine-fix experiment, port of
../2026-9-4_orbit-o-fmix-fix/tier0_avalanche_ofmix.py to the single
ad_mul_combine candidate (only one candidate this time, not multiple
variants -- user asked for exactly one change, "cukup itu").
"""

from __future__ import annotations

import json
from pathlib import Path

from combo_prng_admul import orbit_round1

LOW, HIGH, MIN_BIT_FLOOR = 0.3, 0.7, 0.2
BASE_SEED = 1
CANDIDATE_ID = "ad_mul_combine"

HERE = Path(__file__).parent
OUT_PATH = HERE / "tier0_survivors_admul.jsonl"


def hamming(x: int, y: int) -> int:
    return bin(x ^ y).count("1")


def avalanche_k1(base_seed: int = BASE_SEED) -> dict:
    baseline = orbit_round1(base_seed)
    fracs = []
    for bit in range(32):
        mutated = orbit_round1(base_seed ^ (1 << bit))
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
    stats = avalanche_k1()
    entry = {"id": CANDIDATE_ID, **stats}

    with OUT_PATH.open("w") as f:
        if stats["passed"]:
            f.write(json.dumps(entry) + "\n")

    status = "PASS" if stats["passed"] else "REJECT"
    print(f"Tier 0: {status} {CANDIDATE_ID} "
          f"(low={LOW} high={HIGH} min_bit_floor={MIN_BIT_FLOOR})")
    print(f"  mean_fraction={stats['mean_fraction']:.4f} "
          f"min_bit_fraction={stats['min_bit_fraction']:.4f}")
    print(f"Survivors (if any) written to {OUT_PATH}")

    return entry


if __name__ == "__main__":
    main()
