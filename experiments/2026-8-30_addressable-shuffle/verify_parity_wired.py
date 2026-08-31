#!/usr/bin/env python3
"""verify_parity_wired.py -- bit-for-bit parity check between
scrambler_wired_addressable.c (compiled binary) and
scrambler_wired_addressable.py, across several keys and input sizes.

Sibling of verify_parity.py (not a modification of it -- that script is
reused unchanged elsewhere and stays scoped to scrambler_addressable).
Same KEYS/SIZES matrix and same discipline: a C implementation is not
trusted until it matches a faithful Python port byte-for-byte.

Usage: python3 verify_parity_wired.py
Requires ./scrambler_wired_addressable (compiled) to exist in this directory.
"""

import random
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).parent
BINARY = HERE / "scrambler_wired_addressable"

sys.path.insert(0, str(HERE))
from scrambler_wired_addressable import ra_shuffle  # noqa: E402

KEYS = [0, 1, 42, 12345, 0x7FFFFFFF, 0xFFFFFFFF, 0xDEADBEEF]
SIZES = [0, 1, 2, 254, 255, 256, 511, 1000, 12345]


def run_c(tokens: list[int], key: int) -> list[int]:
    in_path = HERE / "_parity_wired_in.txt"
    out_path = HERE / "_parity_wired_out.txt"
    in_path.write_text(" ".join(map(str, tokens)))
    result = subprocess.run(
        [str(BINARY), "--input", str(in_path), "--output", str(out_path), "--key", str(key)],
        capture_output=True, text=True,
    )
    if result.returncode != 0:
        raise RuntimeError(f"C binary failed (key={key}, n={len(tokens)}): {result.stderr}")
    content = out_path.read_text().strip()
    return list(map(int, content.split())) if content else []


def main():
    if not BINARY.exists():
        print(f"Error: {BINARY} tidak ditemukan. Build dulu dengan:\n"
              f"  gcc -std=c11 -O3 -o scrambler_wired_addressable scrambler_wired_addressable.c",
              file=sys.stderr)
        sys.exit(1)

    rng = random.Random(2026_08_30)
    failures = 0
    checks = 0

    for n in SIZES:
        for key in KEYS:
            tokens = [rng.randrange(0, 2**32) for _ in range(n)]
            c_out = run_c(tokens, key)
            py_out = ra_shuffle(key & 0xFFFFFFFF, tokens[:]) if tokens else []
            checks += 1
            if c_out != py_out:
                failures += 1
                print(f"MISMATCH n={n} key={key}")
                print(f"  C[:10]  = {c_out[:10]}")
                print(f"  Py[:10] = {py_out[:10]}")

    for path in (HERE / "_parity_wired_in.txt", HERE / "_parity_wired_out.txt"):
        if path.exists():
            path.unlink()

    print(f"\n{checks} checks, {failures} failures.")
    if failures:
        sys.exit(1)
    print("PASS: scrambler_wired_addressable.c bit-identical to scrambler_wired_addressable.py "
          f"across {len(SIZES)} sizes x {len(KEYS)} keys.")


if __name__ == "__main__":
    main()
