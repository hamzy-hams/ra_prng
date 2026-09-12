"""Tier 1: PractRand dual-ordering gate for the orbit init x cycle combo
search, run LOCALLY (laptop, not VPS -- Promotion Tier is explicitly out of
scope for this script, see HANDOVER.md 7 step 5 / RESULTS.md's Next steps).

Three sequential sub-stages per combo, cheapest-first, K=1 throughout (Tier
1's mandated K per HANDOVER.md 7 step 4):

  Stage A (screen): base_key 0x12345, BOTH orderings (Weyl --stream, hashed
    --stream-hashed), up to 256MB, early-kill on hard FAIL. 32 combos x 2
    orderings = 64 runs. A combo proceeds to Stage B only if BOTH orderings
    stay clean through 256MB.
  Stage B (confirm): Stage-A survivors only, same base_key + both
    orderings, extended to 2GB in one continuous RNG_test invocation per
    ordering -- covers the range where the historical K=1+hashed defect
    (../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md) FAILed as
    early as 1GB.
  Stage C (reproducibility cross-check): Stage-B survivors only, repeat
    Stage B exactly with base_key 0xABCDEF01 -- the SECOND base_key used
    when the original gap was diagnosed, for apple-to-apple comparison.
    This is a reproducibility CONFIRMATION (HANDOVER.md 9), not a retry to
    explain away a FAIL (feedback_no_reseed_on_fail memory).

MAX_PARALLEL=1 (implicit, sequential): RNG_test -multithreaded already
saturates all cores on this 4-core laptop; concurrent stage runs would
oversubscribe rather than speed anything up (unlike
../2026-9-2_singleblock-cycle-combo-search/promotion_search.py's VPS
MAX_PARALLEL=2 on 8 cores).

Checkpoint parsing mirrors promotion_search.py's iter_checkpoints/FAIL_RE/
SOFT_ANOMALY_RE (lowercase-aware regex). One improvement: "is this the
final requested checkpoint" is derived from PractRand's own
"(2^N bytes)" annotation compared against the requested tlmax in bytes,
rather than a hardcoded string match -- this script targets several
different tlmax values (256MB, 2GB) across stages, unlike promotion_search's
single fixed 16GB target.

No re-seed retry on FAIL (feedback_no_reseed_on_fail): a hard FAIL for one
(combo, ordering, base_key) is final for that combo at that stage -- the
combo is dropped from further stages, never retried with a different seed
to explain the FAIL away.
"""

from __future__ import annotations

import json
import re
import subprocess
import time
from pathlib import Path

from recipes_orbit import combo_id, all_combos

PRACTRAND_BIN = Path.home() / "Documents/research/PractRand/RNG_test"
HERE = Path(__file__).parent
CAND_DIR = HERE / "candidates"
LOGS_DIR = HERE / "tier1_logs"
LOGS_DIR.mkdir(exist_ok=True)
LOG_PATH = HERE / "tier1_log_orbit.jsonl"
SUMMARY_PATH = HERE / "tier1_summary_orbit.json"

BASE_KEY_A = 0x12345
BASE_KEY_B = 0xABCDEF01

FAIL_RE = re.compile(r"\bFAIL\b|very suspicious")
SOFT_ANOMALY_RE = re.compile(r"\bmildly suspicious\b|\bsuspicious\b|\bunusual\b")
LENGTH_RE = re.compile(r"length=\s*([^,]+),")
BYTES_EXP_RE = re.compile(r"\(2\^(\d+) bytes\)")

ORDERINGS = ["weyl", "hashed"]  # --stream / --stream-hashed


def parse_size(s: str) -> int:
    s = s.strip().upper()
    m = re.match(r"^(\d+)(K|M|G)B?$", s)
    if not m:
        raise ValueError(f"cannot parse size: {s!r}")
    n, unit = int(m.group(1)), m.group(2)
    mult = {"K": 1024, "M": 1024 ** 2, "G": 1024 ** 3}[unit]
    return n * mult


def _log(event: dict):
    event["ts"] = time.time()
    with LOG_PATH.open("a") as f:
        f.write(json.dumps(event) + "\n")
        f.flush()


def iter_checkpoints(stream):
    """Yield one full checkpoint block (str) at a time from RNG_test's
    stdout, as lines arrive (live, not buffered at the end). Copied
    verbatim in structure from promotion_search.py."""
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


