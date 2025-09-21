#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// ----------------- Philox4x32-10 Core -----------------
#define PHILOX_M0 0xD256D193U
#define PHILOX_M1 0xCD9E8D57U
#define PHILOX_W0 0x9E3779B9U
#define PHILOX_W1 0xBB67AE85U

static inline void philox4x32_round(uint32_t ctr[4], uint32_t key[2]) {
    uint64_t p0 = (uint64_t)ctr[0] * PHILOX_M0;
    uint64_t p1 = (uint64_t)ctr[2] * PHILOX_M1;
    uint32_t hi0 = (uint32_t)(p0 >> 32);
    uint32_t lo0 = (uint32_t)p0;
    uint32_t hi1 = (uint32_t)(p1 >> 32);
    uint32_t lo1 = (uint32_t)p1;
    uint32_t c1 = ctr[1];
    uint32_t c3 = ctr[3];
    ctr[0] = hi1 ^ c1 ^ key[0];
    ctr[1] = lo1;
    ctr[2] = hi0 ^ c3 ^ key[1];
    ctr[3] = lo0;
    key[0] += PHILOX_W0;
    key[1] += PHILOX_W1;
}

void philox4x32_10(uint32_t out[4], const uint32_t ctr_in[4], const uint32_t key_in[2]) {
    uint32_t ctr[4] = { ctr_in[0], ctr_in[1], ctr_in[2], ctr_in[3] };
    uint32_t key[2] = { key_in[0], key_in[1] };
    for (int i = 0; i < 10; i++) {
        philox4x32_round(ctr, key);
    }
    memcpy(out, ctr, 4 * sizeof(uint32_t));
}

// Global Philox state
static uint32_t philox_ctr[4] = {0,0,0,0};
static uint32_t philox_key[2] = {0xDEADBEEF, 0xFEEDC0DE};
static uint32_t philox_block[4];
static int philox_idx = 4;

// Generate next 32-bit random word
static uint32_t philox_rand32(void) {
    if (philox_idx >= 4) {
        philox4x32_10(philox_block, philox_ctr, philox_key);
        // increment counter little-endian
        if (++philox_ctr[0] == 0) {
            if (++philox_ctr[1] == 0) {
                if (++philox_ctr[2] == 0) {
                    ++philox_ctr[3];
                }
            }
        }
        philox_idx = 0;
    }
    return philox_block[philox_idx++];
}

// Lemire fast reduction: uniform in [0, bound)
static inline uint32_t rand_bounded(uint32_t bound) {
    uint32_t r = philox_rand32();
    uint64_t m = (uint64_t)r * bound;
    return (uint32_t)(m >> 32);
}

int main(int argc, char* argv[]) {
    const char* input_path = NULL;
    const char* output_path = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i+1 < argc) input_path = argv[++i];
        else if (strcmp(argv[i], "-o") == 0 && i+1 < argc) output_path = argv[++i];
    }
    if (!input_path || !output_path) {
        fprintf(stderr, "Usage: %s -i tokens.txt -o scrambled.txt\n", argv[0]);
        return 1;
    }

    FILE* fin = fopen(input_path, "rb");
    if (!fin) { perror("fopen input"); return 1; }
    fseek(fin, 0, SEEK_END);
    long fsize = ftell(fin);
    rewind(fin);
    char* data = (char*)malloc(fsize + 1);
    if (!data) { fprintf(stderr, "Memory alloc failed\n"); fclose(fin); return 1; }
    fread(data, 1, fsize, fin);
    data[fsize] = '\0';
    fclose(fin);

    size_t cap = 1024, count = 0;
    char** tokens = (char**)malloc(cap * sizeof(char*));
    if (!tokens) { fprintf(stderr, "Memory alloc failed\n"); free(data); return 1; }
    char* p = data;
    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        if (count == cap) {
            cap *= 2;
            tokens = (char**)realloc(tokens, cap * sizeof(char*));
            if (!tokens) { fprintf(stderr, "Realloc failed\n"); free(data); return 1; }
        }
        tokens[count++] = p;
        while (*p && !isspace((unsigned char)*p)) p++;
        if (*p) *p++ = '\0';
    }

    clock_t t0 = clock();
    for (size_t i = count; i > 1; i--) {
        uint32_t j = rand_bounded((uint32_t)i);
        char* tmp = tokens[i-1];
        tokens[i-1] = tokens[j]; 
        tokens[j] = tmp;
    }
    clock_t t1 = clock();
    double elapsed = (double)(t1 - t0) / CLOCKS_PER_SEC;
    fprintf(stderr, "Shuffled %zu items in %.3f seconds\n", count, elapsed);

    FILE* fout = fopen(output_path, "wb");
    if (!fout) { perror("fopen output"); free(data); free(tokens); return 1; }
    for (size_t i = 0; i < count; i++) {
        fwrite(tokens[i], 1, strlen(tokens[i]), fout);
        if (i+1 < count) fputc(' ', fout);
    }
    fclose(fout);
    free(data);
    free(tokens);
    return 0;
}
