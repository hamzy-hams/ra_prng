#!/usr/bin/env bash
# BUILD.sh
# Build the two post-reseed BCFN test drivers against the CANONICAL
# ra_core.c (../2026-9-1_family-productionization/ra_core.c) -- never
# copies it, always compiled fresh. See HANDOVER.md sec 2.2.
#
# Run from this folder: ./BUILD.sh
set -euo pipefail
cd "$(dirname "$0")"

CANON_DIR="../2026-9-1_family-productionization"
CFLAGS="-O3 -march=native -std=gnu17 -include stdalign.h -Wall -Wextra"

echo "=== compiling ra_core_nomain.o from ${CANON_DIR}/ra_core.c ==="
gcc $CFLAGS -Dmain=ra_core_unused_main -c "${CANON_DIR}/ra_core.c" \
    -o ra_core_nomain.o

echo "=== multikey_stream_orbit_postreseed_k1 ==="
gcc $CFLAGS multikey_stream_orbit_postreseed_k1.c ra_core_nomain.o \
    -o multikey_stream_orbit_postreseed_k1

echo "=== multikey_stream_orbit_postreseed_k255 ==="
gcc $CFLAGS multikey_stream_orbit_postreseed_k255.c ra_core_nomain.o \
    -o multikey_stream_orbit_postreseed_k255

echo "=== BUILD.sh done ==="
