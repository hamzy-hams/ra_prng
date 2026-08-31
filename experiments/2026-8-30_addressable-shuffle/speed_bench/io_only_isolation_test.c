// io_only_isolation_test.c
//
// Diagnostic derived from ../stream_driven_shuffle.c: identical load/save
// I/O (fscanf/fprintf-based load_tokens_from_file/save_tokens_to_file,
// shared by every ZepFold-family CLI in this repo -- scrambler_addressable.c,
// ra_prng2.c, stream_driven_shuffle.c) but with the shuffle call itself
// removed, to isolate how much of the total runtime is pure text I/O vs.
// actual shuffle work. Used to test (and correct) the hypothesis that the
// ZepFold-family's ~2x slowdown vs xoshiro256**/PCG32/Philox4x32 is caused
// by ra_prng's "double array swap" (own L[] state swap + token swap) --
// see SPEED_BENCHMARK.md's "Analisis Lanjutan" section for the numbers and
// conclusion (I/O dominates: fscanf/fprintf per-token vs xoshiro256.c's
// single fread() + manual pointer tokenizer + raw fwrite()).
//
// Compile: gcc -std=c11 -O3 -D_POSIX_C_SOURCE=200809L -o io_only_isolation_test io_only_isolation_test.c
// Usage:   ./io_only_isolation_test --input data.txt --output copy.txt [--key KEY]
// (output is an unshuffled copy of input -- that's expected, shuffle is disabled)

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdalign.h>

// ---- byte-for-byte copies from winner_wired_addressable.c -----------------

#define ADDR_L_MIX_CONST 0x9e3779b7u

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

static void ra_hash(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}

static void ra_init_state_addressable(uint32_t *L, uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * ADDR_L_MIX_CONST + 0x9e3779b7u * key);
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

static void ra_permutation_cycle(uint32_t cons, size_t it,
                                  const uint32_t *M, uint32_t *L,
                                  uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = (M[(uint8_t)(i + 6)] << 6) ^ (M[(uint8_t)(i + 7)] << 7);

        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & 0xFFu;

        if (*count <= 1) break;
        --(*count);

        uint32_t tmp = L[i];
        L[i] = L[d];
        L[d] = tmp;
    }
}

static uint32_t ra_reseed(uint32_t *M, const uint32_t *L) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];

    uint32_t tmp8[8];
    ra_hash(M, tmp8);

    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

static uint32_t ra_core(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_state_addressable(L, M, key);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

// ---- reused verbatim from scrambler_addressable.c (generic token I/O) -----

static int load_tokens_from_file(const char *filename, uint32_t **out_tokens, size_t *out_len) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error: File '%s' tidak ditemukan.\n", filename);
        return -1;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error: Gagal membaca ukuran file '%s'.\n", filename);
        fclose(fp);
        return -1;
    }
    long fsize = ftell(fp);
    if (fsize < 0) {
        fprintf(stderr, "Error: Gagal membaca ukuran file '%s'.\n", filename);
        fclose(fp);
        return -1;
    }
    rewind(fp);

    char *data = (char *)malloc((size_t)fsize + 1);
    if (!data) {
        fprintf(stderr, "Error: Gagal mengalokasi memori.\n");
        fclose(fp);
        return -1;
    }
    size_t read_len = fread(data, 1, (size_t)fsize, fp);
    fclose(fp);
    data[read_len] = '\0';

    size_t capacity = 1024;
    size_t length = 0;
    uint32_t *tokens = (uint32_t *)malloc(capacity * sizeof(uint32_t));
    if (!tokens) {
        fprintf(stderr, "Error: Gagal mengalokasi memori.\n");
        free(data);
        return -1;
    }

    char *p = data;
    while (*p) {
        while (*p == ' ' || *p == '\n' || *p == '\t' || *p == '\r') p++;
        if (!*p) break;

        char *endptr;
        long value = strtol(p, &endptr, 10);
        if (endptr == p) {
            fprintf(stderr, "Error: Isi file '%s' mengandung token yang bukan bilangan bulat.\n", filename);
            free(tokens);
            free(data);
            return -1;
        }
        p = endptr;

        if (length >= capacity) {
            capacity *= 2;
            uint32_t *tmp = (uint32_t *)realloc(tokens, capacity * sizeof(uint32_t));
            if (!tmp) {
                fprintf(stderr, "Error: Gagal merealokasi memori.\n");
                free(tokens);
                free(data);
                return -1;
            }
            tokens = tmp;
        }
        tokens[length++] = (uint32_t)value;
    }

    free(data);
    *out_tokens = tokens;
    *out_len = length;
    return 0;
}

