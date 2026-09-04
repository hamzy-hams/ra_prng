"""experiments/2026-9-4_orbit-kmin-battery -- dieharder for the two
multikey-chaining stream sources this gate folder builds:
`multikey_stream_orbit_k1` (K=1, `ra_core_orbit` reinit every word) and
`multikey_stream_orbit_k255` (K=255, reinit every 255 words). Both link
against ra_core_nomain_v2.o, i.e. the FIXED w8_f10_i0 + rolling-o orbit
formula from ../2026-9-4_orbit-fix-and-wideo-rolling-optimization/ra_core_v2.c
-- this is the statistical re-validation that folder's HANDOVER.md
explicitly deferred and gated promotion on.

Copied unmodified from
../2026-9-1_production-candidate-battery/run_dieharder_battery.py: the
generator CLI contract (`--stream <base_key> <n>`, 3 args) is identical.
Same 27 "Good"-reliability test list, same piped-no-file-no-rewind
methodology, same BASE_KEY (consistency with every other dieharder run in
this repo).
"""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).parent

GOOD_TESTS = [
    0, 1, 2, 3, 4, 8, 9, 10, 11, 12, 13, 15, 16, 17,
    100, 101, 102,
    200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
]

DEFAULT_WORDS = 300_000_000   # 1.2GB
GCD_WORDS = 2_800_000_000     # 11.2GB -- GCD test (-d 17) needs ~10GB+, else EOF

OVERRIDES: dict[int, list[list[str]]] = {
    201: [["-n", "2"], ["-n", "3"], ["-n", "4"], ["-n", "5"]],
}

BASE_KEY = 111222  # consistent with promotion_search.py / dieharder-inject-crossing


def run_one(binary: Path, d: int, extra_args: list[str]) -> str:
    words = GCD_WORDS if d == 17 else DEFAULT_WORDS
    gen = subprocess.Popen(
        [str(binary), "--stream", str(BASE_KEY), str(words)],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
    )
    args = ["dieharder", "-g", "200", "-d", str(d)] + extra_args
    test = subprocess.run(
        args, stdin=gen.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
    )
    assert gen.stdout is not None
    gen.stdout.close()
    gen.terminate()
    gen.wait()
    return test.stdout.decode(errors="replace")


def run_test(binary: Path, d: int, out_f) -> str:
    invocations = OVERRIDES.get(d, [[]])
    combined = ""
    for extra_args in invocations:
        out = run_one(binary, d, extra_args)
        combined += out
    header = f"=== -d {d} ==="
    out_f.write(header + "\n" + combined + "\n")
    out_f.flush()
    print(header)
    print(combined.strip()[-400:])
    return combined


def main():
    if len(sys.argv) < 3:
        print(f"usage: {sys.argv[0]} <binary_path> <name>")
        sys.exit(1)
    binary = Path(sys.argv[1]).resolve()
    name = sys.argv[2]
    assert binary.exists(), f"binary not found: {binary}"

    out_path = HERE / f"dieharder_{name}_piped.txt"
    results = []
    with open(out_path, "w") as out_f:
        for d in GOOD_TESTS:
            out = run_test(binary, d, out_f)
            results.append((d, "PASSED" in out, "WEAK" in out, "FAILED" in out))

    n_fail = sum(1 for _, _, _, f in results if f)
    n_weak = sum(1 for _, _, w, _ in results if w)
    n_pass = sum(1 for _, p, _, f in results if p and not f)
    print(f"\n=== summary for {name} ===")
    print(f"{n_pass}/{len(GOOD_TESTS)} test invocations show PASSED, "
          f"{n_weak} contain a WEAK sub-result, {n_fail} show FAILED")
    print(f"Raw output: {out_path}")
    if n_fail:
        failed_d = [d for d, _, _, f in results if f]
        print(f"FAILED at -d {failed_d} -- reject, no reseed retry per repo-standing rule.")


if __name__ == "__main__":
    main()
