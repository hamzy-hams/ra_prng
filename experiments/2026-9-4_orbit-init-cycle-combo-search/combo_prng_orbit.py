"""Pure-Python reference oracle for the orbit init-formula x cycle-transform
combo DSL (HANDOVER.md). Used as the sanity-check oracle for compiled C
candidates (combo_gen_orbit.py) -- they must match this byte-for-byte
before any PractRand run, same discipline as
../2026-9-2_singleblock-cycle-combo-search/combo_prng.py.

rot32/fmix32/GUARD_L/GUARD_M/ra_hash/ra_reseed are copied verbatim
(translated to Python with explicit 32-bit masking) from
../2026-9-1_family-productionization/ra_core.c L144-183. NOT modified by
this search, per HANDOVER.md's hard constraint (axis 1/2 only touch
ra_init_state_orbit / ra_permutation_cycle_orbit).
"""

from __future__ import annotations

from recipes_orbit import INIT_VARIANTS, CYCLE_VARIANTS

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
    """Mutates M in place (M[i] ^= L[i]) and returns the new cons, exactly
    mirroring ra_core.c's ra_reseed. Not exercised at rng<=255 (see
    combo_gen_orbit.py's module docstring for why), kept for fidelity /
    future Promotion-tier (rng>255) sanity."""
    for i in range(256):
        M[i] = (M[i] ^ L[i]) & MASK32
    tmp8 = ra_hash(M)
    new_cons = 0
    for e in range(8):
        new_cons ^= (tmp8[e] << e) & MASK32
    return new_cons & MASK32


# ---------------------------------------------------------------------
# Axis 1: init-formula variants
# ---------------------------------------------------------------------

IDX_FN = {
    "linear": lambda i: i,
    "linear_p1": lambda i: i + 1,
    "odd": lambda i: (2 * i + 1) & MASK32,
    "hashed": lambda i: fmix32(i) | 1,
}


def _init_variant_lookup(init_id: str) -> tuple[str, str]:
    for vid, idx_name, keyterm_mode in INIT_VARIANTS:
        if vid == init_id:
            return idx_name, keyterm_mode
    raise KeyError(init_id)


def ra_init_state_orbit_variant(key: int, init_id: str) -> tuple[list[int], list[int]]:
    key &= MASK32
    idx_name, keyterm_mode = _init_variant_lookup(init_id)
    idxf = IDX_FN[idx_name]
    L = [0] * 256
    M = [0] * 256
    if keyterm_mode == "shared":
        keyterm_l = fmix32(GUARD_L ^ ((0x9E3779B7 * key) & MASK32))
        keyterm_m = fmix32(GUARD_M ^ ((0x06A0DD9B * key) & MASK32))
    for i in range(256):
        r = (key ^ i) & MASK32
        ix = idxf(i) & MASK32
        if keyterm_mode == "shared":
            l_val = (((ix * 0x9E3779B7) & MASK32) * keyterm_l) & MASK32
            m_val = (((ix * 0x06A0DD9B) & MASK32) * keyterm_m) & MASK32
        else:  # per_index
            l_val = fmix32(GUARD_L ^ ((0x9E3779B7 * key) & MASK32) ^ ix)
            m_val = fmix32(GUARD_M ^ ((0x06A0DD9B * key) & MASK32) ^ ix)
        L[i] = rot32(l_val, r)
        M[i] = rot32(m_val, r)
    return L, M


# ---------------------------------------------------------------------
# Axis 2: cycle-transform variants
# ---------------------------------------------------------------------


def _cycle_variant_lookup(cycle_id: str):
    for cid, _, stages in CYCLE_VARIANTS:
        if cid == cycle_id:
            return stages
    raise KeyError(cycle_id)


def apply_cycle(c: int, cycle_id: str, *, a: int, b: int, i: int, cons: int, o: int) -> int:
    stages = _cycle_variant_lookup(cycle_id)
    env = {"a": a, "b": b, "i": i, "cons": cons, "o": o}
    for st in stages:
        if st.kind == "MUL":
            c = (c * st.param) & MASK32
        elif st.kind == "XORSHIFT":
            c ^= c >> st.param
        elif st.kind == "ROT":
            c = rot32(c, env[st.param])
        elif st.kind == "ADD":
            c = (c + env[st.param]) & MASK32
        else:
            raise ValueError(st.kind)
    return c & MASK32


def tap_o8(M: list[int], i: int) -> int:
    """8-tap o, recomputed naively every iteration. Production's
    ra_permutation_cycle_orbit maintains this via an incremental rolling
    register (a GF(2)-linear identity, documented bit-exact in ra_core.c),
    but the naive recompute is legal here -- proven equal by the mandatory
    control-combo byte-exact check in combo_gen_orbit.py."""
    o = 0
    for e in range(8):
        o ^= (M[(i + e) & 0xFF] << e) & MASK32
    return o & MASK32


def ra_permutation_cycle_orbit_variant(cons: int, it: int, M: list[int], L: list[int],
                                        count: list[int], cycle_id: str, out: list[int]) -> None:
    """Mutates L in place (swaps) and count[0] in place (decrement), appends
    emitted words to out. Mirrors ra_core.c's ra_permutation_cycle_orbit
    exactly, including the pre-decrement break (last word never triggers an
    L[] swap)."""
    a = cons & MASK32
    d = 0
    i = 255
    while i > 0:
        o = tap_o8(M, i)
        a = ((d ^ o) ^ ((cons + a) & MASK32)) & MASK32
        b = (((cons + a) & MASK32) ^ ((o + d) & MASK32)) & MASK32
        c = rot32((a >> 13) ^ a, b)
        c = apply_cycle(c, cycle_id, a=a, b=b, i=i, cons=cons, o=o)
        out.append(c & MASK32)

        d = c & 0xFF
        if count[0] <= 1:
            break
        count[0] -= 1

        L[i], L[d] = L[d], L[i]
        i -= 1


def orbit_stream_full(key: int, rng: int, init_id: str, cycle_id: str) -> list[int]:
    """Full ra_core_orbit(key, rng, ...) output stream. rng<=255 -> single
    ra_permutation_cycle_orbit_variant call (reseed dead code, see
    ra_reseed's docstring). rng>255 -> reseed loop exercised, kept general
    for future Promotion-tier oracle use even though this session's Tier0/1
    only ever calls with rng in {1, 37}."""
    if rng == 0:
        return []
    key &= MASK32
    L, M = ra_init_state_orbit_variant(key, init_id)
    count = [rng]
    cons = key
    iteration = rng // 255 + 1
    out: list[int] = []
    it = 0
    while it < iteration:
        ra_permutation_cycle_orbit_variant(cons, it, M, L, count, cycle_id, out)
        if count[0] <= 1:
            return out
        cons = ra_reseed(M, L)
        it += 1
    return out


def orbit_round1(key: int, init_id: str, cycle_id: str) -> int:
    return orbit_stream_full(key, 1, init_id, cycle_id)[0]


if __name__ == "__main__":
    # Control candidate (init_linear_shared, w8_f10_i0) must equal
    # production ra_core_orbit(key=1, rng=1) = 397095967 (cross-checked
    # against the compiled ra_core binary, see combo_gen_orbit.py).
    v = orbit_round1(1, "init_linear_shared", "w8_f10_i0")
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
    s = orbit_stream_full(12345, 37, "init_linear_shared", "w8_f10_i0")
    assert s == expected_stream37, (s[:5], expected_stream37[:5])
    print(f"orbit_stream_full(12345, 37, control) OK: matches production, {len(s)} words")
    print("ORACLE vs PRODUCTION: PASS")
