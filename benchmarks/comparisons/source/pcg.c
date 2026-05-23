#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

// ----------------- PCG32 Implementation -----------------
typedef struct {
    uint64_t state;
    uint64_t inc;
} pcg32_random_t;

void pcg32_srandom(pcg32_random_t* rng, uint64_t initstate, uint64_t initseq) {
    rng->state = 0U;
    rng->inc   = (initseq << 1u) | 1u;
    rng->state = rng->state * 6364136223846793005ULL + rng->inc;
    rng->state += initstate;
    rng->state = rng->state * 6364136223846793005ULL + rng->inc;
}

uint32_t pcg32_random_r(pcg32_random_t* rng) {
    uint64_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

// Fast random bound using Lemire's method: uniform in [0, bound)
static inline uint32_t rand_bounded(pcg32_random_t* rng, uint32_t bound) {
    uint64_t r = pcg32_random_r(rng);
    uint64_t m = r * bound;
    return (uint32_t)(m >> 32);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s input.txt -o output.txt\n", argv[0]);
        return 1;
    }
    const char* input_path = argv[1];
    const char* output_path = NULL;
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i+1 < argc) {
            output_path = argv[i+1];
            i++;
        }
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
    
    // Tokenize in-place
    size_t capacity = 1024;
    size_t count = 0;
    char** tokens = (char**)malloc(capacity * sizeof(char*));
    char* ptr = data;
    while (*ptr) {
        while (*ptr && (*ptr==' '||*ptr=='\n'||*ptr=='\t'||*ptr=='\r')) ptr++;
        if (!*ptr) break;
        if (count >= capacity) {
            capacity *= 2;
            tokens = (char**)realloc(tokens, capacity * sizeof(char*));
        }
        tokens[count++] = ptr;
        while (*ptr && !(*ptr==' '||*ptr=='\n'||*ptr=='\t'||*ptr=='\r')) ptr++;
        if (*ptr) *ptr++ = '\0';
    }

    // Initialize PCG
    pcg32_random_t rng;
    pcg32_srandom(&rng, (uint64_t)time(NULL), 54u);

    // Benchmark shuffle
    clock_t t0 = clock();
    for (size_t i = count; i > 1; i--) {
        uint32_t j = rand_bounded(&rng, (uint32_t)i);
        char* tmp = tokens[i-1];
        tokens[i-1] = tokens[j];
        tokens[j] = tmp;
    }
    clock_t t1 = clock();

    double elapsed = (double)(t1 - t0) / CLOCKS_PER_SEC;
    fprintf(stderr, "Shuffled %zu items in %.3f seconds\n", count, elapsed);

    // Write output
    FILE* fout = output_path ? fopen(output_path, "wb") : stdout;
    if (!fout) { perror("fopen output"); free(data); free(tokens); return 1; }
    for (size_t i = 0; i < count; i++) {
        fwrite(tokens[i], 1, strlen(tokens[i]), fout);
        if (i + 1 < count) fputc(' ', fout);
    }
    if (output_path) fclose(fout);

    free(data);
    free(tokens);
    return 0;
}
