// ra_core_param.c
// HANDOVER.md Tahap A (4.1): generalisasi mekanis src/ra_core/c/ra_core.c ke
// state-size N yang bisa dipilih compile-time (RA_N: 256/128/64/32/16),
// "per-half" (total state = 2*RA_N, persis analog M+L 256+256 sekarang),
// plus mode reseed (RA_RESEED_MODE: LOWONLY = produksi saat ini, MUTUAL =
// mutual-XOR kedua half, kandidat baru).
//
// Kesetiaan formula (HANDOVER.md #2.3): TIDAK ada retuning apa pun di sini.
// Setiap literal terkait ukuran (256, 255, cast (uint8_t)) di ra_core.c
// diganti MEKANIS dengan turunan RA_N (RA_MASK=RA_N-1, RA_N-1 sbg periode
// reseed, dst). Konstanta non-ukuran (0x9e3779b7, GUARD_*, dll) TIDAK
// disentuh. Kegagalan kualitas di N kecil adalah TEMUAN, bukan alasan
// retuning sekarang.
//
// Dua cara pakai:
//   1. Standalone (RA_CORE_PARAM_NO_MAIN tidak didefinisikan): file ini
//      punya main() sendiri (checksum-gen/--stream), dipakai utk sanity
//      check byte-identik vs src/ra_core/c/ra_core.c dan KAT capture Tier0.
//   2. Sebagai library utk tools/battery/driver_main_template.c (convention
//      A, CORE_FN): definisikan RA_CORE_PARAM_NO_MAIN sebelum #include ini,
//      lalu #define CORE_FN ra_core_orbit (atau ra_core_singleblock),
//      lalu #include driver_main_template.c. Lihat battery_wrapper_orbit.c
//      / battery_wrapper_singleblock.c di direktori ini.
//
// Signature 4-argumen (key, addr_hi, rng, raw_stream) dipertahankan persis
// supaya kompatibel dengan kontrak driver_main_template.c yang sudah ada.
//
// Copyright (c) 2025 Hamas A. Rahman (derivative research variant)
// Licensed under CC BY-NC-SA 4.0, matching the original this derives from.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdalign.h>

#ifndef RA_N
#define RA_N 256
#endif

#if (RA_N & (RA_N - 1)) != 0
#error "RA_N must be a power of two"
#endif
#if RA_N < 8
#error "RA_N must be >= 8 (ra_hash assumes 8 groups)"
#endif

#define RA_MASK ((uint32_t)(RA_N - 1))
#define RA_HASH_GROUPS 8
#define RA_HASH_GROUPLEN (RA_N / RA_HASH_GROUPS)

#define RESEED_LOWONLY 0
#define RESEED_MUTUAL 1
#ifndef RA_RESEED_MODE
#define RA_RESEED_MODE RESEED_LOWONLY
#endif

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

// key=0 / addr_hi weak-input guards -- verbatim from ra_core.c, size-
// independent, NOT part of this parameterization.
#define GUARD_L 0x38916df4u
#define GUARD_M 0x6c26fc92u
#define GUARD_L2 0x3eb13b90u
#define GUARD_M2 0x392456deu

// hash_access=sequential, HASH_SELFIDX off, HASH_DEPTH = RA_N/8 (was fixed
// 32 for RA_N=256).
static void ra_hash(const uint32_t *K, uint32_t *out8) {
    for (int i = 0; i < RA_HASH_GROUPS; ++i) {
        out8[i] = 0;
        int base = i * RA_HASH_GROUPLEN;
        for (int j = 0; j < RA_HASH_GROUPLEN; ++j) out8[i] ^= K[base + j];
    }
}

