# RESULTS: key=0 degenerate-state guard-XOR fix

## Summary

The guard-XOR fix (user-specified 2026-09-01) **closes the key=0 defect
with no regression on the K=255 usage pattern**, and **has been applied to
`ra_core.c`** (see "Applied to ra_core.c" below). While verifying it,
this session also found a **separate, pre-existing, much more severe
defect in the K=1 singleblock usage pattern** — proven NOT caused by this
fix (a no-guard control run fails identically). That finding is documented
at the bottom and is now the blocker for `production-candidate-battery`'s
remaining K=1 work; it needs a user decision, not a guess, before anyone
continues.

## The fix

Root cause (from `../2026-9-1_production-candidate-battery/RESULTS.md`):
`keyterm_l/keyterm_m = fmix32(CONST*key)`; at `key=0` both are `fmix32(0)
== 0` (fmix32 is a bijection fixing 0), and since the combine is a
**multiply** (`l_val = (i*C)*keyterm_l`), a zero keyterm zeroes `L[]`/`M[]`
entirely — a permanent all-zero output (stable fixed point: `cons=key=0`
too, so `a=b=c=d=0` never leaves zero).

Fix (exact form specified by user, not redesigned): XOR a guard constant
into the input **before** `fmix32`:

```c
// before (ra_core.c, current production formula)
uint32_t keyterm_l = fmix32(0x9e3779b7u * key);
uint32_t keyterm_m = fmix32(0x06a0dd9bu * key);

// after
uint32_t keyterm_l = fmix32(GUARD_L ^ (0x9e3779b7u * key));
uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
```

## Guard constants

Requirements (user-specified): popcount==16 (equal 0/1 bits in 32 bits),
distinct from every constant already in the init path, distinct from each
other. Derived deterministically in `pick_guard_constants.py` (SHA-256 of
a descriptive label, scan until popcount==16 and not excluded — not
hand-picked):

| Constant | Value | Popcount |
|---|---|---|
| `0x9e3779b7` (L keyterm mul, existing) | — | 21 |
| `0x06a0dd9b` (M keyterm mul, existing) | — | 15 |
| `0x85ebca6b` (fmix32 c1, existing) | — | 18 |
| `0xc2b2ae35` (fmix32 c2, existing) | — | 16 |
| **`GUARD_L`** | `0x38916df4` | **16** |
| **`GUARD_M`** | `0x6c26fc92` | **16** |

Run `python3 pick_guard_constants.py` to reproduce/verify.

## Verification

### 1. Diagnostic candidate — `diag_keyzero_guard.c`

Implements the fix, byte-for-byte copy of `ra_core.c`'s
`rot32`/`fmix32`/cycle/`ra_hash`/`ra_reseed` otherwise (only the keyterm
computation changes). Three CLI modes: `--edgecheck`, `--stream` (unbounded
reseeding orbit, for single-key sanity), `--multikey255`/`--multikey1`
(Weyl-stepped multikey chaining, 255-word / 1-word blocks per key — same
shape as the original BCFN discovery / `multikey_stream.c`).

### 2. Edge-case check — PASS

`./diag_keyzero_guard --edgecheck`: tested key=0, 0xFFFFFFFF, 1..10, and
the two keys where a single keyterm becomes 0 under the new guard
(`0x777d91ac` for L, `0x9c0387d6` for M — see file header comment for the
derivation and why these don't reproduce the all-zero attractor: unlike
key=0, `cons=key != 0` there, so `a=(d^o)^(cons+a)` still grows from a
nonzero term and the cycle never collapses). **All 14 keys: non-degenerate
in both orbit (2000 words, 3 reseed cycles) and singleblock (255 words)
modes.** `key=0`'s new keyterms: `keyterm_l=0x484fd5b3`,
`keyterm_m=0xd9028ada` (both nonzero, as required).

### 3. BCFN non-regression — K=255 multikey pattern: PASS, 32GB clean

`./diag_keyzero_guard --multikey255 424242 <N> | RNG_test stdin32 -tlmin
1GB -tlmax 32GB` (`practrand_multikey255_guardfix_32g.log`):

| Checkpoint | Result |
|---|---|
| 1GB | no anomalies (194 tests) |
| 2GB | no anomalies (205 tests) |
| 4GB | no anomalies (217 tests) |
| 8GB | no anomalies (230 tests) |
| 16GB | 1 "unusual": `[Low1/32]FPF-14+6/16:all` R=+4.7, p=6.5e-4 |
| 32GB | **no anomalies (251 tests)** — the 16GB "unusual" did not recur or escalate |

The single 16GB "unusual" (lowest PractRand severity tier, well below
"suspicious"/"FAIL") is consistent with a known artifact already
characterized in this repo's own prior research (`FPF-14+6/16` flagged as
"real/reproducible in shared core loop... not init-specific, no FAIL" —
see `project_parallelization_research_1tb` memory) — not new, and it
self-resolved at 32GB. **Meets and exceeds the 32GB bar the original
Kandidat-4 fix was verified against.** No BCFN reappearance (the original
defect was `BCFN(2+0,13-0,T)` FAIL at 4GB — nothing resembling that shows
up here at any checkpoint through 32GB).

### 4. Non-regression — `ra_core validate` after applying to `ra_core.c`

See "Applied to ra_core.c" below.

## CRITICAL — separate, pre-existing defect found: K=1 singleblock chaining catastrophically FAILs PractRand

