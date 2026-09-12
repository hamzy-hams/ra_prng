// multikey_stream_orbit_postreseed_k255.c
// experiments/2026-9-4_orbit-reseed-postboundary-bcfn -- companion to
// multikey_stream_orbit_postreseed_k1.c, POST_K=255 (the natural
// full-block post-reseed granularity) instead of 1. See that file's
// header comment and HANDOVER.md for full rationale -- only the block
// length differs here.
//
// Method: ra_core_orbit(key, 510, buf) into an fmemopen buffer, discard
// the first 255 words (pre-reseed cycle), keep the LAST 255 words (the
// full post-reseed cycle that follows the single ra_reseed() call
// ra_core_orbit makes between its two 255-word iterations for
// rng=255+255). Chains multiple keys the same way
// ../2026-9-4_orbit-kmin-battery/multikey_stream_orbit_k255.c chains
// pre-reseed blocks: consume `n` total output words in chunks of up to
// 255, one key per chunk.
//
// Same four modes as the K1 driver (argv[1]):
//   --stream <base_key> <n>          post-reseed test, Weyl-stepped keys
//   --stream-hashed <base_key> <n>   post-reseed test, SplitMix32 keys
//   --control <base_key> <n>         pre-reseed control, Weyl keys
//                                     (byte-identical logic to
//                                     ../2026-9-4_orbit-kmin-battery/
//                                     multikey_stream_orbit_k255.c)
//   --control-hashed <base_key> <n>  pre-reseed control, SplitMix32 keys
//
// Links against ra_core_nomain.o built from the CANONICAL
// ../2026-9-1_family-productionization/ra_core.c. See BUILD.sh.
//
// IMPORTANT (glibc fmemopen bug -- see HANDOVER.md sec 4 and
// ../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md):
// the fmemopen buffer must be data_size+1 bytes, never exactly data_size.
//
// IMPORTANT (2026-09-06, addr_hi fix -- see multikey_stream_orbit_postreseed_k1.c
// header for the full rationale): all four modes originally hardcoded
// addr_hi=0u, which does not exercise the configuration that actually
// closed the K=1+hashed-key gate (key AND addr_hi each varied through
// independent chains). Ported the same two-chain pattern here.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern uint32_t ra_core_orbit(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream);

#define WEYL_DELTA_KEY 0x9E3779B9u
#define WEYL_DELTA_ADDR 0x85EBCA6Bu

static uint32_t splitmix32(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    z = z ^ (z >> 31);
    return (uint32_t)(z >> 32);
}

#define PREBOUNDARY 255u
#define POST_K 255u
#define BUFWORDS (PREBOUNDARY + POST_K) /* 510 */

// Emit `want` (<= POST_K) words of the post-reseed cycle for one
// (key, addr_hi) pair.
static void emit_postreseed_block(uint32_t key, uint32_t addr_hi, size_t want) {
    char membuf[BUFWORDS * sizeof(uint32_t) + 1];
    FILE *mf = fmemopen(membuf, sizeof(membuf), "wb");
    if (!mf) { perror("fmemopen"); exit(1); }
    ra_core_orbit(key, addr_hi, BUFWORDS, mf);
    fclose(mf);
    fwrite(membuf + PREBOUNDARY * sizeof(uint32_t), sizeof(uint32_t), want,
           stdout);
}

int main(int argc, char **argv) {
    if (argc < 5) {
        fprintf(stderr,
                "usage: %s {--stream|--stream-hashed|--control|"
                "--control-hashed} <base_key> <base_addr_hi> <n>\n",
                argv[0]);
        return 1;
    }
    const char *mode = argv[1];
    uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
    uint32_t base_addr = (uint32_t)strtoul(argv[3], NULL, 0);
    size_t n = (size_t)strtoull(argv[4], NULL, 0);

    if (strcmp(mode, "--stream") == 0) {
        uint64_t block_idx = 0;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < POST_K ? remaining : POST_K;
            uint32_t key = base_key + (uint32_t)(block_idx * WEYL_DELTA_KEY);
            uint32_t addr_hi = base_addr + (uint32_t)(block_idx * WEYL_DELTA_ADDR);
            emit_postreseed_block(key, addr_hi, block_len);
            remaining -= block_len;
            ++block_idx;
        }
    } else if (strcmp(mode, "--stream-hashed") == 0) {
        uint64_t sm_key = 0xC0FFEE1234ULL ^ base_key;
        uint64_t sm_addr = 0xDEADBEEFCAFEULL ^ base_addr;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < POST_K ? remaining : POST_K;
            uint32_t key = splitmix32(&sm_key);
            uint32_t addr_hi = splitmix32(&sm_addr);
            emit_postreseed_block(key, addr_hi, block_len);
            remaining -= block_len;
        }
    } else if (strcmp(mode, "--control") == 0) {
        uint64_t block_idx = 0;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < POST_K ? remaining : POST_K;
            uint32_t key = base_key + (uint32_t)(block_idx * WEYL_DELTA_KEY);
            uint32_t addr_hi = base_addr + (uint32_t)(block_idx * WEYL_DELTA_ADDR);
            ra_core_orbit(key, addr_hi, block_len, stdout);
            remaining -= block_len;
            ++block_idx;
        }
    } else if (strcmp(mode, "--control-hashed") == 0) {
        uint64_t sm_key = 0xC0FFEE1234ULL ^ base_key;
        uint64_t sm_addr = 0xDEADBEEFCAFEULL ^ base_addr;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < POST_K ? remaining : POST_K;
            uint32_t key = splitmix32(&sm_key);
            uint32_t addr_hi = splitmix32(&sm_addr);
            ra_core_orbit(key, addr_hi, block_len, stdout);
            remaining -= block_len;
        }
    } else {
        fprintf(stderr, "unknown mode '%s'\n", mode);
        return 1;
    }
    return 0;
}
