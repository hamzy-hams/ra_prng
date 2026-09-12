// exhaustive_analysis.c -- karakterisasi ekshaustif f(key) untuk ra_core_orbit
// K=1 (ronde-1 saja). Baca HANDOVER.md di folder ini untuk konteks lengkap
// (kenapa analisis ini ada, larangan mengusulkan fix formula baru, dst).
//
// STALE (2026-09-06): f(key) di bawah adalah formula PRA-promosi Kandidat D
// (root cause yang dianalisis file ini). ra_core.c kanonik sudah dipromosikan
// ke Kandidat D (tambah addr_hi, lihat
// ../2026-9-6_orbit-address-widen-64bit-promotion/) -- keyterm_m di sini
// TIDAK lagi merepresentasikan ra_core.c saat ini. Dibiarkan apa adanya
// karena analisis ini soal defect lama (key-repetition), bukan reimplementasi
// yang harus mengikuti formula terbaru; jangan re-derive tanpa alasan baru.
//
// f(key) diturunkan verbatim dari
// ../2026-9-1_family-productionization/ra_core.c (commit be4a14a, tanpa
// perubahan uncommitted saat sesi ini ditulis):
//   - rot32          : ra_core.c L144-147
//   - fmix32         : ra_core.c L149-156
//   - GUARD_M        : ra_core.c L163  (0x6c26fc92u)
//   - ra_init_state_orbit (dipakai hanya utk M[255],M[0..6]): ra_core.c L197-207
//       keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key))
//       M[idx]    = rot32(((uint32_t)idx * 0x06a0dd9bu) * keyterm_m, key ^ (uint32_t)idx)
//     L[] TIDAK dibutuhkan sama sekali untuk output ronde-1 K=1.
//   - ra_permutation_cycle_orbit, ronde i=255 saja (K=1 langsung count<=1
//     lalu break): ra_core.c L217-252
//       o = XOR_{k=0..7} ( M[(uint8_t)(255+k)] << k )   // = M[255],M[0..6]
//       a = cons=key, b=0, c=0, d=0 (state awal)
//       a_new = (d^o) ^ (cons+a) = o ^ (2*key)          // d=0, cons=a=key
//       b_new = (cons+a_new) ^ (o+d) = (key+a_new) ^ o
//       c     = rot32((a_new>>13)^a_new, b_new); c ^= c>>17
//   `c` adalah output aktual (lewat fwrite ke raw_stream) -- BUKAN return
//   value ra_core_orbit() (yang mengembalikan `cons`, tetap `key` untuk
//   rng=1 karena ra_reseed belum sempat jalan). Mode --verify menangkap
//   `c` lewat open_memstream, bukan lewat return value.
//
// FPF-14+6/16 (test PractRand yang FAIL untuk axis ini) diturunkan verbatim
// dari ~/Documents/research/PractRand/src/tests.cpp (FPF::test_blocks,
// L4084-4235) untuk parameter sig_bits=14, exp_bits=6, stride_bits_L2=4
// (stride=16). Dengan parameter ini: max_exp=(1<<6)-1=63, tapi
// `max_exp-1+sig_bits=76>64` sehingga di-clamp max_exp=64+1-14=51 (tests.cpp
// L3943-3944/L3973-3974). max_sig=(1<<14)-1=16383, inv_sig_bits=32-14=18.
// footprint=sig_bits+max_exp=65 -> `max_exp+sig_bits-1=64` TIDAK <=32,
// jadi masuk cabang "large footprint"; stride_bits=16<32 -> "short stride";
// stride_bits==16 persis -> cabang khusus STRIDE_BITS=16 (tests.cpp
// L4188-4235), diporting persis di fpf_process_word() di bawah.
// count_low_zeroes32 (tests.cpp L3905-3913): trailing-zero count, 32 kalau
// input 0 -- ini __builtin_ctz dengan special-case nol.
//
// ra_core.c TIDAK disentuh oleh file ini (murni read-only/karakterisasi).
//
// Compile: lihat BUILD.sh (butuh ra_core_nomain.o hasil kompilasi ra_core.c
// tanpa main(), untuk extern-link ra_core_orbit di mode --verify).

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>

