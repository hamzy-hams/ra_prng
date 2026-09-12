// fpf_scan_addr_hi.c -- Tahap 3 (HANDOVER.md §5.2 "uji akseptansi utama"):
// reproduksi PERSIS statistik FPF-14+6/16 yang menemukan defect asli
// (Analisis 4, ../2026-9-5_orbit-k1-exhaustive-function-analysis/RESULTS.md),
// sekarang dijalankan terhadap kandidat 2-param (key,addr_hi) yang SUNGGUHAN
// (link objek nyata candidate_a.o/candidate_b.o, BUKAN reimplementasi manual
// -- menghindari risiko silent-staleness yang ditandai untuk
// exhaustive_analysis.c's f_key()).
//
// Mesin FPF (fpf_process_word/fpf_report/fpf_bump/count_low_zeroes32) DAN
// splitmix32_hi/gen_urandom_key disalin VERBATIM dari
// ../2026-9-5_orbit-k1-exhaustive-function-analysis/exhaustive_analysis.c
// (L203-262, L505-544) -- ini algoritma statistik generik atas stream
// uint32_t opaque (porting PractRand tests.cpp), TIDAK terikat ke formula
// kandidat manapun, jadi aman disalin ulang (beda dengan f_key() yang
// reimplementasi formula, yang justru TIDAK disalin di sini).
//
// Beda dari exhaustive_analysis.c: alih-alih f_key(key) manual, setiap
// sampel memanggil ra_core_orbit(key, addr_hi, 1, raw_stream) SUNGGUHAN
// (linked object) via fmemopen untuk menangkap word `c` aktual (bukan
// return value `cons` -- gotcha yang sama seperti checksum_key_core di
// ra_core.c, lihat memory project_orbit_k1_exhaustive_function_analysis).
//
// Ordering yang didukung: weyl (regresi, harus tetap CLEAN -- dua Weyl
// sequence independen utk key & addr_hi), hashed (splitmix64-chain,
// akseptansi utama), urandom (akseptansi utama kedua, independen dari
// splitmix64).
//
// Compile: gcc -O3 -march=native -std=gnu17 -c -Dmain=ra_core_unused_main \
//            candidates/candidate_X.c -o candidates/candidate_X_nomain.o
//          gcc -O3 -march=native -std=gnu17 fpf_scan_addr_hi.c \
//            candidates/candidate_X_nomain.o -lm -o fpf_scan_candidate_X

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>

extern uint32_t ra_core_orbit(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream);

static double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Panggil ra_core_orbit SUNGGUHAN, tangkap word `c` aktual (bukan `cons`)
// via fmemopen -- pola sama seperti ra_core.c's checksum_key_core.
static inline uint32_t real_orbit_word(uint32_t key, uint32_t addr_hi) {
    uint32_t buf[2]; // 1 word slot + 1 spare (fmemopen "wb" trailing NUL guard)
    FILE *fs = fmemopen(buf, sizeof(uint32_t) + 1, "wb");
    ra_core_orbit(key, addr_hi, 1, fs);
    fclose(fs);
    return buf[0];
}

// =======================================================================
// FPF-14+6/16 -- VERBATIM dari exhaustive_analysis.c L203-262.
// =======================================================================
#define FPF_SIG_BITS 14u
#define FPF_MAX_SIG 16383u
#define FPF_MAX_EXP 51u
#define FPF_INV_SIG_BITS 18u
#define FPF_NUM_EXP (FPF_MAX_EXP + 1u)
#define FPF_TOTAL_SIZE (FPF_NUM_EXP * (FPF_MAX_SIG + 1u))

static uint64_t *fpf_counts = NULL;
static uint32_t fpf_cur_reg = 0;

static inline uint32_t count_low_zeroes32(uint32_t v) {
    return v ? (uint32_t)__builtin_ctz(v) : 32u;
}

static inline void fpf_bump(uint32_t e, uint32_t sig) {
    fpf_counts[(uint64_t)e * (FPF_MAX_SIG + 1u) + sig]++;
}

