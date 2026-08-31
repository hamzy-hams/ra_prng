"""Tahap 3, Q1 Method B (re-pointed): interleave K independent
`winner_wired_addressable --stream <key> <n>` streams and run PractRand on
the merged output, adapted from
../2026-8-29_parallelization-research/interleave_practrand.py.

Never modifies interleave_practrand.py in place. `run_interleave_practrand()`
reads `WINNER_BIN` as a genuine module-global inside its function body
(looked up at call time), so monkeypatching
`interleave_practrand.WINNER_BIN` after import correctly re-points it.

Does NOT call interleave_practrand.main() (it writes results via `HERE` from
common.py, which always resolves to the 2026-8-29 folder) -- this script
writes its own results into this folder instead.

Staged like the original: smoke (64MB) -> medium (1GB) -> full (16GB),
each gated on the previous stage being clean. `xlarge` (128GB) is NOT run
by default here -- pass it explicitly once full is clean, since it can take
a long time on this machine (4 cores, ~4GB RAM available); run it in the
background separately.
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent / "2026-8-29_parallelization-research"))
import interleave_practrand  # noqa: E402
from common import TIERS_Q1B  # noqa: E402

HERE = Path(__file__).parent
ADDR_BIN = HERE / "winner_wired_addressable"
interleave_practrand.WINNER_BIN = ADDR_BIN  # re-point the reused worker function

GATED_TIERS = ["smoke", "medium", "full"]  # xlarge run separately, not gated here


def main():
    tiers = sys.argv[1:] or GATED_TIERS
    for tier in tiers:
        k, total_bytes = TIERS_Q1B[tier]
        print(f"=== tahap3_interleave_practrand tier={tier}: K={k} "
              f"total={total_bytes/1024/1024:.0f}MB (addressable) ===")
        result = interleave_practrand.run_interleave_practrand(k, total_bytes)
        print(result["stdout"][-2000:])
        if result["stderr"]:
            print("--- stderr ---")
            print(result["stderr"][-2000:])

        out_path = HERE / f"tahap3_interleave_practrand_results_{tier}.json"
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
