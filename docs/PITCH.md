# RA-PRNG — Landing Page Pitch (Draft)

> Internal draft for a public landing page. English copy below is meant to
> be lifted almost verbatim into HTML. Every number carries a source
> citation in `[brackets]` — keep those when this becomes a doc/PR, drop
> them (or move to a footnote) in the final HTML.
>
> Status: **draft for review** — see the "Honest Limitations" section;
> it is not optional decoration, it must ship on the real page too.

---

## 1. Hero

**RA-PRNG**
### The random number generator that shuffles as it generates.

Most PRNGs generate numbers, then something else shuffles your data with
them. RA-PRNG's core loop *is* the shuffle — two tiny arrays permute and
mutate each other continuously, so entropy generation and index-shuffling
happen in the same step, in the same ~2KB of state.

**At a glance:**
- **8,351 MB/s** peak throughput, batch mode — 6.1× faster than
  Philox4x32-10, 2.27× faster than xoshiro256\*\* on the same machine, same
  session `[experiments/2026-9-1_family-productionization/RESULTS.md:192-204]`
- **0 failures** across 114 Dieharder tests `[benchmarks/results/ra_prng2_original/dieharder_test.txt]`
- **Statistically clean to 1 TB** of output under PractRand, zero anomalies
  at every checkpoint `[experiments/2026-9-6_orbit-address-widen-64bit-promotion/RESULTS.md]`

[ Read the paper ] [ View on GitHub — *link TBD* ]

---

## 2. What it is

RA-PRNG is a pseudo-random number generator family built around one idea:
**the shuffle is the generator.** Instead of drawing a random number and
then using it to swap array elements (the way a Fisher–Yates shuffle
normally works), RA-PRNG's state is two small 256-entry arrays that
continuously rotate, XOR, and swap against each other in a single
branchless loop. There is no separate "generate" step and "shuffle" step —
they are the same operation, running on a state small enough (~2KB) to
live entirely in L1 cache.

The newest member of the family, **`ra_core`**, adds what we call
**Orbit Addressing**: the ability to derive a starting state directly from
a `key` (and a 64-bit `addr_hi` parameter), so you can jump straight to an
independent random stream or address without paying for a full reseed.

---

## 3. Why it's different

| | Classic PRNGs (xorshift, PCG, Mersenne Twister) | Counter-based (Philox) | RA-PRNG |
|---|---|---|---|
| Shuffle vs. generate | Two separate steps | Two separate steps | **One operation** |
| State size | Varies (MT19937: ~2.5 KB+) | Small | **~2 KB, L1-resident** |
| Jump to an arbitrary stream | Full reseed | Native (counter) | **Native (Orbit Addressing)**, different mechanism |
| Reseed cost | Can be expensive (MT19937 ≈ 1257 ns/seed) `[experiments/2026-8-31_prng-family-benchmark/RESULTS.md]` | Low | Low |

RA-PRNG isn't trying to out-crypto anyone — it's a different architectural
bet: fold permutation and generation into one branchless loop, keep the
whole state in cache, and make "give me stream #42" a first-class,
cheap operation.

---

## 4. By the numbers

### Speed

All numbers below are from the **same machine, same benchmark session**
(Intel i3-1115G4, GCC -O3 -march=native, min-of-5+ trials) to keep the
comparison fair — see the note at the end of this section on why that
matters.

| Mode | Configuration | Throughput | vs. Philox4x32-10 | vs. xoshiro256\*\* |
|---|---|---|---|---|
| `ra_core_singleblock` | K=255 (its designed regime: batches ≤255 words/key, no reseed) | **8,351 MB/s** | **6.1× faster** | **2.27× faster** |
| `ra_core_orbit` | Continuous stream | ~1,360 MB/s | roughly on par | ~2.8× slower |

*Source: `experiments/2026-9-1_family-productionization/RESULTS.md:192-204`
and `experiments/2026-9-3_orbit-continuous-bench/RESULTS.md`.*

> **Why we only quote same-session numbers:** an earlier, widely-different
> throughput figure for this project (30,303 MB/s) turned out to be an
> algebraic derivation that didn't hold up under a real multistream
> benchmark (~154–388 MB/s in that specific test) — see
> `experiments/2026-9-3_orbit-continuous-bench/RESULTS.md`. We only publish
> numbers that were measured directly, on one machine, in one session, and
> we say exactly which mode and which K they belong to.

### Statistical quality

| Test suite | Result | Source |
|---|---|---|
| Dieharder (3.31.1, ~113 MB input) | **0 FAILED / 2 WEAK / 112 PASSED** | `benchmarks/results/ra_prng2_original/dieharder_test.txt` |
| PractRand | **Clean to 1 TB (2⁴⁰ bytes)**, 0 anomalies at every checkpoint (64GB → 1TB), current production formula | `experiments/2026-9-6_orbit-address-widen-64bit-promotion/RESULTS.md` |
| NIST STS | Full pass | per paper evaluation, CSAI2025 manuscript |
| TestU01 BigCrush | 160/160 statistics passed | per paper evaluation, CSAI2025 manuscript |

---

## 5. How it works — two modes, pick the right one

RA-PRNG's production core (`ra_core`) ships two entry points. They share
the same underlying mechanism but are tuned — and validated — for
different regimes:

