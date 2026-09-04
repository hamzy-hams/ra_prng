# RESULTS — Statistical re-validation: orbit K-small-defect fix

## Status: **GATE PASS — 0 FAILED across all tests, no anomalies in PractRand**

This folder runs the statistical battery that
`../2026-9-4_orbit-fix-and-wideo-rolling-optimization/HANDOVER.md` §6
explicitly deferred and gated promotion on:

> "Re-validasi statistik (PractRand/dieharder) untuk formula orbit
> K-kecil-fix — deferred ke sesi lain... JANGAN promosikan ke `src/` atau
> `ra_core.c` kanonik sampai battery ini selesai dan PASS."

The formula under test is `ra_permutation_cycle_orbit()` as changed in
`ra_core_v2.c` (8-tap `o` + XORSHIFT(17) finalizer — the same `w8_f10_i0`
formula already promoted to `ra_permutation_cycle_singleblock` on
2026-09-03 — plus the bit-exact rolling-register optimization for `o`,
proven separately via KAT in that folder and not re-tested here since it's
an algebraic identity, not a new formula). Tested in the "multistream"
usage pattern that exposed the original gap: many short `ra_core_orbit()`
calls, fresh key each time (Weyl/golden-ratio stepping, same convention as
every other multikey battery in this repo), for both K=1 (worst case —
one word per reinit) and K=255 (natural full-block reinit).

## Methodology

Mirrors `../2026-9-1_production-candidate-battery/` (the singleblock
K-small-fix gate, commits `e7628c1`/`2bbdce9`), pointed at
`ra_core_nomain_v2.o` (compiled from `../2026-9-4_orbit-fix-and-wideo-rolling-optimization/ra_core_v2.c`)
instead of canonical `ra_core.c` — the fix is not yet in canonical, so the
gate has to run against the file that has it. See `BUILD.sh`.

- `./ra_core_v2 validate` before any generation: 0 checksum mismatches
  (singleblock AND orbit KAT), confirming no drift from the already-verified
  `ra_core_v2.c`.
- `multikey_stream_orbit_k1` / `multikey_stream_orbit_k255`: chaining
  drivers, block_len=1 / 255, `ra_core_orbit()` reinit with a new
  Weyl-stepped key every block.
- dieharder: `run_dieharder_battery.py` (unmodified copy), 27 "Good"-
  reliability tests, `BASE_KEY=111222` (consistent with every dieharder run
  in this repo).
- PractRand: `RNG_test stdin32 -tlmax 16GB -multithreaded`, matching the
  2026-09-03 singleblock gate's 16GB standard.
- collision-scan: `collision_scan_orbit_k1.py`, K=1, 2**32-space Poisson-z
  model (raw 4-byte fingerprint comparison, not hashed), 50,000 keys,
  sequential AND random seed patterns.
- avalanche: `avalanche_orbit_kmin.py`, per-bit Hamming-distance gate
  (band [0.3,0.7], per-bit floor 0.2), both K=1 and K=255.

## Results

| Test | K=1 | K=255 |
|---|---|---|
| dieharder (27 tests) | 25 PASSED, 2 WEAK, **0 FAILED** | 25 PASSED, 1 WEAK, **0 FAILED** |
| PractRand 16GB | no anomalies, all checkpoints 64MB→16GB | no anomalies, all checkpoints 256MB→16GB |
| avalanche | PASS (mean 0.513672, min-bit 0.250000) | PASS (mean 0.498828, min-bit 0.481005) |
| collision-scan (50k keys) | PASS both seed modes (0 collisions, expected~0.3, z=-0.54) | not run (K=255 collision space is 255-word blocks, out of scope for this battery — orbit K=255 was already covered by the pre-existing "128GB clean" long-stream validation and the multistream dieharder/PractRand runs above) |

dieharder WEAK sub-results (not FAILED, isolated single-test noise, same
class of result the singleblock gate accepted on 2026-09-03):
- K=1: `diehard_count_1s_str` (p=0.99998019), `dab_filltree` (p=0.00226324)
- K=255: `diehard_squeeze` (p=0.99867369)

Raw logs: `dieharder_orbit_K1_piped.txt`, `dieharder_orbit_K255_piped.txt`,
`practrand_orbit_K1_16GB.log`, `practrand_orbit_K255_16GB.log`,
`collision_scan_orbit_k1_results_{sequential,random}_full.json`.

## Verdict

Zero FAILED anywhere, zero PractRand anomalies at 16GB for either K=1 or
K=255. Per repo-standing rule (`feedback_no_reseed_on_fail` — one FAIL
would have meant reject, no retry with a different seed), this is an
unambiguous **PASS**. The `w8_f10_i0` cycle-transform fix, when ported to
`ra_permutation_cycle_orbit`, closes the K-small multistream gap the same
way it closed it for singleblock — confirming the root-cause diagnosis in
`../2026-9-4_orbit-fix-and-wideo-rolling-optimization/HANDOVER.md` §1 (the
defect was in the shared cycle-transform formula, not the init path).

**Promotion to canonical `ra_core.c` is now unblocked** — see
`../2026-9-1_family-productionization/ra_core.c`'s updated provenance
comments and `PRODUCTION_READINESS_HANDOVER.md` for the applied change.
Promotion of `ra_core.c` itself to `src/` remains a separate, unscoped
decision (see that doc).
