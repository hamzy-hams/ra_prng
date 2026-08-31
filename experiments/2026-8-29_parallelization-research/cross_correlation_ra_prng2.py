"""Checkpoint follow-up (2026-08-31), Q1 Method A re-pointed at `ra_prng2.c`
(paper-exact, `src/ra_prng2/c/ra_prng2.c`) instead of `winner_wired_v2`
(pruned+wired). Adapted from cross_correlation.py.

Never modifies cross_correlation.py in place. Reuses `analyze_group()`
UNCHANGED -- only `build_group()` is rewritten to pass
`binary=RA_PRNG2_BIN` explicitly, for the same reason
`tahap3_cross_correlation.py` (../2026-8-30_addressable-init-research/)
does it: `stream_values`'s `binary=WINNER_BIN` default argument is already
bound to the winner_wired_v2 path at common.py's import time, so
monkeypatching `common.WINNER_BIN` afterwards would not change it.

Purpose: does pruning+wiring (operand-position-search) change cross-stream
independence relative to the paper-exact original? If `ra_prng2.c` shows
the same near-zero flagged rate as `winner_wired_v2` did, that's evidence
pruning+wiring didn't introduce/remove cross-stream correlation. Does NOT
call cross_correlation.main() (it writes results via `HERE`, always the
same value here, but keeping its own run()/main() mirrors the addressable
Tahap 3 precedent for clarity and to avoid coupling to the original's
argv handling).
"""

from __future__ import annotations

import json
import random
import sys

import numpy as np

from common import HERE, TIERS_Q1A, RA_PRNG2_BIN, ensure_ra_prng2_cli, stream_values
from cross_correlation import ALPHA, analyze_group  # noqa: F401  (reused unmodified)


def build_group_ra_prng2(seeds: list[int], n: int) -> np.ndarray:
    return np.stack([stream_values(s, n, binary=RA_PRNG2_BIN).astype(np.float64) for s in seeds])


def run(tier: str) -> dict:
    ensure_ra_prng2_cli()
    k, n = TIERS_Q1A[tier]
    print(f"=== cross_correlation_ra_prng2 tier={tier}: K={k} n={n:,} ===")

    adjacent_seeds = list(range(k))
    control_seeds = random.Random(42).sample(range(0, 2**32), k)

    adjacent_data = build_group_ra_prng2(adjacent_seeds, n)
    control_data = build_group_ra_prng2(control_seeds, n)

    return {
        "tier": tier,
        "binary": "ra_prng2 (paper-exact)",
        "adjacent": analyze_group(adjacent_data, "adjacent"),
        "control": analyze_group(control_data, "control"),
    }


def main():
    tier = sys.argv[1] if len(sys.argv) > 1 else "full"
    result = run(tier)
    out_path = HERE / f"cross_correlation_ra_prng2_results_{tier}.json"
    out_path.write_text(json.dumps(result, indent=2))
    print(f"\nWrote {out_path}")

    adj_rate = result["adjacent"]["flagged_count"] / result["adjacent"]["m_pairs"]
    ctl_rate = result["control"]["flagged_count"] / result["control"]["m_pairs"]
    print(f"\nadjacent flagged rate={adj_rate:.4f} vs control flagged rate={ctl_rate:.4f} "
          f"(expected under H0: ~{ALPHA:.4f} family-wise, so per-test rate near 0)")


if __name__ == "__main__":
    main()
