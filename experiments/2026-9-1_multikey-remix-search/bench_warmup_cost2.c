#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
static inline uint32_t rot32(uint32_t n, uint32_t r){r&=31;return((n<<r)|(n>>(32-r)))&0xFFFFFFFFu;}
static void ra_hash(const uint32_t *N, uint32_t *out8){for(int i=0;i<8;++i){out8[i]=0;int b=i*32;for(int j=0;j<32;++j)out8[i]^=N[b+j];}}
static uint32_t ra_reseed(uint32_t *M, const uint32_t *L){for(int i=0;i<256;++i)M[i]^=L[i];uint32_t t8[8];ra_hash(M,t8);uint32_t nc=0;for(int e=0;e<8;++e)nc^=(t8[e]<<e);return nc;}
static void ra_init_state_orbit(uint32_t *L, uint32_t *M, uint32_t key){for(int i=0;i<256;++i){uint32_t r=key^(uint32_t)i;uint32_t l=(uint32_t)(i*0x9e3779b7u+0x9e3779b7u*key);uint32_t m=(uint32_t)(i*0x06a0dd9bu+0x06a0dd9bu*key);L[i]=rot32(l,r);M[i]=rot32(m,r);}}
static void ra_cycle(uint32_t cons, size_t it, const uint32_t *M, uint32_t *L, uint64_t *count, uint32_t *sink){
    uint32_t a=cons,b=(uint32_t)it,c=0,d=0;
    for(uint32_t i=255;i>0;--i){
        uint32_t o=(M[(uint8_t)(i+6)]<<6)^(M[(uint8_t)(i+7)]<<7);
        a=(d^o)^(cons+a); b=(cons+a)^(o+d); c=rot32((a>>13)^a,b);
        *sink^=c; d=c&0xFFu;
        if(*count<=1) break;
        --(*count);
        uint32_t tmp=L[i]; L[i]=L[d]; L[d]=tmp;
    }
}
static uint32_t core_baseline(uint32_t key, size_t rng, uint32_t *sink){
    alignas(64) uint32_t L[256],M[256]; ra_init_state_orbit(L,M,key);
    uint64_t count=rng; ra_cycle(key,0,M,L,&count,sink); return key;
}
static uint32_t core_warmup(uint32_t key, size_t rng, size_t depth, uint32_t *sink){
    alignas(64) uint32_t L[256],M[256]; ra_init_state_orbit(L,M,key);
    uint32_t cons;
    if (depth>0){uint64_t wc=depth; uint32_t d0=0; ra_cycle(key,0,M,L,&wc,&d0); cons=ra_reseed(M,L); *sink^=d0;}
    else cons=key;
    uint64_t count=rng; ra_cycle(cons,1,M,L,&count,sink); return cons;
}
int main(int argc, char **argv){
    size_t K = argc>1?(size_t)strtoull(argv[1],NULL,0):255;
    size_t depth = argc>2?(size_t)strtoull(argv[2],NULL,0):255;
    size_t total_words=20000000;
    size_t nkeys=total_words/K;
    uint32_t sink=0;
    struct timespec t0,t1;
    clock_gettime(CLOCK_MONOTONIC,&t0);
    for(size_t k=0;k<nkeys;++k){uint32_t key=(uint32_t)(k*0x9E3779B9u+12345u); core_baseline(key,K,&sink);}
    clock_gettime(CLOCK_MONOTONIC,&t1);
    double t_base=(t1.tv_sec-t0.tv_sec)+(t1.tv_nsec-t0.tv_nsec)/1e9;
    clock_gettime(CLOCK_MONOTONIC,&t0);
    for(size_t k=0;k<nkeys;++k){uint32_t key=(uint32_t)(k*0x9E3779B9u+12345u); core_warmup(key,K,depth,&sink);}
    clock_gettime(CLOCK_MONOTONIC,&t1);
    double t_warm=(t1.tv_sec-t0.tv_sec)+(t1.tv_nsec-t0.tv_nsec)/1e9;
    double words=(double)(nkeys*K);
    printf("K=%zu depth=%zu  baseline=%.4f ns/word  warmup=%.4f ns/word  overhead=%.2fx  sink=%u\n",
           K, depth, t_base/words*1e9, t_warm/words*1e9, t_warm/t_base, sink);
    return 0;
}
