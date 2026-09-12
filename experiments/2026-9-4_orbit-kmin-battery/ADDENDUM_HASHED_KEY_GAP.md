# ADDENDUM (2026-09-04, later session) — K=1 + hashed-key ordering is NOT clean

## Status: gate **PARTIALLY REOPENED** — K=1 multistream orbit fails PractRand
under hashed (SplitMix32) key ordering, even though RESULTS.md's PASS above
is fully correct for what it tested (Weyl-stepped key ordering only).

## How this was found

While building
`../2026-9-4_orbit-reseed-postboundary-bcfn/multikey_stream_orbit_postreseed_k1.c`
(a driver for an unrelated post-reseed BCFN test — see that folder's
HANDOVER.md), its mandatory pre-flight sanity control included a
`--control-hashed` mode: the SAME pre-reseed K=1 pattern this folder's gate
already validated, but with SplitMix32-hashed key ordering instead of
Weyl-stepped, following the parity principle from
`../2026-9-1_multikey-remix-search/HANDOVER.md` ("diagnosis 2026-09-01
membuktikan keduanya [Weyl & hashed] gagal identik pada defect asli, jadi
keduanya WAJIB dicoba lagi ... untuk parity metodologi"). This folder's own
`RESULTS.md` (above) only ever tested Weyl-stepped keys — the "0 FAILED"
verdict never covered hashed ordering.

That control FAILED PractRand, escalating and severe. This addendum
documents the finding; it does NOT attempt a root-cause fix (deferred to a
future session at the user's direction, 2026-09-04).

## Ruling out a harness bug (done before treating this as real)

`diag_orbit_k1_hashed.c` (this folder) links directly against this folder's
own **already-gate-passed** `ra_core_nomain_v2.o` — the exact object file
this RESULTS.md's PASS verdict is about — using the same key-stepping
pattern as `../2026-9-1_family-productionization/multikey_stream_hashed.c`.
Its output is byte-identical (verified via `od`/diff on a shared base_key)
to the new `--control-hashed` mode in the postreseed-bcfn folder's driver,
which itself links against **canonical** `ra_core.c`. Since two
independently-built binaries (different object files, different source
folders) produce bit-identical streams and both show the same PractRand
failure, this is a property of the `w8_f10_i0` formula itself under this
key ordering, not a bug in either new driver.

## Results

| Run | base_key | scale | Outcome |
|---|---|---|---|
| `control_k1_hashed` (postreseed-bcfn folder) | `0x12345` | 2GB (auto-stopped on FAIL) | FAIL at 1GB, `FPF-14+6/16:all` R=+18.7 |
| `diag_k1_hashed_seedA_8GB` (this folder, `-tlmaxonly` to see full curve) | `0x12345` | 8GB | Escalating: R=+7.8 (512MB) → +18.7 (1GB) → +31.2 (2GB) → +64.0 (4GB) → **+132.3 (8GB)** |
| `diag_k1_hashed_seedB_4GB` | `0xABCDEF01` (different base_key) | 4GB | Same escalation shape: R=+8.2 → +14.8 → +32.5 → +65.0 — **reproducible across seeds**, not one bad base_key |
| `diag_k255_hashed_seedA_8GB` | `0x12345` | 8GB | **Clean.** One "unusual" blip at 1GB (`[Low8/32]DC6-9x1Bytes-1`, R=+6.0), resolved by 2GB, 0 anomalies at 2/4/8GB (230 test results) — ordinary statistical noise, not the BCFN pattern. |

Raw logs (copied from the VPS run to
`../2026-9-4_orbit-reseed-postboundary-bcfn/`):
`control_k1_hashed.log`, `control_k1_weyl.log` (clean, for contrast),
`control_k255_hashed.log`, `control_k255_weyl.log` (clean),
`diag_k1_hashed_seedA_8GB.log`, `diag_k1_hashed_seedB_4GB.log`,
`diag_k255_hashed_seedA_8GB.log`.

Test family (`FPF-14+6/16`) and escalation shape match the ORIGINAL
pre-fix defect (`../2026-9-1_multikey-remix-search/HANDOVER.md`, R=+40.7 at
4GB) closely enough to call it the same underlying phenomenon — except this
instance is even more severe at matched scale (R=+64.0 vs +40.7 at 4GB).

## Scope of the gap (precisely, from the data above)

- **K=1 + Weyl-stepped keys**: clean (this folder's original 16GB gate,
  unaffected by this addendum).
- **K=1 + hashed (SplitMix32) keys**: **FAILS**, severely and reproducibly
  across at least 2 independent base_keys.
- **K=255 + Weyl-stepped keys**: clean (this folder's original 16GB gate).
- **K=255 + hashed keys**: clean (8GB, this addendum).

So the residual defect is narrow but real: it appears specifically at the
finest reinit granularity (K=1) combined with a decorrelated/hashed key
sequence. `w8_f10_i0` was evidently validated and (implicitly) tuned only
against Weyl-ordered keys in this folder's gate — that ordering happens not
to expose whatever structure remains, while a hashed ordering does.

## What this does NOT mean

- It does NOT mean `w8_f10_i0` is worthless — K=255 (both orderings) and
  K=1+Weyl are still genuinely clean, and this is real signal, not scope
  creep.
- It does NOT mean the promotion to canonical `ra_core.c`
  (`../2026-9-1_family-productionization/ra_core.c`) was wrong to do based
  on what was known then — the K=1+Weyl and K=255 results this RESULTS.md
  reports are correct and remain valid.
- It does NOT by itself say anything about `ra_reseed`/`ra_hash` (the
  post-reseed-boundary question `../2026-9-4_orbit-reseed-postboundary-bcfn/`
  was built to answer) — that test remains blocked on this gap being
  resolved, since its own mandatory sanity control is exactly this K=1
  pre-reseed pattern.

## Status / next steps (deferred, not done in this session)

Root-causing WHY hashed ordering exposes this while Weyl doesn't (init
formula `ra_init_state_orbit` vs cycle transform
`ra_permutation_cycle_orbit`, or an interaction between the two) needs a
full disciplined elimination chain, same rigor as
`../2026-9-1_multikey-remix-search/HANDOVER.md`. Not attempted here per
user direction (2026-09-04): document and stop, defer full root-cause +
fix search to a dedicated future session.

**Until this is resolved:**
- Treat "orbit K-small defect gate CLOSED" (commit `be4a14a`) as accurate
  for Weyl key ordering only, NOT as a general "K-small is safe" claim.
- `../2026-9-4_orbit-reseed-postboundary-bcfn/`'s post-reseed BCFN test
  stays blocked (its 4 main PractRand runs were never executed) until this
  K=1+hashed gap is fixed and re-validated.
- See `PRODUCTION_READINESS_HANDOVER.md` for the reopened status entry.
