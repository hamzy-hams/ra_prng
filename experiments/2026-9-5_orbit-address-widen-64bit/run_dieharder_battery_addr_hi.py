"""experiments/2026-9-5_orbit-address-widen-64bit -- Tahap 6: dieharder
untuk driver multikey_stream_addr_hi (kandidat 2-param key,addr_hi).

Adaptasi dari
../2026-9-4_orbit-kmin-battery/run_dieharder_battery.py: SAMA PERSIS 27
"Good"-reliability test list, SAMA BASE_KEY, SAMA metodologi piped-no-file.
Satu-satunya beda: generator CLI kontraknya
`<binary> --stream <base_key> <base_addr_hi> <block_len> <order> <n>`
(bukan `<binary> --stream <base_key> <n>`) -- base_addr_hi/block_len/order
di-fix lewat CLI argumen skrip ini, bukan hardcoded, supaya satu skrip bisa
dipakai untuk K=1/K=255 x weyl/hashed x kandidat A/B tanpa duplikasi.
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

BASE_KEY = 111222       # konsisten dengan seluruh dieharder run repo ini
BASE_ADDR_HI = 999888   # arbitrer, konstan lintas semua -d test


def run_one(binary: Path, block_len: int, order: str, d: int, extra_args: list[str]) -> str:
    words = GCD_WORDS if d == 17 else DEFAULT_WORDS
    gen = subprocess.Popen(
        [str(binary), "--stream", str(BASE_KEY), str(BASE_ADDR_HI),
         str(block_len), order, str(words)],
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


def run_test(binary: Path, block_len: int, order: str, d: int, out_f) -> str:
    invocations = OVERRIDES.get(d, [[]])
    combined = ""
    for extra_args in invocations:
        out = run_one(binary, block_len, order, d, extra_args)
        combined += out
    header = f"=== -d {d} ==="
    out_f.write(header + "\n" + combined + "\n")
    out_f.flush()
    print(header)
    print(combined.strip()[-400:])
    return combined


def main():
    if len(sys.argv) < 5:
        print(f"usage: {sys.argv[0]} <binary_path> <block_len> <weyl|hashed> <name>")
        sys.exit(1)
    binary = Path(sys.argv[1]).resolve()
    block_len = int(sys.argv[2])
    order = sys.argv[3]
    name = sys.argv[4]
    assert binary.exists(), f"binary not found: {binary}"

    out_path = HERE / f"dieharder_{name}_piped.txt"
    results = []
    with open(out_path, "w") as out_f:
        for d in GOOD_TESTS:
            out = run_test(binary, block_len, order, d, out_f)
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
