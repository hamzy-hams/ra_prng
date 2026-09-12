#!/usr/bin/env python3
"""Byte-exact oracle for weyl_fmix32.c (and weyl_triple32.c) -- replicates
the C candidate word-for-word in Python with explicit 0xFFFFFFFF masking at
every step (per repo discipline: byte-exact oracle vs codegen MUST pass
before any statistical test, see HANDOVER.md section 6 step 1).

Usage:
  combo_prng_weylfinal.py single <key> <rng> [--finalizer fmix32|triple32]
  combo_prng_weylfinal.py stream <base_key> <n> <K> [--finalizer ...]
  combo_prng_weylfinal.py stream-hashed <base_key> <n> <K> [--finalizer ...]

Writes raw little-endian uint32 words to stdout (same format as the C
binary's stdout), so it can be diffed byte-for-byte against `./weyl_fmix32
--single ...` etc.
"""
import sys
import struct
import argparse

MASK32 = 0xFFFFFFFF
WEYL32 = 0x9e3779b7
GUARD_KEY = 0x17067f32
WEYL32_STREAM_STEP = 0x9E3779B9  # test-harness --stream key-stepping constant


def fmix32(h):
    h &= MASK32
    h ^= h >> 16
    h = (h * 0x85ebca6b) & MASK32
    h ^= h >> 13
    h = (h * 0xc2b2ae35) & MASK32
    h ^= h >> 16
    return h & MASK32


def triple32(x):
    x &= MASK32
    x ^= x >> 17
    x = (x * 0xed5ad4bb) & MASK32
    x ^= x >> 11
    x = (x * 0xac4c1b51) & MASK32
    x ^= x >> 15
    x = (x * 0x31848bab) & MASK32
    x ^= x >> 14
    return x & MASK32


def make_finalizer(name):
    if name == "fmix32":
        return fmix32
    if name == "triple32":
        return triple32
    raise ValueError(f"unknown finalizer {name!r}")


def weyl_core(key, rng, finalizer):
    state0 = finalizer(GUARD_KEY ^ (key & MASK32))
    out = []
    for j in range(rng):
        state_j = (state0 + ((j & MASK32) * WEYL32) & MASK32) & MASK32
        out.append(finalizer(state_j))
    return out


def splitmix32_stream(state_box):
    """Matches multikey_stream_hashed.c splitmix32(uint64_t*) exactly --
    64-bit SplitMix64 step, high-32-bit truncated. state_box is a 1-elem
    list used as a mutable uint64 cell."""
    MASK64 = 0xFFFFFFFFFFFFFFFF
    state_box[0] = (state_box[0] + 0x9E3779B97F4A7C15) & MASK64
    z = state_box[0]
    z = ((z ^ (z >> 30)) * 0xBF58476D1CE4E5B9) & MASK64
    z = ((z ^ (z >> 27)) * 0x94D049BB133111EB) & MASK64
    z = z ^ (z >> 31)
    return (z >> 32) & MASK32


def emit(words):
    sys.stdout.buffer.write(struct.pack(f"<{len(words)}I", *words))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("mode", choices=["single", "stream", "stream-hashed"])
    ap.add_argument("a", type=lambda s: int(s, 0))
    ap.add_argument("b", type=lambda s: int(s, 0))
    ap.add_argument("c", type=lambda s: int(s, 0), nargs="?")
    ap.add_argument("--finalizer", default="fmix32", choices=["fmix32", "triple32"])
    args = ap.parse_args()
    finalizer = make_finalizer(args.finalizer)

    if args.mode == "single":
        key, rng = args.a, args.b
        emit(weyl_core(key, rng, finalizer))
        return

    base_key, n, K = args.a, args.b, args.c
    if K is None or K == 0 or K > 255:
        print("K must be in [1,255]", file=sys.stderr)
        sys.exit(1)

    remaining = n
    if args.mode == "stream":
        block_idx = 0
        while remaining > 0:
            block_len = min(remaining, K)
            key = (base_key + ((block_idx & MASK32) * WEYL32_STREAM_STEP) & MASK32) & MASK32
            emit(weyl_core(key, block_len, finalizer))
            remaining -= block_len
            block_idx += 1
    else:  # stream-hashed
        state_box = [base_key & MASK32]
        while remaining > 0:
            block_len = min(remaining, K)
            key = splitmix32_stream(state_box)
            emit(weyl_core(key, block_len, finalizer))
            remaining -= block_len


if __name__ == "__main__":
    main()
