#!/usr/bin/env python3
"""Pick GUARD_L / GUARD_M constants for the key=0 degenerate-state fix.

Requirements (user-specified, 2026-09-01):
  - popcount(guard) == 16 (equal count of 0-bits and 1-bits in 32 bits)
  - distinct from every constant already used in ra_core.c's init path:
        0x9e3779b7 (L keyterm multiplier)
        0x06a0dd9b (M keyterm multiplier)
        0x85ebca6b (fmix32 internal c1)
        0xc2b2ae35 (fmix32 internal c2)
  - GUARD_L != GUARD_M

Derivation: SHA-256 of a descriptive ASCII label, take the low 32 bits,
scan forward (label, label#2, label#3, ...) until popcount==16 and the
value is not already in the exclusion set. Deterministic/reproducible,
not hand-picked/eyeballed.
"""
import hashlib

EXISTING = {0x9e3779b7, 0x06a0dd9b, 0x85ebca6b, 0xc2b2ae35}


def candidates(label):
    n = 0
    while True:
        n += 1
        h = hashlib.sha256(f"{label}#{n}".encode()).digest()
        val = int.from_bytes(h[:4], "big")
        yield n, val


def pick(label, exclude):
    for n, val in candidates(label):
        if bin(val).count("1") == 16 and val not in exclude:
            return val, n
    raise RuntimeError("unreachable")


guard_l, n_l = pick("ra_core.keyzero_guard_l", EXISTING)
exclude2 = EXISTING | {guard_l}
guard_m, n_m = pick("ra_core.keyzero_guard_m", exclude2)

for name, val, n in (("GUARD_L", guard_l, n_l), ("GUARD_M", guard_m, n_m)):
    assert bin(val).count("1") == 16, f"{name} popcount != 16"
    assert val not in EXISTING, f"{name} collides with an existing constant"
print(f"GUARD_L = 0x{guard_l:08x}  popcount={bin(guard_l).count('1')}  (sha256 attempt #{n_l})")
print(f"GUARD_M = 0x{guard_m:08x}  popcount={bin(guard_m).count('1')}  (sha256 attempt #{n_m})")
assert guard_l != guard_m, "GUARD_L == GUARD_M"
print("OK: both popcount==16, distinct from each other and from all 4 existing constants.")

print()
print("Existing constants (for reference):")
for name, v in {
    "L keyterm mul (0x9e3779b7)": 0x9e3779b7,
    "M keyterm mul (0x06a0dd9b)": 0x06a0dd9b,
    "fmix32 c1 (0x85ebca6b)": 0x85ebca6b,
    "fmix32 c2 (0xc2b2ae35)": 0xc2b2ae35,
}.items():
    print(f"  {name}: popcount={bin(v).count('1')}")
