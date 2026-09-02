// multikey_stream_hashed.c -- diagnostic variant of multikey_stream.c.
// Same idea (chain many ra_core_singleblock calls into one stream) but
// derives each block's key via a SplitMix64-style avalanche mix of the
// block index, instead of a linear Weyl step (base_key + idx*0x9E3779B9).
// Purpose: isolate whether a PractRand BCFN anomaly seen with linearly-
// stepped keys is caused by ra_core_singleblock's own formula, or by the
// linear/predictable key SEQUENCE used to drive it. Not part of the
// production API -- diagnostic tool only, this session.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern uint32_t ra_core_singleblock(uint32_t key, size_t rng, FILE *raw_stream);

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

    size_t remaining = n;
    while (remaining > 0) {
        size_t block_len = remaining < 255 ? remaining : 255;
        uint32_t key = splitmix32(&state);
        ra_core_singleblock(key, block_len, stdout);
        remaining -= block_len;
    }
    return 0;
}
