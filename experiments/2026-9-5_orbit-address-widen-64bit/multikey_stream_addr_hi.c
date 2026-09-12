// multikey_stream_addr_hi.c -- Tahap 6 (HANDOVER.md §5.5): driver generik
// untuk dieharder + PractRand staged, kandidat 2-param (key,addr_hi).
// Adaptasi dari
// ../2026-9-4_orbit-kmin-battery/multikey_stream_orbit_k1.c/_k255.c: alih-
// alih 2 file terpisah per K, satu driver dengan <block_len> sebagai CLI
// arg (K=1 -> reinit tiap word, K=255 -> reinit tiap block, pola chaining
// sama persis -- key/addr_hi Weyl-increment per BLOCK index, bukan per
// word, meniru persis pola *_k255.c untuk block_len>1).
//
// Ordering:
//   weyl   : key/addr_hi = base + block_idx * WEYL_DELTA_{KEY,ADDR}
//            (regresi -- harus tetap CLEAN, dua delta beda spy independen)
//   hashed : key/addr_hi dari 2 splitmix64-chain independen (akseptansi
//            utama -- axis yang FAIL di 32-bit lama)
//
// Link terhadap candidate_X_nomain.o (candidates/), TIDAK PERNAH menyalin
// candidate_X.c -- lihat BUILD.sh pola project ini.
//
// Compile: gcc -O3 -march=native -std=gnu17 multikey_stream_addr_hi.c \
//            candidates/candidate_X_nomain.o -o multikey_addr_hi_candidate_X

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern uint32_t ra_core_orbit(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream);

#define WEYL_DELTA_KEY 0x9E3779B9u
#define WEYL_DELTA_ADDR 0x85EBCA6Bu

static inline uint64_t splitmix64_next(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97f4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}
static inline uint32_t splitmix32_hi(uint64_t *state) {
    return (uint32_t)(splitmix64_next(state) >> 32);
}

int main(int argc, char **argv) {
    if (argc < 6) {
        fprintf(stderr, "usage: %s --stream <base_key> <base_addr_hi> <block_len> "
                         "<weyl|hashed> <n>\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "--stream") != 0) {
        fprintf(stderr, "usage: %s --stream <base_key> <base_addr_hi> <block_len> "
                         "<weyl|hashed> <n>\n", argv[0]);
        return 1;
    }
    uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
    uint32_t base_addr = (uint32_t)strtoul(argv[3], NULL, 0);
    size_t block_len = (size_t)strtoull(argv[4], NULL, 0);
    const char *order = argv[5];
    size_t n = (size_t)strtoull(argv[6], NULL, 0);

    int use_weyl = strcmp(order, "weyl") == 0;
    int use_hashed = strcmp(order, "hashed") == 0;
    if (!use_weyl && !use_hashed) {
        fprintf(stderr, "order tidak dikenal: %s (pakai weyl|hashed)\n", order);
        return 2;
    }

    uint64_t sm_state_key = 0xC0FFEE1234ULL ^ base_key;
    uint64_t sm_state_addr = 0xDEADBEEFCAFEULL ^ base_addr;

    uint64_t block_idx = 0;
    size_t remaining = n;
    while (remaining > 0) {
        size_t len = remaining < block_len ? remaining : block_len;
        uint32_t key, addr_hi;
        if (use_weyl) {
            key = base_key + (uint32_t)(block_idx * WEYL_DELTA_KEY);
            addr_hi = base_addr + (uint32_t)(block_idx * WEYL_DELTA_ADDR);
        } else {
            key = splitmix32_hi(&sm_state_key);
            addr_hi = splitmix32_hi(&sm_state_addr);
        }
        ra_core_orbit(key, addr_hi, len, stdout);
        remaining -= len;
        ++block_idx;
    }
    return 0;
}
