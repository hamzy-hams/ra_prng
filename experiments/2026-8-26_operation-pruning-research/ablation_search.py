"""Driver for the operation-pruning greedy ablation search.

Two phases:

  Phase A: try the "sequential" HASH_ACCESS variant (see operations.py) with
  all 18 boolean flags ON, HASH_DEPTH fixed at 32 in both variants. If it
  clears the quality gate, it becomes the fixed hash-access mode for Phase B
  (a candidate speedup from cache-friendlier indexing, at unchanged mixing
  depth). If it fails, Phase B falls back to the original "strided" access.
  HASH_DEPTH itself is never varied -- see operations.HASH_DEPTH_FIXED for
  why (a prior run confirmed shrinking it lets broken candidates hide from a
  too-weak gate).

  Phase B: greedy ablation over the 18 boolean flags at the Phase-A-chosen
  hash access mode. Each round scans remaining flags in a fixed order (the 8
  TAPs, then the 4 ROTs, then the 4 SHL/SHR terms, then MULT_REDUCE, then
  HASH_SELFIDX), removes the first one whose removal still clears both
  quality-gate tiers, and restarts the round from the new candidate. Stops
  when a full round removes nothing.

MIN_ACTIVE_TAPS constraint (added after Step-1 diagnostic, per user
decision): every unconstrained run so far -- 3 separate converged candidates,
across different gate sizes and both HASH_DEPTH policies -- independently
converged to removing ALL 8 TAP flags, and all 3 failed hard (BCFN/Gap-16)
once validated at a large enough PractRand tier, even when the *loop* gate
had passed them cleanly. That's a consistent enough pattern to treat "all
taps gone" as the likely root cause worth testing directly, before reaching
for a heavier continuous-parameter repair phase (shift/rotate-amount
tuning). This run forbids the greedy loop from ever dropping the active TAP
count below MIN_ACTIVE_TAPS -- cheap to test, and answers whether the
problem really is "no M contribution at all" vs. something requiring
rebalancing.

MIN_ACTIVE_ROTS constraint (added after the shift-repair search failed --
see RESULTS.md/STATUS.md "shift-repair"): the min-taps-constrained winner
(TAP6,TAP7,SHL9,SHL14,SHR13,MULT_REDUCE) had removed ALL 4 ROT_* flags. A
grid search over the surviving SHL/SHR widths, scored by avalanche fraction,
found points with near-perfect avalanche balance that nonetheless FAILED
PractRand hard (FPF-14+6/16/DC6, low-order-bit correlation) at 256MB-2GB --
the original widths (worse avalanche score) stayed clean to 2GB. Per user
guidance (ROT_* was one of the original algorithm's fastest diffusion
contributors), the next hypothesis is that removing an entire ROT_*
category -- same pattern as the TAP trap -- is itself the defect, not
something shift-width tuning can repair. This constraint mirrors
MIN_ACTIVE_TAPS for the ROTS group.

Every trial/accept/converge event is appended to ablation_log.jsonl (one JSON
object per line) for STATUS.md/RESULTS.md to summarize afterward, and so a
later session can see exactly what was already decided.
"""

from __future__ import annotations

import json
import sys
import time
from pathlib import Path

from operations import (
    ALL_FLAGS, TAPS, ROTS, SHIFTS, MULT_REDUCE, HASH_SELFIDX,
    HASH_ACCESS_STRIDED, HASH_ACCESS_SEQUENTIAL, HASH_DEPTH_FIXED, describe,
)
from pruned_prng import Candidate
from quality_gate import quality_gate, DEFAULT_PRACTRAND_BYTES

LOG_PATH = Path(__file__).parent / "ablation_log.jsonl"

# Fixed scan order for Phase B: TAPs first (cheapest single ops, but most of
# them -- removing several taps compounds), then ROTs (each a real rot32
# call), then the SHL/SHR term-drops, then the two remaining single flags.
SCAN_ORDER = list(TAPS) + list(ROTS) + list(SHIFTS) + [MULT_REDUCE, HASH_SELFIDX]
assert set(SCAN_ORDER) == ALL_FLAGS


