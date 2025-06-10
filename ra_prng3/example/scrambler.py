#!/usr/bin/env python3
 
import sys
import time
import argparse

def load_token_ids_from_file(filename):

    try:
        with open(filename, "r") as f:
            content = f.read().strip()
            if not content:
                return []
            # Pisah berdasarkan whitespace, lalu konversi ke int
            tokens = list(map(int, content.split()))
            return tokens
    except FileNotFoundError:
        print(f"Error: File '{filename}' tidak ditemukan.", file=sys.stderr)
        sys.exit(1)
    except ValueError:
        print(f"Error: Isi file '{filename}' mengandung token yang bukan angka bulat.", file=sys.stderr)
        sys.exit(1)

def save_token_ids_to_file(arr, filename):
    try:
        with open(filename, "w") as f:
            # Gabungkan kembali dengan spasi
            text = " ".join(map(str, arr))
            f.write(text)
    except IOError as e:
        print(f"Error: Gagal menulis ke file '{filename}': {e}", file=sys.stderr)
        sys.exit(1)

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

# Shuffle array function
def ZepFold(seed, tokens, multiplier_m, multiplier_l):
    scrambled_tokens = tokens[:]
    tokens_length = len(tokens)
    iteration = tokens_length // 255 + 1
    count = tokens_length - 1

    L = [0] * 256
    M = [0] * 256
    cons = seed

    for i in range(256):
            M[i] = (i * multiplier_m + 0x06a0dd9b) & 0xFFFFFFFF
            L[i] = (i * multiplier_l + 0x9e3779b7) & 0xFFFFFFFF

    for it in range(iteration):
        # shuffle sederhana
        a = cons
        b = it
        c = 0
        for i in range(255, 0, -1):
            o = 0
            for e in range(8):
                o ^= (M[(i + e) & 0xFF] << e) & 0xFFFFFFFF

            a = (rot32(b ^ o, c) ^ (cons + a)) & 0xFFFFFFFF
            b = (rot32(cons + a, i) ^ (o + c)) & 0xFFFFFFFF
            o = (rot32(a ^ o, i) << 9 ^ (b >> 18)) & 0xFFFFFFFF
            c = rot32((o + c << 14) ^ (b >> 13) ^ a, b) & 0xFFFFFFFF
            L[i] = rot32(L[i] ^ (c << 29), i) & 0xFFFFFFFF
            idx = (c * (count + 1)) >> 32
            
            c  = (c * (i + 1)) >> 32
            scrambled_tokens[count], scrambled_tokens[idx] = scrambled_tokens[idx], scrambled_tokens[count]
            L[i], L[c] = L[c], L[i]

            if count <= 1:
                break
            count -= 1

        if count <= 1:
            return scrambled_tokens

        for i in range(256):
            M[i] ^= L[i]
        cons_list = ZepXORhash(M)

        new_cons = 0
        for e in range(8):
            new_cons ^= (cons_list[e] << e) & 0xFFFFFFFF
        cons = new_cons
        


def main():
    start = time.time()
    parser = argparse.ArgumentParser(
        description="Shuffle urutan token ID (angka) dalam file menggunakan PRNG berbasis /dev/urandom."
    )
    parser.add_argument(
        "--input", "-i",
        required=True,
        help="Nama file input (teks, angka dipisah spasi)."
    )
    parser.add_argument(
        "--output", "-o",
        required=True,
        help="Nama file output (hasil acakan)."
    )
    parser.add_argument(
        "--seed", "-s",
        type=int,
        default=None,
        help="(Opsional) Seed integer untuk reproducibility. "
             "Kalau tidak diberi, maka nilai 1 akan digunakan."
    )
    parser.add_argument(
        "--multiplier_m", "-mm",
        type=int,
        default=None,
        help="(Opsional) Seed integer untuk reproducibility. "
             "Kalau tidak diberi, maka nilai 0x06a0dd9b akan digunakan."
    )
    parser.add_argument(
        "--multiplier_l", "-ml",
        type=int,
        default=None,
        help="(Opsional) Seed integer untuk reproducibility. "
             "Kalau tidak diberi, maka nilai 0x9e3779b7 akan digunakan.."
    )
    args = parser.parse_args()
    if args.seed is not None:
        seed = args.seed
    else:
        seed = 1
    
    if args.multiplier_m is not None:
        multiplier_m = args.multiplier_m
    else:
        multiplier_m = 0x06a0dd9b
    
    if args.multiplier_l is not None:
        multiplier_l = args.multiplier_l
    else:
        multiplier_l = 0x9e3779b7
    
    # 3. Shuffle
    tokens = load_token_ids_from_file(args.input)
    if not tokens:
        print("Warning: File input kosong atau hanya berisi whitespace. "
              "Tidak ada data yang akan diacak.")
        save_token_ids_to_file(tokens, args.output)
        print(f"File output '{args.output}' telah dibuat (kosong).")
        sys.exit(0)

    shuffled = ZepFold(seed, tokens, multiplier_m, multiplier_l)
    # 4. Simpan ke file
    save_token_ids_to_file(shuffled, args.output)
    print(f"Proses shuffle selesai. Hasil disimpan di '{args.output}'.")
    print(f"\nDone in {time.time() - start:.3f} seconds")

if __name__ == '__main__':
    main()
 
