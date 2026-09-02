// diag_warmup_depth.c -- DIAGNOSTIC ONLY. Same as diag_warmup_discard.c but
// warm-up cycle depth (steps run+discarded before reseed) is a CLI param,
// to find the minimal depth that still clears the BCFN failure -- full 255
// is expensive at small K (bench_warmup_cost.c: 13.64x at K=1).
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}
static void ra_hash(const uint32_t *N, uint32_t *out8) {
    for (int i=0;i<8;++i){ out8[i]=0; int base=i*32; for(int j=0;j<32;++j) out8[i]^=N[base+j]; }
}
static uint32_t ra_reseed(uint32_t *M, const uint32_t *L) {
    for (int i=0;i<256;++i) M[i]^=L[i];
    uint32_t t8[8]; ra_hash(M,t8);
    uint32_t nc=0; for(int e=0;e<8;++e) nc^=(t8[e]<<e);
    return nc;
}
static void ra_init_state_orbit(uint32_t *L, uint32_t *M, uint32_t key) {
    for (int i=0;i<256;++i) {
        uint32_t r=key^(uint32_t)i;
        uint32_t l=(uint32_t)(i*0x9e3779b7u+0x9e3779b7u*key);
        uint32_t m=(uint32_t)(i*0x06a0dd9bu+0x06a0dd9bu*key);
        L[i]=rot32(l,r); M[i]=rot32(m,r);
    }
}
static void ra_permutation_cycle_orbit(uint32_t cons, size_t it,
                                        const uint32_t *M, uint32_t *L,
                                        uint64_t *count, FILE *raw_stream) {
    uint32_t a=cons,b=(uint32_t)it,c=0,d=0;
    for (uint32_t i=255;i>0;--i) {
        uint32_t o=(M[(uint8_t)(i+6)]<<6)^(M[(uint8_t)(i+7)]<<7);
        a=(d^o)^(cons+a); b=(cons+a)^(o+d); c=rot32((a>>13)^a,b);
        if (raw_stream) fwrite(&c,sizeof(uint32_t),1,raw_stream);
        d=c&0xFFu;
        if (*count<=1) break;
        --(*count);
        uint32_t tmp=L[i]; L[i]=L[d]; L[d]=tmp;
    }
}

static size_t g_warmup_depth = 255;

static void ra_core_warmup(uint32_t key, size_t block_len, FILE *raw_stream) {
    if (block_len==0) return;
    alignas(64) uint32_t L[256], M[256];
    ra_init_state_orbit(L,M,key);

    uint32_t cons;
    if (g_warmup_depth > 0) {
        uint64_t wc = g_warmup_depth;
        ra_permutation_cycle_orbit(key, 0, M, L, &wc, NULL);
        cons = ra_reseed(M, L);
    } else {
        cons = key; // depth 0 == no warmup, matches original defect
    }

    uint64_t count = block_len;
    ra_permutation_cycle_orbit(cons, 1, M, L, &count, raw_stream);
}

int main(int argc, char **argv) {
    if (argc < 5 || strcmp(argv[1], "--stream") != 0) {
        fprintf(stderr, "usage: %s --stream <base_key> <n> <warmup_depth>\n", argv[0]);
        return 1;
    }
    uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
    size_t n = (size_t)strtoull(argv[3], NULL, 0);
    g_warmup_depth = (size_t)strtoull(argv[4], NULL, 0);

    uint64_t block_idx = 0;
    size_t remaining = n;
    while (remaining > 0) {
        size_t block_len = remaining < 255 ? remaining : 255;
        uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
        ra_core_warmup(key, block_len, stdout);
        remaining -= block_len;
        ++block_idx;
    }
    return 0;
}
