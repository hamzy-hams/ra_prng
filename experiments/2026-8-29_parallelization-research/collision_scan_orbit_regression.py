"""Non-regression check for `ra_init_orbit` (the multikey mix-init fix
candidate, ../2026-9-1_multikey-remix-search/diag_init_keyterm_mul.c)
against `winner_wired_addressable`/`ra_core_orbit_affine`'s existing
single-key collision-scan guarantee. Adapted from tahap3_collision_scan.py
(../2026-8-30_addressable-init-research/): `scan_prefix()`/
`scan_blocksweep()` read `collision_scan.WINNER_BIN` as a genuine
module-global at call time, so monkeypatching it after import re-points
every call.

Only sequential mode is run here (matches HANDOVER.md's minimum
verification bar); the random-seed 500k-key tier
(tahap3_collision_scan_results_random_full.json) is the deep one-off
validation already done for winner_wired_addressable itself and is not
re-run for this non-regression pass -- see this folder's plan notes.

Does NOT call collision_scan.main() (writes via common.py's HERE, which
always resolves to this folder anyway since this file lives here too, but
kept explicit for clarity/consistency with the cross-correlation sibling).
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

import collision_scan
from common import TIERS_Q2

HERE = Path(__file__).parent
CANDIDATE_BIN = HERE.parent / "2026-9-1_multikey-remix-search" / "diag_orbit_regression"
collision_scan.WINNER_BIN = CANDIDATE_BIN  # re-point the reused worker functions


def run_sequential(tier: str) -> dict:
    assert CANDIDATE_BIN.exists(), f"{CANDIDATE_BIN} not found -- build it first"
    m_keys, v_words = TIERS_Q2[tier]
    keys = list(range(m_keys))
    print(f"=== collision_scan_orbit_regression sequential tier={tier}: M={m_keys} (ra_init_orbit) ===")
    prefix = collision_scan.scan_prefix(keys, n_words=64)
    print(f"  prefix: {prefix['collisions_found']} collisions, "
          f"digest-collision-prob~{prefix['digest_collision_prob_estimate']:.2e}")
    blocksweep = collision_scan.scan_blocksweep(keys, v_words)
    print(f"  blocksweep: {blocksweep['collisions_found']} collisions, "
          f"digest-collision-prob~{blocksweep['digest_collision_prob_estimate']:.2e}")
    return {"seed_mode": "sequential", "tier": tier, "binary": "diag_orbit_regression (ra_init_orbit)",
            "prefix": prefix, "blocksweep": blocksweep}


def main():
    tier = sys.argv[1] if len(sys.argv) > 1 else "full"
    result = run_sequential(tier)
    out_path = HERE / f"collision_scan_orbit_regression_results_sequential_{tier}.json"
    out_path.write_text(json.dumps(result, indent=2))
    print(f"Wrote {out_path}")


if __name__ == "__main__":
    main()