// Reseed mode LOWONLY: persis ra_core.c produksi (M/L dipetakan ke
// K_low/K_high), K_high TIDAK disentuh langsung di sini (hanya berubah
// lewat swap K_high[i]<->K_high[d] di siklus permutasi orbit).
// Reseed mode MUTUAL (HANDOVER.md #2.2): tambahan K_high[i] ^= K_low_lama[i]
// sekaligus, jadi kedua half berubah.
static uint32_t ra_reseed(uint32_t *K_low, uint32_t *K_high) {
#if RA_RESEED_MODE == RESEED_MUTUAL
    for (int i = 0; i < RA_N; ++i) {
        uint32_t old_low = K_low[i];
        K_low[i] ^= K_high[i];
        K_high[i] ^= old_low;
    }
#else
    for (int i = 0; i < RA_N; ++i) K_low[i] ^= K_high[i];
#endif

    uint32_t tmp8[RA_HASH_GROUPS];
    ra_hash(K_low, tmp8);

    uint32_t new_cons = 0;
    for (int e = 0; e < RA_HASH_GROUPS; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

// ----------------------------------------------------------------------
// Mode orbit: addressable, unbounded stream, reseeds every (RA_N-1) words.
// ----------------------------------------------------------------------

// addr_hi deliberately EXCLUDED from rotation r=key^i (see ra_core.c header
// comment -- folding it into rotation failed PractRand Gap-16). Preserved
// verbatim here.
static void ra_init_state_orbit(uint32_t *L, uint32_t *M, uint32_t key,
                                 uint32_t addr_hi) {
    uint32_t keyterm_l_key   = fmix32(GUARD_L  ^ (0x9e3779b7u * key));
    uint32_t keyterm_l_param = fmix32(GUARD_L2 ^ (0x85ebca77u * addr_hi));
    uint32_t keyterm_l = keyterm_l_key * keyterm_l_param;

    uint32_t keyterm_m_key   = fmix32(GUARD_M  ^ (0x06a0dd9bu * key));
    uint32_t keyterm_m_param = fmix32(GUARD_M2 ^ (0x9e3779b1u * addr_hi));
    uint32_t keyterm_m = keyterm_m_key * keyterm_m_param;

    for (int i = 0; i < RA_N; ++i) {
        uint32_t r = key ^ (uint32_t)i; // addr_hi deliberately excluded
        uint32_t l_val = (uint32_t)(i * 0x9e3779b7u) * keyterm_l;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

// One full (RA_N-1)-step permutation cycle, orbit mode. Wide 8-tap `o` +
// XORSHIFT(17) finalizer + rolling-o register: mekanis sama dengan
// ra_core.c, literal 255/(uint8_t) diganti (RA_N-1)/RA_MASK.
static void ra_permutation_cycle_orbit(uint32_t cons, size_t it,
                                        const uint32_t *M, uint32_t *L,
                                        uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    uint32_t o = (M[(RA_N - 1 + 0) & RA_MASK] << 0) ^ (M[(RA_N - 1 + 1) & RA_MASK] << 1) ^
                 (M[(RA_N - 1 + 2) & RA_MASK] << 2) ^ (M[(RA_N - 1 + 3) & RA_MASK] << 3) ^
                 (M[(RA_N - 1 + 4) & RA_MASK] << 4) ^ (M[(RA_N - 1 + 5) & RA_MASK] << 5) ^
                 (M[(RA_N - 1 + 6) & RA_MASK] << 6) ^ (M[(RA_N - 1 + 7) & RA_MASK] << 7);
    uint32_t o_term7 = M[(RA_N - 1 + 7) & RA_MASK] << 7;

    for (uint32_t i = RA_N - 1; i > 0; --i) {
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 17u;

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & RA_MASK;

        if (*count <= 1) break;
        --(*count);

        uint32_t tmp = L[i];
        L[i] = L[d];
        L[d] = tmp;

        if (i > 1) {
            uint32_t new_term = M[(i - 1) & RA_MASK];
            uint32_t o_next = new_term ^ ((o ^ o_term7) << 1);
            o_term7 = M[(i + 6) & RA_MASK] << 7;
            o = o_next;
        }
    }
}

uint32_t ra_core_orbit(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[RA_N], M[RA_N];
    uint64_t count = rng;
    uint64_t iteration = rng / (RA_N - 1) + 1;

    uint32_t cons = key;
    ra_init_state_orbit(L, M, key, addr_hi);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_orbit(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

// ----------------------------------------------------------------------
// Mode singleblock: no-L fast path, valid ONLY for rng in [1, RA_N-1].
// ----------------------------------------------------------------------

static void ra_init_state_singleblock(uint32_t *M, uint32_t key,
                                       uint32_t addr_hi, size_t rng) {
    uint32_t keyterm_m_key   = fmix32(GUARD_M  ^ (0x06a0dd9bu * key));
    uint32_t keyterm_m_param = fmix32(GUARD_M2 ^ (0x9e3779b1u * addr_hi));
    uint32_t keyterm_m = keyterm_m_key * keyterm_m_param;
    int len = ((int)rng + 7 > RA_N) ? RA_N : (int)(rng + 7);
    int start = (RA_N - (int)rng) & (int)RA_MASK;
    int end = start + len; // may exceed RA_N (wraps past index RA_N-1)

    int first_end = (end <= RA_N) ? end : RA_N;
    for (int i = start; i < first_end; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32(m_val, r);
    }
    if (end > RA_N) {
        for (int i = 0; i < end - RA_N; ++i) {
            uint32_t r = key ^ (uint32_t)i;
            uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
            M[i] = rot32(m_val, r);
        }
    }
}

static void ra_permutation_cycle_singleblock(uint32_t cons, size_t it,
                                              const uint32_t *M,
                                              uint64_t *count,
                                              FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    uint32_t o = (M[(RA_N - 1 + 0) & RA_MASK] << 0) ^ (M[(RA_N - 1 + 1) & RA_MASK] << 1) ^
                 (M[(RA_N - 1 + 2) & RA_MASK] << 2) ^ (M[(RA_N - 1 + 3) & RA_MASK] << 3) ^
                 (M[(RA_N - 1 + 4) & RA_MASK] << 4) ^ (M[(RA_N - 1 + 5) & RA_MASK] << 5) ^
                 (M[(RA_N - 1 + 6) & RA_MASK] << 6) ^ (M[(RA_N - 1 + 7) & RA_MASK] << 7);
    uint32_t o_term7 = M[(RA_N - 1 + 7) & RA_MASK] << 7;

    for (uint32_t i = RA_N - 1; i > 0; --i) {
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 17u;

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & RA_MASK;

        if (*count <= 1) break;
        --(*count);
        // No L[] swap here (matches ra_core.c singleblock).

        if (i > 1) {
            uint32_t new_term = M[(i - 1) & RA_MASK];
            uint32_t o_next = new_term ^ ((o ^ o_term7) << 1);
            o_term7 = M[(i + 6) & RA_MASK] << 7;
            o = o_next;
        }
    }
}

uint32_t ra_core_singleblock(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > (size_t)(RA_N - 1)) {
        fprintf(stderr,
            "ra_core_singleblock: rng=%zu > %d out of scope (no-reseed "
            "fast path valid only for rng in [1,%d]); use ra_core_orbit.\n",
            rng, RA_N - 1, RA_N - 1);
        abort();
    }

    alignas(64) uint32_t M[RA_N];
    uint64_t count = rng;
    uint32_t cons = key;

    ra_init_state_singleblock(M, key, addr_hi, rng);
    ra_permutation_cycle_singleblock(cons, /*it=*/0, M, &count, raw_stream);
    return cons;
}

// ----------------------------------------------------------------------
// Standalone CLI (checksum-gen / --stream) -- dipakai utk sanity check byte-
// identik vs ra_core.c produksi dan KAT capture. Dimatikan (via
// RA_CORE_PARAM_NO_MAIN) saat file ini di-#include oleh battery_wrapper_*.c
// supaya main() dari driver_main_template.c yang dipakai.
// ----------------------------------------------------------------------
#if !defined(RA_CORE_PARAM_NO_MAIN)

typedef uint32_t (*core_fn_t)(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream);
typedef struct { const char *name; core_fn_t fn; long max_rng; } core_entry_t;

static const core_entry_t CORES[] = {
    { "orbit", ra_core_orbit, -1 },
    { "singleblock", ra_core_singleblock, RA_N - 1 },
};

static const core_entry_t *find_core(const char *name) {
    for (size_t i = 0; i < sizeof(CORES) / sizeof(CORES[0]); ++i) {
        if (strcmp(CORES[i].name, name) == 0) return &CORES[i];
    }
    fprintf(stderr, "Unknown core '%s'. Known: ", name);
    for (size_t i = 0; i < sizeof(CORES) / sizeof(CORES[0]); ++i) {
        fprintf(stderr, "%s ", CORES[i].name);
    }
    fprintf(stderr, "\n");
    exit(1);
}

static void validate_keys(uint32_t *keys, int *nkeys_out) {
    int nkeys = 0;
    keys[nkeys++] = 0u;
    keys[nkeys++] = 0xFFFFFFFFu;
    static const uint32_t extra_keys[5] = {
        0xDEADBEEFu, 0x12345678u, 0xCAFEBABEu, 0x9E3779B9u, 0x7F4A7C15u,
    };
    for (int i = 0; i < 5; ++i) keys[nkeys++] = extra_keys[i];
    for (uint32_t k = 0; k < 32; ++k) keys[nkeys++] = k;
    *nkeys_out = nkeys;
}

// KAT checksum over rng=1..(RA_N-1), addr_hi=0 baseline. Buffer is 1 word
// larger than the max write for the same fmemopen-trailing-NUL reason as
// ra_core.c (see checksum_key_core there).
static uint32_t checksum_key_core(core_fn_t fn, uint32_t key) {
    uint32_t chk = 0;
    uint32_t buf[RA_N];
    for (size_t n = 1; n <= (size_t)(RA_N - 1); ++n) {
        FILE *fs = fmemopen(buf, n * sizeof(uint32_t) + 1, "wb");
        fn(key, 0u, n, fs);
        fclose(fs);
        for (size_t j = 0; j < n; ++j) {
            chk = rot32(chk ^ buf[j], 7) + 0x9E3779B9u;
        }
    }
    return chk;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "ra_core_param (RA_N=%d, RESEED_MODE=%s)\n"
            "Usage:\n"
            "  %s checksum-gen <core>\n"
            "  %s --stream <core> <key> <addr_hi> <rng>\n"
            "Cores: orbit singleblock (singleblock valid only for rng in [1,%d])\n",
            RA_N, (RA_RESEED_MODE == RESEED_MUTUAL) ? "MUTUAL" : "LOWONLY",
            argv[0], argv[0], RA_N - 1);
        return 1;
    }

    if (strcmp(argv[1], "checksum-gen") == 0 && argc >= 3) {
        const core_entry_t *core = find_core(argv[2]);
        uint32_t keys[2 + 5 + 32];
        int nkeys = 0;
        validate_keys(keys, &nkeys);
        printf("static const uint32_t %s_N%d_%s_KAT_CHECKSUMS[%d] = {\n",
               strcmp(core->name, "orbit") == 0 ? "ORBIT" : "SINGLEBLOCK",
               RA_N, (RA_RESEED_MODE == RESEED_MUTUAL) ? "MUTUAL" : "LOWONLY",
               nkeys);
        for (int ki = 0; ki < nkeys; ++ki) {
            uint32_t chk = checksum_key_core(core->fn, keys[ki]);
            printf("    0x%08xu,%s", chk, (ki % 6 == 5) ? "\n" : " ");
        }
        printf("\n};\n");
        return 0;
    }

    if (strcmp(argv[1], "--stream") == 0 && argc >= 6) {
        const core_entry_t *core = find_core(argv[2]);
        uint32_t key = (uint32_t)strtoul(argv[3], NULL, 0);
        uint32_t addr_hi = (uint32_t)strtoul(argv[4], NULL, 0);
        size_t rng = (size_t)strtoull(argv[5], NULL, 0);

        if (core->max_rng >= 0 && (long)rng > core->max_rng) {
            fprintf(stderr, "--stream: n=%zu exceeds core '%s' max_rng=%ld.\n",
                    rng, core->name, core->max_rng);
            return 1;
        }

        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        uint32_t last_cons = core->fn(key, addr_hi, rng, stdout);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

        fprintf(stderr, "Streamed %zu pseudorandom outputs in %.3f seconds\n", rng, elapsed);
        fprintf(stderr, "Last cons from RNGing: %u\n", last_cons);
        return 0;
    }

    fprintf(stderr, "Unknown mode or missing args: %s\n", argv[1]);
    return 1;
}

#endif // !RA_CORE_PARAM_NO_MAIN
