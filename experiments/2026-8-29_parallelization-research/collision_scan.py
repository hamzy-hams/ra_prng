"""Q2: probability of cross-stream collision/overlap between different
seeds' `--stream <seed> <n>` output, per HANDOVER.md question 2. Adapted
from full_scale_sanity_check.py::check_no_repeat(), but with ONE global
dict across ALL seeds (not a per-seed set) -- collisions are only reported
when the hash's first owner is a DIFFERENT seed (within-stream repeats are
a separate concern, covered by the periodicity experiments already).

Two modes:
  - prefix:     hash each seed's first N words as one fingerprint. A repeat
                means two different seeds produced byte-identical prefixes.
  - blocksweep: scan V values per seed in 255-word blocks (one full outer
                permutation cycle -- same granularity ra_core's reseed
                operates on), hash every block into the global dict. A repeat
                means two different seeds produced an identical block at
                ANY offset (not necessarily the same position) -- true
                cross-stream overlap, not just matching prefixes.

Both modes rely on the state space (256x32-bit) being astronomically larger
than what's tested here, so the *hash digest*'s own collision probability
(blake2b, 64-bit digest) must stay negligible relative to the number of
(seed,block) pairs compared -- see the birthday estimate printed by main().
If it's negligible, any hit found is a real generator-level signal, not
digest noise.

Two seed sources (Phase 2, --seeds flag):
  - sequential (default): seeds 0..M-1, matches Phase 1's original scale --
    kept as the "adjacent seeds" baseline, not replaced.
  - random: M seeds sampled from the FULL 2**32 space via common.random_seeds(),
    at a raised scale (TIERS_Q2_RANDOM) -- closer to a real production seed
    distribution than sequential 0..M-1.

Both modes are parallelized via `ThreadPoolExecutor` driving concurrent
`subprocess.Popen` calls against a SHARED dict guarded by a `threading.Lock`
-- the same concurrent-subprocess-from-one-process primitive already proven
to work cleanly in Q3's `wall_time_scaling.py`. `subprocess.Popen`/`.read()`
release the GIL while blocked on I/O, and `hashlib.blake2b`'s C update()
releases it too, so this gets real parallelism without ever pickling data
between processes.

NOTE (lesson from this session): an earlier version of this file used
`ProcessPoolExecutor` with a map-reduce merge (each worker building its own
local dict, returned to the main process for merging). It worked at modest
scale (M up to ~20,000) but **deadlocked** at the raised validation scale
(prefix M=500,000 / blocksweep M=25,000, V=250,000) -- workers finished
their chunks (near-zero further CPU ticks) but never returned, most likely
because pickling a several-hundred-MB-to-GB dict back through the pool's
IPC pipe per worker either exceeded practical pipe/queue throughput or hit
a fork-safety edge case from calling `subprocess.Popen` inside pool workers
under sustained load. Switching to threads (shared memory, no IPC) sidesteps
the failure mode entirely rather than working around it.
"""

from __future__ import annotations

import hashlib
import json
import os
import subprocess
import sys
import threading
from concurrent.futures import ThreadPoolExecutor
from math import comb

from common import HERE, TIERS_Q2, TIERS_Q2_RANDOM, WINNER_BIN, random_seeds

BLOCK_WORDS = 255
BLOCK_BYTES = BLOCK_WORDS * 4
DIGEST_SIZE = 8  # bytes -> 64-bit digest


def digest_collision_prob(n_items: int) -> float:
    """Birthday-bound estimate for pure hash-digest collision probability
    across n_items 64-bit digests: C(n_items, 2) / 2^64."""
    return comb(n_items, 2) / (2 ** 64)


# --- prefix mode -------------------------------------------------------------

def scan_prefix(seeds: list[int], n_words: int = 64, max_workers: int | None = None) -> dict:
    max_workers = max_workers or os.cpu_count() or 1
    global_seen: dict[bytes, int] = {}
    collisions: list[dict] = []
    lock = threading.Lock()

    def worker(seed: int) -> None:
        result = subprocess.run(
            [str(WINNER_BIN), "--stream", str(seed), str(n_words)],
            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
        )
        h = hashlib.blake2b(result.stdout, digest_size=DIGEST_SIZE).digest()
        with lock:
            if h in global_seen:
                if global_seen[h] != seed:
                    collisions.append({"seed_a": global_seen[h], "seed_b": seed, "hash": h.hex()})
            else:
                global_seen[h] = seed

    with ThreadPoolExecutor(max_workers=max_workers) as ex:
        list(ex.map(worker, seeds))  # drive to completion; exceptions propagate here

    prob = digest_collision_prob(len(seeds))
    return {
        "mode": "prefix", "m_seeds": len(seeds), "n_words": n_words,
        "fingerprints": len(seeds), "collisions_found": len(collisions),
        "collisions": collisions, "digest_collision_prob_estimate": prob,
    }


