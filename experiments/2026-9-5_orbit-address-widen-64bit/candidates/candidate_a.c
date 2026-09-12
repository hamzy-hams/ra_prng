// candidate_a.c -- Kandidat A: dual keyterm, kombinasi multiplikatif.
//
// Turunan dari ra_core.c kanonik
// (experiments/2026-9-1_family-productionization/ra_core.c), HANYA
// ra_init_state_orbit/ra_init_state_singleblock + signature
// ra_core_orbit/ra_core_singleblock yang berubah (tambah addr_hi).
// ra_permutation_cycle_orbit/ra_permutation_cycle_singleblock disalin
// VERBATIM, tidak diedit sama sekali -- lihat
// ../HANDOVER.md §2/§6 (batas eksplisit + guardrail "255-round engine
// tidak diredesain").
//
// `cons` tetap murni `key` -- addr_hi HANYA masuk lewat
// ra_init_state_orbit/_singleblock, keputusan user 2026-09-05.
//
// Kandidat A: keyterm_l/keyterm_m masing-masing jadi PRODUK dua hash
// terpisah (satu murni dari key, satu murni dari addr_hi), digabung via
// multiply -- sehingga langkah per-index sendiri (bukan cuma arah
// rotasi) jadi fungsi kedua kata. GUARD_L2/GUARD_M2 baru (popcount=16,
// jarak Hamming >=10 bit dari GUARD_L/GUARD_M/0x9e3779b7/0x06a0dd9b/
// fmix32's 0x85ebca6b/0xc2b2ae35 -- disiplin sama seperti guard lama,
// digenerate & diverifikasi terpisah), plus 2 konstanta multiply baru
// (0x85ebca77u/0x9e3779b1u, ganjil, beda dari 4 konstanta multiply/mix
// lama) supaya keyterm addr_hi tidak numerically overlap dengan keyterm
// key.
//
// Saat addr_hi=0: keyterm_*_param = fmix32(GUARD ^ 0) = fmix32(GUARD)
// (konstanta tetap, BUKAN identik keyterm lama) -- artinya Kandidat A
// TIDAK byte-identik dengan ra_core.c asli walau addr_hi=0 (keyterm jadi
// keyterm_key * konstanta_tetap, bukan keyterm_key sendiri). Ini
// trade-off yang melekat pada desain "dual keyterm selalu dikalikan" --
// byte-verify Tahap 1b untuk kandidat ini karena itu HANYA memverifikasi
// compile+avalanche-sanity, BUKAN byte-identity terhadap ra_core.c lama.
//
// Compile: gcc -O3 -march=native -std=gnu17 -include stdalign.h \
//          candidate_a.c -o candidate_a -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

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

#define GUARD_L 0x38916df4u
#define GUARD_M 0x6c26fc92u
// Guard baru khusus addr_hi (2026-09-05): popcount=16, Hamming distance
// >=10 bit dari GUARD_L/GUARD_M dan dari 0x9e3779b7/0x06a0dd9b/
// fmix32's 0x85ebca6b/0xc2b2ae35 -- generated+verified via script, sama
// disiplin dengan GUARD_L/GUARD_M.
#define GUARD_L2 0x3eb13b90u
#define GUARD_M2 0x392456deu

static void ra_hash(const uint32_t *N, uint32_t *out8) {
    for (int i = 0; i < 8; ++i) {
        out8[i] = 0;
        int base = i * 32;
        for (int j = 0; j < 32; ++j) out8[i] ^= N[base + j];
    }
}

static uint32_t ra_reseed(uint32_t *M, const uint32_t *L) {
    for (int i = 0; i < 256; ++i) M[i] ^= L[i];

    uint32_t tmp8[8];
    ra_hash(M, tmp8);

    uint32_t new_cons = 0;
    for (int e = 0; e < 8; ++e) new_cons ^= (tmp8[e] << e);
    return new_cons;
}

// ----------------------------------------------------------------------
// Mode orbit -- Kandidat A: dual keyterm, kombinasi multiplikatif.
// ----------------------------------------------------------------------
static void ra_init_state_orbit(uint32_t *L, uint32_t *M, uint32_t key,
                                 uint32_t addr_hi) {
    uint32_t keyterm_l_key   = fmix32(GUARD_L  ^ (0x9e3779b7u * key));
    uint32_t keyterm_l_param = fmix32(GUARD_L2 ^ (0x85ebca77u * addr_hi));
    uint32_t keyterm_l = keyterm_l_key * keyterm_l_param;

    uint32_t keyterm_m_key   = fmix32(GUARD_M  ^ (0x06a0dd9bu * key));
    uint32_t keyterm_m_param = fmix32(GUARD_M2 ^ (0x9e3779b1u * addr_hi));
    uint32_t keyterm_m = keyterm_m_key * keyterm_m_param;

    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ addr_hi ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * 0x9e3779b7u) * keyterm_l;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}

