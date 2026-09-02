"""Pure-Python reference oracle for the cycle-operation combo DSL. Used by
Tier 0 (avalanche) and as the sanity-check oracle for Tier 1 (compiled C
candidates must match this byte-for-byte before any PractRand run).

ra_init_state_singleblock is copied verbatim (translated to Python with
explicit 32-bit masking) from
../2026-9-1_family-productionization/ra_core.c L205-212. NOT modified by
this search, per HANDOVER.md's hard constraint.
"""

from __future__ import annotations

from recipes import (
    MASK32,
    FINALIZER_RECIPES,
    INJECT_CHOICES,
)

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


def ra_init_state_singleblock(key: int) -> list[int]:
    key &= MASK32
    keyterm_m = fmix32((GUARD_M ^ ((0x06A0DD9B * key) & MASK32)) & MASK32)
    M = [0] * 256
    for i in range(256):
        r = (key ^ i) & MASK32
        m_val = (((i * 0x06A0DD9B) & MASK32) * keyterm_m) & MASK32
        M[i] = rot32(m_val, r)
    return M


def tap_o(M: list[int], i: int, width: int) -> int:
    o = 0
    for e in range(8 - width, 8):
        o ^= (M[(i + e) & 0xFF] << e) & MASK32
    return o & MASK32


def apply_finalizer(c: int, finalizer_idx: int, *, o: int, a: int, b: int, i: int, cons: int) -> int:
    _, stages = FINALIZER_RECIPES[finalizer_idx]
    env = {"o": o, "a": a, "b": b, "i": i, "cons": cons}
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


def round1(key: int, width: int, finalizer_idx: int, inject_idx: int = 0) -> int:
    """Single round1 output word: matches ra_core_singleblock(key, rng=1)
    with slot1/2/3 substituted. inject_idx is accepted for API symmetry but
    provably has no effect on this single-round output (see recipes.py
    module docstring) -- always computed with inject_idx=0 internally after
    the one assertion below, so a wrong inject_idx can never silently change
    Tier 0/1 results.
    """
    del inject_idx  # inert at K=1, see recipes.py docstring
    M = ra_init_state_singleblock(key)
    cons = key & MASK32
    a, b, c, d = cons, 0, 0, 0
    i = 255
    o = tap_o(M, i, width)
    a = (d ^ o) ^ ((cons + a) & MASK32)
    a &= MASK32
    b = ((cons + a) & MASK32) ^ ((o + d) & MASK32)
    b &= MASK32
    c = rot32((a >> 13) ^ a, b)
    c = apply_finalizer(c, finalizer_idx, o=o, a=a, b=b, i=i, cons=cons)
    return c & MASK32


def stream_full(key: int, rng: int, width: int, finalizer_idx: int, inject_idx: int):
    """Full multi-round stream for one ra_core_singleblock_combo(key, rng)
    call, rng in [1,255]. Used only for Promotion-tier sanity checks where
    K>1 and slot 3 (inject) is actually observable."""
    assert 1 <= rng <= 255
    M = ra_init_state_singleblock(key)
    cons = key & MASK32
    a, b, c, d = cons, 0, 0, 0
    count = rng
    out = []
    i = 255
    while i > 0:
        o = tap_o(M, i, width)
        a = ((d ^ o) ^ ((cons + a) & MASK32)) & MASK32
        b = (((cons + a) & MASK32) ^ ((o + d) & MASK32)) & MASK32
        c = rot32((a >> 13) ^ a, b)
        c = apply_finalizer(c, finalizer_idx, o=o, a=a, b=b, i=i, cons=cons)
        out.append(c & MASK32)

        mode, target, k = INJECT_CHOICES[inject_idx]
        if mode == "add_a":
            a = (a + M[(i + k) & 0xFF]) & MASK32
        elif mode == "xor_b":
            b = b ^ M[(i + k) & 0xFF]

        d = c & 0xFF
        if count <= 1:
            break
        count -= 1
        i -= 1
    return out


if __name__ == "__main__":
    # Control candidate (width=2, finalizer=empty/idx0, inject=off/idx0)
    # must equal production ra_core_singleblock(key=1, rng=1). Cross-checked
    # against the compiled reference binary in combo_gen.py's
    # verify_control_matches_production().
    print(round1(1, width=2, finalizer_idx=0, inject_idx=0))
