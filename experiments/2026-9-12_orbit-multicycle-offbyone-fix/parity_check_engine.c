// Parity check: ra_core_orbit_into() vs. ra_core_engine_next() for rng
// values in the multi-cycle off-by-one bug zone (rng % 255 == 1, rng >
// 255), after the fix in ra_core.c. Verification step 4 of
// experiments/2026-9-12_orbit-multicycle-offbyone-fix/HANDOVER.md.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ra_core_engine.h"

static int check_one(uint32_t key, uint32_t addr_hi, size_t rng) {
    // ra_core_orbit_into() writes via fmemopen(out, rng*4+1, "wb"): glibc's
    // fmemopen("wb") appends a trailing NUL right after the last byte
    // written, so `out` must have at least one spare byte past rng words
    // (see ra_core_engine.c's own comment on this convention).
    uint32_t *buf_into = malloc(rng * sizeof(uint32_t) + 1);
    uint32_t *buf_engine = malloc(rng * sizeof(uint32_t));

    ra_core_orbit_into(key, addr_hi, rng, buf_into);

    ra_core_engine_t *e = malloc(ra_core_engine_struct_size());
    ra_core_engine_init(e, key, addr_hi);
    ra_core_engine_next(e, rng, buf_engine);
    free(e);

    int mismatch = memcmp(buf_into, buf_engine, rng * sizeof(uint32_t)) != 0;
    printf("key=%u addr_hi=%u rng=%zu: %s\n", key, addr_hi, rng,
           mismatch ? "MISMATCH" : "match");
    if (mismatch) {
        for (size_t i = 0; i < rng; ++i) {
            if (buf_into[i] != buf_engine[i]) {
                printf("  first diff at word %zu: into=0x%08x engine=0x%08x\n",
                       i, buf_into[i], buf_engine[i]);
                break;
            }
        }
    }
    free(buf_into);
    free(buf_engine);
    return mismatch;
}

int main(void) {
    int fail = 0;
    fail |= check_one(12345u, 0u, 256);
    fail |= check_one(12345u, 0u, 511);
    fail |= check_one(999u, 7u, 766);
    fail |= check_one(999u, 7u, 1021);
    // control: outside bug zone, should also match (engine/orbit always
    // agreed here even before the fix).
    fail |= check_one(12345u, 0u, 255);
    fail |= check_one(12345u, 0u, 300);
    printf(fail ? "PARITY CHECK: FAIL\n" : "PARITY CHECK: PASS\n");
    return fail;
}