// VERBATIM dari ra_core.c (tidak diedit).
static void ra_permutation_cycle_orbit(uint32_t cons, size_t it,
                                        const uint32_t *M, uint32_t *L,
                                        uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    uint32_t o = (M[(uint8_t)(255 + 0)] << 0) ^ (M[(uint8_t)(255 + 1)] << 1) ^
                 (M[(uint8_t)(255 + 2)] << 2) ^ (M[(uint8_t)(255 + 3)] << 3) ^
                 (M[(uint8_t)(255 + 4)] << 4) ^ (M[(uint8_t)(255 + 5)] << 5) ^
                 (M[(uint8_t)(255 + 6)] << 6) ^ (M[(uint8_t)(255 + 7)] << 7);
    uint32_t o_term7 = M[(uint8_t)(255 + 7)] << 7;

    for (uint32_t i = 255; i > 0; --i) {
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 17u;

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & 0xFFu;

        if (*count <= 1) break;
        --(*count);

        uint32_t tmp = L[i];
        L[i] = L[d];
        L[d] = tmp;

        if (i > 1) {
            uint32_t new_term = M[(uint8_t)(i - 1)];
            uint32_t o_next = new_term ^ ((o ^ o_term7) << 1);
            o_term7 = M[(uint8_t)(i + 6)] << 7;
            o = o_next;
        }
    }
}

uint32_t ra_core_orbit(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key; // TIDAK berubah -- keputusan user 2026-09-05
    ra_init_state_orbit(L, M, key, addr_hi);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_orbit(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}

// ----------------------------------------------------------------------
// Mode singleblock -- Kandidat A, analog orbit (M[] only, window sama).
// ----------------------------------------------------------------------
static void ra_init_state_singleblock(uint32_t *M, uint32_t key,
                                       uint32_t addr_hi, size_t rng) {
    uint32_t keyterm_m_key   = fmix32(GUARD_M  ^ (0x06a0dd9bu * key));
    uint32_t keyterm_m_param = fmix32(GUARD_M2 ^ (0x9e3779b1u * addr_hi));
    uint32_t keyterm_m = keyterm_m_key * keyterm_m_param;

    int len = (rng + 7 > 256) ? 256 : (int)(rng + 7);
    int start = (256 - (int)rng) & 0xFF;
    int end = start + len;

    int first_end = (end <= 256) ? end : 256;
    for (int i = start; i < first_end; ++i) {
        uint32_t r = key ^ addr_hi ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32(m_val, r);
    }
    if (end > 256) {
        for (int i = 0; i < end - 256; ++i) {
            uint32_t r = key ^ addr_hi ^ (uint32_t)i;
            uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
            M[i] = rot32(m_val, r);
        }
    }
}

// VERBATIM dari ra_core.c (tidak diedit).
static void ra_permutation_cycle_singleblock(uint32_t cons, size_t it,
                                              const uint32_t *M,
                                              uint64_t *count,
                                              FILE *raw_stream) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;

    uint32_t o = (M[(uint8_t)(255 + 0)] << 0) ^ (M[(uint8_t)(255 + 1)] << 1) ^
                 (M[(uint8_t)(255 + 2)] << 2) ^ (M[(uint8_t)(255 + 3)] << 3) ^
                 (M[(uint8_t)(255 + 4)] << 4) ^ (M[(uint8_t)(255 + 5)] << 5) ^
                 (M[(uint8_t)(255 + 6)] << 6) ^ (M[(uint8_t)(255 + 7)] << 7);
    uint32_t o_term7 = M[(uint8_t)(255 + 7)] << 7;

    for (uint32_t i = 255; i > 0; --i) {
        a = (d ^ o) ^ (cons + a);
        b = (cons + a) ^ (o + d);
        c = rot32((a >> 13) ^ a, b);
        c ^= c >> 17u;

        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & 0xFFu;

        if (*count <= 1) break;
        --(*count);

        if (i > 1) {
            uint32_t new_term = M[(uint8_t)(i - 1)];
            uint32_t o_next = new_term ^ ((o ^ o_term7) << 1);
            o_term7 = M[(uint8_t)(i + 6)] << 7;
            o = o_next;
        }
    }
}

uint32_t ra_core_singleblock(uint32_t key, uint32_t addr_hi, size_t rng,
                              FILE *raw_stream) {
    if (rng == 0) return key;
    if (rng > 255) {
        fprintf(stderr,
            "ra_core_singleblock: rng=%zu > 255 out of scope (no-reseed "
            "fast path valid only for rng in [1,255]); use ra_core_orbit.\n",
            rng);
        abort();
    }

    alignas(64) uint32_t M[256];
    uint64_t count = rng;
    uint32_t cons = key; // TIDAK berubah -- keputusan user 2026-09-05

    ra_init_state_singleblock(M, key, addr_hi, rng);
    ra_permutation_cycle_singleblock(cons, /*it=*/0, M, &count, raw_stream);
    return cons;
}

// ----------------------------------------------------------------------
// CLI dispatch -- kontrak argv diperluas: --stream <core> <key> <addr_hi> <n>
// ----------------------------------------------------------------------

typedef uint32_t (*core_fn_t)(uint32_t key, uint32_t addr_hi, size_t rng,
                               FILE *raw_stream);

typedef struct { const char *name; core_fn_t fn; long max_rng; } core_entry_t;

static const core_entry_t CORES[] = {
    { "orbit", ra_core_orbit, -1 },
    { "singleblock", ra_core_singleblock, 255 },
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

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage:\n"
            "  %s --stream <core> <key> <addr_hi> <n>\n"
            "Cores: orbit singleblock (singleblock valid only for rng in [1,255])\n",
            argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--stream") == 0 && argc >= 6) {
        const core_entry_t *core = find_core(argv[2]);
        uint32_t key = (uint32_t)strtoul(argv[3], NULL, 0);
        uint32_t addr_hi = (uint32_t)strtoul(argv[4], NULL, 0);
        size_t rng = (size_t)strtoull(argv[5], NULL, 0);

        if (core->max_rng >= 0 && (long)rng > core->max_rng) {
            fprintf(stderr,
                "--stream: n=%zu exceeds core '%s' max_rng=%ld.\n",
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
