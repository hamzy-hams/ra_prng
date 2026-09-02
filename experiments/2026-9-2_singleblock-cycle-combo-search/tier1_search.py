"""Tier 1: multikey PractRand staged gate, K=1 (worst-case documented in
../2026-9-2_singleblock-k-threshold-characterization/RESULTS.md).

Driver over Tier 0 survivors (108 candidates unless Tier 0 rejected some):
  1. sanity-check (already done at build time by combo_gen.generate_all_tier01,
     re-verified here defensively) -- C vs combo_prng.py oracle.
  2. Stage 1 (quick-reject), 256MB, base_key=111222, K=1.
  3. Stage 2 (survivor recheck), 2GB, same generator/base_key -- only run for
     Stage 1 survivors.

Staged-size reasoning (stated explicitly per HANDOVER.md's requirement):
  - 256MB: k-threshold-characterization/RESULTS.md shows the UNFIXED
    baseline defect for K in {1,2,4,8} already produces large R (tens to
    thousands) well before 1GB -- a candidate still essentially broken
    should show strong signal by 256MB. Empirically ~8s/candidate here
    (measured directly, see session shell log), so 108 candidates is a few
    minutes of wall time.
  - 2GB: reused directly from ../2026-9-2_singleblock-o-width-fix's own
    Kandidat B triage stage -- the single most relevant precedent (same
    defect/topology: multikey K=1, singleblock). Explicitly NOT sufficient
    on its own for a final verdict: that same triage stage passed 9/9
    N-values cleanly at 2GB, and 3 of those subsequently FAILed at 16GB --
    2GB here is only a second-cut volume filter before the expensive
    Promotion Tier (16GB on the VPS), never a final verdict.

Anomaly detection bugfix (found while reading quality_gate.py/
operand_search.py during planning): both of those scripts grep for literal
uppercase "SUSPICIOUS", but RNG_test's actual Evaluation column uses
lowercase ("mildly suspicious", "unusual", "suspicious", "very suspicious")
-- confirmed by grepping this repo's own PractRand .log files. That
uppercase-only check silently never matches any suspicious-tier line, only
"FAIL". Fixed HERE (scoped to this new script only, per explicit user
decision -- old scripts are left untouched): reject on `\bFAIL\b` or
"very suspicious"; log (but do not auto-reject on) "mildly suspicious" /
"suspicious" / "unusual" counts for human review in RESULTS.md.

No re-seed retries on any Stage 1/2 FAIL, per repo-standing rule -- one
FAIL is final, reject and move on.
"""

from __future__ import annotations

import json
import re
import subprocess
import time
from pathlib import Path

from recipes import candidate_id, candidate_describe
from combo_gen import CAND_DIR

PRACTRAND_BIN = Path.home() / "Documents/research/PractRand/RNG_test"
BASE_KEY = 111222
LOG_PATH = Path(__file__).parent / "combo_search_log.jsonl"

STAGE1_MB = 256
STAGE2_MB = 2048

FAIL_RE = re.compile(r"\bFAIL\b|very suspicious")
SOFT_ANOMALY_RE = re.compile(r"\bmildly suspicious\b|\bsuspicious\b|\bunusual\b")


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
        [str(bin_path), "--stream", str(BASE_KEY), str(n_words), "1"],
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


def run_candidate(width: int, finalizer_idx: int) -> dict:
    cid = candidate_id(width, finalizer_idx, 0)
    bin_path = CAND_DIR / cid
    desc = candidate_describe(width, finalizer_idx, 0)

    t0 = time.time()
    stage1 = run_practrand(bin_path, STAGE1_MB)
    elapsed1 = time.time() - t0
    _log({"event": "stage1", "id": cid, "description": desc,
          "passed": stage1["passed"], "hard_fail": stage1["hard_fail"],
          "soft_anomaly_count": stage1["soft_anomaly_count"],
          "soft_anomaly_kinds": stage1["soft_anomaly_kinds"],
          "elapsed_s": elapsed1})

    if not stage1["passed"]:
        return {"id": cid, "width": width, "finalizer_idx": finalizer_idx,
                "description": desc, "stage1": stage1, "stage2": None, "survivor": False}

    t1 = time.time()
    stage2 = run_practrand(bin_path, STAGE2_MB)
    elapsed2 = time.time() - t1
    _log({"event": "stage2", "id": cid, "description": desc,
          "passed": stage2["passed"], "hard_fail": stage2["hard_fail"],
          "soft_anomaly_count": stage2["soft_anomaly_count"],
          "soft_anomaly_kinds": stage2["soft_anomaly_kinds"],
          "elapsed_s": elapsed2})

    return {"id": cid, "width": width, "finalizer_idx": finalizer_idx,
            "description": desc, "stage1": stage1, "stage2": stage2,
            "survivor": stage2["passed"]}


def main(tier0_survivors_path: Path):
    entries = [json.loads(l) for l in tier0_survivors_path.read_text().splitlines() if l.strip()]
    _log({"event": "start", "n_candidates": len(entries), "stage1_mb": STAGE1_MB,
          "stage2_mb": STAGE2_MB, "base_key": BASE_KEY})

    results = []
    survivors = []
    for e in entries:
        r = run_candidate(e["width"], e["finalizer_idx"])
        results.append(r)
        if r["survivor"]:
            survivors.append(r)
        tag = "SURVIVOR" if r["survivor"] else ("stage1-FAIL" if not r["stage1"]["passed"] else "stage2-FAIL")
        print(f"{r['id']}: {tag}")

    _log({"event": "done", "n_total": len(results), "n_survivors": len(survivors),
          "survivor_ids": [r["id"] for r in survivors]})

    print(f"\nTier 1 done: {len(survivors)}/{len(results)} survived Stage1+Stage2.")
    for r in survivors:
        print(f"  SURVIVOR {r['id']}: {r['description']}")
    return results, survivors


if __name__ == "__main__":
    main(Path(__file__).parent / "tier0_survivors.jsonl")
