// single_mread_isolation_test.c
//
// Checkpoint follow-up (2026-08-31): diagnostic derived from
// ../scrambler_addressable.c to isolate the cost of the per-token 8-wide
// `M[]` read (`for e in 0..8: o ^= (M[(i+e)&0xFF]<<e)`, executed once per
// inner-loop iteration i.e. ~once per token) from the rest of the shuffle
// loop's cost -- replaced here with a single M[] read. SPEED_BENCHMARK.md's
// "Kesimpulan" flagged this as one of three untested suspects for the
// residual ~1.62x gap vs xoshiro/pcg after the I/O fix (the other two:
// block-end reseed frequency, tested by no_reseed_isolation_test.c;
// double-swap array, already disproven by stream_driven_shuffle.c pre- and
// post-I/O-fix).
//
// NOT a correctness-preserving variant -- `o` only mixes in 1/8 of the M[]
// entries a real block would use, so output is NOT statistically valid
// shuffle output (same diagnostic-only category as
// stream_driven_shuffle.c/no_reseed_isolation_test.c). Loop trip counts and
// all other arithmetic are otherwise byte-for-byte identical to
// scrambler_addressable.c's ra_shuffle().
//
// Compile: gcc -std=c11 -O3 -o single_mread_isolation_test single_mread_isolation_test.c
// Usage:   ./single_mread_isolation_test --input data.txt --output shuffled.txt [--key KEY]

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdalign.h>

#define ADDR_L_MIX_CONST 0x9e3779b7u

static inline uint32_t rot32(uint32_t x, uint32_t r) {
    r &= 31;
    return (uint32_t)((x << r) | (x >> (32 - r)));
}

static void ra_hash(uint32_t N[256], uint32_t out8[8]) {
    for (uint8_t i = 0; i < 8; ++i) {
        out8[i] = 0;
    }
    for (uint8_t i = 0; i < 8; ++i) {
        out8[i] ^= N[(uint8_t)N[i]];
        for (uint8_t j = 0; j < 32; ++j) {
            N[i] ^= N[j * 8 + i];
        }
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

// ra_shuffle with the 8-wide `o` mix (per inner-loop iteration) replaced by
// a SINGLE M[] read -- everything else byte-for-byte identical to
// scrambler_addressable.c, including the block-end reseed (kept intact so
// this isolates ONLY the M-read width, not reseed frequency).
static void ra_shuffle(uint32_t key, uint32_t *tokens, size_t tokens_length) {
    if (tokens_length == 0) return;

    uint32_t *scrambled_tokens = (uint32_t *)malloc(tokens_length * sizeof(uint32_t));
    if (!scrambled_tokens) {
        fprintf(stderr, "Error: Gagal mengalokasi memori untuk scramble.\n");
        exit(EXIT_FAILURE);
    }
    memcpy(scrambled_tokens, tokens, tokens_length * sizeof(uint32_t));

    size_t iteration = tokens_length / 255 + 1;
    int64_t count = (int64_t)tokens_length - 1;

    alignas(64) uint32_t L[256];
    alignas(64) uint32_t M[256];
    uint32_t tmp8[8];

    uint32_t cons = key;
    ra_init_state_addressable(L, M, key);

    for (size_t it = 0; it < iteration; ++it) {
        uint32_t a = cons;
        uint32_t b = (uint32_t)it;
        uint32_t c = 0;
        uint32_t d = 0;

        for (int i = 255; i >= 1; --i) {
            // --- single M[] read (diagnostic isolation), was 8-wide ---
            uint32_t o = M[(uint8_t)i];

            a = (rot32(b ^ o, d) ^ cons + a) & 0xFFFFFFFF;
            b = (rot32(cons + a, i) ^ o + d) & 0xFFFFFFFF;
            o = (rot32(a ^ o, i) << 9) ^ (b >> 18) & 0xFFFFFFFF;
            c = rot32(((o + c) << 14) ^ (b >> 13) ^ a, b) & 0xFFFFFFFF;
            uint32_t idx = (uint32_t)((uint64_t)c * (count + 1) >> 32);

            d = (uint32_t)((uint64_t)c * (i + 1) >> 32);

            uint32_t tmp_tok = scrambled_tokens[count];
            scrambled_tokens[count] = scrambled_tokens[idx];
            scrambled_tokens[idx] = tmp_tok;

            uint32_t tmp_l = L[i];
            L[i] = L[d];
            L[d] = tmp_l;

            if (count <= 1) break;
            --count;
        }

        if (count <= 1) break;

        for (uint32_t i = 0; i < 256; ++i) {
            M[i] ^= L[i];
        }

        ra_hash(M, tmp8);

        uint32_t new_cons = 0;
        for (uint8_t e = 0; e < 8; ++e) {
            new_cons ^= (tmp8[e] << e) & 0xFFFFFFFF;
        }
        cons = new_cons;
    }

    memcpy(tokens, scrambled_tokens, tokens_length * sizeof(uint32_t));
    free(scrambled_tokens);
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

    ra_shuffle(opts.key, tokens, token_len);

    if (save_tokens_to_file(tokens, token_len, opts.output_file) != 0) { free(tokens); return EXIT_FAILURE; }

    clock_t t_end = clock();
    double elapsed = (double)(t_end - t_start) / CLOCKS_PER_SEC;
    printf("Proses shuffle selesai. Hasil disimpan di '%s'.\n", opts.output_file);
    printf("Done in %.3f seconds\n", elapsed);

    free(tokens);
    return EXIT_SUCCESS;
}
