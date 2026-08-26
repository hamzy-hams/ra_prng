"""Pure-Python unit tests for ra_prng2.ra_hash / ra_core.

These do not require the C extension to be built; see
tests/verify_equivalence.py for the cross-language safety-net test.
"""
import ra_prng2


def test_ra_hash_does_not_mutate_input():
    """Regression test for the historical bug: ra_hash must be a pure
    function writing into a separate output array, never back into its
    input N - matching Algorithm 2 in the CSAI2025/ICCS2026 papers
    (tmp8 <- ra_hash(M) is a pure map, M is unchanged by the call)."""
    N = [(i * 0x06a0dd9b + 0x06a0dd9b) & 0xFFFFFFFF for i in range(256)]
    original = list(N)
    ra_prng2.ra_hash(N)
    assert N == original


def test_ra_hash_returns_eight_32bit_words():
    out = ra_prng2.ra_hash(list(range(256)))
    assert len(out) == 8
    assert all(0 <= v <= 0xFFFFFFFF for v in out)


def test_ra_core_returns_int_and_is_deterministic():
    cons1 = ra_prng2.ra_core(seed=42, iteration=3)
    cons2 = ra_prng2.ra_core(seed=42, iteration=3)
    assert isinstance(cons1, int)
    assert cons1 == cons2
