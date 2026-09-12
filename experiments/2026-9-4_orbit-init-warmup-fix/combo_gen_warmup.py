"""Per-variant C codegen for the orbit init-warmup fix experiment
(HANDOVER.md), structurally a port of
../2026-9-4_orbit-init-cycle-combo-search/combo_gen_orbit.py's
write_and_compile/sanity_check pattern.

Unlike the sibling combo-search, init-formula and cycle-transform are FIXED
to production (that session proved 0/32 combos escape the defect) -- only
the seed values of `a`/`d` at it==0 vary, across 3 fixed variants: control
(production, d0=0), d0_hash (Candidate A), d0_realround (Candidate B).

CRITICAL deviation from the sibling's codegen: the loop body here is a
LITERAL, VERBATIM copy of ra_core.c's ra_permutation_cycle_orbit
(L217-251), including the rolling-register `o`/`o_term7` maintenance --
NOT the sibling's naive-tap-recompute simplification. HANDOVER.md's
constraint is stricter here ("loop body TIDAK disentuh sama sekali") since
only the *seed values fed into it* are under test, not the loop itself.

Self-contained diagnostic: copies (does not modify) rot32/fmix32/GUARD_L/
GUARD_M/ra_hash/ra_reseed byte-for-byte from
../2026-9-1_family-productionization/ra_core.c (L144-183), plus splitmix32
byte-for-byte from ../2026-9-1_family-productionization/
multikey_stream_hashed.c (L16-22). ra_core.c itself is NOT touched.

fmemopen glibc NUL-terminator bug (HANDOVER.md 9) does NOT apply here --
sanity_check() reads subprocess.run(...).stdout directly (a real pipe).

CLI surface of each generated candidate binary (HANDOVER.md 6):
  <bin> --single <key> <rng>                    one call, sanity check
  <bin> --stream <base_key> <n> <K>              Weyl-stepped multikey stream
  <bin> --stream-hashed <base_key> <n> <K>       SplitMix32-hashed multikey stream
"""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

import combo_prng_warmup

HERE = Path(__file__).parent
CAND_DIR = HERE / "candidates"
CAND_DIR.mkdir(exist_ok=True)

VARIANTS = combo_prng_warmup.VARIANTS  # ("control", "d0_hash", "d0_realround")

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
/* GUARD_D: popcount=16, sha256 attempt #24, distinct from GUARD_L/GUARD_M
 * and the 4 pre-existing constants -- see pick_guard_constant_d.py output. */
#define GUARD_D 0x17067f32u

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

# --- init-formula: FIXED to production (init_linear_shared), not varied ---
INIT_FN = """
static void ra_init_state_orbit_combo(uint32_t *L, uint32_t *M, uint32_t key) {
    uint32_t keyterm_l = fmix32(GUARD_L ^ (0x9e3779b7u * key));
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * 0x9e3779b7u) * keyterm_l;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}
"""

# --- ra_init_warmup_orbit bodies (the ONLY thing that differs per variant) ---
WARMUP_FN = {
    "control": """
static void ra_init_warmup_orbit(uint32_t key, const uint32_t *M, uint32_t *a0, uint32_t *d0) {
    (void)M;
    *a0 = key;
    *d0 = 0u;
}
""",
    "d0_hash": """
static void ra_init_warmup_orbit(uint32_t key, const uint32_t *M, uint32_t *a0, uint32_t *d0) {
    (void)M;
    *d0 = fmix32(GUARD_D ^ key) & 0xFFu;
    *a0 = key;
}
""",
    "d0_realround": """
static void ra_init_warmup_orbit(uint32_t key, const uint32_t *M, uint32_t *a0, uint32_t *d0) {
    uint32_t o255 = (M[(uint8_t)(255 + 0)] << 0) ^ (M[(uint8_t)(255 + 1)] << 1) ^
                    (M[(uint8_t)(255 + 2)] << 2) ^ (M[(uint8_t)(255 + 3)] << 3) ^
                    (M[(uint8_t)(255 + 4)] << 4) ^ (M[(uint8_t)(255 + 5)] << 5) ^
                    (M[(uint8_t)(255 + 6)] << 6) ^ (M[(uint8_t)(255 + 7)] << 7);
    uint32_t a_warm = (0u ^ o255) ^ (key + key);
    uint32_t b_warm = (key + a_warm) ^ (o255 + 0u);
    uint32_t c_warm = rot32((a_warm >> 13) ^ a_warm, b_warm);
    c_warm ^= c_warm >> 17u;
    *d0 = c_warm & 0xFFu;
    *a0 = a_warm;
}
""",
}

