// multikey_stream_orbit.c -- diagnostic tool, this session only. Chains
// many SHORT ra_core_orbit calls (one per key, 255 words each, matching
// singleblock's own segment length) with the SAME linear/Weyl key-stepping
// pattern multikey_stream.c uses for ra_core_singleblock, to test whether
// a PractRand BCFN anomaly seen with linearly-stepped keys comes from the
// shared init-formula's key-dependence (used identically by both
// ra_init_state_orbit and ra_init_state_singleblock for M[]) rather than
// from anything specific to the no-L singleblock code path.
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
