"""production-candidate-battery, Step 0: passive correctness gate on the
FRESH binaries this folder just built (BUILD.sh) before trusting any axis
result below. Two checks:

1. `./ra_core validate` must report 9945/9945 combinations, 0 mismatches
   (ra_core_singleblock bit-identical to ra_core_orbit for every rng in
   [1,255] across the fixed key set) -- same check as Tahap 1, rerun here
   against a binary built from scratch in THIS folder, not inherited from
   family-productionization/ (whose sibling binaries had mtimes older than
   ra_core.c/ra_core_nomain.o, an unverified staleness risk).
2. `./multikey_stream --stream <key> <n>` (K=255 chaining) must match
   `./ra_core --stream singleblock <key> <n>` called once per block,
   byte-for-byte -- same cross-check RESULTS_TAHAP2.md ran, repeated here
   against the fresh build.
"""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).parent
RA_CORE = HERE / "ra_core"
MULTIKEY_STREAM = HERE / "multikey_stream"

TEST_PAIRS = [(1, 255), (0, 1), (0xFFFFFFFF, 254), (12345, 510), (999, 765)]


def check_validate() -> bool:
    result = subprocess.run([str(RA_CORE), "validate"], stdout=subprocess.PIPE, text=True, check=True)
    print(result.stdout.strip())
    return "validate: PASS" in result.stdout


def check_multikey_cross(key: int, n: int) -> bool:
    chained = subprocess.run(
        [str(MULTIKEY_STREAM), "--stream", str(key), str(n)],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
    ).stdout

    manual = b""
    remaining = n
    block_idx = 0
    while remaining > 0:
        block_len = min(remaining, 255)
        block_key = (key + block_idx * 0x9E3779B9) & 0xFFFFFFFF
        manual += subprocess.run(
            [str(RA_CORE), "--stream", "singleblock", str(block_key), str(block_len)],
            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
        ).stdout
        remaining -= block_len
        block_idx += 1

    ok = chained == manual
    print(f"  key={key} n={n}: {'MATCH' if ok else 'MISMATCH'} "
          f"({len(chained)} vs {len(manual)} bytes)")
    return ok


def main():
    assert RA_CORE.exists(), f"{RA_CORE} not found -- run ./BUILD.sh first"
    assert MULTIKEY_STREAM.exists(), f"{MULTIKEY_STREAM} not found -- run ./BUILD.sh first"

    print("=== check 1: ra_core validate ===")
    ok1 = check_validate()

    print("\n=== check 2: multikey_stream vs manual per-block singleblock ===")
    ok2 = all(check_multikey_cross(k, n) for k, n in TEST_PAIRS)

    print(f"\nvalidate PASS: {ok1}")
    print(f"multikey cross-check PASS: {ok2}")
    overall = ok1 and ok2
    print(f"\nverify_fresh_build: {'PASS' if overall else 'FAIL'}")
    sys.exit(0 if overall else 1)


if __name__ == "__main__":
    main()
