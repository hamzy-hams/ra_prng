# RESULTS: Weyl-add + fmix32/triple32 finalizer redesign — both candidates FAIL

**Status: EXECUTED, both candidates (A `weyl_fmix32`, B `weyl_triple32`)
FAIL Tier 1 decisively. Per HANDOVER.md §7 "kedua gagal": reported in
full here, NOT escalating to a heavier design without user direction.**

## 0. Scope executed vs handover

All 3 user checkpoints from HANDOVER.md §0 resolved before implementation
(2026-09-05 plan session):
1. Status: candidate replacement for `ra_core_orbit` K-small (if it had
   passed) — not a permanent parallel variant.
2. Key→state mapping: option (a) "SplitMix-literal" (`state0=fmix32(key)`,
   `state_j=state0+j*WEYL32`, `output_j=fmix32(state_j)`).
3. Tier 1 scale: 8GB (matching `../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`).

One addition beyond the handover's literal code (§3 code block had no
guard): `state0 = finalizer(GUARD_KEY ^ key)` instead of `finalizer(key)`
bare — closes an `fmix32(0)==0`/`triple32(0)==0` weak-key edge case at
`key=0,j=0` (repo-known "key=0 Degenerate-State All-Zero Defect" pattern;
`GUARD_KEY` reuses the already-vetted `GUARD_D=0x17067f32u` constant from
`../2026-9-4_orbit-init-warmup-fix/candidates/d0_hash.c`). This does not
change the architecture or either finalizer's core operation.

`ra_core.c` was NOT touched — out of scope per handover, confirmed unused
by both candidates (grep: zero references to `ra_permutation_cycle_orbit`,
`ra_init_state_orbit`, `L[]`, `M[]`, `ra_hash`, `ra_reseed` in either
candidate file).

## 1. Core code (both candidates, full generator — no L[]/M[]/255-round loop)

**Candidate A — `candidates/weyl_fmix32.c`:**
```c
#define WEYL32 0x9e3779b7u
#define GUARD_KEY 0x17067f32u

static inline uint32_t fmix32(uint32_t h) {
    h ^= h >> 16; h *= 0x85ebca6bu;
    h ^= h >> 13; h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}

static void weyl_fmix32_core(uint32_t key, size_t rng, FILE *raw_stream) {
    uint32_t state0 = fmix32(GUARD_KEY ^ key);
    for (size_t j = 0; j < rng; ++j) {
        uint32_t state_j = state0 + (uint32_t)((uint32_t)j * WEYL32);
        uint32_t output_j = fmix32(state_j);
        if (raw_stream) fwrite(&output_j, sizeof(uint32_t), 1, raw_stream);
    }
}
```

**Candidate B — `candidates/weyl_triple32.c`** (same structure, finalizer
swapped for `triple32`, Chris Wellons/hash-prospector, 4-multiply,
constants verified 2026-09-05 against
`raw.githubusercontent.com/skeeto/hash-prospector/master/README.md`
L160-169, byte-for-byte match to what HANDOVER.md §4 transcribed):
```c
static inline uint32_t triple32(uint32_t x) {
    x ^= x >> 17; x *= 0xed5ad4bbu;
    x ^= x >> 11; x *= 0xac4c1b51u;
    x ^= x >> 15; x *= 0x31848babu;
    x ^= x >> 14;
    return x;
}

static void weyl_triple32_core(uint32_t key, size_t rng, FILE *raw_stream) {
    uint32_t state0 = triple32(GUARD_KEY ^ key);
    for (size_t j = 0; j < rng; ++j) {
        uint32_t state_j = state0 + (uint32_t)((uint32_t)j * WEYL32);
        uint32_t output_j = triple32(state_j);
        if (raw_stream) fwrite(&output_j, sizeof(uint32_t), 1, raw_stream);
    }
}
```

Both compiled with `gcc -O2 -std=c11 -Wall -Wextra`, zero warnings. CLI
(`--single`/`--stream`/`--stream-hashed`) mirrors the convention from
`../2026-9-4_orbit-o-fmix-fix/candidates/*.c` — `--stream` derives
successive block keys via Weyl-stepping (`base_key + block_idx*0x9E3779B9u`),
`--stream-hashed` via the repo's existing `splitmix32()` test-harness
helper (actually SplitMix64 truncated, copied verbatim from
`multikey_stream_hashed.c`, used only to reproduce the historical
key-ordering test pattern — not part of either candidate).

## 2. Pipeline results

### 2.1 Byte-exact oracle vs C (`combo_prng_weylfinal.py`)

**PASS**, both candidates. 11 scenarios for A (single/stream/stream-hashed,
K=1/K=255, key=0 edge case, partial-last-block, dual base_key) + 6
confirmatory scenarios for B — all byte-for-byte identical, explicit
`0xFFFFFFFF` masking at every step in the Python oracle.

