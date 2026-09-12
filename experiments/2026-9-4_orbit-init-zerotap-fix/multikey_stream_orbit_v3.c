// multikey_stream_orbit_v3.c
// experiments/2026-9-4_orbit-init-zerotap-fix -- driver to test whether
// the +1 index-shift patch in ra_core_v3.c's ra_init_state_orbit (fixes
// L[0]/M[0] being always-zero, see that file's header comment and
// RESULTS.md in this folder) closes the K=1+hashed-key BCFN gap found in
// ../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md.
//
// Same 4 modes as ../2026-9-4_orbit-reseed-postboundary-bcfn's driver
// (pre-reseed pattern only, K=1 here; K=255 mode included for regression
// sanity against the fix):
//   --k1-weyl <base_key> <n>     K=1, Weyl-stepped keys
//   --k1-hashed <base_key> <n>   K=1, SplitMix32-hashed keys (the FAILING case)
//   --k255-weyl <base_key> <n>   K=255, Weyl-stepped keys
//   --k255-hashed <base_key> <n> K=255, SplitMix32-hashed keys
// Links against ra_core_v3_nomain.o (patched). See BUILD.sh.

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
    if (argc < 4) {
        fprintf(stderr,
                "usage: %s {--k1-weyl|--k1-hashed|--k255-weyl|--k255-hashed} "
                "<base_key> <n>\n",
                argv[0]);
        return 1;
    }
    const char *mode = argv[1];
    uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
    size_t n = (size_t)strtoull(argv[3], NULL, 0);

    if (strcmp(mode, "--k1-weyl") == 0) {
        for (size_t i = 0; i < n; ++i) {
            uint32_t key = base_key + (uint32_t)((uint64_t)i * 0x9E3779B9u);
            ra_core_orbit(key, 1, stdout);
        }
    } else if (strcmp(mode, "--k1-hashed") == 0) {
        uint64_t state = (uint64_t)base_key;
        for (size_t i = 0; i < n; ++i) {
            uint32_t key = splitmix32(&state);
            ra_core_orbit(key, 1, stdout);
        }
    } else if (strcmp(mode, "--k255-weyl") == 0) {
        uint64_t block_idx = 0;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < 255 ? remaining : 255;
            uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
            ra_core_orbit(key, block_len, stdout);
            remaining -= block_len;
            ++block_idx;
        }
    } else if (strcmp(mode, "--k255-hashed") == 0) {
        uint64_t state = (uint64_t)base_key;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < 255 ? remaining : 255;
            uint32_t key = splitmix32(&state);
            ra_core_orbit(key, block_len, stdout);
            remaining -= block_len;
        }
    } else {
        fprintf(stderr, "unknown mode '%s'\n", mode);
        return 1;
    }
    return 0;
}
