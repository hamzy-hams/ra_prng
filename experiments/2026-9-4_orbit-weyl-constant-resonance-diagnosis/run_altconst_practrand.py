"""Staged PractRand runner for the weyl-constant-resonance-diagnosis
experiment. Port of ../2026-9-4_orbit-init-warmup-fix/tier1_gate_warmup.py's
run_stage() logic, single-ordering (--stream-altconst) instead of dual.

Scale (HANDOVER.md §0.2, user-confirmed 2026-09-04): 2GB staged
64MB->256MB->2GB, matching the sibling sessions' precedent. Single base_key
(0x12345) -- this is a diagnostic, not a full validation gate, so no
Stage-C reproducibility cross-check.

No re-seed retry on FAIL (feedback_no_reseed_on_fail): a hard FAIL is final.
"""

from __future__ import annotations

import json
import re
import subprocess
import sys
import time
from pathlib import Path

PRACTRAND_BIN = Path.home() / "Documents/research/PractRand/RNG_test"
HERE = Path(__file__).parent
BIN_PATH = HERE / "multikey_stream_orbit_altconst"
LOGS_DIR = HERE
BASE_KEY = 0x12345
K = 1

FAIL_RE = re.compile(r"\bFAIL\b|very suspicious")
LENGTH_RE = re.compile(r"length=\s*([^,]+),")
BYTES_EXP_RE = re.compile(r"\(2\^(\d+) bytes\)")


def parse_size(s: str) -> int:
    s = s.strip().upper()
    m = re.match(r"^(\d+)(K|M|G)B?$", s)
    if not m:
        raise ValueError(f"cannot parse size: {s!r}")
    n, unit = int(m.group(1)), m.group(2)
    mult = {"K": 1024, "M": 1024 ** 2, "G": 1024 ** 3}[unit]
    return n * mult


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


def run_stage(const_hex: str, tlmin: str, tlmax: str, stage_tag: str) -> dict:
    target_bytes = parse_size(tlmax)
    n_words = target_bytes // 4
    log_path = LOGS_DIR / f"altconst_{const_hex}_K{K}_{BASE_KEY:#010x}_{stage_tag}.log"

    t0 = time.time()
    gen = subprocess.Popen(
        [str(BIN_PATH), "--stream-altconst", str(BASE_KEY), str(n_words), str(K), const_hex],
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
            checkpoints.append({"length": length_str, "bytes": block_bytes, "hard_fail": hard_fail})
            is_final = block_bytes >= target_bytes
            print(f"    checkpoint {length_str}: {'FAIL' if hard_fail else 'ok'}", flush=True)
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

    last = checkpoints[-1] if checkpoints else {"length": "?", "bytes": 0, "hard_fail": True}
    result = {
        "const_hex": const_hex, "K": K, "base_key": BASE_KEY, "stage": stage_tag,
        "checkpoints": checkpoints, "killed_early": killed_early,
        "final_length": last["length"], "final_hard_fail": last["hard_fail"],
        "clean_to_target": (not killed_early) and (not last["hard_fail"]) and last["bytes"] >= target_bytes,
        "elapsed_s": elapsed,
    }
    verdict = "FAIL" if (killed_early or last["hard_fail"]) else "CLEAN"
    print(f"  [{stage_tag}] const={const_hex} K={K} base={BASE_KEY:#x}: "
          f"{verdict} @ {last['length']} ({elapsed:.0f}s)", flush=True)
    return result


def run_staged(const_hex: str):
    print(f"=== Staged PractRand for const={const_hex}, K={K}, base_key={BASE_KEY:#x} ===", flush=True)
    r_a = run_stage(const_hex, "64MB", "256MB", "A")
    if not r_a["clean_to_target"]:
        print(f"Stage A FAILED for {const_hex} -- stopping (no Stage B).", flush=True)
        return {"stage_a": r_a, "stage_b": None}
    r_b = run_stage(const_hex, "256MB", "2GB", "B")
    return {"stage_a": r_a, "stage_b": r_b}


if __name__ == "__main__":
    const_hex = sys.argv[1] if len(sys.argv) > 1 else "0x2545F491"
    result = run_staged(const_hex)
    out_path = HERE / f"altconst_{const_hex}_result.json"
    out_path.write_text(json.dumps(result, indent=2))
    print(f"Result written to {out_path}")
