#!/bin/bash
# perf stat (instructions,cycles) for all 13 winning wirings + baseline_local
# for reference, 3 repeats each (median taken in the summary step), same
# flags/TOTAL_RNG as RESULTS.md's original methodology.
set -euo pipefail
cd "$(dirname "$0")"

BINARIES="baseline_local winner_wired v01 v02 v03 v04 v06 v07 v08 v09 v10 v11 v12 v13"

OUT=other_winners_perf.log
: > "$OUT"

for b in $BINARIES; do
    echo "=== $b ===" | tee -a "$OUT"
    perf stat -e instructions,cycles -r 3 "./$b" >> "$OUT" 2>&1
    echo "" >> "$OUT"
done

echo "Done. See $OUT"
