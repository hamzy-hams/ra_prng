#!/bin/bash
# Tractability probe for new toy-model configs (Phase 3, HANDOVER_PHASE3.md
# point #1). Runs 3 seeds per new config with a wall-clock timeout before
# committing to a full sweep -- Brent's algorithm cost tracks actual lambda,
# not state-space size, so it can't be predicted without trying.
set -uo pipefail

CM=../2026-8-25_periodicity-heuristic-validation/cycle_measure
TIMEOUT=180

# n w rows probe_seeds mechanism label
PROBES=(
  "16 4 4 3 0 n16w4_permute"
  "16 4 4 3 1 n16w4_inject"
  "8 8 4 3 0 n8w8_permute"
  "8 8 4 3 1 n8w8_inject"
  "2 16 2 3 0 n2w16_permute"
  "2 16 2 3 1 n2w16_inject"
  "4 16 4 3 0 n4w16_permute"
  "4 16 4 3 1 n4w16_inject"
)

for p in "${PROBES[@]}"; do
  read -r n w rows seeds mech label <<< "$p"
  start=$(date +%s.%N)
  if timeout "$TIMEOUT" "$CM" "$n" "$w" "$rows" "$mech" 0 0 "$seeds" > "/tmp/probe_${label}.csv" 2>"/tmp/probe_${label}.err"; then
    end=$(date +%s.%N)
    elapsed=$(awk -v s="$start" -v e="$end" 'BEGIN { printf "%.1f", e - s }')
    maxlambda=$(tail -n +2 "/tmp/probe_${label}.csv" | cut -d, -f2 | sort -n | tail -1)
    echo "OK   $label: ${elapsed}s for $seeds seeds, max_lambda=$maxlambda"
  else
    rc=$?
    echo "FAIL $label: rc=$rc (timeout=$rc -eq 124, see /tmp/probe_${label}.err)"
  fi
done