// ---------------------------------------------------------------------
// f(key): verbatim derivation, lihat comment block atas.
// ---------------------------------------------------------------------
static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

static inline uint32_t fmix32(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6bu;
    h ^= h >> 13;
    h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}

#define GUARD_M 0x6c26fc92u

static inline uint32_t f_key(uint32_t key) {
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    static const int idxs[8] = {255, 0, 1, 2, 3, 4, 5, 6};
    uint32_t o = 0;
    for (int k = 0; k < 8; ++k) {
        uint32_t idx = (uint32_t)idxs[k];
        uint32_t r = key ^ idx;
        uint32_t m_val = (idx * 0x06a0dd9bu) * keyterm_m;
        uint32_t m = rot32(m_val, r);
        o ^= (m << k);
    }
    uint32_t a = o ^ (2u * key);
    uint32_t b = (key + a) ^ o;
    uint32_t c = rot32((a >> 13) ^ a, b);
    c ^= c >> 17u;
    return c;
}

// ---------------------------------------------------------------------
// splitmix64 utility (untuk sampling key acak reproducible)
// ---------------------------------------------------------------------
static inline uint64_t splitmix64_next(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97f4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}
static inline uint32_t splitmix32_next(uint64_t *state) {
    return (uint32_t)splitmix64_next(state);
}

static double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

// =======================================================================
// Mode --verify: byte-exact check terhadap ra_core_orbit produksi asli
// =======================================================================
extern uint32_t ra_core_orbit(uint32_t key, size_t rng, FILE *raw_stream);

static int mode_verify(uint64_t n) {
    uint64_t state = 0x123456789ABCDEF0ULL;
    uint64_t mismatches = 0;
    for (uint64_t i = 0; i < n; ++i) {
        uint32_t key = splitmix32_next(&state);
        uint32_t expected = f_key(key);

        char *buf = NULL;
        size_t sz = 0;
        FILE *mem = open_memstream(&buf, &sz);
        ra_core_orbit(key, 1, mem);
        fclose(mem);

        if (sz != 4) {
            fprintf(stderr, "key=0x%08x: unexpected stream size %zu (expected 4)\n", key, sz);
            mismatches++;
            free(buf);
            continue;
        }
        uint32_t actual;
        memcpy(&actual, buf, 4);
        free(buf);

        if (actual != expected) {
            mismatches++;
            if (mismatches <= 20) {
                fprintf(stderr, "MISMATCH key=0x%08x expected=0x%08x actual=0x%08x\n",
                        key, expected, actual);
            }
        }
    }
    printf("verify: n=%llu mismatches=%llu -> %s\n",
           (unsigned long long)n, (unsigned long long)mismatches,
           mismatches == 0 ? "PASS" : "FAIL");
    return mismatches == 0 ? 0 : 1;
}

// =======================================================================
// Analisis 1: sweep + 2-bit saturating counter (bijectivity/multiplicity)
// =======================================================================
// 1 GiB: 2^32 slot, 2 bit/slot, 4 slot/byte -> 2^30 byte.
static uint8_t *counter1 = NULL;

static inline void counter1_bump(uint32_t val) {
    uint32_t byte_idx = val >> 2;
    uint32_t shift = (val & 3u) * 2u;
    uint8_t cur = (counter1[byte_idx] >> shift) & 3u;
    if (cur < 3u) {
        counter1[byte_idx] = (uint8_t)(counter1[byte_idx] + ((uint8_t)1u << shift));
    }
}

