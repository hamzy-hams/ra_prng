/* C port of toy_prng.py's next_state + cycle_measure.py's Brent's algorithm,
 * for (n, w) configs too slow to fully measure in pure Python (see
 * RESULTS.md's timing note - (8,4) in particular).
 *
 * Same generalization rules as toy_prng.py: rescaled shifts
 * (s_w = round(s*w/32), clamped to [1,w-1]), top-w-bits golden-ratio
 * constants with the zero-collapse fix (c_w = c_w ? c_w : 1), and
 * G = max(1, n/4) for the ra_hash reduction. Kept parameterized by
 * runtime n/w (not hardcoded) so it stays a direct, checkable port rather
 * than a re-derivation.
 *
 * Checkpoint/resume: some configs (e.g. n=8,w=4,rows=8) can run for many
 * hours per seed with no output until the cycle closes. SIGTERM/SIGINT
 * (a plain `kill`, or Ctrl-C) trigger an immediate checkpoint + clean exit
 * (code 42); an unconditional autosave every ~30s also protects against
 * SIGKILL/power loss. Checkpoints are per-seed
 * (.cycle_measure_ckpt_n{n}_w{w}_rows{rows}_seed{seed}.bin in the cwd) -
 * re-running the *same* command line resumes automatically from the last
 * checkpoint of whichever seed was in progress; already-completed seeds
 * in a multi-seed run are NOT skipped on resume (only the in-progress
 * seed's checkpoint is tracked - see RESULTS.md / the plan for why this
 * scope was chosen).
 *
 * State-update-mechanism spectrum for L (see
 * experiments/2026-8-28_state-update-mechanism-research/HANDOVER.md):
 * mechanism 0 = permute (today's swap, default), 1 = inject (ra_prng3-style
 * one-directional overwrite, L[i]=L[d]; L[d]=c), 2 = overwrite (direct
 * overwrite, no relocation, L[i]=c). Mirrors toy_prng.py's MECHANISMS.
 *
 * State-update-mechanism spectrum for M's once-per-cycle reseed fold
 * (HANDOVER.md catalog #4, "symmetric treatment of M" -- design choices
 * made explicit here, see toy_prng.py's M_MECHANISMS docstring for the
 * full rationale): m_mechanism 0 = xor_fold (today's M[i]^=L[i], default),
 * 1 = permute (M[i],M[L[i]%n] swapped for each i), 2 = inject (M[i]=M[j];
 * M[j]=L[i] where j=L[i]%n), 3 = overwrite (M[i]=L[i] for all i).
 *
 * Compile: gcc -O3 -march=native -std=gnu17 cycle_measure.c -o cycle_measure
 * Usage:   ./cycle_measure <n> <w> <rows> <mechanism> <m_mechanism> <seed_start> <seed_count>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <time.h>

static volatile sig_atomic_t g_stop_requested = 0;
static void handle_signal(int sig) { (void)sig; g_stop_requested = 1; }

enum { MECH_PERMUTE = 0, MECH_INJECT = 1, MECH_OVERWRITE = 2 };
enum { MMECH_XOR_FOLD = 0, MMECH_PERMUTE = 1, MMECH_INJECT = 2, MMECH_OVERWRITE = 3 };

typedef struct {
    int n, w, G;
    uint32_t mask;
    uint32_t S9, S18, S13, S14;
    uint32_t c_m, c_l;
    int mechanism;
    int m_mechanism;
} Params;

static uint32_t rotw(uint32_t x, uint32_t r, const Params *p) {
    r &= (uint32_t)(p->w - 1);
    x &= p->mask;
    if (r == 0) return x;
    return ((x << r) | (x >> (p->w - r))) & p->mask;
}

/* Must match Python's round() (round-half-to-even), not round-half-up:
 * s*w/32 hits an exact .5 tie for (s=18, w=8) - round-half-up gives 5,
 * banker's rounding gives 4. Only one convention can match toy_prng.py's
 * rescale_shifts(), which uses Python's builtin round(). */
static uint32_t rescale_shift(int s, int w) {
    long num = (long)s * w; /* denominator is always 32 */
    long quot = num / 32;
    long rem = num % 32;
    long s_w;
    if (2 * rem < 32) s_w = quot;
    else if (2 * rem > 32) s_w = quot + 1;
    else s_w = (quot % 2 == 0) ? quot : quot + 1; /* tie: round to even */
    if (s_w < 1) s_w = 1;
    if (s_w > w - 1) s_w = w - 1;
    return (uint32_t)s_w;
}