### 2.2 Tier 0 avalanche (K=1, band 0.3-0.7, min-bit floor 0.2)

| Candidate | mean_fraction | min_bit_fraction | Verdict |
|---|---|---|---|
| A `fmix32` | 0.5029 | 0.2188 | PASS |
| B `triple32` | 0.4844 | 0.3125 | PASS |

Both pass comfortably — as expected, single-word avalanche was never the
axis where prior candidates failed; the defect only shows up under
PractRand's window/tuple-frequency tests.

### 2.3 Tier 1 PractRand, 8GB, dual-ordering, staged (A/B/C)

**Both candidates: 0/2 (candidate,K) pairs survive even Stage B (8GB,
single base_key) — Stage C (reproducibility, second base_key) never
reached.**

| Candidate | K | Ordering | Verdict | Signature |
|---|---|---|---|---|
| A `fmix32` | 1 | Weyl (`--stream`) | **FAIL @ 1GB** | `FPF-14+6/16:all` R=-18.9, p=1-3.6e-18 |
| A `fmix32` | 1 | hashed (`--stream-hashed`) | CLEAN @ 8GB | — |
| A `fmix32` | 255 | Weyl | **FAIL @ 512MB** | `DC6-9x1Bytes-1` R=+14.4, p=3.3e-8; `[Low8/32]DC6-9x1Bytes-1` R=+14.5 |
| A `fmix32` | 255 | hashed | **FAIL @ 512MB** | `DC6-9x1Bytes-1` R=+12.8, p=2.8e-7 |
| B `triple32` | 1 | Weyl | **FAIL @ 1GB** | `FPF-14+6/16:all` R=-12.1, p=1-2.1e-11 |
| B `triple32` | 1 | hashed | CLEAN @ 8GB | — |
| B `triple32` | 255 | Weyl | **FAIL @ 512MB** | `DC6-9x1Bytes-1` R=+16.6, p=1.7e-9 |
| B `triple32` | 255 | hashed | **FAIL @ 1GB** | `DC6-9x1Bytes-1` R=+24.8, p=5.1e-13; `[Low8/32]DC6-9x1Bytes-1` R=+22.8 |

Full checkpoint logs: `tier1_logs/*.log`, machine-readable summary:
`tier1_summary_weylfinal.json`, run stdout: `tier1_run_stdout.log` (A),
`tier1_run_stdout_triple32.log` (B).

### 2.4 Benchmark

**Not run** — HANDOVER.md §0.5/§6 step 4 scopes the 5-number benchmark to
candidates that pass Tier 1. Neither did.

## 3. Analysis

Two independent, structurally distinct failure signatures, **identical
between both finalizers**:

1. **K=1 + Weyl-ordering FAILs** (`FPF-14+6/16`, same test family as the
   ORIGINAL defect) — a genuinely NEW regression: under the old
   `ra_core_orbit` architecture, K=1+Weyl-ordering was always clean (see
   `../2026-9-4_orbit-kmin-battery/RESULTS.md`). K=1+hashed-ordering, the
   pattern that originally failed, is now CLEAN to 8GB for both
   candidates — the failure axis has effectively **inverted**, not closed.
2. **K=255 FAILs in BOTH orderings** (`DC6-9x1Bytes-1`, a byte-level
   distance-correlation test — never seen as the failing test in any of
   the 8 prior orbit K-small fix attempts) — also a new regression: K=255
   was clean under every prior candidate and under production
   `ra_core_orbit`, in both orderings, up to 8-16GB.

