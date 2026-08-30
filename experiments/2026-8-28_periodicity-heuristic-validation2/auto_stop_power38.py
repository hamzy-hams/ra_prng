"""Monitors .cycle_measure_ckpt_n8_w4_rows4_seed0.bin and stops cycle_measure
when power reaches 2**39 (549,755,813,888) or phase > 0.

Bumped from 2**38 to 2**39 for the corrected re-run (see HANDOVER.md's
"2026-08-29 correction" entry): the original 2**38 run's headline number in
RESULTS.md/STATUS.md (">1.518e11") was written over an hour before this
process actually stopped and does not match the checkpoint left on disk
(true value ~2.75e11) -- this re-run resumes from that same, valid checkpoint
to a new, cleanly-obtained threshold.

CMD_PATTERN below MUST match the exact argv of the cycle_measure invocation
being monitored (n, w, rows, seed_start, seed_count) -- a short prefix like
"cycle_measure 8 4 4" also matches unrelated invocations of this binary
(e.g. run_sweep.py's own (8,4,rows=4) sweep step) and can cause find_pid() to
kill the wrong process. This is exactly what happened on 2026-08-28: run_sweep.py
crashed with subprocess.CalledProcessError (exit 42) at the (8,4,rows=4) step,
almost certainly because this script's old pattern matched and killed that
subprocess instead of (or in addition to) the intended long-running one.
"""

from __future__ import annotations

import os
import signal
import struct
import subprocess
import time
from pathlib import Path

CKPT = Path(__file__).parent / ".cycle_measure_ckpt_n8_w4_rows4_seed0.bin"
TARGET_POWER = 1 << 39  # 549,755,813,888
LOG = Path(__file__).parent / "auto_stop_power38.log"
CMD_PATTERN = "cycle_measure 8 4 4 0 1"  # exact argv of the monitored invocation


def get_status():
    if not CKPT.exists():
        return None
    try:
        with open(CKPT, "rb") as f:
            data = f.read(64)
        if len(data) < 64:
            return None
        magic, version, n, w, rows, seed, phase, lam, power, k, mu = struct.unpack('<IIiiiII4xQQQQ', data)
        return {"phase": phase, "lam": lam, "power": power, "k": k, "mu": mu}
    except Exception:
        return None


def find_pid():
    proc = subprocess.run(["pgrep", "-f", CMD_PATTERN], capture_output=True, text=True)
    pids = [int(p) for p in proc.stdout.split() if p.strip()]
    return pids


def main():
    with open(LOG, "a") as f:
        f.write(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] Started auto-stop monitor for power >= 2**39 ({TARGET_POWER:,}), pattern={CMD_PATTERN!r}\n")
        f.flush()

    while True:
        st = get_status()
        pids = find_pid()
        if not pids:
            with open(LOG, "a") as f:
                f.write(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] cycle_measure process no longer running. Exiting.\n")
            break

        if st:
            power = st["power"]
            phase = st["phase"]
            lam = st["lam"]
            if power >= TARGET_POWER or phase > 0:
                with open(LOG, "a") as f:
                    f.write(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] Condition reached! power={power:,} (2**{power.bit_length()-1}) lam={lam:,} phase={phase}. Terminating cycle_measure pids={pids}...\n")
                for pid in pids:
                    try:
                        os.kill(pid, signal.SIGINT)
                    except Exception:
                        pass
                time.sleep(3)
                for pid in pids:
                    try:
                        os.kill(pid, signal.SIGTERM)
                    except Exception:
                        pass
                with open(LOG, "a") as f:
                    f.write(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] Termination signal sent successfully.\n")
                break

        time.sleep(15)


if __name__ == "__main__":
    main()
