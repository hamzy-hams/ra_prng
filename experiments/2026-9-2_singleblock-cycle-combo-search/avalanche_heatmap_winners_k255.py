"""Avalanche heatmap (K=255, full single-key cycle) for the 11 Promotion
Tier winners of the cycle-operation combo search (see RESULTS.md). Tier 0
only captured K=1 (a single word, proven sufficient to reject the clearly
broken 756-2/3 of the DSL space cheaply -- see recipes.py/tier0_avalanche.py
docstrings) -- it does NOT rule out a K=255-style dead-bit defect like the
one quality_gate.py's avalanche_stats() docstring documents for
`pruned_winner` (seed bits 5/6 avalanche-dead across 253/255 output
positions, invisible to the scalar/K=1 average). This script re-checks
each winner at K=255 to see whether that class of defect resurfaced here.

Direct port of ../2026-8-27_operand-position-search/avalanche_heatmap_winner.py's
matrix/plot approach, retargeted at this experiment's per-candidate binaries
instead of wired_prng.py's Wiring model. Each candidate binary's `--single
<key> <rng>` mode (rng=255) captures exactly one full singleblock cycle for
a fixed key -- no multikey re-keying involved, so this is the same "one
cycle" semantics avalanche_heatmap_winner.py and avalanche_ra_core.py use,
just invoked through this experiment's own CLI surface (combo_gen.py's
TEMPLATE) instead of production ra_core's `--stream mode seed 255`.
"""

from __future__ import annotations

import os
import struct
import subprocess
import sys
from pathlib import Path

import matplotlib

if not os.environ.get("DISPLAY"):
    matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np  # noqa: E402

sys.path.insert(0, str(Path(__file__).parent.parent / "2026-8-26_operation-pruning-research"))
from quality_gate import avalanche_stats  # noqa: E402

HERE = Path(__file__).parent
CAND_DIR = HERE / "candidates"
MASK = 0xFFFFFFFF
BASE_SEED = 1
N_WORDS = 255

WINNERS = [
    "w8_f8_i0", "w8_f9_i0", "w8_f10_i0", "w8_f22_i0", "w8_f24_i0",
    "w8_f25_i0", "w8_f26_i0", "w8_f27_i0", "w8_f28_i0", "w8_f29_i0",
    "w8_f33_i0",
]


def capture_cycle(bin_path: Path, seed: int) -> list[int]:
    out = subprocess.run(
        [str(bin_path), "--single", str(seed & MASK), str(N_WORDS)],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
    ).stdout
    assert len(out) == N_WORDS * 4, f"{bin_path.name}: expected {N_WORDS*4} bytes, got {len(out)}"
    return list(struct.unpack(f"<{N_WORDS}I", out))


def compute_matrix(bin_path: Path, base_seed: int = BASE_SEED):
    capture_fn = lambda seed: capture_cycle(bin_path, seed)  # noqa: E731
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


def plot_single(cid: str, matrix, stats, out_png: Path) -> None:
    plt.figure(figsize=(10, 6))
    plt.imshow(matrix, aspect="auto", vmin=0, vmax=32)
    plt.xlabel("Output word index (0-254)")
    plt.ylabel("Flipped seed bit (0-31)")
    plt.title(
        f"Avalanche heatmap K=255 (full cycle): {cid}\n"
        f"overall={stats['overall_mean_hamming_fraction']:.4f} "
        f"min_bit={stats['min_bit_fraction']:.4f}"
    )
    plt.colorbar(label="Bit differences (0-32)")
    plt.tight_layout()
    plt.savefig(out_png, dpi=150)
    plt.close()
    print(f"  saved {out_png.name}")


def main():
    results = {}
    for cid in WINNERS:
        bin_path = CAND_DIR / cid
        assert bin_path.exists(), f"{bin_path} not found -- run combo_gen.py first"
        matrix, stats = compute_matrix(bin_path)
        results[cid] = (matrix, stats)
        worst_bit = min(range(32), key=lambda b: stats["per_bit_fractions"][b])
        print(f"{cid}: overall={stats['overall_mean_hamming_fraction']:.4f} "
              f"min_bit={stats['min_bit_fraction']:.4f} (bit {worst_bit}) "
              f"K=255 full-cycle")
        dead = [b for b in range(32) if stats["per_bit_fractions"][b] < 0.05]
        if dead:
            print(f"    !! near-dead bits (<0.05): {dead}")
        plot_single(cid, matrix, stats, HERE / f"avalanche_heatmap_{cid}_k255.png")

    n = len(WINNERS)
    ncols = 3
    nrows = -(-n // ncols)
    fig = plt.figure(figsize=(5.3 * ncols + 1.0, 3.6 * nrows))
    gs = fig.add_gridspec(nrows, ncols, left=0.06, right=0.88, top=0.90, bottom=0.06,
                           hspace=0.55, wspace=0.3)
    axes_flat = []
    im = None
    for idx, cid in enumerate(WINNERS):
        row, col = divmod(idx, ncols)
        ax = fig.add_subplot(gs[row, col])
        axes_flat.append(ax)
        matrix, stats = results[cid]
        im = ax.imshow(matrix, aspect="auto", vmin=0, vmax=32)
        ax.set_title(
            f"{cid}\noverall={stats['overall_mean_hamming_fraction']:.3f} "
            f"min_bit={stats['min_bit_fraction']:.3f}",
            fontsize=9,
        )
        ax.set_xlabel("Output word index (0-254)", fontsize=8)
        if col == 0:
            ax.set_ylabel("Flipped seed bit (0-31)", fontsize=8)
        ax.tick_params(labelsize=7)

    fig.suptitle(
        "Avalanche heatmap, K=255 (full singleblock cycle), base_seed=1 -- "
        "11 Promotion Tier winners (2026-9-2 cycle-operation combo search)",
        fontsize=12,
    )
    cbar_ax = fig.add_axes([0.91, 0.15, 0.015, 0.65])
    fig.colorbar(im, cax=cbar_ax, label="Bit differences (0-32)")
    out_png = HERE / "avalanche_heatmap_winners_k255.png"
    plt.savefig(out_png, dpi=150)
    print(f"\nSaved grid heatmap to {out_png}")


if __name__ == "__main__":
    main()
