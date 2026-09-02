"""DSL catalog for the cycle-operation combo search (HANDOVER.md).

Search space over ra_permutation_cycle_singleblock's per-round transform:

    o = tap_combo(M, i)                        # slot 1: tap width
    a = (d ^ o) ^ (cons + a)
    b = (cons + a) ^ (o + d)
    c = rot32((a >> 13) ^ a, b)
    c = finalizer(c)                            # slot 2: 0-2 fmix32-shaped stages
    [extra_inject into a or b]                  # slot 3: extra M[] tap injected into a/b
    d = c & 0xFFu

Slot 1 (tap width) x slot 2 (finalizer) = 3 x 36 = 108 distinct *observable*
combinations at K=1 (see NOTE below). Crossed with slot 3 (7 choices) the
full nominal DSL space is 3 x 36 x 7 = 756, stated explicitly per HANDOVER's
requirement not to silently shrink the catalog.

NOTE on slot 3 (extra_inject) being inert at K=1, PROVEN not assumed:
extra_inject writes into `a` or `b` AFTER `c` (this round's output value) has
already been computed and finalized, and BEFORE `d = c & 0xFFu`. Under
ra_core_singleblock's calling convention, K=1 means ra_permutation_cycle
runs exactly ONE loop iteration per call (count starts at 1, `*count <= 1`
triggers `break` right after the first `d = c & 0xFFu`), and a/b/c/d are
reset fresh (a=cons=key, b=0, c=0, d=0) on every call (fresh key per block in
the multikey harness). So the extra_inject write to `a`/`b` can only affect a
*second* round's `a`/`b` computation -- which never executes at K=1. All 7
slot-3 choices are therefore byte-identical in their K=1 output stream for a
given (width, finalizer) pair. This is verified empirically by
verify_inject_inert_at_k1.py before being relied on.

Consequence: Tier 0 (avalanche, single round) and Tier 1 (multikey PractRand
@ K=1) test only the 108 (width, finalizer) combos -- slot 3 fixed at
"off" -- since testing all 7 injects there would be redundant computation of
byte-identical streams. Slot 3 only becomes observable at K>=2 (multiple
rounds run per key before re-keying), so the Promotion tier (K up to 96)
crosses every Tier-1 survivor with all 7 injects.
"""

from __future__ import annotations

from dataclasses import dataclass

MASK32 = 0xFFFFFFFF

# ---------------------------------------------------------------------
# Slot 1: tap width
# ---------------------------------------------------------------------
# o = XOR_{e=8-width}^{7} M[(i+e)&0xFF] << e
# width=2 -> e in {6,7}: byte-identical to production narrow_o.
# width=8 -> e in {0..7}: byte-identical to diag_wideo_singleblock's wide_o.
TAP_WIDTHS = [2, 4, 8]


def tap_c_expr(width: int) -> str:
    """C expression for `o` at tap width `width`, referencing M[] and i."""
    terms = []
    for e in range(8 - width, 8):
        terms.append(f"(M[(uint8_t)(i + {e})] << {e})")
    return " ^ ".join(terms)


# ---------------------------------------------------------------------
# Slot 2: finalizer -- 0-2 fmix32-shaped stages applied to c after rot32.
# ---------------------------------------------------------------------

MUL_CONSTS = [
    0x85ebca6b,  # fmix32 stage 1
    0xc2b2ae35,  # fmix32 stage 2
    0x9e3779b7,  # already used in this codebase's init (golden ratio)
    0x06a0dd9b,  # already used in this codebase's init
    0xff51afd7,  # Murmur3 fmix64 stage 1 (32-bit truncated)
    0xc4ceb9fe,  # Murmur3 fmix64 stage 2 (32-bit truncated)
]
XORSHIFT_SHIFTS = [13, 15, 16, 17]
ROT_SOURCES = ["b", "i", "cons"]  # raw value, rot32() masks &31 internally
ADD_OPERANDS = ["cons", "o", "a", "b"]


@dataclass(frozen=True)
class FinalizerStage:
    kind: str  # "MUL" | "XORSHIFT" | "ROT" | "ADD"
    param: object  # int for MUL/XORSHIFT, str for ROT/ADD

    def describe(self) -> str:
        if self.kind in ("MUL",):
            return f"MUL(0x{self.param:08x})"
        if self.kind == "XORSHIFT":
            return f"XORSHIFT({self.param})"
        return f"{self.kind}({self.param})"

    def c_stmt(self) -> str:
        if self.kind == "MUL":
            return f"c = (uint32_t)(c * 0x{self.param:08x}u);"
        if self.kind == "XORSHIFT":
            return f"c ^= c >> {self.param}u;"
        if self.kind == "ROT":
            return f"c = rot32(c, {self.param});"
        if self.kind == "ADD":
            return f"c = (uint32_t)(c + {self.param});"
        raise ValueError(self.kind)


