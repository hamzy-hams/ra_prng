"""Internal cycle-structure analysis for the weyl_fmix32/weyl_triple32
candidates (user follow-up request, 2026-09-05, after RESULTS.md's
analysis section hypothesized WHY the fixed linear intra-block advance
`state_j = state0 + j*WEYL32` causes the new K=255 DC6-9x1Bytes-1 failure).

Two claims to verify, not just assert:

1. PERIOD: state_j = state0 + j*WEYL32 (mod 2^32) has full period 2^32 for
   ANY state0, because WEYL32=0x9e3779b7 is odd (gcd(WEYL32, 2^32) = 1).
   Exhaustively verified on a toy 16-bit analog (2^16 states is tractable
   to fully enumerate); the same number-theory argument generalizes to any
   power-of-two modulus, so it holds for the real 32-bit generator too.

2. CONSEQUENCE: because fmix32/triple32 are bijections (permutations of
   the 32-bit space), output_j = finalizer(state_j) is ALSO a pure
   permutation in j over one full period -- i.e. a fixed key's output
   stream can NEVER repeat a value or self-collide until all 2^32 outputs
   are exhausted. A genuinely random 32-bit stream WOULD show collisions
   by the birthday paradox well before that point. This is verified
   empirically on the real 32-bit generator (a large sample, zero
   collisions) and contrasted against a true-random control (os.urandom),
   which DOES show the expected number of collisions.
"""
import os
import struct
from combo_prng_weylfinal import weyl_core, make_finalizer

MASK32 = 0xFFFFFFFF


# ---------------------------------------------------------------------------
# Part 1: exhaustive period check on a toy 16-bit analog of the same
# construction (state_j = state0 + j*STEP mod 2^16, STEP odd), since 2^16
# states can be fully enumerated in well under a second.
# ---------------------------------------------------------------------------

def toy_fmix16(h):
    """16-bit-width analog of fmix32's shape (2 rounds, odd multiplier) --
    just needs to BE a bijection for this test, doesn't need to match the
    real fmix32 bit-for-bit."""
    h &= 0xFFFF
    h ^= h >> 8
    h = (h * 0xacab) & 0xFFFF   # odd constant -> multiplication is a bijection mod 2^16
    h ^= h >> 8
    h = (h * 0xacab) & 0xFFFF
    return h


def check_toy_period():
    STEP = 0x9e37  # odd, 16-bit analog of WEYL32's low bits
    MOD = 1 << 16
    assert STEP & 1 == 1, "STEP must be odd for full period"

    for state0 in (0, 1, 0x1234, 0xFFFF):
        seen = [False] * MOD
        state = state0
        j = 0
        while True:
            if seen[state]:
                break
            seen[state] = True
            state = (state + STEP) & 0xFFFF
            j += 1
        period = j
        all_seen = all(seen)
        print(f"  toy state0={state0:#06x}: period={period} "
              f"(expected {MOD}), all {MOD} states visited: {all_seen}")
        assert period == MOD, f"expected full period {MOD}, got {period}"
        assert all_seen

    # Bijectivity of the toy finalizer (needed for claim 2's toy analog)
    outputs = set()
    for x in range(MOD):
        outputs.add(toy_fmix16(x))
    print(f"  toy_fmix16 bijective over 2^16: {len(outputs) == MOD} "
          f"({len(outputs)} distinct outputs / {MOD})")
    assert len(outputs) == MOD


# ---------------------------------------------------------------------------
# Part 2: empirical no-self-collision check on the REAL 32-bit generator,
# contrasted with a true-random control at the same sample size.
# ---------------------------------------------------------------------------

def count_collisions(values):
    seen = set()
    collisions = 0
    for v in values:
        if v in seen:
            collisions += 1
        else:
            seen.add(v)
    return collisions


def expected_birthday_collisions(n, space=1 << 32):
    # E[collisions] for n draws with replacement from `space` distinct
    # values, standard birthday-paradox approximation for n << space.
    return n * (n - 1) / (2 * space)


def check_real_generator_collisions(n=1 << 22, finalizer_name="fmix32", key=0x12345):
    finalizer = make_finalizer(finalizer_name)
    outputs = weyl_core(key, n, finalizer)
    collisions = count_collisions(outputs)
    print(f"  weyl_{finalizer_name} key={key:#x}, n={n} ({n:,}) consecutive "
          f"outputs: {collisions} self-collisions (expected 0, PERMUTATION)")
    assert collisions == 0, "generator is supposed to be a pure permutation per key"

    # True-random control at the SAME n, to show what genuine randomness
    # looks like at this sample size (birthday paradox DOES kick in).
    random_vals = [struct.unpack("<I", os.urandom(4))[0] for _ in range(n)]
    rand_collisions = count_collisions(random_vals)
    expected = expected_birthday_collisions(n)
    print(f"  os.urandom control, n={n} ({n:,}): {rand_collisions} collisions "
          f"(birthday-paradox expectation ~{expected:.1f})")


def main():
    print("=== Part 1: exhaustive period check (toy 16-bit analog) ===")
    check_toy_period()

    print("\n=== Part 2: real 32-bit generator, self-collision check vs true-random control ===")
    for finalizer_name in ("fmix32", "triple32"):
        check_real_generator_collisions(finalizer_name=finalizer_name)

    print("\nConclusion: both weyl_fmix32 and weyl_triple32 are PURE PERMUTATIONS")
    print("per key/block (zero self-collision, full 2^32 period) -- unlike a truly")
    print("random 32-bit stream, which DOES self-collide at this sample size.")
    print("This 'too-regular, no-collision' structure is exactly the kind of")
    print("deficit PractRand's frequency/distinguishing tests (DC6-9x1Bytes-1,")
    print("FPF-14+6/16) are built to catch -- consistent with RESULTS.md's")
    print("hypothesis that the fixed linear intra-block advance itself, not")
    print("finalizer strength, is the structural root cause.")


if __name__ == "__main__":
    main()
