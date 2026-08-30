"""Scaled-down avalanche / bit-flip sanity check for pruned_wired_toy_prng.py.

Port of ../2026-8-25_periodicity-heuristic-validation/avalanche_check.py
onto the pruned/wired (n, w, rows) toy generator in this directory. Same
gate: if flipping a seed bit does not converge toward ~w/2 bits of
difference across the inner-loop outputs, the toy model must not be
trusted for cycle measurement (run_sweep.py / cycle_measure.py).

Config list extends the original's 5 with 2 new configs chosen so tap
pruning is non-vacuous (G>=4, see pruned_wired_toy_prng.py's module
docstring for why G=8 configs are infeasible/degenerate and excluded).
"""

from __future__ import annotations

from pruned_wired_toy_prng import Params, init_state, rotw, tap_survivors


def capture_inner_outputs(seed: int, p: Params) -> list[int]:
    """One outer iteration, no rehash -- mirrors next_state's inner loop
    exactly (tap-XOR, a/b/c chain, d update) but without the L swap or the
    M mutation, since neither feeds back into the c-sequence within a
    single outer iteration (M is read-only until after the loop; L is only
    ever a swap target, never read by the tap/a/b/c chain).
    """
    mask = p.mask
    n, w, G = p.n, p.w, p.G
    S13 = p.shifts["S13"]
    survivors = tap_survivors(G, w)

    st = init_state(seed=seed, p=p)
    L, M, cons, it = st

    outputs = []
    a = cons
    b = 0
    d = 0
    for i in range(n - 1, 0, -1):
        o = 0
        for e in survivors:
            idx = (i + e) & (n - 1)
            o ^= (M[idx] << e) & mask

        a = ((d ^ o) ^ ((cons + a) & mask)) & mask
        b = (((cons + a) & mask) ^ ((o + d) & mask)) & mask
        shifted = (a >> S13) & mask
        pre_rot = (shifted ^ a) & mask
        c = rotw(pre_rot, b, w) & mask
        outputs.append(c)
        d = c & (n - 1)

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
        "rows": p.rows,
        "G": p.G,
        "n_outputs": n_outputs,
        "per_bit_mean_hamming": per_bit_means,
        "overall_mean_hamming_bits": overall_mean_bits,
        "overall_mean_hamming_fraction": overall_mean_bits / p.w if p.w else 0.0,
    }


# (n, w, rows) -- 5 direct-comparability configs (default rows, same as the
# original experiment) + 2 new non-vacuous-tap-pruning configs (G=4).
CONFIGS = [
    (2, 8, None),
    (4, 8, None),
    (2, 4, None),
    (4, 4, None),
    (8, 4, None),
    (4, 4, 1),   # NEW: G=4, taps={2,3} non-vacuous
    (8, 4, 2),   # NEW: G=4, taps={2,3} non-vacuous
]


if __name__ == "__main__":
    for n, w, rows in CONFIGS:
        p = Params(n=n, w=w, rows=rows)
        result = run_avalanche_check(p)
        frac = result["overall_mean_hamming_fraction"]
        verdict = "OK (~50%)" if 0.3 <= frac <= 0.7 else "SUSPECT"
        print(f"n={n:2d} w={w} rows={p.rows} G={p.G}: mean Hamming = "
              f"{result['overall_mean_hamming_bits']:.2f}/{w} bits ({frac:.1%})  [{verdict}]")
