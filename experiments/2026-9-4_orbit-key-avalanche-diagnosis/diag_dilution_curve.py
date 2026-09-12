"""Part A: dilution curve for the K=1+hashed-key defect in `ra_core_orbit`.

Confirms/refutes the "cold-start" hypothesis: K=1 (rng=1) means every output
word is the FIRST word ever produced from a freshly-initialized L/M array
(ra_reseed never fires for rng<=255, see ../2026-9-1_family-productionization/
ra_core.c L254-270 -- `count<=1` returns before the `ra_reseed` call). K=255
is known CLEAN (../2026-9-4_orbit-kmin-battery/RESULTS.md), K=1 is known FAIL
(../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md, reconfirmed for
ALL 32 init x cycle combos in
../2026-9-4_orbit-init-cycle-combo-search/RESULTS.md). If a defect concentrated
in "word #1 of a fresh call" is diluted away as K grows (more warm words per
cold word), the FAIL->CLEAN transition point pinpoints how localized the
defect is -- informing whether the fix should be a small, targeted "extra
mixing round for the very first word" or something broader.

Uses the EXISTING, already byte-exact-verified-vs-production control binary
from ../2026-9-4_orbit-init-cycle-combo-search/candidates/ -- NO new C code.
Only urutan hashed diuji (Weyl sudah terbukti bersih di semua K termasuk K=1,
lihat orbit-kmin-battery); base_key 0x12345 (sama seperti Tier 1 combo-search,
dan basis original gap diagnosis), skala kecil (early-kill, -tlmin 64MB
-tlmax 1GB) supaya cepat.

run_stage() is a straight port of
../2026-9-4_orbit-init-cycle-combo-search/tier1_gate_orbit.py's run_stage,
parametrized by K instead of hardcoding "1" as the last CLI arg.
"""

from __future__ import annotations

import json
import re
import subprocess
import time
from pathlib import Path

HERE = Path(__file__).parent
CONTROL_BIN = HERE / ".." / "2026-9-4_orbit-init-cycle-combo-search" / "candidates" / "init_linear_shared__w8_f10_i0"
PRACTRAND_BIN = Path.home() / "Documents/research/PractRand/RNG_test"
LOGS_DIR = HERE / "dilution_logs"
LOGS_DIR.mkdir(exist_ok=True)
LOG_PATH = HERE / "dilution_log.jsonl"
SUMMARY_PATH = HERE / "dilution_summary.json"

BASE_KEY = 0x12345
K_VALUES = [1, 2, 4, 8, 16, 32, 64]
TLMIN, TLMAX = "64MB", "1GB"

FAIL_RE = re.compile(r"\bFAIL\b|very suspicious")
SOFT_ANOMALY_RE = re.compile(r"\bmildly suspicious\b|\bsuspicious\b|\bunusual\b")
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


def _log(event: dict):
    event["ts"] = time.time()
    with LOG_PATH.open("a") as f:
        f.write(json.dumps(event) + "\n")
        f.flush()


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


def run_stage(k: int, base_key: int, tlmin: str, tlmax: str) -> dict:
    target_bytes = parse_size(tlmax)
    n_words = target_bytes // 4
    log_path = LOGS_DIR / f"K{k:03d}_hashed_{base_key:#010x}.log"

    t0 = time.time()
    gen = subprocess.Popen(
        [str(CONTROL_BIN), "--stream-hashed", str(base_key), str(n_words), str(k)],
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
        "k": k, "base_key": base_key,
        "checkpoints": checkpoints, "killed_early": killed_early,
        "final_length": last["length"], "final_hard_fail": last["hard_fail"],
        "clean_to_target": (not killed_early) and (not last["hard_fail"]) and last["bytes"] >= target_bytes,
        "elapsed_s": elapsed,
    }
    _log({"event": "result", **{k2: v for k2, v in result.items() if k2 != "checkpoints"},
          "n_checkpoints": len(checkpoints)})
    verdict = "FAIL" if (killed_early or last["hard_fail"]) else "CLEAN"
    print(f"  K={k:<3d} hashed base={base_key:#x}: {verdict} @ {last['length']} ({elapsed:.0f}s)", flush=True)
    return result


def main():
    if not CONTROL_BIN.exists():
        raise SystemExit(f"control binary not found: {CONTROL_BIN} -- build it first "
                          f"(see ../2026-9-4_orbit-init-cycle-combo-search/combo_gen_orbit.py)")
    _log({"event": "start", "k_values": K_VALUES, "base_key": BASE_KEY, "tlmin": TLMIN, "tlmax": TLMAX})
    print(f"=== Dilution curve: K in {K_VALUES}, hashed ordering, base_key={BASE_KEY:#x}, "
          f"{TLMIN}->{TLMAX} ===", flush=True)
    results = []
    for k in K_VALUES:
        r = run_stage(k, BASE_KEY, TLMIN, TLMAX)
        results.append(r)

    SUMMARY_PATH.write_text(json.dumps(results, indent=2))
    _log({"event": "done"})
    print("\n=== DILUTION CURVE DONE ===")
    for r in results:
        verdict = "FAIL" if (r["killed_early"] or r["final_hard_fail"]) else "CLEAN"
        print(f"  K={r['k']:<3d}: {verdict} @ {r['final_length']}")
    print(f"Summary written to {SUMMARY_PATH}")


if __name__ == "__main__":
    main()
