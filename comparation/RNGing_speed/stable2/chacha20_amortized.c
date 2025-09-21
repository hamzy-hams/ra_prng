#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// ----------------- ChaCha20 Core -----------------
static inline uint32_t rotl32(uint32_t x, int r) {
    return (x << r) | (x >> (32 - r));
}
#define QUARTERROUND(a, b, c, d) \
    a += b; d ^= a; d = rotl32(d,16); \
    c += d; b ^= c; b = rotl32(b,12); \
    a += b; d ^= a; d = rotl32(d, 8); \
    c += d; b ^= c; b = rotl32(b, 7);

void chacha20_block(uint32_t out[16], const uint32_t key[8], uint32_t counter, const uint32_t nonce[3]) {
    const uint32_t constants[4] = {0x61707865,0x3320646e,0x79622d32,0x6b206574};
    uint32_t state[16];
    memcpy(state, constants, 4 * sizeof(uint32_t));
    memcpy(state + 4, key, 8 * sizeof(uint32_t));
    state[12] = counter;
    memcpy(state + 13, nonce, 3 * sizeof(uint32_t));
    uint32_t x[16];
    memcpy(x, state, sizeof x);
    for (int i = 0; i < 10; i++) {
        QUARTERROUND(x[0], x[4], x[8],  x[12]);
        QUARTERROUND(x[1], x[5], x[9],  x[13]);
        QUARTERROUND(x[2], x[6], x[10], x[14]);
        QUARTERROUND(x[3], x[7], x[11], x[15]);
        QUARTERROUND(x[0], x[5], x[10], x[15]);
        QUARTERROUND(x[1], x[6], x[11], x[12]);
        QUARTERROUND(x[2], x[7], x[8],  x[13]);
        QUARTERROUND(x[3], x[4], x[9],  x[14]);
    }
    for (int i = 0; i < 16; i++) out[i] = x[i] + state[i];
}

// Lemire fast bounded random: uniform [0, bound)
static inline uint32_t rand_bounded(uint32_t bound, uint32_t key_stream[16], uint32_t *ks_index) {
    if (*ks_index == 0) {
        static const uint32_t key[8]   = {0x03020100,0x07060504,0x0b0a0908,0x0f0e0d0c,
                                          0x13121110,0x17161514,0x1b1a1918,0x1f1e1d1c};
        static const uint32_t nonce[3] = {0x00000000,0x00000000,0x00000000};
        static uint32_t counter = 1;
        chacha20_block(key_stream, key, counter++, nonce);
    }
    uint32_t r = key_stream[*ks_index];
    *ks_index = (*ks_index + 1) & 15;
    uint64_t m = (uint64_t)r * bound;
    return (uint32_t)(m >> 32);
}

int main(int argc, char* argv[]) {
    const char* input_path = NULL;
    const char* output_path = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) input_path = argv[++i];
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) output_path = argv[++i];
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
    if (!data) { fprintf(stderr, "Mem alloc failed\n"); fclose(fin); return 1; }
    fread(data, 1, fsize, fin);
    data[fsize] = '\0';
    fclose(fin);

    size_t cap = 1024, count = 0;
    char** tokens = (char**)malloc(cap * sizeof(char*));
    if (!tokens) { fprintf(stderr, "Mem alloc failed\n"); free(data); return 1; }
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

    uint32_t key_stream[16];
    uint32_t ks_index = 0;
    clock_t t0 = clock();
    for (size_t i = count; i > 1; i--) {
        uint32_t j = rand_bounded((uint32_t)i, key_stream, &ks_index);
        char* tmp = tokens[i - 1];
        tokens[i - 1] = tokens[j];
        tokens[j] = tmp;
    }
    clock_t t1 = clock();
    double elapsed = (double)(t1 - t0) / CLOCKS_PER_SEC;
    fprintf(stderr, "Shuffled %zu items in %.3f seconds\n", count, elapsed);

    FILE* fout = fopen(output_path, "wb");
    if (!fout) { perror("fopen output"); free(data); free(tokens); return 1; }
    for (size_t i = 0; i < count; i++) {
        fwrite(tokens[i], 1, strlen(tokens[i]), fout);
        if (i + 1 < count) fputc(' ', fout);
    }
    fclose(fout);

    free(data);
    free(tokens);
    return 0;
}
