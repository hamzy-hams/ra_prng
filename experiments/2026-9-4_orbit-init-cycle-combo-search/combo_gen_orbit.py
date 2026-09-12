"""Per-candidate C codegen for the orbit init-formula x cycle-transform
combo search, structurally a port of
../2026-9-2_singleblock-cycle-combo-search/combo_gen.py's TEMPLATE/
write_and_compile/sanity_check pattern to ra_core_orbit.

Self-contained diagnostic: copies (does not modify) rot32/fmix32/GUARD_L/
GUARD_M/ra_hash/ra_reseed byte-for-byte from
../2026-9-1_family-productionization/ra_core.c (L144-183), plus splitmix32
byte-for-byte from ../2026-9-1_family-productionization/
multikey_stream_hashed.c (L16-22). ra_permutation_cycle_orbit is `static`
in ra_core.c so it cannot be extern-linked -- verbatim copy instead, same
convention combo_gen.py established. ra_core.c itself is NOT touched.

Deliberate simplification (proven legal, not assumed): the codegen
recomputes the 8-tap `o` naively every iteration
(o = XOR_{e=0}^{7} M[(i+e)&0xFF]<<e) instead of porting production's
incremental rolling-register update. Production's own comment states these
are bit-exact identical. This is re-verified here empirically by the
mandatory control-combo check in verify_control_matches_production_orbit()
(byte-exact vs the real ra_core binary) before any other candidate is
trusted.

Note on ra_hash/ra_reseed: dead code for rng<=255 (see
combo_prng_orbit.py's ra_reseed docstring for the trace) but ported
verbatim anyway for structural fidelity / future Promotion-tier (rng>255)
use, per HANDOVER.md 6's requirement.

fmemopen glibc NUL-terminator bug (HANDOVER.md 9) does NOT apply here --
sanity_check_orbit() reads subprocess.run(...).stdout directly (a real
pipe), never fmemopen.

CLI surface of each generated candidate binary (HANDOVER.md 6):
  <bin> --single <key> <rng>                    one call, sanity check
  <bin> --stream <base_key> <n> <K>              Weyl-stepped multikey stream
  <bin> --stream-hashed <base_key> <n> <K>       SplitMix32-hashed multikey stream
"""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

from recipes_orbit import (
    INIT_VARIANTS,
    CYCLE_VARIANTS,
    idx_c_expr,
    combo_id,
    combo_describe,
    all_combos,
    CONTROL_COMBO,
)
import combo_prng_orbit

HERE = Path(__file__).parent
CAND_DIR = HERE / "candidates"
CAND_DIR.mkdir(exist_ok=True)

# --- shared preamble: rot32/fmix32/GUARDs/ra_hash/ra_reseed/splitmix32 ---
PREAMBLE = r"""#include <stdio.h>
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

/* splitmix32 -- copied verbatim from
 * ../2026-9-1_family-productionization/multikey_stream_hashed.c L16-22 */
static uint32_t splitmix32(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    z = z ^ (z >> 31);
    return (uint32_t)(z >> 32);
}
"""

# --- init-formula bodies (axis 1), keyed by KEYTERM_MODE ---
INIT_FN_SHARED = """
static void ra_init_state_orbit_combo(uint32_t *L, uint32_t *M, uint32_t key) {
    uint32_t keyterm_l = fmix32(GUARD_L ^ (0x9e3779b7u * key));
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t idxv = __IDX_EXPR__;
        uint32_t l_val = (uint32_t)(idxv * 0x9e3779b7u) * keyterm_l;
        uint32_t m_val = (uint32_t)(idxv * 0x06a0dd9bu) * keyterm_m;
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}
"""

INIT_FN_PER_INDEX = """
static void ra_init_state_orbit_combo(uint32_t *L, uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t idxv = __IDX_EXPR__;
        uint32_t l_val = fmix32(GUARD_L ^ (0x9e3779b7u * key) ^ idxv);
        uint32_t m_val = fmix32(GUARD_M ^ (0x06a0dd9bu * key) ^ idxv);
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}
"""

