#!/bin/bash
set -u
cd ~/ra_prng/experiments/2026-9-2_singleblock-o-width-fix
PR=~/Documents/research/PractRand/RNG_test
N=4294967296
for K in 1 2 4 8 16; do
  echo "[start] K=$K $(date)" >> practrand_A_wideo_extended_status.log
  ./diag_wideo_singleblock --stream wideo 111222 $N $K | "$PR" stdin32 -tlmin 256MB -tlmax 16GB -multithreaded > practrand_A_wideo_K${K}_16GB.log 2>&1
  echo "[done] K=$K $(date)" >> practrand_A_wideo_extended_status.log
done
echo "[ALL_DONE] $(date)" >> practrand_A_wideo_extended_status.log
