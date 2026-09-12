// ra_core_engine.c
// Library layer for pip-ra-core, built ON TOP OF the canonical, unmodified
// ra_core.c (included verbatim below -- its formula is not touched or
// duplicated here, only the already-validated static helpers
// (ra_init_state_orbit / ra_permutation_cycle_orbit / ra_reseed) are reused
// directly). `main` is renamed before inclusion purely so it doesn't leak
// as an exported symbol into the shared library this file compiles into --
// same trick used by ../../../experiments/*/multikey_stream*.c (there via
// `-Dmain=...` on the command line; here via #define, since this file is
// compiled standalone by pip-ra-core/setup.py, not linked against a
// separately-compiled ra_core_nomain.o).
//
// Two kinds of API added here, neither changes what a single
// ra_core_orbit/ra_core_singleblock call produces:
//   1. Stateless buffer wrappers (ra_core_orbit_into/ra_core_singleblock_into)
//      -- thin fmemopen shims so ctypes gets a byte buffer back instead of a
//      FILE*, for the "random-access generate" / "auto mode-selection"
//      library features.
//   2. A stateful streaming engine (ra_core_engine_t) for the orbit mode,
//      exposing per-call continuation, position introspection, fork, and
//      save/load -- none of which the public ra_core_orbit() can do since
//      its L/M/cons state is local to one call and discarded on return.
// Plus a deterministic Fisher-Yates shuffle built from (1), using the
// KEY-increment chaining pattern (see chaining note above
// ra_core_shuffle_indices below -- addr_hi-increment chaining was tried
// first and FAILED PractRand catastrophically, see
// experiments/2026-9-7_addr-hi-chaining-validation/RESULTS.md).

#include <stdalign.h> // alignas -- ra_core.c normally gets this via `-include
                       // stdalign.h` on the command line (see its own header
                       // comment); included explicitly here so this file is
                       // self-contained regardless of how setup.py invokes gcc.

#define main ra_core_unused_main
#include "ra_core.c"
#undef main

#include "ra_core_engine.h" // declares the struct tag below as opaque for
                             // external consumers (e.g. the Cython
                             // BitGenerator extension); this file provides
                             // the one real definition.

// ----------------------------------------------------------------------
// Stateless one-shot buffer wrappers. `rng * sizeof(uint32_t) + 1`: the
// extra word-plus-one-byte matches ra_core.c's own checksum_key_core()
// precedent (see its comment) -- glibc's fmemopen("wb") writes a trailing
// NUL right after the last byte written, which clobbers the MSB of the
// final uint32_t on little-endian if the buffer is exactly write-sized.
// ----------------------------------------------------------------------

void ra_core_orbit_into(uint32_t key, uint32_t addr_hi, size_t rng, uint32_t *out) {
    FILE *fs = fmemopen(out, rng * sizeof(uint32_t) + 1, "wb");
    ra_core_orbit(key, addr_hi, rng, fs);
    fclose(fs);
}

void ra_core_singleblock_into(uint32_t key, uint32_t addr_hi, size_t rng, uint32_t *out) {
    FILE *fs = fmemopen(out, rng * sizeof(uint32_t) + 1, "wb");
    ra_core_singleblock(key, addr_hi, rng, fs);
    fclose(fs);
}

// ----------------------------------------------------------------------
// Stateful orbit streaming engine.
// ----------------------------------------------------------------------

struct ra_core_engine_s {
    alignas(64) uint32_t L[256];
    alignas(64) uint32_t M[256];
    uint32_t cons;
    uint32_t key;
    uint32_t addr_hi;
    uint64_t it;              // completed 255-word cycles so far
    uint32_t block_buf[256];  // most recent cycle's words (255 valid + 1 pad)
    uint32_t block_len;       // valid word count in block_buf (0 before first fill)
    uint32_t block_pos;       // consumption cursor into block_buf, [0, block_len]
};

size_t ra_core_engine_struct_size(void) { return sizeof(ra_core_engine_t); }

// Run exactly one full 255-word ra_permutation_cycle_orbit cycle into
// e->block_buf and advance to the next cycle's cons via ra_reseed --
// mirrors ra_core_orbit()'s inner loop body exactly, one iteration at a
// time. `count` is seeded to UINT64_MAX so the cycle never exits early
// (the early-exit path only exists for ra_core_orbit's fixed total-rng
// case, which does not apply to an open-ended stream).
static void ra_core_engine_fill_block(ra_core_engine_t *e) {
    uint64_t count = UINT64_MAX;
    FILE *fs = fmemopen(e->block_buf, 255 * sizeof(uint32_t) + 1, "wb");
    ra_permutation_cycle_orbit(e->cons, e->it, e->M, e->L, &count, fs);
    fclose(fs);
    e->cons = ra_reseed(e->M, e->L);
    ++e->it;
    e->block_len = 255;
    e->block_pos = 0;
}

void ra_core_engine_init(ra_core_engine_t *e, uint32_t key, uint32_t addr_hi) {
    e->key = key;
    e->addr_hi = addr_hi;
    e->cons = key;
    e->it = 0;
    e->block_len = 0;
    e->block_pos = 0;
    ra_init_state_orbit(e->L, e->M, key, addr_hi);
}

