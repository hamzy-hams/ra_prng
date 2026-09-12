// multikey_stream_orbit_postreseed_k1.c
// experiments/2026-9-4_orbit-reseed-postboundary-bcfn -- see HANDOVER.md.
//
// Tests whether ra_reseed/ra_hash (called by ra_core_orbit every 255
// words) leaks structure into the FIRST cycle immediately AFTER one
// reseed, for many independent keys -- the 1-cycle-shifted analog of the
// original pre-reseed BCFN defect test
// (../2026-9-1_multikey-remix-search/HANDOVER.md), which never exercised
// ra_reseed at all (K<=255 -> ra_core_orbit's single iteration returns
// before ra_reseed is ever called; see ra_core.c ra_core_orbit()).
//
// Method: call ra_core_orbit(key, 256, buf) into an fmemopen buffer,
// discard the first 255 words (the pre-reseed cycle, already proven
// clean post-w8_f10_i0 -- see ../2026-9-4_orbit-kmin-battery/RESULTS.md),
// keep ONLY word index 255 (the single word from the post-reseed cycle
// that follows). K=1 here means "one post-reseed word per key", the
// worst-case multistream reinit granularity, same rationale as
// ../2026-9-4_orbit-kmin-battery/multikey_stream_orbit_k1.c for the
// pre-reseed case.
//
// Links against ra_core_nomain.o built from the CANONICAL
// ../2026-9-1_family-productionization/ra_core.c (NOT ra_core_v2.c --
// the w8_f10_i0 fix + rolling-o optimization is already in canonical as
// of 2026-09-04). Never copies any static function from ra_core.c --
// extern-links ra_core_orbit(), same as every other multikey_stream*
// driver in this repo. See BUILD.sh.
//
// Four modes (argv[1]):
//   --stream <base_key> <n>          post-reseed test, Weyl-stepped keys
//   --stream-hashed <base_key> <n>   post-reseed test, SplitMix32 keys
//   --control <base_key> <n>         pre-reseed control, Weyl keys
//                                     (byte-identical logic to
//                                     ../2026-9-4_orbit-kmin-battery/
//                                     multikey_stream_orbit_k1.c)
//   --control-hashed <base_key> <n>  pre-reseed control, SplitMix32 keys
// Control modes exist so HANDOVER.md sec 2.3's sanity check runs from the
// SAME binary, linked against the SAME canonical ra_core.c, without
// rebuilding another folder.
//
// IMPORTANT (glibc fmemopen bug, see HANDOVER.md sec 4 and
// ../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md):
// the fmemopen buffer must be data_size+1 bytes, never exactly data_size
// -- glibc writes a NUL terminator that clobbers the top byte of the last
// word (little-endian) if the buffer is sized exactly to the data.
//
// IMPORTANT (reseed boundary off-by-one, found while implementing this
// file -- NOT documented in HANDOVER.md, verified empirically against
// ra_core_orbit before writing this comment): rng=255+1=256 does NOT
// call ra_reseed() at all. ra_core_orbit's post-cycle check is
// `if (count <= 1) return cons;` -- after the first 255-word cycle,
// remaining count = rng-255. For rng=256 that remainder is exactly 1,
// which trips the early return BEFORE ra_reseed is ever called, so the
// entire "K=1 post-reseed word" premise silently degenerates into "just
// the last word of the pre-reseed cycle" if rng=256 is used. Confirmed
// by direct dump: ra_core_orbit(key, 256, f) writes only 255 words to f,
// never 256. The remainder must be >1 for reseed to fire, so this file
// requests rng=257 (PREBOUNDARY+2) -- guarantees ra_reseed() runs and the
// post-reseed cycle emits at least 2 words -- and keeps ONLY the first
// one (buffer index 255), discarding the second (index 256). That first
// word is still the single word immediately following the one-and-only
// ra_reseed() call, i.e. exactly what HANDOVER.md sec 2.1 intends.
//
// IMPORTANT (2026-09-06, addr_hi fix -- why this file now varies addr_hi
// too): all four modes originally hardcoded addr_hi=0u. That is NOT the
// configuration that closed the K=1+hashed-key gate (see
// ../2026-9-5_orbit-address-widen-64bit/RESULTS.md and
// multikey_stream_addr_hi.c) -- that battery varied BOTH key and addr_hi
// through two INDEPENDENT chains (weyl: independent deltas; hashed: two
// independent splitmix32 state chains). addr_hi held at a constant 0
// collapses back to the old 32-bit-key-only birthday-paradox domain, so
// the original 2026-09-04 --control-hashed FAIL would very likely still
// reproduce here even though the underlying formula is fixed. Ported the
// exact two-chain pattern from multikey_stream_addr_hi.c so the mandatory
// sanity control (HANDOVER.md sec 2.3) actually exercises the
// gate-closing configuration before the post-reseed question is tested.

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
#define POST_K 1u
#define REQUEST_WORDS (PREBOUNDARY + 2u) /* 257 -- see reseed-boundary note above */

// Emit the single post-reseed word for one (key, addr_hi) pair.
static void emit_postreseed_word(uint32_t key, uint32_t addr_hi) {
    char membuf[REQUEST_WORDS * sizeof(uint32_t) + 1];
    FILE *mf = fmemopen(membuf, sizeof(membuf), "wb");
    if (!mf) { perror("fmemopen"); exit(1); }
    ra_core_orbit(key, addr_hi, REQUEST_WORDS, mf);
    fclose(mf);
    fwrite(membuf + PREBOUNDARY * sizeof(uint32_t), sizeof(uint32_t), POST_K,
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
        for (size_t i = 0; i < n; ++i) {
            uint32_t key = base_key + (uint32_t)((uint64_t)i * WEYL_DELTA_KEY);
            uint32_t addr_hi = base_addr + (uint32_t)((uint64_t)i * WEYL_DELTA_ADDR);
            emit_postreseed_word(key, addr_hi);
        }
    } else if (strcmp(mode, "--stream-hashed") == 0) {
        uint64_t sm_key = 0xC0FFEE1234ULL ^ base_key;
        uint64_t sm_addr = 0xDEADBEEFCAFEULL ^ base_addr;
        for (size_t i = 0; i < n; ++i) {
            uint32_t key = splitmix32(&sm_key);
            uint32_t addr_hi = splitmix32(&sm_addr);
            emit_postreseed_word(key, addr_hi);
        }
    } else if (strcmp(mode, "--control") == 0) {
        for (size_t i = 0; i < n; ++i) {
            uint32_t key = base_key + (uint32_t)((uint64_t)i * WEYL_DELTA_KEY);
            uint32_t addr_hi = base_addr + (uint32_t)((uint64_t)i * WEYL_DELTA_ADDR);
            ra_core_orbit(key, addr_hi, POST_K, stdout);
        }
    } else if (strcmp(mode, "--control-hashed") == 0) {
        uint64_t sm_key = 0xC0FFEE1234ULL ^ base_key;
        uint64_t sm_addr = 0xDEADBEEFCAFEULL ^ base_addr;
        for (size_t i = 0; i < n; ++i) {
            uint32_t key = splitmix32(&sm_key);
            uint32_t addr_hi = splitmix32(&sm_addr);
            ra_core_orbit(key, addr_hi, POST_K, stdout);
        }
    } else {
        fprintf(stderr, "unknown mode '%s'\n", mode);
        return 1;
    }
    return 0;
}
