"""Step 7 (PRODUCTION_READINESS_HANDOVER.md) -- score `scrambler_ra_core_
singleblock`'s K=1/K=255 shuffle output using the exact same 4 scoring
functions as `../2026-8-30_addressable-shuffle/scc_test.py` (entropy,
chi-square-vs-uniform, runs test, serial correlation). Those 4 functions
are pure (only depend on the token list), so they're imported unmodified
rather than re-derived -- the "multi-key distinctness" bonus check in that
file's __main__ depends on `scrambler_addressable`/`ra_shuffle` (a
different formula) and is intentionally NOT reused here.

PASS criterion follows that folder's own established convention
(RESULTS.md): |z| < 2 for the runs test, |r| small (~0) for serial
correlation, chi-square close to 0 relative to its own degrees of freedom,
entropy close to the theoretical max for N distinct symbols (log2(N)).
"""

from __future__ import annotations

import sys
from math import log2
from pathlib import Path

HERE = Path(__file__).parent
SCC_DIR = HERE.parent / "2026-8-30_addressable-shuffle"
sys.path.insert(0, str(SCC_DIR))

import types
# scc_test.py imports `ra_shuffle` at module scope for its own __main__ bonus
# check (a different formula, out of scope here) -- load only the pure
# functions we need (read_tokens through serial_correlation) rather than
# importing the whole module, so that unrelated import never triggers.
mod = types.ModuleType("scc_scoring_functions")
src = (SCC_DIR / "scc_test.py").read_text()
keep_start = src.index("def read_tokens")
keep_end = src.index("def multi_key_distinctness_check")
exec(compile("import math\nfrom collections import Counter\n" + src[keep_start:keep_end], "scc_test.py", "exec"), mod.__dict__)


def score(path: Path, n_symbols: int, label: str):
    data = mod.read_tokens(str(path))
    H = mod.entropy(data)
    chi2 = mod.chi_square_uniform(data)
    z, runs, exp_runs = mod.runs_test(data)
    r = mod.serial_correlation(data)
    h_max = log2(n_symbols)

    print(f"=== {label} (n={len(data)} tokens) ===")
    print(f"  Entropy: {H:.6f} bit (max for N={n_symbols}: {h_max:.6f})")
    print(f"  Chi-Square: {chi2:.6f} (df={n_symbols - 1})")
    print(f"  Runs test: z={z:.4f} (runs={runs}, expected~{exp_runs:.2f})")
    print(f"  Serial correlation: {r:.6f}")

    ok_entropy = H > h_max - 0.05
    ok_chi2 = chi2 < 3 * (n_symbols - 1)  # generous: ~3x expected chi2 mean (df) as a loose bound
    ok_runs = abs(z) < 3.0
    ok_corr = abs(r) < 0.01
    verdict = "PASS" if (ok_entropy and ok_chi2 and ok_runs and ok_corr) else "FAIL"
    print(f"  -> {verdict} (entropy={ok_entropy}, chi2={ok_chi2}, runs={ok_runs}, corr={ok_corr})")
    return verdict == "PASS"


def main():
    n_symbols = 255
    p1 = score(HERE / "shuffled_k255.txt", n_symbols, "K=255 (one call/rep)")
    print()
    p2 = score(HERE / "shuffled_k1.txt", n_symbols, "K=1 (fresh reinit/swap-step)")
    print()
    print(f"Overall Step 7: {'PASS' if (p1 and p2) else 'FAIL'}")


if __name__ == "__main__":
    main()
