"""Avalanche + PractRand measurement for the phase-1 (L-only)
state-update-mechanism spectrum, per HANDOVER.md's methodology section.

Reuses experiments/2026-8-26_operation-pruning-research/quality_gate.py's
avalanche_stats()/avalanche_gate_min_bit() unchanged (they're already
generic over any `capture_fn(seed) -> list[int]`). The only new code here
is a `capture_fn` that subprocess's one of THIS folder's `--stream`
binaries (ra_prng2/winner_wired_v2-style CLI: `--stream <seed> <n>`)
instead of pruned_prng's flag-based CLI, and a small practrand_prefix_gate
adapter with the same different argv convention.

Uses avalanche_gate_min_bit (not the weaker avalanche_gate/quality_gate)
per the explicit lesson in this repo: the scalar-average gate let a
dead-bit defect through undetected (2026-8-27's RESULTS.md).
"""

from __future__ import annotations

import json
import struct
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).parent
sys.path.insert(0, str(HERE.parent / "2026-8-26_operation-pruning-research"))
from quality_gate import avalanche_stats  # noqa: E402

PRACTRAND_BIN = Path.home() / "Documents/research/PractRand/RNG_test"

VARIANTS = [f"{t}_{m}" for t in ("original", "winner") for m in ("permute", "inject", "overwrite")]

# IMPORTANT, found by running this script, not by inspection: `c` (the
# emitted stream value) is computed BEFORE L's per-step update each step,
# and L only reaches the output at all via the once-per-255-steps
# `M[i] ^= L[i]` reseed fold (HANDOVER.md's own observation). That means a
# single-cycle capture (quality_gate.py's `_capture_first_cycle`
# convention, 255 values, no reseed) is *mechanism-invariant by
# construction* -- confirmed empirically: original_permute's and
# original_inject's first 255 values are byte-identical. Measuring
# avalanche on one cycle here would silently produce identical,
# uninformative numbers across all 3 L mechanisms. Capturing 2 cycles
# (510 values) is the minimum needed to cross one reseed boundary and
# actually exercise the mechanism under test.
CYCLES = 2
CAPTURE_LEN = 255 * CYCLES


def capture_first_cycle(binary: Path, seed: int) -> list[int]:
    """`CYCLES` outer cycles' worth of `c` values -- see the CAPTURE_LEN
    comment above for why 1 cycle (quality_gate.py's convention) cannot
    distinguish L mechanisms at all."""
    result = subprocess.run(
        [str(binary), "--stream", str(seed), str(CAPTURE_LEN)],
        capture_output=True, check=True,
    )
    raw = result.stdout
    n = len(raw) // 4
    return list(struct.unpack(f"<{n}I", raw[: n * 4]))


def avalanche_gate_min_bit_binary(binary: Path, base_seed: int = 1,
                                   low: float = 0.3, high: float = 0.7,
                                   min_bit_floor: float = 0.2) -> dict:
    stats = avalanche_stats(lambda seed: capture_first_cycle(binary, seed), base_seed)
    fraction = stats["overall_mean_hamming_fraction"]
    passed = (low <= fraction <= high) and (stats["min_bit_fraction"] >= min_bit_floor)
    return {"passed": passed, **stats}


def practrand_prefix_gate_binary(binary: Path, seed: int = 1, n_bytes: int = 16 * 1024 * 1024 * 1024) -> dict:
    if not PRACTRAND_BIN.exists():
        raise FileNotFoundError(f"PractRand RNG_test not found at {PRACTRAND_BIN}")
    assert n_bytes % (1024 * 1024) == 0, "n_bytes must be a whole number of MB"
    length_arg = f"{n_bytes // (1024 * 1024)}MB"
    n_values = n_bytes // 4
    # NOTE: unlike pruned_prng.c's CLI (quality_gate.py's convention, where
    # the corresponding positional arg is an *outer-iteration* count), this
    # folder's binaries use ra_prng2.c's `--stream <seed> <n>` convention
    # where `n` IS the exact number of raw uint32 values to emit. Pass a
    # small surplus over n_values (one full 255-step cycle) so PractRand's
    # own -tlmax is the actual cutoff, not a short read.
    stream_count = n_values + 255

    gen = subprocess.Popen(
        [str(binary), "--stream", str(seed), str(stream_count)],
        stdout=subprocess.PIPE,
    )
    test = subprocess.run(
        [str(PRACTRAND_BIN), "stdin32", "-tlmin", length_arg, "-tlmax", length_arg],
        stdin=gen.stdout,
        capture_output=True,
    )
    gen.stdout.close()
    gen.wait()

    stdout = test.stdout.decode(errors="replace")
    anomaly = any(kw in stdout for kw in ("FAIL", "SUSPICIOUS"))
    return {"passed": not anomaly, "n_bytes": n_bytes, "stdout": stdout,
            "stderr": test.stderr.decode(errors="replace")}


def main():
    results = {}
    for name in VARIANTS:
        binary = HERE / name
        if not binary.exists():
            print(f"SKIP {name}: binary not found (run gen_variants.py first)")
            continue
        print(f"=== {name} ===")
        ava = avalanche_gate_min_bit_binary(binary)
        print(f"  avalanche: overall={ava['overall_mean_hamming_fraction']:.4f} "
              f"min_bit={ava['min_bit_fraction']:.4f} passed={ava['passed']}")
        results[name] = {"avalanche": ava}

    out_path = HERE / "avalanche_results.json"
    out_path.write_text(json.dumps(results, indent=2))
    print(f"\nWrote {out_path}")


if __name__ == "__main__":
    main()
