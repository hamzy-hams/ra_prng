"""Avalanche-effect heatmap for the original algorithm (ALL_OPS,
HASH_ACCESS_STRIDED + HASH_SELFIDX) -- src/ra_prng2/c/ra_prng2.c's exact
`ra_permutation_cycle`/`ra_reseed`.

Fixed in place (2026-08-27). The previous version of this script had
three bugs:

  1. Seed-flip bug: `seed = 0 ^ (1 << bit + 1)`. Due to operator
     precedence (`+` binds tighter than `<<`) this evaluated to
     `1 << (bit+1)` -- never actually XORed a flipped bit against the
     baseline seed (and silently overflowed to 33 bits for bit=31).
     Fixed to `base_seed ^ (1 << bit)`.
  2. `c`/`d` conflation: the old capture function reused a single `c`
     variable for both the raw per-step output and the derived
     rotation-amount `d`, silently corrupting the `(o + c) << 14` term
     on the next iteration (that term needs the previous iteration's raw
     `c`, not `d`). Fixed by keeping `c` and `d` as separate variables,
     matching src/ra_prng2/c/ra_prng2.c exactly.
  3. No `L[i]/L[d]` swap. Harmless for a single reseed-free cycle
     (`o` depends only on `M`, never `L`) but wrong once a reseed
     happens (`ra_reseed` folds `L` into `M`) -- matters now that
     `--cycle N` (N>1) exists. Fixed by adding the swap.

Also added: `--cycle N` (compare cycle 1 against cycle N, N-1 reseeds
first), `--show` (optional interactive window, off by default so
automated/headless runs never block), headless-safe Agg backend when no
`DISPLAY` is set, and `plt.savefig(...)` so a result is always produced
even without a display.

Every run is cross-checked twice before any result is trusted:
  (a) against pruned_prng.py's already-validated, generic stream()
      engine with the default Candidate() (= ALL_FLAGS, i.e. exactly
      this operation set) -- avalanche fractions must match exactly.
  (b) against src/ra_prng2/python/ra_prng2.py's paper-exact ra_core() --
      final `cons` after N cycles must match exactly, the same
      cross-check pruned_prng.py._self_check() performs.
Both cross-folder imports mirror an already-established pattern in this
repo (pruned_prng.py's own _self_check() reaches into
src/ra_prng2/python the same way). This is also why cycle=1's avalanche
fraction is expected to be exactly 0.497576 -- the same algorithm and
seed as experiments/2026-8-27_operand-position-search/other/
avalanche_heatmap_original.py, which independently arrives at the same
number.
"""

from __future__ import annotations

import argparse
import os
import sys
from pathlib import Path

import matplotlib

if not os.environ.get("DISPLAY"):
    matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np  # noqa: E402

_HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(_HERE / ".." / "2026-8-26_operation-pruning-research"))
sys.path.insert(0, str(_HERE / ".." / ".." / "src" / "ra_prng2" / "python"))

from pruned_prng import Candidate, final_cons, stream  # noqa: E402
from ra_prng2 import ra_core as reference_ra_core  # noqa: E402

MASK = 0xFFFFFFFF
BASE_SEED = 1
ALL_OPS = Candidate()  # default ops=ALL_FLAGS, hash_access=HASH_ACCESS_DEFAULT (STRIDED)


def rot32(n: int, r: int) -> int:
    r &= 31
    n &= MASK
    return ((n << r) | (n >> (32 - r))) & MASK


def original_reseed(M: list[int], L: list[int]) -> int:
    """Mirrors ra_reseed()/ra_hash() in src/ra_prng2/c/ra_prng2.c exactly
    (HASH_SELFIDX + HASH_ACCESS_STRIDED, HASH_DEPTH=32)."""
    for i in range(256):
        M[i] ^= L[i]
    out8 = [0] * 8
    for i in range(8):
        out8[i] ^= M[M[i] & 0xFF]
        for j in range(32):
            out8[i] ^= M[j * 8 + i]
    new_cons = 0
    for e in range(8):
        new_cons ^= (out8[e] << e) & MASK
    return new_cons & MASK


