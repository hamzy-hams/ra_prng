/* C port of pruned_wired_toy_prng.py's next_state + cycle_measure.py's
 * Brent's algorithm, for (n, w) configs too slow to fully measure in pure
 * Python (mirrors ../2026-8-25_periodicity-heuristic-validation/
 * cycle_measure.c's structure -- checkpoint/resume machinery is verbatim,
 * only next_state() and the hash-reduction function differ; see this
 * directory's pruned_wired_toy_prng.py module docstring for the exact op
 * mapping / design decisions this C port must match).
 *
 * Op set: TAP6,TAP7 (generalized: top-2 tap survivors of G),ROT_C,SHR13
 * only, of 18 tracked flags. HASH_ACCESS=sequential (pure function, M never
 * mutated by the hash step -- NOT the mutate-in-place pattern the sibling
 * 2026-8-25 experiment's ra_hash_gen used, see pruned_wired_toy_prng.py's
 * "IMPORTANT DEVIATION" docstring note). Wiring: a_xor_operand=d,
 * c_shift_operand=a, rotc_amount_source=b, rotc_xor_operand=a.
 *
 * IMPORTANT CONSTRAINT: tap term e contributes (M[idx]<<e)&mask, a plain
 * left shift -- for e>=w this is identically zero (bits shifted out before
 * masking). params_init() rejects any (n,w,rows) where the tap survivors
 * would collapse this way, matching pruned_wired_toy_prng.py's
 * tap_survivors() assertion.
 *
 * Compile: gcc -O3 -march=native -std=gnu17 cycle_measure.c -o cycle_measure
 * Usage:   ./cycle_measure <n> <w> <rows> <seed_start> <seed_count>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <time.h>

static volatile sig_atomic_t g_stop_requested = 0;
static void handle_signal(int sig) { (void)sig; g_stop_requested = 1; }

typedef struct {
    int n, w, G;
    uint32_t mask;
    uint32_t S13;
    uint32_t c_m, c_l;
    int tap0, tap1; /* tap1 == tap0 when G==1 (single forced tap) */
} Params;

static uint32_t rotw(uint32_t x, uint32_t r, const Params *p) {
    r &= (uint32_t)(p->w - 1);
    x &= p->mask;
    if (r == 0) return x;
    return ((x << r) | (x >> (p->w - r))) & p->mask;
}

/* Must match Python's round() (round-half-to-even). Only S13 is actually
 * used by this algorithm (SHL9/SHR18/SHL14 are off); S13's rescale never
 * hits an exact .5 tie at w=4 (round(13*4/32)=round(1.625)=2) or w=8
 * (round(13*8/32)=round(3.25)=3) -- the tie that bit the sibling
 * experiment (s=18,w=8) involves S18, which this algorithm never uses. */
static uint32_t rescale_shift(int s, int w) {
    long num = (long)s * w;
    long quot = num / 32;
    long rem = num % 32;
    long s_w;
    if (2 * rem < 32) s_w = quot;
    else if (2 * rem > 32) s_w = quot + 1;
    else s_w = (quot % 2 == 0) ? quot : quot + 1;
    if (s_w < 1) s_w = 1;
    if (s_w > w - 1) s_w = w - 1;
    return (uint32_t)s_w;
}

static uint32_t truncate_const(uint32_t c32, int w) {
    uint32_t mask = (w >= 32) ? 0xFFFFFFFFu : ((1u << w) - 1u);
    uint32_t c_w = (c32 >> (32 - w)) & mask;
    return c_w ? c_w : 1u;
}

