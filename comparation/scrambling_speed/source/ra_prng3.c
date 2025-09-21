#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdalign.h> 
static inline uint64_t rot64(uint64_t n, uint64_t r) {
    r &= 63;
    return ((n << r) | (n >> (64 - r))) ;
}

uint64_t ZepXORhash(uint64_t *M) {
    uint64_t f = 0;
    for (uint8_t i = 0; i < 8; ++i) {
        uint8_t  r   = (uint8_t)M[i];
        uint64_t tmp = M[i] ^ M[r];
        for (uint8_t j = 0; j < 32; ++j) {
            tmp ^= M[j * 8 + i];
        }
        f ^= tmp;
    }
    return f;
}

/* 
 * ZepFoldShuffle64:
 * - seed: initial seed
 * - tokens: array of uint64_t items to shuffle (modified in-place)
 * - tokens_len: number of items
 *
 * Uses internal ra_prng3-style engine (as given) and uses 'c' variable
 * to generate random indices via Lemire 64-bit reduction:
 *    idx = (c * range) >> 64
 *
 * Returns last 'cons' seed (same semantic as original ZepFold).
 */
uint64_t ZepFold(uint64_t seed, uint64_t *tokens, size_t tokens_len) {
    if (tokens_len == 0) return seed;

    // Make a working copy so we can swap safely
    uint64_t *scrambled = (uint64_t *)malloc(tokens_len * sizeof(uint64_t));
    if (!scrambled) return seed; // allocation failure: return seed (caller can handle)
    memcpy(scrambled, tokens, tokens_len * sizeof(uint64_t));

    // Initialize internal state
    alignas(64) uint64_t L[256];
    alignas(64) uint64_t M[256];

    // count = last valid index
    uint64_t count = tokens_len - 1;
    uint64_t iteration = tokens_len / 255 + 1;
    uint64_t cons = seed;
    for (int i = 0; i < 256; ++i) {
        // keep your initialization constants as in your snippet
        M[i] = (uint64_t)( (uint64_t)i * 0x06a0dd9b9e3779b7UL + 0x06a0dd9b9e3779b7UL );
        L[i] = (uint64_t)( (uint64_t)i * 0x9e3779b706a0dd9bUL + 0x9e3779b706a0dd9bUL );
    }

    for (size_t it = 0; it < iteration; ++it) {
        uint64_t a = cons;
        uint64_t b = it;
        uint64_t c = 0;
        uint64_t d = 0;
        uint8_t  e = 0;

        // Permutation / churn step (descending i)
        for (uint64_t i = 255; i > 0; --i) {
            uint64_t o = M[i];

            // follow your formulas (with clearer parenthesis)
            a = rot64((b ^ o), c) ^ (cons + a);
            b = rot64((cons + a), i) ^ (o + c);
            o = rot64((a ^ o), b) ^ (b >> 37);
            c = (o << 18) ^ (b >> 29) ^ a;
            d = rot64((L[i] ^ (b << 59)), c);

            // ---- use c to produce an index for shuffling tokens ----
            // range = count + 1 (number of valid indices: 0..count)
            // safe Lemire 64-bit: idx = (c * range) >> 64 using 128-bit mul
            uint32_t idx = (c >> 32) * (count + 1) >> 32;

            // swap scrambled[count] and scrambled[idx]
            // ensure idx <= count (it should)
            uint32_t tmp = scrambled[count];
            scrambled[count] = scrambled[idx];
            scrambled[idx] = tmp;
            // ---------------------------------------------------------

            // Existing array state swapping (keep as in your design)
            // e computed using similar reduction as original (maps to small index)
            // We'll reuse a 32-bit style reduction for selecting 0..255 index
            e = ((c >> 32) * (i + 1) >> 32);

            // swap in L-lists
            L[i] = L[e];
            L[e] = d;

            if (count <= 1) {
                break;
            }
            --count;
        } // end inner i-loop

        if (count <= 1) {
            // copy scrambled back and cleanup
            memcpy(tokens, scrambled, tokens_len * sizeof(uint64_t));
            free(scrambled);
            return cons;
        }

        // Mixing step: M ^= L
        for (uint16_t i = 0; i < 256; ++i) {
            M[i] ^= L[i];
        }

        // Hash to next cons
        cons = ZepXORhash(M);
    } // end iteration

    // copy scrambled back to caller array
    memcpy(tokens, scrambled, tokens_len * sizeof(uint64_t));
    free(scrambled);
    return cons;
}


