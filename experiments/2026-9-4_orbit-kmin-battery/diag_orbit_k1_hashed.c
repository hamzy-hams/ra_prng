// diag_orbit_k1_hashed.c -- diagnostic, this session only (2026-09-04).
// Independent reproduction check for a control-sanity FAIL found while
// building ../2026-9-4_orbit-reseed-postboundary-bcfn/: orbit K=1,
// hashed-key (SplitMix32) ordering shows an escalating BCFN-style
// PractRand anomaly (FPF-14+6/16) even though this gate was already
// closed 2026-09-04 (RESULTS.md in this folder) -- but that gate only
// ever tested Weyl-stepped keys, never hashed. This file links against
// THIS FOLDER'S OWN ra_core_nomain_v2.o (the exact object that passed
// the Weyl gate) to rule out any possibility that the FAIL is an
// artifact of the new driver files in the postreseed-bcfn folder.
// Key-stepping pattern copied from
// ../2026-9-1_family-productionization/multikey_stream_hashed.c.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern uint32_t ra_core_orbit(uint32_t key, size_t rng, FILE *raw_stream);

static uint32_t splitmix32(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    z = z ^ (z >> 31);
    return (uint32_t)(z >> 32);
}

int main(int argc, char **argv) {
    if (argc < 4 || strcmp(argv[1], "--stream") != 0) {
        fprintf(stderr, "usage: %s --stream <base_key> <n>\n", argv[0]);
        return 1;
    }
    uint64_t state = (uint64_t)(uint32_t)strtoul(argv[2], NULL, 0);
    size_t n = (size_t)strtoull(argv[3], NULL, 0);

    for (size_t i = 0; i < n; ++i) {
        uint32_t key = splitmix32(&state);
        ra_core_orbit(key, 1, stdout);
    }
    return 0;
}
