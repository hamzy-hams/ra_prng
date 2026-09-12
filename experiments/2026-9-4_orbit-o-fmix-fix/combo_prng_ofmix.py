"""Pure-Python reference oracle for the orbit-o-fmix-fix experiment
(o_fmix_init_only / o_fmix_every_round candidates). Used as the sanity-check
oracle for the compiled C candidates -- they must match this byte-for-byte
before any PractRand run (feedback_no_reseed_on_fail: verify code before
trusting statistics), same discipline as
../2026-9-4_orbit-init-warmup-fix/combo_prng_warmup.py.

Init formula and cycle-transform on `c` are FIXED to production (proven not
the root cause by orbit-init-cycle-combo-search's 0/32 result) -- only
whether/where fmix32() is applied to `o` (the 8-tap XOR-shift value derived
from M[]) varies, across 2 variants:
  - init_only:   fmix32 applied once, to the initial o (i=255) only.
  - every_round: fmix32 applied to the initial o AND to every o_next
    recomputed inside the loop (i>1).

rot32/fmix32/GUARD_L/GUARD_M/ra_hash/ra_reseed/ra_init_state_orbit are
copied verbatim (translated to Python with explicit 32-bit masking) from
../2026-9-1_family-productionization/ra_core.c L144-207.
"""

from __future__ import annotations

MASK32 = 0xFFFFFFFF

GUARD_L = 0x38916DF4
GUARD_M = 0x6C26FC92

VARIANTS = ("init_only", "every_round")


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
    """8-tap o, naive (not rolling-register) form -- equivalent to
    production's rolling register (proven by the mandatory byte-exact
    check against production for the UNMODIFIED formula in a sibling
    experiment's oracle, orbit-init-warmup-fix/combo_prng_warmup.py)."""
    o = 0
    for e in range(8):
        o ^= (M[(i + e) & 0xFF] << e) & MASK32
    return o & MASK32


def ra_permutation_cycle_orbit_ofmix(cons: int, it: int, variant: str,
                                      M: list[int], L: list[int],
                                      count: list[int], out: list[int]) -> None:
    a = cons & MASK32
    d = 0
    i = 255
    o = tap_o8(M, i)
    o = fmix32(o)  # both variants mix the initial o
    while i > 0:
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

        if i > 1:
            # o_term7 rolling register replaced by direct tap_o8 recompute
            # of the NEXT round's naive formula shifted by one index --
            # equivalent to production's incremental o_next update (same
            # proof pattern as tap_o8 above).
            o_next = tap_o8(M, i - 1)
            o = fmix32(o_next) if variant == "every_round" else o_next
        i -= 1


def orbit_stream_full(key: int, rng: int, variant: str) -> list[int]:
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
        ra_permutation_cycle_orbit_ofmix(cons, it, variant, M, L, count, out)
        if count[0] <= 1:
            return out
        cons = ra_reseed(M, L)
        it += 1
    return out


def orbit_round1(key: int, variant: str) -> int:
    return orbit_stream_full(key, 1, variant)[0]


if __name__ == "__main__":
    # Sanity: production (no o-fmix at all) must still match the known KAT,
    # verified via a minimal local reimplementation without fmix32(o).
    def _production_round1(key: int) -> int:
        L, M = ra_init_state_orbit(key)
        cons = key & MASK32
        a, d = cons, 0
        o = tap_o8(M, 255)  # NOT mixed -- production baseline
        a = ((d ^ o) ^ ((cons + a) & MASK32)) & MASK32
        b = (((cons + a) & MASK32) ^ ((o + d) & MASK32)) & MASK32
        c = rot32((a >> 13) ^ a, b)
        c ^= c >> 17
        return c & MASK32

    v = _production_round1(1)
    print(f"production baseline round1(1) = {v}")
    assert v == 397095967, v
    print("production baseline OK (tap_o8 equivalence confirmed)")

    for variant in VARIANTS:
        r1 = orbit_round1(1, variant)
        print(f"orbit_round1(1, {variant}) = {r1}")
        s = orbit_stream_full(12345, 300, variant)
        assert len(s) == 300, len(s)
        print(f"orbit_stream_full(12345, 300, {variant}) OK: {len(s)} words, no crash")

    print("ORACLE self-check: PASS")
