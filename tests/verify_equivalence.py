#!/usr/bin/env python3
# Copyright (c) 2025 Hamas A. Rahman
# Licensed under CC BY-NC-SA 4.0
# github.com/hamzy-hams
# comparisons between python implementation and cpython binding of the ra_prng2 design.
import sys
import time
import ra_prng

# ---- Common functions ----
def rot32(n: int, r: int) -> int:
    r &= 31
    n &= 0xFFFFFFFF
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFF

def array_state_init():
    for i in range(256):
        M[i] = (i * 0x06a0dd9b + 0x06a0dd9b) & 0xFFFFFFFF
        L[i] = (i * 0x9e3779b7 + 0x9e3779b7) & 0xFFFFFFFF

def ra_core(i):
    global a, b, c, d, L, M

    o = 0
    for e in range(8):
        o ^= (M[(i + e) & 0xFF] << e) & 0xFFFFFFFF

    a = (rot32(b ^ o, d) ^ (cons + a)) & 0xFFFFFFFF
    b = (rot32(cons + a, i) ^ (o + d)) & 0xFFFFFFFF
    o = (rot32(a ^ o, i) << 9 ^ (b >> 18)) & 0xFFFFFFFF
    c = rot32((o + (c << 14)) ^ (b >> 13) ^ a, b) & 0xFFFFFFFF
    
    d  = (c * (i + 1)) >> 32
    L[i], L[d] = L[d], L[i]

    return c

def reseed():
    global cons, M, L

    for i in range(256):
        M[i] ^= L[i]
    cons_list = ra_hash(M)

    new_cons = 0
    for e in range(8):
        new_cons ^= (cons_list[e] << e) & 0xFFFFFFFF
    cons = new_cons  

def ra_hash(N):
    out = [0] * 8
    for i in range(8):
        idx = N[i] & 0xFF
        out[i] ^= N[idx]
        for j in range(32):
            out[i] ^= N[j * 8 + i]
    return out

if __name__ == '__main__':
    seed = 1
    limit = 1000
    counter = 0

    rng = ra_prng.RA_PRNG(seed=seed)

    # ---- array state initialization ---
    L = [0] * 256
    M = [0] * 256

    array_state_init()

    cons = seed & 0xFFFFFFFF

    outer_it = 0
    while outer_it * 255 < limit:
        # Reset per outer iteration
        a = cons
        b = outer_it      # ← BENAR, bukan counter // 256
        c = 0
        d = 0

        for i in range(255, 0, -1):
            ra_core(i)    # jalankan semua 255 inner steps

        reseed()

        # Baru bandingkan cons hasil Python vs cons hasil C extension
        result_cpython = rng.next()
        assert cons == result_cpython, f"Mismatch at outer_it={outer_it}: {cons} != {result_cpython}"

        outer_it += 1

# the output maintain same for both implementation, which is expected as the python code is a direct translation of the cpython binding.
# 10100001011101101110011101011000
# 10100001011101101110011101011000