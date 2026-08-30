"""Full state-space enumeration for pruned/wired PRNG at n=2,w=4 (|S|=131072, default rows=2).

Directly tests the core assumption behind the lambda ~ 0.7824*sqrt(|S|)
periodicity heuristic: that F=next_state behaves like a uniformly random
self-map on S. For a random self-map on a finite set S of size N, the
number of preimages (in-degree) of a fixed point of the image follows
approximately Poisson(1) as N -> infinity. This script computes F's
true in-degree distribution by brute force over the entire domain, and
chi-square-tests it against that Poisson(1) prediction.
"""

from __future__ import annotations

import inspect
import itertools
import json
from collections import Counter
from math import factorial, exp
from pathlib import Path

from scipy import stats

from pruned_wired_toy_prng import Params, init_state, next_state, state_key


def enumerate_state_space(p: Params):
    mask = p.mask
    n, w = p.n, p.w

    base_L = init_state(seed=0, p=p)[0]
    L_variants = list(set(itertools.permutations(base_L)))

    M_range = range(1 << w)
    cons_range = range(1 << w)
    it_range = range(1 << w)

    for L, M, cons, it in itertools.product(
        L_variants, itertools.product(M_range, repeat=n), cons_range, it_range
    ):
        yield (list(L), list(M), cons, it)


def main():
    # Sanity-check import resolution to prevent sibling-shadowing bugs
    src = inspect.getsourcefile(next_state)
    print(f"Import resolution check: next_state is from {src}")
    assert "2026-8-28_periodicity-heuristic-validation2" in str(src), (
        f"next_state imported from wrong location: {src}"
    )

    p = Params(n=2, w=4)
    n, w = p.n, p.w
    expected_size = factorial(n) * (1 << (w * (n + 2)))
    print(f"Params: n={p.n} w={p.w} rows={p.rows} G={p.G}")
    print(f"Expected |S| = n! * 2^(w*(n+2)) = {expected_size}")

    indeg = Counter()
    count = 0
    for state in enumerate_state_space(p):
        nxt = next_state(state, p)
        indeg[state_key(nxt)] += 1
        count += 1

    N = count
    print(f"Actual states enumerated: {N}")
    assert N == expected_size, "domain size mismatch - enumeration bug"

    distinct_images = len(indeg)
    k_counts = Counter(indeg.values())
    k_counts[0] = N - distinct_images  # points with zero preimages

    max_k_observed = max(k_counts)
    print(f"\nMax in-degree observed: {max_k_observed}")
    print(f"Distinct image points: {distinct_images} / {N} "
          f"({100*distinct_images/N:.2f}% of S is in F's image)")

    print("\nIn-degree histogram (k = preimage count):")
    for k in range(max_k_observed + 1):
        c = k_counts.get(k, 0)
        if c:
            print(f"  k={k:2d}: {c:8d} points  ({100*c/N:.4f}%)")

    # --- chi-square goodness-of-fit vs Poisson(1) ---
    CUTOFF = 6
    poisson_pmf = lambda k: exp(-1) / factorial(k)

    observed = []
    expected = []
    for k in range(CUTOFF):
        observed.append(k_counts.get(k, 0))
        expected.append(poisson_pmf(k) * N)

    tail_p = 1.0 - sum(poisson_pmf(k) for k in range(CUTOFF))
    observed.append(sum(k_counts.get(k, 0) for k in range(CUTOFF, max_k_observed + 1)))
    expected.append(tail_p * N)

    print(f"\nChi-square bins (k=0..{CUTOFF-1} individually, k>={CUTOFF} lumped):")
    for k, (o, e) in enumerate(zip(observed, expected)):
        label = f"k={k}" if k < CUTOFF else f"k>={CUTOFF}"
        print(f"  {label:6s}: observed={o:8d}  expected={e:10.2f}")

    chi2, pvalue = stats.chisquare(f_obs=observed, f_exp=expected)
    dof = len(observed) - 1
    print(f"\nChi-square statistic: {chi2:.4f}  (dof={dof})")
    print(f"p-value: {pvalue:.6g}")
    if pvalue < 0.01:
        verdict = "REJECTS Poisson(1) at alpha=0.01 - F's collision structure deviates significantly from a uniform random mapping"
    elif pvalue < 0.05:
        verdict = "rejects Poisson(1) at alpha=0.05 (marginal)"
    else:
        verdict = "FAILS to reject Poisson(1) - consistent with F behaving like a uniform random mapping, at least by this test"
    print(f"Verdict: {verdict}")

    result = {
        "n": n, "w": w, "rows": p.rows, "G": p.G,
        "state_space_size": N,
        "distinct_image_points": distinct_images,
        "image_fraction": distinct_images / N,
        "max_in_degree": max_k_observed,
        "in_degree_histogram": {str(k): k_counts.get(k, 0) for k in range(max_k_observed + 1)},
        "chi_square": {
            "cutoff": CUTOFF,
            "observed": observed,
            "expected": expected,
            "statistic": chi2,
            "dof": dof,
            "pvalue": pvalue,
        },
        "verdict": verdict,
    }
    with open(Path(__file__).parent / "enumerate_n2w4_result.json", "w") as f:
        json.dump(result, f, indent=2)
    print("\nSaved full result to enumerate_n2w4_result.json")


if __name__ == "__main__":
    main()
