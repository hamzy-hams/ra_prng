"""Two-tier quality gate for an ablation candidate.

Tier 0 (avalanche): pure Python, no subprocess, milliseconds. Port of
experiments/2026-8-25_periodicity-heuristic-validation/avalanche_check.py's
approach onto the full-scale (n=256, w=32) pruned_prng.py generator: single
outer iteration, no rehash, flip each of the 32 seed bits, measure mean
Hamming distance across the captured 255-value `c` sequence vs. the unflipped
baseline. Target: convergence toward ~50% of 32 bits differing.

Tier 1 (PractRand prefix): only run for candidates that already passed Tier
0. Streams raw uint32_t `c` bytes from the compiled `pruned_prng` C binary
(cross-validated bit-for-bit against pruned_prng.py -- see pruned_prng.c's
header) directly into the already-built
~/Documents/research/PractRand/RNG_test binary via a pipe (stdin32 mode) --
same piping idea as ra_core's `--stream` mode
(src/ra_prng2/c/ra_prng2.c, commit 5ea01d9).

The pure-Python generator (pruned_prng.py's `stream()`) is still used for
Tier 0 (cheap: 32 x 255 = 8160 steps, milliseconds) but is NOT used for Tier
1 any more -- see DEFAULT_PRACTRAND_BYTES below for why.
"""

from __future__ import annotations

import subprocess
from pathlib import Path

from operations import ops_to_bitmask
from pruned_prng import Candidate, stream, HASH_ACCESS_SEQUENTIAL

PRACTRAND_BIN = Path.home() / "Documents/research/PractRand/RNG_test"
PRUNED_PRNG_BIN = Path(__file__).parent / "pruned_prng"

# Tier 1 default size.
#
# CAUGHT BY RUNNING THE CODE, NOT BY INSPECTION, TWICE (mirrors the
# periodicity experiment's silent-collapse traps):
#   1. A 1MB gate let a near-degenerate 3-of-18-ops candidate (at a
#      since-abandoned reduced HASH_DEPTH=4) through completely clean ("no
#      anomalies"); that candidate FAILED catastrophically (multiple hard
#      FAILs) at 16MB. 2MB was the smallest size that caught it at all.
#   2. Raising the gate to 8MB and rerunning greedy ablation (HASH_DEPTH now
#      correctly fixed at 32) converged to a different 5-of-18-ops
#      candidate that ALSO passed 8MB (and even 24MB) cleanly, then failed
#      hard (multiple BCFN FAILs) at 32MB -- i.e. 8MB was *still* too weak,
#      just for a different, less-broken candidate than trap #1.
# Lesson: there is no small fixed prefix that's safely candidate-agnostic --
# the amount of data needed to expose a given structural weakness scales
# with how subtle that weakness is, not with a size chosen up front from a
# throughput estimate. This is only tractable at all because pruned_prng.c
# (~239 MB/s, vs. pruned_prng.py's ~0.8 MB/s) makes even a large prefix
# cost a fraction of a second. 64MB is used here: 2x the empirically found
# 32MB failure point of trap #2, and still ~0.3s/candidate in C. It is NOT
# proven safe for every possible candidate -- the final converged candidate
# from a search MUST still be independently validated at a larger tier
# (see RESULTS.md "final validation") before being trusted, no matter what
# the loop gate size is.
DEFAULT_PRACTRAND_BYTES = 64 * 1024 * 1024


def hamming(x: int, y: int) -> int:
    return bin(x ^ y).count("1")


def _capture_first_cycle(seed: int, cand: Candidate) -> list[int]:
    """One outer iteration's worth of `c` values (255 of them), no reseed."""
    out = []
    for i, c in enumerate(stream(seed, 1, cand)):
        out.append(c)
    return out


def avalanche_stats(capture_fn, base_seed: int = 1) -> dict:
    """Generic per-bit avalanche measurement: flip each of the 32 seed
    bits, capture one cycle via `capture_fn(seed) -> list[int]`, measure
    mean Hamming distance against the baseline capture.

    Added after `avalanche_gate()`'s scalar (32-bit-averaged) fraction was
    found to hide a real defect: `pruned_winner` (ops={TAP6,TAP7,ROT_C,
    SHR13}) has seed bits 5 and 6 almost completely avalanche-dead (253/255
    output positions show 0 bit difference when either is flipped, mean
    Hamming 0.0078/32) while the *average* fraction (0.418459) still landed
    inside avalanche_gate()'s passing [0.3, 0.7] band -- see
    ../2026-8-27_operand-position-search/HANDOVER.md. Per-bit fractions
    (and their minimum) are the only way to catch this class of defect;
    the overall average alone cannot.

    `capture_fn` is generic (not tied to `Candidate`/`pruned_prng.py`) so
    this is reusable for wiring-parameterized generators too (see
    ../2026-8-27_operand-position-search/wired_prng.py).
    """
    baseline = capture_fn(base_seed)
    per_bit_means = []
    for bit in range(32):
        flipped_seed = (base_seed ^ (1 << bit)) & 0xFFFFFFFF
        mutated = capture_fn(flipped_seed)
        dists = [hamming(a, b) for a, b in zip(baseline, mutated)]
        per_bit_means.append(sum(dists) / len(dists) if dists else 0.0)

    per_bit_fractions = [m / 32.0 for m in per_bit_means]
    overall_bits = sum(per_bit_means) / len(per_bit_means) if per_bit_means else 0.0
    fraction = overall_bits / 32.0
    return {
        "overall_mean_hamming_bits": overall_bits,
        "overall_mean_hamming_fraction": fraction,
        "per_bit_fractions": per_bit_fractions,
        "min_bit_fraction": min(per_bit_fractions) if per_bit_fractions else 0.0,
    }


