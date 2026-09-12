"""Pure-Python reference oracle for orbit-d0-raw-cons-fix (d0_raw_cons
candidate). Direct port of
../2026-9-4_orbit-init-warmup-fix/combo_prng_warmup.py, replacing the
d0_hash/d0_realround variants with a single new one: d0 seeded directly
from cons/key (raw low byte, NO fmix32 hash step) -- narrower test of the
"d starts nonzero" idea the user asked for, after d0_hash (a hashed
nonzero d) already failed PractRand.

Must match the compiled C candidate byte-for-byte before any PractRand run
(feedback_no_reseed_on_fail: verify code before trusting statistics).

rot32/fmix32/GUARD_L/GUARD_M/ra_hash/ra_reseed/ra_init_state_orbit are
copied verbatim (translated to Python with explicit 32-bit masking) from
../2026-9-1_family-productionization/ra_core.c L144-207.
"""

from __future__ import annotations

MASK32 = 0xFFFFFFFF

GUARD_L = 0x38916DF4
GUARD_M = 0x6C26FC92

VARIANTS = ("control", "d0_raw_cons")


def rot32(n: int, r: int) -> int:
    n &= MASK32
    r &= 31
    if r == 0:
        return n
    return ((n << r) | (n >> (32 - r))) & MASK32


def fmix32(h: int) -> int:
    h &= MASK32
    h ^= h >> 16
    h = (h * 0x85EBCA6B) & MASK32
    h ^= h >> 13
    h = (h * 0xC2B2AE35) & MASK32
    h ^= h >> 16
    return h & MASK32


def ra_hash(N: list[int]) -> list[int]:
    out8 = [0] * 8
    for i in range(8):
        v = 0
        base = i * 32
        for j in range(32):
            v ^= N[base + j]
        out8[i] = v & MASK32
    return out8


def ra_reseed(M: list[int], L: list[int]) -> int:
    for i in range(256):
        M[i] = (M[i] ^ L[i]) & MASK32
    tmp8 = ra_hash(M)
    new_cons = 0
    for e in range(8):
        new_cons ^= (tmp8[e] << e) & MASK32
    return new_cons & MASK32


def ra_init_state_orbit(key: int) -> tuple[list[int], list[int]]:
    key &= MASK32
    L = [0] * 256
    M = [0] * 256
    keyterm_l = fmix32(GUARD_L ^ ((0x9E3779B7 * key) & MASK32))
    keyterm_m = fmix32(GUARD_M ^ ((0x06A0DD9B * key) & MASK32))
    for i in range(256):
        r = (key ^ i) & MASK32
        l_val = (((i * 0x9E3779B7) & MASK32) * keyterm_l) & MASK32
        m_val = (((i * 0x06A0DD9B) & MASK32) * keyterm_m) & MASK32
        L[i] = rot32(l_val, r)
        M[i] = rot32(m_val, r)
    return L, M


def tap_o8(M: list[int], i: int) -> int:
    o = 0
    for e in range(8):
        o ^= (M[(i + e) & 0xFF] << e) & MASK32
    return o & MASK32


def warmup(key: int, variant: str) -> tuple[int, int]:
    key &= MASK32
    if variant == "control":
        return key, 0
    if variant == "d0_raw_cons":
        return key, key & 0xFF
    raise KeyError(variant)


def ra_permutation_cycle_orbit_warmup(cons: int, it: int, a0: int, d0: int,
                                       M: list[int], L: list[int],
                                       count: list[int], out: list[int]) -> None:
    a = a0 if it == 0 else (cons & MASK32)
    d = d0 if it == 0 else 0
    i = 255
    while i > 0:
        o = tap_o8(M, i)
        a = ((d ^ o) ^ ((cons + a) & MASK32)) & MASK32
        b = (((cons + a) & MASK32) ^ ((o + d) & MASK32)) & MASK32
        c = rot32((a >> 13) ^ a, b)
        c ^= c >> 17
        c &= MASK32
        out.append(c)

        d = c & 0xFF
        if count[0] <= 1:
            break
        count[0] -= 1

        L[i], L[d] = L[d], L[i]
        i -= 1


def orbit_stream_full(key: int, rng: int, variant: str) -> list[int]:
    if rng == 0:
        return []
    key &= MASK32
    L, M = ra_init_state_orbit(key)
    a0, d0 = warmup(key, variant)
    count = [rng]
    cons = key
    iteration = rng // 255 + 1
    out: list[int] = []
    it = 0
    while it < iteration:
        ra_permutation_cycle_orbit_warmup(cons, it, a0, d0, M, L, count, out)
        if count[0] <= 1:
            return out
        cons = ra_reseed(M, L)
        it += 1
    return out


def orbit_round1(key: int, variant: str) -> int:
    return orbit_stream_full(key, 1, variant)[0]


if __name__ == "__main__":
    v = orbit_round1(1, "control")
    print(f"orbit_round1(1, control) = {v}")
    assert v == 397095967, v

    a0, d0 = warmup(1, "d0_raw_cons")
    print(f"warmup(key=1, d0_raw_cons) -> a0={a0}, d0={d0}")
    assert d0 == (1 & 0xFF), d0

    s2 = orbit_stream_full(12345, 300, "d0_raw_cons")
    assert len(s2) == 300, len(s2)
    print(f"orbit_stream_full(12345, 300, d0_raw_cons) OK: {len(s2)} words, no crash")

    print("ORACLE self-check: PASS")
