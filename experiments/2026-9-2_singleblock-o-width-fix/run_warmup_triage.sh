#!/bin/bash
set -u
cd ~/ra_prng/experiments/2026-9-2_singleblock-o-width-fix
PR=~/Documents/research/PractRand/RNG_test
N_WORDS=536870912
for N in 1 2 4 8 16 24 32 48 64; do
  echo "[start] N=$N $(date)" >> practrand_B_triage_status.log
  ./diag_wideo_warmup_singleblock --stream wideo 111222 $N_WORDS 1 $N | "$PR" stdin32 -tlmin 256MB -tlmax 2GB -multithreaded > practrand_B_wideo_warmup_N${N}_K1_triage.log 2>&1
  echo "[done] N=$N $(date)" >> practrand_B_triage_status.log
done
echo "[ALL_DONE] $(date)" >> practrand_B_triage_status.log
