"""Toy-scale (n, w)-parameterized generalization of ra_prng2's ra_core loop.

Separate, new implementation - not a modification of
src/ra_prng2/python/ra_prng2.py or src/ra_prng2/c/ra_prng2_struct.c.
Structurally faithful port scaled down to array length n (words) and word
width w (bits) so that real cycle lengths become measurable. See
HANDOVER.md and RESULTS.md for the generalization rules and the three
scaling traps (shift amounts, golden-ratio constant truncation, the ra_hash
"8/32" structural constants) this module works around.
"""

from __future__ import annotations

from dataclasses import dataclass

C_M32 = 0x06A0DD9B
C_L32 = 0x9E3779B7

# original plain-shift amounts (not rotates) at w=32, rescaled per config
_ORIG_SHIFTS = {"S9": 9, "S18": 18, "S13": 13, "S14": 14}


def rotw(x: int, r: int, w: int) -> int:
    """Rotate the low w bits of x left by r bits (mod w)."""
    mask = (1 << w) - 1
    r &= (w - 1)  # valid because every swept w is a power of two
    x &= mask
    return ((x << r) | (x >> (w - r))) & mask if r else x


def rescale_shifts(w: int) -> dict[str, int]:
    """Rescale the four fixed 32-bit plain-shift amounts to width w.

    s_w = round(s * w / 32), clamped to [1, w-1] so no term is silently
    deleted (shift >= w zeroes it; shift 0 is a no-op).
    """
    out = {}
    for name, s in _ORIG_SHIFTS.items():
        s_w = round(s * w / 32)
        s_w = min(max(s_w, 1), w - 1)
        out[name] = s_w
    return out


def truncate_const(c32: int, w: int) -> int:
    """Top-w-bits truncation of a 32-bit golden-ratio-derived constant.

    Trap fix: naive truncation can collapse to 0 (e.g. 0x06a0dd9b at w=4),
    which would zero the whole M-init. Force nonzero if that happens.
    """
    mask = (1 << w) - 1
    c_w = (c32 >> (32 - w)) & mask
    return c_w or 1


def group_size(n: int, rows: int) -> int:
    """G(n, rows): generalization of ra_hash's fixed 8/32 structural
    constants (256 = 8 * 32 in the original -> G=8 output words, rows=32
    reduction depth). G is the number of directly-mutated positions /
    output words, rows is how many source terms get XORed into each one;
    G * rows == n always.

    ra_hash_gen's inner loop over j always self-cancels its first term
    (j=0 gives N[i] ^= N[i] = 0 - true in the *original* algorithm too,
    see src/ra_prng2/python/ra_prng2.py:14-20), then accumulates
    (rows - 1) further terms. Two choices of rows were rejected by direct
    empirical testing before settling on a default:
      - rows = 1 (G = n) wipes M to all-zero every step: with rows=1
        there is *only* the self-cancelling term (confirmed: M collapsed
        to (0,0,0,0) from step 0 onward).
      - rows = 2 (G = n/2) means the one surviving term after the
        self-cancel is a raw copy, not a mix: N[i]_final = N[i+G]_orig
        exactly (confirmed: M[i] == M[i + n/2] identically at every step,
        an artificial correlation the real n=256 algorithm does not have).
    Default rows = min(4, n) (giving G = max(1, n // 4)) avoids both: it
    gives rows = 4 for every n >= 4 in the sweep (3 real accumulated terms
    - enough to not be a raw copy) and rows = 2 for n = 2 (unavoidable at
    that size - the handover already flags n=2 as "very degenerate" for
    the same underlying reason).

    `rows` is exposed as an explicit Params field precisely so this
    default can be overridden - RESULTS.md's follow-up section sweeps it
    across its full achievable range (rows can never exceed n) to check
    whether measured lambda moves toward the sqrt(|S|) prediction as the
    reduction deepens back toward the original's 32, separating "the
    heuristic is wrong" from "this default was too shallow to be a fair
    test." Still, for n <= 8, rows is capped far below 32 either way - an
    unavoidable consequence of shrinking n this far, documented in
    RESULTS.md, not silently picked.
    """
    assert n % rows == 0, f"rows={rows} must evenly divide n={n}"
    G = n // rows
    assert G >= 1
    return G


def ra_hash_gen(N: list[int], n: int, w: int, G: int) -> list[int]:
    """Direct structural port of ra_hash (ra_prng2.py) with 256->n, 8->G,
    32->n/G, 0xFF->(n-1). Mutates N in place exactly like the original,
    including the subtle ordering where out[i] is computed from N[i]
    *before* the inner loop mutates N[i] (not after).
    """
    mask = (1 << w) - 1
    assert n % G == 0, f"group_size {G} must evenly divide n={n}"
    rows = n // G
    out = [0] * G
    for i in range(G):
        out[i] ^= N[N[i] & (n - 1)]
        for j in range(rows):
            N[i] ^= N[j * G + i]
        N[i] &= mask
    return out