static uint32_t truncate_const(uint32_t c32, int w) {
    uint32_t mask = (w >= 32) ? 0xFFFFFFFFu : ((1u << w) - 1u);
    uint32_t c_w = (c32 >> (32 - w)) & mask;
    return c_w ? c_w : 1u;
}

static void params_init(Params *p, int n, int w, int rows, int mechanism, int m_mechanism) {
    p->n = n;
    p->w = w;
    p->mask = (w >= 32) ? 0xFFFFFFFFu : ((1u << w) - 1u);
    if (rows < 1 || n % rows != 0) {
        fprintf(stderr, "rows=%d must be >=1 and evenly divide n=%d\n", rows, n);
        exit(1);
    }
    p->G = n / rows;
    if (mechanism < MECH_PERMUTE || mechanism > MECH_OVERWRITE) {
        fprintf(stderr, "mechanism=%d must be 0 (permute), 1 (inject), or 2 (overwrite)\n", mechanism);
        exit(1);
    }
    p->mechanism = mechanism;
    if (m_mechanism < MMECH_XOR_FOLD || m_mechanism > MMECH_OVERWRITE) {
        fprintf(stderr, "m_mechanism=%d must be 0 (xor_fold), 1 (permute), 2 (inject), or 3 (overwrite)\n", m_mechanism);
        exit(1);
    }
    p->m_mechanism = m_mechanism;
    p->S9  = rescale_shift(9, w);
    p->S18 = rescale_shift(18, w);
    p->S13 = rescale_shift(13, w);
    p->S14 = rescale_shift(14, w);
    p->c_m = truncate_const(0x06A0DD9Bu, w);
    p->c_l = truncate_const(0x9E3779B7u, w);
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

/* Checkpoint format: header, then serialized tortoise, then hare.
 * phase: 0 = Brent search (tortoise/hare doubling), 1 = reset (advancing
 * a fresh hare exactly lam steps from x0), 2 = final (mu search). */
#define CKPT_MAGIC 0x52415052u /* "RAPR" */
#define CKPT_VERSION 3u /* bumped: CkptHeader gained `m_mechanism` */

typedef struct {
    uint32_t magic, version;
    int32_t n, w, rows, mechanism, m_mechanism;
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
    CkptHeader h = { CKPT_MAGIC, CKPT_VERSION, p->n, p->w, p->n / p->G, p->mechanism, p->m_mechanism,
                      seed, phase, lam, power, k, mu };
    fwrite(&h, sizeof(h), 1, f);
    state_write(f, tortoise, p->n);
    state_write(f, hare, p->n);
    fclose(f);
    rename(tmp_path, path); /* atomic on POSIX: readers never see a partial file */
}

/* Returns 1 on a validated, matching checkpoint (fields populated), 0 if
 * absent/corrupt/mismatched (caller should start the seed fresh). */
static int load_checkpoint(const char *path, const Params *p, uint32_t seed,
                            int *phase, uint64_t *lam, uint64_t *power, uint64_t *k, uint64_t *mu,
                            State *tortoise, State *hare) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    CkptHeader h;
    if (fread(&h, sizeof(h), 1, f) != 1 ||
        h.magic != CKPT_MAGIC || h.version != CKPT_VERSION ||
        h.n != p->n || h.w != p->w || h.rows != p->n / p->G ||
        h.mechanism != p->mechanism || h.m_mechanism != p->m_mechanism || h.seed != seed ||
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

/* direct port of ra_hash_gen in toy_prng.py */
static void ra_hash_gen(uint32_t *N, const Params *p, uint32_t *out /* size G */) {
    int n = p->n, G = p->G, rows = n / G;
    for (int i = 0; i < G; i++) out[i] = 0;
    for (int i = 0; i < G; i++) {
        out[i] ^= N[N[i] & (uint32_t)(n - 1)];
        for (int j = 0; j < rows; j++) {
            N[i] ^= N[j * G + i];
        }
        N[i] &= p->mask;
    }
}

static void next_state(State *s, const Params *p, uint32_t *hash_out /* scratch, size G */) {
    int n = p->n, w = p->w, G = p->G;
    uint32_t mask = p->mask;
    uint32_t a = s->cons, b = s->it, c = 0, d = 0;

    for (int i = n - 1; i >= 1; i--) {
        uint32_t o = 0;
        for (int e = 0; e < G; e++) {
            int idx = (i + e) & (n - 1);
            o ^= (s->M[idx] << e) & mask;
        }

        a = (rotw(b ^ o, d, p) ^ ((s->cons + a) & mask)) & mask;
        b = (rotw((s->cons + a) & mask, (uint32_t)i, p) ^ ((o + d) & mask)) & mask;
        o = ((rotw(a ^ o, (uint32_t)i, p) << p->S9) & mask) ^ (b >> p->S18);
        c = rotw((((o + c) & mask) << p->S14 & mask) ^ (b >> p->S13) ^ a, b, p) & mask;

        d = (uint32_t)(((uint64_t)c * (uint64_t)(i + 1)) >> w);

        if (p->mechanism == MECH_PERMUTE) {
            uint32_t tmp = s->L[i]; s->L[i] = s->L[(int)d]; s->L[(int)d] = tmp;
        } else if (p->mechanism == MECH_INJECT) {
            s->L[i] = s->L[(int)d];
            s->L[(int)d] = c;
        } else { /* MECH_OVERWRITE */
            s->L[i] = c;
        }
    }

    if (p->m_mechanism == MMECH_XOR_FOLD) {
        for (int i = 0; i < n; i++) s->M[i] ^= s->L[i];
    } else if (p->m_mechanism == MMECH_PERMUTE) {
        for (int i = 0; i < n; i++) {
            int j = (int)(s->L[i] & (uint32_t)(n - 1));
            uint32_t tmp = s->M[i]; s->M[i] = s->M[j]; s->M[j] = tmp;
        }
    } else if (p->m_mechanism == MMECH_INJECT) {
        for (int i = 0; i < n; i++) {
            int j = (int)(s->L[i] & (uint32_t)(n - 1));
            s->M[i] = s->M[j];
            s->M[j] = s->L[i];
        }
    } else { /* MMECH_OVERWRITE */
        for (int i = 0; i < n; i++) s->M[i] = s->L[i];
    }

    ra_hash_gen(s->M, p, hash_out);

    uint32_t new_cons = 0;
    for (int e = 0; e < G; e++) new_cons ^= (hash_out[e] << e) & mask;

    s->cons = new_cons;
    s->it = (s->it + 1) & mask;
}

/* Brent's cycle detection, checkpointed/resumable.
 * Returns 0 with *lam_out and *mu_out set on completion, or -1 if interrupted
 * (SIGTERM/SIGINT) after saving a checkpoint at ckpt_path. */
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
    const double PROGRESS_INTERVAL_SEC = 1800.0; /* 30 min */
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
        /* k must be incremented (to reflect the advance just made) before
         * MAYBE_CHECKPOINT() runs - a checkpoint saved with the pre-increment
         * k here would replay one extra next_state() on resume, leaving
         * hare permanently 1 step off the cycle position phase 2 needs,
         * which makes the mu-search loop below never terminate. */
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
    if (argc != 8) {
        fprintf(stderr, "usage: %s <n> <w> <rows> <mechanism 0=permute|1=inject|2=overwrite> "
                        "<m_mechanism 0=xor_fold|1=permute|2=inject|3=overwrite> "
                        "<seed_start> <seed_count>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    int w = atoi(argv[2]);
    int rows = atoi(argv[3]);
    int mechanism = atoi(argv[4]);
    int m_mechanism = atoi(argv[5]);
    uint32_t seed_start = (uint32_t)strtoul(argv[6], NULL, 10);
    int seed_count = atoi(argv[7]);

    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    Params p;
    params_init(&p, n, w, rows, mechanism, m_mechanism);
    fprintf(stderr, "n=%d w=%d G=%d rows=%d mechanism=%d m_mechanism=%d shifts=(%u,%u,%u,%u) c_m=0x%x c_l=0x%x\n",
            p.n, p.w, p.G, p.n / p.G, p.mechanism, p.m_mechanism, p.S9, p.S18, p.S13, p.S14, p.c_m, p.c_l);

    printf("seed,lambda,mu\n");
    fflush(stdout);
    for (int k = 0; k < seed_count; k++) {
        uint32_t seed = seed_start + (uint32_t)k;
        char ckpt_path[256];
        snprintf(ckpt_path, sizeof(ckpt_path),
                 ".cycle_measure_ckpt_n%d_w%d_rows%d_mech%d_mmech%d_seed%u.bin",
                 p.n, p.w, p.n / p.G, p.mechanism, p.m_mechanism, seed);

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