- **`ra_core_singleblock`** — optimized for short, fixed-size batches
  (≤255 words) drawn from a single key with no reseed. This is where the
  8,351 MB/s number lives, and it's validated clean at **K ≥ 96**
  (best at K=255). Below that threshold, use `ra_core_orbit` instead.
- **`ra_core_orbit`** — the general-purpose, continuous-stream mode.
  Validated clean at *any* batch size, including K=1, all the way to 1 TB
  of PractRand output. Slower in absolute throughput than
  `singleblock`'s best case, but it's the mode to reach for whenever you
  need small or frequent batches, or you're not sure which one to pick.

Both modes support **Orbit Addressing**: pass a `key` (and, for the full
64-bit address space, an `addr_hi` value) to jump directly to an
independent output stream — validated for a single lookup per key, or for
chaining across *keys* (incrementing the key with a Weyl constant,
`addr_hi` fixed). Chaining through sequential `addr_hi` values for a
*single* key is a pattern we tested and it is **not** currently validated
safe — see Limitations.

---

## 6. Use cases

- **Shuffling & sampling for ML pipelines** — dataset shuffling, data
  augmentation order, minibatch sampling, where the shuffle-as-generation
  design removes a separate RNG call per swap.
- **Parallel / distributed preprocessing** — Orbit Addressing lets each
  worker derive its own independent stream from a shared key without
  coordinating a reseed.
- **Reproducible simulations** — deterministic, addressable streams make
  it easy to reproduce exactly which random sequence a given run used.
- **Memory-constrained environments** — a ~2KB, L1-resident state is a
  genuine fit for embedded or cache-sensitive contexts.

RA-PRNG is **not** a cryptographic RNG — see Limitations before using it
for anything security-sensitive.

---

## 7. Research credibility

RA-PRNG grew out of the paper **"ra_prng2 and Beyond: An Array-Based PRNG
Architecture for Efficient Random Generation"** by **Hamas A. Rahman**
(State Polytechnic of Malang, Indonesia), accepted by reviewers at
**CSAI 2025 (Beijing)**. The manuscript has been accepted but not yet
presented or formally published due to funding constraints — we describe
it as an *accepted manuscript*, not a peer-reviewed publication, until
that changes.

Since the paper, the core generator has been re-implemented, unified, and
re-validated as `ra_core.c` (the "Orbit Addressing" work), with its own
independent PractRand/Dieharder runs documented in this repository (see
sources cited throughout this page) — this page's numbers come from that
follow-on validation work, not just the original paper.

---

## 8. Honest Limitations

We'd rather you find out here than in production:

- **Not for cryptographic use.** RA-PRNG has not undergone cryptanalysis
  and makes no security claims. Don't use it to generate keys, tokens, or
  anything where an attacker predicting the output matters.
- **The period is a heuristic estimate, not a proof.** The paper estimates
  an extremely large period (~2^4969.65) from the state space, but this is
  not a formally proven lower bound.
- **All throughput numbers are single-core CPU.** There is currently no
  GPU or SIMD implementation. If your workload is the kind that lives on a
  GPU with massively parallel independent streams (e.g. `jax.random.split`
  -style usage), Philox-family generators remain the more proven choice
  today.
- **`ra_core_singleblock`'s headline speed applies at K ≥ 96** (ideally
  K=255) — batches of that size or larger, from a single key, no reseed.
  At small K it is both slower and not statistically validated; use
  `ra_core_orbit` for small or frequent batches instead.
- **`ra_prng3` ("ZepFold", 64-bit) is experimental and explicitly not
  recommended for use** — it exists in the repository as a research
  variant, not a production one.
- **Cross-stream independence at massive parallel scale is not yet
  validated to the depth Philox has been.** Validation so far covers
  single-stream output quality and specific addressing patterns (single
  `addr_hi` lookups, key-increment chaining); sequential `addr_hi`
  chaining for one key was tested and found unsafe (fails PractRand
  immediately) — don't use that specific pattern.

---

## 9. Roadmap

Active research directions — not yet promoted, not yet claimed above:

- Reducing internal state size further (state-size / speed Pareto
  exploration) to close the gap between the `orbit` and `singleblock`
  throughput profiles.
- Larger-scale parallelization studies (multi-terabyte PractRand runs
  beyond the 1TB milestone already reached).
- SIMD / GPU exploration.

---

## 10. Footer

- Paper: "ra_prng2 and Beyond: An Array-Based PRNG Architecture for
  Efficient Random Generation" — Hamas A. Rahman, State Polytechnic of
  Malang — CSAI 2025 (manuscript accepted).
- Source: *[GitHub link — TODO]*
- Package: *[PyPI link — TODO, once published]*

---

## Reviewer checklist (remove before publishing)

- [ ] Confirm NIST STS / TestU01 BigCrush claims directly against the
      paper text (currently attributed to "per paper evaluation" — not
      independently re-verified against repo test artifacts this session).
- [ ] Fill in GitHub/PyPI links once they exist.
- [ ] Decide whether the comparison table in §3 needs a citation column
      for the classic-PRNG state-size figures.
- [ ] Confirm "Hamas A. Rahman" spelling/affiliation wording with the
      author before publishing.