static void counter1_report(uint64_t domain_n) {
    uint64_t bucket[4] = {0, 0, 0, 0};
    for (uint64_t byte_idx = 0; byte_idx < (1ULL << 30); ++byte_idx) {
        uint8_t byte = counter1[byte_idx];
        bucket[byte & 3u]++;
        bucket[(byte >> 2) & 3u]++;
        bucket[(byte >> 4) & 3u]++;
        bucket[(byte >> 6) & 3u]++;
    }
    double n = (double)domain_n; // #trials into 2^32 bins (lambda=1 kalau domain=2^32)
    double lambda = n / 4294967296.0;
    double p0 = exp(-lambda);
    double p1 = lambda * p0;
    double p2 = lambda * lambda / 2.0 * p0;
    double p3plus = 1.0 - p0 - p1 - p2;
    double exp_cnt[4] = {
        p0 * 4294967296.0, p1 * 4294967296.0, p2 * 4294967296.0, p3plus * 4294967296.0
    };
    printf("\n=== Analisis 1: distribusi multiplicity (2-bit counter, domain_n=%llu) ===\n",
           (unsigned long long)domain_n);
    const char *labels[4] = {"count=0 (never hit)", "count=1", "count=2", "count>=3 (saturated)"};
    for (int i = 0; i < 4; ++i) {
        double z = (bucket[i] - exp_cnt[i]) / sqrt(exp_cnt[i]);
        printf("  %-24s observed=%15llu expected=%18.1f z=%+.3f\n",
               labels[i], (unsigned long long)bucket[i], exp_cnt[i], z);
    }
    uint64_t hit_at_least_once = bucket[1] + bucket[2] + bucket[3];
    double exp_hit = (1.0 - p0) * 4294967296.0;
    double var_hit = 4294967296.0 * p0 * (1.0 - p0); // aproksimasi binomial per-bin
    double z_hit = (hit_at_least_once - exp_hit) / sqrt(var_hit);
    printf("  distinct values hit >=1x: observed=%llu expected~=%.1f z~=%+.3f\n",
           (unsigned long long)hit_at_least_once, exp_hit, z_hit);
    printf("  (z dihitung per-bucket independen, aproksimasi Poisson/binomial -- lihat RESULTS.md untuk interpretasi)\n");
}

// =======================================================================
// Analisis 2: FPF-14+6/16 histogram (exp,sig) -- porting persis tests.cpp
// =======================================================================
#define FPF_SIG_BITS 14u
#define FPF_MAX_SIG 16383u
#define FPF_MAX_EXP 51u
#define FPF_INV_SIG_BITS 18u
#define FPF_NUM_EXP (FPF_MAX_EXP + 1u)      // 52
#define FPF_TOTAL_SIZE (FPF_NUM_EXP * (FPF_MAX_SIG + 1u)) // 851968

static uint64_t *fpf_counts = NULL;
static uint32_t fpf_cur_reg = 0;

static inline uint32_t count_low_zeroes32(uint32_t v) {
    return v ? (uint32_t)__builtin_ctz(v) : 32u;
}

static inline void fpf_bump(uint32_t e, uint32_t sig) {
    fpf_counts[(uint64_t)e * (FPF_MAX_SIG + 1u) + sig]++;
}