# --- cycle body (axis 2): naive tap_o8 recompute + L-swap + reseed loop ---
CYCLE_AND_CORE = """
static void ra_permutation_cycle_orbit_combo(uint32_t cons, size_t it,
                                              const uint32_t *M, uint32_t *L,
                                              uint64_t *count, FILE *raw_stream) {
    uint32_t a = cons, d = 0;
    (void)it;

    for (uint32_t i = 255; i > 0; --i) {
        uint32_t o = (M[(uint8_t)(i + 0)] << 0) ^ (M[(uint8_t)(i + 1)] << 1) ^
                     (M[(uint8_t)(i + 2)] << 2) ^ (M[(uint8_t)(i + 3)] << 3) ^
                     (M[(uint8_t)(i + 4)] << 4) ^ (M[(uint8_t)(i + 5)] << 5) ^
                     (M[(uint8_t)(i + 6)] << 6) ^ (M[(uint8_t)(i + 7)] << 7);

        a = (d ^ o) ^ (cons + a);
        uint32_t b = (cons + a) ^ (o + d);
        uint32_t c = rot32((a >> 13) ^ a, b);
__CYCLE_STMTS__
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);

        d = c & 0xFFu;

        if (*count <= 1) break;
        --(*count);

        uint32_t tmp = L[i];
        L[i] = L[d];
        L[d] = tmp;
    }
}

static uint32_t ra_core_orbit_combo(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_state_orbit_combo(L, M, key);

    for (size_t it2 = 0; it2 < iteration; ++it2) {
        ra_permutation_cycle_orbit_combo(cons, it2, M, L, &count, raw_stream);
        if (count <= 1) return cons;
        cons = ra_reseed(M, L);
    }
    return cons;
}
"""

MAIN_FN = """
int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--single") == 0 && argc >= 4) {
        uint32_t key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t rng = (size_t)strtoull(argv[3], NULL, 0);
        ra_core_orbit_combo(key, rng, stdout);
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--stream") == 0 && argc >= 5) {
        uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t n = (size_t)strtoull(argv[3], NULL, 0);
        size_t K = (size_t)strtoull(argv[4], NULL, 0);
        if (K == 0 || K > 255) { fprintf(stderr, "K must be in [1,255]\\n"); return 1; }
        uint64_t block_idx = 0;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < K ? remaining : K;
            uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
            ra_core_orbit_combo(key, block_len, stdout);
            remaining -= block_len;
            ++block_idx;
        }
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--stream-hashed") == 0 && argc >= 5) {
        uint32_t base_key = (uint32_t)strtoul(argv[2], NULL, 0);
        size_t n = (size_t)strtoull(argv[3], NULL, 0);
        size_t K = (size_t)strtoull(argv[4], NULL, 0);
        if (K == 0 || K > 255) { fprintf(stderr, "K must be in [1,255]\\n"); return 1; }
        uint64_t state = (uint64_t)(uint32_t)base_key;
        size_t remaining = n;
        while (remaining > 0) {
            size_t block_len = remaining < K ? remaining : K;
            uint32_t key = splitmix32(&state);
            ra_core_orbit_combo(key, block_len, stdout);
            remaining -= block_len;
        }
        return 0;
    }
    fprintf(stderr,
        "usage:\\n"
        "  %s --single <key> <rng>\\n"
        "  %s --stream <base_key> <n> <K>\\n"
        "  %s --stream-hashed <base_key> <n> <K>\\n",
        argv[0], argv[0], argv[0]);
    return 1;
}
"""


