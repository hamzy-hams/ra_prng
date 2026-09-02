#!/usr/bin/env bash
# BUILD.sh
# Rebuild every binary this battery needs, FRESH, from
# ../2026-9-1_family-productionization/ra_core.c (source referenced by
# relative path, never copied). This folder is a validation GATE, so it
# must not inherit any binary that could still be linked against a
# pre-fix object file -- see RESULTS.md's "Step 0" note: the old binaries
# in family-productionization/ have mtimes ranging 10:17-11:22, some
# OLDER than ra_core.c/ra_core_nomain.o (17:04, the post-fix rebuild), so
# their provenance is not fully trustworthy without a from-scratch rebuild
# here.
#
# Run from this folder: ./BUILD.sh
set -euo pipefail
cd "$(dirname "$0")"

FAMILY_DIR="../2026-9-1_family-productionization"
CFLAGS="-O3 -march=native -std=gnu17 -include stdalign.h -Wall -Wextra"

echo "=== compiling ra_core_nomain.o from ${FAMILY_DIR}/ra_core.c ==="
gcc $CFLAGS -Dmain=ra_core_unused_main -c "${FAMILY_DIR}/ra_core.c" -o ra_core_nomain.o

echo "=== ra_core (validate/--stream CLI) ==="
gcc $CFLAGS -include stdalign.h "${FAMILY_DIR}/ra_core.c" -o ra_core

echo "=== ra_core_singleblock_cli ==="
gcc $CFLAGS "${FAMILY_DIR}/ra_core_singleblock_cli.c" ra_core_nomain.o -o ra_core_singleblock_cli

echo "=== bench_ra_core ==="
gcc $CFLAGS "${FAMILY_DIR}/bench_ra_core.c" ra_core_nomain.o -o bench_ra_core

echo "=== multikey_stream (K=255 chaining) ==="
gcc $CFLAGS "${FAMILY_DIR}/multikey_stream.c" ra_core_nomain.o -o multikey_stream

echo "=== multikey_stream_singleblock_k1 (K=1 chaining, new) ==="
gcc $CFLAGS multikey_stream_singleblock_k1.c ra_core_nomain.o -o multikey_stream_singleblock_k1

echo "=== scrambler_ra_core_singleblock (shuffle test driver, new) ==="
gcc $CFLAGS scrambler_ra_core_singleblock.c ra_core_nomain.o -o scrambler_ra_core_singleblock

echo "=== BUILD.sh done ==="
