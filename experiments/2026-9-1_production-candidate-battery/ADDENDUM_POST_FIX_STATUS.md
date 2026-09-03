# ADDENDUM_POST_FIX_STATUS.md — Step 4-8, post `w8_f10_i0` promotion

Continues `RESULTS.md`, which stopped after Step 3 when a K=1 structural
defect in `ra_core_singleblock` blocked all remaining steps. That defect
is now fixed and promoted into `ra_core.c` (`w8_f10_i0`: 8-tap `o` +
XORSHIFT(17) finalizer, commit `89ffc95`) — see
`../2026-9-1_family-productionization/PRODUCTION_READINESS_HANDOVER.md`.
This addendum runs Step 4-8 against a **fresh rebuild of this folder's own
binaries** (`./BUILD.sh`, re-run 2026-09-03) from the fixed `ra_core.c`,
not the standalone combo-search candidate binary that validated the same
formula in isolation.

## Step 0 (rebuild) — PASS

`./BUILD.sh` re-run; all 7 binaries changed (checksums differ from the
pre-fix build). `./ra_core validate`: 9945/9945 checksum matches (KAT
table, see `ra_core.c`'s `SINGLEBLOCK_KAT_CHECKSUMS`) — PASS.

## Step 4 — collision-scan K=1: PASS

New script: `collision_scan_singleblock_k1.py`. Not a copy-edit of the
existing K=255 `collision_scan_ra_core_singleblock.py` (that script's
`digest_collision_prob()` assumes a 64-bit BLAKE2b digest space, valid for
255-word blocks but wrong for K=1: a single 32-bit raw word hashed up to
64 bits creates no entropy that wasn't there, so the real collision space
is 2^32, not 2^64). This script compares raw 4-byte output directly
(no hashing) and scores against the correct `comb(M,2)/2**32` expectation
via a Poisson z-score, not a "must be exactly 0" threshold.

| Seed mode | M | Collisions found | Expected (2^32 space) | z | Verdict |
|---|---|---|---|---|---|
| sequential | 50,000 | 0 | 0.291 | -0.54 | PASS |
| random | 50,000 | 0 | 0.291 | -0.54 | PASS |

(Correction to the pre-run estimate in this session's plan: the expected
count is ~0.29, not ~291 — an arithmetic slip during planning; the
methodology point it was illustrating — correct space is 2^32, not
2^64 — still holds and is what the script implements.)

## Step 5 — dieharder, K=255 & K=1: PASS

New driver: `run_dieharder_battery.py` (3-arg CLI adaptation of
`../2026-9-3_dieharder-inject-crossing/run_dieharder_battery.py` for this
folder's fixed-K binaries `multikey_stream`/`multikey_stream_singleblock_k1`).
Same 27 "Good"-reliability tests, same piped-no-file methodology,
`BASE_KEY=111222`.

| Stream source | PASSED | WEAK | FAILED |
|---|---|---|---|
| `multikey_stream` (K=255) | 25/27 | 1 | 0 |
| `multikey_stream_singleblock_k1` (K=1) | 25/27 | 2 | 0 |

**0 FAILED**, comparable to every other dieharder run in this repo for
this formula family. Raw output: `dieharder_K255_piped.txt`,
`dieharder_K1_piped.txt`.

## Step 6 — PractRand, K=255 & K=1, 16GB (VPS): PASS

`RNG_test stdin32 -tlmin 256MB -tlmax 16GB -multithreaded`, fed from this
folder's `multikey_stream`/`multikey_stream_singleblock_k1`, run on the
VPS (`VPS_ACCESS.md`) after syncing the fixed `ra_core.c` and rebuilding
there. **Both fully clean** — "no anomalies" reported at every checkpoint
(168 through 240 test results) up to the full 16GB target, for both K=255
and K=1. Logs: `practrand_K255_16GB.log`, `practrand_K1_16GB.log`.

This confirms, on the actual gate binaries (not just the standalone
combo-search candidate), the K=1 structural defect that originally
blocked this folder is closed.

## Step 7 — shuffle-implementation, K=1 & K=255: PASS with a noted caveat

### Bug found and fixed: glibc `fmemopen` exact-buffer-size clobber

First run of `scrambler_ra_core_singleblock --mode k1` produced a
catastrophic result: every 255-element shuffle collapsed to a trivial
1-position rotation (`[1,2,...,254,0]`), runs-test z=-3528, serial
correlation r=0.976. Root-cause debugging (instrumented rebuild, byte-level
comparison against the raw generator stream) found this was **not an RNG
defect** but a bug in the test harness: `pull_one()`/`pull_n()` in
`scrambler_ra_core_singleblock.c` called `fmemopen(buf, exact_write_size,
"wb")`. glibc's `fmemopen` in write mode appends a trailing NUL byte after
the last byte written; if the buffer has no spare room, that NUL
**overwrites the last byte of the buffer** instead of landing past it —
on little-endian this always clobbers the MSB of the last `uint32_t`
written. Verified directly: an exact 4-byte buffer written with
`0x17ab341f` came back as `0x00ab341f`; a 5-byte buffer (1 spare byte)
came back correct.

**Impact assessment** (grepped every `fmemopen(...)` call in the repo):
- `pull_one()` (K=1 mode): every one of 254 words per shuffle rep hit this
  bug (every call opens a fresh exact-4-byte buffer) — this is the full
  root cause of the catastrophic result above.
- `pull_n()` (K=255 mode): only the LAST of 254 words per rep hit it —
  diluted 1/254, explaining why K=255 looked clean even before the fix.
- `checksum_key()` in `../2026-9-1_family-productionization/ra_core.c`
  (this session's own new KAT-checksum `validate` function): same
  exact-buffer pattern, affecting the last word's MSB per key. Harmless in
  practice (self-consistent — golden checksums and verification both used
  the same buggy capture, so no false PASS/FAIL risk for future
  regressions), but incomplete coverage. **Fixed** (buffer padded by 1
  word; golden `SINGLEBLOCK_KAT_CHECKSUMS` regenerated with the fix).
- Every other `fmemopen` use in the repo (`tahap6_bench.c`,
  `benchmark_all.c`, `diag_keyzero_guard.c`, the OLD (now-retired)
  orbit-vs-singleblock `run_validate_singleblock`) compares two buffers
  that are BOTH captured with the same exact-size pattern — the bug
  cancels out in those bit-identity comparisons, so no historical PASS
  result in this repo is called into question by this finding.

Both `scrambler_ra_core_singleblock.c` and `ra_core.c` were patched (spare
byte in the `fmemopen` buffer), rebuilt, and re-verified (70-vector
cross-check against the independently-validated `w8_f10_i0` candidate
binary, 0 mismatches; `./ra_core validate` PASS with regenerated
checksums).

### Post-fix scoring

Re-ran with fixed binaries: `--mode k255`/`--mode k1`, `--reps 20000`
(5.1M tokens each), scored with the 4 pure functions from
`../2026-8-30_addressable-shuffle/scc_test.py` (`entropy`,
`chi_square_uniform`, `runs_test`, `serial_correlation`) — reused, not
re-derived.

| Mode | Entropy (max 7.994353) | Chi-Square (df=254) | Runs z | Serial corr |
|---|---|---|---|---|
| K=255 | 7.994353 | 0.000000 | -0.35 | -0.0045 |
| K=1 | 7.994353 | 0.000000 | +9.25 | -0.0067 |

The catastrophic pattern is gone. K=255 is clean by every measure. K=1's
entropy/chi-square/serial-correlation are all clean, but its global
runs-test z (+9.25) is a real, non-noise signal (not just a threshold
artifact) — confirmed via a per-repetition breakdown (2000 reps, 255
elements each): mean per-rep z = 0.242 (SE 0.022, t≈11) for K=1 vs 0.155
(SE 0.023, t≈7) for K=255. **Both modes show a small systematic positive
bias** — plausibly partly a known artifact of applying the classic
runs-test formula (derived for i.i.d. continuous data) to permutation
data, which has no ties/repeats by construction — but K=1's bias is
larger than K=255's by ~2.8 sigma, a real (if small) difference, not
explained away by the shared permutation-methodology factor alone.

**Verdict (user-confirmed 2026-09-03): PASS with a noted caveat.** This
bias is far smaller than the pre-fix catastrophic defect, is invisible to
bulk PractRand at 16GB (Step 6), and does not block the `w8_f10_i0`
promotion already shipped in `ra_core.c`. It is recorded here as a known,
narrow limitation specific to low-bit-extraction use over `singleblock`'s
most aggressive K=1 reinit pattern (e.g. Fisher-Yates index derivation),
not a reason to revisit the promotion decision.

## Overall gate verdict: PASS

Steps 0-7 all PASS (Step 7 with the caveat above, explicitly accepted).
`production-candidate-battery` gate for `ra_core.c` is **CLOSED** —
`ra_core_orbit`/`ra_core_singleblock` (with the `w8_f10_i0` fix) have no
outstanding blocker from this gate. See
`PRODUCTION_READINESS_HANDOVER.md` for the updated top-level status.
