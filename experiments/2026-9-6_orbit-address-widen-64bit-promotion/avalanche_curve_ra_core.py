"""Avalanche-effect curve for `ra_core.c`, replicating the CSAI2025/ICCS2026
paper's exact methodology (Section III.A.2, "Avalanche-Effect Analysis with
Multi-Seed Perturbation Models") against the production `ra_core` binary,
instead of the toy Python model the paper's original script
(experiments/others/avalanche_effect_analysis.py) targets.

Base seed 0x00000001, K=255 (first block/cycle), 3 perturbation schemes
(bit-flip, incremental, power-of-two) x 32 seeds each, curve = mean Hamming
distance per output position (0..254) across the 32 perturbed seeds.

This is descriptive documentation for release prep, not a pass/fail gate --
`avalanche_ra_core.py` (bit-flip only, scalar) already gates that scheme as
part of the closed production-battery. Incremental and power-of-two have
never been run against ra_core.c before; an out-of-band result here is a new
finding to report, not something this script fixes.
"""

from __future__ import annotations

import sys
import csv
from pathlib import Path

import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = Path(__file__).parent
PROD_DIR = HERE.parent / "2026-9-1_family-productionization"
sys.path.insert(0, str(PROD_DIR))
from avalanche_ra_core import capture_cycle  # noqa: E402  (reused: CLI invoke + parsing)

BASE_SEED = 0x00000001
K = 255
MASK = 0xFFFFFFFF
LOW, HIGH = 0.3, 0.7  # informational band from quality_gate.py, not asserted

SCHEMES = {
    "bitflip": lambda i: BASE_SEED ^ (1 << i),
    "incremental": lambda i: (BASE_SEED + (i + 1)) & MASK,
    "powoftwo": lambda i: (1 << (i + 1)) & MASK,
}


def compute_curve(mode: str, baseline: list[int], scheme_fn) -> np.ndarray:
    matrix = np.zeros((32, K), dtype=int)
    for i in range(32):
        mutated = capture_cycle(mode, scheme_fn(i))
        matrix[i] = [bin(a ^ b).count("1") for a, b in zip(baseline, mutated)]
    return matrix.mean(axis=0)  # shape (K,), values in [0, 32] bits


def save_csv(path: Path, curve: np.ndarray) -> None:
    with open(path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["position", "mean_hamming_bits", "mean_hamming_fraction"])
        for pos, bits in enumerate(curve):
            w.writerow([pos, f"{bits:.6f}", f"{bits / 32:.6f}"])


def plot_core(mode: str, curves: dict[str, np.ndarray], out_png: Path) -> None:
    fig, axes = plt.subplots(1, 3, figsize=(18, 5), sharey=True)
    for ax, (name, curve) in zip(axes, curves.items()):
        ax.plot(range(K), curve, linewidth=0.8)
        ax.axhline(32 * LOW, color="grey", linestyle="--", linewidth=0.6)
        ax.axhline(32 * HIGH, color="grey", linestyle="--", linewidth=0.6)
        ax.set_title(name)
        ax.set_xlabel("Output index (0-254)")
        ax.set_ylim(0, 32)
    axes[0].set_ylabel("Mean Hamming distance (bits, 0-32)")
    fig.suptitle(
        f"Avalanche effect curve: ra_core_{mode} "
        f"(base seed=1, K=255, 32 perturbed seeds/scheme)"
    )
    fig.tight_layout()
    fig.savefig(out_png, dpi=150)
    plt.close(fig)


def main():
    summary = []
    for mode in ("orbit", "singleblock"):
        baseline = capture_cycle(mode, BASE_SEED)
        curves = {}
        for name, fn in SCHEMES.items():
            curve = compute_curve(mode, baseline, fn)
            curves[name] = curve
            save_csv(HERE / f"avalanche_curve_{mode}_{name}.csv", curve)
            frac = curve.mean() / 32.0
            in_band = LOW <= frac <= HIGH
            summary.append((mode, name, frac, in_band))
            print(
                f"{mode}/{name}: overall_mean_hamming_fraction={frac:.6f} "
                f"(band [{LOW},{HIGH}]: {'OK' if in_band else 'OUT OF BAND'})"
            )
        plot_core(mode, curves, HERE / f"avalanche_curve_{mode}.png")

    print("\n=== summary ===")
    for mode, name, frac, in_band in summary:
        print(f"{mode:12s} {name:12s} fraction={frac:.6f} {'OK' if in_band else 'OUT OF BAND'}")


if __name__ == "__main__":
    main()
