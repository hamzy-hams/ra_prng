"""Toy-scale (n, w, rows)-parameterized port of winner_wired_v2.c's
permutation cycle.

Child of TWO prior experiments:
  - ../2026-8-25_periodicity-heuristic-validation/ (the periodicity heuristic
    itself, and toy_prng.py, whose Params/rotw/rescale_shifts/truncate_const/
    group_size/init_state/state_key are REUSED UNCHANGED here via import --
    only next_state and the hash-reduction function differ).
  - ../2026-8-27_operand-position-search/ (winner_wired_v2.c, the greedy-
    ablation + operand-rewiring winner this module ports to toy scale), which
    itself builds on ../2026-8-26_operation-pruning-research/'s ablation
    search (ops={TAP6,TAP7,ROT_C,SHR13} of 18 tracked ops).

Op set kept (of 18 tracked flags in operations.py): TAP6, TAP7, ROT_C,
SHR13 only. Everything else (TAP0-5, ROT_A, ROT_B, ROT_O, SHL9, SHR18,
SHL14, MULT_REDUCE, HASH_SELFIDX) is off. HASH_ACCESS=sequential (not the
original's strided default). Wiring (see wiring.py in the 2026-8-27
folder): a_xor_operand=d (default is b), c_shift_operand=a (default is b),
rotc_amount_source=b (default), rotc_xor_operand=a (default).

Resulting per-step chain (verified directly against winner_wired_v2.c):
    o = M[(i+top1)&(n-1)]<<top1 ^ M[(i+top2)&(n-1)]<<top2   # only the 2
                                                              # highest-index
                                                              # taps of G
    a = (d ^ o) ^ (cons + a)          # a_xor=d; ROT_A off (no rotate)
    b = (cons + a) ^ (o + d)          # ROT_B off (no rotate)
    c = rotw((a>>S13) ^ a, b, w)      # c_shift=a, rotc_xor=a (both post-
                                       # update a, confirmed not a bug --
                                       # see wired_prng.py's `post` dict);
                                       # SHL14 dropped entirely, c's own
                                       # previous value plays no role;
                                       # rotc_amount=b (post-update b)
    d = c & (n-1)                      # MULT_REDUCE off -> plain AND,
                                       # generalized 0xFF -> (n-1)

Reseed (once per full (n-1)-step cycle):
    M[i] ^= L[i] for all i
    out[i] = XOR of M[i*rows .. i*rows+rows-1]   for i in range(G)
    new_cons = XOR_e (out[e] << e)

IMPORTANT DEVIATION FROM toy_prng.py's ra_hash_gen (documented finding, see
HANDOVER.md "Design decisions" #1): the real ra_hash (src/ra_prng2/python/
ra_prng2.py) and winner_wired_v2.c's ra_hash are both PURE, READ-ONLY
functions of M (M is never mutated inside the hash step -- only the earlier
`M[i] ^= L[i]` touches M). toy_prng.py's ra_hash_gen, by contrast, mutates
its `N` argument in place (`N[i] ^= N[j*G+i]`) and that mutated-into-N value
is DISCARDED -- `out[i]` there only ever receives the HASH_SELFIDX term.
This is a real structural divergence from ground truth in the sibling
experiment's baseline model, not something this module should copy. This
module's ra_hash_gen_sequential is instead a direct, pure-function port of
winner_wired_v2.c's actual ra_hash.

IMPORTANT CONSTRAINT (caught during design, not present in either sibling
experiment): tap term e contributes `(M[idx] << e) & mask` -- a PLAIN left
shift, not a rotate. For any e >= w, every bit of M[idx] is shifted out of
the low w bits before the mask is applied, so the term is IDENTICALLY ZERO
-- not a subtle statistical weakening but a hard structural collapse (o's
tap-XOR loop contributes nothing at all). The real algorithm never hits
this (w=32, e<=7). At toy scale this caps which (n, w, rows) configs are
usable: the surviving taps are e in {G-2, G-1} (or {0} at G=1), so any
config needs G <= w. This RULES OUT the naive "G=8, exact TAP6/TAP7 match"
config at w=4 (e=6,7 both >= 4 -> zero); a genuine G=8 match needs w>=8,
which forces log2|S| into the already-documented-infeasible range (see
run_sweep.py's INFEASIBLE list and HANDOVER.md). next_state() asserts
max(survivors) < w and raises rather than silently returning a collapsed
result.

Note also: as G approaches w, low-index bits of M[idx] survive the shift
but many high-index bits do not (e.g. at w=4, e=3 keeps only M[idx]'s
bit 0, shifted into position 3 -- 3 of 4 bits are lost). This is a
quantifiable degradation, not a collapse (nonzero, data-dependent), and is
noted in HANDOVER.md rather than treated as disqualifying.
"""

from __future__ import annotations

import sys
from pathlib import Path

# append (NOT insert(0, ...)): the sibling folder has same-named files
# (cycle_measure.py, avalanche_check.py, run_sweep.py, enumerate_*.py) --
# inserting at the front would silently shadow THIS folder's own modules
# for any later bare `import X` anywhere in the process (a real bug caught
# during this session: it made cycle_measure.brent() transparently run the
# sibling's unpruned toy_prng.next_state instead of this module's pruned/
# wired one, with no error, just a wrong lambda). Appending guarantees the
# current directory (always first on sys.path for a directly-run script)
# resolves first; the sibling is only a fallback for names not found
# locally (toy_prng itself, which has no local same-named counterpart).
sys.path.append(str(Path(__file__).parent.parent /
                     "2026-8-25_periodicity-heuristic-validation"))