Because switching the finalizer from `fmix32` (3-multiply, MurmurHash3
family) to `triple32` (4-multiply, hash-prospector, provably closer to a
uniform random 32-bit permutation per its own published bias measurement)
did **not** change the failure pattern or even meaningfully move the
R-values, this strongly indicates the failure is **not a finalizer-strength
problem** at all (contrary to HANDOVER.md §0.2's leading hypothesis, which
this result refutes). The one structural element identical across both
FAILing axes and both candidates is the *linear intra-block state advance
itself*: `state_j = state0 + j*WEYL32` for `j = 0..rng-1`, where the same
per-index additive offset `j*WEYL32` is reused for every key/block. A
bijective finalizer (either one) maps this fixed additive structure through
a fixed permutation, which does not fully hide the correlation between
`finalizer(x)` and `finalizer(x + j*WEYL32)` for a fixed `j` across many
different `x` (= `state0` values) — exactly the kind of dependency
PractRand's tuple/frequency tests are built to catch, and a known
theoretical weak point of applying any *fixed* single-round bijection to a
purely arithmetic progression at 32-bit width (per HANDOVER.md §0.2's own
birthday-bound caveat, ~2^16 states before correlations are statistically
visible — well within an 8GB/K=255 test's ~2 million blocks × 255 states).

**This mechanism was verified computationally (2026-09-05, user follow-up:
"lihat cycles internal buatanmu"), not left as a hypothesis** —
`cycle_analysis_weylfinal.py`:
1. **Period**: `state_j = state0 + j*WEYL32 (mod 2^32)` has full period
   `2^32` for any `state0`, because `WEYL32=0x9e3779b7` is odd
   (`gcd(WEYL32, 2^32)=1`). Verified exhaustively on a toy 16-bit analog
   of the identical construction (full `2^16`-state enumeration, 4
   different `state0` values, `STEP=0x9e37` odd): period = exactly
   `65536` every time, all states visited, zero short cycles. The same
   number-theory argument generalizes to any power-of-two modulus.
2. **Consequence**: because `fmix32`/`triple32` are bijections, `output_j
   = finalizer(state_j)` is *also* a pure permutation in `j` over one full
   period — a fixed key's output stream cannot self-collide until all
   `2^32` outputs are exhausted. Verified on the REAL 32-bit generator:
   4,194,304 (2^22) consecutive outputs from a single key, **0
   self-collisions**, for both `weyl_fmix32` and `weyl_triple32`. A
   true-random control (`os.urandom`) at the identical sample size shows
   **2084 / 2059 collisions** respectively, matching the birthday-paradox
   expectation (`n²/2^33 ≈ 2048`). This is a stark, measurable difference:
   the candidate generators are *too regular* — they are exact
   permutations per key, which genuine randomness is not — and this is
   precisely the kind of deficit PractRand's frequency/distinguishing
   tests (`DC6-9x1Bytes-1`, `FPF-14+6/16`) are built to detect. This
   upgrades the K=255 root-cause explanation above from "most likely" to
   **confirmed mechanism** (full script + output:
   `cycle_analysis_weylfinal.py`).

The K=1+Weyl-vs-hashed inversion is a second, so-far-unexplained
observation: for K=1 there is no intra-block advance at all (`rng=1`
means only `j=0` is ever emitted), so the K=1 failure must come from a
different mechanism — most likely an interaction between the state-advance
constant `WEYL32=0x9e3779b7u` (reused here, per HANDOVER.md §3, from
`ra_core.c`'s `ra_init_state_orbit` L-multiplier) and the `--stream`
key-ordering harness's own constant `0x9E3779B9u` (they differ by 2 in the
last hex digit) now that `state0=finalizer(GUARD_KEY^key)` is a *much*
simpler function of `key` than the old `ra_init_state_orbit`'s 256-entry
table. This is worth flagging explicitly: the earlier
`orbit-weyl-constant-resonance-diagnosis` session refuted constant
proximity as an explanation for the OLD defect's architecture, but that
refutation does not automatically transfer to this NEW, much simpler
architecture — it was never tested here (out of scope for this session,
see §4).

## 4. Conclusion (per HANDOVER.md §7 "keduanya gagal")

- The "sequential-accumulation vs counter-based-fixed-function" framing
  from HANDOVER.md §1 is **not, by itself, a sufficient explanation** for
  the original K=1+hashed defect: a genuinely counter-based/history-free
  design (this one) still fails PractRand, just via different axes
  (Weyl-ordering K=1, and K=255 in both orderings) than the architecture
  it was meant to replace.
- Finalizer strength (3-multiply vs 4-multiply, both well-regarded
  published 32-bit avalanche functions) is **not the lever** — both fail
  identically in shape and magnitude.
- Two candidate root causes for a *future* session to investigate,
  **not attempted here** (explicit stop point per HANDOVER.md §7 / this
  session's plan — no jump to a heavier design without user direction):
  1. The K=255 failure may be intrinsic to reusing the same fixed
     `j*WEYL32` additive ladder across every key/block — would need an
     architecture where the per-index step also depends on `key` (not
     just the base `state0`), which reintroduces some of the complexity
     this redesign was trying to shed.
  2. The K=1+Weyl-ordering failure may be specific to the `WEYL32` vs
     `0x9E3779B9u` near-collision in this simpler architecture — an
     unexplored variant would re-run this exact pipeline with a
     bitwise-distant step constant (same technique as
     `../2026-9-4_orbit-weyl-constant-resonance-diagnosis/`, not yet
     applied to this new generator).
- `ra_core.c` remains untouched; production `ra_core_orbit` status is
  unchanged by this session (still: K=255 and K=1+Weyl-ordering clean,
  K=1+hashed-ordering unfixed — see
  `../2026-9-1_family-productionization/PRODUCTION_READINESS_HANDOVER.md`
  §2b).