static void params_init(Params *p, int n, int w, int rows) {
    p->n = n;
    p->w = w;
    p->mask = (w >= 32) ? 0xFFFFFFFFu : ((1u << w) - 1u);
    if (rows < 1 || n % rows != 0) {
        fprintf(stderr, "rows=%d must be >=1 and evenly divide n=%d\n", rows, n);
        exit(1);
    }
    p->G = n / rows;
    p->S13 = rescale_shift(13, w);
    p->c_m = truncate_const(0x06A0DD9Bu, w);
    p->c_l = truncate_const(0x9E3779B7u, w);

    if (p->G >= 2) {
        p->tap0 = p->G - 2;
        p->tap1 = p->G - 1;
    } else {
        p->tap0 = p->tap1 = 0;
    }
    if (p->tap1 >= w) {
        fprintf(stderr, "n=%d w=%d rows=%d (G=%d): tap index %d >= w=%d would "
                        "collapse to zero (plain left-shift) -- config rejected.\n",
                n, w, rows, p->G, p->tap1, w);
        exit(1);
    }
}

typedef struct {
    uint32_t *L, *M;
    uint32_t cons, it;
} State;

static void state_alloc(State *s, int n) {
    s->L = malloc(sizeof(uint32_t) * n);
    s->M = malloc(sizeof(uint32_t) * n);
}
static void state_free(State *s) { free(s->L); free(s->M); }
static void state_copy(State *dst, const State *src, int n) {
    memcpy(dst->L, src->L, sizeof(uint32_t) * n);
    memcpy(dst->M, src->M, sizeof(uint32_t) * n);
    dst->cons = src->cons;
    dst->it = src->it;
}
static int state_equal(const State *a, const State *b, int n) {
    if (a->cons != b->cons || a->it != b->it) return 0;
    return memcmp(a->L, b->L, sizeof(uint32_t) * n) == 0 &&
           memcmp(a->M, b->M, sizeof(uint32_t) * n) == 0;
}

#define CKPT_MAGIC 0x52415052u /* "RAPR" */
#define CKPT_VERSION 1u

typedef struct {
    uint32_t magic, version;
    int32_t n, w, rows;
    uint32_t seed;
    int32_t phase;
    uint64_t lam, power, k, mu;
} CkptHeader;

static void state_write(FILE *f, const State *s, int n) {
    fwrite(s->L, sizeof(uint32_t), (size_t)n, f);
    fwrite(s->M, sizeof(uint32_t), (size_t)n, f);
    fwrite(&s->cons, sizeof(uint32_t), 1, f);
    fwrite(&s->it, sizeof(uint32_t), 1, f);
}

static int state_read(FILE *f, State *s, int n) {
    if (fread(s->L, sizeof(uint32_t), (size_t)n, f) != (size_t)n) return -1;
    if (fread(s->M, sizeof(uint32_t), (size_t)n, f) != (size_t)n) return -1;
    if (fread(&s->cons, sizeof(uint32_t), 1, f) != 1) return -1;
    if (fread(&s->it, sizeof(uint32_t), 1, f) != 1) return -1;
    return 0;
}

static void save_checkpoint(const char *path, const Params *p, uint32_t seed,
                             int phase, uint64_t lam, uint64_t power, uint64_t k, uint64_t mu,
                             const State *tortoise, const State *hare) {
    char tmp_path[512];
    snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);
    FILE *f = fopen(tmp_path, "wb");
    if (!f) { perror("checkpoint fopen"); return; }
    CkptHeader h = { CKPT_MAGIC, CKPT_VERSION, p->n, p->w, p->n / p->G, seed,
                      phase, lam, power, k, mu };
    fwrite(&h, sizeof(h), 1, f);
    state_write(f, tortoise, p->n);
    state_write(f, hare, p->n);
    fclose(f);
    rename(tmp_path, path);
}

static int load_checkpoint(const char *path, const Params *p, uint32_t seed,
                            int *phase, uint64_t *lam, uint64_t *power, uint64_t *k, uint64_t *mu,
                            State *tortoise, State *hare) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    CkptHeader h;
    if (fread(&h, sizeof(h), 1, f) != 1 ||
        h.magic != CKPT_MAGIC || h.version != CKPT_VERSION ||
        h.n != p->n || h.w != p->w || h.rows != p->n / p->G || h.seed != seed ||
        state_read(f, tortoise, p->n) != 0 ||
        state_read(f, hare, p->n) != 0) {
        fclose(f);
        return 0;
    }
    fclose(f);
    *phase = h.phase; *lam = h.lam; *power = h.power; *k = h.k; *mu = h.mu;
    return 1;
}

