"""Promotion tier: runs ON THE VPS (not locally -- 8 K-values x up to 29
candidates x up to 16GB PractRand each). Meant to be started via
`nohup python3 promotion_search.py > promotion_status.log 2>&1 & disown`,
matching this repo's standing VPS convention (VPS_ACCESS.md).

Design (per HANDOVER.md's Promotion tier + user-approved staged refinement,
2026-09-02 check-in): each (candidate, K) pair runs ONE continuous
`RNG_test stdin32 -tlmin 256MB -tlmax 16GB -multithreaded` invocation (same
pattern as ../2026-9-2_singleblock-o-width-fix/run_extended_16gb.sh) piped
from the candidate binary's --stream mode. PractRand accumulates statistics
continuously from byte 0 regardless of -tlmin (tlmin only controls which
checkpoints get REPORTED), so this single invocation naturally produces
checkpoints at 256MB,512MB,1GB,2GB,4GB,8GB,16GB -- no need for two separate
invocations to get a "cheap 8GB pre-filter then confirm 16GB" effect.
Instead: the orchestrator here watches each checkpoint as it streams in and
KILLS the pipeline the moment a checkpoint before 16GB shows a hard
FAIL/very-suspicious anomaly -- saving the remaining compute for the (very
likely) rest of that candidate's failure, without ever re-running or
re-seeding anything (this is an early-stop on an already-committed run, not
a retry). A candidate that stays clean (or only soft-anomaly) all the way
runs to the full 16GB, exactly per HANDOVER's target scale.

w8_f0_i0 (width=8, empty finalizer) is EXCLUDED from this run: it is
structurally identical to ../2026-9-2_singleblock-o-width-fix's "wideo"
Kandidat A control, which that experiment already ran to 16GB and which
FAILED (K=1 TMFn FAIL, K=2/K=4 BCFN FAIL) -- see that folder's RESULTS.md.
Re-running it here would waste VPS compute on an already-known result;
RESULTS.md in this folder cites that prior data directly instead.

Anomaly detection uses the same bugfixed regex as tier1_search.py
(lowercase-aware; scoped to this new script only, per user decision).

No re-seed retries: base_key is fixed at 111222 throughout (same convention
as every prior experiment in this line); a K's FAIL is final.
"""

from __future__ import annotations

import concurrent.futures
import json
import re
import subprocess
import sys
import time
from pathlib import Path

PRACTRAND_BIN = Path.home() / "Documents/research/PractRand/RNG_test"
BASE_KEY = 111222
N_WORDS_16GB = 4294967296  # 16GB / 4 bytes
K_VALUES = [1, 2, 4, 8, 16, 32, 64, 96]
MAX_PARALLEL = 2  # VPS spec: 8 cores, RNG_test -multithreaded is the bottleneck

HERE = Path(__file__).parent
CAND_DIR = HERE / "candidates"
LOG_PATH = HERE / "promotion_log.jsonl"
LOGS_DIR = HERE / "promotion_logs"
LOGS_DIR.mkdir(exist_ok=True)

FAIL_RE = re.compile(r"\bFAIL\b|very suspicious")
SOFT_ANOMALY_RE = re.compile(r"\bmildly suspicious\b|\bsuspicious\b|\bunusual\b")
LENGTH_RE = re.compile(r"length=\s*([^,]+),")

# 29 Tier-1 survivors, w8_f0_i0 excluded (see module docstring).
SURVIVORS = [
    ("w2_f14_i0", 2, 14), ("w2_f17_i0", 2, 17), ("w2_f31_i0", 2, 31),
    ("w4_f22_i0", 4, 22), ("w4_f25_i0", 4, 25), ("w4_f29_i0", 4, 29),
    ("w8_f1_i0", 8, 1), ("w8_f2_i0", 8, 2), ("w8_f3_i0", 8, 3),
    ("w8_f4_i0", 8, 4), ("w8_f5_i0", 8, 5), ("w8_f8_i0", 8, 8),
    ("w8_f9_i0", 8, 9), ("w8_f10_i0", 8, 10), ("w8_f11_i0", 8, 11),
    ("w8_f12_i0", 8, 12), ("w8_f14_i0", 8, 14), ("w8_f17_i0", 8, 17),
    ("w8_f22_i0", 8, 22), ("w8_f23_i0", 8, 23), ("w8_f24_i0", 8, 24),
    ("w8_f25_i0", 8, 25), ("w8_f26_i0", 8, 26), ("w8_f27_i0", 8, 27),
    ("w8_f28_i0", 8, 28), ("w8_f29_i0", 8, 29), ("w8_f31_i0", 8, 31),
    ("w8_f33_i0", 8, 33), ("w8_f34_i0", 8, 34),
]
assert len(SURVIVORS) == 29


def _log(event: dict):
    event["ts"] = time.time()
    with LOG_PATH.open("a") as f:
        f.write(json.dumps(event) + "\n")
        f.flush()