// Generate n words, continuing from wherever this engine last left off.
// Byte-identical to the corresponding slice of
// ra_core_orbit(key, addr_hi, N, stream) for N >= (words emitted so far + n)
// -- both draw from the same sequence of full 255-word cycles.
void ra_core_engine_next(ra_core_engine_t *e, size_t n, uint32_t *out) {
    size_t written = 0;
    while (written < n) {
        if (e->block_pos >= e->block_len) ra_core_engine_fill_block(e);
        size_t avail = e->block_len - e->block_pos;
        size_t take = (n - written < avail) ? (n - written) : avail;
        memcpy(out + written, e->block_buf + e->block_pos, take * sizeof(uint32_t));
        e->block_pos += (uint32_t)take;
        written += take;
    }
}

// Total words emitted to callers so far (completed cycles' words minus
// whatever is still buffered/unconsumed from the most recent cycle).
uint64_t ra_core_engine_position(const ra_core_engine_t *e) {
    return (uint64_t)e->it * 255ull - (uint64_t)(e->block_len - e->block_pos);
}

// Metadata getters: exist so callers (the ctypes layer) never need to know
// ra_core_engine_t's byte layout -- state stays an opaque blob on the
// Python side, same philosophy as bindings/cpython/_ra_prng2.py's RA_PRNG.
uint32_t ra_core_engine_key(const ra_core_engine_t *e) { return e->key; }
uint32_t ra_core_engine_addr_hi(const ra_core_engine_t *e) { return e->addr_hi; }

// Fork semantics (confirmed 2026-09-07 design session): `child` becomes an
// independent engine whose L/M state has gone through ONE FORCED reseed
// (M ^= L, re-hash -> new cons) applied on top of a copy of `src`'s current
// state -- exactly ra_reseed(), the same step ra_core_engine_fill_block()
// already triggers naturally every 255 words, just invoked early for this
// branch only. `src` itself is left completely unmodified and keeps
// generating its own trajectory as if fork() had never been called.
void ra_core_engine_fork(const ra_core_engine_t *src, ra_core_engine_t *child) {
    *child = *src;
    child->cons = ra_reseed(child->M, child->L);
    child->block_len = 0; // discard src's cached partial block; child's next
    child->block_pos = 0; // .next() call fills a fresh post-reseed cycle
}

void ra_core_engine_save(const ra_core_engine_t *e, uint8_t *out) {
    memcpy(out, e, sizeof(ra_core_engine_t));
}

void ra_core_engine_load(ra_core_engine_t *e, const uint8_t *in) {
    memcpy(e, in, sizeof(ra_core_engine_t));
}

// ----------------------------------------------------------------------
// Deterministic Fisher-Yates shuffle (permutes indices [0, n) in place).
//
// Chaining note: the original plan for n > 255 was addr_hi-increment
// chaining (key fixed, addr_hi = block index) through repeated
// ra_core_singleblock calls. That pattern was PractRand-tested in
// experiments/2026-9-7_addr-hi-chaining-validation/ and FAILED
// catastrophically at the very first checkpoint (256MB) for both K=1 and
// K=255 -- see that folder's RESULTS.md. This function instead uses
// KEY-increment chaining (addr_hi fixed at 0, block_key = key + block_idx *
// 0x9E3779B9), the exact pattern already validated clean down to K=1 by
// the production-candidate-battery / K-threshold-characterization gates
// for the current ra_core.c formula.
//
// Range reduction: Fisher-Yates needs a uniform index in [0, i] for each
// shrinking i. Reducing a 32-bit word via plain modulo biases small
// ranges; this uses Lemire's fast multiply-shift method instead
// (`((uint64_t)word * range) >> 32`), which is the standard choice for
// this exact problem (same one NumPy/PCG use), single word per step, no
// rejection loop.
// ----------------------------------------------------------------------

#define RA_CORE_SHUFFLE_BLOCK_K 255u

void ra_core_shuffle_indices(uint32_t key, size_t n, uint32_t *out_indices) {
    for (size_t i = 0; i < n; ++i) out_indices[i] = (uint32_t)i;
    if (n < 2) return;

    uint32_t block[RA_CORE_SHUFFLE_BLOCK_K];
    uint32_t block_len = 0, block_pos = 0;
    uint64_t block_idx = 0;

    for (size_t i = n - 1; i > 0; --i) {
        if (block_pos >= block_len) {
            size_t remaining_steps = i; // steps left including this one (i, i-1, ..., 1)
            block_len = (uint32_t)((remaining_steps < RA_CORE_SHUFFLE_BLOCK_K)
                                        ? remaining_steps
                                        : RA_CORE_SHUFFLE_BLOCK_K);
            uint32_t block_key = key + (uint32_t)(block_idx * 0x9E3779B9u);
            ra_core_singleblock_into(block_key, /*addr_hi=*/0u, block_len, block);
            block_pos = 0;
            ++block_idx;
        }
        uint32_t word = block[block_pos++];
        size_t range = i + 1; // draw j uniformly from [0, i]
        size_t j = (size_t)(((uint64_t)word * (uint64_t)range) >> 32);
        uint32_t tmp = out_indices[i];
        out_indices[i] = out_indices[j];
        out_indices[j] = tmp;
    }
}
