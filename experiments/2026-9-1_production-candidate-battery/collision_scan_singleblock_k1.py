"""Step 4 (PRODUCTION_READINESS_HANDOVER.md) -- collision-scan for
`ra_core_singleblock` at K=1 (one word per key, the most aggressive reinit
usage pattern this gate exists to check).

Adapted from `../2026-8-29_parallelization-research/collision_scan_ra_core_
singleblock.py`, NOT an edit of that file -- its K=255 fullblock mode stays
valid and reusable as-is. This is a separate file because K=1 needs a
different collision-probability model, not just a different BLOCK_WORDS
constant:

At K=1 the raw output is a single 32-bit word (4 bytes). The K=255 script
hashes each 255-word (8160-bit) block down to a 64-bit BLAKE2b digest and
applies a 2**64 birthday bound -- valid there because the *input* to the
hash has far more entropy than the 64-bit digest, so the digest space is
the bottleneck. At K=1 that assumption inverts: the input itself is only
32 bits, so hashing it up to 64 bits creates no entropy that wasn't there,
and the real collision space is 2**32, not 2**64. Concretely, at M=50,000
keys the *correct* expectation is `comb(50000,2)/2**32 ~= 291` collisions
-- not ~0. Applying the K=255 script's 2**64 model unmodified here would
mislabel this expected birthday-collision rate as a false FAIL.

This script therefore (a) compares the raw 4-byte output directly instead
of hashing it (hashing a 32-bit value to summarize it is pointless -- the
raw bytes ARE the fingerprint, and comparing them directly is both simpler
and exactly correct, with no risk of the hash question above at all), and
(b) scores PASS/FAIL via a Poisson z-score around the correct 2**32-space
expectation, not a "must be exactly 0" threshold.
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

SINGLEBLOCK_CLI = HERE / "ra_core_singleblock_cli"  # this folder's own fresh build
BLOCK_WORDS = 1
BLOCK_BYTES = BLOCK_WORDS * 4

TIERS = {"smoke": 5_000, "full": 50_000}


def raw_collision_prob(n_items: int) -> float:
    """Birthday-bound estimate for raw 32-bit-word collision across
    n_items independent K=1 outputs: C(n_items, 2) / 2**32."""
    return comb(n_items, 2) / (2 ** 32)


def poisson_z(observed: int, expected: float) -> float:
    """Normal-approximation z-score for a Poisson(expected) count -- same
    spirit as scc_test.py's runs-test z, |z| < ~3 is unremarkable for a
    single-sample count this small."""
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
            [str(SINGLEBLOCK_CLI), "--stream", str(key), str(BLOCK_WORDS)],
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
        "mode": "k1_raw32", "m_keys": len(keys), "block_words": BLOCK_WORDS,
        "fingerprints": len(keys), "collisions_found": observed,
        "collisions": collisions[:50],  # cap stored examples, count is authoritative
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

    assert SINGLEBLOCK_CLI.exists(), f"{SINGLEBLOCK_CLI} not found -- run ./BUILD.sh first"
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
    print(f"=== collision_scan_singleblock_k1 {label} ({nworkers} workers) ===")
    result = scan_k1(keys, max_workers=nworkers)
    verdict = "PASS" if result["pass"] else "FAIL"
    print(f"  {result['fingerprints']} fingerprints, {result['collisions_found']} collisions "
          f"(expected~{result['expected_collisions_32bit_space']:.1f} in 2^32 space, "
          f"z={result['poisson_z']:.2f}) -- {verdict}")

    out = {"seed_mode": seed_mode, "tier": tier, "k1_raw32": result}
    out_path = HERE / f"collision_scan_singleblock_k1_results_{seed_mode}_{tier}.json"
    out_path.write_text(json.dumps(out, indent=2))
    print(f"\nWrote {out_path}")


if __name__ == "__main__":
    main()
