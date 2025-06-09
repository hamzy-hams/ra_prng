import sys
import time

# last updated 29/5/2025 - see references on heatmap.py

# PERFECT VERSION 29 MAY  
# 6a0dd9b3
# FFFFFFFF

def rot32(n: int, r: int) -> int:
    r &= 31
    n &= 0xFFFFFFFF
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFF

def ZepXORhash(N):
    out = [0] * 8
    for i in range(8):
        out[i] ^= N[N[i] & 0xFF]
        for j in range(32):
            N[i] ^= N[j * 8 + i]
    return out

def ZepFold(seed, iteration):
    L = [0] * 256
    M = [0] * 256
    cons = seed
    print(cons)

    for i in range(256):
            M[i] = (i * 0x06a0dd9b + 0x06a0dd9b) & 0xFFFFFFFF
            L[i] = (i * 0x9e3779b7 + 0x9e3779b7) & 0xFFFFFFFF

    for _ in range(iteration):
        # shuffle sederhana
        a = cons
        b = _
        c = 0
        print(b, end=' ')
        for i in range(255, 0, -1):
            o = 0
            for e in range(8):
                o ^= (M[(i + e) & 0xFF] << e) & 0xFFFFFFFF

            print( b, end=' ')
            a = (rot32(b ^ o, c) ^ (cons + a)) & 0xFFFFFFFF
            b = (rot32(cons + a, i) ^ (o + c)) & 0xFFFFFFFF
            o = (rot32(a ^ o, i) << 9 ^ (b >> 18)) & 0xFFFFFFFF
            c = rot32((o + c << 14) ^ (b >> 13) ^ a, b) & 0xFFFFFFFF
            
            c  = (c * (i + 1)) >> 32
            
            L[i], L[c] = L[c], L[i]

        for i in range(256):
            M[i] ^= L[i]
        print()
        cons_list = ZepXORhash(M)

        new_cons = 0
        for e in range(8):
            new_cons ^= (cons_list[e] << e) & 0xFFFFFFFF
        cons = new_cons
        


def main():
    start = time.time()
    ZepFold(seed=1, iteration=1)
    print(f"\nDone in {time.time() - start:.3f} seconds")

if __name__ == '__main__':
    main()

'''
Terakhir diupdate pada 29 mei
kecepatan : 372.6/4 juta
sha256: 157.9180462360382/1 juta
'''