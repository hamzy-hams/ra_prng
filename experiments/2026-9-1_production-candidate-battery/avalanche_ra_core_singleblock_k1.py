"""production-candidate-battery, Step 1: avalanche gate for `singleblock`
K=1 mode (one word per reinit -- the most aggressive reinit pattern, never
avalanche-tested before this folder). Uses this folder's freshly rebuilt
`ra_core_singleblock_cli` (Step 0) with `--stream <seed> 1`.

Same gate logic as avalanche_orbit_singleblock_k255.py (quality_gate.py's
avalanche_stats()/band [0.3,0.7]/floor 0.2) -- only capture_fn differs
(1-word capture instead of 255).
"""

from __future__ import annotations

import subprocess
import struct
import sys
from pathlib import Path

HERE = Path(__file__).parent
CLI_BIN = HERE / "ra_core_singleblock_cli"

sys.path.insert(0, str(HERE.parent / "2026-8-26_operation-pruning-research"))
from quality_gate import avalanche_stats  # noqa: E402

LOW, HIGH, MIN_BIT_FLOOR = 0.3, 0.7, 0.2


def capture_one(seed: int) -> list[int]:
    out = subprocess.run(
        [str(CLI_BIN), "--stream", str(seed & 0xFFFFFFFF), "1"],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
    ).stdout
    assert len(out) == 4, f"expected 4 bytes, got {len(out)}"
    return list(struct.unpack("<1I", out))


def main():
    assert CLI_BIN.exists(), f"{CLI_BIN} not found -- run ./BUILD.sh first"
    stats = avalanche_stats(capture_one, base_seed=1)
    fraction = stats["overall_mean_hamming_fraction"]
    passed = (LOW <= fraction <= HIGH) and (stats["min_bit_fraction"] >= MIN_BIT_FLOOR)

    print("=== avalanche gate: singleblock K=1 (fresh build) ===")
    print(f"  overall_mean_hamming_fraction: {fraction:.6f} (band [{LOW},{HIGH}])")
    print(f"  min_bit_fraction: {stats['min_bit_fraction']:.6f} (floor {MIN_BIT_FLOOR})")
    print(f"  PASSED: {passed}")
    if not passed:
        worst = min(range(32), key=lambda b: stats["per_bit_fractions"][b])
        print(f"  weakest bit: {worst} (fraction {stats['per_bit_fractions'][worst]:.6f})")
    sys.exit(0 if passed else 1)


if __name__ == "__main__":
    main()
