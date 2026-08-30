"""Avalanche-effect heatmap for `pruned_winner.c`'s exact operation set
(TAP6, TAP7, ROT_C, SHR13; hash_access=sequential).

This is a fixed, retargeted port of
experiments/others/avalanche_effect_analysis.py (which has since also
been fixed in place, 2026-08-27 -- it now targets the original algorithm
correctly and has its own --cycle/--show support; this file remains
separate because it targets pruned_winner's much smaller 4-op set
instead). That script originally had three bugs, all fixed here too:

  1. It targeted the full ALL_OPS algorithm (8-tap `o` loop, SHL9/SHR18/
     SHL14/MULT_REDUCE, etc.), not pruned_winner's much smaller 4-op set.
     `pruned_winner_capture()` below implements pruned_winner.c's exact
     `ra_permutation_cycle` formula instead:
         o = (M[(i+6)&0xFF]<<6) ^ (M[(i+7)&0xFF]<<7)
         a = (b^o) ^ (cons+a)
         b = (cons+a) ^ (o+d)
         c = rot32((b>>13)^a, b)
         d = c & 0xFF
     (Verified by hand-tracing pruned_prng.py's generic `permutation_cycle`
     with cand.ops == {TAP6,TAP7,ROT_C,SHR13}: every term gated by an
     inactive flag -- ROT_A, ROT_B, SHL9, SHR18, SHL14, MULT_REDUCE --
     reduces to the identity/zero, collapsing exactly to the formula
     above.)
  2. Seed-flip bug: the original wrote `seed = 0 ^ (1 << bit + 1)`, which
     due to operator precedence (`+` binds tighter than `<<`) evaluates to
     `1 << (bit+1)` -- never actually XORs a flipped bit against the
     baseline seed. Fixed to `base_seed ^ (1 << bit)`, matching
     quality_gate.py's `avalanche_gate()`.
  3. `c`/`d` conflation: the original reused a single `c` variable for
     both the raw per-step output and the derived rotation-amount `d`,
     silently corrupting the `o + c<<14` term on the next iteration. Not
     applicable to pruned_winner's formula (SHL14 is inactive, so that
     term is always 0 regardless), but the root bug is documented here
     since it's what made the ancestor script wrong for the full
     algorithm.

The L[i]/L[d] swap is included for structural correctness even though it
has no effect on the captured `c` sequence for a single reseed-free cycle
(`o` depends only on `M`, never `L`) -- it DOES matter once `--cycle`
crosses a reseed boundary (see below), since `ra_reseed` folds `L` into
`M`.

By default (`--cycle 1`) this reproduces the original script's behavior:
one 255-step cycle, no reseed, cross-checked against
quality_gate.py's `avalanche_gate()` (the correct, already-validated
successor of the old script). `--cycle N` (N > 1) instead runs N-1
reseeds first (via `pruned_winner_reseed()`, `pruned_winner.c`'s exact
`ra_reseed`/`ra_hash`) and compares cycle 1's heatmap against cycle N's,
side by side -- does the avalanche effect from a single seed-bit flip
survive N-1 rounds of reseeding, strengthen, or wash out? Every cycle
analyzed is independently cross-checked against pruned_prng.py's
already-validated `stream()` generator (same Candidate) before being
trusted; both must agree exactly.
"""

from __future__ import annotations

import argparse
import os
from pathlib import Path

import matplotlib

# Force the non-GUI Agg backend only when there's no display to draw a
# window on (e.g. this script run automatically, no DISPLAY set). When a
# display is available -- as in an interactive terminal -- let matplotlib
# pick its normal GUI backend so `--show` can pop up a window, matching
# experiments/others/avalanche_effect_analysis.py's original plt.show()
# behavior.
if not os.environ.get("DISPLAY"):
    matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np  # noqa: E402

from pruned_prng import Candidate, HASH_ACCESS_SEQUENTIAL, stream  # noqa: E402
from quality_gate import avalanche_gate  # noqa: E402

MASK = 0xFFFFFFFF
BASE_SEED = 1
CAND = Candidate(
    ops=frozenset({"TAP6", "TAP7", "ROT_C", "SHR13"}),
    hash_access=HASH_ACCESS_SEQUENTIAL,
)


def rot32(n: int, r: int) -> int:
    r &= 31
    n &= MASK
    return ((n << r) | (n >> (32 - r))) & MASK


def pruned_winner_reseed(M: list[int], L: list[int]) -> int:
    """Mirrors ra_reseed()/ra_hash() in pruned_winner.c exactly
    (HASH_ACCESS_SEQUENTIAL, HASH_DEPTH=32, no HASH_SELFIDX)."""
    for i in range(256):
        M[i] ^= L[i]
    new_cons = 0
    for e in range(8):
        base = e * 32
        h = 0
        for j in range(32):
            h ^= M[base + j]
        new_cons ^= (h << e) & MASK
    return new_cons & MASK


