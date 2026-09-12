"""Tier 0: avalanche gate for orbit-d0-raw-cons-fix, port of
../2026-9-4_orbit-init-warmup-fix/tier0_avalanche_warmup.py to the single
d0_raw_cons variant (plus control for reference).
"""

from __future__ import annotations

import json
from pathlib import Path

from combo_prng_d0raw import orbit_round1, VARIANTS

LOW, HIGH, MIN_BIT_FLOOR = 0.3, 0.7, 0.2
BASE_SEED = 1

HERE = Path(__file__).parent
OUT_PATH = HERE / "tier0_survivors_d0raw.jsonl"


def hamming(x: int, y: int) -> int:
    return bin(x ^ y).count("1")


def avalanche_k1(variant: str, base_seed: int = BASE_SEED) -> dict:
    baseline = orbit_round1(base_seed, variant)
    fracs = []
    for bit in range(32):
        mutated = orbit_round1(base_seed ^ (1 << bit), variant)
        fracs.append(hamming(baseline, mutated) / 32.0)
    mean_fraction = sum(fracs) / 32.0
    min_bit_fraction = min(fracs)
    passed = (LOW <= mean_fraction <= HIGH) and (min_bit_fraction >= MIN_BIT_FLOOR)
    return {"mean_fraction": mean_fraction, "min_bit_fraction": min_bit_fraction, "passed": passed}


def main():
    all_results = []
    survivors = []
    for variant in VARIANTS:
        stats = avalanche_k1(variant)
        entry = {"id": variant, "variant": variant, **stats}
        all_results.append(entry)
        if stats["passed"]:
            survivors.append(entry)

    with OUT_PATH.open("w") as f:
        for entry in survivors:
            f.write(json.dumps(entry) + "\n")

    n_total = len(all_results)
    n_pass = len(survivors)
    print(f"Tier 0: {n_pass}/{n_total} variants passed avalanche gate "
          f"(low={LOW} high={HIGH} min_bit_floor={MIN_BIT_FLOOR})")
    print(f"Survivors written to {OUT_PATH}")

    for e in all_results:
        status = "PASS" if e["passed"] else "REJECT"
        print(f"  {status} {e['id']}: mean={e['mean_fraction']:.4f} min_bit={e['min_bit_fraction']:.4f}")

    return all_results, survivors


if __name__ == "__main__":
    main()
