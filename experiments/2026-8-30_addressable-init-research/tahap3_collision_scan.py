"""Tahap 3, Q2 (re-pointed): cross-key collision/overlap scan for
`winner_wired_addressable`, adapted from
../2026-8-29_parallelization-research/collision_scan.py.

Never modifies collision_scan.py in place. `scan_prefix()`/`scan_blocksweep()`
read `WINNER_BIN` as a genuine module-global inside their worker closures
(looked up at call time, not baked in as a default argument), so
monkeypatching `collision_scan.WINNER_BIN` after import correctly re-points
every call -- unlike cross_correlation.py's build_group() (see
tahap3_cross_correlation.py's docstring for why that one needed a rewrite
instead).

Does NOT call collision_scan.main() (it writes results via `HERE` from
common.py, which always resolves to the 2026-8-29 folder) -- this script
writes its own results into this folder instead.
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent / "2026-8-29_parallelization-research"))
import collision_scan  # noqa: E402
from common import TIERS_Q2, TIERS_Q2_RANDOM, random_seeds  # noqa: E402

HERE = Path(__file__).parent
ADDR_BIN = HERE / "winner_wired_addressable"
collision_scan.WINNER_BIN = ADDR_BIN  # re-point the reused worker functions


def run_sequential(tier: str) -> dict:
    m_keys, v_words = TIERS_Q2[tier]
    keys = list(range(m_keys))
    print(f"=== tahap3_collision_scan sequential tier={tier}: M={m_keys} (addressable) ===")
    prefix = collision_scan.scan_prefix(keys, n_words=64)
    print(f"  prefix: {prefix['collisions_found']} collisions, "
          f"digest-collision-prob~{prefix['digest_collision_prob_estimate']:.2e}")
    blocksweep = collision_scan.scan_blocksweep(keys, v_words)
    print(f"  blocksweep: {blocksweep['collisions_found']} collisions, "
          f"digest-collision-prob~{blocksweep['digest_collision_prob_estimate']:.2e}")
    return {"seed_mode": "sequential", "tier": tier, "prefix": prefix, "blocksweep": blocksweep}


def run_random(tier: str) -> dict:
    spec = TIERS_Q2_RANDOM[tier]
    v_words = spec["v_words"]
    prefix_keys = random_seeds(spec["prefix_m"], rng_seed=42)
    blocksweep_keys = random_seeds(spec["blocksweep_m"], rng_seed=43)
    print(f"=== tahap3_collision_scan random tier={tier}: prefix_M={spec['prefix_m']} "
          f"blocksweep_M={spec['blocksweep_m']} (addressable) ===")
    prefix = collision_scan.scan_prefix(prefix_keys, n_words=64)
    print(f"  prefix: {prefix['collisions_found']} collisions, "
          f"digest-collision-prob~{prefix['digest_collision_prob_estimate']:.2e}")
    blocksweep = collision_scan.scan_blocksweep(blocksweep_keys, v_words)
    print(f"  blocksweep: {blocksweep['collisions_found']} collisions, "
          f"digest-collision-prob~{blocksweep['digest_collision_prob_estimate']:.2e}")
    return {"seed_mode": "random", "tier": tier, "prefix": prefix, "blocksweep": blocksweep}


def main():
    for tier in (sys.argv[1:] or ["smoke", "full"]):
        for result in (run_sequential(tier), run_random(tier)):
            out_path = HERE / f"tahap3_collision_scan_results_{result['seed_mode']}_{tier}.json"
            out_path.write_text(json.dumps(result, indent=2))
            print(f"Wrote {out_path}\n")


if __name__ == "__main__":
    main()
