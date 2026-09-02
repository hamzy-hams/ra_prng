#!/bin/bash
set -u
cd ~/ra_prng/experiments/2026-9-2_singleblock-o-width-fix
PR=~/Documents/research/PractRand/RNG_test
STATUS=practrand_B_confirm_16gb_status.log
: > "$STATUS"
N_WORDS=4294967296
for N in 1 4; do
  for K in 1 2 4 8 16; do
    echo "[start] N=$N K=$K $(date)" | tee -a "$STATUS"
    ./diag_wideo_warmup_singleblock --stream wideo 111222 "$N_WORDS" "$K" "$N" \
      | "$PR" stdin32 -tlmin 256MB -tlmax 16GB -multithreaded \
      > "practrand_B_wideo_warmup_N${N}_K${K}_16GB.log" 2>&1
    echo "[done] N=$N K=$K $(date)" | tee -a "$STATUS"
  done
done
echo "[ALL_DONE] $(date)" | tee -a "$STATUS"
