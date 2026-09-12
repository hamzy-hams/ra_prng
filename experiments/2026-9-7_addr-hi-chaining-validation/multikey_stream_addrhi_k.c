// multikey_stream_addrhi_k.c
// Fase 0 validasi untuk rencana pip library ra-core: pola chaining
// BERBEDA dari yang sudah closed-gate. Battery yang sudah PASS
// (../2026-9-2_singleblock-k-threshold-characterization/multikey_stream_k.c,
// ../2026-9-1_production-candidate-battery/multikey_stream_singleblock_k1.c)
// semuanya chaining lewat KEY-increment (Weyl 0x9E3779B9) dengan addr_hi
// TETAP 0. Driver ini membalik peran: `key` TETAP (base_key), `addr_hi`
// yang increment tiap block baru (addr_hi = block_idx) -- pola yang
// diusulkan untuk fitur shuffle N>255 di pip-ra-core (chaining block
// singleblock lewat address, bukan key).
//
// Tidak mengubah/menduplikasi formula ra_core.c: link ke object file
// ra_core_nomain.o (dikompilasi dari ra_core.c dengan -Dmain=... seperti
// driver-driver sejenis lainnya).
//
// --stream <base_key> <n> <K>: emit `n` word uint32 total, tiap block
// berisi <=K word dari ra_core_singleblock(base_key, addr_hi=block_idx, ...).

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern uint32_t ra_core_singleblock(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream);

int main(int argc, char **argv) {
    if (argc < 5 || strcmp(argv[1], "--stream") != 0) {
        fprintf(stderr, "usage: %s --stream <base_key> <n> <K>\n", argv[0]);
        return 1;
    }
    uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
    size_t n = (size_t)strtoull(argv[3], NULL, 0);
    size_t K = (size_t)strtoull(argv[4], NULL, 0);
    if (K == 0 || K > 255) {
        fprintf(stderr, "K must be in [1,255]\n");
        return 1;
    }

    uint32_t addr_hi = 0;
    size_t remaining = n;
    while (remaining > 0) {
        size_t block_len = remaining < K ? remaining : K;
        ra_core_singleblock(base_key, addr_hi, block_len, stdout);
        remaining -= block_len;
        ++addr_hi;
    }
    return 0;
}
