#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
static inline uint32_t rot32(uint32_t n, uint32_t r){r&=31;return((n<<r)|(n>>(32-r)))&0xFFFFFFFFu;}
static inline uint32_t fmix32(uint32_t h){h^=h>>16;h*=0x85ebca6bu;h^=h>>13;h*=0xc2b2ae35u;h^=h>>16;return h;}
static void ra_init_state_orbit(uint32_t *L, uint32_t *M, uint32_t key){
    for(int i=0;i<256;++i){
        uint32_t r=key^(uint32_t)i;
        uint32_t l=(uint32_t)(i*0x9e3779b7u+0x9e3779b7u*key);
        uint32_t m=(uint32_t)(i*0x06a0dd9bu+0x06a0dd9bu*key);
        L[i]=rot32(l,r); M[i]=rot32(m,r);
    }
}
static void ra_init_state_avalanche(uint32_t *L, uint32_t *M, uint32_t key){
    for(int i=0;i<256;++i){
        uint32_t r=key^(uint32_t)i;
        uint32_t l=(uint32_t)(i*0x9e3779b7u+0x9e3779b7u*key);
        uint32_t m=(uint32_t)(i*0x06a0dd9bu+0x06a0dd9bu*key);
        L[i]=fmix32(rot32(l,r)); M[i]=fmix32(rot32(m,r));
    }
}
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
static uint32_t core_avalanche(uint32_t key, size_t rng, uint32_t *sink){
    alignas(64) uint32_t L[256],M[256]; ra_init_state_avalanche(L,M,key);
    uint64_t count=rng; ra_cycle(key,0,M,L,&count,sink); return key;
}
int main(int argc, char **argv){
    size_t K = argc>1?(size_t)strtoull(argv[1],NULL,0):255;
    size_t total_words=20000000;
    size_t nkeys=total_words/K;
    uint32_t sink=0;
    struct timespec t0,t1;
    clock_gettime(CLOCK_MONOTONIC,&t0);
    for(size_t k=0;k<nkeys;++k){uint32_t key=(uint32_t)(k*0x9E3779B9u+12345u); core_baseline(key,K,&sink);}
    clock_gettime(CLOCK_MONOTONIC,&t1);
    double t_base=(t1.tv_sec-t0.tv_sec)+(t1.tv_nsec-t0.tv_nsec)/1e9;
    clock_gettime(CLOCK_MONOTONIC,&t0);
    for(size_t k=0;k<nkeys;++k){uint32_t key=(uint32_t)(k*0x9E3779B9u+12345u); core_avalanche(key,K,&sink);}
    clock_gettime(CLOCK_MONOTONIC,&t1);
    double t_ava=(t1.tv_sec-t0.tv_sec)+(t1.tv_nsec-t0.tv_nsec)/1e9;
    double words=(double)(nkeys*K);
    printf("K=%zu  baseline=%.4f ns/word  avalanche_init=%.4f ns/word  overhead=%.2fx  sink=%u\n",
           K, t_base/words*1e9, t_ava/words*1e9, t_ava/t_base, sink);
    return 0;
}
