# PRNG Family Benchmark: Results

Implements `HANDOVER.md`'s 12-candidate benchmark (`benchmark_all.c` +
`benchmark_all_run.py`, this directory). Every candidate's timed function
seeds **and** generates its `rng` words in one call — no candidate gets its
reseed cost hidden outside the timer the way the old Philox pattern in
`tahap6_bench.c`'s `mode_reinit_sweep` did. Full raw numbers are in
`benchmark_all_results.json`.

## Topline answer

MT19937's fixed seeding cost is **~26x** the next-highest candidate
(chacha20) and **~260x** pcg32's — and it lines up almost exactly with the
theoretical prediction (624-word `init_genrand` fill + 624-word first twist
≈ 1248 word-equivalent ops ≈ the measured ~1257ns), which is a good sign the
harness is measuring something real, not noise. At K=1 (frequent reinit —
the addressable use case `ra_prng`'s research line targets), that makes
MT19937 ~39x more expensive per word than `singleblock`. But at steady
state (K=1,000,000) MT19937 actually edges out Philox and the `ra_prng`
variants. Reporting MT19937's flat throughput alone (as most benchmarks do)
would make it look competitive; only measuring seed+generate together
exposes that it's specifically bad at the frequent-reseed pattern
`ra_prng`'s addressable line optimizes for.

## Validation

- **KAT**: 20 vectors across 6 candidates (Philox, xoshiro256\*\*, pcg32,
  chacha20 ×2, MT19937 ×2, SplitMix64) — all PASS.
- **Exhaustive crosscheck**: `singleblock` vs `addrcont`, 9,945 combinations
  (39 keys × 255 lengths) — 0 mismatches.
- **Byte-for-byte crosscheck**: `paperorig`/`wiredv2`/`addrcont` `--stream`
  output diffed against the real `ra_prng2.c`/`winner_wired_v2.c`/
  `tahap6_bench` binaries, 5 keys × 4 lengths each — all PASS.
- **MB/s sanity check** vs `HANDOVER.md`'s literature reference numbers —
  all overlapping candidates land within 0.98–1.30x (table below), no
  order-of-magnitude porting-bug flags.
- **Bug found in existing repo reference file**: while building chacha20's
  KAT, `benchmarks/comparisons/RNGing_speed/src/chacha20.c`'s own benchmark
  `main()` turned out to use a byte-order-swapped nonce constant versus RFC
  8439 §2.3.2 (`0x00000009`/`0x0000004a` instead of the correct
  `0x09000000`/`0x4a000000`). Verified against the actual RFC 8439 text
  fetched during this session. The `chacha20_block()` round-function logic
  itself is correct — only that file's hardcoded test constant is wrong.
  Not fixed in place (read-only precedent per HANDOVER §6); `benchmark_all.c`
  uses the corrected nonce and documents the discrepancy in a comment.

## Seeding cost — OLS fit `call_ns(K) = a + b·K`, sorted by fixed cost `a` ascending

| core | `a_ns` (fixed seed cost) | `b_ns_per_word` (steady-state) |
|---|---:|---:|
| philox | −6.1 *(noise around ~0 — Philox's reseed really is just a word assignment)* | 2.600 |
| splitmix64 | 3.6 | 0.245 |
| pcg32 | 4.8 | 1.288 |
| paperorig | 10.4 | 5.263 |
| xoshiro256 | 12.7 | 0.937 |
| singleblock | 33.7 | 0.001 |
| wiredv2 | 33.8 | 2.080 |
| addrcont | 39.4 | 2.077 |
| chacha20 | 47.9 | 7.344 |
| **mt19937** | **1257.2** | 1.775 |

## K=1 (frequent reinit) vs largest-K (steady state), sorted by K=1 cost ascending

| core | ns/word at K=1 | largest K measured | ns/word at largest K |
|---|---:|---:|---:|
| splitmix64 | 3.0 | 1,000,000 | 0.245 |
| pcg32 | 3.2 | 1,000,000 | 1.288 |
| xoshiro256 | 6.5 | 1,000,000 | 0.937 |
| philox | 12.1 | 1,000,000 | 2.600 |
| paperorig | 24.9 | 1,000,000 | 5.263 |
| wiredv2 | 25.2 | 1,000,000 | 2.080 |
| singleblock | 33.4 | 255 *(hard cap, see note)* | 0.132 |
| addrcont | 52.1 | 1,000,000 | 2.077 |
| chacha20 | 121.5 | 1,000,000 | 7.344 |
| **mt19937** | **1289.4** | 1,000,000 | 1.776 |

`singleblock`'s "largest K" is capped at 255 by design (no-reseed fast path,
valid only for `rng<=255` — see `HANDOVER_TAHAP6.md`), so its steady-state
column isn't the same asymptotic flat-throughput regime the uncapped
candidates reach at K=1,000,000; it's still the best amortization
`singleblock` can reach within its valid range.

## MB/s sanity check, sorted descending

| core | measured MB/s | HANDOVER.md reference | ratio |
|---|---:|---:|---:|
| singleblock | 30303.0 | n/a *(no reseed within cap — see note above, not directly comparable)* | — |
| splitmix64 | 16326.5 | n/a *(new candidate, no prior literature figure)* | — |
| xoshiro256 | 4268.9 | 3574.0 | 1.19 |
| pcg32 | 3105.6 | 3065.0 | 1.01 |
| mt19937 | 2252.3 | n/a *(new candidate)* | — |
| addrcont | 1925.9 | n/a *(no prior flat-throughput figure)* | — |
| wiredv2 | 1923.1 | n/a | — |
| philox | 1538.5 | 1304.0 | 1.18 |
| paperorig | 760.0 | 745.6 | 1.02 |
| chacha20 | 544.7 | 555.5 | 0.98 |
| dev_urandom | 507.0 | 389.6 | 1.30 |

## Closing note

This repo's `CLAUDE.md` mandates `/graphify --update` before closing a
research task. No `graphify` skill or CLI is actually installed anywhere on
this machine (checked `~/.claude/skills`, `~/.claude/commands`, installed
plugins, `which graphify`, `pip`/`npm` global — nothing found; only a stale
`.agents/rules/graphify.md` from a different tool, and old `graphify-out/`
snapshots from a prior, no-longer-available run). Flagging rather than
silently skipping, per this repo's own `HANDOVER.md` §8 instruction to do
exactly that if it's still missing.
