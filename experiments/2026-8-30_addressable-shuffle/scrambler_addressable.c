// Copyright (c) 2025 Hamas A. Rahman
// Licensed under CC BY-NC-SA 4.0
// github.com/hamzy-hams
//
// scrambler_addressable.c
//
// Derived from experiments/2025-10-5_scramble-design/scrambler.c (the
// "ZepFold" token-shuffling CLI): same shuffle loop (Fisher-Yates-style swap
// driven by an a/b/o/c/d recurrence, Lemire's fast reduction for the swap
// index, block-reseed via ra_hash every 255 elements). The ONLY change is
// how the initial (L, M) state tables are built: instead of the old fixed
// formula parameterized by tunable --multiplier_m/--multiplier_l constants
// (seed-independent, never collision-tested), this uses
// ra_init_state_addressable(L, M, key), copied byte-for-byte from
// ../2026-8-30_addressable-init-research/winner_wired_addressable.c (Tahap
// 1-5: "Orbit Addressing" -- 0 collisions across 500k random keys, healthy
// avalanche, PractRand-clean to 128GB). The per-address parameter is named
// `key` (Tahap 1 naming), not `seed`, matching that research's convention.
//
// See ../2026-8-30_addressable-init-research/RESULTS.md for the addressable
// init's own validation; see RESULTS.md in this folder for this shuffle
// CLI's parity/statistical validation.
//
// Usage:
//   gcc -std=c11 -O3 -o scrambler_addressable scrambler_addressable.c
//   ./scrambler_addressable --input data.txt --output shuffled.txt [--key KEY]
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
 * ra_hash:
 *   - N: a 256-element uint32_t array as the input state (will be modified)
 *   - out8: an 8-element uint32_t output array (32-bit values; only the 8 LSBs are used after combining into new_cons)
 */

static void ra_hash(uint32_t N[256], uint32_t out8[8]) {
    /* Inisialisasi out8 ke 0 */
    for (uint8_t i = 0; i < 8; ++i) {
        out8[i] = 0;
    }
    /* Proses mixing */
    for (uint8_t i = 0; i < 8; ++i) {
        out8[i] ^= N[(uint8_t)N[i]];
        for (uint8_t j = 0; j < 32; ++j) {
            N[i] ^= N[j * 8 + i];
        }
    }
}

/*
 * ra_init_state_addressable:
 *   Byte-for-byte copy of winner_wired_addressable.c's function of the same
 *   name (Tahap 1 frozen formula, Kandidat 5, address = key only). Builds
 *   the initial (L, M) state as a function of `key` instead of a fixed
 *   formula -- this is what makes per-key/per-stream shuffling collision-safe
 *   (validated in ../2026-8-30_addressable-init-research/, Tahap 0-3).
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
 *   - key: per-address selector (Tahap 1 naming; was `seed` in scrambler.c)
 *   - tokens: a uint32_t array containing the tokens to be shuffled
 *   - tokens_length: the number of elements in tokens
 *
 * This function modifies the tokens array in-place, so the token order
 * becomes shuffled. Shuffle loop itself is unchanged from scrambler.c --
 * only the (L, M) initialization (below) differs.
 *
 * Named `ra_shuffle` (not `ra_core`) to avoid colliding, in name only, with
 * the unrelated stream-generator `ra_core` in stream_driven_shuffle.c /
 * winner_wired_addressable.c -- this function does full array-permutation
 * shuffling, not raw stream generation.
 */

