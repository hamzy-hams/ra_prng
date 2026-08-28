"""Per-bit avalanche stats (cycle 1) for all 13 winners from RESULTS.md's
table, generic over Wiring -- extends avalanche_heatmap_winner.py's
methodology (which only covers winner #5) to all 13 for a head-to-head
comparison. Pure Python, no C needed for this axis.
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent /
                       "2026-8-26_operation-pruning-research"))
from quality_gate import avalanche_stats  # noqa: E402

from wiring import Wiring  # noqa: E402
from wired_prng import stream  # noqa: E402

from other_winners_gen import WINNERS  # noqa: E402

WINNER5 = ("v05", Wiring(a_xor_operand="d", c_shift_operand="b",
                          rotc_amount_source="b", rotc_xor_operand="a"))

ALL13 = sorted(WINNERS + [WINNER5], key=lambda t: t[0])


def capture(seed: int, w: Wiring) -> list[int]:
    out = []
    for c in stream(seed, 1, w):
        out.append(c)
    return out


def main():
    results = {}
    for name, w in ALL13:
        stats = avalanche_stats(lambda seed, w=w: capture(seed, w), base_seed=1)
        results[name] = {
            "wiring": f"a_xor={w.a_xor_operand} c_shift={w.c_shift_operand} "
                      f"rotc_amount={w.rotc_amount_source} rotc_xor={w.rotc_xor_operand}",
            "overall": stats["overall_mean_hamming_fraction"],
            "min_bit": stats["min_bit_fraction"],
            "per_bit": stats["per_bit_fractions"],
        }
        print(f"{name}: overall={stats['overall_mean_hamming_fraction']:.6f} "
              f"min_bit={stats['min_bit_fraction']:.6f}")

    out_path = Path(__file__).parent / "other_winners_avalanche.json"
    out_path.write_text(json.dumps(results, indent=2))
    print(f"\nSaved to {out_path}")


if __name__ == "__main__":
    main()
