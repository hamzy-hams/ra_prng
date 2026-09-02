"""production-candidate-battery, Step 1: avalanche gate for `orbit` and
`singleblock` K=255 mode, against THIS folder's freshly rebuilt `ra_core`
binary (Step 0) -- NOT ../2026-9-1_family-productionization/ra_core.

Adapted from ../2026-9-1_family-productionization/avalanche_ra_core.py,
which hardcodes `RA_CORE_BIN = Path(__file__).parent / "ra_core"` --
running that script directly (even from this folder) would still resolve
to its OWN directory's binary, defeating this folder's rebuild-fresh
guarantee (Step 0). Only change from the original: RA_CORE_BIN points
here. Reuses quality_gate.py's avalanche_stats()/avalanche_gate_min_bit()
band [0.3,0.7], floor per-bit 0.2 -- identical gate logic, unmodified.
"""

from __future__ import annotations

import subprocess
import struct
import sys
from pathlib import Path

HERE = Path(__file__).parent
RA_CORE_BIN = HERE / "ra_core"

sys.path.insert(0, str(HERE.parent / "2026-8-26_operation-pruning-research"))
from quality_gate import avalanche_stats  # noqa: E402

LOW, HIGH, MIN_BIT_FLOOR = 0.3, 0.7, 0.2


def capture_cycle(mode: str, seed: int) -> list[int]:
    out = subprocess.run(
        [str(RA_CORE_BIN), "--stream", mode, str(seed & 0xFFFFFFFF), "255"],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
    ).stdout
    assert len(out) == 255 * 4, f"expected 1020 bytes, got {len(out)}"
    return list(struct.unpack("<255I", out))


def run_gate(mode: str) -> dict:
    stats = avalanche_stats(lambda seed: capture_cycle(mode, seed), base_seed=1)
    fraction = stats["overall_mean_hamming_fraction"]
    passed = (LOW <= fraction <= HIGH) and (stats["min_bit_fraction"] >= MIN_BIT_FLOOR)
    return {"mode": mode, "passed": passed, **stats}


def main():
    assert RA_CORE_BIN.exists(), f"{RA_CORE_BIN} not found -- run ./BUILD.sh first"
    for mode in ("orbit", "singleblock"):
        r = run_gate(mode)
        print(f"=== avalanche gate: {mode} (K=255, fresh build) ===")
        print(f"  overall_mean_hamming_fraction: {r['overall_mean_hamming_fraction']:.6f} "
              f"(band [{LOW},{HIGH}])")
        print(f"  min_bit_fraction: {r['min_bit_fraction']:.6f} (floor {MIN_BIT_FLOOR})")
        print(f"  PASSED: {r['passed']}")
        if not r["passed"]:
            worst = min(range(32), key=lambda b: r["per_bit_fractions"][b])
            print(f"  weakest bit: {worst} (fraction {r['per_bit_fractions'][worst]:.6f})")


if __name__ == "__main__":
    main()
