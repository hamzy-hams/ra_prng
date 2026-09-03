"""dieharder "Good"-reliability battery for the two singleblock K-small-defect
Pareto-selection co-frontier fix candidates, `w8_f10_i0` and `w8_f28_i0`
(see ../2026-9-3_combo-winner-pareto-selection/RESULTS.md). Only PractRand
had been run on these two before -- dieharder was action item #1 in
../2026-9-1_family-productionization/PRODUCTION_READINESS_HANDOVER.md.

Adapted from ../2026-9-1_dieharder-battery/run_dieharder_battery.py: the only
change is the generator CLI, which for these combo-search candidates is
`--stream <base_key> <n> <K>` (4 args), not `--stream <seed> <n>` (3 args) --
K selects the multikey re-init block length. Reuses the exact piped-no-file
methodology (one dieharder test at a time via direct pipe, no rewind).

K values tested: 1 (worst case, most defect-prone) and 96 (the safe-K floor
recommended in ../2026-9-2_singleblock-k-threshold-characterization/RESULTS.md).
"""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).parent

# 27 "Good"-reliability dieharder tests -- identical list to
# ../2026-9-1_dieharder-battery/run_dieharder_battery.py (dieharder 3.31.1,
# same machine, re-verify with `dieharder -l` if ever run elsewhere).
GOOD_TESTS = [
    0, 1, 2, 3, 4, 8, 9, 10, 11, 12, 13, 15, 16, 17,
    100, 101, 102,
    200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
]

DEFAULT_WORDS = 300_000_000   # 1.2GB -- matches prior dieharder runs' default
GCD_WORDS = 2_800_000_000     # 11.2GB -- GCD test (-d 17) needs ~10GB+, else EOF

OVERRIDES: dict[int, list[list[str]]] = {
    201: [["-n", "2"], ["-n", "3"], ["-n", "4"], ["-n", "5"]],
}

BASE_KEY = 111222  # consistent with promotion_search.py / tier1_search.py


def run_one(binary: Path, k: int, d: int, extra_args: list[str]) -> str:
    words = GCD_WORDS if d == 17 else DEFAULT_WORDS
    gen = subprocess.Popen(
        [str(binary), "--stream", str(BASE_KEY), str(words), str(k)],
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


def run_test(binary: Path, k: int, d: int, out_f) -> str:
    invocations = OVERRIDES.get(d, [[]])
    combined = ""
    for extra_args in invocations:
        out = run_one(binary, k, d, extra_args)
        combined += out
    header = f"=== -d {d} ==="
    out_f.write(header + "\n" + combined + "\n")
    out_f.flush()
    print(header)
    print(combined.strip()[-400:])
    return combined


def main():
    if len(sys.argv) < 4:
        print(f"usage: {sys.argv[0]} <binary_path> <name> <K>")
        sys.exit(1)
    binary = Path(sys.argv[1]).resolve()
    name = sys.argv[2]
    k = int(sys.argv[3])
    assert binary.exists(), f"binary not found: {binary}"

    out_path = HERE / f"dieharder_{name}_K{k}_piped.txt"
    results = []
    with open(out_path, "w") as out_f:
        for d in GOOD_TESTS:
            out = run_test(binary, k, d, out_f)
            results.append((d, "PASSED" in out, "WEAK" in out, "FAILED" in out))

    n_fail = sum(1 for _, _, _, f in results if f)
    n_weak = sum(1 for _, _, w, _ in results if w)
    n_pass = sum(1 for _, p, _, f in results if p and not f)
    print(f"\n=== summary for {name} K={k} ===")
    print(f"{n_pass}/{len(GOOD_TESTS)} test invocations show PASSED, "
          f"{n_weak} contain a WEAK sub-result, {n_fail} show FAILED")
    print(f"Raw output: {out_path}")
    if n_fail:
        failed_d = [d for d, _, _, f in results if f]
        print(f"FAILED at -d {failed_d} -- reject, no reseed retry per repo-standing rule.")


if __name__ == "__main__":
    main()