static int save_tokens_to_file(const uint32_t *tokens, size_t len, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Error: Gagal membuka file '%s' untuk menulis.\n", filename);
        return -1;
    }

    /* Upper bound: 10 digits (max uint32_t) + 1 separator per token. */
    size_t capacity = len * 11 + 1;
    char *buf = (char *)malloc(capacity);
    if (!buf) {
        fprintf(stderr, "Error: Gagal mengalokasi memori untuk menulis.\n");
        fclose(fp);
        return -1;
    }

    size_t pos = 0;
    for (size_t i = 0; i < len; ++i) {
        pos += (size_t)snprintf(buf + pos, capacity - pos, "%u", tokens[i]);
        if (i + 1 < len) buf[pos++] = ' ';
    }

    size_t written = fwrite(buf, 1, pos, fp);
    free(buf);
    fclose(fp);
    if (written != pos) {
        fprintf(stderr, "Error: Gagal menulis ke file '%s'.\n", filename);
        return -1;
    }
    return 0;
}

// ---- new: single-swap Fisher-Yates driven purely by the ra_core stream ----
//
// Same loop shape as benchmarks/comparisons/source/xoshiro256.c's shuffle:
// one rand_bounded() draw + one swap per token. The "rand_bounded" draw here
// is just an index into a buffer pre-filled by one ra_core() call (rather
// than a per-call PRNG next(), since ra_core doesn't expose an incremental
// next()-style API) -- the entropy source and Lemire-style reduction are
// otherwise identical in spirit to xoshiro256.c's.

static void stream_driven_shuffle(uint32_t key, uint32_t *tokens, size_t count) {
    if (count < 2) return;

    size_t needed = count - 1;
    uint32_t *randbuf = (uint32_t *)malloc(needed * sizeof(uint32_t));
    if (!randbuf) {
        fprintf(stderr, "Error: Gagal mengalokasi buffer stream.\n");
        exit(EXIT_FAILURE);
    }

    FILE *memstream = fmemopen(randbuf, needed * sizeof(uint32_t), "wb");
    if (!memstream) {
        fprintf(stderr, "Error: fmemopen gagal.\n");
        free(randbuf);
        exit(EXIT_FAILURE);
    }
    ra_core(key, needed, memstream);
    fclose(memstream);

    size_t ridx = 0;
    for (size_t i = count; i > 1; i--) {
        uint64_t r = randbuf[ridx++];
        uint32_t j = (uint32_t)((r * (uint64_t)i) >> 32); // Lemire reduction, bound i
        uint32_t tmp = tokens[i - 1];
        tokens[i - 1] = tokens[j];
        tokens[j] = tmp;
    }

    free(randbuf);
}

// ---- CLI --------------------------------------------------------------

typedef struct {
    char *input_file;
    char *output_file;
    uint32_t key;
} Options;

static int parse_args(int argc, char *argv[], Options *opts) {
    if (argc < 5) {
        fprintf(stderr,
                "Usage: %s --input <input_file> --output <output_file> [--key <key>]\n",
                argv[0]);
        return -1;
    }

    opts->key = 1;
    opts->input_file = NULL;
    opts->output_file = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--input") == 0 || strcmp(argv[i], "-i") == 0) {
            if (i + 1 < argc) opts->input_file = argv[++i];
            else { fprintf(stderr, "Error: --input memerlukan argumen.\n"); return -1; }
        } else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) opts->output_file = argv[++i];
            else { fprintf(stderr, "Error: --output memerlukan argumen.\n"); return -1; }
        } else if (strcmp(argv[i], "--key") == 0 || strcmp(argv[i], "-k") == 0) {
            if (i + 1 < argc) opts->key = (uint32_t)strtoul(argv[++i], NULL, 0);
            else { fprintf(stderr, "Error: --key memerlukan argumen.\n"); return -1; }
        } else {
            fprintf(stderr, "Error: Opsi tidak dikenali: %s\n", argv[i]);
            return -1;
        }
    }

    if (!opts->input_file || !opts->output_file) {
        fprintf(stderr, "Error: --input dan --output wajib disertakan.\n");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    Options opts;
    if (parse_args(argc, argv, &opts) != 0) return EXIT_FAILURE;

    clock_t t_start = clock();

    uint32_t *tokens = NULL;
    size_t token_len = 0;
    if (load_tokens_from_file(opts.input_file, &tokens, &token_len) != 0) return EXIT_FAILURE;
    if (token_len == 0) {
        if (save_tokens_to_file(tokens, token_len, opts.output_file) != 0) { free(tokens); return EXIT_FAILURE; }
        printf("Warning: File input kosong. File output '%s' dibuat kosong.\n", opts.output_file);
        free(tokens);
        return EXIT_SUCCESS;
    }

    // shuffle intentionally disabled -- see header comment

    if (save_tokens_to_file(tokens, token_len, opts.output_file) != 0) { free(tokens); return EXIT_FAILURE; }

    clock_t t_end = clock();
    double elapsed = (double)(t_end - t_start) / CLOCKS_PER_SEC;
    printf("Proses shuffle selesai. Hasil disimpan di '%s'.\n", opts.output_file);
    printf("Done in %.3f seconds\n", elapsed);

    free(tokens);
    return EXIT_SUCCESS;
}
