"""Tahap 2 (family-productionization) follow-up, Q2 re-pointed at
`ra_core_singleblock` instead of `winner_wired_v2`. Adapted from
collision_scan.py -- reuses `digest_collision_prob()` unmodified; `scan_*`
are re-implemented (not imported) only to swap the subprocess arg shape and
because blocksweep's "V words per seed, several blocks" doesn't apply here
(one key = at most one 255-word block, by design -- see
HANDOVER_TAHAP6.md). So this file only has ONE scan mode: one full block
(255 words, the max available) hashed per key, directly comparable to the
original's "prefix" mode but at the mode's natural full length instead of a
64-word prefix.

Purpose: does the "aggressive reinit, many independent keys" use case ever
produce the SAME 255-word block for two different keys? ra_core_orbit is
NOT re-tested here -- it already has its own dedicated collision-scan
validation at 500k-key production scale from the addressable-init-research
track (Tahap 3), fully inherited via Tahap 1's bit-identity proof.
"""

from __future__ import annotations

import hashlib
import json
import os
import subprocess
import sys
import threading
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

from common import HERE, random_seeds
from collision_scan import digest_collision_prob

FAMILY_DIR = HERE.parent / "2026-9-1_family-productionization"
SINGLEBLOCK_CLI = FAMILY_DIR / "ra_core_singleblock_cli"
BLOCK_WORDS = 255  # ra_core_singleblock's hard cap (rng<=255) -- the full block
BLOCK_BYTES = BLOCK_WORDS * 4
DIGEST_SIZE = 8

TIERS = {  # M keys
    "smoke": 5_000,
    "full": 50_000,
}


def scan_fullblock(keys: list[int], max_workers: int | None = None) -> dict:
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
        h = hashlib.blake2b(result.stdout, digest_size=DIGEST_SIZE).digest()
        with lock:
            if h in global_seen:
                if global_seen[h] != key:
                    collisions.append({"key_a": global_seen[h], "key_b": key, "hash": h.hex()})
            else:
                global_seen[h] = key

    with ThreadPoolExecutor(max_workers=max_workers) as ex:
        list(ex.map(worker, keys))

    prob = digest_collision_prob(len(keys))
    return {
        "mode": "fullblock", "m_keys": len(keys), "block_words": BLOCK_WORDS,
        "fingerprints": len(keys), "collisions_found": len(collisions),
        "collisions": collisions, "digest_collision_prob_estimate": prob,
    }


def main():
    args = sys.argv[1:]
    seed_mode = "sequential"
    if "--seeds" in args:
        i = args.index("--seeds")
        seed_mode = args[i + 1]
        del args[i:i + 2]
    tier = args[0] if args else "full"

    assert SINGLEBLOCK_CLI.exists(), f"{SINGLEBLOCK_CLI} not found -- build it first"
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
    print(f"=== collision_scan_ra_core_singleblock {label}: fullblock mode ({nworkers} workers) ===")
    result = scan_fullblock(keys, max_workers=nworkers)
    print(f"  {result['fingerprints']} fingerprints, {result['collisions_found']} collisions, "
          f"digest-collision-prob~{result['digest_collision_prob_estimate']:.2e}")

    out = {"seed_mode": seed_mode, "tier": tier, "fullblock": result}
    out_path = FAMILY_DIR / f"collision_scan_singleblock_results_{seed_mode}_{tier}.json"
    out_path.write_text(json.dumps(out, indent=2))
    print(f"\nWrote {out_path}")


if __name__ == "__main__":
    main()
