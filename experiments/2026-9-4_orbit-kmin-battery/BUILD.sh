#!/usr/bin/env bash
# BUILD.sh
# Rebuild every binary this battery needs, FRESH, from
# ../2026-9-4_orbit-fix-and-wideo-rolling-optimization/ra_core_v2.c
# (referenced by relative path, never copied). This folder statistically
# re-validates the orbit K-small-defect fix + rolling-o optimization that
# HANDOVER.md in that folder explicitly deferred and gated on this battery
# passing before any promotion to canonical ra_core.c or src/.
#
# Run from this folder: ./BUILD.sh
set -euo pipefail
cd "$(dirname "$0")"

V2_DIR="../2026-9-4_orbit-fix-and-wideo-rolling-optimization"
CFLAGS="-O3 -march=native -std=gnu17 -include stdalign.h -Wall -Wextra"

echo "=== compiling ra_core_nomain_v2.o from ${V2_DIR}/ra_core_v2.c ==="
gcc $CFLAGS -Dmain=ra_core_unused_main -c "${V2_DIR}/ra_core_v2.c" -o ra_core_nomain_v2.o

echo "=== ra_core_v2 (validate/--stream CLI, own copy of the main) ==="
gcc $CFLAGS "${V2_DIR}/ra_core_v2.c" -o ra_core_v2

echo "=== multikey_stream_orbit_k1 (K=1 chaining, fixed formula) ==="
gcc $CFLAGS multikey_stream_orbit_k1.c ra_core_nomain_v2.o -o multikey_stream_orbit_k1

echo "=== multikey_stream_orbit_k255 (K=255 chaining, fixed formula) ==="
gcc $CFLAGS multikey_stream_orbit_k255.c ra_core_nomain_v2.o -o multikey_stream_orbit_k255

echo "=== BUILD.sh done ==="
