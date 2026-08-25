#!/usr/bin/env python3
# ra_prng2.py
# Copyright (c) 2025 Hamas A. Rahman
# Licensed under CC BY-NC-SA 4.0
# github.com/hamzy-hams
# version 0.1.0

import time

def rot32(n: int, r: int) -> int:
    r &= 31
    n &= 0xFFFFFFFF
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFF


def ra_hash(N: list) -> list:
    """Hash M[256] -> out8[8]: nonlinear XOR reduction."""
    out = [0] * 8
    for i in range(8):
        idx = N[i] & 0xFF
        out[i] ^= N[idx]
        for j in range(32):
            out[i] ^= N[j * 8 + i]
    return out


def ra_core(seed: int, iterations: int) -> int:
    """Run 'iterations' outer iterations, return last cons."""
    if iterations == 0:
        return seed

    L = [0] * 256
    M = [0] * 256
    cons = seed

    # Array state initialization
    for i in range(256):
        M[i] = (i * 0x06a0dd9b + 0x06a0dd9b) & 0xFFFFFFFF
        L[i] = (i * 0x9e3779b7 + 0x9e3779b7) & 0xFFFFFFFF

    for it in range(iterations):
        a = cons
        b = it
        c = 0
        d = 0

        # Permutation step: inner loop i from 255 down to 1
        for i in range(255, 0, -1):
            # Aggregate mixer o from M
            o = 0
            for e in range(8):
                o ^= (M[(i + e) & 0xFF] << e) & 0xFFFFFFFF

            # Core transforms
            a = (rot32(b ^ o, d) ^ (cons + a)) & 0xFFFFFFFF
            b = (rot32(cons + a, i) ^ (o + d)) & 0xFFFFFFFF
            o = (rot32(a ^ o, i) << 9 ^ (b >> 18)) & 0xFFFFFFFF
            c = rot32((o + (c << 14)) ^ (b >> 13) ^ a, b) & 0xFFFFFFFF
            
            # Lemire's fast reduction: random index d in [0, i]
            d = (c * (i + 1)) >> 32

            # Swap L[i] <-> L[d]
            L[i], L[d] = L[d], L[i]
            

        # Mix M ^= L (entropy diffusion)
        for i in range(256):
            M[i] ^= L[i]

        # Hash M to produce next cons
        tmp8 = ra_hash(M)

        new_cons = 0
        for e in range(8):
            new_cons ^= (tmp8[e] << e) & 0xFFFFFFFF
        cons = new_cons
    return cons

def main():
    start = time.time()
    last_cons = ra_core(seed=1, iterations=1999)
    elapsed = time.time() - start
    print(f"Generated 1999 outer iterations in {elapsed:.3f} seconds")
    print(f"Last cons: {last_cons}")


if __name__ == '__main__':
    main()