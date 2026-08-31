#!/usr/bin/env python3
# Copyright (c) 2025 Hamas A. Rahman
# Licensed under CC BY-NC-SA 4.0
# github.com/hamzy-hams

"""
scrambler_wired_addressable.py

Python reference port of scrambler_wired_addressable.c. Same relationship
to scrambler_addressable.py as the .c file has to scrambler_addressable.c:
identical double-swap ZepFold shuffle loop, only the a/b/o/c/d recurrence
body is replaced with winner_wired_v2's pruned+wired formula (byte-for-byte
from ../2026-8-30_addressable-init-research/winner_wired_addressable.c's
ra_permutation_cycle()).

This exists to bit-for-bit verify the C implementation (see
verify_parity_wired.py) -- same discipline as scrambler_addressable's
verify_parity.py / Tahap 2 of the addressable-init research.
"""

import sys
import time
import argparse

MASK32 = 0xFFFFFFFF
ADDR_L_MIX_CONST = 0x9e3779b7


def load_token_ids_from_file(filename):
    try:
        with open(filename, "r") as f:
            content = f.read().strip()
            if not content:
                return []
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
            text = " ".join(map(str, arr))
            f.write(text)
    except IOError as e:
        print(f"Error: Gagal menulis ke file '{filename}': {e}", file=sys.stderr)
        sys.exit(1)


def rot32(n: int, r: int) -> int:
    r &= 31
    n &= MASK32
    return ((n << r) | (n >> (32 - r))) & MASK32


def ra_hash(N):
    out = [0] * 8
    for i in range(8):
        out[i] ^= N[N[i] & 0xFF]
        for j in range(32):
            N[i] ^= N[j * 8 + i]
    return out


def ra_init_state_addressable(key: int):
    """Byte-for-byte port of winner_wired_addressable.c's function of the
    same name (Tahap 1 frozen formula, Kandidat 5, address = key only) --
    unchanged from scrambler_addressable.py."""
    L = [0] * 256
    M = [0] * 256
    for i in range(256):
        r = (key ^ i) & MASK32
        l_val = (i * ADDR_L_MIX_CONST + ADDR_L_MIX_CONST * key) & MASK32
        m_val = (i * 0x06a0dd9b + 0x06a0dd9b * key) & MASK32
        L[i] = rot32(l_val, r)
        M[i] = rot32(m_val, r)
    return L, M


# Shuffle array function -- same double-swap ZepFold loop shape as
# scrambler_addressable.py's ra_shuffle(). The ONLY change is the
# a/b/o/c/d recurrence body (winner_wired_v2's pruned+wired formula instead
# of the original one) and the resulting `d` update used for the L[] swap.
def ra_shuffle(key, tokens):
    scrambled_tokens = tokens[:]
    tokens_length = len(tokens)
    iteration = tokens_length // 255 + 1
    count = tokens_length - 1

    L, M = ra_init_state_addressable(key)
    cons = key

    for it in range(iteration):
        a = cons
        b = it
        c = 0
        d = 0

        for i in range(255, 0, -1):
            # winner_wired_v2's wiring: o from M[i+6]/M[i+7] only.
            o = (M[(i + 6) & 0xFF] << 6) ^ (M[(i + 7) & 0xFF] << 7)

            a = (d ^ o) ^ (cons + a)
            a &= MASK32
            b = (cons + a) ^ (o + d)
            b &= MASK32
            c = rot32((a >> 13) ^ a, b)

            idx = (c * (count + 1)) >> 32

            d = c & 0xFF
            scrambled_tokens[count], scrambled_tokens[idx] = scrambled_tokens[idx], scrambled_tokens[count]
            L[i], L[d] = L[d], L[i]

            if count <= 1:
                break
            count -= 1

        if count <= 1:
            return scrambled_tokens

        for i in range(256):
            M[i] ^= L[i]
        cons_list = ra_hash(M)

        new_cons = 0
        for e in range(8):
            new_cons ^= (cons_list[e] << e) & MASK32
        cons = new_cons

    return scrambled_tokens


def main():
    start = time.time()
    parser = argparse.ArgumentParser(
        description="Shuffle urutan token ID (angka) dalam file, addressable-init + winner_wired_v2 cycle per key."
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
        "--key", "-k",
        type=int,
        default=None,
        help="(Opsional) Key integer untuk reproducibility/addressing. "
             "Kalau tidak diberi, maka nilai 1 akan digunakan."
    )
    args = parser.parse_args()
    key = args.key if args.key is not None else 1

    tokens = load_token_ids_from_file(args.input)
    if not tokens:
        print("Warning: File input kosong atau hanya berisi whitespace. "
              "Tidak ada data yang akan diacak.")
        save_token_ids_to_file(tokens, args.output)
        print(f"File output '{args.output}' telah dibuat (kosong).")
        sys.exit(0)

    shuffled = ra_shuffle(key & MASK32, tokens)
    save_token_ids_to_file(shuffled, args.output)
    print(f"Proses shuffle selesai. Hasil disimpan di '{args.output}'.")
    print(f"\nDone in {time.time() - start:.3f} seconds")


if __name__ == '__main__':
    main()
