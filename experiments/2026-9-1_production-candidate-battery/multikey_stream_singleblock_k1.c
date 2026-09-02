// multikey_stream_singleblock_k1.c
// production-candidate-battery, Step 5/6: same chaining idea as
// ../2026-9-1_family-productionization/multikey_stream.c, but with
// block_len hardcoded to 1 instead of 255 -- the most aggressive reinit
// pattern ra_core_singleblock supports (one word generated, then
// immediately reinitialized with the next key). This pattern was never
// tested by the original multikey battery (which only exercised the
// natural 255-word full-block use), so it gets its own dedicated
// dieharder/PractRand driver rather than a parametrized CLI flag, to keep
// the 3-argument `run_dieharder_battery_multikey.py --stream <key> <n>`
// contract unchanged (same rationale as multikey_stream_orbit.c/
// multikey_stream_hashed.c: one file = one fixed pattern).
//
// Does not modify or duplicate ra_core.c's formula: links against
// ra_core.c directly (built as an object file with -Dmain=<other name>).
//
// --stream <base_key> <n>: emits `n` raw uint32 words total, one
// ra_core_singleblock(key, 1, ...) call per word. Key derivation is the
// same Weyl/golden-ratio increment (0x9E3779B9) as multikey_stream.c, so
// results are directly comparable modulo block_len.
//
// Compile: see BUILD.sh (links against ra_core_nomain.o).

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern uint32_t ra_core_singleblock(uint32_t key, size_t rng, FILE *raw_stream);

int main(int argc, char **argv) {
    if (argc < 4 || strcmp(argv[1], "--stream") != 0) {
        fprintf(stderr, "usage: %s --stream <base_key> <n>\n", argv[0]);
        return 1;
    }
    uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
    size_t n = (size_t)strtoull(argv[3], NULL, 0);

    for (size_t i = 0; i < n; ++i) {
        uint32_t key = base_key + (uint32_t)((uint64_t)i * 0x9E3779B9u);
        ra_core_singleblock(key, 1, stdout);
    }
    return 0;
}
