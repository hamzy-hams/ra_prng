// multikey_stream_orbit_k255.c
// experiments/2026-9-4_orbit-kmin-battery -- companion to
// multikey_stream_orbit_k1.c, block_len=255 (the natural full-block reinit
// pattern) instead of 1. Adapted from
// ../2026-9-1_family-productionization/multikey_stream_orbit.c, whose
// extern declaration and chaining logic are unchanged -- the only
// difference is this file links against ra_core_nomain_v2.o (the FIXED
// w8_f10_i0 + rolling-o formula from ra_core_v2.c) instead of the
// canonical pre-fix ra_core.c object. See BUILD.sh.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern uint32_t ra_core_orbit(uint32_t key, size_t rng, FILE *raw_stream);

int main(int argc, char **argv) {
    if (argc < 4 || strcmp(argv[1], "--stream") != 0) {
        fprintf(stderr, "usage: %s --stream <base_key> <n>\n", argv[0]);
        return 1;
    }
    uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
    size_t n = (size_t)strtoull(argv[3], NULL, 0);

    uint64_t block_idx = 0;
    size_t remaining = n;
    while (remaining > 0) {
        size_t block_len = remaining < 255 ? remaining : 255;
        uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
        ra_core_orbit(key, block_len, stdout);
        remaining -= block_len;
        ++block_idx;
    }
    return 0;
}