// Porting persis tests.cpp L4188-4235 (STRIDE_BITS=16 branch), diaplikasikan
// per "word" berurutan (word = f(key) untuk key berurutan). `next_word` =
// f(key+1), dibutuhkan hanya saat cabang e>=inv_sig_bits (jarang, p~2^-18).
static inline void fpf_process_word(uint32_t word, uint32_t next_word) {
    uint32_t cur = fpf_cur_reg;
    uint32_t w = word;
    uint32_t e, sig;

    // --- first half-word ---
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

    // --- second half-word ---
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

static void fpf_report(void) {
    uint64_t total = 0;
    uint64_t obs_e[FPF_NUM_EXP];
    for (uint32_t e = 0; e < FPF_NUM_EXP; ++e) {
        uint64_t s = 0;
        for (uint32_t sig = 0; sig <= FPF_MAX_SIG; ++sig) s += fpf_counts[(uint64_t)e * (FPF_MAX_SIG + 1u) + sig];
        obs_e[e] = s;
        total += s;
    }
    printf("\n=== Analisis 2: FPF-14+6/16 histogram (total_samples=%llu) ===\n",
           (unsigned long long)total);
    printf("  -- per-exponen (inter/cross) vs ekspektasi geometrik P(e)=2^-(e+1), e=max clamp 2^-max_exp --\n");
    for (uint32_t e = 0; e < FPF_NUM_EXP; ++e) {
        double p = (e < FPF_MAX_EXP) ? pow(0.5, (double)e + 1.0) : pow(0.5, (double)FPF_MAX_EXP);
        double expc = p * (double)total;
        if (expc < 1e-6) continue;
        double z = (obs_e[e] - expc) / sqrt(expc);
        if (fabs(z) > 4.0 || e < 6 || e == FPF_MAX_EXP) {
            printf("  e=%2u observed=%12llu expected=%14.2f z=%+8.3f%s\n",
                   e, (unsigned long long)obs_e[e], expc, z, fabs(z) > 6.0 ? "  <-- ANOMALI" : "");
        }
    }

    printf("  -- top 15 bin (e,sig) full-resolution paling menyimpang (min expected>=20) --\n");
    typedef struct { double z; uint32_t e, sig; uint64_t obs; double expc; } worst_t;
    worst_t worst[15];
    int nworst = 0;
    for (uint32_t e = 0; e < FPF_NUM_EXP && e < 30; ++e) { // e>30 -> expected/bin sudah <20 pasti
        double p_e = (e < FPF_MAX_EXP) ? pow(0.5, (double)e + 1.0) : pow(0.5, (double)FPF_MAX_EXP);
        double expc_bin = p_e * (double)total / (double)(FPF_MAX_SIG + 1u);
        if (expc_bin < 20.0) continue;
        for (uint32_t sig = 0; sig <= FPF_MAX_SIG; ++sig) {
            uint64_t obs = fpf_counts[(uint64_t)e * (FPF_MAX_SIG + 1u) + sig];
            double z = (obs - expc_bin) / sqrt(expc_bin);
            double az = fabs(z);
            if (nworst < 15) {
                worst[nworst++] = (worst_t){z, e, sig, obs, expc_bin};
                if (nworst == 15) {
                    // sort ascending by |z| supaya gampang evict yang terkecil
                    for (int a = 0; a < 15; a++) for (int b = a+1; b < 15; b++)
                        if (fabs(worst[a].z) > fabs(worst[b].z)) { worst_t t=worst[a]; worst[a]=worst[b]; worst[b]=t; }
                }
            } else if (az > fabs(worst[0].z)) {
                worst[0] = (worst_t){z, e, sig, obs, expc_bin};
                for (int a = 0; a < 14; a++) if (fabs(worst[a].z) > fabs(worst[a+1].z)) { worst_t t=worst[a]; worst[a]=worst[a+1]; worst[a+1]=t; }
            }
        }
    }
    for (int a = 0; a < 15; a++) for (int b = a+1; b < 15; b++)
        if (fabs(worst[a].z) < fabs(worst[b].z)) { worst_t t=worst[a]; worst[a]=worst[b]; worst[b]=t; }
    for (int i = 0; i < nworst; i++) {
        printf("  e=%2u sig=%5u observed=%10llu expected=%12.2f z=%+8.3f\n",
               worst[i].e, worst[i].sig, (unsigned long long)worst[i].obs, worst[i].expc, worst[i].z);
    }

    // Agregat chi-square intra-eksponen (full resolution, sig vs uniform)
    // dijumlahkan lintas eksponen yang punya cukup sampel -- ini lebih
    // dekat ke semangat statistik ":all" PractRand (jumlah banyak bias
    // kecil di banyak bin bisa signifikan agregat walau tiap bin sendiri
    // tidak ekstrem), bukan sekadar mencari bin tunggal terburuk di atas.
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
            chi2_e += d*d/expc_bin;
        }
        double z_e = (chi2_e - (double)FPF_MAX_SIG) / sqrt(2.0 * (double)FPF_MAX_SIG);
        printf("  intra e=%2u chi2=%10.1f df=%5u z=%+8.3f%s\n", e, chi2_e, FPF_MAX_SIG, z_e,
               fabs(z_e) > 4.0 ? "  <-- kontributor" : "");
        agg_chi2 += chi2_e;
        agg_df += FPF_MAX_SIG; // df = num_bins-1 = 16383
    }
    double agg_z = (agg_df > 0) ? (agg_chi2 - (double)agg_df) / sqrt(2.0 * (double)agg_df) : 0.0;
    printf("  -- agregat chi-square intra-eksponen (full-res, e dgn expected/bin>=20) --\n");
    printf("  agg_chi2=%.1f agg_df=%llu agg_z=%+.3f\n", agg_chi2, (unsigned long long)agg_df, agg_z);
}

