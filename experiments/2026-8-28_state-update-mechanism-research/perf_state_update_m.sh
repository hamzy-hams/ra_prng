#!/bin/bash
set -euo pipefail
BINARIES=(original_permute original_m_permute original_m_inject original_m_overwrite winner_permute winner_m_permute winner_m_inject winner_m_overwrite)
OUT=perf_state_update_m.log
: > "$OUT"
for b in "${BINARIES[@]}"; do
  echo "=== $b ===" | tee -a "$OUT"
  perf stat -e instructions,cycles -r 5 "./$b" >> "$OUT" 2>&1
done
