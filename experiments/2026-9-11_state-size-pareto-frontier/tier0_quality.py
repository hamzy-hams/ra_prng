"""HANDOVER.md Tahap C (4.3, Tier 0): avalanche 3-arah + heatmap 32x(N-1)
untuk SEMUA 15 varian inti (5 N x {orbit-LOWONLY, orbit-MUTUAL} + 5 N x
singleblock). Laptop-only, murah, cuma subprocess pipe.

Reuse tanpa modifikasi:
  - quality_gate.avalanche_stats() (band [0.3,0.7], per-bit floor 0.2) --
    ../2026-8-26_operation-pruning-research/quality_gate.py.
  - Pola 3-arah key_only/addr_hi_only/combined dari
    ../2026-9-5_orbit-address-widen-64bit/avalanche_3way_addr_hi.py, generalisasi
    n_words tetap (dulu {1,255}) jadi {1, N-1} per varian.
  - compute_matrix() dari
    ../2026-9-2_singleblock-cycle-combo-search/avalanche_heatmap_winners_k255.py,
    generalisasi n=255 hardcode jadi N-1, dijalankan pada K=N-1 (full block)
    saja (K=1 heatmap trivial: cuma 1 kolom).

Target: binary battery_orbit_N<N>_<TAG> / battery_singleblock_N<N> di bin/
(kontrak --single <key> <addr_hi> <rng>, lihat driver_main_template.c).
"""

from __future__ import annotations

import json
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

HERE = Path(__file__).parent
BIN_DIR = HERE / "bin"
HEATMAP_DIR = HERE / "heatmaps"
HEATMAP_DIR.mkdir(exist_ok=True)

sys.path.insert(0, str(HERE.parent / "2026-8-26_operation-pruning-research"))
from quality_gate import avalanche_stats  # noqa: E402

LOW, HIGH, MIN_BIT_FLOOR = 0.3, 0.7, 0.2
MASK = 0xFFFFFFFF

BASE_KEY = 1
BASE_ADDR_HI = 0x9E3779B9

NS = [256, 128, 64, 32, 16]
ORBIT_MODES = ["LOWONLY", "MUTUAL"]


def variant_list():
    variants = []
    for n in NS:
        for tag in ORBIT_MODES:
            variants.append({
                "label": f"orbit_N{n}_{tag}",
                "binary": BIN_DIR / f"battery_orbit_N{n}_{tag}",
                "n": n,
                "core": "orbit",
                "reseed_mode": tag,
            })
        variants.append({
            "label": f"singleblock_N{n}",
            "binary": BIN_DIR / f"battery_singleblock_N{n}",
            "n": n,
            "core": "singleblock",
            "reseed_mode": None,
        })
    return variants


def capture(binary: Path, key: int, addr_hi: int, n_words: int) -> list[int]:
    out = subprocess.run(
        [str(binary), "--single", str(key & MASK), str(addr_hi & MASK), str(n_words)],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
    ).stdout
    assert len(out) == n_words * 4, f"{binary.name}: expected {n_words*4} bytes, got {len(out)}"
    return list(struct.unpack(f"<{n_words}I", out))


def run_direction(binary: Path, n_words: int, direction: str) -> dict:
    if direction == "key_only":
        cfn = lambda seed: capture(binary, seed, BASE_ADDR_HI, n_words)  # noqa: E731
        base_seed = BASE_KEY
    elif direction == "addr_hi_only":
        cfn = lambda seed: capture(binary, BASE_KEY, seed, n_words)  # noqa: E731
        base_seed = BASE_ADDR_HI
    elif direction == "combined":
        cfn = lambda pattern: capture(binary, BASE_KEY ^ pattern, BASE_ADDR_HI ^ pattern, n_words)  # noqa: E731
        base_seed = 0
    else:
        raise ValueError(direction)

    stats = avalanche_stats(cfn, base_seed=base_seed)
    fraction = stats["overall_mean_hamming_fraction"]
    passed = (LOW <= fraction <= HIGH) and (stats["min_bit_fraction"] >= MIN_BIT_FLOOR)
    return {"direction": direction, "passed": passed, **stats}