static inline void fpf_process_word(uint32_t word, uint32_t next_word) {
    uint32_t cur = fpf_cur_reg;
    uint32_t w = word;
    uint32_t e, sig;

    cur >>= 16;
    cur |= w << 16;
    w >>= 16;
    e = count_low_zeroes32(cur);
    if (e < FPF_INV_SIG_BITS) {
        sig = (cur >> (e + 1)) & FPF_MAX_SIG;
        fpf_bump(e, sig);
    } else {
        uint64_t cur2 = (uint64_t)cur | ((uint64_t)w << 32) | ((uint64_t)next_word << (64 - 16));
        if (e == 32u) e += count_low_zeroes32((uint32_t)(cur2 >> 32));
        if (e < FPF_MAX_EXP) sig = (uint32_t)(cur2 >> (e + 1)) & FPF_MAX_SIG;
        else { e = FPF_MAX_EXP; sig = (uint32_t)(cur2 >> e) & FPF_MAX_SIG; }
        fpf_bump(e, sig);
    }

    cur >>= 16;
    cur |= w << 16;
    e = count_low_zeroes32(cur);
    if (e < FPF_INV_SIG_BITS) {
        sig = (cur >> (e + 1)) & FPF_MAX_SIG;
        fpf_bump(e, sig);
    } else {
        w >>= 16;
        uint64_t cur2 = (uint64_t)cur | ((uint64_t)w << 32) | ((uint64_t)next_word << (64 - 32));
        if (e == 32u) e += count_low_zeroes32((uint32_t)(cur2 >> 32));
        if (e < FPF_MAX_EXP) sig = (uint32_t)(cur2 >> (e + 1)) & FPF_MAX_SIG;
        else { e = FPF_MAX_EXP; sig = (uint32_t)(cur2 >> e) & FPF_MAX_SIG; }
        fpf_bump(e, sig);
    }

    fpf_cur_reg = cur;
}

static double fpf_report(uint64_t *total_out) {
    uint64_t total = 0;
    uint64_t obs_e[FPF_NUM_EXP];
    for (uint32_t e = 0; e < FPF_NUM_EXP; ++e) {
        uint64_t s = 0;
        for (uint32_t sig = 0; sig <= FPF_MAX_SIG; ++sig) s += fpf_counts[(uint64_t)e * (FPF_MAX_SIG + 1u) + sig];
        obs_e[e] = s;
        total += s;
    }
    *total_out = total;
    printf("=== FPF-14+6/16 (total_samples=%llu) ===\n", (unsigned long long)total);
    for (uint32_t e = 0; e < FPF_NUM_EXP; ++e) {
        double p = (e < FPF_MAX_EXP) ? pow(0.5, (double)e + 1.0) : pow(0.5, (double)FPF_MAX_EXP);
        double expc = p * (double)total;
        if (expc < 1e-6) continue;
        double z = (obs_e[e] - expc) / sqrt(expc);
        if (fabs(z) > 4.0 || e < 3 || e == FPF_MAX_EXP) {
            printf("  e=%2u observed=%12llu expected=%14.2f z=%+8.3f%s\n",
                   e, (unsigned long long)obs_e[e], expc, z, fabs(z) > 6.0 ? "  <-- ANOMALI" : "");
        }
    }

    double agg_chi2 = 0.0;
    uint64_t agg_df = 0;
    for (uint32_t e = 0; e < FPF_NUM_EXP; ++e) {
        double p_e = (e < FPF_MAX_EXP) ? pow(0.5, (double)e + 1.0) : pow(0.5, (double)FPF_MAX_EXP);
        double expc_bin = p_e * (double)total / (double)(FPF_MAX_SIG + 1u);
        if (expc_bin < 20.0) continue;
        double chi2_e = 0.0;
        for (uint32_t sig = 0; sig <= FPF_MAX_SIG; ++sig) {
            uint64_t obs = fpf_counts[(uint64_t)e * (FPF_MAX_SIG + 1u) + sig];
            double d = (double)obs - expc_bin;
            chi2_e += d * d / expc_bin;
        }
        agg_chi2 += chi2_e;
        agg_df += FPF_MAX_SIG;
    }
    double agg_z = (agg_df > 0) ? (agg_chi2 - (double)agg_df) / sqrt(2.0 * (double)agg_df) : 0.0;
    printf("  agg_chi2=%.1f agg_df=%llu agg_z=%+.3f\n", agg_chi2, (unsigned long long)agg_df, agg_z);
    return agg_z;
}

// =======================================================================
// Generator (key,addr_hi) -- VERBATIM splitmix32_hi/gen_urandom_key style
// dari exhaustive_analysis.c L505-544, diperluas jadi PASANGAN 64-bit.
// =======================================================================
static uint32_t splitmix32_hi(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    z = z ^ (z >> 31);
    return (uint32_t)(z >> 32);
}

#define URANDOM_BUF_BYTES (1u << 20)
static FILE *g_urandom_fp = NULL;
static uint8_t g_urandom_buf[URANDOM_BUF_BYTES];
static size_t g_urandom_pos = URANDOM_BUF_BYTES;

