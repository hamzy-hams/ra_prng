/*
 * zep_shuffle.c
 *
 * Fitur:
 *  - Membaca token (integer) dari file teks (dipisah spasi/newline)
 *  - Mengacak urutan token menggunakan ZepFold
 *  - Menulis kembali token yang sudah diacak ke file teks (dipisah spasi)
 *
 * Penggunaan:
 *   gcc -std=c11 -O3 -o zep_shuffle zep_shuffle.c
 *   ./zep_shuffle --input data.txt --output shuffled.txt [--seed SEED] [--multiplier_m MM] [--multiplier_l ML]
 *
 * Jika opsi --seed tidak diberikan, maka seed default = 1.
 * Jika --multiplier_m tidak diberikan, maka default = 0x06a0dd9b.
 * Jika --multiplier_l tidak diberikan, maka default = 0x9e3779b7.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

/* Rotate-left 32-bit */
static inline uint32_t rot32(uint32_t x, uint32_t r) {
    r &= 31;
    return (uint32_t)((x << r) | (x >> (32 - r)));
}

/*
 * ZepXORhash:
 *   - N: array 256 elemen uint32_t sebagai state input (akan dimodifikasi)
 *   - out8: output 8 elemen uint32_t (nilai 32-bit; hanya 8 bit LSB yang digunakan setelah dikombinasi ke new_cons)
 */
static void ZepXORhash(uint32_t N[256], uint32_t out8[8]) {
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
 * ZepFold:
 *   - seed: initial seed (uint32_t)
 *   - tokens: array uint32_t yang berisi token yang akan di-shuffle
 *   - tokens_length: jumlah elemen dalam tokens
 *   - multiplier_m, multiplier_l: konstanta untuk inisialisasi tabel M dan L
 *
 * Fungsi ini memodifikasi array tokens secara in-place, sehingga urutan token menjadi teracak.
 */
static void ZepFold(uint32_t seed,
                    uint32_t *tokens,
                    size_t tokens_length,
                    uint32_t multiplier_m,
                    uint32_t multiplier_l) {

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

    /* Tabel state L dan M, serta array tmp8 untuk ZepXORhash */
    alignas(64) uint32_t L[256];
    alignas(64) uint32_t M[256];
    uint32_t tmp8[8];

    /* Inisialisasi konstanta dan tabel L, M */
    uint32_t cons = seed;
    for (uint32_t i = 0; i < 256; ++i) {
        M[i] = (uint32_t)(i * multiplier_m + 0x06a0dd9b) & 0xFFFFFFFF;
        L[i] = (uint32_t)(i * multiplier_l + 0x9e3779b7U) & 0xFFFFFFFF;
    }

    /* Loop utama: tiap 'round' akan men-shuffle satu blok 255 token */
    for (size_t it = 0; it < iteration; ++it) {
        uint32_t a = cons;
        uint32_t b = (uint32_t)it;
        uint32_t c = 0;

        for (int i = 255; i >= 1; --i) {
            /* Hitung o = XOR mixing dari M */
            uint32_t o = 0;
            for (uint8_t e = 0; e < 8; ++e) {
                /* (i+e) mod 256 → dengan & 0xFF karena i < 256 */
                o ^= (M[(uint8_t)(i + e)] << e) & 0xFFFFFFFF;
            }

            /* Update a, b, o, c sesuai rumus */
            a = (rot32(b ^ o, c) ^ cons + a) & 0xFFFFFFFF;
            b = (rot32(cons + a, i) ^ o + c) & 0xFFFFFFFF;
            o = (rot32(a ^ o, i) << 9) ^ (b >> 18) & 0xFFFFFFFF;
            c = rot32(((o + c) << 14) ^ (b >> 13) ^ a, b) & 0xFFFFFFFF;
            /* Penghitungan idx menggunakan Lemire’s fast reduction: (c * (count+1)) >> 32 */
            uint32_t idx = (uint32_t)((uint64_t)c * (count + 1) >> 32);

            /* Perbarui c dengan Langkah berikutnya */
            c = (uint32_t)((uint64_t)c * (i + 1) >> 32);

            /* Swap pada scrambled_tokens dan pada tabel L */
            uint32_t tmp_tok = scrambled_tokens[count];
            scrambled_tokens[count] = scrambled_tokens[idx];
            scrambled_tokens[idx] = tmp_tok;

            o = L[i]
            L[i] = L[c];
            L[c] = o;

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

        /* Hitung ZepXORhash dari M ke tmp8 */
        ZepXORhash(M, tmp8);

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
 *   - filename: nama file teks berisi integer dipisah spasi/newline
 *   - *out_tokens: pointer yang akan diisi alamat buffer hasil alokasi
 *   - *out_len: panjang (jumlah elemen) token yang dibaca
 *
 * Mengembalikan 0 jika sukses, atau -1 jika terjadi error (file tidak ada atau format salah).
 * Buffer tokens harus dibebaskan oleh pemanggil (free).
 */
static int load_tokens_from_file(const char *filename, uint32_t **out_tokens, size_t *out_len) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: File '%s' tidak ditemukan.\n", filename);
        return -1;
    }

    /* Baca integer satu per satu menggunakan fscanf */
    size_t capacity = 1024;
    size_t length = 0;
    uint32_t *tokens = (uint32_t *)malloc(capacity * sizeof(uint32_t));
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
            uint32_t *tmp = (uint32_t *)realloc(tokens, capacity * sizeof(uint32_t));
            if (!tmp) {
                fprintf(stderr, "Error: Gagal merealokasi memori.\n");
                free(tokens);
                fclose(fp);
                return -1;
            }
            tokens = tmp;
        }
        tokens[length++] = (uint32_t)value;
    }

    fclose(fp);
    *out_tokens = tokens;
    *out_len = length;
    return 0;
}

/*
 * save_tokens_to_file:
 *   - tokens: array uint32_t
 *   - len: jumlah elemen dalam tokens
 *   - filename: nama file tujuan untuk menyimpan token dipisah spasi
 *
 * Mengembalikan 0 jika sukses, atau -1 jika gagal menulis file.
 */
static int save_tokens_to_file(const uint32_t *tokens, size_t len, const char *filename) {
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
    uint32_t seed;
    uint32_t multiplier_m;
    uint32_t multiplier_l;
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
    opts->seed = 1;
    opts->multiplier_m = 0x06a0dd9bU;
    opts->multiplier_l = 0x9e3779b7U;
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
                opts->seed = (uint32_t)strtoul(argv[++i], NULL, 10);
            } else {
                fprintf(stderr, "Error: --seed memerlukan argumen.\n");
                return -1;
            }
        } else if (strcmp(argv[i], "--multiplier_m") == 0 || strcmp(argv[i], "-mm") == 0) {
            if (i + 1 < argc) {
                opts->multiplier_m = (uint32_t)strtoul(argv[++i], NULL, 0);
            } else {
                fprintf(stderr, "Error: --multiplier_m memerlukan argumen.\n");
                return -1;
            }
        } else if (strcmp(argv[i], "--multiplier_l") == 0 || strcmp(argv[i], "-ml") == 0) {
            if (i + 1 < argc) {
                opts->multiplier_l = (uint32_t)strtoul(argv[++i], NULL, 0);
            } else {
                fprintf(stderr, "Error: --multiplier_l memerlukan argumen.\n");
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

    /* 2. Shuffle menggunakan ZepFold */
    ZepFold(opts.seed, tokens, token_len, opts.multiplier_m, opts.multiplier_l);

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
