"""Part B: bit/position bias localization for the K=1+hashed-key defect.

Reads raw little-endian uint32 words (from the SAME already byte-exact-
verified-vs-production control binary used in Part A -- no new C code) and
computes, for each of the 32 output-bit positions, the marginal frequency of
1-bits across all words, with a z-score against the null hypothesis of an
unbiased bit (p=0.5). Run side-by-side on hashed-ordering K=1 output (every
word is "word #1 of a fresh call", the FAIL case) vs Weyl-ordering K=1 output
(same structural situation, but known CLEAN) to see whether a marginal
single-bit bias exists and, if so, whether it differs between orderings (it
plausibly won't -- see note in analyze() below on why a marginal per-bit test
may not be enough) or is masked by the specific FPF-14+6/16 windowed test
PractRand actually flagged (a frequency-of-N-bit-tuples-in-a-window test).

Self-check discipline (repo convention: verify the tool before trusting its
output on the real question): run first against synthetic os.urandom data,
which MUST report no significant bias, before trusting any result on real
orbit_round1 output.
"""

from __future__ import annotations

import argparse
import math
import os
import struct
import sys
from pathlib import Path

WORDS_PER_CHUNK = 1 << 20  # 1M words (4MB) per read chunk


def iter_words(path: Path, max_words: int | None = None):
    n = 0
    with path.open("rb") as f:
        while True:
            chunk = f.read(WORDS_PER_CHUNK * 4)
            if not chunk:
                return
            nwords = len(chunk) // 4
            if nwords == 0:
                return
            words = struct.unpack(f"<{nwords}I", chunk[: nwords * 4])
            for w in words:
                yield w
                n += 1
                if max_words is not None and n >= max_words:
                    return


def bit_frequencies(path: Path, max_words: int | None = None):
    """Return (n_words, counts[32]) where counts[b] = number of words with bit b set."""
    counts = [0] * 32
    n = 0
    for w in iter_words(path, max_words):
        n += 1
        b = 0
        x = w
        while x:
            lsb = x & (-x)
            idx = lsb.bit_length() - 1
            counts[idx] += 1
            x &= x - 1
    return n, counts


def bit_frequencies_fast(path: Path, max_words: int | None = None):
    """Vectorized-ish version: unpack in bulk, tally via bit masks per chunk."""
    counts = [0] * 32
    n = 0
    with path.open("rb") as f:
        while True:
            chunk = f.read(WORDS_PER_CHUNK * 4)
            if not chunk:
                break
            nwords = len(chunk) // 4
            if nwords == 0:
                break
            if max_words is not None and n + nwords > max_words:
                nwords = max_words - n
                if nwords <= 0:
                    break
                chunk = chunk[: nwords * 4]
            words = struct.unpack(f"<{nwords}I", chunk)
            for b in range(32):
                mask = 1 << b
                counts[b] += sum(1 for w in words if w & mask)
            n += nwords
            if max_words is not None and n >= max_words:
                break
    return n, counts


def z_scores(n: int, counts: list[int]):
    """z-score per bit under null p=0.5, plus overall chi-square-style max |z|."""
    expected = n / 2.0
    stderr = math.sqrt(n * 0.25)
    zs = [(counts[b] - expected) / stderr for b in range(32)]
    return zs


def report(label: str, n: int, counts: list[int]):
    zs = z_scores(n, counts)
    print(f"\n=== {label} (N={n:,} words) ===")
    print(f"{'bit':>4} {'count':>12} {'frac':>8} {'z':>8}")
    flagged = []
    for b in range(32):
        frac = counts[b] / n
        z = zs[b]
        marker = ""
        if abs(z) >= 4:
            marker = "  <-- |z|>=4"
            flagged.append((b, z))
        print(f"{b:>4} {counts[b]:>12} {frac:>8.5f} {z:>8.2f}{marker}")
    max_abs_z = max(abs(z) for z in zs)
    print(f"max |z| across 32 bits: {max_abs_z:.2f}  (>=4 is strong evidence of marginal bias; "
          f"note a windowed/tuple-frequency defect like PractRand's FPF-14+6/16 need NOT show up "
          f"as a marginal single-bit bias -- absence of a flag here does not clear the RNG)")
    if flagged:
        print(f"FLAGGED bits (|z|>=4): {flagged}")
    else:
        print("No single bit flagged at |z|>=4.")
    return {"n": n, "counts": counts, "z": zs, "max_abs_z": max_abs_z, "flagged": flagged}


def self_check():
    print("Self-check: os.urandom, expect no flagged bits...")
    tmp = Path("/tmp") / "bit_bias_selfcheck.bin"
    tmp.write_bytes(os.urandom(4 * 2_000_000))
    n, counts = bit_frequencies_fast(tmp)
    result = report("SELF-CHECK (os.urandom)", n, counts)
    tmp.unlink()
    if result["flagged"]:
        print("WARNING: self-check flagged bits on pure random data -- "
              "investigate the analysis script before trusting real results.")
        return False
    print("Self-check OK: no false positives on random data.")
    return True


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--self-check-only", action="store_true")
    ap.add_argument("files", nargs="*", help="label=path pairs, e.g. hashed=out_hashed.bin weyl=out_weyl.bin")
    ap.add_argument("--max-words", type=int, default=None)
    args = ap.parse_args()

    ok = self_check()
    if args.self_check_only:
        sys.exit(0 if ok else 1)
    if not ok:
        print("Aborting: self-check failed, fix the script first.")
        sys.exit(1)

    for spec in args.files:
        label, _, path_str = spec.partition("=")
        if not path_str:
            print(f"skip malformed arg {spec!r}, expected label=path")
            continue
        path = Path(path_str)
        n, counts = bit_frequencies_fast(path, args.max_words)
        report(label, n, counts)


if __name__ == "__main__":
    main()
