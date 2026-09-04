"""experiments/2026-9-4_orbit-kmin-battery -- avalanche gate for
`ra_core_orbit` at K=1 and K=255, against the FIXED w8_f10_i0 + rolling-o
formula built here (Step 0, ./BUILD.sh) from
../2026-9-4_orbit-fix-and-wideo-rolling-optimization/ra_core_v2.c.

Adapted from
../2026-9-1_production-candidate-battery/avalanche_ra_core_singleblock_k1.py
and avalanche_orbit_singleblock_k255.py: same quality_gate.py
avalanche_stats() gate logic (band [0.3,0.7], per-bit floor 0.2),
unmodified. Only change: points at this folder's `ra_core_v2` binary with
`--stream orbit <seed> <n>` instead of the canonical pre-fix `ra_core`
binary, and covers only `orbit` (singleblock's avalanche property is
already fully validated and unaffected by this session's changes).
"""

from __future__ import annotations

import subprocess
import struct
import sys
from pathlib import Path

HERE = Path(__file__).parent
RA_CORE_V2_BIN = HERE / "ra_core_v2"

sys.path.insert(0, str(HERE.parent / "2026-8-26_operation-pruning-research"))
from quality_gate import avalanche_stats  # noqa: E402

LOW, HIGH, MIN_BIT_FLOOR = 0.3, 0.7, 0.2


def capture(n_words: int, seed: int) -> list[int]:
    out = subprocess.run(
        [str(RA_CORE_V2_BIN), "--stream", "orbit", str(seed & 0xFFFFFFFF), str(n_words)],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
    ).stdout
    assert len(out) == n_words * 4, f"expected {n_words * 4} bytes, got {len(out)}"
    return list(struct.unpack(f"<{n_words}I", out))


def run_gate(n_words: int) -> dict:
    stats = avalanche_stats(lambda seed: capture(n_words, seed), base_seed=1)
    fraction = stats["overall_mean_hamming_fraction"]
    passed = (LOW <= fraction <= HIGH) and (stats["min_bit_fraction"] >= MIN_BIT_FLOOR)
    return {"n_words": n_words, "passed": passed, **stats}


def main():
    assert RA_CORE_V2_BIN.exists(), f"{RA_CORE_V2_BIN} not found -- run ./BUILD.sh first"
    overall_pass = True
    for n_words in (1, 255):
        r = run_gate(n_words)
        overall_pass &= r["passed"]
        print(f"=== avalanche gate: orbit K={n_words} (ra_core_v2, fixed formula) ===")
        print(f"  overall_mean_hamming_fraction: {r['overall_mean_hamming_fraction']:.6f} "
              f"(band [{LOW},{HIGH}])")
        print(f"  min_bit_fraction: {r['min_bit_fraction']:.6f} (floor {MIN_BIT_FLOOR})")
        print(f"  PASSED: {r['passed']}")
        if not r["passed"]:
            worst = min(range(32), key=lambda b: r["per_bit_fractions"][b])
            print(f"  weakest bit: {worst} (fraction {r['per_bit_fractions'][worst]:.6f})")
    sys.exit(0 if overall_pass else 1)


if __name__ == "__main__":
    main()
