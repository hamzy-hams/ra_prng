# RESULTS: production-candidate battery for `ra_core.c`

## Status (updated 2026-09-01): key=0 defect FIXED and Step 0-3 RECONFIRMED
## clean, but a SEPARATE, more severe defect (K=1 pattern) now blocks
## further progress — gate STILL NOT PASSED

Original key=0 finding (below, unedited) triggered
`../2026-9-1_keyzero-guard-fix/` — full root cause, guard-XOR fix
(user-specified formula), and verification are there
(`../2026-9-1_keyzero-guard-fix/RESULTS.md`). Summary: fix applied to
`ra_core.c` 2026-09-01, `./ra_core validate` still 9945/9945, K=255
multikey PractRand clean to 32GB (no BCFN regression). **This folder's
Step 0 and Step 3 were rebuilt and rerun against the fixed `ra_core.c`**:

- Step 0 rerun (`./BUILD.sh` + `verify_fresh_build.py`): PASS. `key=0`
  now included correctly in the multikey cross-check (`key=0 n=1: MATCH`).
- Step 3 rerun (`cross_correlation_ra_core_singleblock.py full`,
  `collision_scan_ra_core_singleblock.py full` both seed modes): **PASS,
  and the `RuntimeWarning: invalid value encountered in divide` that
  originally surfaced the key=0 defect is GONE** — confirms the fix closes
  the specific issue this axis flagged. 0/130,816 pairs flagged (adjacent
  & control), 0/50,000 collisions (sequential & random).

**However, while verifying the key=0 fix didn't regress the BCFN fix, a
NEW and much more severe defect was found in the K=1 usage pattern** —
proven to be pre-existing (NOT caused by the key=0 fix, NOT caused by
anything in this battery folder) via a no-guard control test. Full
detail in `../2026-9-1_keyzero-guard-fix/RESULTS.md`'s "CRITICAL --
separate, pre-existing defect" section; summary:

`ra_core_singleblock` chained at K=1 (one word per key, the most
aggressive reinit usage pattern) **catastrophically fails PractRand at
just 1GB** — `BCFN(2+0,13-1,T)` R=+1069 (vs. the original defect's
R=+40.7 at 4GB), plus `BRank(12):1K/2K/3K` failing with R in the
thousands (near-linear structure across keys). Root-cause hypothesis:
at K=1 the 255-round permutation cycle only ever executes **one**
iteration before `count<=1` breaks the loop, always at the fixed index
`i=255` — so the cycle's mixing never actually happens; the output
degenerates to a low-degree function of `(key, M[5], M[6])` only. This
is a **cycle/output-formula** structural issue, not an init-formula
issue, so it is **not fixable by the key=0 guard fix or any other init
change** — it needs its own diagnostic session (redesigning how the
cycle is exercised at very small `rng`, or restricting `ra_core_singleblock`
at small K analogous to its existing `rng<=255` guard).

**This blocks Steps 4-8 as originally planned** (all of which include a
K=1 axis: collision-scan K=1, dieharder K=1, PractRand K=1, shuffle K=1).
Per the fail-fast principle this folder's plan already established, and
per explicit instruction to stop and surface unexplained new FAILs rather
than guess forward: **execution stopped here again**, without attempting
the K=255-only subset of Steps 5-7 unilaterally — that's a scope/priority
call for the user (see "Open question" at the bottom), not something to
decide solo, especially right after one weak-key surprise already
invalidated an assumption ("other axes would have caught it") this
session made once already.

**Do not promote `ra_core.c` to `src/`.** The key=0 defect is fixed and
reconfirmed; the K=1 structural defect is new, unfixed, and arguably more
serious (worse magnitude, affects a full explicit usage pattern rather
than one specific input value).

## Original Step 0-3 run (2026-09-01, before the key=0 fix) — kept for
## history, superseded by the rerun above for Steps 0 and 3 specifically

## Provenance (Step 0)

All binaries in this folder were rebuilt from scratch from
`../2026-9-1_family-productionization/ra_core.c` via `./BUILD.sh`
(command: `gcc -O3 -march=native -std=gnu17 -include stdalign.h ra_core.c
-o ra_core -Wall -Wextra`, and equivalent for the other binaries, all
linking against a freshly compiled `ra_core_nomain.o`). `ra_core.c` itself
was NOT modified or copied — referenced by relative path only.

