"""Tahap 3, Q1 Method A (re-pointed): Pearson cross-correlation between pairs
of independent `winner_wired_addressable --stream <key> <n>` streams, adapted
from ../2026-8-29_parallelization-research/cross_correlation.py.

Never modifies cross_correlation.py in place. Reuses `analyze_group()`
(the actual Bonferroni-corrected z-test logic) UNCHANGED -- only
`build_group()` is rewritten, because the original calls
`stream_values(s, n)` relying on `stream_values`'s `binary=WINNER_BIN`
DEFAULT ARGUMENT, which was already bound to the ORIGINAL winner_wired_v2
path at common.py's import time. Monkeypatching `common.WINNER_BIN`
afterwards would NOT change that already-bound default, so `build_group()`
must explicitly pass `binary=ADDR_BIN` instead.

Does NOT call cross_correlation.main() (it writes results via `HERE` from
common.py, which always resolves to the 2026-8-29 folder) -- this script
writes its own results into this folder instead.
"""

from __future__ import annotations

import json
import random
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent / "2026-8-29_parallelization-research"))
from common import TIERS_Q1A, stream_values  # noqa: E402
from cross_correlation import analyze_group  # noqa: E402  (reused unmodified)

HERE = Path(__file__).parent
ADDR_BIN = HERE / "winner_wired_addressable"


def build_group_addr(keys: list[int], n: int):
    import numpy as np
    return np.stack([stream_values(k, n, binary=ADDR_BIN).astype(np.float64) for k in keys])


def run(tier: str) -> dict:
    k, n = TIERS_Q1A[tier]
    print(f"=== tahap3_cross_correlation tier={tier}: K={k} n={n:,} (addressable) ===")

    adjacent_keys = list(range(k))
    control_keys = random.Random(42).sample(range(0, 2**32), k)

    adjacent_data = build_group_addr(adjacent_keys, n)
    control_data = build_group_addr(control_keys, n)

    return {
        "tier": tier,
        "adjacent": analyze_group(adjacent_data, "adjacent"),
        "control": analyze_group(control_data, "control"),
    }


def main():
    tiers = sys.argv[1:] or ["smoke", "full"]
    for tier in tiers:
        result = run(tier)
        out_path = HERE / f"tahap3_cross_correlation_results_{tier}.json"
        out_path.write_text(json.dumps(result, indent=2))
        print(f"Wrote {out_path}")
        adj_rate = result["adjacent"]["flagged_count"] / result["adjacent"]["m_pairs"]
        ctl_rate = result["control"]["flagged_count"] / result["control"]["m_pairs"]
        print(f"tier={tier}: adjacent flagged rate={adj_rate:.4f} vs control flagged rate={ctl_rate:.4f}\n")


if __name__ == "__main__":
    main()