from toy_prng import Params, init_state, state_key, rotw  # noqa: E402  (unchanged, reused as-is)

State = tuple[list[int], list[int], int, int]  # L, M, cons, it


def tap_survivors(G: int, w: int) -> tuple[int, ...]:
    """Which tap indices e (0..G-1) survive pruning, generalizing "keep
    TAP6, TAP7 of TAP0..TAP7" (top 2 of 8 by index -- TAP_e contributes
    M[...]<<e, so TAP6/TAP7 are literally the two highest-shift taps that
    were kept).

    G=1: only one tap exists at all -- forced-keep (o would be entirely
         absent otherwise); textually identical to toy_prng.py's own
         o-loop at G=1 (pruning is a structural no-op there).
    G=2: both taps ARE "the top two" -- also a structural no-op vs the
         unpruned baseline at this G.
    G>=3: real pruning -- keep e = G-2, G-1.

    Rejected alternative: proportional position (round(6G/8), round(7G/8))
    collides at G=4 (round(3.0)=3, round(3.5)=4 under round-half-to-even,
    but max valid index is G-1=3 -- both clamp to 3, cancelling to a single
    tap where two were intended) and at G=2 (both round to 1). Fixed
    top-2-by-count avoids this entirely and, at G=8, reproduces the real
    TAP6/TAP7 indices exactly -- the strongest evidence for this choice.
    """
    if G >= 2:
        survivors = (G - 2, G - 1)
    else:
        survivors = (0,)
    if max(survivors) >= w:
        raise ValueError(
            f"tap_survivors({G=}, {w=}): survivor index {max(survivors)} >= w={w} "
            f"would collapse to zero (plain left-shift, not rotate) -- this "
            f"(n,w,rows) config is not usable for the pruned/wired model."
        )
    return survivors


def ra_hash_gen_sequential(N: list[int], n: int, w: int, G: int) -> list[int]:
    """HASH_ACCESS=sequential port of winner_wired_v2.c's ra_hash: each
    output word is the XOR-fold of one contiguous `rows`-element block of
    N. Pure function -- N is READ, never mutated (see module docstring's
    "IMPORTANT DEVIATION" note for why this differs from toy_prng.py's
    ra_hash_gen). No HASH_SELFIDX term (that flag is off in winner_wired_v2).
    """
    mask = (1 << w) - 1
    assert n % G == 0, f"G={G} must evenly divide n={n}"
    rows = n // G
    out = [0] * G
    for i in range(G):
        base = i * rows
        h = 0
        for j in range(rows):
            h ^= N[base + j]
        out[i] = h & mask
    return out


def next_state(state: State, p: Params) -> State:
    L, M, cons, it = state
    L = list(L)
    M = list(M)
    mask = p.mask
    n, w, G = p.n, p.w, p.G
    S13 = p.shifts["S13"]  # only SHR13 survives of SHL9/SHR18/SHL14/SHR13
    survivors = tap_survivors(G, w)

    a = cons
    b = it
    d = 0

    for i in range(n - 1, 0, -1):
        o = 0
        for e in survivors:
            idx = (i + e) & (n - 1)
            o ^= (M[idx] << e) & mask
        # No ROT_O/SHL9/SHR18 second-stage mixing at all (all off) -- o is
        # used exactly as built above, straight into a's and b's formulas.
        # c's formula never references o at all (SHL14, the only place o
        # would appear there, is off).

        a = ((d ^ o) ^ ((cons + a) & mask)) & mask
        # a_xor_operand=d (not the default b); ROT_A off (no rotate)

        b = (((cons + a) & mask) ^ ((o + d) & mask)) & mask
        # ROT_B off (no rotate); formula shape itself is unaffected by
        # wiring (wiring.py only rewires a's xor operand and c's chain)

        shifted = (a >> S13) & mask
        # c_shift_operand=a (the just-computed, post-update a above)
        pre_rot = (shifted ^ a) & mask
        # rotc_xor_operand=a -- the SAME post-update a as the shift operand;
        # confirmed correct, not a bug (wiring.py: two independently-named
        # slots that happen to both resolve to "a" for this wiring; matches
        # wired_prng.py's `post` dict, built after a/b are reassigned)
        c = rotw(pre_rot, b, w) & mask
        # ROT_C kept, rotate amount = b (post-update b, rotc_amount=b,
        # default, unaffected by rewiring)

        d = c & (n - 1)
        # MULT_REDUCE off -> plain AND, generalized 0xFF -> (n-1)

        L[i], L[d] = L[d], L[i]

    for i in range(n):
        M[i] ^= L[i]

    cons_list = ra_hash_gen_sequential(M, n, w, G)

    new_cons = 0
    for e in range(G):
        new_cons ^= (cons_list[e] << e) & mask

    new_it = (it + 1) & mask

    return (L, M, new_cons, new_it)


if __name__ == "__main__":
    p = Params(n=4, w=8)
    print(f"Params: n={p.n} w={p.w} G={p.G} S13={p.shifts['S13']} "
          f"taps={tap_survivors(p.G, p.w)} c_m=0x{p.c_m:x} c_l=0x{p.c_l:x}")
    st = init_state(seed=1, p=p)
    print("init:", state_key(st))
    for step in range(5):
        st = next_state(st, p)
        print(f"step {step}:", state_key(st))
