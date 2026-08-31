#!/usr/bin/env python3
# Copyright (c) 2025 Hamas A. Rahman
# Licensed under CC BY-NC-SA 4.0
# github.com/hamzy-hams

"""
scc_test_wired.py

Sibling of scc_test.py (not a modification of it), run against
scrambler_wired_addressable's output (wired_shuffled.txt). Reuses
entropy()/chi_square_uniform()/runs_test()/serial_correlation()/read_tokens()
from scc_test.py UNCHANGED -- only the multi-key distinctness spot-check is
redefined here, pointed at scrambler_wired_addressable's ra_shuffle instead
of scrambler_addressable's (same pattern cross_correlation_ra_prng2.py uses:
reuse the analysis function, rewrite only the part that binds to a
different generator).

How to run:
 python3 scc_test_wired.py
"""

import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
sys.path.insert(0, str(HERE))
from scc_test import entropy, chi_square_uniform, runs_test, serial_correlation, read_tokens  # noqa: E402
from scrambler_wired_addressable import ra_shuffle  # noqa: E402


def multi_key_distinctness_check(input_tokens, n_keys=200, sample_size=2000, seed=2026_08_30):
    """Same spot-check as scc_test.py's function of the same name, bound to
    scrambler_wired_addressable's ra_shuffle instead."""
    rng = random.Random(seed)
    sample = input_tokens[:sample_size]
    keys = [rng.randrange(0, 2**32) for _ in range(n_keys)]
    seen = {}
    collisions = []
    for key in keys:
        out = tuple(ra_shuffle(key, sample[:]))
        if out in seen:
            collisions.append((seen[out], key))
        else:
            seen[out] = key
    return len(keys), len(collisions), collisions


if __name__ == "__main__":
    filename = "wired_shuffled.txt"
    data = read_tokens(filename)

    if not data:
        print("No data to test (empty or invalid file).")
        sys.exit(0)

    H = entropy(data)
    print(f"Entropy (bit): {H:.6f}")

    chi2 = chi_square_uniform(data)
    print(f"Chi-Square Statistic: {chi2:.6f}")

    z_stat, actual_runs, expected_runs = runs_test(data)
    print(f"Runs Test: z = {z_stat:.4f}, actual_runs = {actual_runs}, expected_runs ~ {expected_runs:.2f}")

    r = serial_correlation(data)
    print(f"Serial Correlation Coefficient: {r:.6f}")

    print()
    input_tokens = read_tokens("tokens.txt")
    if input_tokens:
        n_keys, n_collisions, collisions = multi_key_distinctness_check(input_tokens)
        print(f"Multi-key distinctness spot-check: {n_keys} keys, {n_collisions} permutation collisions")
        if collisions:
            print(f"  Collisions (key_a, key_b): {collisions}")
    else:
        print("tokens.txt tidak ditemukan, skip multi-key distinctness spot-check.")
