"""Catalog of swappable operand-position slots for `baseline.c`'s
(`pruned_winner`'s) `ra_permutation_cycle`, ops={TAP6,TAP7,ROT_C,SHR13}
fixed -- see HANDOVER.md for why the op set itself is not touched here.

The chain being rewired (baseline.c, comment block + ra_permutation_cycle):

    o = (M[(i+6)&0xFF] << 6) ^ (M[(i+7)&0xFF] << 7)
    a = (b ^ o) ^ (cons + a)
    b = (cons + a) ^ (o + d)
    c = rot32((b >> 13) ^ a, b)
    d = c & 0xFF

Four slots are open for rewiring (chosen to respect HANDOVER.md's role
constraints: `cons` is read-only per-step, `i` is loop-counter-only, `d`
must stay a valid [0,255] index after `c & 0xFF` -- that AND is not part of
the 4 active ops and is left untouched):

  a_xor_operand    -- which variable is XORed with `o` in a's first term
                      (default "b", i.e. `b ^ o`).
  c_shift_operand  -- which variable receives the `>> 13` shift in c's
                      update (default "b", i.e. `b >> 13`).
  rotc_amount_source -- which variable supplies ROT_C's rotate amount
                      (default "b", i.e. `rot32(..., b)`).
  rotc_xor_operand -- which variable is XORed with the shift result before
                      rotation (default "a", i.e. `(b>>13) ^ a`).

Every choice is evaluated using values already available at that point in
the step (the *new* `a`/`b` where the original formula already uses the
updated value, `o` freshly built this iteration, `d` from the previous
iteration, `i` the loop counter) -- see wired_prng.py's `permutation_cycle`
for the exact evaluation order.
"""

from __future__ import annotations

from dataclasses import dataclass
from itertools import product

A_XOR_CHOICES = ("b", "a", "d")
C_SHIFT_CHOICES = ("b", "a", "o")
ROTC_AMOUNT_CHOICES = ("b", "a", "d", "i")
ROTC_XOR_CHOICES = ("a", "o", "d")


@dataclass(frozen=True)
class Wiring:
    a_xor_operand: str = "b"
    c_shift_operand: str = "b"
    rotc_amount_source: str = "b"
    rotc_xor_operand: str = "a"


# baseline.c's current wiring -- must reproduce it bit-for-bit (see
# wired_prng.py's _self_check()).
DEFAULT_WIRING = Wiring()


def all_wirings():
    """All 3*3*4*3 = 108 combinations of the four slots."""
    for aw, cs, ra, rx in product(A_XOR_CHOICES, C_SHIFT_CHOICES,
                                   ROTC_AMOUNT_CHOICES, ROTC_XOR_CHOICES):
        yield Wiring(a_xor_operand=aw, c_shift_operand=cs,
                     rotc_amount_source=ra, rotc_xor_operand=rx)


def describe(w: Wiring) -> str:
    default_note = " [DEFAULT/baseline.c]" if w == DEFAULT_WIRING else ""
    return (f"a_xor={w.a_xor_operand} c_shift={w.c_shift_operand} "
            f"rotc_amount={w.rotc_amount_source} rotc_xor={w.rotc_xor_operand}"
            f"{default_note}")


if __name__ == "__main__":
    count = sum(1 for _ in all_wirings())
    print(f"{count} total wirings in the search space")
    print(f"default: {describe(DEFAULT_WIRING)}")
