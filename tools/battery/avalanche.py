#!/usr/bin/env python3
"""tools/battery/avalanche.py -- generic Tier 0 avalanche gate.

Generalized port of the `hamming` / `avalanche_stats` / `avalanche_gate_min_bit`
logic in
../../experiments/2026-8-26_operation-pruning-research/quality_gate.py:61-133
(same thresholds, same full-cycle-not-just-first-word measurement -- see that
file's docstring for why per-bit *minimum*, not just the mean, matters: a
candidate there passed the mean-only gate at 0.42 while two seed bits were
almost completely avalanche-dead, 253/255 positions unaffected).

Unlike every prior tier0_avalanche_*.py in experiments/2026-9-*/, this does
NOT require hand-porting the candidate core to Python. It drives the
compiled candidate binary directly via `--single <seed> <n>` (see
driver_main_template.c) and reads raw little-endian uint32 words back from
stdout. This removes the "verify the Python oracle matches the C bit-for-bit"
step that every prior experiment had to do by hand before trusting any
statistic (feedback_verify_external_baselines) -- at the cost of one
subprocess spawn per seed-bit flip (33 total per candidate), which is
sub-second in practice for a 255-word cycle.

Usage:
    python tools/battery/avalanche.py --binary ./candidate
    python tools/battery/avalanche.py --binary ./candidate --cycle-words 255 \\
        --base-seed 1 --low 0.3 --high 0.7 --floor 0.2 --json out.json

Exit code 0 = PASS, 1 = REJECT, 2 = usage/runtime error.
"""

from __future__ import annotations

import argparse
import json
import struct
import subprocess
import sys
from pathlib import Path

DEFAULT_LOW, DEFAULT_HIGH, DEFAULT_FLOOR = 0.3, 0.7, 0.2
DEFAULT_CYCLE_WORDS = 255  # one full outer cycle for K<=255, matches
                            # quality_gate.py's _capture_first_cycle precedent
DEFAULT_BASE_SEED = 1
DEFAULT_ADDR_HI = 0  # "single global address" baseline, matches ra_core.c's
                      # checksum_key_core / ra_core_param.c's KAT convention


def hamming(x: int, y: int) -> int:
    return bin(x ^ y).count("1")


def capture_via_binary(binary: Path, seed: int, cycle_words: int,
                        core: str | None = None, addr_hi: int = DEFAULT_ADDR_HI) -> list[int]:
    """Run `<binary> [<core>] --single <seed> <addr_hi> <cycle_words>` and
    parse raw uint32 LE words from stdout. This is the default, robust
    capture path -- see module docstring for why it replaces a hand-ported
    Python oracle.

    `core` selects the dual-core CLI convention (driver_main_template.c's
    CORE_INIT/CORE_TRANSFORM mode, `<binary> orbit|singleblock --single ...`)
    -- leave it None for the single-core CORE_FN convention (`<binary>
    --single ...`).

    `addr_hi` (2026-09-11, HANDOVER.md state-size-pareto-frontier Tahap D
    fix): driver_main_template.c's --single/--stream/--stream-hashed gained
    a mandatory addr_hi argument on 2026-09-06 (Kandidat D 64-bit address
    widening) -- this function predates that and was calling the binary with
    a now-missing argument, which the driver rejects as a usage error.
    Fixed by threading addr_hi through, default DEFAULT_ADDR_HI=0 so seed-bit
    avalanche (which only flips `seed`/key bits) keeps its existing baseline
    unless a caller explicitly wants to test at a non-zero address."""
    # Resolve to an absolute path before exec: str(Path("./binary")) strips
    # the "./" (becomes "binary"), and subprocess with no "/" in the name
    # searches $PATH instead of cwd -- a bare relative binary would then
    # FileNotFoundError even though it exists right there (#seen with
    # --binary ./baseline).
    prefix = [core] if core else []
    proc = subprocess.run(
        [str(Path(binary).resolve()), *prefix, "--single", str(seed), str(addr_hi), str(cycle_words)],
        stdout=subprocess.PIPE, check=True,
    )
    out = proc.stdout
    expected_bytes = cycle_words * 4
    if len(out) < expected_bytes:
        raise RuntimeError(
            f"binary produced {len(out)} bytes, expected {expected_bytes} "
            f"for {'--core ' + core + ' ' if core else ''}--single {seed} {addr_hi} {cycle_words} "
            f"(check CORE_FN/CORE_TRANSFORM or rng cap)"
        )
    return list(struct.unpack(f"<{cycle_words}I", out[:expected_bytes]))


