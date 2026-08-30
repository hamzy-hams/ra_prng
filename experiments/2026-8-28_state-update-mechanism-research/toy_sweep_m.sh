#!/bin/bash
# Toy-model periodicity sweep (phase 2, M-only mechanisms), mirroring
# toy_sweep.sh's methodology exactly but with L fixed at "permute" (the
# phase-1-recommended safe default) and M_MECHANISMS varied instead
# (xor_fold=0 today's default, permute=1, inject=2, overwrite=3). See
# toy_prng.py's M_MECHANISMS docstring for the exact rules.
set -euo pipefail

CM=../2026-8-25_periodicity-heuristic-validation/cycle_measure
OUT=.
mkdir -p "$OUT"

# n w rows seed_count -- same grid/seed-counts as phase 1's toy_sweep.sh
CONFIGS=(
  "2 4 2 300"
  "2 8 2 300"
  "4 4 4 300"
  "4 8 4 100"
  "8 4 4 50"
)
MMECHS=(0 1 2 3)
L_MECH=0  # permute, fixed per phase ordering (L dulu, baru M)

for cfg in "${CONFIGS[@]}"; do
  read -r n w rows seeds <<< "$cfg"
  for mm in "${MMECHS[@]}"; do
    out_csv="$OUT/toy_sweep_m_n${n}_w${w}_mmech${mm}.csv"
    echo "=== n=$n w=$w rows=$rows m_mechanism=$mm seeds=$seeds -> $out_csv ==="
    "$CM" "$n" "$w" "$rows" "$L_MECH" "$mm" 0 "$seeds" > "$out_csv"
  done
done

rm -f "$OUT"/.cycle_measure_ckpt_*.bin
echo "M-mechanism sweep complete."
