"""Non-regression check for `ra_init_orbit` (the multikey mix-init fix
candidate, ../2026-9-1_multikey-remix-search/diag_init_keyterm_mul.c)
against `winner_wired_addressable`/`ra_core_orbit_affine`'s existing
single-key cross-correlation guarantee. Adapted from
tahap3_cross_correlation.py (../2026-8-30_addressable-init-research/),
same re-pointing pattern: `build_group` explicitly passes
`binary=CANDIDATE_BIN` because `stream_values`'s `binary=WINNER_BIN`
default argument is bound at common.py's import time.

Baseline to compare against: tahap3_cross_correlation_results_full.json
(../2026-8-30_addressable-init-research/), which validated
winner_wired_addressable/ra_core_orbit_affine originally. "No regression" =
this candidate's flagged rate profile matches that baseline (adjacent rate
~= control rate ~= near 0, both near ALPHA=0.01 family-wise).

Does NOT call cross_correlation.main() (writes into 2026-8-29 folder via
common.py's HERE) -- writes its own results into this folder instead.
"""

from __future__ import annotations

import json
import random
import sys
from pathlib import Path

from common import TIERS_Q1A, stream_values
from cross_correlation import analyze_group  # noqa: F401  (reused unmodified)

HERE = Path(__file__).parent
CANDIDATE_BIN = HERE.parent / "2026-9-1_multikey-remix-search" / "diag_orbit_regression"


def build_group(keys: list[int], n: int):
    import numpy as np
    return np.stack([stream_values(k, n, binary=CANDIDATE_BIN).astype(np.float64) for k in keys])


def run(tier: str) -> dict:
    assert CANDIDATE_BIN.exists(), f"{CANDIDATE_BIN} not found -- build it first"
    k, n = TIERS_Q1A[tier]
    print(f"=== cross_correlation_orbit_regression tier={tier}: K={k} n={n:,} (ra_init_orbit) ===")

    adjacent_keys = list(range(k))
    control_keys = random.Random(42).sample(range(0, 2**32), k)

    adjacent_data = build_group(adjacent_keys, n)
    control_data = build_group(control_keys, n)

    return {
        "tier": tier,
        "binary": "diag_orbit_regression (ra_init_orbit)",
        "adjacent": analyze_group(adjacent_data, "adjacent"),
        "control": analyze_group(control_data, "control"),
    }


def main():
    tier = sys.argv[1] if len(sys.argv) > 1 else "full"
    result = run(tier)
    out_path = HERE / f"cross_correlation_orbit_regression_results_{tier}.json"
    out_path.write_text(json.dumps(result, indent=2))
    print(f"Wrote {out_path}")
    adj_rate = result["adjacent"]["flagged_count"] / result["adjacent"]["m_pairs"]
    ctl_rate = result["control"]["flagged_count"] / result["control"]["m_pairs"]
    print(f"tier={tier}: adjacent flagged rate={adj_rate:.4f} vs control flagged rate={ctl_rate:.4f}")


if __name__ == "__main__":
    main()
