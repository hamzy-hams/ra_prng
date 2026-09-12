"""Tier 0: avalanche gate for the weyl_fmix32 / weyl_triple32 candidates,
port of ../2026-9-4_orbit-init-warmup-fix/tier0_avalanche_warmup.py. Same
threshold band, same rationale: single K=1 output word (j=0) for base_seed
and each of its 32 single-bit-flipped key neighbors must diffuse well.

FAIL here -> stop, do not proceed to PractRand (HANDOVER.md section 6 step
2 / feedback_no_reseed_on_fail discipline: d0_realround and the MUL-combine
candidate were both rejected at this exact gate in prior sessions).
"""

from __future__ import annotations

import json
from pathlib import Path

from combo_prng_weylfinal import weyl_core, make_finalizer

LOW, HIGH, MIN_BIT_FLOOR = 0.3, 0.7, 0.2
BASE_SEED = 1
# Candidate A (fmix32) FAILED Tier 1 decisively 2026-09-05 (see RESULTS.md):
# FAIL K=1+Weyl @1GB, FAIL K=255 both orderings @512MB. Per HANDOVER.md
# section 4 ("uji A dulu, B hanya kalau A FAIL"), Candidate B now in scope.
CANDIDATES = ["fmix32", "triple32"]

HERE = Path(__file__).parent
OUT_PATH = HERE / "tier0_survivors_weylfinal.jsonl"


def hamming(x: int, y: int) -> int:
    return bin(x ^ y).count("1")


def orbit_round1(key: int, finalizer_name: str) -> int:
    finalizer = make_finalizer(finalizer_name)
    return weyl_core(key, 1, finalizer)[0]


def avalanche_k1(candidate: str, base_seed: int = BASE_SEED) -> dict:
    baseline = orbit_round1(base_seed, candidate)
    fracs = []
    for bit in range(32):
        mutated = orbit_round1(base_seed ^ (1 << bit), candidate)
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
    for candidate in CANDIDATES:
        stats = avalanche_k1(candidate)
        entry = {"id": candidate, "candidate": candidate, **stats}
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

    for e in all_results:
        status = "PASS" if e["passed"] else "REJECT"
        print(f"  {status} {e['id']}: mean={e['mean_fraction']:.4f} min_bit={e['min_bit_fraction']:.4f}")

    return all_results, survivors


if __name__ == "__main__":
    main()
