"""Byte-identical verification for diag_init_candidates.c's four cores
(orbit_baseline, orbit_accum, singleblock_baseline, singleblock_accum)
against the unmodified production ra_core binary's --stream mode.

Primary oracle: full-stream byte-identical diff vs production ra_core for
every (key, n) pair below, for all four cores (baseline cores against
production as a sanity check that the copy itself is faithful; accum
cores against production as the actual Priority 1 correctness gate).
Any mismatch, anywhere, disqualifies that candidate from Priority 1 --
no re-run with a different key to explain it away
(feedback_no_reseed_on_fail.md applies beyond just PractRand).

Must be run (0 mismatches for *_accum) before any --bench timing run.
Read-only against ra_core.c and the production ra_core binary.
"""
from __future__ import annotations

import subprocess
from pathlib import Path

HERE = Path(__file__).parent
DIAG_BIN = HERE / "diag_init_candidates"
PROD_RA_CORE = HERE / "../2026-9-1_family-productionization/ra_core"

# orbit: unbounded. singleblock: capped at 255 by production itself.
# Pairs chosen to cover: key=0 (historical weak-key case), key=0xFFFFFFFF,
# a small edge (n=1), the singleblock upper bound (n=255), an orbit
# reseed-crossing case (n=256, n=510, n=1000), and one more ordinary key.
ORBIT_PAIRS = [(1, 1), (0, 1), (1, 256), (1, 510), (42, 1000), (0xFFFFFFFF, 300)]
SINGLEBLOCK_PAIRS = [(1, 1), (0, 1), (1, 255), (42, 255), (7, 128), (0xFFFFFFFF, 200)]

CORES = [
    ("orbit_baseline", "orbit", ORBIT_PAIRS),
    ("orbit_accum", "orbit", ORBIT_PAIRS),
    ("singleblock_baseline", "singleblock", SINGLEBLOCK_PAIRS),
    ("singleblock_accum", "singleblock", SINGLEBLOCK_PAIRS),
]


def stream_bytes(binary: Path, core: str, key: int, n: int) -> bytes:
    return subprocess.run(
        [str(binary), "--stream", core, str(key), str(n)],
        capture_output=True, check=True,
    ).stdout


def main():
    if not DIAG_BIN.exists():
        raise SystemExit(f"{DIAG_BIN} not found -- compile diag_init_candidates.c first")
    if not PROD_RA_CORE.exists():
        raise SystemExit(f"{PROD_RA_CORE} not found -- build production ra_core first")

    total = 0
    mismatches = 0
    per_candidate_status: dict[str, bool] = {}

    for diag_core, prod_core, pairs in CORES:
        candidate_ok = True
        for key, n in pairs:
            total += 1
            a = stream_bytes(DIAG_BIN, diag_core, key, n)
            b = stream_bytes(PROD_RA_CORE, prod_core, key, n)
            if a != b:
                mismatches += 1
                candidate_ok = False
                print(f"MISMATCH {diag_core} vs production {prod_core} "
                      f"key={key} n={n}: {len(a)} bytes vs {len(b)} bytes, "
                      f"{'differ' if len(a) == len(b) else 'length differs'}")
            else:
                print(f"OK {diag_core} vs production {prod_core} "
                      f"key={key} n={n} ({len(a)} bytes)")
        per_candidate_status[diag_core] = candidate_ok

    print(f"\n{total - mismatches}/{total} checks passed, {mismatches} mismatches.\n")
    print("Per-candidate verdict:")
    for name, ok in per_candidate_status.items():
        print(f"  {name}: {'BIT-IDENTICAL (Priority 1 eligible)' if ok else 'MISMATCH (disqualified from Priority 1, not re-tried with a different key)'}")

    if mismatches:
        raise SystemExit(1)
    print("\nverify_init_candidates: PASS -- all four cores verified "
          "byte-identical to production before any timing run.")


if __name__ == "__main__":
    main()