def compile_candidate(cid: str) -> Path:
    bin_path = CAND_DIR / cid
    c_path = CAND_DIR / f"{cid}.c"
    subprocess.run(
        ["gcc", "-O3", "-march=native", "-std=gnu17", "-include", "stdalign.h",
         str(c_path), "-o", str(bin_path), "-Wall", "-Wextra"],
        check=True, capture_output=True, text=True,
    )
    return bin_path


def iter_checkpoints(stream):
    """Yield one full checkpoint block (str) at a time from RNG_test's
    stdout, as lines arrive (live, not buffered at the end)."""
    current = []
    for raw in stream:
        line = raw.decode(errors="replace") if isinstance(raw, bytes) else raw
        if line.strip() == "":
            if current:
                yield "".join(current)
                current = []
            continue
        current.append(line)
    if current:
        yield "".join(current)


def run_one(cid: str, bin_path: Path, K: int) -> dict:
    log_path = LOGS_DIR / f"{cid}_K{K}_16GB.log"
    t0 = time.time()
    gen = subprocess.Popen(
        [str(bin_path), "--stream", str(BASE_KEY), str(N_WORDS_16GB), str(K)],
        stdout=subprocess.PIPE,
    )
    test = subprocess.Popen(
        [str(PRACTRAND_BIN), "stdin32", "-tlmin", "256MB", "-tlmax", "16GB", "-multithreaded"],
        stdin=gen.stdout, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, bufsize=1,
    )
    gen.stdout.close()

    checkpoints = []
    killed_early = False
    with log_path.open("w") as logf:
        for block in iter_checkpoints(test.stdout):
            logf.write(block + "\n\n")
            logf.flush()
            m = LENGTH_RE.search(block)
            length_str = m.group(1).strip() if m else "?"
            hard_fail = bool(FAIL_RE.search(block))
            soft_hits = sorted(set(SOFT_ANOMALY_RE.findall(block)))
            checkpoints.append({"length": length_str, "hard_fail": hard_fail,
                                 "soft_anomaly_kinds": soft_hits})
            is_final = "16 gigabyte" in block or "16 giga" in block.lower()
            if hard_fail and not is_final:
                test.terminate()
                gen.terminate()
                killed_early = True
                logf.write(f"\n[EARLY KILL after {length_str}: hard FAIL detected]\n")
                break

    test.wait(timeout=10) if test.poll() is None else None
    gen.wait(timeout=10) if gen.poll() is None else None
    elapsed = time.time() - t0

    last = checkpoints[-1] if checkpoints else {"length": "?", "hard_fail": True, "soft_anomaly_kinds": []}
    result = {
        "id": cid, "K": K, "checkpoints": checkpoints,
        "killed_early": killed_early, "final_length": last["length"],
        "final_hard_fail": last["hard_fail"],
        "clean_to_16gb": (not killed_early) and (not last["hard_fail"]) and "16 giga" in last["length"].lower(),
        "elapsed_s": elapsed,
    }
    _log({"event": "result", **{k: v for k, v in result.items() if k != "checkpoints"},
          "n_checkpoints": len(checkpoints)})
    return result


def run_candidate_all_k(cid: str, width: int, finalizer_idx: int) -> list[dict]:
    bin_path = compile_candidate(cid)
    _log({"event": "compiled", "id": cid})
    results = []
    for K in K_VALUES:
        r = run_one(cid, bin_path, K)
        results.append(r)
        status = "KILLED-FAIL" if r["killed_early"] else ("CLEAN-16GB" if r["clean_to_16gb"] else "FAIL-AT-FINAL")
        print(f"{cid} K={K}: {status} (final={r['final_length']}, {r['elapsed_s']:.0f}s)", flush=True)
        if r["killed_early"] or r["final_hard_fail"]:
            # No re-seed retry: this candidate is rejected outright. Per
            # HANDOVER's request to still characterize the full K range for
            # RESULTS.md completeness, we DO continue to the next K value
            # (not abort the whole candidate) rather than stopping the
            # candidate's sweep entirely.
            continue
    return results


def main():
    _log({"event": "start", "n_candidates": len(SURVIVORS), "k_values": K_VALUES,
          "base_key": BASE_KEY, "max_parallel": MAX_PARALLEL})
    all_results = {}
    with concurrent.futures.ThreadPoolExecutor(max_workers=MAX_PARALLEL) as ex:
        futures = {ex.submit(run_candidate_all_k, cid, w, f): cid for cid, w, f in SURVIVORS}
        for fut in concurrent.futures.as_completed(futures):
            cid = futures[fut]
            try:
                all_results[cid] = fut.result()
            except Exception as e:
                _log({"event": "error", "id": cid, "error": str(e)})
                print(f"ERROR {cid}: {e}", flush=True)

    n_clean = sum(1 for rs in all_results.values() if all(r["clean_to_16gb"] for r in rs))
    _log({"event": "done", "n_candidates": len(all_results), "n_fully_clean": n_clean})
    print(f"\n=== PROMOTION TIER DONE: {n_clean}/{len(all_results)} candidates clean across ALL K to 16GB ===", flush=True)
    for cid, rs in all_results.items():
        if all(r["clean_to_16gb"] for r in rs):
            print(f"  FULLY CLEAN: {cid}", flush=True)


if __name__ == "__main__":
    main()
