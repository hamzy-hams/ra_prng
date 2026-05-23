#include <stdint.h>
#include <stdio.h>
#include <time.h>

// ----------------- Benchmark /dev/urandom -----------------

int main() {
    const size_t N = 255000000;      // total 32-bit numbers
    FILE *ur = fopen("/dev/urandom", "rb");
    if (!ur) {
        perror("fopen");
        return 1;
    }

    uint32_t block[4];
    size_t produced = 0;

    clock_t t0 = clock();
    while (produced < N) {
        // read 4 bytes of data
        size_t got = fread(block, sizeof(uint32_t), 4, ur);
        if (got != 4) {
            perror("fread");
            break;
        }
        // return 4×32-bit
        for (int i = 0; i < 4 && produced < N; i++, produced++) {
            volatile uint32_t v = block[i];
            (void)v;  // preventing elide optimization
        }
    }
    clock_t t1 = clock();

    fclose(ur);

    double elapsed   = (double)(t1 - t0) / CLOCKS_PER_SEC;
    double total_mb  = (double)N * sizeof(uint32_t) / (1024.0 * 1024.0);
    double mb_per_s  = total_mb / elapsed;

    printf("Generated %zu 32-bit values from /dev/urandom in %.3f seconds\n",
           N, elapsed);
    printf("Throughput: %.2f MB/s\n", mb_per_s);

    return 0;
}
