"""Live-streaming + SIGHUP-resilient variant of tahap3_interleave_practrand,
tier `1tb` only. Never edits interleave_practrand.py / tahap3_interleave_practrand.py
in place (repo's "reuse, don't edit" convention) -- reuses WINNER_BIN pattern
and TIERS_Q1B from those modules.

Why this exists: an unattended `python3 tahap3_interleave_practrand.py 1tb`
run on 2026-08-31 was killed (no OOM, no suspend/resume in `journalctl -k`)
around ~89% through the ~3h52m run, with ZERO partial output recovered --
`run_interleave_practrand()` in `../2026-8-29_parallelization-research/
interleave_practrand.py` only prints/saves results once at the very end via
`test.communicate()`. This variant instead:
  1. Reads PractRand's stdout line-by-line in a background thread and
     flushes each line to a log file the moment it arrives, so an
     interruption only loses work since the last completed checkpoint.
  2. Is meant to be launched via `nohup ... & disown` so a SIGHUP to the
     parent shell (the likely cause of the previous kill) does not
     propagate to this process.
"""

from __future__ import annotations

import subprocess
import sys
import threading
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent.parent / "2026-8-29_parallelization-research"))
from common import PRACTRAND_BIN, TIERS_Q1B  # noqa: E402

HERE = Path(__file__).parent
WINNER_BIN = HERE / "winner_wired_addressable"
CHUNK_WORDS = 262_144


def run_live(k: int, total_bytes: int, log_path: Path) -> None:
    assert total_bytes % 4 == 0
    total_words = total_bytes // 4
    words_per_stream = -(-total_words // k)
    supply_words_per_stream = words_per_stream + CHUNK_WORDS * 8
    tlmax_arg = f"{total_bytes // (1024 * 1024)}MB"
    tlmin_bytes = max(1024 * 1024, total_bytes // 16)
    tlmin_arg = f"{tlmin_bytes // (1024 * 1024)}MB"

    procs = [
        subprocess.Popen(
            [str(WINNER_BIN), "--stream", str(seed), str(supply_words_per_stream)],
            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
        )
        for seed in range(k)
    ]
    test = subprocess.Popen(
        [str(PRACTRAND_BIN), "stdin32", "-tlmin", tlmin_arg, "-tlmax", tlmax_arg],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
    )

    log_f = open(log_path, "a", buffering=1)

    def reader():
        assert test.stdout is not None
        for line in iter(test.stdout.readline, b""):
            log_f.write(line.decode(errors="replace"))
            log_f.flush()

    def stderr_reader():
        assert test.stderr is not None
        for line in iter(test.stderr.readline, b""):
            log_f.write("STDERR: " + line.decode(errors="replace"))
            log_f.flush()

    t1 = threading.Thread(target=reader, daemon=True)
    t2 = threading.Thread(target=stderr_reader, daemon=True)
    t1.start()
    t2.start()

    words_written = 0
    chunk_bytes = CHUNK_WORDS * 4
    try:
        while True:
            bufs = []
            for p in procs:
                assert p.stdout is not None
                buf = p.stdout.read(chunk_bytes)
                bufs.append(buf or b"")
            if all(len(b) == 0 for b in bufs):
                break
            if words_written >= total_words + k * CHUNK_WORDS * 8:
                break
            min_len = min(len(b) for b in bufs)
            round_words = min_len // 4
            if round_words == 0:
                break
            arrs = [np.frombuffer(b[: round_words * 4], dtype=np.uint32) for b in bufs]
            interleaved = np.stack(arrs, axis=1).reshape(-1)
            payload = interleaved.tobytes()
            assert test.stdin is not None
            test.stdin.write(payload)
            words_written += len(interleaved)
    except BrokenPipeError:
        pass
    finally:
        if test.stdin is not None:
            try:
                test.stdin.close()
            except BrokenPipeError:
                pass
        for p in procs:
            assert p.stdout is not None
            p.stdout.close()
            p.terminate()

    test.wait()
    t1.join(timeout=5)
    t2.join(timeout=5)
    for p in procs:
        p.wait()
    log_f.write(f"\n[done] words_written={words_written}\n")
    log_f.close()


if __name__ == "__main__":
    tier = sys.argv[1] if len(sys.argv) > 1 else "1tb"
    k, total_bytes = TIERS_Q1B[tier]
    log_path = HERE / f"tahap3_interleave_practrand_{tier}_live.log"
    print(f"=== live run tier={tier}: K={k} total={total_bytes / 1024 / 1024:.0f}MB -> {log_path} ===")
    run_live(k, total_bytes, log_path)
    print("done, see", log_path)
