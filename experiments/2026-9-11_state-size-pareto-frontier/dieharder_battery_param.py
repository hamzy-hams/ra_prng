"""experiments/2026-9-11_state-size-pareto-frontier/dieharder_battery_param.py
-- Tier 2 dieharder for the state-size Pareto survivors (Tahap G).

Adapted from ../2026-9-4_orbit-kmin-battery/run_dieharder_battery.py (NOT
editing that file -- its generator CLI contract predates the 2026-09-06
addr_hi widening). That script called `<binary> --stream <key> <n>` (3
args); driver_main_template.c's current --stream is `<base_key> <addr_hi>
<n> <K>` (4 args, see driver_main_template.c:166-171) -- this file threads
addr_hi through the same way practrand_gate.py does:
addr_hi = base_key ^ ADDR_HI_XOR.

Same 27 "Good"-reliability GOOD_TESTS list, same BASE_KEY=111222 (repo
convention, consistency with every other dieharder run here), same
piped-no-file-no-rewind methodology, same GCD (-d 17) word-count override
-- all copied verbatim from the reference script.

Usage:
    python3 dieharder_battery_param.py <binary_path> <name> <K>

`K` is the reinit interval passed as the 4th --stream arg (e.g. 1 for
cold-start, N-1 for steady-state) -- required because the new contract has
no implicit default.
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

BASE_KEY = 111222  # consistent with run_dieharder_battery.py / promotion_search.py
ADDR_HI_XOR = 0x5EEDA55D  # same derivation as tools/battery/practrand_gate.py
ADDR_HI = (BASE_KEY ^ ADDR_HI_XOR) & 0xFFFFFFFF


def run_one(binary: Path, d: int, extra_args: list[str], k: int) -> str:
    words = GCD_WORDS if d == 17 else DEFAULT_WORDS
    gen = subprocess.Popen(
        [str(binary), "--stream", str(BASE_KEY), str(ADDR_HI), str(words), str(k)],
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


def run_test(binary: Path, d: int, k: int, out_f) -> str:
    invocations = OVERRIDES.get(d, [[]])
    combined = ""
    for extra_args in invocations:
        out = run_one(binary, d, extra_args, k)
        combined += out
    header = f"=== -d {d} ==="
    out_f.write(header + "\n" + combined + "\n")
    out_f.flush()
    print(header, flush=True)
    print(combined.strip()[-400:], flush=True)
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
            out = run_test(binary, d, k, out_f)
            results.append((d, "PASSED" in out, "WEAK" in out, "FAILED" in out))

    n_fail = sum(1 for _, _, _, f in results if f)
    n_weak = sum(1 for _, _, w, _ in results if w)
    n_pass = sum(1 for _, p, _, f in results if p and not f)
    print(f"\n=== summary for {name} K={k} ===", flush=True)
    print(f"{n_pass}/{len(GOOD_TESTS)} test invocations show PASSED, "
          f"{n_weak} contain a WEAK sub-result, {n_fail} show FAILED", flush=True)
    print(f"Raw output: {out_path}", flush=True)
    if n_fail:
        failed_d = [d for d, _, _, f in results if f]
        print(f"FAILED at -d {failed_d} -- reject, no reseed retry per repo-standing rule.", flush=True)
    sys.exit(1 if n_fail else 0)


if __name__ == "__main__":
    main()