def avalanche_stats(capture_fn, base_seed: int = DEFAULT_BASE_SEED) -> dict:
    """Flip each of the 32 seed bits, capture one cycle via
    `capture_fn(seed) -> list[int]`, measure per-position Hamming distance
    against the baseline capture. Same shape as quality_gate.py's
    avalanche_stats(), generalized over capture_fn."""
    baseline = capture_fn(base_seed)
    per_bit_means = []
    for bit in range(32):
        flipped_seed = (base_seed ^ (1 << bit)) & 0xFFFFFFFF
        mutated = capture_fn(flipped_seed)
        dists = [hamming(a, b) for a, b in zip(baseline, mutated)]
        per_bit_means.append(sum(dists) / len(dists) if dists else 0.0)

    per_bit_fractions = [m / 32.0 for m in per_bit_means]
    overall_bits = sum(per_bit_means) / len(per_bit_means) if per_bit_means else 0.0
    fraction = overall_bits / 32.0
    return {
        "overall_mean_hamming_bits": overall_bits,
        "overall_mean_hamming_fraction": fraction,
        "per_bit_fractions": per_bit_fractions,
        "min_bit_fraction": min(per_bit_fractions) if per_bit_fractions else 0.0,
    }


def avalanche_gate_min_bit(capture_fn, base_seed: int = DEFAULT_BASE_SEED,
                            low: float = DEFAULT_LOW, high: float = DEFAULT_HIGH,
                            min_bit_floor: float = DEFAULT_FLOOR) -> dict:
    stats = avalanche_stats(capture_fn, base_seed)
    fraction = stats["overall_mean_hamming_fraction"]
    passed = (low <= fraction <= high) and (stats["min_bit_fraction"] >= min_bit_floor)
    return {"passed": passed, **stats}


def run(binary: Path, cycle_words: int = DEFAULT_CYCLE_WORDS,
        base_seed: int = DEFAULT_BASE_SEED, low: float = DEFAULT_LOW,
        high: float = DEFAULT_HIGH, floor: float = DEFAULT_FLOOR,
        core: str | None = None, addr_hi: int = DEFAULT_ADDR_HI) -> dict:
    def capture_fn(seed: int) -> list[int]:
        return capture_via_binary(binary, seed, cycle_words, core, addr_hi)

    result = avalanche_gate_min_bit(capture_fn, base_seed, low, high, floor)
    result["binary"] = str(binary)
    result["cycle_words"] = cycle_words
    result["base_seed"] = base_seed
    result["core"] = core
    result["thresholds"] = {"low": low, "high": high, "min_bit_floor": floor}
    return result


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0],
                                  formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--binary", required=True, type=Path,
                     help="compiled candidate binary (must implement --single, "
                          "see driver_main_template.c)")
    ap.add_argument("--cycle-words", type=int, default=DEFAULT_CYCLE_WORDS)
    ap.add_argument("--base-seed", type=lambda s: int(s, 0), default=DEFAULT_BASE_SEED)
    ap.add_argument("--addr-hi", type=lambda s: int(s, 0), default=DEFAULT_ADDR_HI)
    ap.add_argument("--core", choices=["orbit", "singleblock"], default=None,
                     help="dual-core convention (CORE_INIT/CORE_TRANSFORM): prefix "
                          "the binary's argv with this core name. Omit for the "
                          "single-core CORE_FN convention.")
    ap.add_argument("--low", type=float, default=DEFAULT_LOW)
    ap.add_argument("--high", type=float, default=DEFAULT_HIGH)
    ap.add_argument("--floor", type=float, default=DEFAULT_FLOOR)
    ap.add_argument("--json", type=Path, default=None,
                     help="also write the full result (incl. per-bit fractions) here")
    args = ap.parse_args(argv)

    if not args.binary.exists():
        print(f"error: binary not found: {args.binary}", file=sys.stderr)
        return 2

    try:
        result = run(args.binary, args.cycle_words, args.base_seed,
                     args.low, args.high, args.floor, args.core, args.addr_hi)
    except (subprocess.CalledProcessError, RuntimeError) as e:
        print(f"error running binary: {e}", file=sys.stderr)
        return 2

    if args.json:
        args.json.write_text(json.dumps(result, indent=2))

    status = "PASS" if result["passed"] else "REJECT"
    print(f"Tier 0 avalanche: {status}")
    print(f"  binary={result['binary']} cycle_words={result['cycle_words']} "
          f"base_seed={result['base_seed']}")
    print(f"  mean_fraction={result['overall_mean_hamming_fraction']:.4f} "
          f"min_bit_fraction={result['min_bit_fraction']:.4f} "
          f"(band=[{args.low},{args.high}], floor={args.floor})")
    return 0 if result["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
