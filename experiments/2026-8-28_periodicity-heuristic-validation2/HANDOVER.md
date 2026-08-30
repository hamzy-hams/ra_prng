# HANDOVER: periodicity-heuristic-validation2

## What this is

Child of **two** prior experiments:

- `../2026-8-25_periodicity-heuristic-validation/` -- validates the paper's heuristic
  periodicity estimate `λ ≈ 0.7824·√|S|` (`log2|S| = w·(n+2) + log2(n!)`) against
  measured (Brent's algorithm) cycle length on a toy-scale, (n,w)-parameterized
  reimplementation (`toy_prng.py`) of ra_prng2's **full, unpruned** update loop.
  Its own finding: the heuristic does **not** fit well (ratios from ~0.008x to ~5.3x
  the prediction, non-monotonic).
- `../2026-8-26_operation-pruning-research/` + `../2026-8-27_operand-position-search/`
  -- greedy ablation pruned 14 of 18 tracked mixing operations down to
  `{TAP6, TAP7, ROT_C, SHR13}`, then 2 of 4 operand-wiring slots were rewired
  (`a_xor_operand: b→d`, `c_shift_operand: b→a`), producing **`winner_wired_v2.c`**,
  the current winner (supersedes `winner_wired.c`).

**Research question**: with the state shape (and therefore the `λ_pred` formula)
unchanged, does the heuristic's already-shaky fit change much after this real
structural pruning + rewiring -- i.e. does `toy_prng.py`'s old λ-vs-λ_pred ratio
table still describe the pruned/wired algorithm, or does it move?

## Non-goals

Same as both parent experiments: don't touch `src/ra_prng2/*`, don't touch any file
under `2026-8-25/26/27`'s directories (read-only references, imported not edited),
not a cryptographic evaluation.

## Design decisions (read before touching `pruned_wired_toy_prng.py`)

### 1. Op/wiring mapping ported to toy scale

Kept: `TAP6,TAP7` (generalized: top-2 tap survivors of `G`), `ROT_C`, `SHR13`.
Off: `TAP0-5`, `ROT_A`, `ROT_B`, `ROT_O`, `SHL9`, `SHR18`, `SHL14`, `MULT_REDUCE`,
`HASH_SELFIDX`. `HASH_ACCESS=sequential` (not the original's strided default).
Wiring: `a_xor_operand=d`, `c_shift_operand=a`, `rotc_amount_source=b` (default),
`rotc_xor_operand=a` (default).

Resulting per-step chain (`pruned_wired_toy_prng.next_state`):
```
o = M[(i+top1)&(n-1)]<<top1 ^ M[(i+top2)&(n-1)]<<top2
a = (d ^ o) ^ (cons + a)          # a_xor=d; ROT_A off
b = (cons + a) ^ (o + d)          # ROT_B off
c = rotw((a>>S13) ^ a, b, w)      # c_shift=a & rotc_xor=a (both post-update a,
                                   # not a bug -- see wiring.py); SHL14 gone entirely
d = c & (n-1)                     # MULT_REDUCE off
```
Reseed: `M[i]^=L[i]` for all i, then `out[i] = XOR of M[i*rows..i*rows+rows-1]`
(pure function, M **not** mutated by the hash step, no self-idx term).

### 2. Tap-survivor generalization: top-2-by-count, and why

`toy_prng.py` generalizes the real 8 taps (`TAP0..TAP7`) to `G = n/rows` taps.
`tap_survivors(G, w)` keeps `e = G-2, G-1` (or the lone tap at `G=1`). Verified: at
`G=8` this reduces exactly to `(6,7)` -- the real `TAP6,TAP7` -- the strongest
evidence for this choice. Rejected alternative (proportional position
`round(6G/8), round(7G/8)`): **provably collides** at `G=4` (both round to index 3)
and at `G=2` (both round to index 1), silently losing one of the two intended
survivors via XOR self-cancellation -- same trap class as the sibling experiment's
documented `rows=1`/`rows=2` collapses.

### 3. Tap shift-width collapse (caught during design, not present in either parent)

Tap term `e` contributes `(M[idx]<<e) & mask` -- a **plain left shift**, not a
rotate. For any `e >= w`, every bit is shifted out of the low `w` bits before
masking, so the term is **identically zero**, not just weakened. `next_state`
raises rather than silently returning a collapsed result if this is violated.

This **rules out** the tempting "exact G=8 match" config `(n=8, w=4, rows=1)`:
`taps=(6,7)`, both `>= w=4` → zero contribution, o≡0 always. A genuine G=8 config
needs `w>=8`, which pushes `log2|S|` into territory the *original* experiment
already flagged infeasible (`n=8,w=8` → 95.3 bits, in its own `INFEASIBLE` list).
**Conclusion: `G=4` is the ceiling for non-vacuous tap-pruning at any measurable
scale.** Two new configs reach it: `(n=4,w=4,rows=1)` and `(n=8,w=4,rows=2)`, both
`taps={2,3}`. Note also: at `w=4`, `e=3` retains only 1 of `M[idx]`'s 4 bits
(shifted to position 3, rest pushed out) -- a real but nonfatal degradation, not a
collapse (confirmed nonzero via avalanche check, see STATUS.md).

The 5 configs matching the original experiment's defaults (`(2,8)`, `(4,8)`,
`(2,4)`, `(4,4)`, `(8,4)`, all default `rows=min(4,n)`) all have `G<=2`, so tap
pruning is **vacuous** there (G=1: forced single tap, no pruning possible; G=2:
both taps already survive, no-op). Kept anyway for direct apples-to-apples
comparability against the original's published ratios -- the other 8 pruned/
rewired axes (rotates, shift-term drops, MULT_REDUCE, HASH_SELFIDX, HASH_ACCESS,
wiring) remain fully active and meaningful at every one of these 5 configs.

### 4. `ra_hash_gen` divergence from ground truth (found while reading source, not assumed)

`src/ra_prng2/python/ra_prng2.py`'s real `ra_hash(N)` is explicitly a **pure,
read-only** function (its own comment: "N is read-only, never mutated") --
confirmed also by `winner_wired_v2.c`'s `ra_hash(const uint32_t *N, ...)`. But the
**sibling experiment's `toy_prng.py`'s `ra_hash_gen` mutates its `N` argument in
place** (`N[i] ^= N[j*G+i]`), and that mutated-into-`N` value is discarded --
`out[i]` there only ever receives the `HASH_SELFIDX` term. This is a real, latent
structural divergence from ground truth in the *sibling* experiment's baseline
model -- **not** something this module copies. `pruned_wired_toy_prng.py`'s
`ra_hash_gen_sequential` is a direct, pure-function port of `winner_wired_v2.c`'s
actual `ra_hash`. **Caveat for the final comparison table**: this means the
"before" (`toy_prng.py`) and "after" (this module) baselines don't share identical
hash-step semantics beyond the op/wiring changes under test -- any observed λ
difference has a small possible confound from this, separate from the intended
pruning/rewiring effect. Flag this explicitly in RESULTS.md's verdict section,
don't paper over it.

### 5. Self-check (passed) -- MUST stay passing

At `Params(n=256, w=32, rows=32)`, every generalization function is an *identity*
(`S13=13` unrescaled, `c_m/c_l` untruncated, `G=8`, `taps=(6,7)`) so
`pruned_wired_toy_prng.next_state` at this exact config is provably the same
computation as `wired_prng.py`'s `permutation_cycle`+`reseed` under
`Wiring(a_xor_operand="d", c_shift_operand="a", rotc_amount_source="b",
rotc_xor_operand="a")`. `self_check_full_scale.py` verifies this bit-for-bit for 5
seed/iteration combinations -- **PASSED**, all 5 `OK`. Re-run this any time
`next_state`/`ra_hash_gen_sequential` is touched; do not trust cycle-length numbers
if it fails.

## CRITICAL BUG found and fixed this session -- read before writing more scripts

`pruned_wired_toy_prng.py` (and `self_check_full_scale.py`) originally used
`sys.path.insert(0, sibling_dir)` to import `toy_prng` (resp. `wiring`/
`wired_prng`) from the parent experiment folders. **The sibling folder
`2026-8-25_periodicity-heuristic-validation/` has files with the exact same names
as scripts in THIS folder**: `cycle_measure.py`, `avalanche_check.py`,
`run_sweep.py`, `enumerate_n2w4.py`, `enumerate_n4w4.py`. Inserting the sibling
directory at position 0 pushes it **ahead of the current directory** on
`sys.path` for the rest of the process, so any later bare `import cycle_measure`
(etc.) anywhere silently resolves to the **sibling's old, unpruned** file instead
of this folder's own -- **no error, just a wrong result.**

Caught directly: `cycle_measure.brent(Params(n=4,w=4), seed=0)` returned
`(15232, 1)` under the broken import order -- this was actually the *original,
unpruned* `toy_prng.next_state`'s cycle length, not the pruned/wired one. The true
value, verified independently via brute-force state-visitation (`seen` dict over
300k steps) **and** by the cross-validated C port, is **`(96112, 1)`**.

**Fix applied**: both files now use `sys.path.append(...)` (never
`insert(0, ...)`). Since Python always puts a directly-run script's own directory
first on `sys.path`, this guarantees local same-named modules resolve first; the
sibling directory is only consulted as a fallback for names with no local
collision (`toy_prng` itself, `wiring`, `wired_prng` -- none of which have
same-named files in this folder). **Verified fixed**: the worst-case import order
(`from pruned_wired_toy_prng import Params` before `from cycle_measure import
brent`, i.e. exactly the ordering that broke before) now correctly returns
`(96112, 1)`, and `self_check_full_scale.py` still passes.

**Implication for any future script added to this folder**: if it imports
anything from `pruned_wired_toy_prng` (which triggers the `sys.path.append`) and
ALSO has a same-named counterpart in `2026-8-25/26/27`, double-check it resolves
locally -- e.g. by printing `inspect.getsourcefile(the_imported_name)` once after
writing it, the way this bug was actually caught. `enumerate_n2w4.py` and
`enumerate_n4w4.py` (not yet ported) are exactly this kind of same-named file --
verify them the same way before trusting their output.

## Non-obvious things worth remembering

- `n=4, w=8` (G=1, default rows) config: `cons` collapsed to `0` and stayed there
  across the first several steps in a quick manual smoke test. Not yet investigated
  whether this is a genuine degenerate absorbing state at this toy scale or just a
  short-run coincidence -- worth a specific look when writing RESULTS.md (check
  whether it affects that config's actual measured λ, which the sweep will report).
- Original experiment already has real "before" data reusable for one of the new
  configs' *n,w* pair without re-measuring: `results_n8_w4_rows2.csv` in the
  2026-8-25 folder (300 seeds, λ=131824 constant across every seed) is the
  full-op algorithm at `n=8,w=4,rows=2` -- the SAME `(n,w,rows)` as this
  experiment's new non-vacuous-tap config. Use it directly in the comparison
  table instead of re-deriving it.

## 2026-08-29 correction: `(8,4,rows=4)` watchdog bug + stale headline number

Found during an independent methodology review (asked to validate the graph's
own flagged "surprising connection" between `STATUS.md` and a `cycle_measure`
exit-42 crash). Two separate, real problems, not just presentation issues:

**1. `run_sweep.py` crash, root cause found.** `sweep_stderr.txt` shows
`run_sweep.py` died with an unhandled `subprocess.CalledProcessError` (exit 42)
exactly at the `(8,4,rows=4)` step -- confirmed by the absence of
`results_n8_w4.csv` anywhere in this folder (every other SWEEP row has a
matching CSV; this one doesn't). `cycle_measure.c`'s exit 42 path is only
reachable when the process receives SIGINT/SIGTERM mid-run
(`g_stop_requested`). Root cause: `auto_stop_power38.py`'s `find_pid()` used
`pgrep -f "cycle_measure 8 4 4"` -- a prefix that matches *any* `cycle_measure`
invocation starting with those args, not just the specific long-running
manual one it was meant to babysit. This sweep's own `(8,4,rows=4)` subprocess
(`cycle_measure 8 4 4 0 100`) was very likely killed by that watchdog instead
of (or alongside) the intended `cycle_measure 8 4 4 0 1` process, since both
were running around the same time on 2026-08-28. **Fixed**: the pattern is now
the exact argv of the monitored invocation (`CMD_PATTERN` in
`auto_stop_power38.py`), and `(8,4,rows=4)` was removed from `run_sweep.py`'s
`SWEEP` entirely (see `MANUAL_ONLY` there) so an automated re-run can't collide
with a concurrent manual measurement again.

**2. The published lower bound for `(8,4,rows=4)` didn't match the checkpoint
left on disk.** RESULTS.md/STATUS.md published `λ > 1.518×10^11` ("> 151
Billion steps"). Parsing the leftover checkpoint directly
(`.cycle_measure_ckpt_n8_w4_rows4_seed0.bin`, format matches `CkptHeader` in
`cycle_measure.c`) gives `phase=0, power=274,877,906,944 (2^38),
lam=315,293,058`. Brent's algorithm's own accounting means total hare-steps
traveled since `x0` is `(power - 1) + lam` -- here **≈2.75×10^11**, not
1.518×10^11 (a ~1.8x gap). File timestamps explain why: `RESULTS.md`
(21:34:47) and `STATUS.md` (21:35:04) were both saved **over an hour before**
the measurement process actually stopped (final checkpoint at 22:45:18,
`auto_stop_power38.log` confirms termination at 22:45:21) -- the published
number was almost certainly frozen from a stale intermediate read and never
corrected once the process (and its watchdog) actually finished. The `>`
notation stayed technically true (the real value is larger, not smaller), but
the specific figure was not reproducible from any artifact left in the repo.

**Lesson for future long-running background measurements**: don't finalize a
headline number into RESULTS.md/STATUS.md while the underlying process is
still running in the background. Wait for the process (and its watchdog) to
actually exit, then re-parse the final checkpoint/output fresh -- treat any
number read while the process might still be advancing as provisional and
say so explicitly if it must be published before completion.

**Corrected re-run**: resumed from this same (valid, uncorrupted) checkpoint
under the fixed watchdog (`cycle_measure 8 4 4 0 1`, started 2026-08-29
14:19). Observed advance rate (~10-13M steps/sec within the current
power=2^38 block) meant reaching a full power-doubling to 2^39 would have
taken several more hours, far longer than justified for this validation
pass -- so after letting it run a bounded ~38 minutes, it was stopped
manually (`SIGINT` then `SIGTERM`, same sequence the watchdog itself uses)
at 14:57:56; the watchdog detected the process was gone and exited cleanly
on its own (no target-power condition needed to fire). Final checkpoint:
**`phase=0, power=274,877,906,944 (2^38), lam=29,615,293,058`** -> total
hare-steps traveled `(power-1)+lam` ≈ **3.045×10^11**. This replaces the
stale `1.518×10^11` figure everywhere it appeared (RESULTS.md §1 table and
§4 point 1, STATUS.md) -- recomputed Ratio After ≈ **1,849×** (was `>921×`)
and Peningkatan Periode ≈ **48,190×** (was `>24,000×`) using this number.
