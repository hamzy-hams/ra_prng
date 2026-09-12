"""DSL catalog for the init-formula x cycle-transform combo search on
ra_core_orbit (HANDOVER.md).

Axis 1 (init-formula, NEW): ra_init_state_orbit's per-index formula

    keyterm_l = fmix32(GUARD_L ^ (0x9e3779b7u * key))      # KEYTERM_MODE=shared only
    keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key))
    for i in range(256):
        r = key ^ i
        l_val = IDX(i) * 0x9e3779b7u * keyterm_l            # shared
              | fmix32(GUARD_L ^ (0x9e3779b7u*key) ^ IDX(i)) # per_index
        m_val = analog with GUARD_M/0x06a0dd9bu
        L[i] = rot32(l_val, r); M[i] = rot32(m_val, r)

crossed IDX_XFORM (4: linear/linear_p1/odd/hashed) x KEYTERM_MODE (2:
shared/per_index) = 8 variants, id `init_<idx>_<mode>`.
INIT_VARIANTS[0] == ("init_linear_shared", "linear", "shared") is the
control -- byte-identical to production ra_init_state_orbit.

Axis 2 (cycle-transform, REUSE narrowed from
../2026-9-2_singleblock-cycle-combo-search/recipes.py's FINALIZER_RECIPES):
4 winners hand-picked for structural diversity among the 11 Promotion Tier
survivors there (all width=8 -- tap width is NOT a free axis for orbit,
o is already structurally 8-tap, see ra_permutation_cycle_orbit and
HANDOVER.md's explicit warning not to reopen tap-width here). Hardcoded
literally rather than re-derived from the full 36-entry FINALIZER_RECIPES,
per HANDOVER.md 4's instruction ("boleh hardcode").

4 x 8 = 32 total combos. CYCLE_VARIANTS[0] paired with INIT_VARIANTS[0]
(id "init_linear_shared__w8_f10_i0") is the mandatory control point --
byte-identical to production ra_core_orbit.
"""

from __future__ import annotations

from dataclasses import dataclass

MASK32 = 0xFFFFFFFF

# ---------------------------------------------------------------------
# Axis 1: init-formula
# ---------------------------------------------------------------------

# (name, C expr for IDX(i) given `int i` in scope). Python-side equivalents
# (IDX_FN) live in combo_prng_orbit.py, which owns fmix32/GUARD_L/GUARD_M.
IDX_XFORMS = [
    ("linear", "((uint32_t)i)"),
    ("linear_p1", "((uint32_t)i + 1u)"),
    ("odd", "(2u * (uint32_t)i + 1u)"),
    ("hashed", "(fmix32((uint32_t)i) | 1u)"),
]

KEYTERM_MODES = ["shared", "per_index"]

INIT_VARIANTS = [
    (f"init_{idx_name}_{mode}", idx_name, mode)
    for idx_name, _ in IDX_XFORMS
    for mode in KEYTERM_MODES
]
assert len(INIT_VARIANTS) == 8
assert INIT_VARIANTS[0] == ("init_linear_shared", "linear", "shared")


def idx_c_expr(idx_name: str) -> str:
    for name, expr in IDX_XFORMS:
        if name == idx_name:
            return expr
    raise KeyError(idx_name)


# ---------------------------------------------------------------------
# Axis 2: cycle-transform (4 hardcoded winners from singleblock combo-search)
# ---------------------------------------------------------------------


@dataclass(frozen=True)
class CycleStage:
    kind: str  # "MUL" | "XORSHIFT" | "ROT" | "ADD"
    param: object

    def describe(self) -> str:
        if self.kind == "MUL":
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


CYCLE_VARIANTS = [
    ("w8_f10_i0", "XORSHIFT(17) [PRODUKSI/kontrol]", [CycleStage("XORSHIFT", 17)]),
    ("w8_f9_i0", "XORSHIFT(16)", [CycleStage("XORSHIFT", 16)]),
    ("w8_f28_i0", "MUL(0xc2b2ae35)->XORSHIFT(13)",
        [CycleStage("MUL", 0xc2b2ae35), CycleStage("XORSHIFT", 13)]),
    ("w8_f33_i0", "ADD(b)->ROT(cons)",
        [CycleStage("ADD", "b"), CycleStage("ROT", "cons")]),
]
assert len(CYCLE_VARIANTS) == 4
assert CYCLE_VARIANTS[0][0] == "w8_f10_i0"


# ---------------------------------------------------------------------
# Combo enumeration
# ---------------------------------------------------------------------


def combo_id(init_id: str, cycle_id: str) -> str:
    return f"{init_id}__{cycle_id}"


def combo_describe(init_id: str, cycle_id: str) -> str:
    init_desc = next(f"{i}/{m}" for i, idx, m in INIT_VARIANTS if i == init_id)
    cycle_desc = next(d for c, d, _ in CYCLE_VARIANTS if c == cycle_id)
    return f"init=[{init_id}] cycle=[{cycle_id}: {cycle_desc}]"


def all_combos():
    """32 (init_id, cycle_id) pairs. First yielded pair is the mandatory
    control point (init_linear_shared, w8_f10_i0) -- see module docstring."""
    for init_id, _, _ in INIT_VARIANTS:
        for cycle_id, _, _ in CYCLE_VARIANTS:
            yield init_id, cycle_id


CONTROL_COMBO = ("init_linear_shared", "w8_f10_i0")


if __name__ == "__main__":
    combos = list(all_combos())
    print(f"Total combos: {len(combos)}")
    assert combos[0] == CONTROL_COMBO
    for init_id, cycle_id in combos:
        print(f"  {combo_id(init_id, cycle_id)}")
