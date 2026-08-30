#!/bin/bash
# Q3(b): perf stat (instructions,cycles) across N concurrent `--stream`
# processes (seeds 0..N-1), each emitting N_PER_PROC values to /dev/null.
# perf tracks the whole process tree spawned by the wrapping `bash -c`
# (same as `time` shell builtin), so instructions/cycles reported are the
# aggregate across all N processes -- divide by (N * N_PER_PROC) for
# instructions-per-element, the repo's trusted (deterministic) axis
# (cycles are noisy under contention -- see other_winners_perf.sh /
# perf_state_update.sh comments). Run on an idle machine, before any other
# CPU-heavy job (PractRand in particular).
set -euo pipefail
cd "$(dirname "$0")"

BIN="../2026-8-27_operand-position-search/winner_wired_v2"
N_PER_PROC=200000000
N_VALUES="1 2 3 4 8"

OUT=perf_scaling.log
: > "$OUT"

for N in $N_VALUES; do
    echo "=== N=$N (n_per_proc=$N_PER_PROC) ===" | tee -a "$OUT"
    perf stat -e instructions,cycles -r 3 -- bash -c "
        for i in \$(seq 0 $((N-1))); do
            \"$BIN\" --stream \"\$i\" $N_PER_PROC >/dev/null 2>/dev/null &
        done
        wait
    " >> "$OUT" 2>&1
    echo "" >> "$OUT"
done

echo "Done. See $OUT"