// =======================================================================
// Analisis 3: korelasi Weyl vs kontrol random-delta (contingency 256x256)
// =======================================================================
static uint64_t *ct_weyl = NULL;   // 256*256
static uint64_t *ct_ctrl = NULL;   // 256*256
#define WEYL_DELTA 0x9E3779B9u

static inline void ct_bump(uint64_t *table, uint32_t a, uint32_t b) {
    table[((a >> 24) << 8) | (b >> 24)]++;
}

static double chi2_independence(const uint64_t *table, uint64_t total) {
    uint64_t row_sum[256] = {0}, col_sum[256] = {0};
    for (int r = 0; r < 256; r++) for (int c = 0; c < 256; c++) {
        row_sum[r] += table[r*256+c];
        col_sum[c] += table[r*256+c];
    }
    double chi2 = 0.0;
    for (int r = 0; r < 256; r++) for (int c = 0; c < 256; c++) {
        double expc = (double)row_sum[r] * (double)col_sum[c] / (double)total;
        if (expc < 1e-9) continue;
        double d = (double)table[r*256+c] - expc;
        chi2 += d*d/expc;
    }
    return chi2;
}

static void weyl_report(uint64_t n) {
    double dfree = 255.0 * 255.0; // df kontingensi 256x256 independensi
    double chi2_w = chi2_independence(ct_weyl, n);
    double chi2_c = chi2_independence(ct_ctrl, n);
    double z_w = (chi2_w - dfree) / sqrt(2.0 * dfree);
    double z_c = (chi2_c - dfree) / sqrt(2.0 * dfree);
    printf("\n=== Analisis 3: korelasi (top-8-bit f(key), top-8-bit f(key+delta)) ===\n");
    printf("  n_pairs=%llu, df=%.0f\n", (unsigned long long)n, dfree);
    printf("  Weyl delta=0x%08x     : chi2=%.2f  z=%+.3f\n", WEYL_DELTA, chi2_w, z_w);
    printf("  kontrol random delta   : chi2=%.2f  z=%+.3f\n", chi2_c, z_c);
    printf("  (z besar & positif = bukti korelasi/dependensi menyimpang dari independen uniform)\n");
}

// =======================================================================
// Mode --timing-sample
// =======================================================================
static void mode_timing_sample(uint64_t n) {
    volatile uint32_t sink = 0;
    double t0 = now_sec();
    for (uint64_t i = 0; i < n; ++i) sink ^= f_key((uint32_t)i);
    double t1 = now_sec();
    double per_call = (t1 - t0) / (double)n;
    printf("timing-sample: n=%llu time=%.4fs (%.2f ns/call, sink=0x%08x)\n",
           (unsigned long long)n, t1 - t0, per_call * 1e9, sink);
    double full_domain = 4294967296.0;
    // full-run per iterasi: ~1 f_key baru (c_next) + 1 f_key (c2 weyl) + 1 f_key (c3 ctrl)
    // (c_cur diamortisasi dari c_next iterasi sebelumnya) = ~3 f_key call/iterasi.
    double proj_full = full_domain * 3.0 * per_call;
    printf("proyeksi --full-run (3 f_key/iterasi x 2^32 iterasi): %.1f s (%.2f menit)\n",
           proj_full, proj_full / 60.0);
}

// =======================================================================
// Mode --full-run / --sample
// =======================================================================
static uint32_t g_random_delta = 0;

static void init_random_delta(void) {
    FILE *ur = fopen("/dev/urandom", "rb");
    if (ur) {
        if (fread(&g_random_delta, 4, 1, ur) != 1) g_random_delta = 0xC2FE5E7Bu;
        fclose(ur);
    } else {
        g_random_delta = 0xC2FE5E7Bu;
    }
    if (g_random_delta == 0) g_random_delta = 0xC2FE5E7Bu;
    printf("delta_random (kontrol) = 0x%08x (dipilih acak sekali di awal run ini)\n", g_random_delta);
}

