"""Operation-flag-parameterized reimplementation of ra_prng2's permutation
cycle + reseed, at full scale (n=256, w=32).

Ported from src/ra_prng2/python/ra_prng2.py (the paper-exact reference,
untouched) and experiments/others/avalanche_effect_analysis.py's
zepfold_capture() (the existing full-scale c-sequence-capture pattern).

This file is the sandbox: it is allowed to change output. It must NOT be
confused with the reference -- run this module directly (`python
pruned_prng.py`) to self-check that ALL_OPS reproduces the reference
bit-for-bit before trusting any ablation result.
"""

from __future__ import annotations

from dataclasses import dataclass, field

from operations import (
    ALL_FLAGS, HASH_DEPTH_FIXED, HASH_ACCESS_DEFAULT, HASH_ACCESS_STRIDED,
    HASH_ACCESS_SEQUENTIAL, MULT_REDUCE, HASH_SELFIDX,
    ROT_A, ROT_B, ROT_O, ROT_C, SHL9, SHR18, SHL14, SHR13,
)

MASK = 0xFFFFFFFF


def rot32(n: int, r: int) -> int:
    r &= 31
    n &= MASK
    return ((n << r) | (n >> (32 - r))) & MASK


DEFAULT_SHIFT_WIDTHS = (9, 18, 14, 13)  # (SHL9, SHR18, SHL14, SHR13), original values


@dataclass(frozen=True)
class Candidate:
    """One ablation candidate: which flags are active, and the hash access
    pattern. HASH_DEPTH is deliberately not a field here -- it is fixed at
    32 everywhere (see operations.py's HASH_DEPTH_FIXED docstring for why).

    shift_widths: (s9, s18, s14, s13) -- the actual shift amounts used by
    whichever of SHL9/SHR18/SHL14/SHR13 are active in `ops`. Defaults to the
    original algorithm's constants. Only meaningful for flags that are ON;
    an OFF flag's width value is irrelevant (that XOR term is dropped
    entirely regardless of width, per operations.py's "off" semantics).
    Used for the post-convergence "shift repair" search: with the winning
    candidate's op set held fixed, scan these widths for the point of
    maximum avalanche diffusion ("chaos"), independent of which ops are on.
    """
    ops: frozenset[str] = field(default_factory=lambda: frozenset(ALL_FLAGS))
    hash_access: str = HASH_ACCESS_DEFAULT
    shift_widths: tuple[int, int, int, int] = DEFAULT_SHIFT_WIDTHS

    def has(self, flag: str) -> bool:
        return flag in self.ops


def ra_hash(N: list[int], cand: Candidate) -> list[int]:
    out = [0] * 8
    for i in range(8):
        if cand.has(HASH_SELFIDX):
            out[i] ^= N[N[i] & 0xFF]
        if cand.hash_access == HASH_ACCESS_SEQUENTIAL:
            base = i * HASH_DEPTH_FIXED
            for j in range(HASH_DEPTH_FIXED):
                out[i] ^= N[base + j]
        else:  # HASH_ACCESS_STRIDED (original)
            for j in range(HASH_DEPTH_FIXED):
                out[i] ^= N[j * 8 + i]
    return out


def init_state():
    M = [0] * 256
    L = [0] * 256
    for i in range(256):
        M[i] = (i * 0x06a0dd9b + 0x06a0dd9b) & MASK
        L[i] = (i * 0x9e3779b7 + 0x9e3779b7) & MASK
    return L, M


def _build_o(M: list[int], i: int, cand: Candidate) -> int:
    o = 0
    for e in range(8):
        if cand.has(f"TAP{e}"):
            o ^= (M[(i + e) & 0xFF] << e) & MASK
    return o


def permutation_cycle(cons: int, it: int, M: list[int], L: list[int], cand: Candidate):
    """One full 255-step permutation cycle. Yields each step's `c` (the real
    per-step RNG output) and mutates L in place via the swap, mirroring
    ra_permutation_cycle in src/ra_prng2/c/ra_prng2.c exactly when
    cand == ALL_OPS.
    """
    a = cons
    b = it
    c = 0
    d = 0

    for i in range(255, 0, -1):
        o = _build_o(M, i, cand)

        raw_a = (b ^ o) & MASK
        a = (rot32(raw_a, d) if cand.has(ROT_A) else raw_a) ^ ((cons + a) & MASK)
        a &= MASK

        raw_b = (cons + a) & MASK
        b = (rot32(raw_b, i) if cand.has(ROT_B) else raw_b) ^ ((o + d) & MASK)
        b &= MASK

        s9, s18, s14, s13 = cand.shift_widths
        rotated_o = rot32(a ^ o, i) if cand.has(ROT_O) else (a ^ o)
        left = ((rotated_o << s9) & MASK) if cand.has(SHL9) else 0
        right = (b >> s18) if cand.has(SHR18) else 0
        if cand.has(SHL9) and cand.has(SHR18):
            o = left ^ right
        elif cand.has(SHL9):
            o = left
        elif cand.has(SHR18):
            o = right
        else:
            o = 0

        term_oc = ((o + c) << s14) & MASK if cand.has(SHL14) else 0
        term_b13 = (b >> s13) if cand.has(SHR13) else 0
        pre_rot_c = term_oc ^ term_b13 ^ a
        c = (rot32(pre_rot_c, b) if cand.has(ROT_C) else pre_rot_c) & MASK

        yield c

        if cand.has(MULT_REDUCE):
            d = (c * (i + 1)) >> 32
        else:
            d = c & 0xFF

        L[i], L[d] = L[d], L[i]


def reseed(M: list[int], L: list[int], cand: Candidate) -> int:
    for i in range(256):
        M[i] ^= L[i]

    tmp8 = ra_hash(M, cand)
    new_cons = 0
    for e in range(8):
        new_cons ^= (tmp8[e] << e) & MASK
    return new_cons


def stream(seed: int, iterations: int, cand: Candidate):
    """Yield every `c` value across `iterations` full permutation cycles,
    reseeding between each -- the same shape as ra_core's outer loop, minus
    the mid-cycle truncation-count logic (which only matters for the last
    partial cycle of a specific `rng` total, irrelevant for quality testing
    a long stream).
    """
    L, M = init_state()
    cons = seed
    for it in range(iterations):
        yield from permutation_cycle(cons, it, M, L, cand)
        cons = reseed(M, L, cand)


def final_cons(seed: int, iterations: int, cand: Candidate) -> int:
    cons = seed
    L, M = init_state()
    for it in range(iterations):
        for _ in permutation_cycle(cons, it, M, L, cand):
            pass
        cons = reseed(M, L, cand)
    return cons


def _self_check():
    """ALL_OPS must reproduce src/ra_prng2/python/ra_prng2.py bit-for-bit."""
    import sys
    sys.path.insert(0, "../../src/ra_prng2/python")
    from ra_prng2 import ra_core as reference_ra_core

    all_ops = Candidate()
    for seed, iterations in [(1, 1), (1, 2), (42, 3), (0xDEADBEEF, 1)]:
        expected = reference_ra_core(seed, iterations)
        got = final_cons(seed, iterations, all_ops)
        status = "OK" if expected == got else "MISMATCH"
        print(f"seed={seed} iterations={iterations}: expected={expected} got={got} [{status}]")
        assert expected == got, "pruned_prng.py with ALL_OPS must match the paper-exact reference"
    print("Self-check passed: ALL_OPS is bit-identical to src/ra_prng2/python/ra_prng2.py")


if __name__ == "__main__":
    _self_check()