def compute_matrix(binary: Path, n_words: int, base_key: int = BASE_KEY,
                    base_addr_hi: int = BASE_ADDR_HI):
    cfn = lambda seed: capture(binary, seed, base_addr_hi, n_words)  # noqa: E731
    baseline = cfn(base_key)
    matrix = np.zeros((32, n_words), dtype=int)
    for bit in range(32):
        flipped = (base_key ^ (1 << bit)) & MASK
        mutated = cfn(flipped)
        for i in range(n_words):
            matrix[bit, i] = bin(baseline[i] ^ mutated[i]).count("1")
    stats = avalanche_stats(cfn, base_key)
    return matrix, stats


def plot_heatmap(label: str, n: int, matrix, stats, out_png: Path) -> None:
    plt.figure(figsize=(10, 6))
    plt.imshow(matrix, aspect="auto", vmin=0, vmax=32)
    plt.xlabel(f"Output word index (0-{n-2})")
    plt.ylabel("Flipped key bit (0-31)")
    plt.title(
        f"Avalanche heatmap K={n-1} (full block): {label}\n"
        f"overall={stats['overall_mean_hamming_fraction']:.4f} "
        f"min_bit={stats['min_bit_fraction']:.4f}"
    )
    plt.colorbar(label="Bit differences (0-32)")
    plt.tight_layout()
    plt.savefig(out_png, dpi=150)
    plt.close()


def main():
    variants = variant_list()
    results = []

    for v in variants:
        if not v["binary"].exists():
            print(f"SKIP {v['label']}: binary not found ({v['binary']})")
            continue

        n = v["n"]
        row = {"label": v["label"], "n": n, "core": v["core"],
               "reseed_mode": v["reseed_mode"], "directions": {}}

        # 3-way avalanche at K=1 and K=N-1
        variant_pass = True
        for k in (1, n - 1):
            for direction in ("key_only", "addr_hi_only", "combined"):
                r = run_direction(v["binary"], k, direction)
                variant_pass &= r["passed"]
                row["directions"][f"K{k}_{direction}"] = {
                    "passed": r["passed"],
                    "mean": r["overall_mean_hamming_fraction"],
                    "min_bit": r["min_bit_fraction"],
                }

        # heatmap at K=N-1 only (K=1 heatmap is a single column, not useful)
        matrix, hm_stats = compute_matrix(v["binary"], n - 1)
        out_png = HEATMAP_DIR / f"heatmap_{v['label']}.png"
        plot_heatmap(v["label"], n, matrix, hm_stats, out_png)
        dead_bits = [b for b in range(32) if hm_stats["per_bit_fractions"][b] < 0.05]
        row["heatmap"] = {
            "mean": hm_stats["overall_mean_hamming_fraction"],
            "min_bit": hm_stats["min_bit_fraction"],
            "dead_bits": dead_bits,
        }
        if dead_bits:
            variant_pass = False

        row["tier0_pass"] = variant_pass
        results.append(row)

        status = "PASS" if variant_pass else "FAIL"
        print(f"[{status}] {v['label']:<22} avalanche(K1,K{n-1}) 3-way + heatmap "
              f"mean={hm_stats['overall_mean_hamming_fraction']:.4f} "
              f"min_bit={hm_stats['min_bit_fraction']:.4f} "
              f"dead_bits={dead_bits}")

    with open(HERE / "tier0_results.json", "w") as f:
        json.dump(results, f, indent=2)

    print("\n=== RINGKASAN TIER 0 ===")
    survivors = [r["label"] for r in results if r["tier0_pass"]]
    failures = [r["label"] for r in results if not r["tier0_pass"]]
    print(f"Survivor ({len(survivors)}/{len(results)}): {survivors}")
    print(f"FAIL ({len(failures)}): {failures}")
    print(f"\nDetail tersimpan: {HERE / 'tier0_results.json'}")
    print(f"Heatmap PNG: {HEATMAP_DIR}/")


if __name__ == "__main__":
    main()