# --- blocksweep mode ----------------------------------------------------------

def scan_blocksweep(seeds: list[int], v_words: int, max_workers: int | None = None) -> dict:
    max_workers = max_workers or os.cpu_count() or 1
    global_seen: dict[bytes, tuple[int, int]] = {}
    collisions: list[dict] = []
    lock = threading.Lock()

    def worker(seed: int) -> int:
        proc = subprocess.Popen(
            [str(WINNER_BIN), "--stream", str(seed), str(v_words)],
            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
        )
        buf = b""
        pos = 0
        assert proc.stdout is not None
        while True:
            chunk = proc.stdout.read(1024 * 1024)
            if not chunk:
                break
            buf += chunk
            while len(buf) >= BLOCK_BYTES:
                block = buf[:BLOCK_BYTES]
                buf = buf[BLOCK_BYTES:]
                h = hashlib.blake2b(block, digest_size=DIGEST_SIZE).digest()
                with lock:
                    if h in global_seen:
                        owner_seed, owner_pos = global_seen[h]
                        if owner_seed != seed:
                            collisions.append({
                                "seed_a": owner_seed, "pos_a": owner_pos,
                                "seed_b": seed, "pos_b": pos, "hash": h.hex(),
                            })
                    else:
                        global_seen[h] = (seed, pos)
                pos += 1
        proc.stdout.close()
        proc.wait()
        return pos  # blocks processed for this seed

    with ThreadPoolExecutor(max_workers=max_workers) as ex:
        counts = list(ex.map(worker, seeds))
    total_blocks = sum(counts)

    prob = digest_collision_prob(total_blocks)
    return {
        "mode": "blocksweep", "m_seeds": len(seeds), "v_words": v_words,
        "total_blocks": total_blocks, "collisions_found": len(collisions),
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

    if seed_mode == "sequential":
        m_seeds, v_words = TIERS_Q2[tier]
        prefix_seeds = list(range(m_seeds))
        blocksweep_seeds = list(range(m_seeds))
        label = f"sequential tier={tier} M={m_seeds}"
    elif seed_mode == "random":
        spec = TIERS_Q2_RANDOM[tier]
        v_words = spec["v_words"]
        prefix_seeds = random_seeds(spec["prefix_m"], rng_seed=42)
        blocksweep_seeds = random_seeds(spec["blocksweep_m"], rng_seed=43)
        label = f"random tier={tier} prefix_M={spec['prefix_m']} blocksweep_M={spec['blocksweep_m']}"
    else:
        raise SystemExit(f"unknown --seeds mode {seed_mode!r} (expected sequential|random)")

    nworkers = os.cpu_count() or 1
    print(f"=== collision_scan {label}: prefix mode ({nworkers} workers) ===")
    prefix_result = scan_prefix(prefix_seeds, n_words=64, max_workers=nworkers)
    print(f"  {prefix_result['fingerprints']} fingerprints, "
          f"{prefix_result['collisions_found']} collisions, "
          f"digest-collision-prob~{prefix_result['digest_collision_prob_estimate']:.2e}")

    print(f"=== collision_scan {label}: blocksweep mode (V={v_words:,}, {nworkers} workers) ===")
    blocksweep_result = scan_blocksweep(blocksweep_seeds, v_words, max_workers=nworkers)
    print(f"  {blocksweep_result['total_blocks']:,} blocks, "
          f"{blocksweep_result['collisions_found']} collisions, "
          f"digest-collision-prob~{blocksweep_result['digest_collision_prob_estimate']:.2e}")

    out = {"seed_mode": seed_mode, "tier": tier, "prefix": prefix_result, "blocksweep": blocksweep_result}
    out_path = HERE / f"collision_scan_results_{seed_mode}_{tier}.json"
    out_path.write_text(json.dumps(out, indent=2))
    print(f"\nWrote {out_path}")


if __name__ == "__main__":
    main()
