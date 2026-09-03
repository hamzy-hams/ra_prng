"""Staged PractRand screening for the 12 inject-crossing candidates
(w8_f10_i1..i6, w8_f28_i1..i6), at K=96 -- NOT K=1.

Why K=96 and not the original Tier1's K=1: recipes.py/combo_prng.py prove
extra_inject (slot 3) is byte-identical to "off" at K=1 (the inject write
happens after the single round's output and the loop breaks right after).
Running the original K=1 Tier0/Tier1 methodology against these candidates
would just reproduce i0's already-known-clean results -- not a real test.
K=96 is the safe-K floor recommended in
../2026-9-2_singleblock-k-threshold-characterization/RESULTS.md, so it's a
representative production K where inject is actually observable.

Staged sizes reused directly from tier1_search.py's own reasoning (256MB
quick-reject, 2GB survivor recheck) -- this is a volume-only screen (no
avalanche-gate equivalent exists for K>1), not a final verdict: only
Promotion Tier (16GB, K in {1,2,4,8,16,32,64,96}) is final.

No re-seed retries on any Stage 1/2 FAIL, per repo-standing rule.
"""

from __future__ import annotations

import json
import re
import time
from pathlib import Path
import subprocess

HERE = Path(__file__).parent
CAND_DIR = HERE / "candidates"
PRACTRAND_BIN = Path.home() / "Documents/research/PractRand/RNG_test"
BASE_KEY = 111222
K = 96
LOG_PATH = HERE / "inject_crossing_log.jsonl"
SURVIVORS_PATH = HERE / "staged_survivors.jsonl"

STAGE1_MB = 256
STAGE2_MB = 2048

FAIL_RE = re.compile(r"\bFAIL\b|very suspicious")
SOFT_ANOMALY_RE = re.compile(r"\bmildly suspicious\b|\bsuspicious\b|\bunusual\b")

CANDIDATES = [
    f"w8_f{f}_i{i}"
    for f in (10, 28)
    for i in (1, 2, 3, 4, 5, 6)
]


def _log(event: dict):
    event["ts"] = time.time()
    with LOG_PATH.open("a") as f:
        f.write(json.dumps(event) + "\n")


def n_words_for_mb(mb: int) -> int:
    return (mb * 1024 * 1024) // 4


def run_practrand(bin_path: Path, mb: int) -> dict:
    n_words = n_words_for_mb(mb)
    length_arg = f"{mb}MB"
    gen = subprocess.Popen(
        [str(bin_path), "--stream", str(BASE_KEY), str(n_words), str(K)],
        stdout=subprocess.PIPE,
    )
    test = subprocess.run(
        [str(PRACTRAND_BIN), "stdin32", "-tlmin", length_arg, "-tlmax", length_arg,
         "-multithreaded"],
        stdin=gen.stdout, capture_output=True,
    )
    gen.stdout.close()
    gen.wait()
    stdout = test.stdout.decode(errors="replace")

    hard_fail = bool(FAIL_RE.search(stdout))
    soft_hits = SOFT_ANOMALY_RE.findall(stdout)
    return {
        "passed": not hard_fail,
        "mb": mb,
        "hard_fail": hard_fail,
        "soft_anomaly_count": len(soft_hits),
        "soft_anomaly_kinds": sorted(set(soft_hits)),
        "stdout_tail": stdout[-2000:],
    }


def run_candidate(cid: str) -> dict:
    bin_path = CAND_DIR / cid

    t0 = time.time()
    stage1 = run_practrand(bin_path, STAGE1_MB)
    elapsed1 = time.time() - t0
    _log({"event": "stage1", "id": cid, "k": K,
          "passed": stage1["passed"], "hard_fail": stage1["hard_fail"],
          "soft_anomaly_count": stage1["soft_anomaly_count"],
          "soft_anomaly_kinds": stage1["soft_anomaly_kinds"],
          "elapsed_s": elapsed1})

    if not stage1["passed"]:
        return {"id": cid, "stage1": stage1, "stage2": None, "survivor": False}

    t1 = time.time()
    stage2 = run_practrand(bin_path, STAGE2_MB)
    elapsed2 = time.time() - t1
    _log({"event": "stage2", "id": cid, "k": K,
          "passed": stage2["passed"], "hard_fail": stage2["hard_fail"],
          "soft_anomaly_count": stage2["soft_anomaly_count"],
          "soft_anomaly_kinds": stage2["soft_anomaly_kinds"],
          "elapsed_s": elapsed2})

    return {"id": cid, "stage1": stage1, "stage2": stage2, "survivor": stage2["passed"]}


def main():
    _log({"event": "start", "n_candidates": len(CANDIDATES), "k": K,
          "stage1_mb": STAGE1_MB, "stage2_mb": STAGE2_MB, "base_key": BASE_KEY})

    results = []
    survivors = []
    for cid in CANDIDATES:
        r = run_candidate(cid)
        results.append(r)
        if r["survivor"]:
            survivors.append(r)
        tag = "SURVIVOR" if r["survivor"] else ("stage1-FAIL" if not r["stage1"]["passed"] else "stage2-FAIL")
        print(f"{cid}: {tag}")

    with SURVIVORS_PATH.open("w") as f:
        for r in survivors:
            f.write(json.dumps({"id": r["id"]}) + "\n")

    _log({"event": "done", "n_total": len(results), "n_survivors": len(survivors),
          "survivor_ids": [r["id"] for r in survivors]})

    print(f"\nStaged screening (K={K}) done: {len(survivors)}/{len(results)} survived Stage1+Stage2.")
    for r in survivors:
        print(f"  SURVIVOR {r['id']}")
    return results, survivors


if __name__ == "__main__":
    main()
