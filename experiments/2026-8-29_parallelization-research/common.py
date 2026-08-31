"""Shared helpers for the parallelization-research harness. Never modifies
`winner_wired_v2.c`/`ra_prng2.c` -- only subprocess-drives the existing
`--stream <seed> <n>` CLI (raw uint32 binary to stdout, log to stderr),
per HANDOVER.md's read-only constraint.
"""

from __future__ import annotations

import random
import subprocess
import numpy as np
from pathlib import Path

HERE = Path(__file__).parent
WINNER_BIN = HERE.parent / "2026-8-27_operand-position-search" / "winner_wired_v2"
PRACTRAND_BIN = Path.home() / "Documents/research/PractRand/RNG_test"

# Optional comparator (paper-exact original). Compiled into THIS folder if/when
# used -- src/ra_prng2/c/ra_prng2.c itself is only ever read, never edited.
RA_PRNG2_SRC = HERE.parent.parent / "src" / "ra_prng2" / "c" / "ra_prng2.c"
RA_PRNG2_BIN = HERE / "ra_prng2_cli"


def stream_values(seed: int, n: int, binary: Path = WINNER_BIN) -> np.ndarray:
    """Bounded capture: run `binary --stream seed n`, return n uint32 values.

    Suitable for n up to a few hundred million (holds full output in memory).
    For larger streams, use stream_popen() and read incrementally.
    """
    result = subprocess.run(
        [str(binary), "--stream", str(seed), str(n)],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
    )
    arr = np.frombuffer(result.stdout, dtype=np.uint32)
    assert len(arr) == n, f"expected {n} values, got {len(arr)} (binary exited early?)"
    return arr


def stream_popen(seed: int, n: int, binary: Path = WINNER_BIN) -> subprocess.Popen:
    """Streaming capture: caller reads/closes proc.stdout incrementally.
    Use for large n where materializing the full output isn't desired.
    """
    return subprocess.Popen(
        [str(binary), "--stream", str(seed), str(n)],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
    )


def random_seeds(m: int, rng_seed: int = 42) -> list[int]:
    """m distinct uint32 seeds sampled uniformly from the FULL 2**32 space
    (not sequential 0..m-1). Reproducible via rng_seed. Same convention as
    cross_correlation.py's "control" group -- random.sample on a range
    object doesn't materialize the population, so this is fine even for
    range(2**32).
    """
    return random.Random(rng_seed).sample(range(2**32), m)


def ensure_ra_prng2_cli() -> Path:
    """Compile the optional paper-exact comparator into this folder, if not
    already built. Reads src/ra_prng2/c/ra_prng2.c but never edits it, and
    writes the binary here (not into src/) -- consistent with the read-only
    constraint on src/ra_prng2/*.
    """
    if RA_PRNG2_BIN.exists():
        return RA_PRNG2_BIN
    subprocess.run(
        ["gcc", "-O3", "-march=native", "-std=gnu17", "-include", "stdalign.h",
         str(RA_PRNG2_SRC), "-o", str(RA_PRNG2_BIN)],
        check=True,
    )
    return RA_PRNG2_BIN


# --- Tier presets -----------------------------------------------------------
# Each question's script picks its own tier dict; kept here just so the
# smoke -> medium -> full progression is consistent and visible in one place.

TIERS_Q1A = {  # cross_correlation.py: (K per group, n per stream)
    "smoke": (8, 200_000),
    "full": (128, 1_000_000),
}

TIERS_Q1B = {  # interleave_practrand.py: (K streams, total interleaved bytes)
    "smoke": (4, 64 * 1024 * 1024),
    "medium": (8, 1024 * 1024 * 1024),
    "full": (8, 16 * 1024 * 1024 * 1024),
    "xlarge": (8, 128 * 1024 * 1024 * 1024),  # Phase 2 raised validation
    "1tb": (8, 1024 * 1024 * 1024 * 1024),  # Checkpoint follow-up (2026-08-31),
    # only run after xlarge (128GB) PASSED clean -- see RESULTS.md Method B.
}

TIERS_Q2 = {  # collision_scan.py --seeds sequential: (M seeds, V values-per-seed for blocksweep)
    "smoke": (100, 10_000),
    "full": (10_000, 250_000),
}

# Phase 2: --seeds random draws M seeds from the FULL 2**32 space (random_seeds()),
# not sequential 0..M-1. prefix and blocksweep get separate M since blocksweep is
# far more expensive per-seed (streams V words vs. prefix's fixed small n_words).
TIERS_Q2_RANDOM = {
    "smoke": {"prefix_m": 2_000, "blocksweep_m": 500, "v_words": 10_000},
    "full": {"prefix_m": 500_000, "blocksweep_m": 25_000, "v_words": 250_000},
}

TIERS_Q3 = {  # wall_time_scaling.py / perf_scaling.sh: N-process sweep
    "n_values": [1, 2, 3, 4, 8],
    "n_per_proc": 200_000_000,
}
