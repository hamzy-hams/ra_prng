"""Mandatory correctness gate -- MUST pass before trusting any cycle-length
number produced by this experiment.

toy_prng.py and wired_prng.py don't operate at the same (n, w), so a literal
bit-identical check across the two toy models isn't directly possible at toy
scale. But Params(n=256, w=32, rows=32) is a LEGAL instantiation of
pruned_wired_toy_prng (no upper bound on n, only n>=2 power of two, w>=2),
and at exactly this (n, w, rows) every one of the generalization functions
becomes an identity:
  - rescale_shifts(32): s_w = round(s*32/32) = s unchanged -> S13=13,
    matching winner_wired_v2.c's literal `>> 13`.
  - truncate_const(c32, 32): top-32-bits of a 32-bit constant = itself,
    unchanged -> c_m=0x06A0DD9B, c_l=0x9E3779B7, matching wired_prng.py's
    init_state() literals exactly.
  - Params(n=256, rows=32).G == 8, tap_survivors(8, 32) == (6, 7) -- exact
    match to TAP6, TAP7.
  - ra_hash_gen_sequential(M, 256, 32, 8): rows=32, base=i*32, loop
    j in range(32) -- textually the same reduction as winner_wired_v2.c's
    ra_hash / wired_prng.py's reseed().
  - d = c & (256-1) = c & 0xFF -- matches winner_wired_v2.c's
    `d = c & 0xFFu` bit-for-bit.

So at (n=256, w=32, rows=32), pruned_wired_toy_prng.next_state should be
byte-for-byte the same computation as wired_prng.py's permutation_cycle +
reseed under Wiring(a_xor_operand="d", c_shift_operand="a",
rotc_amount_source="b", rotc_xor_operand="a") -- winner_wired_v2's actual
wiring (NOT wiring.py's DEFAULT_WIRING, which is pruned_winner's baseline,
all-default "b"/"b"/"b"/"a").
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.append(str(Path(__file__).parent.parent /
                     "2026-8-27_operand-position-search"))
from wiring import Wiring  # noqa: E402
from wired_prng import final_cons as wired_final_cons  # noqa: E402

from pruned_wired_toy_prng import Params, init_state, next_state, tap_survivors  # noqa: E402

WINNER_WIRED_V2 = Wiring(a_xor_operand="d", c_shift_operand="a",
                          rotc_amount_source="b", rotc_xor_operand="a")


def toy_final_cons(seed: int, iterations: int, p: Params) -> int:
    state = init_state(seed, p)
    for _ in range(iterations):
        state = next_state(state, p)
    return state[2]  # cons


def self_check() -> None:
    p = Params(n=256, w=32, rows=32)
    assert p.G == 8, f"expected G=8, got {p.G}"
    assert p.shifts["S13"] == 13, f"expected S13=13 unrescaled, got {p.shifts['S13']}"
    assert p.c_m == 0x06A0DD9B, f"expected c_m=0x06A0DD9B, got {p.c_m:#x}"
    assert p.c_l == 0x9E3779B7, f"expected c_l=0x9E3779B7, got {p.c_l:#x}"
    assert tap_survivors(p.G, p.w) == (6, 7), f"expected taps (6,7), got {tap_survivors(p.G, p.w)}"
    print(f"Params check: G={p.G} S13={p.shifts['S13']} c_m={p.c_m:#x} c_l={p.c_l:#x} "
          f"taps={tap_survivors(p.G, p.w)}  [all identities confirmed]")

    all_ok = True
    for seed, iterations in [(1, 1), (1, 2), (1, 3), (42, 3), (0xDEADBEEF, 1)]:
        expected = wired_final_cons(seed, iterations, WINNER_WIRED_V2)
        got = toy_final_cons(seed, iterations, p)
        status = "OK" if expected == got else "MISMATCH"
        if expected != got:
            all_ok = False
        print(f"seed={seed} iterations={iterations}: expected={expected} got={got} [{status}]")

    if not all_ok:
        raise AssertionError(
            "pruned_wired_toy_prng at n=256,w=32,rows=32 does NOT match "
            "winner_wired_v2 exactly -- do not trust any cycle-length numbers "
            "from this experiment until this is fixed."
        )
    print("\nSelf-check PASSED: pruned_wired_toy_prng.next_state is bit-identical "
          "to winner_wired_v2.c / wired_prng.py at full (n=256,w=32) scale.")


if __name__ == "__main__":
    self_check()
