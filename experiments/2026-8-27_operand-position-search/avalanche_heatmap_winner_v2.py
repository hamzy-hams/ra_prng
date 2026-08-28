"""Avalanche-effect heatmap for `winner_wired_v2.c` (wiring "v08" in
RESULTS.md's 13-winner survey), the candidate that superseded
`winner_wired.c` on 2026-08-28: Wiring(a_xor_operand="d",
c_shift_operand="a", rotc_amount_source="b", rotc_xor_operand="a").

Direct port of avalanche_heatmap_winner.py (which targets `winner_wired.c`
/ v05), retargeted at this wiring so the two are visually and numerically
comparable (same axes, same cross-check structure). See RESULTS.md's
"Survey of the other 12 winners" section for the full 13-way table this
heatmap is one entry of.
"""

from __future__ import annotations

import argparse
import os
from pathlib import Path

import matplotlib

if not os.environ.get("DISPLAY"):
    matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np  # noqa: E402

from wiring import Wiring  # noqa: E402
from wired_prng import stream  # noqa: E402

import sys  # noqa: E402
sys.path.insert(0, str(Path(__file__).parent.parent /
                       "2026-8-26_operation-pruning-research"))
from quality_gate import avalanche_stats  # noqa: E402

MASK = 0xFFFFFFFF
BASE_SEED = 1
WINNER = Wiring(a_xor_operand="d", c_shift_operand="a",
                 rotc_amount_source="b", rotc_xor_operand="a")


def rot32(n: int, r: int) -> int:
    r &= 31
    n &= MASK
    return ((n << r) | (n >> (32 - r))) & MASK


def winner_reseed(M: list[int], L: list[int]) -> int:
    """Mirrors winner_wired_v2.c's ra_reseed()/ra_hash() exactly (identical
    to baseline.c's -- rewiring only touches ra_permutation_cycle's a/b/c
    chain, never reseed)."""
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


def winner_capture(seed: int, cycle: int = 1) -> list[int]:
    """Cycle `cycle`'s (1-indexed) 255-step permutation-cycle output,
    hand-rolled mirror of winner_wired_v2.c's ra_core loop (used purely as
    an independent cross-check against wired_prng.py's stream())."""
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
            a = ((d ^ o) ^ ((cons + a) & MASK)) & MASK
            b = (((cons + a) & MASK) ^ ((o + d) & MASK)) & MASK
            c = rot32((a >> 13) ^ a, b)
            cycle_outputs.append(c)
            d = c & 0xFF
            L[i], L[d] = L[d], L[i]
        if it == cycle - 1:
            target_outputs = cycle_outputs
        cons = winner_reseed(M, L)
    return target_outputs


def stream_capture(seed: int, cycle: int) -> list[int]:
    all_values = list(stream(seed, cycle, WINNER))
    return all_values[(cycle - 1) * 255:cycle * 255]


def analyze_cycle(cycle: int):
    hand_matrix, hand_stats = compute_avalanche_matrix(
        lambda seed: winner_capture(seed, cycle)
    )
    _, stream_stats = compute_avalanche_matrix(
        lambda seed: stream_capture(seed, cycle)
    )
    if hand_stats["overall_mean_hamming_fraction"] != stream_stats["overall_mean_hamming_fraction"]:
        raise SystemExit(
            f"Cross-check FAILED at cycle {cycle}: hand-rolled capture vs. "
            f"wired_prng.py's stream() disagree -- "
            f"{hand_stats!r} != {stream_stats!r}. A bug remains."
        )
    return hand_matrix, hand_stats


def compute_avalanche_matrix(capture_fn, base_seed: int = BASE_SEED):
    baseline = capture_fn(base_seed)
    n = len(baseline)
    matrix = np.zeros((32, n), dtype=int)
    for bit in range(32):
        flipped_seed = (base_seed ^ (1 << bit)) & MASK
        mutated = capture_fn(flipped_seed)
        for i in range(n):
            matrix[bit, i] = bin(baseline[i] ^ mutated[i]).count("1")
    stats = avalanche_stats(capture_fn, base_seed)
    return matrix, stats


def plot_single(matrix, stats, out_png: Path) -> None:
    plt.figure(figsize=(10, 6))
    plt.imshow(matrix, aspect="auto", vmin=0, vmax=32)
    plt.xlabel("Output Index (0-254)")
    plt.ylabel("Flipped Seed Bit (0-31)")
    plt.title(
        "Avalanche Effect Heatmap: winner_wired_v2 (a_xor=d, c_shift=a, "
        f"rotc_amount=b, rotc_xor=a)\noverall={stats['overall_mean_hamming_fraction']:.6f} "
        f"min_bit={stats['min_bit_fraction']:.6f}"
    )
    plt.colorbar(label="Bit Differences (0-32)")
    plt.tight_layout()
    plt.savefig(out_png, dpi=150)
    print(f"Saved heatmap to {out_png}")


def plot_comparison(matrix1, stats1, matrixN, statsN, cycle: int, out_png: Path) -> None:
    fig, axes = plt.subplots(1, 2, figsize=(18, 6), sharey=True)
    for ax, matrix, title in (
        (axes[0], matrix1, f"Cycle 1 (initial state)\navalanche fraction = {stats1['overall_mean_hamming_fraction']:.6f}"),
        (axes[1], matrixN, f"Cycle {cycle}\navalanche fraction = {statsN['overall_mean_hamming_fraction']:.6f}"),
    ):
        im = ax.imshow(matrix, aspect="auto", vmin=0, vmax=32)
        ax.set_xlabel("Output Index (0-254)")
        ax.set_title(title)
    axes[0].set_ylabel("Flipped Seed Bit (0-31)")
    fig.colorbar(im, ax=axes, label="Bit Differences (0-32)", shrink=0.85)
    fig.suptitle(
        "Avalanche Effect: winner_wired_v2 (a_xor=d, c_shift=a, rotc_amount=b, "
        f"rotc_xor=a) -- cycle 1 vs. cycle {cycle}"
    )
    plt.savefig(out_png, dpi=150)
    print(f"Saved comparison heatmap to {out_png}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cycle", type=int, default=1, metavar="N",
                         help="Compare cycle 1 against cycle N (N-1 reseeds first).")
    parser.add_argument("--show", action="store_true")
    args = parser.parse_args()
    if args.cycle < 1:
        parser.error("--cycle must be >= 1")

    if args.cycle == 1:
        matrix, stats = analyze_cycle(1)
        print(f"Overall avalanche fraction: {stats['overall_mean_hamming_fraction']:.6f}")
        print(f"Min per-bit fraction:       {stats['min_bit_fraction']:.6f}")
        for bit in range(32):
            print(f"  bit {bit:2d}: {stats['per_bit_fractions'][bit]:.4f}")
        out_png = Path(__file__).parent / "avalanche_heatmap_winner_v2.png"
        plot_single(matrix, stats, out_png)
    else:
        matrix1, stats1 = analyze_cycle(1)
        matrixN, statsN = analyze_cycle(args.cycle)
        print(f"Cycle 1 overall: {stats1['overall_mean_hamming_fraction']:.6f} (min_bit={stats1['min_bit_fraction']:.6f})")
        print(f"Cycle {args.cycle} overall: {statsN['overall_mean_hamming_fraction']:.6f} (min_bit={statsN['min_bit_fraction']:.6f})")
        out_png = Path(__file__).parent / f"avalanche_heatmap_winner_v2_cycle1_vs_cycle{args.cycle}.png"
        plot_comparison(matrix1, stats1, matrixN, statsN, args.cycle, out_png)

    if args.show:
        plt.show()


if __name__ == "__main__":
    main()
