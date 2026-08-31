"""Checkpoint follow-up (2026-08-31), Q1 Method B re-pointed at `ra_prng2.c`
(paper-exact, `src/ra_prng2/c/ra_prng2.c`) instead of `winner_wired_v2`.

Never modifies interleave_practrand.py in place. `run_interleave_practrand()`
reads `WINNER_BIN` as a module-global inside its function body (looked up
at call time), so monkeypatching `interleave_practrand.WINNER_BIN` after
import correctly re-points it -- same technique as
`../2026-8-30_addressable-init-research/tahap3_interleave_practrand.py`.

Scope: smoke (64MB) -> medium (1GB) only. `full`/`xlarge`/`1tb` are NOT run
here -- the independence question this answers (does pruning+wiring change
cross-stream nonlinear dependence vs. the paper-exact original) only needs
detection at moderate scale; `winner_wired_v2`'s own depth validation
(16GB/128GB/1TB) already covers production-scale statistical quality and
isn't being re-litigated for `ra_prng2.c` here.
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

import interleave_practrand  # noqa: E402
from common import TIERS_Q1B, RA_PRNG2_BIN, ensure_ra_prng2_cli  # noqa: E402

HERE = Path(__file__).parent
interleave_practrand.WINNER_BIN = RA_PRNG2_BIN  # re-point the reused worker function

GATED_TIERS = ["smoke", "medium"]


def main():
    ensure_ra_prng2_cli()
    tiers = sys.argv[1:] or GATED_TIERS
    for tier in tiers:
        k, total_bytes = TIERS_Q1B[tier]
        print(f"=== interleave_practrand_ra_prng2 tier={tier}: K={k} "
              f"total={total_bytes/1024/1024:.0f}MB (ra_prng2 paper-exact) ===")
        result = interleave_practrand.run_interleave_practrand(k, total_bytes)
        print(result["stdout"][-2000:])
        if result["stderr"]:
            print("--- stderr ---")
            print(result["stderr"][-2000:])

        out_path = HERE / f"interleave_practrand_ra_prng2_results_{tier}.json"
        out_path.write_text(json.dumps(
            {kk: vv for kk, vv in result.items() if kk != "stdout"} | {"stdout_tail": result["stdout"][-4000:]},
            indent=2,
        ))
        print(f"Wrote {out_path}")

        if result["passed"]:
            print(f"tier={tier}: PASSED (no FAIL/SUSPICIOUS, real test-result lines present)\n")
        else:
            status = "RUN FAILED (I/O error / no valid test-result lines)" if (not result["ran_real_test"] or result["io_error"]) else "ANOMALY DETECTED"
            print(f"tier={tier}: {status}")
            print("Stopping -- not proceeding to the next tier.")
            sys.exit(1)


if __name__ == "__main__":
    main()
