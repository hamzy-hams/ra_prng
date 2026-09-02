// multikey_stream.c
// Tahap 2 (family-productionization): production-representative stream for
// ra_core_singleblock's ACTUAL use case -- aggressive reinit, many
// independent keys/addresses, each producing a short (<=255-word) block --
// not one long continuous stream from a single key (which is what
// ra_core_orbit is for, and which singleblock cannot even do: a single
// ra_core_singleblock call hard-aborts above rng=255).
//
// Standard streaming test tools (dieharder via stdin_input_raw, PractRand
// via stdin) need a long continuous byte stream and don't know or care
// about internal call boundaries. This program chains many
// ra_core_singleblock calls, each with a distinct key, into one such
// stream -- the same shape a real "call-and-discard" workload actually
// produces -- so those tools can evaluate the mode the way it's really
// used, instead of the (too-short-to-test) output of one isolated call.
//
// Does not modify or duplicate ra_core.c's formula: links against
// ra_core.c directly (built as an object file with -Dmain=<other name> to
// avoid a duplicate main() symbol, since this file provides its own).
//
// --stream <base_key> <n>: emits `n` raw uint32 words total, in blocks of
// up to 255 words, one ra_core_singleblock call per block. Each block's
// key is derived from base_key and the block index via a Weyl/golden-ratio
// increment (0x9E3779B9, a fixed odd multiplicative constant -- guarantees
// a distinct key per block index for every block up to 2^32), mirroring
// the key-derivation pattern tahap6_bench.c's reinit-sweep benchmark mode
// already used (`key = (uint32_t)(c * 2654435761u + 1u)`).
//
// Compile:
//   gcc -O3 -march=native -std=gnu17 -include stdalign.h \
//       -Dmain=ra_core_unused_main -c ra_core.c -o ra_core_nomain.o -Wall -Wextra
//   gcc -O3 -march=native -std=gnu17 multikey_stream.c ra_core_nomain.o \
//       -o multikey_stream -Wall -Wextra

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

    uint64_t block_idx = 0;
    size_t remaining = n;
    while (remaining > 0) {
        size_t block_len = remaining < 255 ? remaining : 255;
        uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
        ra_core_singleblock(key, block_len, stdout);
        remaining -= block_len;
        ++block_idx;
    }
    return 0;
}
