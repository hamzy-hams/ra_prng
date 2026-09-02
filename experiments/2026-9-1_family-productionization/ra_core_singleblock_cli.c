// ra_core_singleblock_cli.c
// Tahap 2 (family-productionization): thin CLI wrapper exposing
// ra_core_singleblock via the plain `--stream <key> <n>` shape (no mode
// arg), matching common.py's `stream_values()`/`stream_popen()` default
// invocation (`[binary, "--stream", seed, n]`) so
// experiments/2026-8-29_parallelization-research/'s cross_correlation.py
// and collision_scan.py can be reused unmodified against
// ra_core_singleblock by re-pointing `binary=` -- same pattern
// cross_correlation_ra_prng2.py already used for ra_prng2.c
// (`ensure_ra_prng2_cli()`, a dedicated single-purpose CLI binary, rather
// than editing those scripts' hard-coded arg shape). Does not modify or
// duplicate ra_core.c's formula: links against ra_core.c as an object file.
//
// Compile:
//   gcc -O3 -march=native -std=gnu17 -include stdalign.h \
//       -Dmain=ra_core_unused_main -c ra_core.c -o ra_core_nomain.o
//   gcc -O3 -march=native -std=gnu17 ra_core_singleblock_cli.c \
//       ra_core_nomain.o -o ra_core_singleblock_cli

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern uint32_t ra_core_singleblock(uint32_t key, size_t rng, FILE *raw_stream);

int main(int argc, char **argv) {
    if (argc < 4 || strcmp(argv[1], "--stream") != 0) {
        fprintf(stderr, "usage: %s --stream <key> <n>  (n must be <= 255)\n", argv[0]);
        return 1;
    }
    uint32_t key = (uint32_t)strtoul(argv[2], NULL, 0);
    size_t n = (size_t)strtoull(argv[3], NULL, 0);
    ra_core_singleblock(key, n, stdout);
    return 0;
}