@dataclass(frozen=True)
class Params:
    n: int
    w: int
    rows: int | None = None  # ra_hash reduction depth; None -> default min(4, n)
    mechanism: str = "permute"  # L state-update mechanism, see MECHANISMS above
    m_mechanism: str = "xor_fold"  # M state-update mechanism, see M_MECHANISMS below

    def __post_init__(self):
        assert self.n >= 2 and (self.n & (self.n - 1)) == 0, "n must be a power of two >= 2"
        assert self.w >= 2, "w must be >= 2"
        assert self.mechanism in MECHANISMS, f"mechanism must be one of {MECHANISMS}"
        assert self.m_mechanism in M_MECHANISMS, f"m_mechanism must be one of {M_MECHANISMS}"
        rows = self.rows if self.rows is not None else min(4, self.n)
        group_size(self.n, rows)  # validates n % rows == 0
        object.__setattr__(self, "rows", rows)

    @property
    def mask(self) -> int:
        return (1 << self.w) - 1

    @property
    def G(self) -> int:
        return group_size(self.n, self.rows)

    @property
    def shifts(self) -> dict[str, int]:
        return rescale_shifts(self.w)

    @property
    def c_m(self) -> int:
        return truncate_const(C_M32, self.w)

    @property
    def c_l(self) -> int:
        return truncate_const(C_L32, self.w)


MECHANISMS = ("permute", "inject", "overwrite")
"""State-update-mechanism spectrum for L, per
experiments/2026-8-28_state-update-mechanism-research/HANDOVER.md:
  - permute:   today's swap, L[i], L[d] = L[d], L[i] (unchanged default).
  - inject:    ra_prng3-style one-directional overwrite, L[i] = L[d];
               L[d] = c (this step's freshly computed output; no new
               mixing function invented).
  - overwrite: direct overwrite, no relocation at all, L[i] = c.
"""

M_MECHANISMS = ("xor_fold", "permute", "inject", "overwrite")
"""State-update-mechanism spectrum for M's once-per-cycle reseed fold, per
HANDOVER.md catalog item #4 ("symmetric treatment of M") -- design choices
made explicit here since HANDOVER.md left the exact rule open ("permute
M's slots" or "replace M's XOR-fold", a menu not a mandate). Each mirrors
one of the L mechanisms' *shape*, applied at M's cadence (once per n-1
step outer cycle, using L's already-finalized values this cycle as the
index/injection source, analogous to how L's own mechanisms read M's
already-finalized `o` tap without aliasing issues):
  - xor_fold:   today's fold, unchanged default: M[i] ^= L[i] for all i.
  - permute:    relocation-only, mirrors L's "permute" -- M[i], M[L[i]%n]
                swapped for each i (a whole-array permutation of M driven
                by L, no XOR, no bit loss).
  - inject:     mirrors L's "inject" -- M[i] = M[j]; M[j] = L[i], where
                j = L[i] % n (one-directional relocate + fresh injection
                of L's finalized value, no new mixing function).
  - overwrite:  mirrors L's "overwrite" -- M[i] = L[i] for all i (direct
                assign, all prior M content discarded every cycle).
"""

State = tuple[list[int], list[int], int, int]  # L, M, cons, it


def init_state(seed: int, p: Params) -> State:
    mask = p.mask
    c_m, c_l = p.c_m, p.c_l
    M = [(i * c_m + c_m) & mask for i in range(p.n)]
    L = [(i * c_l + c_l) & mask for i in range(p.n)]
    cons = seed & mask
    it = 0
    return (L, M, cons, it)


def next_state(state: State, p: Params) -> State:
    L, M, cons, it = state
    L = list(L)
    M = list(M)
    mask = p.mask
    n, w, G = p.n, p.w, p.G
    s = p.shifts
    S9, S18, S13, S14 = s["S9"], s["S18"], s["S13"], s["S14"]

    a = cons
    b = it
    c = 0
    d = 0

    for i in range(n - 1, 0, -1):
        o = 0
        for e in range(G):
            idx = (i + e) & (n - 1)
            o ^= (M[idx] << e) & mask

        a = (rotw(b ^ o, d, w) ^ ((cons + a) & mask)) & mask
        b = (rotw((cons + a) & mask, i, w) ^ ((o + d) & mask)) & mask
        o = ((rotw(a ^ o, i, w) << S9) & mask) ^ (b >> S18)
        c = rotw((((o + c) & mask) << S14 & mask) ^ (b >> S13) ^ a, b, w) & mask

        d = (c * (i + 1)) >> w  # Lemire-style reduction; always < n

        if p.mechanism == "permute":
            L[i], L[d] = L[d], L[i]
        elif p.mechanism == "inject":
            L[i], L[d] = L[d], c
        else:  # "overwrite"
            L[i] = c

    if p.m_mechanism == "xor_fold":
        for i in range(n):
            M[i] ^= L[i]
    elif p.m_mechanism == "permute":
        for i in range(n):
            j = L[i] & (n - 1)
            M[i], M[j] = M[j], M[i]
    elif p.m_mechanism == "inject":
        for i in range(n):
            j = L[i] & (n - 1)
            M[i], M[j] = M[j], L[i]
    else:  # "overwrite"
        for i in range(n):
            M[i] = L[i]

    cons_list = ra_hash_gen(M, n, w, G)

    new_cons = 0
    for e in range(G):
        new_cons ^= (cons_list[e] << e) & mask

    new_it = (it + 1) & mask

    return (L, M, new_cons, new_it)


def state_key(state: State) -> tuple:
    L, M, cons, it = state
    return (tuple(L), tuple(M), cons, it)


if __name__ == "__main__":
    p = Params(n=4, w=8)
    print(f"Params: n={p.n} w={p.w} G={p.G} shifts={p.shifts} "
          f"c_m=0x{p.c_m:x} c_l=0x{p.c_l:x}")
    st = init_state(seed=1, p=p)
    print("init:", state_key(st))
    for step in range(5):
        st = next_state(st, p)
        print(f"step {step}:", state_key(st))
