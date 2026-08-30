"""Full state-space enumeration for pruned/wired PRNG at n=4,w=4 (default rows=4 -> G=1),
|S| = 4! * 2^(4*6) = 402,653,184.

Same in-degree / Poisson(1) chi-square test as enumerate_n2w4.py, but at
n=4 for direct comparison against the n=2 result. Vectorized over numpy arrays:
the domain is chunked by `cons` (16 chunks), and within each chunk every
(L-permutation x M x it) combination - 24 * 65536 * 16 = 25,165,824 states -
is advanced through next_state in one shot using elementwise numpy ops.
Correctness is cross-checked against scalar next_state reference on random
samples before the full run.
"""

from __future__ import annotations

import inspect
import itertools
import json
import random
import time
from collections import Counter
from math import factorial, exp
from pathlib import Path

import numpy as np
from scipy import stats

from pruned_wired_toy_prng import Params, init_state, next_state as scalar_next_state, state_key, tap_survivors


def rotw_vec(x, r, w, mask):
    r = r & (w - 1)
    x = x & mask
    return ((x << r) | (x >> (w - r))) & mask


def vec_next_state(L, M, cons, it, p):
    n, w, G = p.n, p.w, p.G
    mask = p.mask
    S13 = p.shifts["S13"]
    survivors = tap_survivors(G, w)

    N = L.shape[1]
    L = L.copy()
    M = M.copy()
    col = np.arange(N)

    cons_full = np.broadcast_to(np.asarray(cons, dtype=np.uint32) & mask, (N,)).copy()
    a = cons_full.copy()
    b = np.broadcast_to(np.asarray(it, dtype=np.uint32) & mask, (N,)).copy()
    c = np.zeros(N, dtype=np.uint32)
    d = np.zeros(N, dtype=np.uint32)

    for i in range(n - 1, 0, -1):
        o = np.zeros(N, dtype=np.uint32)
        for e in survivors:
            idx = (i + e) & (n - 1)
            o = o ^ ((M[idx] << e) & mask)

        a = ((d ^ o) ^ ((cons_full + a) & mask)) & mask
        b = (((cons_full + a) & mask) ^ ((o + d) & mask)) & mask

        shifted = (a >> S13) & mask
        pre_rot = (shifted ^ a) & mask
        c = rotw_vec(pre_rot, b, w, mask) & mask

        d = c & (n - 1)

        # per-lane swap: L[i], L[d] = L[d], L[i]
        old_Li = L[i].copy()
        gathered = L[d, col]
        L[i] = gathered
        L[d, col] = old_Li

    for i in range(n):
        M[i] = M[i] ^ L[i]

    # ra_hash_gen_sequential: M is read-only (pure function), no mutation of M
    rows = n // G
    new_cons = np.zeros(N, dtype=np.uint32)
    for e in range(G):
        base = e * rows
        h = np.zeros(N, dtype=np.uint32)
        for j in range(rows):
            h = h ^ M[base + j]
        new_cons = new_cons ^ (((h & mask) << e) & mask)

    new_it = (np.broadcast_to(np.asarray(it, dtype=np.uint32), (N,)) + 1) & mask

    return L, M, new_cons, new_it


