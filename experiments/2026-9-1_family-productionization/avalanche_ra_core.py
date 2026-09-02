"""Tahap 2 (family-productionization): avalanche gate for ra_core.c's two
production entry points, `ra_core_orbit` and `ra_core_singleblock`, against
the compiled `ra_core` binary's own `--stream` output (not the toy Python
model) -- this is the actual production binary, so we test it directly
rather than assuming inheritance from wired_prng.py's earlier survey.

Reuses experiments/2026-8-26_operation-pruning-research/quality_gate.py's
`avalanche_stats()` (generic: takes any `capture_fn(seed) -> list[int]`),
per HANDOVER.md's Tahap 2 spec ("reuse quality_gate.py's
avalanche_gate_min_bit(), band [0.3,0.7], floor per-bit >=0.2 -- NOT the old
average-only gate that missed operand-position-search's bit 5/6 defect").
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
    assert RA_CORE_BIN.exists(), f"{RA_CORE_BIN} not found -- compile ra_core.c first"
    for mode in ("orbit", "singleblock"):
        r = run_gate(mode)
        print(f"=== avalanche gate: {mode} ===")
        print(f"  overall_mean_hamming_fraction: {r['overall_mean_hamming_fraction']:.6f} "
              f"(band [{LOW},{HIGH}])")
        print(f"  min_bit_fraction: {r['min_bit_fraction']:.6f} (floor {MIN_BIT_FLOOR})")
        print(f"  PASSED: {r['passed']}")
        if not r["passed"]:
            worst = min(range(32), key=lambda b: r["per_bit_fractions"][b])
            print(f"  weakest bit: {worst} (fraction {r['per_bit_fractions'][worst]:.6f})")


if __name__ == "__main__":
    main()
