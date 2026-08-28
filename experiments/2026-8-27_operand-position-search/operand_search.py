"""Driver for the operand-position rewiring search over wiring.py's 108
candidate wirings, all under the fixed op set {TAP6,TAP7,ROT_C,SHR13}
(hash_access=sequential) -- see HANDOVER.md. Op count/set never changes
here; only which variable occupies which slot in the a/b/c update chain
does (ablation_search.py's greedy walk from the sibling experiment doesn't
apply -- it walks on/off flags, not operand assignments, per HANDOVER.md's
"Reuse vs rebuild search infrastructure" open question).

Two tiers, mirroring quality_gate.py's shape:

  Tier 0 (avalanche, min-bit floor): pure Python, cheap (108 wirings x 33
  captures x 255 steps). Uses quality_gate.avalanche_stats() (generic,
  capture_fn-based) plus a min-bit floor check -- this is the exact check
  that would have caught DEFAULT_WIRING's (baseline.c's) bit 5/6 dead zone
  (min_bit_fraction ~0.000245, see quality_gate.py's avalanche_gate_min_bit
  docstring), which the sibling experiment's scalar-average gate missed.

  Tier 1 (PractRand prefix, SMALL): only for candidates that already passed
  Tier 0. No C harness exists for wiring (only baseline.c's fixed wiring
  has one) -- so this streams wired_prng.py's raw uint32 `c` bytes
  (Python-generated) directly into RNG_test via a pipe, same idea as
  quality_gate.practrand_prefix_gate() but without the C binary. Kept
  SMALL (a few MB, quality_gate.py's own DEFAULT_PRACTRAND_BYTES comment
  notes 2MB is the smallest prefix that caught its first documented trap)
  -- this is a search-loop filter only, per HANDOVER.md's "don't run
  full-scale PractRand/dieharder inside a search loop". The wiring that
  survives both tiers still needs full-scale validation via a hardcoded
  winner_wired.c (separate, later step -- see RESULTS.md).

Every trial is appended to operand_search_log.jsonl (one JSON object per
line), mirroring ablation_log.jsonl's format.
"""

from __future__ import annotations

import json
import struct
import subprocess
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent /
                       "2026-8-26_operation-pruning-research"))
from quality_gate import avalanche_stats, PRACTRAND_BIN  # noqa: E402

from wiring import Wiring, DEFAULT_WIRING, all_wirings, describe  # noqa: E402
from wired_prng import stream  # noqa: E402

LOG_PATH = Path(__file__).parent / "operand_search_log.jsonl"

MIN_BIT_FLOOR = 0.2
LOW, HIGH = 0.3, 0.7
TIER1_BYTES = 2 * 1024 * 1024  # search-loop filter only, see module docstring


def _log(event: dict):
    event["ts"] = time.time()
    with LOG_PATH.open("a") as f:
        f.write(json.dumps(event) + "\n")
    print(json.dumps(event))


def _capture_first_cycle(seed: int, w: Wiring) -> list[int]:
    out = []
    for c in stream(seed, 1, w):
        out.append(c)
    return out


def tier0_avalanche(w: Wiring, base_seed: int = 1) -> dict:
    stats = avalanche_stats(lambda seed: _capture_first_cycle(seed, w), base_seed)
    fraction = stats["overall_mean_hamming_fraction"]
    passed = (LOW <= fraction <= HIGH) and (stats["min_bit_fraction"] >= MIN_BIT_FLOOR)
    return {"passed": passed, **stats}


def tier1_practrand_small(w: Wiring, seed: int = 1, n_bytes: int = TIER1_BYTES) -> dict:
    if not PRACTRAND_BIN.exists():
        raise FileNotFoundError(f"PractRand RNG_test not found at {PRACTRAND_BIN}")

    n_values = n_bytes // 4
    iterations = n_values // 255 + 2

    buf = bytearray()
    for c in stream(seed, iterations, w):
        buf += struct.pack("<I", c)
        if len(buf) >= n_bytes:
            break
    buf = bytes(buf[:n_bytes])

    length_arg = f"{n_bytes // (1024 * 1024)}MB"
    test = subprocess.run(
        [str(PRACTRAND_BIN), "stdin32", "-tlmin", length_arg, "-tlmax", length_arg],
        input=buf, capture_output=True,
    )
    stdout = test.stdout.decode(errors="replace")
    anomaly = any(kw in stdout for kw in ("FAIL", "SUSPICIOUS"))
    return {"passed": not anomaly, "n_bytes": n_bytes, "stdout": stdout}


def main():
    _log({"event": "start", "search_space": sum(1 for _ in all_wirings()),
          "min_bit_floor": MIN_BIT_FLOOR, "low": LOW, "high": HIGH,
          "tier1_bytes": TIER1_BYTES})

    winners = []
    for w in all_wirings():
        is_default = (w == DEFAULT_WIRING)
        t0 = time.time()
        t0_result = tier0_avalanche(w)
        elapsed0 = time.time() - t0
        _log({
            "event": "tier0", "wiring": describe(w), "is_default": is_default,
            "passed": t0_result["passed"],
            "overall_mean_hamming_fraction": t0_result["overall_mean_hamming_fraction"],
            "min_bit_fraction": t0_result["min_bit_fraction"],
            "elapsed_s": elapsed0,
        })
        if not t0_result["passed"]:
            continue

        t1 = time.time()
        t1_result = tier1_practrand_small(w)
        elapsed1 = time.time() - t1
        _log({
            "event": "tier1", "wiring": describe(w), "passed": t1_result["passed"],
            "n_bytes": t1_result["n_bytes"], "elapsed_s": elapsed1,
        })
        if t1_result["passed"]:
            winners.append((w, t0_result))

    _log({"event": "done", "n_winners": len(winners),
          "winners": [describe(w) for w, _ in winners]})

    print("\n=== WINNERS (passed Tier 0 + Tier 1) ===")
    for w, stats in winners:
        print(describe(w), "-> overall=%.6f min_bit=%.6f" % (
            stats["overall_mean_hamming_fraction"], stats["min_bit_fraction"]))
    if not winners:
        print("(none)")


if __name__ == "__main__":
    main()
