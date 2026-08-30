#!/bin/bash
# Toy-model periodicity sweep (phase 1, L-only mechanisms) per HANDOVER.md's
# scope decision #2(a): Brent lambda/mu across the same n/w grid the
# 2026-8-25 experiment validated as tractable, now repeated for all 3 L
# mechanisms (permute=0, inject=1, overwrite=2). Uses the C port
# (cycle_measure.c) throughout since it's cross-validated bit-for-bit
# against toy_prng.py (see this folder's own parity check) and far faster.
set -euo pipefail

CM=../2026-8-25_periodicity-heuristic-validation/cycle_measure
OUT=.
mkdir -p "$OUT"

# n w rows seed_count
CONFIGS=(
  "2 4 2 300"
  "2 8 2 300"
  "4 4 4 300"
  "4 8 4 100"
  "8 4 4 50"
)
MECHS=(0 1 2)

for cfg in "${CONFIGS[@]}"; do
  read -r n w rows seeds <<< "$cfg"
  for m in "${MECHS[@]}"; do
    out_csv="$OUT/toy_sweep_n${n}_w${w}_mech${m}.csv"
    echo "=== n=$n w=$w rows=$rows mechanism=$m seeds=$seeds -> $out_csv ==="
    "$CM" "$n" "$w" "$rows" "$m" 0 0 "$seeds" > "$out_csv"
  done
done

rm -f "$OUT"/.cycle_measure_ckpt_*.bin
echo "Sweep complete."
