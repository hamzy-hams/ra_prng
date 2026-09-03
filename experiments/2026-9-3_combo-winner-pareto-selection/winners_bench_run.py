"""Axis A orchestrator (HANDOVER.md sect 2): K=1 / K=255 reinit-sweep and
continuous-stream MB/s for all 12 winners_bench.c CORES[] entries (11
combo-search winners + the unmodified singleblock baseline).

Writes axis_a_results.jsonl (one record per core) and prints a summary
table. Does not touch ra_core.c or any file under
../2026-9-2_singleblock-cycle-combo-search/.
"""
from __future__ import annotations

import json
import subprocess
import time
from pathlib import Path

HERE = Path(__file__).parent
BENCH = HERE / "winners_bench"

CORES = [
    "singleblock",
    "w8_f8_i0", "w8_f9_i0", "w8_f10_i0",
    "w8_f22_i0", "w8_f24_i0", "w8_f25_i0",
    "w8_f26_i0", "w8_f27_i0", "w8_f28_i0", "w8_f29_i0",
    "w8_f33_i0",
]

REINIT_CYCLES_TARGET_WORDS = 100_000_000
STREAM_BASE_KEY = 111222  # consistent with the rest of this research line
STREAM_N_WORDS = 1_000_000_000  # >= 4GB per HANDOVER sect 2
STREAM_K = 255


def run_reinit_sweep(core: str, k: int) -> dict:
    out = subprocess.run(
        [str(BENCH), "reinit-sweep", core, str(REINIT_CYCLES_TARGET_WORDS), str(k)],
        capture_output=True, text=True, check=True,
    ).stdout.strip()
    # "core <name> K <k> cycles <c> call_ns <x> ns_per_word_steadystate <y> checksum <z>"
    parts = out.split()
    d = {parts[i]: parts[i + 1] for i in range(0, len(parts), 2)}
    return {
        "k": int(d["K"]),
        "cycles": int(d["cycles"]),
        "call_ns": float(d["call_ns"]),
        "ns_per_word": float(d["ns_per_word_steadystate"]),
    }


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
        raise SystemExit(f"{BENCH} not found -- compile winners_bench.c first")

    results = []
    for core in CORES:
        k1 = run_reinit_sweep(core, 1)
        k255 = run_reinit_sweep(core, 255)
        stream = run_continuous_stream(core)
        rec = {"core": core, "k1": k1, "k255": k255, "continuous_stream": stream}
        results.append(rec)
        print(f"{core:14s} K=1 {k1['ns_per_word']:8.3f} ns/word   "
              f"K=255 {k255['ns_per_word']:8.4f} ns/word   "
              f"stream {stream['mb_per_s']:9.1f} MB/s")

    out_path = HERE / "axis_a_results.jsonl"
    with open(out_path, "w") as f:
        for rec in results:
            f.write(json.dumps(rec) + "\n")
    print(f"\nwrote {out_path}")


if __name__ == "__main__":
    main()
