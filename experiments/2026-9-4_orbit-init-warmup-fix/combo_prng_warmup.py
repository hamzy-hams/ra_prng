"""Pure-Python reference oracle for the orbit init-warmup fix experiment
(HANDOVER.md). Used as the sanity-check oracle for compiled C candidates
(combo_gen_warmup.py) -- they must match this byte-for-byte before any
PractRand run, same discipline as
../2026-9-4_orbit-init-cycle-combo-search/combo_prng_orbit.py.

Unlike the sibling combo-search, init-formula and cycle-transform are FIXED
to production (proven not the root cause by that session's 0/32 result) --
only the seed values of `a`/`d` at it==0 vary, across 3 fixed variants:
control (production, d0=0), d0_hash (Candidate A), d0_realround (Candidate B).

rot32/fmix32/GUARD_L/GUARD_M/ra_hash/ra_reseed and the init/cycle formulas
are copied verbatim (translated to Python with explicit 32-bit masking) from
../2026-9-1_family-productionization/ra_core.c L144-251.

GUARD_D provenance: picked by pick_guard_constant_d.py, popcount==16,
sha256 attempt #24, distinct from GUARD_L/GUARD_M/the 4 pre-existing
constants -- see that script's output.
"""

from __future__ import annotations

MASK32 = 0xFFFFFFFF

GUARD_L = 0x38916DF4
GUARD_M = 0x6C26FC92
GUARD_D = 0x17067F32  # popcount=16, sha256 attempt #24 (pick_guard_constant_d.py)

VARIANTS = ("control", "d0_hash", "d0_realround")


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
    """Mutates M in place (M[i] ^= L[i]) and returns the new cons, exactly
    mirroring ra_core.c's ra_reseed. it>=1 keeps d0=0 by design (HANDOVER
    §2), so this path is byte-identical to production for it>=1."""
    for i in range(256):
        M[i] = (M[i] ^ L[i]) & MASK32
    tmp8 = ra_hash(M)
    new_cons = 0
    for e in range(8):
        new_cons ^= (tmp8[e] << e) & MASK32
    return new_cons & MASK32


# ---------------------------------------------------------------------
# Init formula: FIXED to production (init_linear_shared), not varied.
# ---------------------------------------------------------------------


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
    """8-tap o, recomputed naively -- proven equal to production's rolling
    register by the mandatory control byte-exact check (see §7 step 1)."""
    o = 0
    for e in range(8):
        o ^= (M[(i + e) & 0xFF] << e) & MASK32
    return o & MASK32


# ---------------------------------------------------------------------
# Warmup: computes (a0, d0) fed into round it==0 only. it>=1 is untouched
# by design (a=cons, d=0), so warmup() is never called for it>=1.
# ---------------------------------------------------------------------


def warmup(key: int, M: list[int], variant: str) -> tuple[int, int]:
    key &= MASK32
    if variant == "control":
        return key, 0
    if variant == "d0_hash":
        d0 = fmix32(GUARD_D ^ key) & 0xFF
        return key, d0
    if variant == "d0_realround":
        o255 = tap_o8(M, 255)
        a_warm = ((0 ^ o255) ^ ((key + key) & MASK32)) & MASK32
        b_warm = (((key + a_warm) & MASK32) ^ ((o255 + 0) & MASK32)) & MASK32
        c_warm = rot32((a_warm >> 13) ^ a_warm, b_warm)
        c_warm ^= c_warm >> 17
        c_warm &= MASK32
        d0 = c_warm & 0xFF
        return a_warm, d0
    raise KeyError(variant)


def ra_permutation_cycle_orbit_warmup(cons: int, it: int, a0: int, d0: int,
                                       M: list[int], L: list[int],
                                       count: list[int], out: list[int]) -> None:
    """Mirrors ra_core.c's ra_permutation_cycle_orbit exactly, except `a`/`d`
    are seeded from a0/d0 when it==0 (else cons/0 as production does)."""
    a = a0 if it == 0 else (cons & MASK32)
    d = d0 if it == 0 else 0
    i = 255
    while i > 0:
        o = tap_o8(M, i)
        a = ((d ^ o) ^ ((cons + a) & MASK32)) & MASK32
        b = (((cons + a) & MASK32) ^ ((o + d) & MASK32)) & MASK32
        c = rot32((a >> 13) ^ a, b)
        c ^= c >> 17  # w8_f10_i0, production finalizer, fixed (not varied)
        c &= MASK32
        out.append(c)

        d = c & 0xFF
        if count[0] <= 1:
            break
        count[0] -= 1

        L[i], L[d] = L[d], L[i]
        i -= 1


def orbit_stream_full(key: int, rng: int, variant: str) -> list[int]:
    """Full ra_core_orbit_combo(key, rng, variant, ...) output stream."""
    if rng == 0:
        return []
    key &= MASK32
    L, M = ra_init_state_orbit(key)
    a0, d0 = warmup(key, M, variant)
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

    expected_stream37 = [
        670168745, 3936143691, 3608548021, 1722067444, 2825167964, 306529444,
        1273708769, 1261272265, 1652874764, 1520180321, 2531340418, 2572871808,
        4084899721, 1569101729, 690356714, 449364945, 237328769, 2018141994,
        3151204448, 2001952962, 3080562412, 627326989, 129591293, 970466891,
        3764253286, 1380135954, 3243837900, 3913715010, 3721160998, 459735621,
        2585413588, 225605702, 3668554893, 1025168546, 3910409673, 4102811069,
        284556126,
    ]
    s = orbit_stream_full(12345, 37, "control")
    assert s == expected_stream37, (s[:5], expected_stream37[:5])
    print(f"orbit_stream_full(12345, 37, control) OK: matches production, {len(s)} words")

    for variant in ("d0_hash", "d0_realround"):
        a0, d0 = warmup(1, ra_init_state_orbit(1)[1], variant)
        print(f"warmup(key=1, {variant}) -> a0={a0}, d0={d0}")
        s2 = orbit_stream_full(12345, 300, variant)
        assert len(s2) == 300, len(s2)
        print(f"orbit_stream_full(12345, 300, {variant}) OK: {len(s2)} words, no crash")

    print("ORACLE self-check: PASS")