`verify_fresh_build.py`: **PASS**
- `./ra_core validate`: 9945/9945 combinations, 0 mismatches.
- `multikey_stream` (K=255 chaining) vs manual per-block `ra_core --stream
  singleblock` cross-check: byte-identical for 5 (key,n) pairs incl. n>255.

**Important side-finding**: byte-comparing this folder's fresh binaries
against `../2026-9-1_family-productionization/`'s existing ones —
`ra_core`, `ra_core_singleblock_cli`, `bench_ra_core` are byte-identical
(safe to treat as already post-fix), but **`multikey_stream` differs**
(`cmp` reports a difference at byte 265). Consistent with the mtime
concern that motivated Step 0's from-scratch rebuild (that binary was
older than `ra_core.c`/`ra_core_nomain.o`, so it most likely was still
linked against the pre-fix object file). This folder's fresh
`multikey_stream` is the one trusted for all results below; the old one in
`family-productionization/` should not be reused for anything without its
own rebuild.

## Step 1 — Avalanche: PASS (all 3 usage patterns)

Reused `quality_gate.py`'s `avalanche_stats()`/band [0.3,0.7]/floor 0.2.
`orbit` and `singleblock` K=255 rerun via `avalanche_orbit_singleblock_k255.py`
(adapted from `avalanche_ra_core.py` — only change: `RA_CORE_BIN` repointed
to this folder's own fresh binary, since the original hardcodes
`Path(__file__).parent`, which would have silently tested the OLD binary
if the original script were invoked unmodified from here).

| Mode | overall_mean_hamming_fraction | min_bit_fraction | PASS |
|---|---|---|---|
| orbit (K=255) | 0.500605 | 0.489216 | YES |
| singleblock (K=255) | 0.500605 | 0.489216 | YES |
| singleblock K=1 (new, `avalanche_ra_core_singleblock_k1.py`) | 0.502930 | 0.281250 | YES |

K=1's `min_bit_fraction` (0.281) is noticeably closer to the 0.2 floor than
K=255's (0.489) — expected, since each bit-flip trial only has 1 sample
word instead of 255, so it's a noisier estimate. Still clears the bar.

**Important caveat discovered later (see "Critical defect" below): this
avalanche methodology structurally cannot detect the key=0 degenerate-state
defect**, because it never uses key=0 as either the base seed or lands on
it except as one specific bit-flip of base_seed=1 (bit 0: 1^1=0) — and
comparing a normal pseudorandom word against the resulting all-zero output
coincidentally averages ~50% Hamming distance (differs from 0 by
`popcount(word)`, which is ~16/32 on average for a healthy word), which
looks like a *normal* avalanche result instead of flagging an anomaly. This
is a real blind spot of hamming-distance-based avalanche testing for this
specific failure mode (total single-key state collapse), not a bug in the
test.

## Step 2 — Speed benchmark: recorded, no regression flagged

`bench_ra_core throughput`/`reinit-sweep`, both cores, output in
`bench_ra_core_results.txt`.

| K | orbit ns/word | singleblock ns/word | checksum match? |
|---|---|---|---|
| 1 | 55.368 | 38.780 | YES (3986491784 both) |
| 10 | 7.477 | 4.271 | YES (3335603665 both) |
| 100 | 2.656 | 1.230 | YES (3358492716 both) |
| 255 | 2.171 | 0.488 | YES (1651810055 both) |

Checksum-match at every K is a passive correctness cross-check (orbit and
singleblock must agree for rng<=255) — confirms no divergence introduced
by this rebuild. Not a hard gate (plan: "referensi, bukan gate keras"), but
worth noting: singleblock K=255 here (0.488 ns/word) is ~3.6x slower than
`RESULTS_TAHAP2.md`'s historical figure (0.135 ns/word) for the same
formula shape — same order of magnitude, not flagged as a regression per
the plan's criterion, but plausibly machine-load noise (this exact
reinit-sweep microbenchmark was already documented as noisy across runs in
`project_addressable_init_research` memory — a single control run varied
71→90.5→111.7 ns/word across sessions previously). Not independently
re-measured with a median-of-3 orchestrator here; flagging for awareness,
not blocking.

## Step 3 — Cross-correlation & collision-scan, K=255: PASS on their own
## terms, but surfaced the critical defect below

