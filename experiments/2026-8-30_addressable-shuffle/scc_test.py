#!/usr/bin/env python3
# Copyright (c) 2025 Hamas A. Rahman
# Licensed under CC BY-NC-SA 4.0
# github.com/hamzy-hams

"""
scc_test.py

Adapted from experiments/2025-10-5_scramble-design/scc_test.py, run against
scrambler_addressable's output. Measures several randomness quality metrics
on integer data in 'shuffled.txt':
1. Shannon Entropy
2. Chi-Square Goodness-of-Fit (against uniform distribution)
3. Runs Test (Wald-Wolfowitz) for ascending/descending order
4. Serial Correlation Coefficient (linear correlation between consecutive elements)

Additionally (new for this file, not in the original scc_test.py): a
lightweight multi-key distinctness spot-check -- shuffling the same input
with several different keys must not collide on the resulting permutation.
This is NOT a replacement for the addressable-init research's own Tahap 3
collision-scan (500k keys, done there); it's a proportionate sanity check
that the addressable property carries over into this shuffle application.

How to run:
 python3 scc_test.py
"""

import math
import random
import sys
from collections import Counter
from pathlib import Path

HERE = Path(__file__).parent
sys.path.insert(0, str(HERE))
from scrambler_addressable import ra_shuffle  # noqa: E402


def read_tokens(filename):
    try:
        with open(filename, 'r') as f:
            content = f.read().strip()
            if not content:
                return []
            tokens = list(map(int, content.split()))
            return tokens
    except FileNotFoundError:
        print(f"Error: File '{filename}' tidak ditemukan.")
        return []
    except ValueError:
        print(f"Error: Isi file '{filename}' mengandung token yang bukan integer.")
        return []


def entropy(data):
    n = len(data)
    if n == 0:
        return 0.0
    freq = Counter(data)
    ent = 0.0
    for count in freq.values():
        p = count / n
        ent -= p * math.log2(p)
    return ent


def chi_square_uniform(data):
    n = len(data)
    if n == 0:
        return 0.0
    distinct_values = len(set(data))
    expected = n / distinct_values
    freq = Counter(data)
    chi2 = sum((count - expected) ** 2 / expected for count in freq.values())
    return chi2


def runs_test(data):
    n = len(data)
    if n < 2:
        return 0.0, 0, 0.0

    runs = 1
    for i in range(1, n):
        prev_cmp = (data[i - 1] > data[i - 2]) if i >= 2 else None
        curr_cmp = (data[i] > data[i - 1])
        if i == 1 or curr_cmp != prev_cmp:
            runs += 1

    expected_runs = (2 * n - 1) / 3
    variance_runs = (16 * n - 29) / 90
    if variance_runs <= 0:
        z = 0.0
    else:
        z = (runs - expected_runs) / math.sqrt(variance_runs)
    return z, runs, expected_runs


def serial_correlation(data):
    n = len(data)
    if n < 2:
        return 0.0

    mean = sum(data) / n
    sum_xi_xip1 = sum(data[i] * data[i + 1] for i in range(n - 1))
    sum_xi_squared = sum(x ** 2 for x in data)

    numerator = sum_xi_xip1 - (n - 1) * (mean ** 2)
    denominator = sum_xi_squared - n * (mean ** 2)

    if denominator == 0:
        return 0.0

    r = numerator / denominator
    return r


def multi_key_distinctness_check(input_tokens, n_keys=200, sample_size=2000, seed=2026_08_30):
    """Shuffle a fixed-size sample of input_tokens with n_keys different
    random keys; assert no two keys produce the identical output permutation
    (a permutation collision would defeat the point of per-key addressing)."""
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
    filename = "shuffled.txt"
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
