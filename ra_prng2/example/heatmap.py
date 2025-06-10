import sys
import time
import numpy as np
import matplotlib.pyplot as plt

# PERFECT VERSION 29 MAY  
# 6a0dd9b3
# FFFFFFFF

def rot32(n, r):
    r &= 31
    n &= (1 << 32) - 1
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFF

def ZepXORhash(N):
    for i in range(8):
        N[i] ^= N[N[i] & 0xFF]
        for a in range(32):
            N[i] ^= N[a * 8 + i]
    return N[:8]

def ZepFold(seed, iteration):
    L = [0] * 256
    M = [0] * 256
    const = seed

    # Inisialisasi matriks hitung perpindahan: rows=original index, cols=new index
    counts = np.zeros((256, 256), dtype=int)
    for i in range(256):
            M[i] = (i * 0x06a0dd9b + 0x06a0dd9b) & 0xFFFFFFFF
            L[i] = (i * 0x9e3779b7 + 0x9e3779b7) & 0xFFFFFFFF

    data = [0] * 256

    for _ in range(iteration):

        # reset data ke identitas
        for i in range(256):
            data[i] = i

        # shuffle sederhana
        a = const
        b = _
        c = 0
        for i in range(255, 0, -1):
            o = 0
            for e in range(8):
                o ^= (M[i + e & 0xFF] << e) & 0xFFFFFFFF

            a  = (rot32(b ^ o, c) ^ (const + a)) & 0xFFFFFFFF
            b  = (rot32(const + a, i) ^ (o ^ c)) & 0xFFFFFFFF
            o = (rot32(a ^ o, i) << 9 ^ (b >> 18)) & 0xFFFFFFFF
            c = rot32((o + c << 14) ^ (b >> 13) ^ a, b) & 0xFFFFFFFF
            #print(hex(c)[2:].zfill(8), end='')
            c  = (c * (i + 1)) >> 32
            

            L[i], L[c] = L[c], L[i]
            data[i], data[c] = data[c], data[i]
            

        # ** Rekam perpindahan: untuk setiap posisi dest=a, orig=data[a] 
        #print(bin((rot32(a ^ o, i) << 20) & 0xFFFFFFFF)[2:].zfill(32), " ^ ", bin(b >> 12)[2:].zfill(32))
            
        for dest in range(256):
            orig = data[dest]
            counts[orig, dest] += 1
        
        for i in range(256):
            M[i] ^= L[i]
        const_list = ZepXORhash(M)
        # result nya adalah nilai M yang meng XOR result sebelumnya
        new_cons = 0
        for e in range(8):
            new_cons ^= const_list[e] << e
        const = new_cons

    return counts

def main():
    start = time.time()
    counts = ZepFold(seed=1, iteration=100000)  # misal 50k iterasi
    print(f"Done in {time.time() - start:.3f} seconds")

    # Plot heatmap perpindahan
    plt.figure(figsize=(8, 8))
    plt.imshow(counts, interpolation='nearest', aspect='auto')
    plt.colorbar(label='Frekuensi perpindahan')
    plt.xlabel('Posisi tujuan (dest index)')
    plt.ylabel('Posisi asal (original index)')
    plt.title('Heatmap: Seberapa Sering Indeks i Pindah ke a')
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    main()