/*
 * load_tokens_from_file:
 *   - filename: nama file teks berisi integer dipisah spasi/newline
 *   - *out_tokens: pointer yang akan diisi alamat buffer hasil alokasi
 *   - *out_len: panjang (jumlah elemen) token yang dibaca
 *
 * Mengembalikan 0 jika sukses, atau -1 jika terjadi error (file tidak ada atau format salah).
 * Buffer tokens harus dibebaskan oleh pemanggil (free).
 */
static int load_tokens_from_file(const char *filename, uint64_t **out_tokens, size_t *out_len) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: File '%s' tidak ditemukan.\n", filename);
        return -1;
    }

    /* Baca integer satu per satu menggunakan fscanf */
    size_t capacity = 1024;
    size_t length = 0;
    uint64_t *tokens = (uint64_t *)malloc(capacity * sizeof(uint64_t));
    if (!tokens) {
        fprintf(stderr, "Error: Gagal mengalokasi memori.\n");
        fclose(fp);
        return -1;
    }

    while (1) {
        long value;
        int ret = fscanf(fp, "%ld", &value);
        if (ret == EOF) {
            break;
        }
        if (ret != 1) {
            fprintf(stderr, "Error: Isi file '%s' mengandung token yang bukan bilangan bulat.\n", filename);
            free(tokens);
            fclose(fp);
            return -1;
        }
        if (length >= capacity) {
            capacity *= 2;
            uint64_t *tmp = (uint64_t *)realloc(tokens, capacity * sizeof(uint64_t));
            if (!tmp) {
                fprintf(stderr, "Error: Gagal merealokasi memori.\n");
                free(tokens);
                fclose(fp);
                return -1;
            }
            tokens = tmp;
        }
        tokens[length++] = (uint64_t)value;
    }

    fclose(fp);
    *out_tokens = tokens;
    *out_len = length;
    return 0;
}

/*
 * save_tokens_to_file:
 *   - tokens: array uint64_t
 *   - len: jumlah elemen dalam tokens
 *   - filename: nama file tujuan untuk menyimpan token dipisah spasi
 *
 * Mengembalikan 0 jika sukses, atau -1 jika gagal menulis file.
 */
static int save_tokens_to_file(const uint64_t *tokens, size_t len, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Gagal membuka file '%s' untuk menulis.\n", filename);
        return -1;
    }

    for (size_t i = 0; i < len; ++i) {
        if (fprintf(fp, "%u", tokens[i]) < 0) {
            fprintf(stderr, "Error: Gagal menulis ke file '%s'.\n", filename);
            fclose(fp);
            return -1;
        }
        if (i + 1 < len) {
            fputc(' ', fp);
        }
    }

    fclose(fp);
    return 0;
}

/* Struktur untuk menampung opsi dari command line */
typedef struct {
    char *input_file;
    char *output_file;
    uint64_t seed;
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
                "Usage: %s --input <input_file> --output <output_file> [--seed <seed>] [--multiplier_m <mm>] [--multiplier_l <ml>]\n",
                argv[0]);
        return -1;
    }

    /* Set default */
    opts->seed = 736456;
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
        } else if (strcmp(argv[i], "--seed") == 0 || strcmp(argv[i], "-s") == 0) {
            if (i + 1 < argc) {
                opts->seed = (uint64_t)strtoul(argv[++i], NULL, 10);
            } else {
                fprintf(stderr, "Error: --seed memerlukan argumen.\n");
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
    uint64_t *tokens = NULL;
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

    /* 2. Shuffle menggunakan ZepFold */
    ZepFold(opts.seed, tokens, token_len);

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
