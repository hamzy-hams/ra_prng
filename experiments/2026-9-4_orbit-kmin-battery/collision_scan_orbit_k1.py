"""experiments/2026-9-4_orbit-kmin-battery -- collision-scan for
`ra_core_orbit` at K=1 (one word per key, the multistream pattern whose
K-small defect exposure motivated this whole battery), against the FIXED
w8_f10_i0 + rolling-o formula built here (Step 0, ./BUILD.sh) from
../2026-9-4_orbit-fix-and-wideo-rolling-optimization/ra_core_v2.c.

Adapted from
../2026-9-1_production-candidate-battery/collision_scan_singleblock_k1.py
(same 2**32-space Poisson-z gate logic, unmodified -- see that file's
docstring for why K=1 needs a 2**32 birthday bound instead of the K=255
script's 2**64 hashed-digest bound). Only difference: uses this folder's
`ra_core_v2` binary with `--stream orbit <key> 1` (one process per key)
instead of a core-specific CLI, since ra_core_v2.c's CLI takes an explicit
core-name argument.
"""

from __future__ import annotations

import json
import math
import os
import subprocess
import sys
import threading
from concurrent.futures import ThreadPoolExecutor
from math import comb
from pathlib import Path

HERE = Path(__file__).parent
PARALLEL_DIR = HERE.parent / "2026-8-29_parallelization-research"
sys.path.insert(0, str(PARALLEL_DIR))
from common import random_seeds  # noqa: E402

RA_CORE_V2_BIN = HERE / "ra_core_v2"  # this folder's own fresh build
BLOCK_WORDS = 1
BLOCK_BYTES = BLOCK_WORDS * 4

TIERS = {"smoke": 5_000, "full": 50_000}


def raw_collision_prob(n_items: int) -> float:
    """Birthday-bound estimate for raw 32-bit-word collision across
    n_items independent K=1 outputs: C(n_items, 2) / 2**32."""
    return comb(n_items, 2) / (2 ** 32)


def poisson_z(observed: int, expected: float) -> float:
    """Normal-approximation z-score for a Poisson(expected) count."""
    if expected <= 0:
        return float("inf") if observed > 0 else 0.0
    return (observed - expected) / math.sqrt(expected)


def scan_k1(keys: list[int], max_workers: int | None = None) -> dict:
    max_workers = max_workers or os.cpu_count() or 1
    global_seen: dict[bytes, int] = {}
    collisions: list[dict] = []
    lock = threading.Lock()

    def worker(key: int) -> None:
        result = subprocess.run(
            [str(RA_CORE_V2_BIN), "--stream", "orbit", str(key), str(BLOCK_WORDS)],
            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
        )
        assert len(result.stdout) == BLOCK_BYTES
        fp = result.stdout
        with lock:
            if fp in global_seen:
                if global_seen[fp] != key:
                    collisions.append({"key_a": global_seen[fp], "key_b": key, "word": fp.hex()})
            else:
                global_seen[fp] = key

    with ThreadPoolExecutor(max_workers=max_workers) as ex:
        list(ex.map(worker, keys))

    expected = raw_collision_prob(len(keys))
    observed = len(collisions)
    z = poisson_z(observed, expected)
    return {
        "mode": "orbit_k1_raw32", "m_keys": len(keys), "block_words": BLOCK_WORDS,
        "fingerprints": len(keys), "collisions_found": observed,
        "collisions": collisions[:50],
        "expected_collisions_32bit_space": expected,
        "poisson_z": z,
        "pass": abs(z) < 3.0,
    }


def main():
    args = sys.argv[1:]
    seed_mode = "sequential"
    if "--seeds" in args:
        i = args.index("--seeds")
        seed_mode = args[i + 1]
        del args[i:i + 2]
    tier = args[0] if args else "full"

    assert RA_CORE_V2_BIN.exists(), f"{RA_CORE_V2_BIN} not found -- run ./BUILD.sh first"
    m_keys = TIERS[tier]
    if seed_mode == "sequential":
        keys = list(range(m_keys))
        label = f"sequential tier={tier} M={m_keys}"
    elif seed_mode == "random":
        keys = random_seeds(m_keys, rng_seed=42)
        label = f"random tier={tier} M={m_keys}"
    else:
        raise SystemExit(f"unknown --seeds mode {seed_mode!r} (expected sequential|random)")

    nworkers = os.cpu_count() or 1
    print(f"=== collision_scan_orbit_k1 {label} ({nworkers} workers) ===")
    result = scan_k1(keys, max_workers=nworkers)
    verdict = "PASS" if result["pass"] else "FAIL"
    print(f"  {result['fingerprints']} fingerprints, {result['collisions_found']} collisions "
          f"(expected~{result['expected_collisions_32bit_space']:.1f} in 2^32 space, "
          f"z={result['poisson_z']:.2f}) -- {verdict}")

    out = {"seed_mode": seed_mode, "tier": tier, "orbit_k1_raw32": result}
    out_path = HERE / f"collision_scan_orbit_k1_results_{seed_mode}_{tier}.json"
    out_path.write_text(json.dumps(out, indent=2))
    print(f"\nWrote {out_path}")


if __name__ == "__main__":
    main()