def pruned_winner_capture(seed: int, cycle: int = 1) -> list[int]:
    """Cycle `cycle`'s (1-indexed) 255-step permutation-cycle output.
    cycle=1 is the first cycle, no prior reseed (matches the original
    script's single-cycle behavior); cycle=N runs N-1 reseeds first.
    Mirrors ra_core's outer loop + ra_permutation_cycle/ra_reseed in
    pruned_winner.c."""
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
        a, b, d = cons, it & MASK, 0
        cycle_outputs = []
        for i in range(255, 0, -1):
            o = ((M[(i + 6) & 0xFF] << 6) ^ (M[(i + 7) & 0xFF] << 7)) & MASK
            a = ((b ^ o) ^ ((cons + a) & MASK)) & MASK
            b = (((cons + a) & MASK) ^ ((o + d) & MASK)) & MASK
            c = rot32((b >> 13) ^ a, b)
            cycle_outputs.append(c)
            d = c & 0xFF
            L[i], L[d] = L[d], L[i]
        if it == cycle - 1:
            target_outputs = cycle_outputs
        cons = pruned_winner_reseed(M, L)
    return target_outputs


def stream_capture(seed: int, cycle: int) -> list[int]:
    """Same target cycle as pruned_winner_capture(), but derived from
    pruned_prng.py's already-validated, generic `stream()` generator --
    an independent implementation used purely as a cross-check."""
    all_values = list(stream(seed, cycle, CAND))
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


def analyze_cycle(cycle: int):
    """Compute the avalanche matrix/fraction for one cycle, cross-checked
    against pruned_prng.py's stream() -- raises if they disagree."""
    hand_matrix, hand_fraction = compute_avalanche_matrix(
        lambda seed: pruned_winner_capture(seed, cycle)
    )
    _, stream_fraction = compute_avalanche_matrix(
        lambda seed: stream_capture(seed, cycle)
    )
    if hand_fraction != stream_fraction:
        raise SystemExit(
            f"Cross-check FAILED at cycle {cycle}: hand-rolled capture vs. "
            f"pruned_prng.py's stream() disagree -- "
            f"{hand_fraction!r} != {stream_fraction!r}. A bug remains."
        )
    return hand_matrix, hand_fraction


def plot_single(matrix, fraction: int, out_png: Path) -> None:
    plt.figure(figsize=(10, 6))
    plt.imshow(matrix, aspect="auto", vmin=0, vmax=32)
    plt.xlabel("Output Index (0-254)")
    plt.ylabel("Flipped Seed Bit (0-31)")
    plt.title("Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)")
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
    axes[0].set_ylabel("Flipped Seed Bit (0-31)")
    fig.colorbar(im, ax=axes, label="Bit Differences (0-32)", shrink=0.85)
    fig.suptitle(
        "Avalanche Effect: pruned_winner (TAP6, TAP7, ROT_C, SHR13) "
        f"-- cycle 1 vs. cycle {cycle}"
    )
    plt.savefig(out_png, dpi=150)
    print(f"Saved comparison heatmap to {out_png}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--cycle", type=int, default=1, metavar="N",
        help="Compare cycle 1 (initial state) against cycle N (N-1 "
             "reseeds first). Default 1 = original single-cycle "
             "behavior, no comparison panel.",
    )
    parser.add_argument(
        "--show", action="store_true",
        help="Also pop up an interactive window (needs a display), "
             "matching the original avalanche_effect_analysis.py's "
             "plt.show(). Off by default so automated/headless runs "
             "never block waiting for a window to be closed.",
    )
    args = parser.parse_args()
    if args.cycle < 1:
        parser.error("--cycle must be >= 1")

    if args.cycle == 1:
        matrix, fraction = analyze_cycle(1)

        gate_result = avalanche_gate(CAND, base_seed=BASE_SEED)
        gate_fraction = gate_result["overall_mean_hamming_fraction"]
        print(f"Heatmap-derived avalanche fraction: {fraction:.6f}")
        print(f"avalanche_gate() avalanche fraction: {gate_fraction:.6f}")
        if fraction != gate_fraction:
            raise SystemExit(
                "Cross-check FAILED: heatmap and avalanche_gate() disagree "
                f"-- {fraction!r} != {gate_fraction!r}. A bug remains."
            )
        print("Cross-check MATCH: heatmap and avalanche_gate() agree exactly.")

        out_png = Path(__file__).parent / "avalanche_heatmap_pruned_winner.png"
        plot_single(matrix, fraction, out_png)
    else:
        matrix1, fraction1 = analyze_cycle(1)
        matrixN, fractionN = analyze_cycle(args.cycle)

        # Cycle 1 also gets the avalanche_gate() cross-check, since that
        # function only ever evaluates cycle 1.
        gate_result = avalanche_gate(CAND, base_seed=BASE_SEED)
        gate_fraction = gate_result["overall_mean_hamming_fraction"]
        if fraction1 != gate_fraction:
            raise SystemExit(
                "Cross-check FAILED: cycle-1 heatmap and avalanche_gate() "
                f"disagree -- {fraction1!r} != {gate_fraction!r}."
            )

        print(f"Cycle 1 (initial state) avalanche fraction: {fraction1:.6f}")
        print(f"Cycle {args.cycle} avalanche fraction:            {fractionN:.6f}")
        print(f"Delta (cycle {args.cycle} - cycle 1):             {fractionN - fraction1:+.6f}")
        print("Cross-check MATCH: both cycles agree exactly with independent "
              "stream()-based computation (and cycle 1 with avalanche_gate()).")

        out_png = (
            Path(__file__).parent
            / f"avalanche_heatmap_pruned_winner_cycle1_vs_cycle{args.cycle}.png"
        )
        plot_comparison(matrix1, fraction1, matrixN, fractionN, args.cycle, out_png)

    if args.show:
        plt.show()


if __name__ == "__main__":
    main()
