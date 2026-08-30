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
    bytes_per_stream = words_per_stream * 4
    length_arg = f"{total_bytes // (1024*1024)}MB"

    procs = [
        subprocess.Popen(
            [str(WINNER_BIN), "--stream", str(seed), str(words_per_stream)],
            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
        )
        for seed in range(k)
    ]

    test = subprocess.Popen(
        [str(PRACTRAND_BIN), "stdin32", "-tlmin", length_arg, "-tlmax", length_arg],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
    )

    words_written = 0
    chunk_bytes = CHUNK_WORDS * 4
    try:
        while words_written < total_words:
            bufs = []
            for p in procs:
                assert p.stdout is not None
                buf = p.stdout.read(chunk_bytes)
                if not buf:
                    buf = b""
                bufs.append(buf)
            if all(len(b) == 0 for b in bufs):
                break
            # pad short reads (end of a stream) with zeros so np.stack works;
            # target sizing (words_per_stream * k >= total_words) means this
            # should only bite on the very last round, if at all.
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
    anomaly = any(kw in stdout_s for kw in ("FAIL", "SUSPICIOUS"))
    return {
        "k": k, "total_bytes": total_bytes, "words_written": words_written,
        "passed": not anomaly, "stdout": stdout_s,
        "stderr": stderr.decode(errors="replace"),
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
    status = "PASSED (no FAIL/SUSPICIOUS)" if result["passed"] else "ANOMALY DETECTED"
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