def validate(p: Params, n_samples: int = 500, seed: int = 12345):
    """Cross-check vec_next_state against the scalar reference on random
    states drawn from the actual enumeration domain."""
    rng = random.Random(seed)
    n, w = p.n, p.w
    base_L = init_state(seed=0, p=p)[0]
    L_variants = list(set(itertools.permutations(base_L)))

    samples = []
    for _ in range(n_samples):
        L = list(rng.choice(L_variants))
        M = [rng.randrange(1 << w) for _ in range(n)]
        cons = rng.randrange(1 << w)
        it = rng.randrange(1 << w)
        samples.append((L, M, cons, it))

    # scalar reference
    scalar_results = []
    for L, M, cons, it in samples:
        nxt = scalar_next_state((L, M, cons, it), p)
        scalar_results.append(state_key(nxt))

    # vectorized, batched as one call
    Larr = np.array([[s[0][i] for s in samples] for i in range(n)], dtype=np.uint32)
    Marr = np.array([[s[1][i] for s in samples] for i in range(n)], dtype=np.uint32)
    cons_arr = np.array([s[2] for s in samples], dtype=np.uint32)
    it_arr = np.array([s[3] for s in samples], dtype=np.uint32)

    nL, nM, ncons, nit = vec_next_state(Larr, Marr, cons_arr, it_arr, p)

    mismatches = 0
    for k in range(n_samples):
        vec_key = (tuple(int(nL[i, k]) for i in range(n)),
                   tuple(int(nM[i, k]) for i in range(n)),
                   int(ncons[k]), int(nit[k]))
        if vec_key != scalar_results[k]:
            mismatches += 1
            if mismatches <= 3:
                print(f"  MISMATCH sample {k}: input={samples[k]}")
                print(f"    scalar={scalar_results[k]}")
                print(f"    vector={vec_key}")

    print(f"Validation: {n_samples - mismatches}/{n_samples} match scalar reference")
    return mismatches == 0


def pack_key(L, M, cons, it, n, w):
    key = np.asarray(it, dtype=np.uint64) & ((1 << w) - 1)
    key = key | ((np.asarray(cons, dtype=np.uint64) & ((1 << w) - 1)) << w)
    shift = 2 * w
    for i in range(n):
        key = key | ((M[i].astype(np.uint64) & ((1 << w) - 1)) << shift)
        shift += w
    for i in range(n):
        key = key | ((L[i].astype(np.uint64) & ((1 << w) - 1)) << shift)
        shift += w
    return key


def decode_batch(flat_idx, n_perms, n, w):
    idx = flat_idx.copy()
    base = 1 << w
    it = idx % base
    idx //= base
    ms = []
    for _ in range(n):
        ms.append(idx % base)
        idx //= base
    ms.reverse()
    lperm = idx
    return lperm, ms, it


