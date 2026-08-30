"""Full-scale (real 256x32-bit) practical periodicity sanity check, per
HANDOVER.md's methodology (b): "verify the real-scale candidates don't
collapse into a short cycle within a practically reachable number of
iterations... at minimum confirm no exact output-block repeat across the
largest PractRand run performed." This is NOT a proof of astronomical
period -- just a guard against a mechanism accidentally collapsing to
something practically short (billions of outputs, not the astronomical
true period, which can't be computed directly at this scale).

Method: stream N raw uint32 values from each candidate binary, hash every
255-value block (one full outer permutation cycle's worth of `c` values --
the same granularity ra_core's reseed operates on) with a fast 64-bit
hash, and check for any exact duplicate block. A repeated block this deep
into the stream would be strong practical evidence of a short cycle (a
different starting state producing byte-identical output for 255
consecutive words is not plausible by chance at these state sizes).
"""

from __future__ import annotations

import hashlib
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).parent
VARIANTS = [f"{t}_{m}" for t in ("original", "winner") for m in ("permute", "inject", "overwrite")]

BLOCK_WORDS = 255  # one full outer permutation cycle
BLOCK_BYTES = BLOCK_WORDS * 4


def check_no_repeat(binary: Path, n_values: int, seed: int = 1) -> dict:
    proc = subprocess.Popen(
        [str(binary), "--stream", str(seed), str(n_values)],
        stdout=subprocess.PIPE,
    )
    seen = set()
    blocks_checked = 0
    duplicate_at = None
    buf = b""
    assert proc.stdout is not None
    while True:
        chunk = proc.stdout.read(1024 * 1024)
        if not chunk:
            break
        buf += chunk
        while len(buf) >= BLOCK_BYTES:
            block = buf[:BLOCK_BYTES]
            buf = buf[BLOCK_BYTES:]
            h = hashlib.blake2b(block, digest_size=8).digest()
            if h in seen:
                duplicate_at = blocks_checked
                break
            seen.add(h)
            blocks_checked += 1
        if duplicate_at is not None:
            break
    proc.stdout.close()
    proc.terminate()
    proc.wait()

    return {
        "blocks_checked": blocks_checked,
        "values_checked": blocks_checked * BLOCK_WORDS,
        "duplicate_found": duplicate_at is not None,
        "duplicate_at_block": duplicate_at,
    }


def main():
    n_values = int(sys.argv[1]) if len(sys.argv) > 1 else 1_000_000_000
    for name in VARIANTS:
        binary = HERE / name
        if not binary.exists():
            print(f"SKIP {name}: binary not found")
            continue
        print(f"=== {name}: checking {n_values:,} values for repeated {BLOCK_WORDS}-word blocks ===")
        r = check_no_repeat(binary, n_values)
        status = "DUPLICATE FOUND (possible short cycle!)" if r["duplicate_found"] else "no repeat"
        print(f"  {r['values_checked']:,} values / {r['blocks_checked']:,} blocks checked -> {status}")


if __name__ == "__main__":
    main()
