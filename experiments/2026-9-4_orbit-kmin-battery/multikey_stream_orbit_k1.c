// multikey_stream_orbit_k1.c
// experiments/2026-9-4_orbit-kmin-battery -- statistical re-validation gate
// for the orbit K-small-defect fix (w8_f10_i0 formula + finalizer) applied
// in ../2026-9-4_orbit-fix-and-wideo-rolling-optimization/ra_core_v2.c.
// Deferred battery per that folder's HANDOVER.md sec 6: "JANGAN promosikan
// ke src/ atau ra_core.c kanonik sampai battery ini selesai dan PASS."
//
// Same chaining idea as
// ../2026-9-1_production-candidate-battery/multikey_stream_singleblock_k1.c,
// but calls ra_core_orbit() from ra_core_v2.c (the FIXED formula) instead
// of ra_core_singleblock() from canonical ra_core.c. block_len=1 is the
// worst-case "multistream" reinit pattern -- one word generated, then
// immediately reinitialized with the next key -- exactly the usage
// pattern whose K-small defect exposure motivated this whole battery
// (user question: "apa yang terjadi kalau ada orang mengambil orbit K=1
// sebagai input?").
//
// Links against ra_core_nomain_v2.o (ra_core_v2.c built with
// -Dmain=ra_core_unused_main), never copies ra_core_v2.c. See BUILD.sh.
//
// --stream <base_key> <n>: emits `n` raw uint32 words total, one
// ra_core_orbit(key, 1, ...) call per word. Key derivation is the same
// Weyl/golden-ratio increment (0x9E3779B9) as every other multikey_stream*
// driver in this repo, so results are directly comparable.

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

    for (size_t i = 0; i < n; ++i) {
        uint32_t key = base_key + (uint32_t)((uint64_t)i * 0x9E3779B9u);
        ra_core_orbit(key, 1, stdout);
    }
    return 0;
}
