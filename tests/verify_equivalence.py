"""Cross-language equivalence: Python reference vs C ground truth.

Safety net that must stay green BEFORE any performance optimization work
starts on the C engine (src/ra_prng2/c/*.c). It checks that the pure-Python
reference (src/ra_prng2/python/ra_prng2.py, with the ra_hash mutation bug
fixed) and the canonical C implementation (src/ra_prng2/c/ra_prng2_struct.c,
via the ctypes binding bindings/cpython/_ra_prng2.py::RaPrng2) agree
bit-for-bit on `cons` for every seed / iteration-count combination below.

Requires `make build-ext` (-> build/lib/libra_prng2.so). If that has not
been run, this whole module is skipped (not failed).
"""
import pytest

import ra_prng2

try:
    from bindings.cpython._ra_prng2 import RaPrng2
except FileNotFoundError as exc:
    pytest.skip(str(exc), allow_module_level=True)

# A few small seeds, a few large ones, and both documented 32-bit edges.
SEEDS = [0, 1, 2, 42, 12345, 0x7FFFFFFF, 0xDEADBEEF, 0xFFFFFFFF]

# 0 exercises "no outer iteration ran" (cons == seed on both sides);
# 1-5 exercise the actual mixing loop without the multi-second runtime
# thousands of outer iterations would cost in pure Python.
ITERATIONS = [0, 1, 2, 5]


@pytest.mark.parametrize("iterations", ITERATIONS)
@pytest.mark.parametrize("seed", SEEDS)
def test_python_reference_matches_c_ground_truth(seed, iterations):
    expected = RaPrng2(seed=seed).advance(iterations)
    actual = ra_prng2.ra_core(seed=seed, iteration=iterations)
    assert actual == expected, (
        f"seed=0x{seed:08X} iterations={iterations}: "
        f"python cons=0x{actual:08X} != C cons=0x{expected:08X}"
    )