def zepfold_capture(seed: int, cycle: int = 1) -> list[int]:
    """Cycle `cycle`'s (1-indexed) 255-step permutation-cycle output.
    cycle=1 is the first cycle, no prior reseed; cycle=N runs N-1
    reseeds first. Mirrors ra_core's outer loop + ra_permutation_cycle/
    ra_reseed in src/ra_prng2/c/ra_prng2.c."""
    if cycle < 1:
        raise ValueError("cycle must be >= 1")

    M = [0] * 256
    L = [0] * 256
    for i in range(256):
        M[i] = (i * 0x06A0DD9B + 0x06A0DD9B) & MASK
        L[i] = (i * 0x9E3779B7 + 0x9E3779B7) & MASK

    cons = seed
    target_outputs = None
    for it in range(cycle):
        a, b, c, d = cons, it & MASK, 0, 0
        cycle_outputs = []
        for i in range(255, 0, -1):
            o = 0
            for e in range(8):
                o ^= (M[(i + e) & 0xFF] << e) & MASK

            a = (rot32(b ^ o, d) ^ ((cons + a) & MASK)) & MASK
            b = (rot32((cons + a) & MASK, i) ^ ((o + d) & MASK)) & MASK
            o = ((rot32(a ^ o, i) << 9) ^ (b >> 18)) & MASK
            c = rot32((((o + c) & MASK) << 14) ^ (b >> 13) ^ a, b)
            cycle_outputs.append(c)
            d = (c * (i + 1)) >> 32
            L[i], L[d] = L[d], L[i]
        if it == cycle - 1:
            target_outputs = cycle_outputs
        cons = original_reseed(M, L)
    return target_outputs


def stream_capture(seed: int, cycle: int) -> list[int]:
    """Independent cross-check capture via pruned_prng.py's generic,
    already-validated stream() engine with the default (ALL_FLAGS)
    Candidate."""
    all_values = list(stream(seed, cycle, ALL_OPS))
    return all_values[(cycle - 1) * 255:cycle * 255]


def compute_avalanche_matrix(capture_fn, base_seed: int = BASE_SEED):
    baseline = capture_fn(base_seed)
    n = len(baseline)
    matrix = np.zeros((32, n), dtype=int)
    for bit in range(32):
        flipped_seed = (base_seed ^ (1 << bit)) & MASK
        mutated = capture_fn(flipped_seed)
        for i in range(n):
            matrix[bit, i] = bin(baseline[i] ^ mutated[i]).count("1")
    return matrix, matrix.mean() / 32.0


def cross_check_final_cons(cycle: int) -> None:
    """Cross-check (b): final cons after `cycle` iterations must match
    src/ra_prng2/python/ra_prng2.py's paper-exact reference, for the
    baseline seed and every one of the 32 bit-flipped seeds actually
    exercised by the heatmap."""
    seeds = [BASE_SEED] + [(BASE_SEED ^ (1 << bit)) & MASK for bit in range(32)]
    for seed in seeds:
        expected = reference_ra_core(seed, cycle)
        got = final_cons(seed, cycle, ALL_OPS)
        if expected != got:
            raise SystemExit(
                f"Cross-check (b) FAILED at seed={seed}, cycle={cycle}: "
                f"reference ra_core()={expected} != pruned_prng.py final_cons()={got}. "
                "A bug remains -- not reporting any result."
            )


