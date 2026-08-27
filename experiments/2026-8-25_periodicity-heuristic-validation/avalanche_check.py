"""Scaled-down avalanche / bit-flip sanity check for toy_prng.py.

Port of experiments/others/avalanche_effect_analysis.py's approach (single
outer iteration, no rehashing, capture the sequence of `c` values, flip one
seed bit at a time, measure Hamming-distance convergence) onto the (n, w)
toy generator in this directory.

This is the handover's explicit gate (step 2): if flipping a seed bit does
not converge toward ~w/2 bits of difference across the inner-loop outputs,
the toy model's shift/constant rescaling is wrong and cycle measurement in
cycle_measure.py / run_sweep.py must not be trusted.
"""

from __future__ import annotations

from toy_prng import Params, init_state, rotw


def capture_inner_outputs(seed: int, p: Params) -> list[int]:
    """One outer iteration, no rehash - mirrors zepfold_capture() in
    experiments/others/avalanche_effect_analysis.py, generalized to (n, w).
    Returns the sequence of `c` values produced at each inner-loop index.
    """
    mask = p.mask
    n, w, G = p.n, p.w, p.G
    s = p.shifts
    S9, S18, S13, S14 = s["S9"], s["S18"], s["S13"], s["S14"]

    st = init_state(seed=seed, p=p)
    L, M, cons, it = st

    outputs = []
    a = cons
    b = 0
    c = 0
    for i in range(n - 1, 0, -1):
        o = 0
        for e in range(G):
            idx = (i + e) & (n - 1)
            o ^= (M[idx] << e) & mask

        a = (rotw(b ^ o, c, w) ^ ((cons + a) & mask)) & mask
        b = (rotw((cons + a) & mask, i, w) ^ ((o + c) & mask)) & mask
        o = ((rotw(a ^ o, i, w) << S9) & mask) ^ (b >> S18)
        c = rotw((((o + c) & mask) << S14 & mask) ^ (b >> S13) ^ a, b, w) & mask
        outputs.append(c)
        c = (c * (i + 1)) >> w

    return outputs


def hamming(x: int, y: int) -> int:
    return bin(x ^ y).count("1")


def run_avalanche_check(p: Params, base_seed: int = 1) -> dict:
    """Flip each bit of base_seed, compare captured c-sequences against the
    unflipped baseline, return per-position mean Hamming distance (in bits)
    plus the overall mean as a fraction of w (target: ~0.5).
    """
    baseline = capture_inner_outputs(base_seed & p.mask, p)
    n_outputs = len(baseline)

    per_bit_means = []
    for bit in range(p.w):
        flipped_seed = (base_seed ^ (1 << bit)) & p.mask
        mutated = capture_inner_outputs(flipped_seed, p)
        dists = [hamming(bo, mo) for bo, mo in zip(baseline, mutated)]
        per_bit_means.append(sum(dists) / len(dists) if dists else 0.0)

    overall_mean_bits = sum(per_bit_means) / len(per_bit_means) if per_bit_means else 0.0
    return {
        "n": p.n,
        "w": p.w,
        "n_outputs": n_outputs,
        "per_bit_mean_hamming": per_bit_means,
        "overall_mean_hamming_bits": overall_mean_bits,
        "overall_mean_hamming_fraction": overall_mean_bits / p.w if p.w else 0.0,
    }


if __name__ == "__main__":
    for n, w in [(2, 8), (4, 8), (2, 4), (4, 4), (8, 4)]:
        p = Params(n=n, w=w)
        result = run_avalanche_check(p)
        frac = result["overall_mean_hamming_fraction"]
        verdict = "OK (~50%)" if 0.3 <= frac <= 0.7 else "SUSPECT"
        print(f"n={n:2d} w={w}: mean Hamming = {result['overall_mean_hamming_bits']:.2f}/{w} "
              f"bits ({frac:.1%})  [{verdict}]")
