# RESULTS — zero-tap init hypothesis: FALSIFIED

## Status: **hypothesis tested and REJECTED — patch does not close the K=1+hashed-key gap**

## Hypothesis (see conversation, 2026-09-04)

Static code reading of `ra_init_state_orbit` (canonical `ra_core.c`) found
that the original formula `l_val = (i * const) * keyterm` / `m_val = (i *
const) * keyterm` is exactly 0 at `i=0` (since `0 * anything = 0`, and
`rot32(0, r) = 0` for any rotation `r`). This makes **`L[0]` and `M[0]`
always zero, for every key, unconditionally** — confirmed both by a Python
arithmetic replica and by a real compiled-C probe
(`ra_probe_init_orbit_zero_check`, added to `ra_core_v3.c`).

`ra_permutation_cycle_orbit`'s wide-`o` computation reads
`M[(uint8_t)(255+1)] = M[0]` as one of its 8 XOR taps, unconditionally, on
the very first cycle of every `ra_core_orbit` call — so that tap
contributes zero entropy on cycle 0, effectively narrowing 8 taps to 7 for
the first output word of every call. Hypothesis: this dead tap is the
mechanism behind the K=1+hashed-key BCFN gap documented in
`../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md` (K=1 exposes it
maximally since there's only ever one cycle step; larger K dilutes it away
across many steps, matching the pattern already seen: K=1+hashed FAILs,
K=255 both orderings clean).

## Patch tested

`ra_core_v3.c` (copy of canonical `ra_core.c`, this folder only): shifted
the index in `ra_init_state_orbit`'s multiply from `i` to `(i + 1)`, so no
index in `[0,255]` ever zeroes the multiply:

```c
uint32_t l_val = (uint32_t)((i + 1) * 0x9e3779b7u) * keyterm_l;
uint32_t m_val = (uint32_t)((i + 1) * 0x06a0dd9bu) * keyterm_m;
```

**Verified the patch does what it claims**, before running any statistics:
`ra_probe_init_orbit_zero_check` on 6 keys (`0`, `1`, `0x12345`,
`0xABCDEF01`, `0xFFFFFFFF`, `0xdeadbeef`) shows `M[0]`/`L[0]` are now
well-distributed nonzero 32-bit values for every key tested, matching an
independent Python arithmetic replica exactly (e.g. key=0 → `M[0] =
0x7aa843fe` in both). So the specific static-analysis finding is real and
the patch genuinely eliminates it — the fix hypothesis itself was
correctly implemented.

## PractRand result: essentially NO CHANGE from the unpatched formula

| Run | Unpatched (`ADDENDUM_HASHED_KEY_GAP.md`) | Patched (`ra_core_v3.c`, this folder) |
|---|---|---|
| K=1+hashed, base_key=0x12345, @512MB | R=+7.8 | R=+6.9 |
| @1GB | R=+18.7 (FAIL) | R=+13.7 (FAIL) |
| @2GB | R=+31.2 | R=+27.3 |
| @4GB | R=+64.0 | R=+63.7 |
| @8GB | **R=+132.3** | **R=+135.2** |
| K=1+hashed, base_key=0xABCDEF01, @512MB | R=+8.2 | R=+10.2 |
| @1GB | R=+14.8 (FAIL) | R=+17.3 (FAIL) |
| @2GB | R=+32.5 | R=+33.4 |
| @4GB | **R=+65.0** | **R=+69.8** |

Same escalating `FPF-14+6/16` signature, same order of magnitude, same
checkpoint-by-checkpoint shape, for both independent base_keys. The
patched run at 8GB (R=+135.2) is not better than unpatched (R=+132.3) —
within run-to-run noise, arguably marginally worse. **The dead-M[0]/L[0]
tap is NOT the (sole) mechanism causing this defect.**

Raw logs: `v3_k1_hashed_seedA_8GB.log`, `v3_k1_hashed_seedB_4GB.log`.

## What this rules out / what's still open

- Ruled out: the specific "one of 8 `o`-taps is always zero on cycle 0"
  mechanism, as the (sole) explanation for the K=1+hashed gap. The patch
  correctly fixes that specific defect (verified independently) and it
  changes essentially nothing statistically.
- Still open: the real mechanism. Candidates not yet tested — the raw
  (unhashed) use of `key` directly as `cons`/`a`'s seed in
  `ra_permutation_cycle_orbit` (`a = o ^ (key+key)`, `b = (key+a) ^ o`);
  the shared structure of `keyterm_l`/`keyterm_m` (computed once per key,
  reused via multiply-by-small-constant across the 8 relevant M[]
  indices — this reuse itself, not just the i=0 special case, may be the
  actual thin point); or something in `ra_permutation_cycle_orbit`'s
  transform (`rot32((a>>13)^a, b)` + `c^=c>>17`) that's specifically weak
  for a single-round (K=1) application regardless of how M[]/L[] are
  built.
- No further hypothesis tested this session — per repo standard
  (`feedback_no_reseed_on_fail`), a clear negative result stands, no
  retry/rationalization. Regression sanity (K=1+Weyl, K=255 both
  orderings) with this patch was NOT run since the patch is not being
  promoted or pursued further — no point spending compute validating a
  fix candidate already shown ineffective.

## Verdict

**Reject this fix candidate.** `ra_core_v3.c` and this folder are a
negative result for the record, not a promotion candidate. The K=1+hashed
gap remains open — see
`../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md` for status. A
real root-cause diagnosis needs the disciplined elimination-chain approach
(isolate one variable at a time, same rigor as
`../2026-9-1_multikey-remix-search/HANDOVER.md`), not a single
static-analysis-derived guess-and-check.