def _log(event: dict):
    event["ts"] = time.time()
    with LOG_PATH.open("a") as f:
        f.write(json.dumps(event) + "\n")
    print(json.dumps(event))


def phase_a_hash_access_check(n_bytes: int = DEFAULT_PRACTRAND_BYTES) -> str:
    _log({"phase": "A", "event": "start", "hash_depth_fixed": HASH_DEPTH_FIXED})
    chosen = HASH_ACCESS_STRIDED
    cand = Candidate(ops=frozenset(ALL_FLAGS), hash_access=HASH_ACCESS_SEQUENTIAL)
    t0 = time.time()
    result = quality_gate(cand, n_bytes=n_bytes)
    elapsed = time.time() - t0
    _log({
        "phase": "A", "event": "trial", "hash_access": HASH_ACCESS_SEQUENTIAL,
        "passed": result["passed"], "tier": result["tier"],
        "avalanche_fraction": result["avalanche"]["overall_mean_hamming_fraction"],
        "elapsed_s": elapsed,
    })
    if result["passed"]:
        chosen = HASH_ACCESS_SEQUENTIAL
    _log({"phase": "A", "event": "done", "chosen_hash_access": chosen})
    return chosen


def phase_b_greedy_ablation(hash_access: str, n_bytes: int = DEFAULT_PRACTRAND_BYTES,
                             min_active_taps: int = 2, min_active_rots: int = 0) -> Candidate:
    current = frozenset(ALL_FLAGS)
    _log({"phase": "B", "event": "start", "hash_access": hash_access,
          "min_active_taps": min_active_taps, "min_active_rots": min_active_rots,
          "baseline": describe(current, hash_access)})

    floors = [(TAPS, min_active_taps, "min_taps"), (ROTS, min_active_rots, "min_rots")]

    improved = True
    while improved:
        improved = False
        for op in SCAN_ORDER:
            if op not in current:
                continue
            skip = False
            for group, floor, label in floors:
                if op in group and sum(1 for g in group if g in current) <= floor:
                    _log({"phase": "B", "event": f"skipped_{label}", "op_removed": op,
                          "active": sum(1 for g in group if g in current)})
                    skip = True
                    break
            if skip:
                continue
            trial_ops = current - {op}
            cand = Candidate(ops=trial_ops, hash_access=hash_access)
            t0 = time.time()
            result = quality_gate(cand, n_bytes=n_bytes)
            elapsed = time.time() - t0
            _log({
                "phase": "B", "event": "trial", "op_removed": op,
                "passed": result["passed"], "tier": result["tier"],
                "avalanche_fraction": result["avalanche"]["overall_mean_hamming_fraction"],
                "elapsed_s": elapsed,
                "candidate": describe(trial_ops, hash_access),
            })
            if result["passed"]:
                current = trial_ops
                improved = True
                _log({"phase": "B", "event": "accepted", "op_removed": op,
                      "candidate": describe(current, hash_access)})
                break  # restart the round from the new candidate
    _log({"phase": "B", "event": "converged", "final": describe(current, hash_access)})
    return Candidate(ops=current, hash_access=hash_access)


def main():
    n_bytes = DEFAULT_PRACTRAND_BYTES
    if len(sys.argv) > 1:
        n_bytes = int(sys.argv[1]) * 1024 * 1024
    min_active_taps = int(sys.argv[2]) if len(sys.argv) > 2 else 2
    min_active_rots = int(sys.argv[3]) if len(sys.argv) > 3 else 0

    hash_access = phase_a_hash_access_check(n_bytes=n_bytes)
    final = phase_b_greedy_ablation(hash_access, n_bytes=n_bytes,
                                     min_active_taps=min_active_taps, min_active_rots=min_active_rots)
    print("\n=== FINAL CANDIDATE ===")
    print(describe(final.ops, final.hash_access))


if __name__ == "__main__":
    main()