def _build_finalizer_recipes() -> list[tuple[str, list[FinalizerStage]]]:
    recipes: list[tuple[str, list[FinalizerStage]]] = []

    # 1 control: empty finalizer (current production behavior).
    recipes.append(("empty", []))

    # 6 single-stage MUL
    for const in MUL_CONSTS:
        st = FinalizerStage("MUL", const)
        recipes.append((st.describe(), [st]))

    # 4 single-stage XORSHIFT
    for s in XORSHIFT_SHIFTS:
        st = FinalizerStage("XORSHIFT", s)
        recipes.append((st.describe(), [st]))

    # 3 single-stage ROT
    for src in ROT_SOURCES:
        st = FinalizerStage("ROT", src)
        recipes.append((st.describe(), [st]))

    # 4 single-stage ADD
    for op in ADD_OPERANDS:
        st = FinalizerStage("ADD", op)
        recipes.append((st.describe(), [st]))

    # 8 two-stage XORSHIFT(s) -> MUL(const), s in {13,16} x const in 4 consts
    for s in (13, 16):
        for const in (0x85ebca6b, 0xc2b2ae35, 0x9e3779b7, 0x06a0dd9b):
            st1 = FinalizerStage("XORSHIFT", s)
            st2 = FinalizerStage("MUL", const)
            recipes.append((f"{st1.describe()}->{st2.describe()}", [st1, st2]))

    # 4 two-stage MUL(const) -> XORSHIFT(s), const in {0x85ebca6b,0xc2b2ae35} x s in {13,16}
    for const in (0x85ebca6b, 0xc2b2ae35):
        for s in (13, 16):
            st1 = FinalizerStage("MUL", const)
            st2 = FinalizerStage("XORSHIFT", s)
            recipes.append((f"{st1.describe()}->{st2.describe()}", [st1, st2]))

    # 4 two-stage ADD(operand) -> ROT(source), fixed pairs
    add_rot_pairs = [("o", "b"), ("cons", "i"), ("a", "b"), ("b", "cons")]
    for operand, src in add_rot_pairs:
        st1 = FinalizerStage("ADD", operand)
        st2 = FinalizerStage("ROT", src)
        recipes.append((f"{st1.describe()}->{st2.describe()}", [st1, st2]))

    # 2 two-stage ROT(source) -> XORSHIFT(s), fixed pairs
    rot_xorshift_pairs = [("b", 16), ("i", 13)]
    for src, s in rot_xorshift_pairs:
        st1 = FinalizerStage("ROT", src)
        st2 = FinalizerStage("XORSHIFT", s)
        recipes.append((f"{st1.describe()}->{st2.describe()}", [st1, st2]))

    return recipes


FINALIZER_RECIPES = _build_finalizer_recipes()
assert len(FINALIZER_RECIPES) == 36, len(FINALIZER_RECIPES)


# ---------------------------------------------------------------------
# Slot 3: extra inject -- off, or +M[(i+k)&0xFF] into a, or ^M[(i+k)&0xFF] into b.
# ---------------------------------------------------------------------

INJECT_CHOICES = [
    ("off", None, None),
    ("add_a", "a", 16),
    ("add_a", "a", 32),
    ("add_a", "a", 64),
    ("xor_b", "b", 16),
    ("xor_b", "b", 32),
    ("xor_b", "b", 64),
]
assert len(INJECT_CHOICES) == 7


def inject_describe(idx: int) -> str:
    mode, target, k = INJECT_CHOICES[idx]
    if mode == "off":
        return "inject_off"
    op = "+=" if mode == "add_a" else "^="
    return f"inject_{target}{op}M[i+{k}]"


def inject_c_stmt(idx: int) -> str:
    mode, target, k = INJECT_CHOICES[idx]
    if mode == "off":
        return ""
    if mode == "add_a":
        return f"a = (uint32_t)(a + M[(uint8_t)(i + {k})]);"
    if mode == "xor_b":
        return f"b = b ^ M[(uint8_t)(i + {k})];"
    raise ValueError(mode)


def candidate_id(width: int, finalizer_idx: int, inject_idx: int) -> str:
    return f"w{width}_f{finalizer_idx}_i{inject_idx}"


def candidate_describe(width: int, finalizer_idx: int, inject_idx: int) -> str:
    fdesc = FINALIZER_RECIPES[finalizer_idx][0]
    return f"width={width} finalizer=[{fdesc}] {inject_describe(inject_idx)}"


def all_tier01_candidates():
    """108 (width, finalizer_idx) pairs, inject fixed at 0 (off) -- see
    module docstring for why slot 3 is redundant at K=1."""
    for width in TAP_WIDTHS:
        for fidx in range(len(FINALIZER_RECIPES)):
            yield width, fidx


def all_promotion_candidates(width: int, finalizer_idx: int):
    """7 inject variants for a single (width, finalizer_idx) Tier-1 survivor."""
    for iidx in range(len(INJECT_CHOICES)):
        yield width, finalizer_idx, iidx


if __name__ == "__main__":
    n_t01 = sum(1 for _ in all_tier01_candidates())
    print(f"Tier0/Tier1 candidates (inject fixed off): {n_t01}")
    print(f"Full DSL space (3 x 36 x 7): {len(TAP_WIDTHS) * len(FINALIZER_RECIPES) * len(INJECT_CHOICES)}")
