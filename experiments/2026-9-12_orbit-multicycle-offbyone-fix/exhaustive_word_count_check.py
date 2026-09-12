#!/usr/bin/env python3
"""Exhaustive word-count check for ra_core_orbit's off-by-one fix.

For every rng in [1, RNG_MAX], runs `ra_core --stream orbit <key> <addr_hi>
<rng>` and asserts the number of 4-byte words written to stdout equals rng
exactly. Before the fix this must fail (word count == rng-1) at every rng in
{256, 511, 766, 1021, ...} (rng % 255 == 1, rng > 255). After the fix it
must pass everywhere.

Usage: exhaustive_word_count_check.py <path-to-ra_core-binary> [rng_max]
"""
import subprocess
import sys

RNG_MAX = 3000
KEY = "12345"
ADDR_HI = "0"


def word_count(binary: str, rng: int) -> int:
    out = subprocess.run(
        [binary, "--stream", "orbit", KEY, ADDR_HI, str(rng)],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
    ).stdout
    assert len(out) % 4 == 0, f"rng={rng}: stdout length {len(out)} not a multiple of 4"
    return len(out) // 4


def main() -> int:
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <path-to-ra_core-binary> [rng_max]", file=sys.stderr)
        return 2
    binary = sys.argv[1]
    rng_max = int(sys.argv[2]) if len(sys.argv) > 2 else RNG_MAX

    mismatches = []
    for rng in range(1, rng_max + 1):
        got = word_count(binary, rng)
        if got != rng:
            mismatches.append((rng, got))

    predicted_bug_zone = {rng for rng in range(1, rng_max + 1) if rng > 255 and rng % 255 == 1}

    print(f"Checked rng=1..{rng_max}: {len(mismatches)} mismatches")
    if mismatches:
        got_rngs = {rng for rng, _ in mismatches}
        print(f"Mismatched rng values: {sorted(got_rngs)[:20]}"
              f"{' ...' if len(got_rngs) > 20 else ''}")
        if got_rngs == predicted_bug_zone:
            print("Matches predicted pre-fix bug zone (rng % 255 == 1, rng > 255) exactly.")
        else:
            print(f"WARNING: does NOT match predicted bug zone "
                  f"(predicted {len(predicted_bug_zone)} values, got {len(got_rngs)}).")
        return 1

    print("PASS: all rng in range produced exactly rng words.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