Reused `cross_correlation_ra_core_singleblock.py`/
`collision_scan_ra_core_singleblock.py` (`experiments/2026-8-29_parallelization-research/`)
unmodified — `SINGLEBLOCK_CLI` points at
`../2026-9-1_family-productionization/ra_core_singleblock_cli`, proven
byte-identical to this folder's fresh build (see Step 0 side-finding), so
no repoint needed.

- Cross-correlation (tier full, K=512, n=255): adjacent flagged=0/130816,
  control flagged=0/130816. **BUT**: numpy raised `RuntimeWarning: invalid
  value encountered in divide` during `np.corrcoef` — a silent hint that at
  least one of the 512 streams has zero variance (a degenerate/constant
  stream). Because `NaN > z_crit` is always `False` in Python/numpy, a
  fully degenerate stream would NOT be flagged by this script's z-test —
  it fails silently instead of failing loudly. This warning is what led to
  the investigation below.
- Collision-scan (tier full, M=50,000, sequential AND random seed modes):
  0/50,000 collisions in both modes (`collision_scan_singleblock_results_{sequential,random}_full.json`).
  Also does not flag the defect below (see "why collision-scan doesn't
  catch it" in the next section) — its own criterion (0 collisions,
  ≪ expected ~6.78e-11) is still met and remains a valid PASS on its own
  terms.

## CRITICAL DEFECT: key=0 produces a permanent all-zero stream (both modes)

Investigating the `RuntimeWarning` above (adjacent group includes keys
`0..511`, so key=0 is in scope):

```
$ ./ra_core --stream singleblock 0 16   # all 16 words are 0x00000000
$ ./ra_core --stream orbit 0 16         # same, all zero
$ ./ra_core --stream orbit 0 2000       # still all zero at word 2000
                                         # (3 reseed cycles in) -- PERMANENT,
                                         # not just a first-cycle artifact
```

**Root cause**: `ra_init_state_orbit`/`ra_init_state_singleblock` compute
`keyterm_l = fmix32(0x9e3779b7u * key)` / `keyterm_m = fmix32(0x06a0dd9bu *
key)`, then multiply every `L[i]`/`M[i]` term by this key-term. For
`key=0`: `0x9e3779b7u * 0 = 0` and `0x06a0dd9bu * 0 = 0`, and **`fmix32(0)
== 0`** (verified: MurmurHash3's finalizer maps 0 to 0, since every step —
xor-shift, multiply-by-odd-constant — is a bijection fixing 0). So
`keyterm_l = keyterm_m = 0`, which zeroes out `l_val`/`m_val` for every
`i` regardless of the `i*CONST` term (since it's a **multiply**, not an
add) — `L[]` and `M[]` become entirely `0`. With `M` all zero, every `o`
computed in the permutation cycle is `0`, and with `cons=key=0`, the
recurrence `a=(d^o)^(cons+a)` etc. never leaves the all-zero fixed point:
`a=b=c=d=0` forever, every output word is `0`. For `orbit` mode, `ra_reseed`
on an all-zero `M`/`L` (`M[i]^=L[i]` stays 0, `ra_hash` of an all-zero
array is 0, `new_cons=0`) keeps `cons=0` permanently — this is a **stable
degenerate fixed point**, not a transient defect that self-corrects after
reseed.

Because `0x9e3779b7u` and `0x06a0dd9bu` are both odd (hence invertible mod
2^32) and `fmix32` is a bijection on 32-bit words with a unique zero
pre-image at `0`, **`key=0` is the unique key** that triggers this — no
other key produces `keyterm_l=0`/`keyterm_m=0` simultaneously. (Verified
computationally: `fmix32(0) == 0`, both multipliers are odd.)

