"""Pure-Python reference oracle for the orbit-ad-mul-combine-fix experiment
(ad_mul_combine candidate). Used as the sanity-check oracle for the
compiled C candidate -- must match byte-for-byte before any PractRand run
(feedback_no_reseed_on_fail: verify code before trusting statistics).

Single targeted change vs production: `a = (d ^ o) ^ (cons + a)` becomes
`a = (d * o) ^ (cons + a)`. Everything else (b, c, o's rolling-register
update, ra_init_state_orbit) is byte-for-byte production.

rot32/fmix32/GUARD_L/GUARD_M/ra_hash/ra_reseed/ra_init_state_orbit are
copied verbatim (translated to Python with explicit 32-bit masking) from
../2026-9-1_family-productionization/ra_core.c L144-207.
"""

from __future__ import annotations

MASK32 = 0xFFFFFFFF

GUARD_L = 0x38916DF4
GUARD_M = 0x6C26FC92


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


def ra_permutation_cycle_orbit_admul(cons: int, it: int,
                                      M: list[int], L: list[int],
                                      count: list[int], out: list[int]) -> None:
    a = cons & MASK32
    d = 0
    i = 255
    o = tap_o8(M, i)  # NOT mixed, production formula
    while i > 0:
        a = (((d * o) & MASK32) ^ ((cons + a) & MASK32)) & MASK32  # <-- the one change
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

        if i > 1:
            o = tap_o8(M, i - 1)  # equivalent to production's rolling register (proven elsewhere)
        i -= 1


def orbit_stream_full(key: int, rng: int) -> list[int]:
    if rng == 0:
        return []
    key &= MASK32
    L, M = ra_init_state_orbit(key)
    count = [rng]
    cons = key
    iteration = rng // 255 + 1
    out: list[int] = []
    it = 0
    while it < iteration:
        ra_permutation_cycle_orbit_admul(cons, it, M, L, count, out)
        if count[0] <= 1:
            return out
        cons = ra_reseed(M, L)
        it += 1
    return out


def orbit_round1(key: int) -> int:
    return orbit_stream_full(key, 1)[0]


if __name__ == "__main__":
    v = orbit_round1(1)
    print(f"orbit_round1(1) = {v}")
    # d=0 at round 1 (i=255), so d*o=0 there -- a=(0)^(cons+a)=cons+a=2*cons for key=1.
    # Just report the value, no fixed KAT to assert against (new formula).
    s = orbit_stream_full(12345, 300)
    assert len(s) == 300, len(s)
    print(f"orbit_stream_full(12345, 300) OK: {len(s)} words, no crash")
    print("ORACLE self-check: PASS")