static void init_state(State *s, uint32_t seed, const Params *p) {
    for (int i = 0; i < p->n; i++) {
        s->M[i] = (uint32_t)((uint64_t)i * p->c_m + p->c_m) & p->mask;
        s->L[i] = (uint32_t)((uint64_t)i * p->c_l + p->c_l) & p->mask;
    }
    s->cons = seed & p->mask;
    s->it = 0;
}

/* HASH_ACCESS=sequential, pure function -- N is READ, never mutated (see
 * pruned_wired_toy_prng.py's "IMPORTANT DEVIATION" note for why this
 * differs from the sibling experiment's mutate-in-place ra_hash_gen). */
static void ra_hash_gen_sequential(const uint32_t *N, const Params *p, uint32_t *out) {
    int G = p->G, rows = p->n / G;
    for (int i = 0; i < G; i++) {
        uint32_t h = 0;
        int base = i * rows;
        for (int j = 0; j < rows; j++) h ^= N[base + j];
        out[i] = h & p->mask;
    }
}

static void next_state(State *s, const Params *p, uint32_t *hash_out /* scratch, size G */) {
    int n = p->n;
    uint32_t mask = p->mask;
    uint32_t a = s->cons, b = s->it, d = 0;
    int tap0 = p->tap0, tap1 = p->tap1;
    int single_tap = (p->G == 1);

    for (int i = n - 1; i >= 1; i--) {
        uint32_t o;
        {
            int idx0 = (i + tap0) & (n - 1);
            o = (s->M[idx0] << tap0) & mask;
            if (!single_tap) {
                int idx1 = (i + tap1) & (n - 1);
                o ^= (s->M[idx1] << tap1) & mask;
            }
        }

        a = ((d ^ o) ^ ((s->cons + a) & mask)) & mask;
        b = (((s->cons + a) & mask) ^ ((o + d) & mask)) & mask;

        uint32_t shifted = (a >> p->S13) & mask;
        uint32_t pre_rot = (shifted ^ a) & mask;
        uint32_t c = rotw(pre_rot, b, p) & mask;

        d = c & (uint32_t)(n - 1);

        uint32_t tmp = s->L[i]; s->L[i] = s->L[(int)d]; s->L[(int)d] = tmp;
    }

    for (int i = 0; i < n; i++) s->M[i] ^= s->L[i];

    ra_hash_gen_sequential(s->M, p, hash_out);

    uint32_t new_cons = 0;
    for (int e = 0; e < p->G; e++) new_cons ^= (hash_out[e] << e) & mask;

    s->cons = new_cons;
    s->it = (s->it + 1) & mask;
}