def render_c(init_id: str, cycle_id: str) -> str:
    cid = combo_id(init_id, cycle_id)
    desc = combo_describe(init_id, cycle_id)

    _, idx_name, keyterm_mode = next(v for v in INIT_VARIANTS if v[0] == init_id)
    idx_expr = idx_c_expr(idx_name)
    init_fn_template = INIT_FN_SHARED if keyterm_mode == "shared" else INIT_FN_PER_INDEX
    init_fn = init_fn_template.replace("__IDX_EXPR__", idx_expr)

    _, _, stages = next(v for v in CYCLE_VARIANTS if v[0] == cycle_id)
    cycle_stmts = "\n".join(f"        {st.c_stmt()}" for st in stages)
    cycle_and_core = CYCLE_AND_CORE.replace("__CYCLE_STMTS__", cycle_stmts)

    header = (
        f"// {cid}.c -- auto-generated by combo_gen_orbit.py, DO NOT EDIT BY HAND\n"
        f"// {desc}\n"
        f"// rot32/fmix32/GUARD_L/GUARD_M/ra_hash/ra_reseed copied verbatim from\n"
        f"// ../../2026-9-1_family-productionization/ra_core.c (L144-183).\n"
        f"// splitmix32 copied verbatim from\n"
        f"// ../../2026-9-1_family-productionization/multikey_stream_hashed.c (L16-22).\n"
        f"// ra_core.c is NOT modified by this file.\n\n"
    )

    return header + PREAMBLE + init_fn + cycle_and_core + MAIN_FN


def write_and_compile(init_id: str, cycle_id: str) -> Path:
    cid = combo_id(init_id, cycle_id)
    c_path = CAND_DIR / f"{cid}.c"
    bin_path = CAND_DIR / cid
    c_path.write_text(render_c(init_id, cycle_id))
    subprocess.run(
        ["gcc", "-O3", "-march=native", "-std=gnu17", "-include", "stdalign.h",
         str(c_path), "-o", str(bin_path), "-Wall", "-Wextra"],
        check=True, capture_output=True, text=True,
    )
    return bin_path


def sanity_check(bin_path: Path, init_id: str, cycle_id: str,
                  key: int = 12345, rng: int = 37) -> None:
    """Compare compiled binary's --single output (multi-round, rng=37)
    against combo_prng_orbit.py's orbit_stream_full() oracle, byte-for-byte."""
    result = subprocess.run(
        [str(bin_path), "--single", str(key), str(rng)],
        capture_output=True,
    )
    c_words = [result.stdout[i:i + 4] for i in range(0, len(result.stdout), 4)]
    c_ints = [int.from_bytes(w, "little") for w in c_words]
    py_ints = combo_prng_orbit.orbit_stream_full(key, rng, init_id, cycle_id)
    cid = combo_id(init_id, cycle_id)
    if c_ints != py_ints:
        raise SystemExit(
            f"{cid}: SANITY CHECK FAILED -- C output != combo_prng_orbit.py oracle "
            f"(codegen bug). C={c_ints[:5]}... PY={py_ints[:5]}..."
        )


def verify_control_matches_production() -> None:
    """The mandatory hand-trace: (init_linear_shared, w8_f10_i0) must
    reproduce production ra_core_orbit(key=1, rng=1) exactly."""
    init_id, cycle_id = CONTROL_COMBO
    bin_path = write_and_compile(init_id, cycle_id)
    result = subprocess.run([str(bin_path), "--single", "1", "1"], capture_output=True)
    c_val = int.from_bytes(result.stdout[:4], "little")
    py_val = combo_prng_orbit.orbit_round1(1, init_id, cycle_id)
    if c_val != py_val:
        raise SystemExit(f"CONTROL MISMATCH vs combo_prng_orbit.py: C={c_val} PY={py_val}")
    expected_production = 397095967  # ra_core --stream orbit 1 1
    if c_val != expected_production:
        raise SystemExit(
            f"CONTROL MISMATCH vs production ra_core_orbit: "
            f"C={c_val} expected={expected_production}"
        )
    print(f"control ({combo_id(init_id, cycle_id)}) OK: matches production ra_core_orbit byte-for-byte ({c_val})")


def generate_all(verbose: bool = True) -> list[tuple[str, str, Path]]:
    results = []
    for init_id, cycle_id in all_combos():
        bin_path = write_and_compile(init_id, cycle_id)
        sanity_check(bin_path, init_id, cycle_id)
        results.append((init_id, cycle_id, bin_path))
        if verbose:
            print(f"  built+sanity-OK: {combo_id(init_id, cycle_id)}")
    return results


if __name__ == "__main__":
    verify_control_matches_production()
    if "--all" in sys.argv:
        built = generate_all()
        print(f"Built and sanity-checked {len(built)} combos in {CAND_DIR}")
