// scrambler_ra_core_singleblock.c
// production-candidate-battery, Step 7: shuffle-implementation test for
// `ra_core_singleblock` -- no direct precedent in the repo (the existing
// shuffle research, ../2026-8-30_addressable-shuffle/scrambler_addressable.c,
// tests a derivative of `ra_core_orbit`'s formula/reseed loop, not
// singleblock's no-L[], <=255-word, aggressive-reinit contract).
//
// Fisher-Yates shuffle of a fixed N-element array (values 0..N-1, N<=255),
// repeated `reps` times with independent keys (Weyl/golden-ratio increment
// 0x9E3779B9, same convention as multikey_stream.c), swap-index source is
// ra_core_singleblock -- two modes matching this battery's K=1/K=255 axis
// naming (words generated per singleblock call before the next
// reinit/key):
//
//   --mode k255: ONE ra_core_singleblock(key_r, N, ...) call per
//                repetition supplies all N swap-index words at once --
//                the "natural" full-block singleblock use.
//   --mode k1:   ra_core_singleblock(step_key, 1, ...) is called fresh
//                for EVERY swap step (N-1 reinits per repetition) -- the
//                most aggressive reinit pattern singleblock supports.
//
// All `reps` shuffled arrays are concatenated and written space-separated
// to stdout (or --output <file>), matching scc_test.py's read_tokens()
// format -- large enough (reps*N values) for entropy/chi-square/runs/
// serial-correlation statistics, unlike a single 255-element permutation.
//
// Does not modify or duplicate ra_core.c's formula: links against it as an
// object file (extern declaration only). Compile: see BUILD.sh.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern uint32_t ra_core_singleblock(uint32_t key, size_t rng, FILE *raw_stream);

static uint32_t pull_one(uint32_t key) {
    uint32_t word;
    FILE *f = fmemopen(&word, sizeof(word), "wb");
    ra_core_singleblock(key, 1, f);
    fclose(f);
    return word;
}

static void pull_n(uint32_t key, size_t n, uint32_t *out) {
    FILE *f = fmemopen(out, n * sizeof(uint32_t), "wb");
    ra_core_singleblock(key, n, f);
    fclose(f);
}

// One Fisher-Yates pass, mode k1: a fresh ra_core_singleblock(key,1,...)
// call sources each swap step's word.
static void fisher_yates_k1(uint32_t *arr, size_t n, uint32_t base_step_key) {
    uint32_t step = 0;
    for (size_t i = n - 1; i >= 1; --i) {
        uint32_t step_key = base_step_key + (uint32_t)(step * 0x9E3779B9u);
        uint32_t w = pull_one(step_key);
        uint32_t idx = (uint32_t)(((uint64_t)w * (i + 1)) >> 32);
        uint32_t tmp = arr[i];
        arr[i] = arr[idx];
        arr[idx] = tmp;
        ++step;
        if (i == 0) break;
    }
}

int main(int argc, char **argv) {
    const char *mode = NULL;
    uint32_t base_key = 1;
    size_t n = 255;
    size_t reps = 2000;
    const char *output = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--mode") == 0 && i + 1 < argc) mode = argv[++i];
        else if (strcmp(argv[i], "--key") == 0 && i + 1 < argc) base_key = (uint32_t)strtoul(argv[++i], NULL, 0);
        else if (strcmp(argv[i], "--n") == 0 && i + 1 < argc) n = (size_t)strtoull(argv[++i], NULL, 0);
        else if (strcmp(argv[i], "--reps") == 0 && i + 1 < argc) reps = (size_t)strtoull(argv[++i], NULL, 0);
        else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) output = argv[++i];
        else {
            fprintf(stderr, "unknown arg: %s\n", argv[i]);
            return 1;
        }
    }
    if (!mode || (strcmp(mode, "k255") != 0 && strcmp(mode, "k1") != 0)) {
        fprintf(stderr,
            "usage: %s --mode <k255|k1> [--key K] [--n N<=255] [--reps R] [--output FILE]\n",
            argv[0]);
        return 1;
    }
    if (n == 0 || n > 255) {
        fprintf(stderr, "--n must be in [1,255] (ra_core_singleblock hard cap)\n");
        return 1;
    }

    FILE *out = output ? fopen(output, "w") : stdout;
    if (!out) {
        fprintf(stderr, "cannot open output file: %s\n", output);
        return 1;
    }

    uint32_t *arr = malloc(n * sizeof(uint32_t));
    uint32_t *words = malloc((n > 1 ? n - 1 : 1) * sizeof(uint32_t));
    if (!arr || !words) {
        fprintf(stderr, "allocation failure\n");
        return 1;
    }

    for (size_t r = 0; r < reps; ++r) {
        for (size_t i = 0; i < n; ++i) arr[i] = (uint32_t)i;
        uint32_t key_r = base_key + (uint32_t)((uint64_t)r * 0x9E3779B9u);

        if (strcmp(mode, "k255") == 0) {
            if (n > 1) pull_n(key_r, n - 1, words);
            for (size_t i = n - 1; i >= 1; --i) {
                uint32_t w = words[n - 1 - i];
                uint32_t idx = (uint32_t)(((uint64_t)w * (i + 1)) >> 32);
                uint32_t tmp = arr[i];
                arr[i] = arr[idx];
                arr[idx] = tmp;
                if (i == 0) break;
            }
        } else {
            fisher_yates_k1(arr, n, key_r);
        }

        for (size_t i = 0; i < n; ++i) {
            fprintf(out, "%u", arr[i]);
            if (r + 1 < reps || i + 1 < n) fputc(' ', out);
        }
    }

    if (output) fclose(out);
    free(arr);
    free(words);
    return 0;
}
