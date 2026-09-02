"""Langkah 2 of HANDOVER_1TB_FOLLOWUP.md: re-run the interleaved 1TB
Method B test against `winner_wired_v2` with a DIFFERENT seed set
(default seeds(1000..1007) instead of interleave_practrand.py's hardcoded
range(k)), to check whether the 3 tests flagged at the original 1TB
checkpoint (BCFN(2+0,13-0,T), FPF-14+6/16:(5,14-0), FPF-14+6/16:all)
recur with independent seeds -- answers (a) real cross-stream signal vs.
(c) single-run multiple-testing noise.

Never edits interleave_practrand.py / common.py in place (repo's "reuse,
don't edit" convention) -- reuses WINNER_BIN/PRACTRAND_BIN/TIERS_Q1B from
common.py, and reuses the live-streaming/per-line-flush technique from
`../2026-8-30_addressable-init-research/tahap3_interleave_practrand_1tb_live.py`
(an unattended `interleave_practrand.py 1tb` run only prints/saves once at
the very end via `test.communicate()`, so a mid-run interruption loses the
whole run rather than just the tail -- this variant avoids that by flushing
each PractRand checkpoint line to disk as it arrives).
"""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

import numpy as np

from common import PRACTRAND_BIN, TIERS_Q1B, WINNER_BIN

HERE = Path(__file__).parent
CHUNK_WORDS = 262_144

DEFAULT_SEEDS = list(range(1000, 1008))  # different from interleave_practrand.py's range(k)


def run_live(seeds: list[int], total_bytes: int, log_path: Path) -> None:
    k = len(seeds)
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
        for seed in seeds
    ]
    test = subprocess.Popen(
        [str(PRACTRAND_BIN), "stdin32", "-tlmin", tlmin_arg, "-tlmax", tlmax_arg],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
    )

    log_f = open(log_path, "a", buffering=1)
    log_f.write(f"=== seeds={seeds} k={k} total_bytes={total_bytes} ===\n")
    log_f.flush()

    import threading

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
    _, total_bytes = TIERS_Q1B[tier]
    seeds = DEFAULT_SEEDS
    log_path = HERE / f"interleave_practrand_{tier}_confirm_live.log"
    print(f"=== confirm live run tier={tier}: seeds={seeds} total={total_bytes / 1024 / 1024:.0f}MB -> {log_path} ===")
    run_live(seeds, total_bytes, log_path)
    print("done, see", log_path)