def run_stage(cid: str, bin_path: Path, ordering: str, base_key: int,
              tlmin: str, tlmax: str, stage_tag: str) -> dict:
    target_bytes = parse_size(tlmax)
    n_words = target_bytes // 4
    stream_flag = "--stream" if ordering == "weyl" else "--stream-hashed"
    log_path = LOGS_DIR / f"{cid}_{ordering}_{base_key:#010x}_{stage_tag}.log"

    t0 = time.time()
    gen = subprocess.Popen(
        [str(bin_path), stream_flag, str(base_key), str(n_words), "1"],
        stdout=subprocess.PIPE,
    )
    test = subprocess.Popen(
        [str(PRACTRAND_BIN), "stdin32", "-tlmin", tlmin, "-tlmax", tlmax, "-multithreaded"],
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
            bm = BYTES_EXP_RE.search(block)
            block_bytes = (2 ** int(bm.group(1))) if bm else 0
            hard_fail = bool(FAIL_RE.search(block))
            soft_hits = sorted(set(SOFT_ANOMALY_RE.findall(block)))
            checkpoints.append({
                "length": length_str, "bytes": block_bytes,
                "hard_fail": hard_fail, "soft_anomaly_kinds": soft_hits,
            })
            is_final = block_bytes >= target_bytes
            if hard_fail and not is_final:
                test.terminate()
                gen.terminate()
                killed_early = True
                logf.write(f"\n[EARLY KILL after {length_str}: hard FAIL detected]\n")
                break

    if test.poll() is None:
        test.wait(timeout=10)
    if gen.poll() is None:
        gen.wait(timeout=10)
    elapsed = time.time() - t0

    last = checkpoints[-1] if checkpoints else {
        "length": "?", "bytes": 0, "hard_fail": True, "soft_anomaly_kinds": [],
    }
    result = {
        "id": cid, "ordering": ordering, "base_key": base_key, "stage": stage_tag,
        "checkpoints": checkpoints, "killed_early": killed_early,
        "final_length": last["length"], "final_hard_fail": last["hard_fail"],
        "clean_to_target": (not killed_early) and (not last["hard_fail"]) and last["bytes"] >= target_bytes,
        "elapsed_s": elapsed,
    }
    _log({"event": "result", **{k: v for k, v in result.items() if k != "checkpoints"},
          "n_checkpoints": len(checkpoints)})
    verdict = "FAIL" if (killed_early or last["hard_fail"]) else "CLEAN"
    print(f"  [{stage_tag}] {cid} {ordering} base={base_key:#x}: "
          f"{verdict} @ {last['length']} ({elapsed:.0f}s)", flush=True)
    return result


def run_dual_ordering_stage(combos, base_key: int, tlmin: str, tlmax: str, stage_tag: str):
    """Run both orderings for each combo; a combo survives only if BOTH are
    clean_to_target. Returns (survivors, all_results)."""
    survivors = []
    all_results = []
    for init_id, cycle_id in combos:
        cid = combo_id(init_id, cycle_id)
        bin_path = CAND_DIR / cid
        per_ordering = {}
        clean = True
        for ordering in ORDERINGS:
            r = run_stage(cid, bin_path, ordering, base_key, tlmin, tlmax, stage_tag)
            per_ordering[ordering] = r
            all_results.append(r)
            if not r["clean_to_target"]:
                clean = False
        _log({"event": f"{stage_tag}_verdict", "id": cid, "base_key": base_key, "clean": clean})
        if clean:
            survivors.append((init_id, cycle_id))
    return survivors, all_results


def main():
    combos = list(all_combos())
    _log({"event": "start", "n_combos": len(combos), "base_key_a": BASE_KEY_A, "base_key_b": BASE_KEY_B})

    print(f"=== Stage A (screen): {len(combos)} combos x {len(ORDERINGS)} orderings, "
          f"base_key={BASE_KEY_A:#x}, up to 256MB ===", flush=True)
    stage_a_survivors, stage_a_results = run_dual_ordering_stage(
        combos, BASE_KEY_A, "64MB", "256MB", "A")
    print(f"Stage A done: {len(stage_a_survivors)}/{len(combos)} survived dual-ordering to 256MB\n", flush=True)

    print(f"=== Stage B (confirm): {len(stage_a_survivors)} Stage-A survivors, "
          f"base_key={BASE_KEY_A:#x}, up to 2GB ===", flush=True)
    stage_b_survivors, stage_b_results = run_dual_ordering_stage(
        stage_a_survivors, BASE_KEY_A, "256MB", "2GB", "B")
    print(f"Stage B done: {len(stage_b_survivors)}/{len(stage_a_survivors)} survived dual-ordering to 2GB\n", flush=True)

    print(f"=== Stage C (reproducibility): {len(stage_b_survivors)} Stage-B survivors, "
          f"base_key={BASE_KEY_B:#x}, up to 2GB ===", flush=True)
    stage_c_survivors, stage_c_results = run_dual_ordering_stage(
        stage_b_survivors, BASE_KEY_B, "256MB", "2GB", "C")
    print(f"Stage C done: {len(stage_c_survivors)}/{len(stage_b_survivors)} survived dual-ordering (base_key #2) to 2GB\n", flush=True)

    summary = {
        "n_combos_total": len(combos),
        "stage_a_survivors": [combo_id(i, c) for i, c in stage_a_survivors],
        "stage_b_survivors": [combo_id(i, c) for i, c in stage_b_survivors],
        "stage_c_survivors_CLEAN_DUAL": [combo_id(i, c) for i, c in stage_c_survivors],
        "stage_a_results": stage_a_results,
        "stage_b_results": stage_b_results,
        "stage_c_results": stage_c_results,
    }
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2))
    _log({"event": "done", "n_clean_dual": len(stage_c_survivors)})

    print("=== TIER 1 DONE ===")
    print(f"CLEAN-DUAL survivors (clean at both base_keys, both orderings, up to 2GB): {len(stage_c_survivors)}")
    for init_id, cycle_id in stage_c_survivors:
        print(f"  {combo_id(init_id, cycle_id)}")
    print(f"Summary written to {SUMMARY_PATH}")


if __name__ == "__main__":
    main()
