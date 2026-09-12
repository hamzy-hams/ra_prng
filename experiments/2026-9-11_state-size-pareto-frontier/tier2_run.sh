#!/usr/bin/env bash
# tier2_run.sh -- HANDOVER.md Tahap G (VPS): deep PractRand (2G/8G/16G,
# dual base-key) + dieharder 27-test (K=1 and K=N-1) for the 12 survivor
# variants from Tier1 (N in {256,128,64,32}, N=16 excluded entirely --
# FAILed Tier1 + non-monotonic floor scan).
#
# Runs 2 variants at a time in the background (VPS_ACCESS.md: 8 cores,
# "cukup untuk 2 run PractRand paralel"), live-flushes progress per line to
# SWEEP_SUMMARY.txt so an SSH disconnect does not lose already-checkpointed
# results (same pattern as the prior 1TB VPS runs). No retry on any
# individual FAIL (feedback_no_reseed_on_fail) -- continues past it to give
# every variant a verdict in one pass.
set -uo pipefail
cd "$(dirname "$0")"

NS=(256 128 64 32)
MODES=(LOWONLY MUTUAL)

mkdir -p tier2_out
SUMMARY=tier2_out/SWEEP_SUMMARY.txt
: > "$SUMMARY"

run_variant() {
    local label=$1 binary=$2 core=$3 n=$4
    local kmax=$((n - 1))
    local outdir="tier2_out/$label"
    mkdir -p "$outdir"
    echo "[$label] START $(date -u +%H:%M:%S)" >> "$SUMMARY"

    local continuous_flag=""
    [ "$core" = "orbit" ] && continuous_flag="--continuous"

    python3 ../../tools/battery/run_battery.py \
        --binary "$binary" --core "$core" \
        --skip-tier0 --k "1,${kmax}" $continuous_flag \
        --sizes 2G,8G,16G \
        --outdir "$outdir" > "$outdir/practrand.log" 2>&1
    local pr_rc=$?
    local pr_verdict="CLEAN"
    [ "$pr_rc" -ne 0 ] && pr_verdict="FAIL_OR_ERROR(rc=$pr_rc)"
    echo "[$label] PRACTRAND $pr_verdict" >> "$SUMMARY"

    python3 dieharder_battery_param.py "$binary" "$label" 1 > "$outdir/dieharder_K1.log" 2>&1
    local dh1_rc=$?
    local dh1_verdict="CLEAN"
    [ "$dh1_rc" -ne 0 ] && dh1_verdict="FAIL(rc=$dh1_rc)"
    echo "[$label] DIEHARDER_K1 $dh1_verdict" >> "$SUMMARY"

    python3 dieharder_battery_param.py "$binary" "$label" "$kmax" > "$outdir/dieharder_Kmax.log" 2>&1
    local dhk_rc=$?
    local dhk_verdict="CLEAN"
    [ "$dhk_rc" -ne 0 ] && dhk_verdict="FAIL(rc=$dhk_rc)"
    echo "[$label] DIEHARDER_K${kmax} $dhk_verdict" >> "$SUMMARY"

    local overall="CLEAN"
    { [ "$pr_rc" -ne 0 ] || [ "$dh1_rc" -ne 0 ] || [ "$dhk_rc" -ne 0 ]; } && overall="FAIL"
    echo "$label: $overall (practrand=$pr_verdict dieharder_K1=$dh1_verdict dieharder_K${kmax}=$dhk_verdict)" >> "$SUMMARY"
    echo "[$label] DONE $(date -u +%H:%M:%S)" >> "$SUMMARY"
}

# build the 12-variant queue
VARIANTS=()
for n in "${NS[@]}"; do
    for tag in "${MODES[@]}"; do
        VARIANTS+=("orbit_N${n}_${tag}|bin/battery_orbit_N${n}_${tag}|orbit|${n}")
    done
    VARIANTS+=("singleblock_N${n}|bin/battery_singleblock_N${n}|singleblock|${n}")
done

echo "TIER2 SWEEP START $(date -u +%Y-%m-%dT%H:%M:%SZ) -- ${#VARIANTS[@]} variants, 2 at a time" >> "$SUMMARY"

i=0
while [ "$i" -lt "${#VARIANTS[@]}" ]; do
    pids=()
    for slot in 0 1; do
        idx=$((i + slot))
        if [ "$idx" -lt "${#VARIANTS[@]}" ]; then
            IFS='|' read -r label binary core n <<< "${VARIANTS[$idx]}"
            run_variant "$label" "$binary" "$core" "$n" &
            pids+=($!)
        fi
    done
    for pid in "${pids[@]}"; do
        wait "$pid"
    done
    i=$((i + 2))
done

echo "TIER2 SWEEP DONE $(date -u +%Y-%m-%dT%H:%M:%SZ)" >> "$SUMMARY"
echo "=== FINAL SUMMARY ==="
cat "$SUMMARY"
