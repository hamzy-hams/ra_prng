#!/usr/bin/env bash
# BUILD.sh
# Rebuild the addr_hi-increment chaining driver FRESH from the promoted,
# canonical src/ra_core/c/ra_core.c (the exact source the pip-ra-core
# library will wrap) -- source referenced by relative path, never copied.
#
# Run from this folder: ./BUILD.sh
set -euo pipefail
cd "$(dirname "$0")"

CORE_SRC="../../src/ra_core/c/ra_core.c"
CFLAGS="-O3 -march=native -std=gnu17 -include stdalign.h -Wall -Wextra"

echo "=== compiling ra_core_nomain.o from ${CORE_SRC} ==="
gcc $CFLAGS -Dmain=ra_core_unused_main -c "${CORE_SRC}" -o ra_core_nomain.o

echo "=== multikey_stream_addrhi_k (addr_hi-increment chaining, new) ==="
gcc $CFLAGS multikey_stream_addrhi_k.c ra_core_nomain.o -o multikey_stream_addrhi_k

echo "=== BUILD.sh done ==="
