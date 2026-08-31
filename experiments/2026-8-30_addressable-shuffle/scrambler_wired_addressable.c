// Copyright (c) 2025 Hamas A. Rahman
// Licensed under CC BY-NC-SA 4.0
// github.com/hamzy-hams
//
// scrambler_wired_addressable.c
//
// Follow-up to scrambler_addressable.c, closing a gap flagged by the user
// (2026-08-31): scrambler_addressable.c deliberately kept the ORIGINAL
// (unpruned) a/b/o/c/d recurrence unchanged from scrambler.c -- it only
// swapped in ra_init_state_addressable(key) for the (L, M) init. Nobody had
// yet built the combination that's actually most relevant for a real
// shuffling application: addressable-init + the pruned/rewired
// winner_wired_v2 recurrence (the validated fastest/best candidate from
// ../2026-8-27_operand-position-search/, ~6% fewer instructions than
// winner_wired.c) in the SAME double-swap ZepFold shuffle-loop shape as
// scrambler_addressable.c (stream_driven_shuffle.c in this folder DOES use
// the winner_wired_v2 recurrence, but pairs it with a different,
// single-swap Fisher-Yates loop shape built to test an unrelated
// "double-swap" hypothesis -- see HANDOVER.md).
//
// Derivation -- this file changes exactly ONE thing relative to
// scrambler_addressable.c: the a/b/o/c/d computation inside the 255-step
// loop. Everything else (ra_init_state_addressable, ra_hash, block-reseed
// every 255 elements, Lemire's fast reduction for the token-array swap
// index, the double swap of scrambled_tokens[]/L[], I/O via
// fread()+strtol()/fwrite()) is copied verbatim from scrambler_addressable.c.
//
// The new a/b/o/c/(d) block is byte-for-byte copied from
// ../2026-8-30_addressable-init-research/winner_wired_addressable.c's
// ra_permutation_cycle() (lines ~70-79), which is itself byte-for-byte from
// winner_wired_v2.c:
//   o = (M[(i+6)&0xFF] << 6) ^ (M[(i+7)&0xFF] << 7)
//   a = (d ^ o) ^ (cons + a)
//   b = (cons + a) ^ (o + d)
//   c = rot32((a >> 13) ^ a, b)
//   d = c & 0xFF
// Note this `d` (used for both the next iteration's `a` and this
// implementation's L[i]/L[d] state-table swap) replaces
// scrambler_addressable.c's `d = (c * (i+1)) >> 32` -- winner_wired_v2's own
// `d` update, not a value invented for this file. The Lemire reduction for
// the token-array swap index (`idx = (c * (count+1)) >> 32`) is unchanged
// application-layer logic, not part of the PRNG cycle itself.
//
// Usage:
//   gcc -std=c11 -O3 -o scrambler_wired_addressable scrambler_wired_addressable.c
//   ./scrambler_wired_addressable --input data.txt --output shuffled.txt [--key KEY]
//
// If --key is not provided, the default key = 1.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdalign.h>

// Tahap 1 frozen constant (see winner_wired_addressable.c).
#define ADDR_L_MIX_CONST 0x9e3779b7u

/* Rotate-left 32-bit */
static inline uint32_t rot32(uint32_t x, uint32_t r) {
    r &= 31;
    return (uint32_t)((x << r) | (x >> (32 - r)));
}

/*
 * ra_hash: byte-for-byte copy of scrambler_addressable.c's ra_hash().
 */
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

/*
 * ra_init_state_addressable: byte-for-byte copy of
 * ../2026-8-30_addressable-init-research/winner_wired_addressable.c's
 * function of the same name (also already used, unchanged, by
 * scrambler_addressable.c in this folder).
 */
static void ra_init_state_addressable(uint32_t *L, uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * ADDR_L_MIX_CONST + 0x9e3779b7u * key);
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

