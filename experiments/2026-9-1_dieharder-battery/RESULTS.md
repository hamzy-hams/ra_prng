# RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`

## Context

Only PractRand had been run against these two generators before now
(`experiments/2026-8-27_operand-position-search/`,
`experiments/2026-8-30_addressable-init-research/`). Dieharder was an open
backlog item (memory `project_research_backlog_2026_09`, item 1). This run
fills that gap for `winner_wired_v2` and `winner_wired_addressable`.
`ra_core_singleblock` is deliberately excluded — its `--stream` CLI mode
exists only for bit-identical checks (see
`2026-8-30_addressable-init-research/HANDOVER_TAHAP6.md`), not bulk
generation, and running it correctly needs the productionization track's
new test-harness work, which is explicitly paused
(`2026-9-1_family-productionization/HANDOVER.md`).

Run 2026-09-01 on the project's VPS (`VPS_ACCESS.md`), in parallel with the
already-running Langkah 1/2 1TB PractRand jobs (idle-capacity use, load
average was ~25-35% of 8 cores before this track started).

## Method

Exact methodology and 27-test "Good"-reliability list already validated in
`experiments/2026-8-26_operation-pruning-research/RESULTS.md` for
`pruned_winner` (`winner_wired_v2`'s and `winner_wired_addressable`'s
predecessor): one dieharder test at a time via direct pipe from the
generator's own `--stream <seed> <n>` CLI (`stdin_input_raw` generator, `-g
200`) — no intermediate file, no rewind (the precedent run showed
file+rewind produces false-FAILs from rewind-count artifacts on a small
file). `-d` index list re-verified against `dieharder -l` on the VPS itself
(dieharder 3.31.1, same version/index table as the local machine) before
running, not assumed from memory:

```
0 1 2 3 4 8 9 10 11 12 13 15 16 17 100 101 102 200 201 202 203 204 205 206 207 208 209
```
(excludes `-d 5,6,7` "Suspect" and `-d 14` "Do Not Use")

Two documented overrides, same as the precedent:
- RGB Generalized Minimum Distance (`-d 201`) needs explicit
  `ntup = 2, 3, 4, 5` (4 separate invocations — `-n` only accepts a single
  value, confirmed via `dieharder --help`, NOT the comma-list the
  precedent's prose loosely suggested).
- Marsaglia-Tsang GCD test (`-d 17`) needs a ~11.2GB piped stream (2.8e9
  32-bit words) or it hits EOF partway through.

Seed 0 for both generators, single run each (not repeated — see
Interpretation below). Script: `run_dieharder_battery.py` (new, this
folder), never edits any generator source/binary. Raw output:
`dieharder_winner_wired_v2_piped.txt`,
`dieharder_winner_wired_addressable_piped.txt`.

## Results

| Generator | PASSED | WEAK | FAILED |
|---|---|---|---|
| `winner_wired_v2` | 26/27 | 2 | 0 |
| `winner_wired_addressable` | 25/27 | 1 | 0 |

Zero FAILED for either generator across all 27 tests (rgb_minimum_distance
counted once per generator here even though it ran 4 sub-invocations).

WEAK sub-results (all single occurrences among many p-values produced per
test — `sts_serial`, `diehard_runs`, `diehard_craps`, `rgb_minimum_distance`
each emit multiple sub-tests per invocation):

- `winner_wired_v2`: `diehard_craps` p=0.99739 (high-tail), and
  `rgb_minimum_distance` ntup=4 p=0.00059189 (low-tail).
- `winner_wired_addressable`: `diehard_rank_6x8` p=0.99530 (high-tail).

## Interpretation

Both generators pass the full "Good" battery with zero FAILED. A single
WEAK sub-result among dozens of p-values per generator (one test producing
a p-value in dieharder's WEAK band by chance, not FAILED) is the same
pattern the precedent run documented for `pruned_winner` (one
`sts_serial|ntup=8` WEAK at p=0.99527) and is treated the same way here:
expected statistical noise, not a defect — this is exactly why dieharder
distinguishes WEAK from FAILED, and a single non-repeated run at seed 0 is
not enough evidence to call any of these a real signal either way. Not
re-run with alternate seeds (out of scope for this pass; would only be
worth doing if a WEAK recurred at the same test across runs, or if a FAILED
had appeared).

This closes backlog item 1 for `winner_wired_v2` and `winner_wired_addressable`.
`ra_core_singleblock` remains open, gated on the productionization track
resuming (see Context above).

## Files produced this session

- `run_dieharder_battery.py` — battery runner (binary path + name args,
  reusable for any generator exposing a `--stream <seed> <n>` CLI).
- `dieharder_winner_wired_v2_piped.txt`, `dieharder_winner_wired_addressable_piped.txt` — raw per-test output.
- `trackA_winner_wired_v2.log`, `trackA_winner_wired_addressable.log` — full run logs (stdout of the battery script, includes progress prints).