**This is a regression introduced by the "Kandidat 4" fix, not a
pre-existing limitation.** The formula it replaced (additive combine,
`l_val = i*CONST + CONST*key`, from `winner_wired_addressable.c`/
`scrambler_addressable.c`'s `ra_init_state_addressable`) does NOT degenerate
at `key=0`: the `i*CONST` term survives independently of the key term, so
`L[]`/`M[]` are still a healthy non-constant sequence even when the
key-dependent addend is 0. Switching the combine operator from `+` to `*`
(the change that fixed the BCFN multikey defect, per
`../2026-9-1_multikey-remix-search/HANDOVER.md`) traded one defect for
another: it closed the "shared shape, shifted by an offset" weakness that
`+`/`^` had, but introduced a new absorbing-zero weak-key at `key=0`
specifically because multiplication has a zero divisor that addition
doesn't.

**Why no other axis caught this**:
- **Avalanche**: see Step 1's caveat — the hamming-distance metric doesn't
  distinguish "flipped bit produced a good pseudorandom word" from
  "flipped bit produced all-zeros", because both compare to the baseline
  with a similar average Hamming distance.
- **Collision-scan**: only detects when TWO DIFFERENT keys produce the
  SAME output — since key=0 is the unique key with this degenerate output,
  there's no second key to collide with it, so 0 collisions is still
  correctly reported (not a false negative in the tool's own terms, just
  outside what it's designed to catch).
- **Cross-correlation**: DID surface it, but only as a silent NaN warning,
  not as a flagged pair — the z-test comparison itself is blind to
  zero-variance rows because `NaN` never satisfies `> z_crit`.
- **Dieharder/PractRand (not yet run)**: a single degenerate key mixed
  into a large multikey stream (1 word in 50,000+ keys × up to 255 words
  each) would likely be statistically invisible to bulk randomness tests —
  this defect is a **weak-key** issue (specific to how a caller might use
  key=0), not a bulk statistical quality issue, so those tools would
  likely also miss it, which is exactly why it's dangerous to rely on the
  battery's remaining axes to catch it.

## What's left (NOT run — this section describes the ORIGINAL key=0 block;
## superseded by the K=1 block described above, which is what's actually
## stopping progress now)

Step 4 (collision-scan K=1), Step 5 (dieharder K=255/K=1), Step 6
(PractRand K=255/K=1), Step 7 (shuffle-implementation K=1/K=255), Step 8
(`ADDENDUM_POST_FIX_STATUS.md`) — **still not started**, now blocked on
the K=1 structural defect rather than key=0 (which is fixed).

## Recommendation (key=0 — RESOLVED, kept for history)

~~This needs a decision before the battery continues~~ — resolved:
option 1 below was chosen and executed (`../2026-9-1_keyzero-guard-fix/`).
1. ~~Fix the init formula~~ — **DONE**, guard-XOR fix applied to
   `ra_core.c`, verified (edgecheck, 32GB PractRand K=255, `validate`
   9945/9945), Step 0+3 rerun clean.
2. Document key=0 as forbidden/reserved input — not chosen.
3. Do nothing — not chosen (correctly, per the analysis below).

## Recommendation (K=1 structural defect — NEW, open, needs a decision)

Same three-shaped choice as before, but for a different and more serious
problem — this is NOT an init-formula issue (see root cause above), so
"fix the init formula" isn't even an available option this time:

1. **Redesign how `ra_core_singleblock`'s cycle behaves at very small
   `rng`** (e.g. K=1-10) so at least a few rounds of mixing happen before
   output, instead of the single fixed-`i=255` iteration it gets today —
   the most robust option, but a real research task (needs a candidate
   search + PractRand verification, likely touching
   `ra_permutation_cycle_singleblock`'s loop structure, not just its init)
   — would belong in a new diagnostic folder, analogous to
   `../2026-9-1_multikey-remix-search/`.
2. **Restrict `ra_core_singleblock`'s documented valid range** away from
   very small `rng` (e.g. require `rng >= N` for some threshold found by
   testing where PractRand actually clears), analogous to its existing
   `rng<=255` guard — cheaper, but narrows the "aggressive call-and-discard,
   few words per address" use case this mode exists for in the first
   place (per `project_ra_prng_family_productionization` memory: K=1 is
   literally the extreme point of that intended use case, not an edge
   case nobody would hit).
3. **Do nothing / ship undocumented** — not recommended, for the same
   reason as before: K=1 (init once, draw one word, reinit) is a
   completely foreseeable calling pattern for this function, not an
   obscure misuse.

**Also open**: does this same structural issue affect `singleblock` at
other very-small-but->1 `rng` values (K=2, K=5, K=10...), or is K=1
uniquely bad because it's the ONLY value where the loop executes exactly
one iteration? Not tested this session — worth checking before picking
option 1 or 2 above, since it changes where the safe/unsafe threshold is.

Not decided in this session — flagged as an open question for the user,
same as the key=0 question was (and note how that one turned out: the
"other axes will catch it" assumption failed there too — worth erring
toward actually testing K=2/5/10 rather than assuming continuity from K=1
to K=255).