static int brent_resumable(const Params *p, uint32_t seed, const char *ckpt_path,
                            uint64_t *lam_out, uint64_t *mu_out) {
    int n = p->n;
    uint32_t *hash_out = malloc(sizeof(uint32_t) * p->G);

    State x0, tortoise, hare;
    state_alloc(&x0, n); state_alloc(&tortoise, n); state_alloc(&hare, n);
    init_state(&x0, seed, p);

    int phase;
    uint64_t power, lam, k, mu;

    if (load_checkpoint(ckpt_path, p, seed, &phase, &lam, &power, &k, &mu, &tortoise, &hare)) {
        fprintf(stderr, "resumed seed=%u from checkpoint: phase=%d lam=%llu power=%llu "
                        "k=%llu mu=%llu\n", seed, phase, (unsigned long long)lam,
                (unsigned long long)power, (unsigned long long)k, (unsigned long long)mu);
    } else {
        state_copy(&tortoise, &x0, n);
        state_copy(&hare, &x0, n);
        next_state(&hare, p, hash_out);
        phase = 0; power = 1; lam = 1; k = 0; mu = 0;
    }

    time_t last_ckpt = time(NULL);
    time_t last_progress = last_ckpt;
    time_t seed_start = last_ckpt;
    uint64_t since_check = 0;
    const uint64_t CHECK_EVERY = 200000;
    const double CKPT_INTERVAL_SEC = 30.0;
    const double PROGRESS_INTERVAL_SEC = 1800.0;
    int interrupted = 0;

#define MAYBE_CHECKPOINT() \
    do { \
        if (++since_check >= CHECK_EVERY) { \
            since_check = 0; \
            time_t now = time(NULL); \
            if (g_stop_requested || difftime(now, last_ckpt) >= CKPT_INTERVAL_SEC) { \
                save_checkpoint(ckpt_path, p, seed, phase, lam, power, k, mu, &tortoise, &hare); \
                last_ckpt = now; \
                if (g_stop_requested) { interrupted = 1; goto done; } \
            } \
            if (difftime(now, last_progress) >= PROGRESS_INTERVAL_SEC) { \
                fprintf(stderr, "progress seed=%u phase=%d lam=%llu power=%llu k=%llu mu=%llu " \
                                "elapsed_this_run=%.0fs\n", \
                        seed, phase, (unsigned long long)lam, (unsigned long long)power, \
                        (unsigned long long)k, (unsigned long long)mu, difftime(now, seed_start)); \
                fflush(stderr); \
                last_progress = now; \
            } \
        } \
    } while (0)

    if (phase == 0) {
        while (!state_equal(&tortoise, &hare, n)) {
            if (power == lam) {
                state_copy(&tortoise, &hare, n);
                power *= 2;
                lam = 0;
            }
            next_state(&hare, p, hash_out);
            lam++;
            MAYBE_CHECKPOINT();
        }
        phase = 1;
        state_copy(&tortoise, &x0, n);
        state_copy(&hare, &x0, n);
        k = 0;
    }

    if (phase == 1) {
        while (k < lam) {
            next_state(&hare, p, hash_out);
            k++;
            MAYBE_CHECKPOINT();
        }
        phase = 2;
        mu = 0;
    }

    if (phase == 2) {
        while (!state_equal(&tortoise, &hare, n)) {
            next_state(&tortoise, p, hash_out);
            next_state(&hare, p, hash_out);
            mu++;
            MAYBE_CHECKPOINT();
        }
    }

#undef MAYBE_CHECKPOINT
done:
    state_free(&x0); state_free(&tortoise); state_free(&hare);
    free(hash_out);
    if (interrupted) return -1;

    *lam_out = lam;
    *mu_out = mu;
    remove(ckpt_path);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr, "usage: %s <n> <w> <rows> <seed_start> <seed_count>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    int w = atoi(argv[2]);
    int rows = atoi(argv[3]);
    uint32_t seed_start = (uint32_t)strtoul(argv[4], NULL, 10);
    int seed_count = atoi(argv[5]);

    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    Params p;
    params_init(&p, n, w, rows);
    fprintf(stderr, "n=%d w=%d G=%d rows=%d S13=%u taps=(%d,%d) c_m=0x%x c_l=0x%x\n",
            p.n, p.w, p.G, p.n / p.G, p.S13, p.tap0, p.tap1, p.c_m, p.c_l);

    printf("seed,lambda,mu\n");
    fflush(stdout);
    for (int k = 0; k < seed_count; k++) {
        uint32_t seed = seed_start + (uint32_t)k;
        char ckpt_path[256];
        snprintf(ckpt_path, sizeof(ckpt_path),
                 ".cycle_measure_ckpt_n%d_w%d_rows%d_seed%u.bin", p.n, p.w, p.n / p.G, seed);

        uint64_t lam, mu;
        int rc = brent_resumable(&p, seed, ckpt_path, &lam, &mu);
        if (rc != 0) {
            fprintf(stderr, "interrupted at seed=%u - checkpoint saved to %s. "
                            "Re-run the same command to resume.\n", seed, ckpt_path);
            return 42;
        }
        printf("%u,%llu,%llu\n", seed, (unsigned long long)lam, (unsigned long long)mu);
        fflush(stdout);
    }
    return 0;
}
