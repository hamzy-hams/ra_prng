#!/usr/bin/env python3
"""Pick GUARD_D constant for the orbit K=1+hashed-key warmup fix (Candidate A).

Same method as ../2026-9-1_keyzero-guard-fix/pick_guard_constants.py:
  - popcount(guard) == 16
  - distinct from every constant already in ra_core.c's init path
    (0x9e3779b7, 0x06a0dd9b, 0x85ebca6b, 0xc2b2ae35) and from GUARD_L/GUARD_M
    (0x38916df4 / 0x6c26fc92, both popcount 16, picked 2026-09-01).

Derivation: SHA-256 of a descriptive ASCII label, take the low 32 bits of the
digest, scan forward (label#1, label#2, ...) until popcount==16 and the value
is not already in the exclusion set. Deterministic/reproducible.
"""
import hashlib

EXISTING = {0x9e3779b7, 0x06a0dd9b, 0x85ebca6b, 0xc2b2ae35, 0x38916df4, 0x6c26fc92}


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


guard_d, n_d = pick("ra_core.orbit_warmup_guard_d", EXISTING)

assert bin(guard_d).count("1") == 16, "GUARD_D popcount != 16"
assert guard_d not in EXISTING, "GUARD_D collides with an existing constant"
print(f"GUARD_D = 0x{guard_d:08x}  popcount={bin(guard_d).count('1')}  (sha256 attempt #{n_d})")
print("OK: popcount==16, distinct from GUARD_L, GUARD_M, and the 4 pre-existing constants.")

print()
print("Existing constants (for reference):")
for name, v in {
    "L keyterm mul (0x9e3779b7)": 0x9e3779b7,
    "M keyterm mul (0x06a0dd9b)": 0x06a0dd9b,
    "fmix32 c1 (0x85ebca6b)": 0x85ebca6b,
    "fmix32 c2 (0xc2b2ae35)": 0xc2b2ae35,
    "GUARD_L (0x38916df4)": 0x38916df4,
    "GUARD_M (0x6c26fc92)": 0x6c26fc92,
}.items():
    print(f"  {name}: popcount={bin(v).count('1')}")