# --- cycle body: LITERAL VERBATIM copy of ra_core.c L217-251 (rolling
# register o/o_term7), only the a/d initializer line changed vs production ---
CYCLE_AND_CORE = """
static void ra_permutation_cycle_orbit_combo(uint32_t cons, size_t it,
                                              uint32_t a0, uint32_t d0,
                                              const uint32_t *M, uint32_t *L,
                                              uint64_t *count, FILE *raw_stream) {
    uint32_t a = (it == 0) ? a0 : cons;
    uint32_t b = (uint32_t)it, c = 0;
    uint32_t d = (it == 0) ? d0 : 0u;

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

static uint32_t ra_core_orbit_combo(uint32_t key, size_t rng, FILE *raw_stream) {
    if (rng == 0) return key;

    alignas(64) uint32_t L[256], M[256];
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;

    uint32_t cons = key;
    ra_init_state_orbit_combo(L, M, key);

    uint32_t a0, d0;
    ra_init_warmup_orbit(key, M, &a0, &d0);

    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_orbit_combo(cons, it, a0, d0, M, L, &count, raw_stream);
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


def render_c(variant: str) -> str:
    header = (
        f"// {variant}.c -- auto-generated by combo_gen_warmup.py, DO NOT EDIT BY HAND\n"
        f"// orbit-init-warmup-fix experiment, variant={variant}\n"
        f"// Loop body (ra_permutation_cycle_orbit_combo) is a LITERAL VERBATIM copy\n"
        f"// of ra_core.c's ra_permutation_cycle_orbit (L217-251) -- only ra_init_warmup_orbit\n"
        f"// (the a0/d0 seed at it==0) differs across control/d0_hash/d0_realround.\n"
        f"// rot32/fmix32/GUARD_L/GUARD_M/ra_hash/ra_reseed copied verbatim from\n"
        f"// ../../2026-9-1_family-productionization/ra_core.c (L144-183).\n"
        f"// splitmix32 copied verbatim from\n"
        f"// ../../2026-9-1_family-productionization/multikey_stream_hashed.c (L16-22).\n"
        f"// ra_core.c is NOT modified by this file.\n\n"
    )
    return header + PREAMBLE + INIT_FN + WARMUP_FN[variant] + CYCLE_AND_CORE + MAIN_FN


def write_and_compile(variant: str) -> Path:
    c_path = CAND_DIR / f"{variant}.c"
    bin_path = CAND_DIR / variant
    c_path.write_text(render_c(variant))
    subprocess.run(
        ["gcc", "-O3", "-march=native", "-std=gnu17", "-include", "stdalign.h",
         str(c_path), "-o", str(bin_path), "-Wall", "-Wextra"],
        check=True, capture_output=True, text=True,
    )
    return bin_path


def sanity_check(bin_path: Path, variant: str, key: int = 12345, rng: int = 37) -> None:
    """Compare compiled binary's --single output against
    combo_prng_warmup.py's orbit_stream_full() oracle, byte-for-byte."""
    result = subprocess.run(
        [str(bin_path), "--single", str(key), str(rng)],
        capture_output=True,
    )
    c_words = [result.stdout[i:i + 4] for i in range(0, len(result.stdout), 4)]
    c_ints = [int.from_bytes(w, "little") for w in c_words]
    py_ints = combo_prng_warmup.orbit_stream_full(key, rng, variant)
    if c_ints != py_ints:
        raise SystemExit(
            f"{variant} (key={key},rng={rng}): SANITY CHECK FAILED -- C output != "
            f"combo_prng_warmup.py oracle (codegen bug). "
            f"C={c_ints[:5]}... PY={py_ints[:5]}..."
        )


def verify_control_matches_production() -> None:
    """The mandatory hand-trace: control (production behavior byte-for-byte)
    must reproduce production ra_core_orbit(key=1, rng=1) exactly."""
    bin_path = write_and_compile("control")
    result = subprocess.run([str(bin_path), "--single", "1", "1"], capture_output=True)
    c_val = int.from_bytes(result.stdout[:4], "little")
    py_val = combo_prng_warmup.orbit_round1(1, "control")
    if c_val != py_val:
        raise SystemExit(f"CONTROL MISMATCH vs combo_prng_warmup.py: C={c_val} PY={py_val}")
    expected_production = 397095967  # ra_core --stream orbit 1 1
    if c_val != expected_production:
        raise SystemExit(
            f"CONTROL MISMATCH vs production ra_core_orbit: "
            f"C={c_val} expected={expected_production}"
        )
    print(f"control OK: matches production ra_core_orbit byte-for-byte ({c_val})")


def generate_all(verbose: bool = True) -> list[tuple[str, Path]]:
    results = []
    for variant in VARIANTS:
        bin_path = write_and_compile(variant)
        sanity_check(bin_path, variant)
        # extra sanity call exercising it>=1 (rng>255 forces >=1 ra_reseed)
        sanity_check(bin_path, variant, key=777, rng=2000)
        results.append((variant, bin_path))
        if verbose:
            print(f"  built+sanity-OK (rng=37 and rng=2000): {variant}")
    return results


if __name__ == "__main__":
    verify_control_matches_production()
    if "--all" in sys.argv:
        built = generate_all()
        print(f"Built and sanity-checked {len(built)} variants in {CAND_DIR}")
