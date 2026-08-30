#!/bin/bash
# Phase 3 toy-model sweep (HANDOVER_PHASE3.md point #1): resample the two
# under-sampled configs to 300 seeds, and add the new w=16 data point that
# turned out tractable. See RESULTS.md's "Fase 3" section for the full
# analysis; see toy_probe_phase3.sh for the tractability probes that ruled
# out n=16,w=4 / n=4,w=16 / n=8,w=8-inject before this was run.
#
# NOTE: run as bash (not a zsh inline loop) -- zsh doesn't word-split
# `read -r ... <<< "$cfg"` by default (bug from an earlier session).
set -euo pipefail

CM=../2026-8-25_periodicity-heuristic-validation/cycle_measure
OUT=.
mkdir -p "$OUT"

# n w rows seed_count suffix
CONFIGS=(
  "4 8 4 300 _300seed"
  "8 4 4 300 _300seed"
  "2 16 2 300 "
)
MECHS=(0 1)

for cfg in "${CONFIGS[@]}"; do
  read -r n w rows seeds suffix <<< "$cfg"
  for m in "${MECHS[@]}"; do
    out_csv="$OUT/toy_sweep_n${n}_w${w}_mech${m}${suffix}.csv"
    echo "=== n=$n w=$w rows=$rows mechanism=$m seeds=$seeds -> $out_csv ==="
    "$CM" "$n" "$w" "$rows" "$m" 0 0 "$seeds" > "$out_csv"
  done
done

rm -f "$OUT"/.cycle_measure_ckpt_*.bin
echo "Phase 3 sweep complete."