static uint32_t gen_urandom_word(void) {
    if (!g_urandom_fp) {
        g_urandom_fp = fopen("/dev/urandom", "rb");
        if (!g_urandom_fp) { fprintf(stderr, "gagal buka /dev/urandom\n"); exit(1); }
    }
    if (g_urandom_pos + 4 > URANDOM_BUF_BYTES) {
        size_t got = fread(g_urandom_buf, 1, URANDOM_BUF_BYTES, g_urandom_fp);
        if (got != URANDOM_BUF_BYTES) { fprintf(stderr, "urandom short read (%zu)\n", got); exit(1); }
        g_urandom_pos = 0;
    }
    uint32_t v;
    memcpy(&v, g_urandom_buf + g_urandom_pos, 4);
    g_urandom_pos += 4;
    return v;
}

#define WEYL_DELTA_KEY 0x9E3779B9u
#define WEYL_DELTA_ADDR 0x85EBCA6Bu // konstanta beda supaya 2 stream weyl tak berkorelasi trivial

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s <weyl|hashed|urandom> <N> [fixed_addr_hi_hex]\n", argv[0]);
        fprintf(stderr, "  [fixed_addr_hi_hex]: opsional, kalau diisi addr_hi TIDAK di-generate\n");
        fprintf(stderr, "  dari stream (dipakai konstan) -- untuk kontrol positif/reproduksi bug\n");
        fprintf(stderr, "  32-bit lama lewat candidate_c (byte-identik addr_hi=0).\n");
        return 1;
    }
    const char *order = argv[1];
    uint64_t n = strtoull(argv[2], NULL, 0);
    int has_fixed_addr = argc >= 4;
    uint32_t fixed_addr_hi = has_fixed_addr ? (uint32_t)strtoul(argv[3], NULL, 0) : 0;

    int use_weyl = strcmp(order, "weyl") == 0;
    int use_hashed = strcmp(order, "hashed") == 0;
    int use_urandom = strcmp(order, "urandom") == 0;
    if (!use_weyl && !use_hashed && !use_urandom) {
        fprintf(stderr, "order tidak dikenal: %s (pakai weyl|hashed|urandom)\n", order);
        return 2;
    }

    fpf_counts = calloc(FPF_TOTAL_SIZE, sizeof(uint64_t));
    if (!fpf_counts) { fprintf(stderr, "alokasi fpf_counts gagal\n"); return 1; }

    uint64_t sm_state_key = 0xC0FFEE1234ULL;
    uint64_t sm_state_addr = 0xDEADBEEFCAFEULL; // state independen dari key
    uint32_t key = 0, addr_hi = 0;

    double t0 = now_sec();
    uint32_t c_next, c_cur;
    int have_seed = 0;
    uint32_t next_key, next_addr;

    for (uint64_t i = 0; i <= n; ++i) {
        if (use_weyl) {
            next_key = (uint32_t)(i * WEYL_DELTA_KEY);
            next_addr = (uint32_t)(i * WEYL_DELTA_ADDR);
        } else if (use_hashed) {
            next_key = splitmix32_hi(&sm_state_key);
            next_addr = splitmix32_hi(&sm_state_addr);
        } else { // urandom
            next_key = gen_urandom_word();
            next_addr = has_fixed_addr ? fixed_addr_hi : gen_urandom_word();
        }
        if (has_fixed_addr && !use_urandom) next_addr = fixed_addr_hi;

        c_next = real_orbit_word(next_key, next_addr);

        if (!have_seed) {
            fpf_cur_reg = c_next;
            have_seed = 1;
        } else if (i < n) {
            fpf_process_word(c_cur, c_next);
        }
        c_cur = c_next;
        (void)key; (void)addr_hi;

        if ((i & ((1ULL << 24) - 1)) == 0 && i > 0) {
            double elapsed = now_sec() - t0;
            double frac = (double)i / (double)n;
            fprintf(stderr, "  progress: %.2f%% (%.1fs elapsed, proyeksi total %.1fs)\n",
                    frac * 100.0, elapsed, elapsed / frac);
        }
    }

    double t1 = now_sec();
    printf("order=%s n=%llu waktu=%.1fs (%.1f ns/sample)\n",
           order, (unsigned long long)n, t1 - t0, (t1 - t0) * 1e9 / (double)n);

    uint64_t total;
    double agg_z = fpf_report(&total);
    printf("VERDICT: %s (|agg_z|=%.3f, threshold 6.0 -- konvensi Analisis 4)\n",
           fabs(agg_z) < 6.0 ? "CLEAN" : "ANOMALI", agg_z);
    return 0;
}
