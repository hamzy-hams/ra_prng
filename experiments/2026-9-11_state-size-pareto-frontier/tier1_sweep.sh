#!/usr/bin/env bash
# tier1_sweep.sh -- HANDOVER.md Tahap D (4.3 Tier1): PractRand screen
# (DEFAULT_SIZES 256M/2G/2G, weyl+hashed, dual base_key) for all 15
# survivor-Tier0 variants. --skip-tier0 (already done separately in
# tier0_quality.py). Continues past a FAILing variant (does not abort the
# whole sweep) so every variant gets a verdict in one pass -- no retry on
# FAIL for any individual lane (feedback_no_reseed_on_fail).
set -uo pipefail
cd "$(dirname "$0")"

NS=(256 128 64 32 16)
MODES=(LOWONLY MUTUAL)

mkdir -p tier1_out
SUMMARY=tier1_out/SWEEP_SUMMARY.txt
: > "$SUMMARY"

run_variant() {
    local label=$1 binary=$2 core=$3 k=$4 continuous_flag=$5
    local outdir="tier1_out/$label"
    mkdir -p "$outdir"
    echo "===================================================================="
    echo "== $label (core=$core k=$k continuous=$continuous_flag) =="
    echo "===================================================================="
    python3 ../../tools/battery/run_battery.py \
        --binary "$binary" --core "$core" \
        --skip-tier0 --k "$k" $continuous_flag \
        --outdir "$outdir" 2>&1 | tee "$outdir/run.log"
    local rc=${PIPESTATUS[0]}
    local verdict="CLEAN"
    [ "$rc" -ne 0 ] && verdict="FAIL_OR_ERROR(rc=$rc)"
    echo "$label: $verdict" >> "$SUMMARY"
}

for n in "${NS[@]}"; do
    kmax=$((n - 1))
    for tag in "${MODES[@]}"; do
        run_variant "orbit_N${n}_${tag}" "bin/battery_orbit_N${n}_${tag}" orbit "1,${kmax}" "--continuous"
    done
    run_variant "singleblock_N${n}" "bin/battery_singleblock_N${n}" singleblock "1,${kmax}" ""
done

echo "===================================================================="
echo "SWEEP DONE. Summary:"
cat "$SUMMARY"
