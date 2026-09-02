"""Track A of the 2026-09-01 idle-VPS-capacity plan: dieharder "Good"-
reliability battery for `winner_wired_v2` and `winner_wired_addressable`.
Only PractRand had been run on these two generators before -- dieharder
was still an open backlog item (see memory `project_research_backlog_2026_09`
item 1).

Reuses the exact piped-no-file methodology already validated for
`pruned_winner` (`experiments/2026-8-26_operation-pruning-research/RESULTS.md`,
"Quality -- dieharder" section): one dieharder test at a time via direct
pipe (no file, no rewind -- the precedent run showed file+rewind produces
false-FAILs from rewind-count artifacts on a small file). Never edits any
generator source/binary -- only subprocess-drives the existing
`--stream <seed> <n>` CLI.
"""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).parent

# 27 "Good"-reliability dieharder tests (from `dieharder -l`), excluding
# -d 5,6,7 (Suspect) and -d 14 (Do Not Use). Verified identical -d index
# table between the local machine and the VPS (dieharder 3.31.1 both
# sides) before this list was written -- re-verify with `dieharder -l` if
# ever run against a different dieharder version/machine.
GOOD_TESTS = [
    0, 1, 2, 3, 4, 8, 9, 10, 11, 12, 13, 15, 16, 17,
    100, 101, 102,
    200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
]

DEFAULT_WORDS = 300_000_000   # 1.2GB -- matches pruned_winner's default supply
GCD_WORDS = 2_800_000_000     # 11.2GB -- GCD test (-d 17) needs ~10GB+, else EOF

# Per-test dieharder CLI overrides (test index -> list of extra-arg lists,
# one dieharder invocation per entry). `-n` takes a SINGLE ntuple value
# (verified via `dieharder --help` -- NOT a comma-separated list), so RGB
# Generalized Minimum Distance (-d 201, false-FAILs at the default ntup=0)
# needs 4 separate invocations to reproduce the original algorithm's own
# passing methodology (ntup=2..5, see precedent RESULTS.md).
OVERRIDES: dict[int, list[list[str]]] = {
    201: [["-n", "2"], ["-n", "3"], ["-n", "4"], ["-n", "5"]],
}


def run_one(binary: Path, seed: int, d: int, extra_args: list[str]) -> str:
    words = GCD_WORDS if d == 17 else DEFAULT_WORDS
    gen = subprocess.Popen(
        [str(binary), "--stream", str(seed), str(words)],
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


def run_test(binary: Path, seed: int, d: int, out_f) -> str:
    invocations = OVERRIDES.get(d, [[]])
    combined = ""
    for extra_args in invocations:
        out = run_one(binary, seed, d, extra_args)
        combined += out
    header = f"=== -d {d} ==="
    out_f.write(header + "\n" + combined + "\n")
    out_f.flush()
    print(header)
    print(combined.strip()[-400:])
    return combined


def main():
    if len(sys.argv) < 3:
        print(f"usage: {sys.argv[0]} <binary_path> <name> [seed]")
        sys.exit(1)
    binary = Path(sys.argv[1]).resolve()
    name = sys.argv[2]
    seed = int(sys.argv[3]) if len(sys.argv) > 3 else 0
    assert binary.exists(), f"binary not found: {binary}"

    out_path = HERE / f"dieharder_{name}_piped.txt"
    results = []
    with open(out_path, "w") as out_f:
        for d in GOOD_TESTS:
            out = run_test(binary, seed, d, out_f)
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
        print(f"FAILED at -d {failed_d} -- diagnose before concluding a real defect "
              f"(precedent in 2026-8-26_operation-pruning-research/RESULTS.md: two "
              f"apparent FAILs there were both test-invocation artifacts, not RNG "
              f"defects, once the invocation was corrected).")


if __name__ == "__main__":
    main()