def avalanche_gate(cand: Candidate, base_seed: int = 1,
                    low: float = 0.3, high: float = 0.7) -> dict:
    stats = avalanche_stats(lambda seed: _capture_first_cycle(seed, cand), base_seed)
    fraction = stats["overall_mean_hamming_fraction"]
    passed = low <= fraction <= high
    return {
        "passed": passed,
        "overall_mean_hamming_bits": stats["overall_mean_hamming_bits"],
        "overall_mean_hamming_fraction": fraction,
    }


def avalanche_gate_min_bit(cand: Candidate, base_seed: int = 1,
                            low: float = 0.3, high: float = 0.7,
                            min_bit_floor: float = 0.2) -> dict:
    """Like avalanche_gate(), but also rejects any candidate where the
    *weakest* single seed bit falls below `min_bit_floor` -- closes the
    blind spot documented in avalanche_stats()'s docstring. Used by
    ../2026-8-27_operand-position-search/operand_search.py's Tier 0."""
    stats = avalanche_stats(lambda seed: _capture_first_cycle(seed, cand), base_seed)
    fraction = stats["overall_mean_hamming_fraction"]
    passed = (low <= fraction <= high) and (stats["min_bit_fraction"] >= min_bit_floor)
    return {"passed": passed, **stats}


def practrand_prefix_gate(cand: Candidate, seed: int = 1,
                           n_bytes: int = DEFAULT_PRACTRAND_BYTES) -> dict:
    if not PRACTRAND_BIN.exists():
        raise FileNotFoundError(
            f"PractRand RNG_test not found at {PRACTRAND_BIN} -- build it first "
            "(see HANDOVER.md/RESULTS.md 'tooling' section)."
        )
    if not PRUNED_PRNG_BIN.exists():
        raise FileNotFoundError(
            f"{PRUNED_PRNG_BIN} not found -- compile it first: "
            "gcc -O3 -march=native -o pruned_prng pruned_prng.c"
        )

    n_values = n_bytes // 4
    # iterations chosen generously; RNG_test's -tlmax below is the actual
    # cutoff, so any value >= ceil(n_values/255) works -- reseeding
    # mid-generation is fine, PractRand doesn't care about cycle boundaries.
    iterations = n_values // 255 + 2

    bitmask = ops_to_bitmask(cand.ops)
    access_code = 1 if cand.hash_access == HASH_ACCESS_SEQUENTIAL else 0

    # A bare number is parsed by RNG_test as log2(bytes), not a byte count,
    # and a plain byte-count + "B" suffix is rejected ("invalid test
    # length") -- RNG_test only accepts KB/MB/GB/TB/PB-suffixed lengths (or a
    # time). See `RNG_test -help`, "notes on lengths". n_bytes is required to
    # be a whole number of MB by callers of this function.
    assert n_bytes % (1024 * 1024) == 0, "practrand_prefix_gate requires n_bytes to be a whole MB"
    length_arg = f"{n_bytes // (1024 * 1024)}MB"

    widths = [str(w) for w in cand.shift_widths]
    gen = subprocess.Popen(
        [str(PRUNED_PRNG_BIN), str(seed), str(iterations), f"{bitmask:X}", str(access_code),
         *widths, "--stream"],
        stdout=subprocess.PIPE,
    )
    test = subprocess.run(
        [str(PRACTRAND_BIN), "stdin32", "-tlmin", length_arg, "-tlmax", length_arg],
        stdin=gen.stdout,
        capture_output=True,
    )
    gen.stdout.close()
    gen.wait()

    stdout = test.stdout.decode(errors="replace")
    anomaly = any(kw in stdout for kw in ("FAIL", "SUSPICIOUS"))
    return {
        "passed": not anomaly,
        "n_bytes": n_bytes,
        "stdout": stdout,
        "stderr": test.stderr.decode(errors="replace"),
    }


def quality_gate(cand: Candidate, n_bytes: int = DEFAULT_PRACTRAND_BYTES) -> dict:
    """Run Tier 0 then (only if it passes) Tier 1. Returns a combined report."""
    ava = avalanche_gate(cand)
    if not ava["passed"]:
        return {"passed": False, "tier": 0, "avalanche": ava}

    pr = practrand_prefix_gate(cand, n_bytes=n_bytes)
    return {
        "passed": pr["passed"],
        "tier": 1,
        "avalanche": ava,
        "practrand": pr,
    }


if __name__ == "__main__":
    from operations import all_ops_baseline
    baseline_cand = Candidate(ops=all_ops_baseline())
    print("Tier 0 (avalanche) on ALL_OPS baseline:")
    result = avalanche_gate(baseline_cand)
    print(result)
