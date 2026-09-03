"""Promotion tier for the 12 inject-crossing candidates (w8_f10_i1..i6,
w8_f28_i1..i6) -- PRODUCTION_READINESS_HANDOVER.md action item 2. Runs ON
THE VPS, adapted verbatim from
../2026-9-2_singleblock-cycle-combo-search/promotion_search.py (same
early-kill-on-hard-FAIL, live-checkpoint, ThreadPoolExecutor(2) pattern for
the 8-core VPS). Meant to be started via
`nohup python3 promotion_search_inject.py > promotion_status.log 2>&1 & disown`.

K=1 is DELIBERATELY OMITTED from K_VALUES here: recipes.py/combo_prng.py
(../2026-9-2_singleblock-cycle-combo-search/) prove extra_inject is
byte-identical to "off" at K=1 (the inject write happens after the single
round's output, and the loop breaks right after at K=1). w8_f10_i0 and
w8_f28_i0 already ran clean at K=1 in the original Promotion Tier
(../2026-9-2_singleblock-cycle-combo-search/promotion_logs/), so i1..i6's
K=1 result is already known without spending compute on it.

No re-seed retries: base_key fixed at 111222 throughout.
"""

from __future__ import annotations

import concurrent.futures
import json
import re
import subprocess
import time
from pathlib import Path

PRACTRAND_BIN = Path.home() / "Documents/research/PractRand/RNG_test"
BASE_KEY = 111222
N_WORDS_16GB = 4294967296  # 16GB / 4 bytes
K_VALUES = [2, 4, 8, 16, 32, 64, 96]  # K=1 omitted, see module docstring
MAX_PARALLEL = 2  # VPS spec: 8 cores, RNG_test -multithreaded is the bottleneck

HERE = Path(__file__).parent
CAND_DIR = HERE / "candidates"
LOG_PATH = HERE / "promotion_log_inject.jsonl"
LOGS_DIR = HERE / "promotion_logs_inject"
LOGS_DIR.mkdir(exist_ok=True)

FAIL_RE = re.compile(r"\bFAIL\b|very suspicious")
SOFT_ANOMALY_RE = re.compile(r"\bmildly suspicious\b|\bsuspicious\b|\bunusual\b")
LENGTH_RE = re.compile(r"length=\s*([^,]+),")

CANDIDATES = [f"w8_f{f}_i{i}" for f in (10, 28) for i in (1, 2, 3, 4, 5, 6)]
assert len(CANDIDATES) == 12


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


def run_candidate_all_k(cid: str) -> list[dict]:
    bin_path = compile_candidate(cid)
    _log({"event": "compiled", "id": cid})
    results = []
    for K in K_VALUES:
        r = run_one(cid, bin_path, K)
        results.append(r)
        status = "KILLED-FAIL" if r["killed_early"] else ("CLEAN-16GB" if r["clean_to_16gb"] else "FAIL-AT-FINAL")
        print(f"{cid} K={K}: {status} (final={r['final_length']}, {r['elapsed_s']:.0f}s)", flush=True)
    return results


def main():
    _log({"event": "start", "n_candidates": len(CANDIDATES), "k_values": K_VALUES,
          "base_key": BASE_KEY, "max_parallel": MAX_PARALLEL,
          "note": "K=1 omitted -- provably identical to i0's already-clean K=1 result"})
    all_results = {}
    with concurrent.futures.ThreadPoolExecutor(max_workers=MAX_PARALLEL) as ex:
        futures = {ex.submit(run_candidate_all_k, cid): cid for cid in CANDIDATES}
        for fut in concurrent.futures.as_completed(futures):
            cid = futures[fut]
            try:
                all_results[cid] = fut.result()
            except Exception as e:
                _log({"event": "error", "id": cid, "error": str(e)})
                print(f"ERROR {cid}: {e}", flush=True)

    n_clean = sum(1 for rs in all_results.values() if all(r["clean_to_16gb"] for r in rs))
    _log({"event": "done", "n_candidates": len(all_results), "n_fully_clean": n_clean})
    print(f"\n=== INJECT-CROSSING PROMOTION TIER DONE: {n_clean}/{len(all_results)} candidates clean across ALL K to 16GB ===", flush=True)
    for cid, rs in all_results.items():
        if all(r["clean_to_16gb"] for r in rs):
            print(f"  FULLY CLEAN: {cid}", flush=True)


if __name__ == "__main__":
    main()
