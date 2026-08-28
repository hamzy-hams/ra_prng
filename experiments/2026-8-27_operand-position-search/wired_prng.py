"""Operand-position-parameterized reimplementation of `baseline.c`'s
(`pruned_winner`'s) permutation cycle -- same fixed operation set
(TAP6, TAP7, ROT_C, SHR13; hash_access=sequential) as
../2026-8-26_operation-pruning-research/pruned_prng.py's
`Candidate(ops={TAP6,TAP7,ROT_C,SHR13}, hash_access=HASH_ACCESS_SEQUENTIAL)`,
but with the a/b/c update chain's operand assignments parameterized by a
`Wiring` (see wiring.py) instead of fixed.

`DEFAULT_WIRING` must reproduce `baseline.c` / that Candidate bit-for-bit --
run this module directly (`python wired_prng.py`) to self-check before
trusting any rewiring result.
"""

from __future__ import annotations

from wiring import Wiring, DEFAULT_WIRING

MASK = 0xFFFFFFFF


def rot32(n: int, r: int) -> int:
    r &= 31
    n &= MASK
    return ((n << r) | (n >> (32 - r))) & MASK


def init_state():
    M = [0] * 256
    L = [0] * 256
    for i in range(256):
        M[i] = (i * 0x06a0dd9b + 0x06a0dd9b) & MASK
        L[i] = (i * 0x9e3779b7 + 0x9e3779b7) & MASK
    return L, M


def permutation_cycle(cons: int, it: int, M: list[int], L: list[int], w: Wiring):
    """One full 255-step permutation cycle under wiring `w`. Yields each
    step's `c`, mutates L in place via the swap -- same shape as
    pruned_prng.py's permutation_cycle, but rewired per `w` instead of
    ablated per a Candidate's flags. Op set is always {TAP6,TAP7,ROT_C,
    SHR13}, unconditionally -- no on/off branching here, that axis is
    orthogonal and belongs to the sibling pruning experiment.
    """
    a = cons & MASK
    b = it & MASK
    d = 0

    for i in range(255, 0, -1):
        o = ((M[(i + 6) & 0xFF] << 6) ^ (M[(i + 7) & 0xFF] << 7)) & MASK

        pre = {"a": a, "b": b, "d": d, "i": i & MASK}
        a = (pre[w.a_xor_operand] ^ o) ^ ((cons + a) & MASK)
        a &= MASK

        b = ((cons + a) & MASK) ^ ((o + d) & MASK)
        b &= MASK

        post = {"a": a, "b": b, "d": d, "i": i & MASK, "o": o}
        shifted = post[w.c_shift_operand] >> 13
        pre_rot = (shifted ^ post[w.rotc_xor_operand]) & MASK
        amount = post[w.rotc_amount_source]
        c = rot32(pre_rot, amount)

        yield c

        d = c & 0xFF
        L[i], L[d] = L[d], L[i]


def reseed(M: list[int], L: list[int]) -> int:
    """Mirrors baseline.c's ra_reseed()/ra_hash() exactly (HASH_ACCESS
    sequential, HASH_DEPTH=32, no HASH_SELFIDX) -- unaffected by wiring,
    since the rewiring scope is limited to ra_permutation_cycle's a/b/c
    chain (see wiring.py)."""
    for i in range(256):
        M[i] ^= L[i]
    new_cons = 0
    for e in range(8):
        base = e * 32
        h = 0
        for j in range(32):
            h ^= M[base + j]
        new_cons ^= (h << e) & MASK
    return new_cons & MASK


def stream(seed: int, iterations: int, w: Wiring = DEFAULT_WIRING):
    L, M = init_state()
    cons = seed
    for it in range(iterations):
        yield from permutation_cycle(cons, it, M, L, w)
        cons = reseed(M, L)


def final_cons(seed: int, iterations: int, w: Wiring = DEFAULT_WIRING) -> int:
    cons = seed
    L, M = init_state()
    for it in range(iterations):
        for _ in permutation_cycle(cons, it, M, L, w):
            pass
        cons = reseed(M, L)
    return cons


def _self_check():
    """DEFAULT_WIRING must reproduce baseline.c / pruned_prng.py's
    Candidate(ops={TAP6,TAP7,ROT_C,SHR13}, hash_access=sequential)
    bit-for-bit."""
    import sys
    from pathlib import Path
    sys.path.insert(0, str(Path(__file__).parent.parent /
                           "2026-8-26_operation-pruning-research"))
    from pruned_prng import Candidate, final_cons as pruned_final_cons, \
        HASH_ACCESS_SEQUENTIAL

    pruned_cand = Candidate(ops=frozenset({"TAP6", "TAP7", "ROT_C", "SHR13"}),
                             hash_access=HASH_ACCESS_SEQUENTIAL)
    for seed, iterations in [(1, 1), (1, 2), (1, 3), (42, 3), (0xDEADBEEF, 1)]:
        expected = pruned_final_cons(seed, iterations, pruned_cand)
        got = final_cons(seed, iterations, DEFAULT_WIRING)
        status = "OK" if expected == got else "MISMATCH"
        print(f"seed={seed} iterations={iterations}: expected={expected} got={got} [{status}]")
        assert expected == got, "DEFAULT_WIRING must match baseline.c / pruned_winner exactly"
    print("Self-check passed: DEFAULT_WIRING is bit-identical to baseline.c / pruned_winner.")


if __name__ == "__main__":
    _self_check()
