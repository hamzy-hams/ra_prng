# HANDOVER: Init-loop speed optimization untuk `ra_core_orbit` / `ra_core_singleblock`

Status: **spec only, NOT executed.** Written 2026-09-03, to be picked up in a
future session. Do not assume any candidate has been built or benchmarked --
this file is the plan, not a report.

## Why this experiment exists

Diagnostic done in the session that wrote this file (read-only: source
inspection + `gcc -O3 -march=native -S` assembly comparison, no repo changes)
found that `ra_core_orbit`/`ra_core_singleblock`
(`experiments/2026-9-1_family-productionization/ra_core.c`) are slower than
`paperorig` (`ra_core_p1`,
`experiments/2026-8-31_prng-family-benchmark/benchmark_all.c` L65-117) --
**not** because of the hot/generate loop (which is actually *lighter* than
paperorig's: 2 reads of `M[]` per round vs paperorig's 8-tap `o` plus a
64-bit multiply, `benchmark_all.c` L77-78/86), but because of the **256-
element init loop** that fills `L[256]`/`M[256]`, which runs in full on
every call regardless of how many words are requested.

Compared via assembly (`gcc -O3 -march=native -S`):

| | Init formula | Vector instructions / 8-lane |
|---|---|---|
| **paperorig** (`ra_init_state_p1`, `benchmark_all.c` L65-70) | `M[i]=i*C+C`, `L[i]=i*C+C` -- linear, **seed-independent**, key never enters | ~6 (`vpmulld`x2, `vpaddd`x2, store x2) |
| **singleblock** (`ra_init_state_singleblock`, `ra_core.c` L205-212) | fills M[] only, per-index `rot32(val, r)` with **variable** shift `r=key^i` | ~18 (needs `vpsrlvd`/`vpsllvd` -- variable-shift vector ops, much costlier than constant-shift) |
| **orbit** (`ra_init_state_orbit`, `ra_core.c` L139-149) | same as singleblock but for **L[] and M[]** (double the work) | ~2x singleblock's |

Measured (K=1, apples-to-apples microbench): paperorig 24.9 ns/word <
singleblock 33.4-39.1 ns/word (two sessions) < orbit 56.8 ns/word (older
session, not directly comparable machine but consistent ordering).

This is **not a bug**. The key-dependent init is deliberate -- it's what
gives orbit/singleblock the addressable/anti-weak-key property (`GUARD_L`/
`GUARD_M` guard constants, `ra_core.c` comment block L132-138, fixed
2026-09-01 after a real key=0 degenerate-output defect found in
`../2026-9-1_keyzero-guard-fix/RESULTS.md`). paperorig has no such property
at all -- key only enters later as `cons`, never touches init. The open
question this experiment exists to answer: **can the same addressable/
anti-weak-key property be achieved for less than ~15-18 vector instructions
per 8-lane?**

Relevant code, verbatim as of this writing:

```c
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

// orbit (ra_core.c L139-149)
static void ra_init_state_orbit(uint32_t *L, uint32_t *M, uint32_t key) {
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

// singleblock (ra_core.c L205-212) -- same shape, M[] only
static void ra_init_state_singleblock(uint32_t *M, uint32_t key) {
    uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu) * keyterm_m;
        M[i] = rot32(m_val, r);
    }
}
```

## Scope

Both `ra_core_orbit` and `ra_core_singleblock`. They share the same init
shape (singleblock fills M[] only, orbit fills L[]+M[] with the same
per-element formula), so a fix found for one very likely transfers directly
to the other -- treat as one experiment, not two.

## Goal -- three-tier strategy, in priority order

This order and the exit criteria between tiers were set explicitly by the
user (2026-09-03) -- do not reorder or skip a tier "because it looks
unlikely to work."

### Priority 1 (try first): find redundant/reducible operations with ZERO output change

