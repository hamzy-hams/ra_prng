#!/bin/sh
# Build exhaustive_analysis. Perlu ra_core_nomain.o (ra_core.c tanpa main())
# untuk extern-link ra_core_orbit di mode --verify.
set -e
cd "$(dirname "$0")"
FAMILY_DIR=../2026-9-1_family-productionization

if [ ! -f "$FAMILY_DIR/ra_core_nomain.o" ]; then
    gcc -O3 -march=native -std=gnu17 -include stdalign.h \
        -Dmain=ra_core_unused_main -c "$FAMILY_DIR/ra_core.c" \
        -o "$FAMILY_DIR/ra_core_nomain.o" -Wall -Wextra
fi

gcc -O3 -march=native -std=gnu17 -include stdalign.h \
    exhaustive_analysis.c -o exhaustive_analysis \
    "$FAMILY_DIR/ra_core_nomain.o" -lm -Wall -Wextra
echo "OK: ./exhaustive_analysis"