While extending the same verification to the K=1 pattern (single-word
block per key — the most aggressive `ra_core_singleblock` reinit rate,
one of the three usage patterns `production-candidate-battery` needs to
validate), found:

```
./diag_keyzero_guard --multikey1 987654 <N> | RNG_test stdin32 -tlmin 1GB -tlmax 8GB
length= 1 gigabyte (2^30 bytes), time= 30.7 seconds
  BCFN(2+0,13-1,T)     R= +1069   p= 2.5e-575   FAIL !!!!!!!
  BCFN(2+1,13-1,T)     R= +262.9  p= 5.0e-141   FAIL !!!!!
  DC6-9x1Bytes-1       R= +352.3  p= 2.2e-182   FAIL !!!!!!
  FPF-14+6/16:all      R= +243.0  p= 1.4e-227   FAIL !!!!!!
  BRank(12):1K(2)      R= +5666   p~= 1e-1706   FAIL !!!!!!!!
  BRank(12):2K(2)      R=+11511   p~= 4e-3466   FAIL !!!!!!!!
  BRank(12):3K(1)      R=+12273   p~= 1e-3695   FAIL !!!!!!!!
  ...and more (see practrand_multikey1_guardfix_8g.log)
```

**This is dramatically worse than the original BCFN defect** (R=+40.7 at
4GB for the affine formula) — here BCFN hits R=+1069 at just **1GB**, plus
catastrophic `BRank` (binary-rank, detects linear/low-rank structure)
failures with R in the thousands.

**Proven NOT caused by this session's guard-XOR fix**: a control run
using the plain, unguarded Kandidat-4 formula (`fmix32(CONST*key)`, no
`GUARD_L`/`GUARD_M`) against the identical `--multikey1` pattern fails
just as catastrophically, even earlier (512MB):
`control_noguard_multikey1_1g.log` — `BCFN(2+0,13-1,T)` R=+531.6 at
512MB, `BRank(12):1K(2)` R=+5666 (same magnitude as the guarded run).
**This defect pre-dates both fixes** — it was simply never tested before
(per `production-candidate-battery/RESULTS.md`: "PractRand khusus pola
K=1... belum pernah diuji sama sekali").

**Root-cause hypothesis (not yet fully verified, flagging for whoever
picks this up)**: at K=1, `ra_permutation_cycle_singleblock`'s loop
executes **exactly one iteration** — `count` starts at 1, so
`if (*count <= 1) break;` fires right after the first `fwrite`. That one
iteration is always `i=255` (the loop's starting value), so:
- `o = (M[(uint8_t)(255+6)]<<6) ^ (M[(uint8_t)(255+7)]<<7)` is **always**
  `(M[5]<<6) ^ (M[6]<<7)` — only 2 of the 256 `M[]` elements are ever
  read, at fixed indices, for every single key.
- `a=cons=key`, `b=(uint32_t)it=0`, `c=d=0` at the start of every call (a
  fresh call per key, `it` is always 0).
- So the single output word is `rot32(((o^2*key)>>13)^(o^2*key), 2*key^o)`
  where `o` is itself a low-degree function of `key` (via `M[5]`/`M[6]`'s
  own init formula) — the 255-round permutation cycle that gives the
  design its mixing power **never gets to iterate** at K=1; it degenerates
  to a single-round hash of `(key, M[5], M[6])`. This is consistent with
  the catastrophic `BRank` failures (a near-linear relationship between
  consecutive Weyl-stepped keys' outputs is exactly what a low-rank binary
  matrix test would catch) and is a **cycle/output-formula** issue, not an
  **init-formula** issue — no init fix (guard-XOR or otherwise) can close
  this, since the problem is that mixing barely happens at all when
  `count` starts at 1.

**This is out of scope for this fix** (init-only, as directed) and is
flagged here rather than guessed at further. **`ra_core_singleblock` at
K=1 should be treated as UNVALIDATED / likely broken** until a
separate diagnostic session investigates — this affects one of the three
explicit "pola pakai" (`singleblock` K=1, `singleblock` K=255, `orbit`)
this whole productionization effort is meant to validate, so it is a
real blocker, not a minor caveat.

## Applied to `ra_core.c`

Applied 2026-09-01 (this session). Changed `ra_init_state_orbit` (keyterm_l
and keyterm_m lines) and `ra_init_state_singleblock` (keyterm_m line) in
`../2026-9-1_family-productionization/ra_core.c` to the guard-XOR form,
added `#define GUARD_L 0x38916df4u` / `#define GUARD_M 0x6c26fc92u` next to
`fmix32`, and updated the header provenance comment to point at this
folder instead of leaving the pre-guard Kandidat-4 description
unqualified. Rebuilt `ra_core_nomain.o`, `ra_core`, `ra_core_singleblock_cli`,
`bench_ra_core` in `family-productionization/` — compiled clean, `./ra_core
validate`: **9945/9945 combinations, 0 mismatches** (structural
orbit/singleblock consistency unaffected by this change, since both use
the same `GUARD_M`-guarded M formula). `./ra_core --stream orbit 0 16` /
`--stream singleblock 0 16` spot-checked non-zero (matches
`diag_keyzero_guard --edgecheck`'s key=0 result exactly, confirming the
applied `ra_core.c` code matches the verified diagnostic candidate).

No `_affine`/duplicate side-by-side kept — direct replacement, consistent
with this repo's established pattern for applying a verified fix (see
`../2026-9-1_multikey-remix-search/HANDOVER.md`'s "Applied to ra_core.c"
section for the precedent this follows).