static void ra_shuffle(uint32_t key,
                    uint32_t *tokens,
                    size_t tokens_length) {

    if (tokens_length == 0) {
        return;
    }

    /* Salin token ke scrambled_tokens */
    uint32_t *scrambled_tokens = (uint32_t *)malloc(tokens_length * sizeof(uint32_t));
    if (!scrambled_tokens) {
        fprintf(stderr, "Error: Gagal mengalokasi memori untuk scramble.\n");
        exit(EXIT_FAILURE);
    }
    memcpy(scrambled_tokens, tokens, tokens_length * sizeof(uint32_t));

    /* Hitung jumlah iterasi (dan count sebagai indeks akhir) */
    size_t iteration = tokens_length / 255 + 1;
    int64_t count = (int64_t)tokens_length - 1;

    /* Tabel state L dan M, serta array tmp8 untuk ra_hash */
    alignas(64) uint32_t L[256];
    alignas(64) uint32_t M[256];
    uint32_t tmp8[8];

    /* Inisialisasi konstanta dan tabel L, M via addressable init */
    uint32_t cons = key;
    ra_init_state_addressable(L, M, key);

    /* Loop utama: tiap 'round' akan men-shuffle satu blok 255 token */
    for (size_t it = 0; it < iteration; ++it) {
        uint32_t a = cons;
        uint32_t b = (uint32_t)it;
        uint32_t c = 0;
        uint32_t d = 0;

        for (int i = 255; i >= 1; --i) {
            /* Hitung o = XOR mixing dari M */
            uint32_t o = 0;
            for (uint8_t e = 0; e < 8; ++e) {
                /* (i+e) mod 256 → dengan & 0xFF karena i < 256 */
                o ^= (M[(uint8_t)(i + e)] << e) & 0xFFFFFFFF;
            }

            /* Update a, b, o, c, d*/
            a = (rot32(b ^ o, d) ^ cons + a) & 0xFFFFFFFF;
            b = (rot32(cons + a, i) ^ o + d) & 0xFFFFFFFF;
            o = (rot32(a ^ o, i) << 9) ^ (b >> 18) & 0xFFFFFFFF;
            c = rot32(((o + c) << 14) ^ (b >> 13) ^ a, b) & 0xFFFFFFFF;
            /* Penghitungan idx menggunakan Lemire's fast reduction: (c * (count+1)) >> 32 */
            uint32_t idx = (uint32_t)((uint64_t)c * (count + 1) >> 32);

            /* Perbarui d dengan Langkah berikutnya */
            d = (uint32_t)((uint64_t)c * (i + 1) >> 32);

            /* Swap pada scrambled_tokens dan pada tabel L */
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

        /* XOR mixing antara M dan L */
        for (uint32_t i = 0; i < 256; ++i) {
            M[i] ^= L[i];
        }

        /* Hitung ra_hash dari M ke tmp8 */
        ra_hash(M, tmp8);

        /* Gabungkan tmp8 menjadi new cons */
        uint32_t new_cons = 0;
        for (uint8_t e = 0; e < 8; ++e) {
            new_cons ^= (tmp8[e] << e) & 0xFFFFFFFF;
        }
        cons = new_cons;
    }

    /* Salin hasil scrambled_tokens kembali ke tokens asli */
    memcpy(tokens, scrambled_tokens, tokens_length * sizeof(uint32_t));
    free(scrambled_tokens);
}

/*
 * load_tokens_from_file:
 *   - filename: the name of the text file containing integers separated by spaces/newlines
 *   - *out_tokens: pointer to be set to the address of the allocated buffer
 *   - *out_len: the length (number of elements) of the tokens read
 *
 * Returns 0 on success, or -1 on error (file not found or invalid format).
 * The tokens buffer must be freed by the caller (free).
 *
 * Reads the whole file in one fread() (pattern from
 * benchmarks/comparisons/source/xoshiro256.c), then walks a pointer through
 * the in-memory buffer parsing each token with strtol -- avoids the
 * per-token fscanf() call overhead that dominated runtime (see
 * SPEED_BENCHMARK.md "Analisis Lanjutan"). Tokens are still stored as
 * uint32_t (not left as raw substrings) so downstream shuffle/save code and
 * output formatting stay byte-identical to before.
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

/*
 * save_tokens_to_file:
 *   - tokens: a uint32_t array
 *   - len: number of elements in tokens
 *   - filename: the target filename to save the space-separated tokens
 *
 * Returns 0 on success, or -1 if writing the file fails.
 *
 * Formats the whole output into one in-memory buffer, then issues a single
 * fwrite() (pattern from xoshiro256.c) instead of one fprintf() per token.
 * Output format (decimal, single-space separated, no trailing separator)
 * is unchanged from the previous fprintf-based version.
 */

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

/* Struktur untuk menampung opsi dari command line */
typedef struct {
    char *input_file;
    char *output_file;
    uint32_t key;
} Options;

/*
 * parse_args:
 *   - argc, argv: argumen command line
 *   - opts: pointer ke struktur Options yang akan diisi
 *
 * Mengembalikan 0 jika parsing berhasil, atau -1 jika ada kesalahan.
 */
static int parse_args(int argc, char *argv[], Options *opts) {
    if (argc < 5) {
        fprintf(stderr,
                "Usage: %s --input <input_file> --output <output_file> [--key <key>]\n",
                argv[0]);
        return -1;
    }

    /* Set default */
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

    /* Mulai timer */
    clock_t t_start = clock();

    /* 1. Load tokens dari file */
    uint32_t *tokens = NULL;
    size_t token_len = 0;
    if (load_tokens_from_file(opts.input_file, &tokens, &token_len) != 0) {
        return EXIT_FAILURE;
    }
    if (token_len == 0) {
        /* Jika file kosong, buat file output kosong juga */
        if (save_tokens_to_file(tokens, token_len, opts.output_file) != 0) {
            free(tokens);
            return EXIT_FAILURE;
        }
        printf("Warning: File input kosong. File output '%s' dibuat kosong.\n", opts.output_file);
        free(tokens);
        return EXIT_SUCCESS;
    }

    /* 2. Shuffle menggunakan ra_shuffle (addressable init) */
    ra_shuffle(opts.key, tokens, token_len);

    /* 3. Simpan hasil shuffle ke file output */
    if (save_tokens_to_file(tokens, token_len, opts.output_file) != 0) {
        free(tokens);
        return EXIT_FAILURE;
    }

    /* Cetak waktu eksekusi */
    clock_t t_end = clock();
    double elapsed = (double)(t_end - t_start) / CLOCKS_PER_SEC;
    printf("Proses shuffle selesai. Hasil disimpan di '%s'.\n", opts.output_file);
    printf("Done in %.3f seconds\n", elapsed);

    free(tokens);
    return EXIT_SUCCESS;
}
