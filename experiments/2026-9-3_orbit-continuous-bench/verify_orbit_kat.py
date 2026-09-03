"""Byte-identical + KAT verification for orbit_bench.c's extern-linked
ra_core_orbit, against (a) the unmodified production ra_core binary's
--stream mode, and (b) fixed KAT values already computed/cited in prior
sessions' RESULTS.md files. Must be run (0 mismatches) before any timing
run. Read-only against ra_core.c and the production ra_core binary.
"""
from __future__ import annotations

import struct
import subprocess
from pathlib import Path

HERE = Path(__file__).parent
ORBIT_BENCH = HERE / "orbit_bench"
PROD_RA_CORE = HERE / "../2026-9-1_family-productionization/ra_core"

# (key, n, expected_last_word) -- last_word is the final uint32 in the raw
# stream. Values freshly re-verified in this session by running the
# unmodified production `ra_core --stream orbit <key> <n>` binary directly
# (read-only invocation, see HANDOVER.md this folder).
#
# NOTE: for orbit, requesting rng=n does NOT guarantee exactly n words are
# written -- ra_core_orbit's count/break logic returns as soon as its
# internal `count` hits <=1, which for n=256 happens after only 255 words
# (the second 255-word cycle never starts). This is existing, unmodified
# production behavior (confirmed byte-identical against the production
# binary below), not a harness bug -- so `n` here is a request, and the
# actual stream length is looked up per-case rather than assumed to be n*4.
KAT = [
    (1, 1, 2452187902),
    (0, 1, 2019748745),
    (1, 256, 725782582),   # requesting 256 words yields only 255 (see NOTE)
    (1, 510, 3006171839),
    (42, 1000, 2983272652),
]


def stream_bytes(binary: Path, core: str, key: int, n: int) -> bytes:
    out = subprocess.run(
        [str(binary), "--stream", core, str(key), str(n)],
        capture_output=True, check=True,
    ).stdout
    return out


def last_word(raw: bytes) -> int:
    assert len(raw) % 4 == 0, f"stream length {len(raw)} not a multiple of 4"
    return struct.unpack("<I", raw[-4:])[0]


def main():
    if not ORBIT_BENCH.exists():
        raise SystemExit(f"{ORBIT_BENCH} not found -- compile orbit_bench.c first")

    total = 0
    mismatches = 0

    # 1. Fixed KAT check (orbit_bench's own output).
    for key, n, expected in KAT:
        total += 1
        raw = stream_bytes(ORBIT_BENCH, "orbit", key, n)
        if len(raw) == 0 or len(raw) % 4 != 0:
            mismatches += 1
            print(f"MISMATCH (length) key={key} n={n}: got {len(raw)} bytes (not a positive multiple of 4)")
            continue
        got = last_word(raw)
        if got != expected:
            mismatches += 1
            print(f"MISMATCH (KAT) key={key} n={n}: got {got}, expected {expected}")
        else:
            print(f"OK (KAT) key={key} n={n} last_word={got}")

    # 2. Byte-identical vs production ra_core binary's --stream orbit,
    #    full stream (not just last word), if that binary exists.
    if PROD_RA_CORE.exists():
        for key, n, _ in KAT:
            total += 1
            a = stream_bytes(ORBIT_BENCH, "orbit", key, n)
            b = stream_bytes(PROD_RA_CORE, "orbit", key, n)
            if a != b:
                mismatches += 1
                print(f"MISMATCH (vs production ra_core) key={key} n={n}: "
                      f"{len(a)} bytes vs {len(b)} bytes, "
                      f"{'differ' if len(a) == len(b) else 'length differs'}")
            else:
                print(f"OK (vs production ra_core, {len(a)} bytes) key={key} n={n}")
    else:
        print(f"NOTE: production binary {PROD_RA_CORE} not found -- "
              f"skipping direct binary comparison, relying on fixed KAT only.")

    print(f"\n{total - mismatches}/{total} checks passed, {mismatches} mismatches.")
    if mismatches:
        raise SystemExit(1)
    print("verify_orbit_kat: PASS -- orbit_bench's ra_core_orbit is verified "
          "correct before any timing run.")


if __name__ == "__main__":
    main()