static void alloc_all(void) {
    counter1 = calloc(1ULL << 30, 1);
    fpf_counts = calloc(FPF_TOTAL_SIZE, sizeof(uint64_t));
    ct_weyl = calloc(65536, sizeof(uint64_t));
    ct_ctrl = calloc(65536, sizeof(uint64_t));
    if (!counter1 || !fpf_counts || !ct_weyl || !ct_ctrl) {
        fprintf(stderr, "alokasi memori gagal\n");
        exit(1);
    }
}

// sequential=1: key = 0..n-1 berurutan (dipakai --full-run, n=2^32).
// sequential=0: key = sampel acak-seragam sebanyak n (dipakai --sample).
static void run_analysis(uint64_t n, int sequential) {
    alloc_all();
    init_random_delta();

    double t0 = now_sec();
    uint64_t state = 0xF00DCAFEBABEULL;

    uint32_t key0 = sequential ? 0u : splitmix32_next(&state);
    uint32_t c_next = f_key(key0);
    uint32_t key_prev = key0;
    int have_fpf_seed = 0;

    uint64_t weyl_pairs = 0;

    for (uint64_t i = 0; i < n; ++i) {
        uint32_t key = key_prev;
        uint32_t c_cur = c_next;

        // Analisis 1
        counter1_bump(c_cur);

        uint32_t key_after = sequential ? (uint32_t)(i + 1) : splitmix32_next(&state);
        c_next = f_key(key_after);

        // Analisis 2 (hanya bermakna penuh utk stream sequential; tetap
        // dijalankan di mode --sample sbg batas bawah kasar, dicatat di
        // RESULTS.md sbg keterbatasan)
        if (!have_fpf_seed) {
            fpf_cur_reg = c_cur;
            have_fpf_seed = 1;
        } else if (i + 1 < n) {
            fpf_process_word(c_cur, c_next);
        }

        // Analisis 3
        uint32_t c2 = f_key(key + WEYL_DELTA);
        uint32_t c3 = f_key(key + g_random_delta);
        ct_bump(ct_weyl, c_cur, c2);
        weyl_pairs++;
        ct_bump(ct_ctrl, c_cur, c3);

        key_prev = key_after;

        if ((i & ((1ULL << 27) - 1)) == 0 && i > 0) {
            double elapsed = now_sec() - t0;
            double frac = (double)i / (double)n;
            fprintf(stderr, "  progress: %.2f%% (%.1fs elapsed, proyeksi total %.1fs)\n",
                    frac * 100.0, elapsed, elapsed / frac);
        }
    }

    double t1 = now_sec();
    printf("run selesai: n=%llu sequential=%d waktu=%.1fs\n",
           (unsigned long long)n, sequential, t1 - t0);

    counter1_report(n);
    fpf_report();
    weyl_report(weyl_pairs);
}

// =======================================================================
// Mode --fpf-order: koreksi cakupan -- jalankan FPF-binning pada stream
// key SESUAI urutan yang benar-benar dipakai axis PractRand yang FAIL/CLEAN
// (hashed-splitmix32-chain vs Weyl-chain), bukan cuma integer berurutan
// (yang merupakan urutan ke-4 yang belum pernah diuji PractRand sama
// sekali). Konvensi splitmix32 disamakan PERSIS dengan
// ../2026-9-4_orbit-kmin-battery/diag_orbit_k1_hashed.c (ambil UPPER 32
// bit `z`, bukan lower) supaya stream identik dengan yang dipakai run
// PractRand asli.
static uint32_t splitmix32_hi(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    z = z ^ (z >> 31);
    return (uint32_t)(z >> 32);
}

// Hipotesis lanjutan (muncul dari hasil hashed vs weyl vs sequential):
// mungkin bukan soal Weyl-vs-hashed spesifik, tapi soal KEY-INDEPENDEN
// berturutan (splitmix32-chain) vs KEY-BERHUBUNGAN-ARITMETIK-TETAP
// (step konstan berapa pun, termasuk step=1 sequential). Dua order
// tambahan untuk menguji ini murah (beberapa detik): "step:<hex>" (step
// aritmetik konstan sembarang, BUKAN 1 atau Weyl) dan "fmixhash" (key_i =
// fmix32((uint32_t)i) -- independen-terlihat tapi BUKAN splitmix32, buat
// pastikan efeknya bukan spesifik ke splitmix32).

