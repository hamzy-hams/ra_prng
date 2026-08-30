"""Q1 Method B: interleave K independent `--stream <seed> <n>` streams
word-by-word (round-robin) and run PractRand on the combined output.
HANDOVER.md question 1's second suggested approach: "treat gabungan/
interleaved N stream sebagai satu input besar dan jalankan lewat PractRand".

Rationale: if the streams were correlated, merging them into "one big RNG"
should let PractRand detect a pattern far sooner than the already-verified
128GB-clean single-stream baseline (2026-8-27 experiment). A clean result
here across the interleave is evidence against cross-stream dependence that
Method A's linear/lag-0 test can't see (nonlinear, bit-level dependence).

Staged like the repo's established PractRand convention (small prefix is
informative for a go/no-go, NOT a final production-scale verdict): smoke
(64MB) -> medium (1GB) -> full (16GB), each gated on the previous stage
being clean before proceeding.
"""

from __future__ import annotations

import subprocess
import sys

import numpy as np

from common import PRACTRAND_BIN, TIERS_Q1B, WINNER_BIN

CHUNK_WORDS = 262_144  # 1MB per stream per round


def run_interleave_practrand(k: int, total_bytes: int) -> dict:
    assert total_bytes % 4 == 0
    total_words = total_bytes // 4
    words_per_stream = -(-total_words // k)  # ceil
    # Over-supply a little slack per stream (a few rounds' worth) and let
    # PractRand's own -tlmax stop the writer via BrokenPipeError, rather than
    # cutting the writer off at an exact word count. Lesson from this session:
    # a xlarge (128GB) run came up exactly 8 words short of the exact target
    # (1 word/stream) and PractRand reported "error reading standard input"
    # instead of a clean 128GB checkpoint -- matches the repo's own proven
    # historical convention (over-supply + let RNG_test stop reading) in
    # other_winners_practrand.sh, rather than relying on an exact byte count.
    supply_words_per_stream = words_per_stream + CHUNK_WORDS * 8
    tlmax_arg = f"{total_bytes // (1024*1024)}MB"
    # tlmin == tlmax (a single fixed-size target) works up to ~16GB, but broke
    # at 128GB ("error reading standard input", no test-result lines at all).
    # Every large-scale PractRand run already proven in this repo
    # (2026-8-27_operand-position-search/RESULTS.md, 128GB and 1TB) instead
    # uses a RANGE -- tlmin well below tlmax, letting PractRand checkpoint at
    # each power-of-2 doubling on the way up. total_bytes/16 reproduces that
    # exact convention for the 128GB case (tlmin=8GB) and scales sensibly for
    # smaller tiers too.
    tlmin_bytes = max(1024 * 1024, total_bytes // 16)
    tlmin_arg = f"{tlmin_bytes // (1024*1024)}MB"

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

    words_written = 0
    chunk_bytes = CHUNK_WORDS * 4
    try:
        while True:
            bufs = []
            for p in procs:
                assert p.stdout is not None
                buf = p.stdout.read(chunk_bytes)
                if not buf:
                    buf = b""
                bufs.append(buf)
            if all(len(b) == 0 for b in bufs):
                break
            # Safety cap: PractRand should close stdin (BrokenPipeError below)
            # once it reaches -tlmax, well before the oversupplied per-stream
            # slack runs out. If that signal is ever missed for some reason,
            # stop instead of writing unbounded data past the intended target.
            if words_written >= total_words + k * CHUNK_WORDS * 8:
                break
            # short reads (end of a stream) truncate this round to the
            # shortest buffer; with generous per-stream oversupply this
            # should only bite once genuinely near EOF, if at all.
            arrs = []
            min_len = min(len(b) for b in bufs)
            round_words = min_len // 4
            if round_words == 0:
                break
            for b in bufs:
                arrs.append(np.frombuffer(b[: round_words * 4], dtype=np.uint32))
            interleaved = np.stack(arrs, axis=1).reshape(-1)
            payload = interleaved.tobytes()
            assert test.stdin is not None
            test.stdin.write(payload)
            words_written += len(interleaved)
    except BrokenPipeError:
        pass  # PractRand hit -tlmax and stopped reading; expected at the end
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

    stdout, stderr = test.communicate()
    for p in procs:
        p.wait()

    stdout_s = stdout.decode(errors="replace")
    stderr_s = stderr.decode(errors="replace")
    anomaly = any(kw in stdout_s for kw in ("FAIL", "SUSPICIOUS"))
    # Absence of FAIL/SUSPICIOUS is NOT sufficient evidence of a clean pass --
    # RNG_test can error out before producing any real result line (e.g. "error
    # reading standard input"), leaving stdout with only the startup banner.
    # Require actual checkpoint evidence ("no anomalies in N test result(s)" or
    # a "length=" line) and no error text in stderr.
    ran_real_test = "length=" in stdout_s and "test result" in stdout_s
    io_error = "error reading" in stderr_s.lower() or "error writing" in stderr_s.lower()
    passed = ran_real_test and not anomaly and not io_error
    return {
        "k": k, "total_bytes": total_bytes, "words_written": words_written,
        "passed": passed, "ran_real_test": ran_real_test, "io_error": io_error,
        "stdout": stdout_s,
        "stderr": stderr_s,
    }


def main():
    tier = sys.argv[1] if len(sys.argv) > 1 else "smoke"
    k, total_bytes = TIERS_Q1B[tier]
    print(f"=== interleave_practrand tier={tier}: K={k} total={total_bytes/1024/1024:.0f}MB ===")
    result = run_interleave_practrand(k, total_bytes)
    print(result["stdout"])
    if result["stderr"]:
        print("--- stderr ---")
        print(result["stderr"])
    if result["passed"]:
        status = "PASSED (no FAIL/SUSPICIOUS, real test-result lines present)"
    elif not result["ran_real_test"] or result["io_error"]:
        status = "RUN FAILED (no valid test-result lines / I/O error -- NOT a statistical verdict)"
    else:
        status = "ANOMALY DETECTED"
    print(f"tier={tier}: {status}")

    from common import HERE
    import json
    out_path = HERE / f"interleave_practrand_results_{tier}.json"
    out_path.write_text(json.dumps(
        {k_: v for k_, v in result.items() if k_ != "stdout"} | {"stdout_tail": result["stdout"][-4000:]},
        indent=2,
    ))
    print(f"Wrote {out_path}")


if __name__ == "__main__":
    main()
