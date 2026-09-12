#!/usr/bin/env bash
# BUILD.sh -- HANDOVER.md Tahap A (4.1): compile all state-size-pareto-frontier
# variants. Two families of binary per (N, reseed_mode):
#   1. ra_core_param_N<N>_<MODE>       -- standalone CLI (checksum-gen/--stream),
#                                          both cores selectable by name.
#   2. battery_orbit_N<N>_<MODE>       -- tools/battery/* CLI contract, orbit only.
#      battery_singleblock_N<N>        -- same, singleblock only (mode-agnostic).
#
# N in {256,128,64,32,16} (5), reseed_mode in {LOWONLY,MUTUAL} (2, orbit-relevant
# only) -> 10 standalone + 10 battery_orbit + 5 battery_singleblock = 25 binaries.
set -euo pipefail
cd "$(dirname "$0")"

CFLAGS="-O3 -march=native -std=gnu17 -include stdalign.h -Wall -Wextra"
NS=(256 128 64 32 16)
MODES=(RESEED_LOWONLY RESEED_MUTUAL)
MODE_TAGS=(LOWONLY MUTUAL)

mkdir -p bin

for N in "${NS[@]}"; do
    for idx in 0 1; do
        MODE="${MODES[$idx]}"
        TAG="${MODE_TAGS[$idx]}"

        echo "== N=$N RESEED_MODE=$TAG =="
        gcc $CFLAGS -DRA_N=$N -DRA_RESEED_MODE=$MODE \
            ra_core_param.c -o "bin/ra_core_param_N${N}_${TAG}"

        gcc $CFLAGS -DRA_N=$N -DRA_RESEED_MODE=$MODE \
            battery_wrapper_orbit.c -o "bin/battery_orbit_N${N}_${TAG}"
    done

    # singleblock: reseed mode is dead code, build once per N (LOWONLY tag,
    # matches production naming; MUTUAL would be byte-identical).
    gcc $CFLAGS -DRA_N=$N -DRA_RESEED_MODE=RESEED_LOWONLY \
        battery_wrapper_singleblock.c -o "bin/battery_singleblock_N${N}"
done

echo "Build done: $(ls bin | wc -l) binaries in bin/"
