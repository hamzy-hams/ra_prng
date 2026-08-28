#!/bin/bash
# PractRand quality screen (16GB, single checkpoint) for all 13 winning
# wirings + baseline_local for reference. This is a comparative screen
# across candidates, NOT a substitute for the full 128GB/1TB validation
# winner_wired.c already has -- 16GB is chosen as a reasonable middle
# ground between operand_search.py's 2MB search-loop filter and the
# expensive full-scale runs.
#
# No `set -e`/pipefail: the producer binary intentionally writes far more
# than RNG_test reads (count=999999999999 vs a 16GB checkpoint), so it gets
# SIGPIPE once RNG_test stops reading -- that is EXPECTED, not a failure.
# RNG_test's own exit status/output (checked via grep) is what matters.
set -u
cd "$(dirname "$0")"

PRACTRAND=~/Documents/research/PractRand/RNG_test
BINARIES="${1:-winner_wired v01 v02 v03 v04 v06 v07 v08 v09 v10 v11 v12 v13}"

mkdir -p practrand_16gb
for b in $BINARIES; do
    OUT="practrand_16gb/${b}.txt"
    echo "=== $b: starting $(date +%T) ==="
    ./"$b" --stream 1 999999999999 2>/dev/null | "$PRACTRAND" stdin32 -tlmin 16GB -tlmax 16GB -multithreaded > "$OUT" 2>&1
    if grep -qE "FAIL|SUSPICIOUS" "$OUT"; then
        echo "=== $b: ANOMALY FOUND $(date +%T) ==="
    else
        echo "=== $b: clean, done $(date +%T) ==="
    fi
done
echo "ALL DONE"
