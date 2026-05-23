#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

// ----------------- xoshiro256** Core -----------------
static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}
static uint64_t s[4];
void xoshiro256_init(uint64_t seed0, uint64_t seed1, uint64_t seed2, uint64_t seed3) {
    s[0] = seed0;
    s[1] = seed1;
    s[2] = seed2;
    s[3] = seed3;
}
uint64_t xoshiro256_next(void) {
    const uint64_t result = rotl(s[1] * 5, 7) * 9;
    const uint64_t t = s[1] << 17;
    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];
    s[2] ^= t;
    s[3] = rotl(s[3], 45);
    return result;
}

// ----------------- Buffered RNG wrapper -----------------
#define XOSHIRO_BUFFER_SIZE 1024
static uint64_t xoshiro_buffer[XOSHIRO_BUFFER_SIZE];
static size_t xoshiro_index = XOSHIRO_BUFFER_SIZE; // start "empty"

static inline uint64_t xoshiro_buffered_next(void) {
    if (xoshiro_index >= XOSHIRO_BUFFER_SIZE) {
        for (size_t i = 0; i < XOSHIRO_BUFFER_SIZE; i++) {
            xoshiro_buffer[i] = xoshiro256_next();
        }
        xoshiro_index = 0;
    }
    return xoshiro_buffer[xoshiro_index++];
}

// Lemire fast bounded random: uniform [0, bound)
static inline uint64_t rand_bounded(uint64_t bound) {
    uint64_t r = xoshiro_buffered_next();
    __uint128_t m = (__uint128_t)r * bound;
    return (uint64_t)(m >> 64);
}

int main(int argc, char* argv[]) {
    const char* input_path = NULL;
    const char* output_path = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            input_path = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_path = argv[++i];
        }
    }
    if (!input_path || !output_path) {
        fprintf(stderr, "Usage: %s -i tokens.txt -o scrambled.txt\n", argv[0]);
        return 1;
    }

    // Read file into memory
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

    // Tokenize by whitespace
    size_t capacity = 1024;
    size_t count = 0;
    char** tokens = (char**)malloc(capacity * sizeof(char*));
    char* p = data;
    while (*p) {
        while (*p && (*p==' '||*p=='\n'||*p=='\t'||*p=='\r')) p++;
        if (!*p) break;
        if (count == capacity) {
            capacity *= 2;
            tokens = (char**)realloc(tokens, capacity * sizeof(char*));
            if (!tokens) { fprintf(stderr, "Realloc failed\n"); free(data); return 1; }
        }
        tokens[count++] = p;
        while (*p && !(*p==' '||*p=='\n'||*p=='\t'||*p=='\r')) p++;
        if (*p) *p++ = '\0';
    }

    // Initialize PRNG with time-based seed
    uint64_t seed = (uint64_t)time(NULL);
    xoshiro256_init(seed, seed ^ 0x9E3779B97F4A7C15ULL, seed << 13, seed >> 7);

    // Shuffle with Fisher-Yates
    clock_t t0 = clock();
    for (size_t i = count; i > 1; i--) {
        uint64_t j = rand_bounded(i);
        char* tmp = tokens[i-1];
        tokens[i-1] = tokens[j];
        tokens[j] = tmp;
    }
    clock_t t1 = clock();

    double elapsed = (double)(t1 - t0) / CLOCKS_PER_SEC;
    fprintf(stderr, "Shuffled %zu items in %.3f seconds\n", count, elapsed);

    // Write output
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
