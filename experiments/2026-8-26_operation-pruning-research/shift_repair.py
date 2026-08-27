"""Post-convergence "shift repair" search.

User's request after seeing the winning discrete-ablation candidate (6/18
ops active: TAP6, TAP7, SHL9, SHL14, SHR13, MULT_REDUCE, hash_access=
sequential): before running the full validation battery, check whether the
*shift widths* inherited unchanged from the original algorithm (9, 18, 14,
13) are actually the best choice for this reduced op set, or whether some
other width finds a more "chaotic" (better-diffusing) point -- same op set,
same TAP count, only the SHL9/SHL14/SHR13 constants vary. (SHR18 is OFF in
this candidate, so s18 is irrelevant here and left at its original value.)

Motivating observation: the winning candidate's avalanche fraction at the
ORIGINAL widths is only ~36% (target ~50%) -- notably worse than the ~50%
every earlier full-op-set candidate showed. That's a concrete sign the
inherited widths may not suit this smaller op set, i.e. exactly the
"balance got disturbed by removing operations" mechanism the user
hypothesized earlier in this session.

Method: full grid search over s9, s14, s13 in [1,31] (s18 fixed at 18,
unused by this candidate), scored by avalanche_gate's
overall_mean_hamming_fraction -- cheap (pure Python, ~28ms/eval, no
subprocess), so a full 31^3 grid (~14 min) is tractable, unlike PractRand.
Objective: minimize |fraction - 0.5| (closest to ideal 50% diffusion =
"most chaos"). Top candidates are then re-checked with the real two-tier
gate (quality_gate, which adds the C-backed PractRand tier) before trusting
any of them -- avalanche alone is a proxy, not proof.
"""

from __future__ import annotations

import json
import time
from pathlib import Path

from pruned_prng import Candidate
from quality_gate import avalanche_gate

LOG_PATH = Path(__file__).parent / "shift_repair_log.jsonl"

WINNING_OPS = frozenset(["TAP6", "TAP7", "SHL9", "SHL14", "SHR13", "MULT_REDUCE"])
HASH_ACCESS = "sequential"
ORIGINAL_WIDTHS = (9, 18, 14, 13)
S18_FIXED = 18  # SHR18 is OFF in WINNING_OPS -- irrelevant, kept at original


def score(fraction: float) -> float:
    return abs(fraction - 0.5)


def main():
    t_start = time.time()
    best = None  # (score, s9, s14, s13, fraction)
    results = []

    total = 31 * 31 * 31
    done = 0
    for s9 in range(1, 32):
        for s14 in range(1, 32):
            for s13 in range(1, 32):
                cand = Candidate(ops=WINNING_OPS, hash_access=HASH_ACCESS,
                                  shift_widths=(s9, S18_FIXED, s14, s13))
                r = avalanche_gate(cand)
                frac = r["overall_mean_hamming_fraction"]
                sc = score(frac)
                results.append((sc, s9, s14, s13, frac))
                if best is None or sc < best[0]:
                    best = (sc, s9, s14, s13, frac)
                done += 1
        elapsed = time.time() - t_start
        print(f"s9={s9} done, {done}/{total} evals, {elapsed:.1f}s elapsed, "
              f"best so far: s9={best[1]} s14={best[2]} s13={best[3]} "
              f"frac={best[4]:.4f} score={best[0]:.4f}")

    results.sort(key=lambda t: t[0])
    top20 = results[:20]

    with LOG_PATH.open("w") as f:
        f.write(json.dumps({
            "original_widths": ORIGINAL_WIDTHS,
            "original_score": score(avalanche_gate(
                Candidate(ops=WINNING_OPS, hash_access=HASH_ACCESS, shift_widths=ORIGINAL_WIDTHS)
            )["overall_mean_hamming_fraction"]),
            "top20": [{"s9": s9, "s14": s14, "s13": s13, "s18": S18_FIXED,
                       "fraction": frac, "score": sc} for sc, s9, s14, s13, frac in top20],
            "total_evals": total,
            "elapsed_s": time.time() - t_start,
        }, indent=2) + "\n")

    print("\n=== TOP 20 (closest to 50% avalanche) ===")
    for sc, s9, s14, s13, frac in top20:
        print(f"s9={s9:2d} s14={s14:2d} s13={s13:2d}  fraction={frac:.4f}  score={sc:.4f}")

    orig_frac = avalanche_gate(
        Candidate(ops=WINNING_OPS, hash_access=HASH_ACCESS, shift_widths=ORIGINAL_WIDTHS)
    )["overall_mean_hamming_fraction"]
    print(f"\nOriginal widths (9,18,14,13): fraction={orig_frac:.4f} score={score(orig_frac):.4f}")


if __name__ == "__main__":
    main()