**Constraint (user's explicit choice, 2026-09-03): portable C only.** No
manual SIMD intrinsics, no inline asm, no arch-specific restructuring. Any
change must be plain C that a standard `-O3 -march=native` compile picks up
-- this is different from the `winner_wired_addressable.c` precedent
(x86-only opt-in variant), which is explicitly *out of scope* here.

**Concrete starting hypothesis** (found in the session that wrote this
file -- a lead to try and measure, not a finished solution): the current
code computes, per iteration, `l_val = (uint32_t)(i * 0x9e3779b7u) *
keyterm_l` then `rot32(l_val, r)`. In `uint32_t` arithmetic (i.e. the ring
Z/2^32Z), this is `(i*A mod 2^32) * B mod 2^32` where `A=0x9e3779b7u`,
`B=keyterm_l` -- two multiplies per iteration before the rotate. Modular
multiplication is associative/commutative, so:

```
(i*A mod 2^32) * B mod 2^32  ==  i*(A*B mod 2^32) mod 2^32
```

This means `step_l = (A * B) mod 2^32` can be computed **once**, outside
the loop, and `l_val` accumulated via `l_val += step_l` each iteration
(starting at 0) instead of a fresh multiply-by-`i` and multiply-by-`keyterm`
every time. **This is bit-identical by construction** (a modular-arithmetic
identity, not an approximation) -- replaces 2 multiplies/iteration with 1
add/iteration for the pre-rotate value. Same substitution applies to
`m_val`/`keyterm_m`.

Caveat to verify, not assume: this only removes the multiply-chain, **not**
`rot32(val, r)` itself -- which is the actual expensive part (the
variable-shift `vpsrlvd`/`vpsllvd` per the assembly diagnostic above). It
may buy little if the rotate dominates. Benchmark it; do not assume it's
sufficient.

**Other directions to explore** (unverified, not promised -- for whoever
picks this up):
- Does restructuring how `r = key ^ i` is computed, or reordering/
  regrouping the 256 iterations by shift-amount, help the compiler avoid
  the full variable-shift path anywhere? Computation *order* is free to
  change as long as each `L[i]`/`M[i]` ends up with the same value it has
  today -- output-order in the array is what must stay fixed, not the
  loop's internal iteration order.
- `ra_hash`/`ra_reseed` (`ra_core.c` L108-125, called by orbit once per
  255-word cycle, not on every call) -- lower priority since it's already
  amortized over 255 words, but worth a quick look if Priority 1 is being
  done thoroughly.

**Verification bar (mandatory, before any speed measurement):** KAT +
full-stream byte-identical diff against the current `ra_core.c` output,
across multiple keys (not just one) -- reuse/extend the pattern from
`experiments/2026-9-3_orbit-continuous-bench/verify_orbit_kat.py` (extern-
link the candidate the same way, or verbatim-copy per the non-goals below).
Any output difference, even one bit, disqualifies the candidate from
Priority 1 -- it becomes a Priority 2 candidate instead, not a "redundant-op
removal."

**Exit criterion to Priority 2:** if no candidate gets a significant
speedup. "Significant" needs to be pinned down with the user at that point
-- a rough starting suggestion (closing at least half the gap to paperorig,
i.e. singleblock noticeably under ~29 ns/word, orbit under ~41 ns/word) is
offered here only as a talking point, not a mandated threshold.

### Priority 2 (if P1 fails to help significantly): search for a new init formula (output allowed to change)

Must preserve, at minimum:
- **High avalanche**, measured as **per-bit minimum** fraction, not the
  scalar average -- this repo has been burned by the average-vs-min-bit gap
  before (a near-dead-bit candidate passed the average gate in
  `experiments/2026-8-27_operand-position-search/`, only caught by the
  min-bit version). Reuse `quality_gate.py`'s `avalanche_gate_min_bit`
  pattern from that folder.
- **Low inter-key/inter-seed correlation** -- reuse/extend
  `experiments/2026-8-29_parallelization-research/collision_scan.py`.
- **Good randomness** via PractRand/dieharder, per the rigor below.
- **Anti-weak-key**, as a mandatory *additional* gate stage (this property
  is specific to the addressable init and is NOT covered by the generic
  cycle-combo-search pipeline being reused for rigor below) -- reuse the
  edgecheck methodology from `../2026-9-1_keyzero-guard-fix/RESULTS.md`
  (14 keys including key=0 and derived single-keyterm-zero cases, all must
  be non-degenerate).

**Rigor: identical to `../2026-9-2_singleblock-cycle-combo-search/`**
(user's explicit choice, 2026-09-03 -- not a lighter version):
- Tier 0 -- avalanche filter (cheap, min-per-bit gate).
- Tier 1 -- multikey PractRand **staged search-loop** gate (not a long
  single-stream gate -- this repo has hit the "gate size trap" repeatedly:
  1MB/8MB in `2026-8-26_operation-pruning-research`, 2MB/16MB in
  `2026-8-27_operand-position-search`, 16MB-clean-but-16GB-FAIL in
  `2026-9-2_singleblock-o-width-fix`). Gate at K=1 specifically.
- Promotion tier -- survivors tested across K in {1,2,4,8,16,32,64,96} at
  **16GB** via the multikey harness, on the VPS (check `VPS_ACCESS.md`,
  gitignored, for any process still running before starting new ones, per
  `CLAUDE.md`).

**DSL/search-space for candidate init formulas is NOT yet designed in this
document** -- that is an explicit open task for whoever starts Priority 2.
Build it on top of whatever Priority 1 established (if P1 partially reduced
cost, P2's baseline should be P1's best candidate, not the original
formula from scratch). Do not restart the multiply-vs-rotate tradeoff
analysis from zero.

### Priority 3 (if both P1 and P2 fail): re-evaluate the tradeoff

Not an autonomous decision point -- an explicit checkpoint to stop and
bring findings back to the user. Options on the table (user's own framing):
- (a) Accept current speed as-is, with the existing safe-K floor (K>=96 for
  singleblock, per memory `project_singleblock_kmin_defect`) as the
  tradeoff already accepted.
- (b) Reopen discussion -- including the possibility of connecting to the
  state-size Pareto frontier idea already parked (memory
  `project_state_size_pareto_frontier`).

Whoever reaches this tier should present P1+P2 findings and stop, not pick
a fallback unilaterally.

## Non-goals / hard constraints

- Do **not** modify `ra_core.c` directly during search. Build every
  candidate as a self-contained diagnostic file (verbatim-copy of
  `rot32`/`fmix32`/`GUARD_L`/`GUARD_M`/init functions, same pattern as
  `../2026-9-2_singleblock-o-width-fix/diag_wideo_singleblock.c` -- do not
  extern-link if the originals stay `static`; extern-link is fine only for
  read-only verification against the real binary, as
  `../2026-9-3_orbit-continuous-bench/orbit_bench.c` does).
- **Applying any winning candidate to `ra_core.c` requires a separate,
  explicit user confirmation -- always, even for a Priority-1 candidate
  proven bit-identical by KAT + full-stream diff.** This is a deliberate
  choice (user, 2026-09-03) to keep the same gate every other experiment in
  this repo goes through, not a relaxed fast-path for "provably safe"
  changes.
- Priority 1 changes: portable C only, no intrinsics/inline-asm/arch-
  specific restructuring (user's explicit choice, 2026-09-03). An
  arch-specific opt-in variant (mirroring `winner_wired_addressable.c`) is
  explicitly out of scope for this experiment -- a separate future ask if
  ever wanted.
- **No re-seed retries on FAIL** (standing rule, memory
  `feedback_no_reseed_on_fail.md`): a FAIL on one seed/key at any tier is
  decisive -- reject and move on, never re-run with a different seed to
  explain it away.
- Do not touch `experiments/2026-9-1_production-candidate-battery/` (stays
  paused).
- Never write VPS credentials into any file tracked by git.

## Deliverables

- `RESULTS.md` in this folder: candidate table, overhead numbers (ns/word,
  reinit-sweep microbench pattern from `bench_ra_core.c`), and an explicit
  statement of which tier the search actually exited at and why. "No
  candidate beats the constraint" is a valid, complete outcome at any tier
  -- write it up with the same rigor as a positive result.
- Run `/graphify --update` before closing out the task, per `CLAUDE.md`.
- Update memory with the outcome (new memory and/or extend
  `project_singleblock_kmin_defect`/create a sibling init-speed memory).

## Critical files to reference

- `experiments/2026-9-1_family-productionization/ra_core.c` L86-98
  (`rot32`/`fmix32`/`GUARD_L`/`GUARD_M`), L100-125 (`ra_hash`/`ra_reseed`),
  L132-256 (both init functions + both permutation-cycle functions +
  `ra_core_orbit`/`ra_core_singleblock`) -- the unchanged baseline.
- `experiments/2026-8-31_prng-family-benchmark/benchmark_all.c` L57-117
  (`ra_core_p1`/`ra_init_state_p1`/`ra_permutation_cycle_p1`) -- paperorig
  comparison baseline.
- `experiments/2026-8-27_operand-position-search/quality_gate.py` --
  `avalanche_gate_min_bit` pattern (min-per-bit, not average).
- `experiments/2026-8-29_parallelization-research/collision_scan.py` --
  inter-key/seed correlation scan pattern.
- `experiments/2026-9-2_singleblock-cycle-combo-search/HANDOVER.md` +
  `RESULTS.md` -- the Tier0/Tier1/Promotion rigor this experiment's
  Priority 2 must replicate exactly.
- `experiments/2026-9-1_keyzero-guard-fix/RESULTS.md` -- anti-weak-key
  edgecheck methodology (14-key check including key=0).
- `experiments/2026-9-3_orbit-continuous-bench/orbit_bench.c` +
  `verify_orbit_kat.py` -- extern-link pattern and KAT/byte-identical
  verification harness pattern to reuse for Priority 1 candidates.

## Verification

- Priority 1: bit-identical check (KAT + full-stream diff, multiple keys)
  is mandatory *before* any speed measurement for every candidate.
- `git diff --stat -- experiments/2026-9-1_family-productionization/ra_core.c`
  must stay empty throughout the entire experiment (all tiers).
- RESULTS.md must state explicitly that no FAIL (Priority 2 PractRand
  stage) was re-run with a different seed/key to explain it away.
