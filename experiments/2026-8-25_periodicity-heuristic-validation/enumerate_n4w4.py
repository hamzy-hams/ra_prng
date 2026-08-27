"""Full state-space enumeration for n=4,w=4 (default rows=4 -> G=1),
|S| = 4! * 2^(4*6) = 402,653,184.

Same in-degree / Poisson(1) chi-square test as enumerate_n2w4.py, but at
n=4 for direct comparison against the n=2 result. Too large for the pure
scalar-Python approach used at n=2 (~4h+ estimated at that throughput), so
this vectorizes next_state over numpy arrays: the domain is chunked by
`cons` (16 chunks), and within each chunk every (L-permutation x M x it)
combination - 24 * 65536 * 16 = 25,165,824 states - is advanced through
next_state in one shot using elementwise numpy ops. Correctness is
cross-checked against the scalar toy_prng.next_state reference on random
samples before the full run.
"""

from __future__ import annotations

import itertools
import json
import random
import time
from collections import Counter
from math import factorial, exp

import numpy as np
from scipy import stats

from toy_prng import Params, init_state, next_state as scalar_next_state, state_key


def rotw_vec(x, r, w, mask):
    """Vectorized rotw: rotate low w bits of x left by r bits (mod w).
    x, r are numpy uint32 arrays (or broadcastable); r need not be
    pre-reduced mod w. Correct even when a lane's r is 0 (x >> w on a
    masked w-bit value is 0, matching the scalar `if r else x` branch).
    """
    r = r & (w - 1)
    x = x & mask
    return ((x << r) | (x >> (w - r))) & mask


def vec_next_state(L, M, cons, it, p):
    """L, M: numpy uint32 arrays of shape (n, N) - row i is position i's
    value across all N states in the batch. cons, it: numpy uint32 arrays
    of shape (N,) (or python ints, broadcast).
    Returns (new_L, new_M, new_cons, new_it), same shapes.
    Direct array port of toy_prng.next_state / ra_hash_gen.
    """
    n, w, G = p.n, p.w, p.G
    mask = p.mask
    s = p.shifts
    S9, S18, S13, S14 = s["S9"], s["S18"], s["S13"], s["S14"]
    rows = n // G

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
        for e in range(G):
            idx = (i + e) & (n - 1)
            o = o ^ ((M[idx] << e) & mask)

        a = (rotw_vec(b ^ o, d, w, mask) ^ ((cons_full + a) & mask)) & mask
        b = (rotw_vec((cons_full + a) & mask, np.uint32(i), w, mask) ^ ((o + d) & mask)) & mask
        o = ((rotw_vec(a ^ o, np.uint32(i), w, mask) << S9) & mask) ^ (b >> S18)
        c = rotw_vec((((o + c) & mask) << S14 & mask) ^ (b >> S13) ^ a, b, w, mask) & mask

        d = ((c.astype(np.uint64) * np.uint64(i + 1)) >> np.uint64(w)).astype(np.uint32)

        # per-lane swap: L[i], L[d] = L[d], L[i]
        old_Li = L[i].copy()
        gathered = L[d, col]
        L[i] = gathered
        L[d, col] = old_Li

    for i in range(n):
        M[i] = M[i] ^ L[i]

    # ra_hash_gen, vectorized: out[k] for k in range(G); N-array here is M
    out = [np.zeros(N, dtype=np.uint32) for _ in range(G)]
    Mw = M.copy()  # ra_hash_gen mutates its input array in place
    for k in range(G):
        idx = Mw[k] & (n - 1)  # per-lane gather index into Mw's *current* rows
        # gather Mw[idx[lane], lane] for each lane
        out[k] = out[k] ^ Mw[idx, col]
        acc = np.zeros(N, dtype=np.uint32)
        for j in range(rows):
            acc = acc ^ Mw[j * G + k]
        Mw[k] = (Mw[k] ^ acc) & mask

    new_cons = np.zeros(N, dtype=np.uint32)
    for e in range(G):
        new_cons = new_cons ^ ((out[e] << e) & mask)

    new_it = (np.broadcast_to(np.asarray(it, dtype=np.uint32), (N,)) + 1) & mask

    return L, Mw, new_cons, new_it


def validate(p: Params, n_samples: int = 500, seed: int = 12345):
    """Cross-check vec_next_state against the scalar reference on random
    states drawn from the actual enumeration domain."""
    rng = random.Random(seed)
    n, w = p.n, p.w
    base_L = init_state(seed=0, p=p)[0]
    L_variants = list(set(itertools.permutations(base_L)))
    mask = p.mask

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
    """Pack (L, M, cons, it) arrays into a single uint64 key array, w bits
    per field, for fast counting via np.unique instead of Python tuples."""
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
    """Mixed-radix decompose a flat index array into (lperm, m_0..m_{n-1}, it)
    component arrays, fastest-varying last (it), without ever materializing
    Python-level tuples - pure numpy arithmetic, safe for huge batches."""
    idx = flat_idx.copy()
    base = 1 << w
    it = idx % base
    idx //= base
    ms = []
    for _ in range(n):
        ms.append(idx % base)
        idx //= base
    ms.reverse()  # so ms[0]=m0 .. ms[n-1]=m_{n-1}
    lperm = idx  # remaining value, < n_perms
    return lperm, ms, it


def main():
    p = Params(n=4, w=4)
    n, w = p.n, p.w
    expected_size = factorial(n) * (1 << (w * (n + 2)))
    print(f"Params: n={p.n} w={p.w} rows={p.rows} G={p.G}", flush=True)
    print(f"Expected |S| = n! * 2^(w*(n+2)) = {expected_size}", flush=True)

    print("\nValidating vectorized next_state against scalar reference...", flush=True)
    ok = validate(p)
    if not ok:
        print("VALIDATION FAILED - aborting, do not trust a full run until fixed.", flush=True)
        return
    print("Validation OK.\n", flush=True)

    base_L = init_state(seed=0, p=p)[0]
    L_variants = sorted(set(itertools.permutations(base_L)))
    n_perms = len(L_variants)
    L_base_arr = np.array(L_variants, dtype=np.uint32)  # shape (n_perms, n)
    print(f"L permutations: {n_perms} (expected {factorial(n)})", flush=True)

    Nchunk = n_perms * (1 << (w * n)) * (1 << w)  # per-cons-value chunk size (all Lperm x M x it)
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

            L_batch = L_base_arr[lperm].T.astype(np.uint32).copy()  # (n, batch)
            M_batch = np.stack([m.astype(np.uint32) for m in ms])   # (n, batch)
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
    with open("enumerate_n4w4_result.json", "w") as f:
        json.dump(result, f, indent=2)
    print("\nSaved full result to enumerate_n4w4_result.json")


if __name__ == "__main__":
    main()
