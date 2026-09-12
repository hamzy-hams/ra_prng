#!/usr/bin/env bash
# BUILD.sh
# Build the K1/K255 x Weyl/hashed driver against the PATCHED ra_core_v3.c
# (init formula +1 index shift, see that file's header comment). Not
# canonical -- this folder only tests a fix hypothesis.
set -euo pipefail
cd "$(dirname "$0")"

CFLAGS="-O3 -march=native -std=gnu17 -include stdalign.h -Wall -Wextra"

echo "=== compiling ra_core_v3_nomain.o from ra_core_v3.c ==="
gcc $CFLAGS -Dmain=ra_core_unused_main -c ra_core_v3.c -o ra_core_v3_nomain.o

echo "=== multikey_stream_orbit_v3 ==="
gcc $CFLAGS multikey_stream_orbit_v3.c ra_core_v3_nomain.o -o multikey_stream_orbit_v3

echo "=== BUILD.sh done ==="
