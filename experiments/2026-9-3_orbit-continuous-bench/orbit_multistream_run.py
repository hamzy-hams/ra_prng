"""Continuous-stream (real, syscall-inclusive) MB/s for ra_core_orbit via
orbit_bench.c's --multistream mode. Parameters identical to
../2026-9-3_combo-winner-pareto-selection/winners_bench_run.py's
run_continuous_stream() so the result is apples-to-apples with the existing
singleblock continuous-stream figure (154.8 MB/s) cited in that folder's
RESULTS.md.

Runs the measurement 2x and reports the mean, matching this research line's
established noise-handling protocol for this exact metric ("K=255 and
continuous-stream are the mean of the 2 original runs", per that RESULTS.md
"K=1 measurement robustness" section).

Does not touch ra_core.c or any file under
../2026-9-3_combo-winner-pareto-selection/.
"""
from __future__ import annotations

import json
import subprocess
import time
from pathlib import Path

HERE = Path(__file__).parent
BENCH = HERE / "orbit_bench"

STREAM_BASE_KEY = 111222  # consistent with the rest of this research line
STREAM_N_WORDS = 1_000_000_000  # >= 4GB, matches winners_bench_run.py
STREAM_K = 255


def run_continuous_stream(core: str) -> dict:
    with open("/dev/null", "wb") as devnull:
        t0 = time.perf_counter()
        subprocess.run(
            [str(BENCH), "--multistream", core, str(STREAM_BASE_KEY),
             str(STREAM_N_WORDS), str(STREAM_K)],
            stdout=devnull, check=True,
        )
        t1 = time.perf_counter()
    elapsed = t1 - t0
    mb_per_s = (STREAM_N_WORDS * 4) / elapsed / 1e6
    return {"n_words": STREAM_N_WORDS, "elapsed_s": elapsed, "mb_per_s": mb_per_s}


def main():
    if not BENCH.exists():
        raise SystemExit(f"{BENCH} not found -- compile orbit_bench.c first")

    runs = []
    for i in range(2):
        print(f"Run {i+1}/2 ...")
        r = run_continuous_stream("orbit")
        print(f"  elapsed={r['elapsed_s']:.3f}s mb_per_s={r['mb_per_s']:.1f}")
        runs.append(r)

    mean_mb_per_s = sum(r["mb_per_s"] for r in runs) / len(runs)
    result = {
        "core": "orbit",
        "runs": runs,
        "mean_mb_per_s": mean_mb_per_s,
        "params": {
            "base_key": STREAM_BASE_KEY,
            "n_words": STREAM_N_WORDS,
            "K": STREAM_K,
        },
    }

    out_path = HERE / "orbit_multistream_results.jsonl"
    with open(out_path, "w") as f:
        f.write(json.dumps(result) + "\n")

    print(f"\ncore=orbit mean_mb_per_s={mean_mb_per_s:.1f} "
          f"(runs: {[round(r['mb_per_s'],1) for r in runs]})")
    print(f"Written to {out_path}")


if __name__ == "__main__":
    main()