def main():
    src = inspect.getsourcefile(scalar_next_state)
    print(f"Import resolution check: scalar_next_state is from {src}")
    assert "2026-8-28_periodicity-heuristic-validation2" in str(src), (
        f"scalar_next_state imported from wrong location: {src}"
    )

    p = Params(n=4, w=4)
    n, w = p.n, p.w
    expected_size = factorial(n) * (1 << (w * (n + 2)))
    print(f"Params: n={p.n} w={p.w} rows={p.rows} G={p.G}", flush=True)
    print(f"Expected |S| = n! * 2^(w*(n+2)) = {expected_size}", flush=True)

    print("\nValidating vectorized next_state against scalar reference...", flush=True)
    ok = validate(p)
    if not ok:
        print("VALIDATION FAILED - aborting.", flush=True)
        return
    print("Validation OK.\n", flush=True)

    base_L = init_state(seed=0, p=p)[0]
    L_variants = sorted(set(itertools.permutations(base_L)))
    n_perms = len(L_variants)
    L_base_arr = np.array(L_variants, dtype=np.uint32)
    print(f"L permutations: {n_perms} (expected {factorial(n)})", flush=True)

    Nchunk = n_perms * (1 << (w * n)) * (1 << w)
    print(f"States per cons-chunk: {Nchunk:,}  x 16 cons values = {Nchunk * 16:,}", flush=True)

    BATCH = 2_000_000
    total_indeg = Counter()
    t0 = time.time()
    total_states = 0
    for cons_val in range(1 << w):
        for start in range(0, Nchunk, BATCH):
            end = min(start + BATCH, Nchunk)
            flat = np.arange(start, end, dtype=np.int64)
            lperm, ms, it = decode_batch(flat, n_perms, n, w)

            L_batch = L_base_arr[lperm].T.astype(np.uint32).copy()
            M_batch = np.stack([m.astype(np.uint32) for m in ms])
            it_batch = it.astype(np.uint32)

            nL, nM, ncons, nit = vec_next_state(L_batch, M_batch, cons_val, it_batch, p)
            keys = pack_key(nL, nM, ncons, nit, n, w)
            uniq, counts = np.unique(keys, return_counts=True)
            for u, cnt in zip(uniq.tolist(), counts.tolist()):
                total_indeg[u] += cnt
            total_states += (end - start)

        elapsed = time.time() - t0
        rate = total_states / elapsed if elapsed > 0 else 0
        print(f"  cons={cons_val:2d}/15 done, cumulative states={total_states:,}, "
              f"elapsed={elapsed:.1f}s, rate={rate:,.0f} states/s, "
              f"distinct_images_so_far={len(total_indeg):,}", flush=True)

    N = total_states
    assert N == expected_size, f"domain size mismatch: {N} != {expected_size}"

    distinct_images = len(total_indeg)
    k_counts = Counter(total_indeg.values())
    k_counts[0] = N - distinct_images

    max_k_observed = max(k_counts)
    print(f"\nMax in-degree observed: {max_k_observed}")
    print(f"Distinct image points: {distinct_images} / {N} "
          f"({100*distinct_images/N:.4f}% of S is in F's image)")

    print("\nIn-degree histogram (k = preimage count, nonzero bins only):")
    for k in sorted(k_counts):
        c = k_counts[k]
        print(f"  k={k:4d}: {c:10d} points  ({100*c/N:.6f}%)")

    CUTOFF = 6
    poisson_pmf = lambda k: exp(-1) / factorial(k)
    observed = []
    expected = []
    for k in range(CUTOFF):
        observed.append(k_counts.get(k, 0))
        expected.append(poisson_pmf(k) * N)
    tail_p = 1.0 - sum(poisson_pmf(k) for k in range(CUTOFF))
    observed.append(sum(c for k, c in k_counts.items() if k >= CUTOFF))
    expected.append(tail_p * N)

    print(f"\nChi-square bins (k=0..{CUTOFF-1} individually, k>={CUTOFF} lumped):")
    for k, (o, e) in enumerate(zip(observed, expected)):
        label = f"k={k}" if k < CUTOFF else f"k>={CUTOFF}"
        print(f"  {label:6s}: observed={o:12d}  expected={e:14.2f}")

    chi2, pvalue = stats.chisquare(f_obs=observed, f_exp=expected)
    dof = len(observed) - 1
    print(f"\nChi-square statistic: {chi2:.4f}  (dof={dof})")
    print(f"p-value: {pvalue:.6g}")
    if pvalue < 0.01:
        verdict = "REJECTS Poisson(1) at alpha=0.01 - F's collision structure deviates significantly from a uniform random mapping"
    else:
        verdict = "FAILS to reject Poisson(1) - consistent with F behaving like a uniform random mapping, at least by this test"
    print(f"Verdict: {verdict}")

    result = {
        "n": n, "w": w, "rows": p.rows, "G": p.G,
        "state_space_size": N,
        "distinct_image_points": distinct_images,
        "image_fraction": distinct_images / N,
        "max_in_degree": max_k_observed,
        "in_degree_histogram": {str(k): v for k, v in sorted(k_counts.items())},
        "chi_square": {
            "cutoff": CUTOFF,
            "observed": observed,
            "expected": expected,
            "statistic": chi2,
            "dof": dof,
            "pvalue": pvalue,
        },
        "verdict": verdict,
        "elapsed_seconds": time.time() - t0,
    }
    with open(Path(__file__).parent / "enumerate_n4w4_result.json", "w") as f:
        json.dump(result, f, indent=2)
    print("\nSaved full result to enumerate_n4w4_result.json")


if __name__ == "__main__":
    main()
