#!/usr/bin/env python3
# Copyright (c) 2025 Hamas A. Rahman
# Licensed under CC BY-NC-SA 4.0
# github.com/hamzy-hams

import sys
import time

def rot32(n: int, r: int) -> int:
    r &= 31
    n &= 0xFFFFFFFF
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFF

def ra_hash(N):
    # Pure function: N is read-only, never mutated. Matches Algorithm 2 in
    # the CSAI2025/ICCS2026 papers (tmp8 <- ra_hash(M) is a pure map into a
    # separate 8-element output) and src/ra_prng2/c/ra_prng2_struct.c.
    out = [0] * 8
    for i in range(8):
        out[i] ^= N[N[i] & 0xFF]
        for j in range(32):
            out[i] ^= N[j * 8 + i]
    return out

def ra_core(seed, iteration, verbose: bool = False):
    L = [0] * 256
    M = [0] * 256
    cons = seed
    if verbose:
        print(cons)

    for i in range(256):
            M[i] = (i * 0x06a0dd9b + 0x06a0dd9b) & 0xFFFFFFFF
            L[i] = (i * 0x9e3779b7 + 0x9e3779b7) & 0xFFFFFFFF

    for _ in range(iteration):
        # shuffle sederhana
        a = cons
        b = _
        c = 0
        d = 0

        if verbose:
            print(b, end=' ')
        for i in range(255, 0, -1):
            o = 0
            for e in range(8):
                o ^= (M[(i + e) & 0xFF] << e) & 0xFFFFFFFF

            if verbose:
                print(b, end=' ')
            a = (rot32(b ^ o, d) ^ (cons + a)) & 0xFFFFFFFF
            b = (rot32(cons + a, i) ^ (o + d)) & 0xFFFFFFFF
            o = (rot32(a ^ o, i) << 9 ^ (b >> 18)) & 0xFFFFFFFF
            c = rot32((o + c << 14) ^ (b >> 13) ^ a, b) & 0xFFFFFFFF

            d  = (c * (i + 1)) >> 32

            L[i], L[d] = L[d], L[i]

        for i in range(256):
            M[i] ^= L[i]
        if verbose:
            print()
        cons_list = ra_hash(M)

        new_cons = 0
        for e in range(8):
            new_cons ^= (cons_list[e] << e) & 0xFFFFFFFF
        cons = new_cons

    return cons


def main():
    start = time.time()
    cons = ra_core(seed=1, iteration=1, verbose=True)
    print(f"\ncons = {cons}")
    print(f"Done in {time.time() - start:.3f} seconds")

if __name__ == '__main__':
    main()
