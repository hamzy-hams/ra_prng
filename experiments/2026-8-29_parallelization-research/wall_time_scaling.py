"""Q3(a): wall-clock speedup/efficiency when running N independent
`winner_wired_v2 --stream <seed> <n>` processes concurrently, each with its
own seed (0..N-1) and no shared state/lock -- per HANDOVER.md's question 3
("seberapa dekat generator ini ke scaling linear di multi-core").

Run BEFORE any CPU-heavy job (PractRand in particular) -- perf/wall-time
numbers are only meaningful on an idle machine (repo convention, see
other_winners_perf.sh / perf_state_update.sh comments).
"""

from __future__ import annotations

import json
import subprocess
import time

from common import HERE, TIERS_Q3, WINNER_BIN


def run_n_concurrent(n: int, n_per_proc: int) -> float:
    """Launch n concurrent --stream processes (seeds 0..n-1), each emitting
    n_per_proc values to /dev/null. Return wall-clock seconds for all to finish.
    """
    t0 = time.perf_counter()
    procs = [
        subprocess.Popen(
            [str(WINNER_BIN), "--stream", str(seed), str(n_per_proc)],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        )
        for seed in range(n)
    ]
    for p in procs:
        rc = p.wait()
        assert rc == 0, f"process exited with {rc}"
    t1 = time.perf_counter()
    return t1 - t0


def main():
    n_per_proc = TIERS_Q3["n_per_proc"]
    results = {}
    throughput_1 = None
    print(f"n_per_proc={n_per_proc:,}")
    for n in TIERS_Q3["n_values"]:
        wall = run_n_concurrent(n, n_per_proc)
        throughput = n * n_per_proc / wall
        if n == 1:
            throughput_1 = throughput
        speedup = throughput / throughput_1
        efficiency = speedup / n
        results[n] = {
            "wall_seconds": wall,
            "throughput_values_per_sec": throughput,
            "speedup": speedup,
            "efficiency": efficiency,
        }
        print(f"N={n}: wall={wall:.3f}s throughput={throughput:,.0f} val/s "
              f"speedup={speedup:.3f} efficiency={efficiency:.3f}")

    out_path = HERE / "wall_time_scaling_results.json"
    out_path.write_text(json.dumps(results, indent=2))
    print(f"\nWrote {out_path}")


if __name__ == "__main__":
    main()