/*
 * ra_shuffle:
 *   Same double-swap ZepFold shuffle loop as scrambler_addressable.c's
 *   ra_shuffle() -- the ONLY change is the a/b/o/c/d recurrence body, now
 *   winner_wired_v2's pruned+wired formula instead of the original one.
 */
static void ra_shuffle(uint32_t key,
                    uint32_t *tokens,
                    size_t tokens_length) {

    if (tokens_length == 0) {
        return;
    }

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
            /* winner_wired_v2's wiring: o built from M[i+6]/M[i+7] only
             * (not an 8-wide XOR mix), a/b/c updated per the pruned+wired
             * formula, d = c & 0xFF (not the original's (c*(i+1))>>32). */
            uint32_t o = (M[(uint8_t)(i + 6)] << 6) ^ (M[(uint8_t)(i + 7)] << 7);

            a = (d ^ o) ^ (cons + a);
            b = (cons + a) ^ (o + d);
            c = rot32((a >> 13) ^ a, b);

            /* Lemire's fast reduction for the token-array swap index --
             * application-layer logic, unchanged from scrambler_addressable.c. */
            uint32_t idx = (uint32_t)((uint64_t)c * (count + 1) >> 32);

            /* winner_wired_v2's own d update, reused for the L[] state swap. */
            d = c & 0xFFu;

            uint32_t tmp_tok = scrambled_tokens[count];
            scrambled_tokens[count] = scrambled_tokens[idx];
            scrambled_tokens[idx] = tmp_tok;

            uint32_t tmp_l = L[i];
            L[i] = L[d];
            L[d] = tmp_l;

            if (count <= 1) {
                break;
            }
            --count;
        }

        if (count <= 1) {
            break;
        }

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

/*
 * load_tokens_from_file / save_tokens_to_file: byte-for-byte copies of
 * scrambler_addressable.c's I/O functions (fread()+strtol() /
 * single fwrite(), post-I/O-optimization pattern).
 */

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
        if (i + 1 < len) {
            buf[pos++] = ' ';
        }
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
            if (i + 1 < argc) {
                opts->input_file = argv[++i];
            } else {
                fprintf(stderr, "Error: --input memerlukan argumen.\n");
                return -1;
            }
        } else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                opts->output_file = argv[++i];
            } else {
                fprintf(stderr, "Error: --output memerlukan argumen.\n");
                return -1;
            }
        } else if (strcmp(argv[i], "--key") == 0 || strcmp(argv[i], "-k") == 0) {
            if (i + 1 < argc) {
                opts->key = (uint32_t)strtoul(argv[++i], NULL, 0);
            } else {
                fprintf(stderr, "Error: --key memerlukan argumen.\n");
                return -1;
            }
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
    if (parse_args(argc, argv, &opts) != 0) {
        return EXIT_FAILURE;
    }

    clock_t t_start = clock();

    uint32_t *tokens = NULL;
    size_t token_len = 0;
    if (load_tokens_from_file(opts.input_file, &tokens, &token_len) != 0) {
        return EXIT_FAILURE;
    }
    if (token_len == 0) {
        if (save_tokens_to_file(tokens, token_len, opts.output_file) != 0) {
            free(tokens);
            return EXIT_FAILURE;
        }
        printf("Warning: File input kosong. File output '%s' dibuat kosong.\n", opts.output_file);
        free(tokens);
        return EXIT_SUCCESS;
    }

    ra_shuffle(opts.key, tokens, token_len);

    if (save_tokens_to_file(tokens, token_len, opts.output_file) != 0) {
        free(tokens);
        return EXIT_FAILURE;
    }

    clock_t t_end = clock();
    double elapsed = (double)(t_end - t_start) / CLOCKS_PER_SEC;
    printf("Proses shuffle selesai. Hasil disimpan di '%s'.\n", opts.output_file);
    printf("Done in %.3f seconds\n", elapsed);

    free(tokens);
    return EXIT_SUCCESS;
}
