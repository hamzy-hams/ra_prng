"""Q1 Method A: Pearson cross-correlation between pairs of independent
`--stream <seed> <n>` streams. HANDOVER.md question 1: do adjacent seeds
(0,1,2,...) produce statistically correlated streams?

Two seed groups are compared so we can tell "correlated because adjacent"
apart from "correlated in general, at this n": an *adjacent* group (seeds
0..K-1) and a *control* group (K seeds scattered across the full uint32
range). If adjacency has no effect, both groups should show the same
(near-zero) rate of flagged pairs.

Under H0 (independent streams, output uniform on [0, 2^32)), Pearson r for
n samples is asymptotically N(0, 1/n) by the CLT (finite variance is all
that's required -- holds here since output is bounded). So
z = r * sqrt(n) ~ N(0,1) under H0; flag |z| > z_crit at a
Bonferroni-corrected family-wise alpha for the m = C(K,2) pairs tested.

Caveat (documented, not silently glossed over): this only catches LINEAR,
lag-0, element-aligned correlation. Nonlinear/bit-level dependence is out
of scope here -- that's what Method B (interleave_practrand.py) is for.
"""

from __future__ import annotations

import json
import random

import numpy as np
from scipy import stats

from common import HERE, TIERS_Q1A, stream_values

ALPHA = 0.01


def build_group(seeds: list[int], n: int) -> np.ndarray:
    """Return float64 matrix (K, n) of stream values for the given seeds."""
    return np.stack([stream_values(s, n).astype(np.float64) for s in seeds])


def analyze_group(data: np.ndarray, label: str) -> dict:
    k, n = data.shape
    m = k * (k - 1) // 2
    z_crit = stats.norm.ppf(1 - ALPHA / (2 * m))

    R = np.corrcoef(data)
    flagged = []
    for i in range(k):
        for j in range(i + 1, k):
            r = R[i, j]
            z = r * np.sqrt(n)
            if abs(z) > z_crit:
                flagged.append({"i": i, "j": j, "r": float(r), "z": float(z)})

    print(f"[{label}] K={k} n={n} m_pairs={m} z_crit={z_crit:.3f} "
          f"flagged={len(flagged)} ({100*len(flagged)/m:.3f}%)")
    return {"k": k, "n": n, "m_pairs": m, "z_crit": z_crit,
            "flagged_count": len(flagged), "flagged": flagged}


def run(tier: str) -> dict:
    k, n = TIERS_Q1A[tier]
    print(f"=== cross_correlation tier={tier}: K={k} n={n:,} ===")

    adjacent_seeds = list(range(k))
    control_seeds = random.Random(42).sample(range(0, 2**32), k)

    adjacent_data = build_group(adjacent_seeds, n)
    control_data = build_group(control_seeds, n)

    result = {
        "tier": tier,
        "adjacent": analyze_group(adjacent_data, "adjacent"),
        "control": analyze_group(control_data, "control"),
    }
    return result


def main():
    import sys
    tier = sys.argv[1] if len(sys.argv) > 1 else "full"
    result = run(tier)
    out_path = HERE / f"cross_correlation_results_{tier}.json"
    out_path.write_text(json.dumps(result, indent=2))
    print(f"\nWrote {out_path}")

    adj_rate = result["adjacent"]["flagged_count"] / result["adjacent"]["m_pairs"]
    ctl_rate = result["control"]["flagged_count"] / result["control"]["m_pairs"]
    print(f"\nadjacent flagged rate={adj_rate:.4f} vs control flagged rate={ctl_rate:.4f} "
          f"(expected under H0: ~{ALPHA:.4f} family-wise, so per-test rate near 0)")


if __name__ == "__main__":
    main()