// Buffered /dev/urandom reader -- JANGAN fread 4 byte per key (terlalu
// banyak syscall untuk ratusan juta key), baca blok besar sekali lalu
// konsumsi dari buffer.
#define URANDOM_BUF_BYTES (1u << 20)
static FILE *g_urandom_fp = NULL;
static uint8_t g_urandom_buf[URANDOM_BUF_BYTES];
static size_t g_urandom_pos = URANDOM_BUF_BYTES;

static uint32_t gen_urandom_key(void) {
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

// Uji hipotesis "key berulang -> f(key) berulang -> bias FPF" (2026-09-05
// lanjutan, setelah /dev/urandom terbukti FAIL juga -- jadi bukan spesifik
// SplitMix32, tapi mungkin soal draw "dengan pengembalian" vs permutasi
// tanpa pengulangan). Bitmap "seen" 1 bit/kemungkinan 32-bit key (512MiB) --
// kalau draw kena key yang sudah pernah dipakai di run ini, buang dan draw
// lagi (advance generator) sampai dapat key baru. dedup-hashed/dedup-urandom
// pakai generator yang sama persis dengan hashed/urandom, cuma dijamin
// tanpa pengulangan key dalam satu run.
static uint8_t *g_seen_bitmap = NULL;
static uint64_t g_redraw_attempts = 0;

static inline int seen_test_and_set(uint32_t val) {
    uint32_t byte_idx = val >> 3;
    uint8_t mask = (uint8_t)(1u << (val & 7u));
    int was_set = (g_seen_bitmap[byte_idx] & mask) != 0;
    g_seen_bitmap[byte_idx] |= mask;
    return was_set;
}

static void mode_fpf_order(const char *order, uint64_t n) {
    fpf_counts = calloc(FPF_TOTAL_SIZE, sizeof(uint64_t));
    if (!fpf_counts) { fprintf(stderr, "alokasi fpf_counts gagal\n"); exit(1); }
    fpf_cur_reg = 0;

    const char *seed_env = getenv("FPF_SM_SEED");
    uint64_t sm_state = seed_env ? strtoull(seed_env, NULL, 0) : 0xC0FFEE1234ULL;
    uint32_t key = 0;
    int use_weyl = strcmp(order, "weyl") == 0;
    int use_hashed = strcmp(order, "hashed") == 0;
    int use_sequential = strcmp(order, "sequential") == 0;
    int use_fmixhash = strcmp(order, "fmixhash") == 0;
    int use_urandom = strcmp(order, "urandom") == 0;
    int use_dedup_hashed = strcmp(order, "dedup-hashed") == 0;
    int use_dedup_urandom = strcmp(order, "dedup-urandom") == 0;
    int is_dedup = use_dedup_hashed || use_dedup_urandom;
    uint32_t step_const = 0;
    int use_step = 0;
    if (strncmp(order, "step:", 5) == 0) {
        use_step = 1;
        step_const = (uint32_t)strtoul(order + 5, NULL, 0);
    }
    if (!use_weyl && !use_hashed && !use_sequential && !use_fmixhash && !use_step &&
        !use_urandom && !use_dedup_hashed && !use_dedup_urandom) {
        fprintf(stderr, "order tidak dikenal: %s (pakai sequential|weyl|hashed|fmixhash|urandom|"
                         "dedup-hashed|dedup-urandom|step:<hex>)\n", order);
        exit(2);
    }
    if (is_dedup) {
        g_seen_bitmap = calloc(1ULL << 29, 1); // 512MiB, 1 bit/kemungkinan key
        if (!g_seen_bitmap) { fprintf(stderr, "alokasi seen_bitmap gagal\n"); exit(1); }
    }

    // raw_next(): draw satu key mentah dari generator dasar (splitmix32-hi
    // untuk *hashed, urandom untuk *urandom). Untuk order dedup, dibungkus
    // loop redraw sampai key belum pernah dipakai.
    #define RAW_NEXT_HASHED() splitmix32_hi(&sm_state)
    #define RAW_NEXT_URANDOM() gen_urandom_key()

    uint32_t c_next;
    if (use_weyl) c_next = f_key(key);
    else if (use_hashed) { key = RAW_NEXT_HASHED(); c_next = f_key(key); }
    else if (use_urandom) { key = RAW_NEXT_URANDOM(); c_next = f_key(key); }
    else if (use_dedup_hashed) {
        do { key = RAW_NEXT_HASHED(); if (seen_test_and_set(key)) g_redraw_attempts++; else break; } while (1);
        c_next = f_key(key);
    } else if (use_dedup_urandom) {
        do { key = RAW_NEXT_URANDOM(); if (seen_test_and_set(key)) g_redraw_attempts++; else break; } while (1);
        c_next = f_key(key);
    } else if (use_fmixhash) { key = fmix32(0); c_next = f_key(key); }
    else if (use_step) c_next = f_key(key);
    else c_next = f_key(0);

    for (uint64_t i = 0; i < n; ++i) {
        uint32_t c_cur = c_next;
        uint32_t next_key;
        if (use_weyl) next_key = key + WEYL_DELTA;
        else if (use_hashed) next_key = RAW_NEXT_HASHED();
        else if (use_urandom) next_key = RAW_NEXT_URANDOM();
        else if (use_dedup_hashed) {
            do { next_key = RAW_NEXT_HASHED(); if (seen_test_and_set(next_key)) g_redraw_attempts++; else break; } while (1);
        } else if (use_dedup_urandom) {
            do { next_key = RAW_NEXT_URANDOM(); if (seen_test_and_set(next_key)) g_redraw_attempts++; else break; } while (1);
        } else if (use_fmixhash) next_key = fmix32((uint32_t)(i + 1));
        else if (use_step) next_key = key + step_const;
        else next_key = (uint32_t)(i + 1);
        c_next = f_key(next_key);

        if (i == 0) { fpf_cur_reg = c_cur; }
        else if (i + 1 < n) { fpf_process_word(c_cur, c_next); }

        key = next_key;
    }
    if (is_dedup) {
        printf("dedup: n=%llu redraw_attempts=%llu (key unik terpakai=%llu, harus==n)\n",
               (unsigned long long)n, (unsigned long long)g_redraw_attempts, (unsigned long long)n);
    }
    printf("=== --fpf-order %s, n=%llu ===\n", order, (unsigned long long)n);
    fpf_report();
}

// =======================================================================
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "usage:\n"
            "  %s --verify N\n"
            "  %s --timing-sample [N]\n"
            "  %s --full-run\n"
            "  %s --sample N\n", argv[0], argv[0], argv[0], argv[0]);
        return 2;
    }
    if (strcmp(argv[1], "--verify") == 0) {
        uint64_t n = argc > 2 ? strtoull(argv[2], NULL, 10) : 1000000ULL;
        return mode_verify(n);
    } else if (strcmp(argv[1], "--timing-sample") == 0) {
        uint64_t n = argc > 2 ? strtoull(argv[2], NULL, 10) : 100000000ULL;
        mode_timing_sample(n);
        return 0;
    } else if (strcmp(argv[1], "--full-run") == 0) {
        run_analysis(4294967296ULL, 1);
        return 0;
    } else if (strcmp(argv[1], "--sample") == 0) {
        if (argc < 3) { fprintf(stderr, "butuh N untuk --sample\n"); return 2; }
        uint64_t n = strtoull(argv[2], NULL, 10);
        run_analysis(n, 0);
        return 0;
    } else if (strcmp(argv[1], "--fpf-order") == 0) {
        if (argc < 4) { fprintf(stderr, "usage: %s --fpf-order {sequential|weyl|hashed} N\n", argv[0]); return 2; }
        uint64_t n = strtoull(argv[3], NULL, 10);
        mode_fpf_order(argv[2], n);
        return 0;
    }
    fprintf(stderr, "mode tidak dikenal: %s\n", argv[1]);
    return 2;
}
