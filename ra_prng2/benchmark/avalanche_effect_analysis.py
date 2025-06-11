import numpy as np
import matplotlib.pyplot as plt

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

def zepfold_capture(seed, iteration=1):
    L = [0] * 256
    M = [0] * 256
    cons = seed
    outputs = []  # collect c values

    for i in range(256):
        M[i] = (i * 0x06a0dd9b + 0x06a0dd9b) & 0xFFFFFFFF
        L[i] = (i * 0x9e3779b7 + 0x9e3779b7) & 0xFFFFFFFF

    for _ in range(iteration):
        a = cons
        b = _
        c = 0
        for i in range(255, 0, -1):
            o = 0
            for e in range(8):
                o ^= (M[(i + e) & 0xFF] << e) & 0xFFFFFFFF

            a = (rot32(b ^ o, c) ^ (cons + a)) & 0xFFFFFFFF
            b = (rot32(cons + a, i) ^ (o + c)) & 0xFFFFFFFF
            o = (rot32(a ^ o, i) << 9 ^ (b >> 18)) & 0xFFFFFFFF
            c = rot32((o + c << 14) ^ (b >> 13) ^ a, b) & 0xFFFFFFFF
            outputs.append(c)
            c = (c * (i + 1)) >> 32
            

        for i in range(256):
            M[i] ^= L[i]

        cons_list = ZepXORhash(M)
        new_cons = 0
        for e in range(8):
            new_cons ^= (cons_list[e] << e) & 0xFFFFFFFF
        cons = new_cons

    return outputs

# Baseline outputs
seed = 1
orig_outputs = zepfold_capture(seed, iteration=1)

# Avalanche matrix: rows=32 bit flips, cols=255 outputs, values=hamming distance
num_outputs = len(orig_outputs)
avalanche_matrix = np.zeros((32, num_outputs), dtype=int)

for bit in range(32):
    seed = 0 ^ (1 << bit + 1)
    mutated_outputs = zepfold_capture(seed, iteration=1)
    # compute hamming distance per output
    for i in range(num_outputs):
        xor_val = orig_outputs[i] ^ mutated_outputs[i]
        avalanche_matrix[bit, i] = bin(xor_val).count('1')

# Plot heatmap: rows=bit flips, cols=output index
plt.figure(figsize=(10, 6))
plt.imshow(avalanche_matrix, aspect='auto')
plt.xlabel('Output Index (0-254)')
plt.ylabel('Flipped Constant Bit (0-31)')
plt.title('Avalanche Effect Heatmap: Hamming Distances')
plt.colorbar(label='Bit Differences (0-32)')
plt.tight_layout()
plt.show()
