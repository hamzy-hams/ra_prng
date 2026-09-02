// multikey_stream_k.c
// Karakterisasi ambang K minimum yang aman untuk ra_core_singleblock.
// Sama persis dengan ../2026-9-1_family-productionization/multikey_stream.c
// kecuali block_len (K) sekarang parameter CLI, bukan hardcode 255 --
// supaya bisa sweep K=1..255 tanpa menulis satu file per titik K.
// Key derivation (Weyl/golden-ratio increment 0x9E3779B9) identik.
//
// Compile:
//   gcc -O3 -march=native -std=gnu17 -include stdalign.h \
//       -Dmain=ra_core_unused_main -c ../2026-9-1_family-productionization/ra_core.c \
//       -o ra_core_nomain.o -Wall -Wextra
//   gcc -O3 -march=native -std=gnu17 multikey_stream_k.c ra_core_nomain.o \
//       -o multikey_stream_k -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern uint32_t ra_core_singleblock(uint32_t key, size_t rng, FILE *raw_stream);

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

    uint64_t block_idx = 0;
    size_t remaining = n;
    while (remaining > 0) {
        size_t block_len = remaining < K ? remaining : K;
        uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
        ra_core_singleblock(key, block_len, stdout);
        remaining -= block_len;
        ++block_idx;
    }
    return 0;
}
