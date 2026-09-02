"""Tahap 2 (family-productionization) follow-up, Q1 Method A re-pointed at
`ra_core_singleblock` (experiments/2026-9-1_family-productionization/ra_core.c)
instead of `winner_wired_v2`. Adapted from cross_correlation.py, same
re-pointing pattern as cross_correlation_ra_prng2.py in this folder (binary=
override; stream_values()'s default binary is bound at common.py's import
time, so monkeypatching common.WINNER_BIN afterwards would not take effect).

Unlike the original (n up to 1,000,000 per stream), ra_core_singleblock
hard-caps at rng<=255 -- one key/address = at most 255 words, by design (see
HANDOVER_TAHAP6.md's dead-code-for-L proof). So n=255 (the maximum available
per key) is used here instead of a tier's usual n, and K (keys per group) is
raised to compensate some of the lost per-pair statistical power from the
much shorter n.

Purpose: does the "aggressive reinit, many independent keys" use case show
cross-key correlation that a single-key bit-identity check (already proven
in RESULTS.md) cannot catch? ra_core_orbit is NOT re-tested here -- it
already has its own dedicated cross-correlation validation at 500k-key
production scale from the addressable-init-research track (Tahap 3), fully
inherited via Tahap 1's bit-identity proof.
"""

from __future__ import annotations

import json
import random
import subprocess
import sys
from pathlib import Path

import numpy as np

from common import HERE
from cross_correlation import ALPHA, analyze_group  # noqa: F401  (reused unmodified)

FAMILY_DIR = HERE.parent / "2026-9-1_family-productionization"
SINGLEBLOCK_CLI = FAMILY_DIR / "ra_core_singleblock_cli"
N_WORDS = 255  # ra_core_singleblock's hard cap (rng<=255)

TIERS = {  # (K per group)
    "smoke": 64,
    "full": 512,
}


def stream_values_singleblock(key: int, n: int = N_WORDS) -> np.ndarray:
    result = subprocess.run(
        [str(SINGLEBLOCK_CLI), "--stream", str(key), str(n)],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
    )
    arr = np.frombuffer(result.stdout, dtype=np.uint32)
    assert len(arr) == n, f"expected {n} values, got {len(arr)}"
    return arr


def build_group(keys: list[int]) -> np.ndarray:
    return np.stack([stream_values_singleblock(k).astype(np.float64) for k in keys])


def run(tier: str) -> dict:
    assert SINGLEBLOCK_CLI.exists(), f"{SINGLEBLOCK_CLI} not found -- build it first"
    k = TIERS[tier]
    print(f"=== cross_correlation_ra_core_singleblock tier={tier}: K={k} n={N_WORDS} ===")

    adjacent_keys = list(range(k))
    control_keys = random.Random(42).sample(range(0, 2**32), k)

    adjacent_data = build_group(adjacent_keys)
    control_data = build_group(control_keys)

    return {
        "tier": tier,
        "binary": "ra_core_singleblock",
        "n_words": N_WORDS,
        "adjacent": analyze_group(adjacent_data, "adjacent"),
        "control": analyze_group(control_data, "control"),
    }


def main():
    tier = sys.argv[1] if len(sys.argv) > 1 else "full"
    result = run(tier)
    out_path = FAMILY_DIR / f"cross_correlation_singleblock_results_{tier}.json"
    out_path.write_text(json.dumps(result, indent=2))
    print(f"\nWrote {out_path}")

    adj_rate = result["adjacent"]["flagged_count"] / result["adjacent"]["m_pairs"]
    ctl_rate = result["control"]["flagged_count"] / result["control"]["m_pairs"]
    print(f"\nadjacent flagged rate={adj_rate:.4f} vs control flagged rate={ctl_rate:.4f} "
          f"(expected under H0: ~{ALPHA:.4f} family-wise, so per-test rate near 0)")


if __name__ == "__main__":
    main()
