#!/bin/bash
# perf comparison for the 6 phase-1 L-mechanism binaries, mirroring
# ../2026-8-27_operand-position-search/other_winners_perf.sh's methodology:
# perf stat -e instructions,cycles -r 5, default (no-arg) run mode, each
# binary's own TOTAL_RNG. Instructions is the trusted axis (deterministic);
# cycles is noisy under any concurrent CPU load (see HANDOVER.md/RESULTS.md).
set -euo pipefail
BINARIES=(original_permute original_inject original_overwrite winner_permute winner_inject winner_overwrite)
OUT=perf_state_update.log
: > "$OUT"
for b in "${BINARIES[@]}"; do
  echo "=== $b ===" | tee -a "$OUT"
  perf stat -e instructions,cycles -r 5 "./$b" >> "$OUT" 2>&1
done