def analyze_cycle(cycle: int):
    """Compute the avalanche matrix/fraction for one cycle, cross-checked
    twice (against stream(), and against the paper-exact reference) --
    raises if either disagrees."""
    hand_matrix, hand_fraction = compute_avalanche_matrix(
        lambda seed: zepfold_capture(seed, cycle)
    )
    _, stream_fraction = compute_avalanche_matrix(
        lambda seed: stream_capture(seed, cycle)
    )
    if hand_fraction != stream_fraction:
        raise SystemExit(
            f"Cross-check (a) FAILED at cycle {cycle}: hand-rolled capture vs. "
            f"pruned_prng.py's stream() disagree -- "
            f"{hand_fraction!r} != {stream_fraction!r}. A bug remains."
        )
    cross_check_final_cons(cycle)
    return hand_matrix, hand_fraction


def plot_single(matrix, fraction: float, out_png: Path) -> None:
    plt.figure(figsize=(10, 6))
    plt.imshow(matrix, aspect="auto", vmin=0, vmax=32)
    plt.xlabel("Output Index (0-254)")
    plt.ylabel("Flipped Constant Bit (0-31)")
    plt.title("Avalanche Effect Heatmap: Hamming Distances (original algorithm)")
    plt.colorbar(label="Bit Differences (0-32)")
    plt.tight_layout()
    plt.savefig(out_png, dpi=150)
    print(f"Saved heatmap to {out_png}")


def plot_comparison(matrix1, fraction1, matrixN, fractionN, cycle: int, out_png: Path) -> None:
    fig, axes = plt.subplots(1, 2, figsize=(18, 6), sharey=True)
    for ax, matrix, title in (
        (axes[0], matrix1, f"Cycle 1 (initial state)\navalanche fraction = {fraction1:.6f}"),
        (axes[1], matrixN, f"Cycle {cycle}\navalanche fraction = {fractionN:.6f}"),
    ):
        im = ax.imshow(matrix, aspect="auto", vmin=0, vmax=32)
        ax.set_xlabel("Output Index (0-254)")
        ax.set_title(title)
    axes[0].set_ylabel("Flipped Constant Bit (0-31)")
    fig.colorbar(im, ax=axes, label="Bit Differences (0-32)", shrink=0.85)
    fig.suptitle(f"Avalanche Effect: original algorithm (ALL_OPS) -- cycle 1 vs. cycle {cycle}")
    plt.savefig(out_png, dpi=150)
    print(f"Saved comparison heatmap to {out_png}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--cycle", type=int, default=1, metavar="N",
        help="Compare cycle 1 (initial state) against cycle N (N-1 "
             "reseeds first). Default 1 = single heatmap, no comparison.",
    )
    parser.add_argument(
        "--show", action="store_true",
        help="Also pop up an interactive window (needs a display). Off "
             "by default so automated/headless runs never block.",
    )
    args = parser.parse_args()
    if args.cycle < 1:
        parser.error("--cycle must be >= 1")

    if args.cycle == 1:
        matrix, fraction = analyze_cycle(1)
        print(f"Avalanche fraction (original algorithm, cycle 1): {fraction:.6f}")
        print("Cross-checks MATCH: stream() and reference ra_core() both agree exactly.")
        out_png = _HERE / "avalanche_effect_heatmap.png"
        plot_single(matrix, fraction, out_png)
    else:
        matrix1, fraction1 = analyze_cycle(1)
        matrixN, fractionN = analyze_cycle(args.cycle)
        print(f"Cycle 1 (initial state) avalanche fraction: {fraction1:.6f}")
        print(f"Cycle {args.cycle} avalanche fraction:            {fractionN:.6f}")
        print(f"Delta (cycle {args.cycle} - cycle 1):             {fractionN - fraction1:+.6f}")
        print("Cross-checks MATCH: stream() and reference ra_core() both agree exactly, both cycles.")
        out_png = _HERE / f"avalanche_effect_heatmap_cycle1_vs_cycle{args.cycle}.png"
        plot_comparison(matrix1, fraction1, matrixN, fractionN, args.cycle, out_png)

    if args.show:
        plt.show()


if __name__ == "__main__":
    main()
