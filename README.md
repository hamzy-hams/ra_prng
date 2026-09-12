# ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation

`ra_prng` is a deterministic, array-based PRNG architecture that makes array-index
permutation the *source* of randomness rather than something built on top of it.
Two internal 256-entry arrays continuously permute and mutate each other; entropy
generation and index shuffling happen inside the same in-place loop. That keeps
internal state small (~2 KB, L1-cache friendly) while still passing large-scale
statistical batteries, which makes it well suited to data shuffling in AI/ML
training pipelines, neural-network weight initialization, deterministic
simulations, statistical experiments, or general-purpose use.

> **Note:** Some code comments/identifiers are still in Indonesian and have not
> yet been translated to English.
> This PRNG is **not designed or intended for cryptographic use!!!**

## What's in this repository

| Path | What it is | Status |
|---|---|---|
| `src/ra_prng2/c/`, `src/ra_prng2/python/` | The 32-bit generator described in the CSAI2025 paper — canonical (`ra_prng2.c`), OpenMP-parallel (`ra_prng2_thread.c`), struct-refactored (`ra_prng2_struct.c`, backs the Python binding), and a pure-Python reference port (`ra_prng2.py`) | Stable |
| `src/ra_prng3/c/` | 64-bit experimental successor (`ZepFold` core), larger internal state, theoretical period up to 2^32832 | Experimental — **do not use** |
| `src/ra_core/c/` | The addressable/orbit-addressing successor (`ra_core_orbit`, `ra_core_singleblock`) — see the `ra_core` section below | Stable; pip binding in `pip-ra-core/` (see below) |
| `bindings/cpython/` | Real, installable `pip install .` package — a `ctypes` wrapper calling straight into `ra_prng2_struct.c`, no logic reimplemented in Python | Stable |
| `benchmarks/comparisons/source/` | A *derived* file-shuffling CLI tool (`--input`/`--output`/`--seed`/...) built on the same mixing ideas but with its own permutation/index math — not the paper algorithm itself | Separate tool |
| `benchmarks/results/`, `benchmarks/comparisons/` | Precomputed statistical-test output and throughput/entropy comparisons vs xoshiro256\*\*, PCG32, Philox4x32, ChaCha20, `/dev/urandom` | Reference data |
| `experiments/` | Exploratory research (new addressing/initialization schemes, cycle-operation search, cross-family benchmarking, etc.), one dated subfolder per investigation | Active research, not part of the stable API |
| `sub-projects/` | Downstream applications (`image_encrypting`, `fingerprinting_tools`) | Placeholders, not yet implemented |
| `research/` | Conference paper manuscripts — CSAI2025 (accepted by reviewers for oral presentation & publication, but never actually presented or published due to lack of funding), with its acceptance letter; plus a companion ICCS2026 manuscript (prepared/submitted, also not published) | See Citation |

See `docs/STRUCTURE.md` for the full directory map and exactly how `ra_prng2`,
`ra_prng3`, and the CLI shuffle variants under `benchmarks/comparisons/source/`
relate to each other.

## Build & install

```bash
make build-c       # -> build/bin/{ra_prng2, ra_prng2_thread, ra_prng3}
make build-ext      # -> build/lib/libra_prng2.so (used by bindings/cpython/_ra_prng2.py)
make install         # editable install of the pure-Python module (pip install -e .)
make test              # run tests/ (currently empty scaffolding)
make clean              # remove build/
```

Or manually:

```bash
# Standard version
gcc -O3 -march=native -std=gnu17 -include stdalign.h src/ra_prng2/c/ra_prng2.c -o ra_prng2

# OpenMP-enabled version
gcc -O3 -march=native -std=gnu17 -include stdalign.h -fopenmp src/ra_prng2/c/ra_prng2_thread.c -o ra_prng2_thread
```

The default output is either the raw random stream or the result of the
folding process, depending on the binary.

### Python

```bash
pip install .   # compiles the C engine with gcc at install time and bundles
                 # libra_prng2.so into the installed package automatically
```

Because compilation uses `-march=native`, install **on the machine it will
run on** — do not copy a wheel built on one machine to another with a
different CPU.

```python
from bindings.cpython._ra_prng2 import RaPrng2

r = RaPrng2(seed=42)
r.next()        # one output
r.advance(10)   # advance 10 outputs, return the last
```

See `CONTRIBUTING.md` for the full build/test workflow and coding conventions.

## Statistical validation

Results below are for `ra_prng2` (`benchmarks/results/ra_prng2_original/`):

* **Dieharder**: 113 PASSED, 2 WEAK, 0 FAILED — WEAK flags are expected and
  also appear in Xoshiro, PCG, and ChaCha20
* **NIST STS**: full battery passed, all sub-tests within the expected
  pass-rate range
* **TestU01 BigCrush** (v1.2.3): all 160 statistics passed, no failures
* **PractRand**: no anomalies through 128 GB of output

## Performance

Benchmarked on Intel Core i3-1115G4 / Arch Linux 6.8.7 / GCC 13.2.0
(`benchmarks/comparisons/`):

* `ra_prng2` outperforms ChaCha20 and `/dev/urandom` in raw throughput
* `ra_prng3` (experimental) even surpasses Philox4x32 in RNG speed

A newer, independently cross-checked 12-candidate benchmark
(`experiments/2026-8-31_prng-family-benchmark/`, validated against KAT
vectors and byte-for-byte against the real binaries) measures **seed +
generate together** instead of steady-state throughput alone. That exposes
`ra_prng`'s actual niche: at frequent-reinit workloads it lands far ahead of
MT19937, whose fixed seeding cost (~1257 ns) dominates any workload that
reseeds often — exactly the pattern the addressable-initialization research
below targets.

> **Note:** an even earlier throughput claim for this line of work (30303
> MB/s) turned out to be non-reproducible, even replaying the exact old
> formula on this machine (~7019 MB/s, 4.3x off) — most likely an
> extrapolation artifact in that old benchmark's methodology, not a real
> regression. Treat any number from before 2026-09 with that in mind; the
> table below is the current, re-verified baseline.

### `ra_core` (now in `src/ra_core/c/`, C only)

`src/ra_core/c/ra_core.c` is the unified, addressable-init successor
(`ra_core_orbit` for unbounded continuous streams, `ra_core_singleblock` for
single-key batches of up to 255 words with no reseed), promoted from
`experiments/2026-9-1_family-productionization/ra_core.c` (2026-09-06). It
has passed its full statistical battery (dieharder 0 FAILED, PractRand
clean, avalanche/collision-scan PASS, both cores, full valid K range) and
`make build-c` / `./build/bin/ra_core validate` are part of the standard
build now. The `experiments/2026-9-1_family-productionization/` copy
remains the active research working copy — future formula changes land
there first and get synced to `src/ra_core/c/` on each promotion, not
automatically.

**Pip binding**: `pip-ra-core/` (`pip install ./pip-ra-core` from a checkout
of this repo, requires `gcc`) wraps `ra_core_orbit`/`ra_core_singleblock`
with a Python API — random-access `generate(key, addr_hi, n)`, a stateful
`RaCoreEngine` (continue/fork/save/resume, position introspection),
`spawn_streams()`, a deterministic `shuffle()`, and a compiled (Cython)
`numpy.random.BitGenerator` integration. `ra_core.c` itself is not modified
by this binding — see `pip-ra-core/README.md` for the full API and
`src/ra_core/c/ra_core_engine.c` for how it's built on top without touching
the validated formula.

`ra_reseed`/`ra_hash` (the state-mixing step `ra_core_orbit` runs every 255
words for streams longer than one block) had an open question about
whether they leak detectable structure into the first post-reseed cycle —
closed 2026-09-06 (`experiments/2026-9-4_orbit-reseed-postboundary-bcfn/RESULTS.md`):
PractRand clean to 8GB for both K=1 and K=255, both Weyl-stepped and
SplitMix32-hashed key orderings (the ordering that originally exposed the
now-fixed `addr_hi` birthday-paradox gap), with only isolated single-checkpoint
"unusual" blips that resolved on the next checkpoint rather than escalating.

**Addressing capacity (2026-09-06)**: both entry points now take a second
32-bit input, `addr_hi`, alongside `key` — `ra_core_orbit(key, addr_hi, rng,
stream)` / `ra_core_singleblock(key, addr_hi, rng, stream)` — giving a
combined 64-bit effective address space instead of 32-bit. This closes a
statistically real defect found in `experiments/2026-9-5_orbit-k1-exhaustive-function-analysis/`:
with only a 32-bit `key`, aggressive-reinit workloads that can *repeat* a
key (hashed-chain keys, `/dev/urandom`-drawn keys — as opposed to a
repeat-free Weyl sequence) hit birthday-paradox collisions early enough to
fail PractRand at K=1. `addr_hi` is hashed independently from `key`
(`experiments/2026-9-5_orbit-address-widen-64bit/candidates/candidate_d.c`,
Candidate D, chosen over 4 other candidates after a full battery — see that
folder's `RESULTS.md`) and deliberately excluded from the rotation amount,
which is what let it pass PractRand where two earlier candidates (which did
fold it into rotation) failed Gap-16. Existing callers are unaffected:
`--stream <mode> <seed> <n>` (4 args) still works with `addr_hi` implied 0;
the new 5-arg form `--stream <mode> <seed> <addr_hi> <n>` sets it
explicitly. `SINGLEBLOCK_KAT_CHECKSUMS`/`ORBIT_KAT_CHECKSUMS` were
regenerated against this formula (`addr_hi=0` baseline); full promotion
details, byte-identical merge verification, and refreshed benchmarks are in
`experiments/2026-9-6_orbit-address-widen-64bit-promotion/`.

> **Update 2026-09-12**: the table below was revised after discovering
> `bench_release.c` had a benchmark **fairness bug**, not a `ra_core.c`
> formula change. Philox4x32-10/Xoshiro256\*\* were plain `static` functions
> in the same translation unit as the benchmark (fully inlined by the
> compiler); `ra_core_orbit`/`ra_core_singleblock` were `extern`-linked
> against a separately compiled object, and — after switching that to
> `#include` too — turned out to *still* sit just over GCC's default
> per-function auto-inline size threshold, so the compiler kept emitting
> real (uninlined) `CALL`s for `ra_core_singleblock`'s two hot-loop helpers
> while fully inlining the reference baselines. Fixed by adding
> `static inline __attribute__((always_inline))` to those two helpers in
> `ra_core.c` (their behavior is unchanged — verified byte-identical
> against the KAT self-test — only their codegen is). This mainly affects
> `singleblock` K=255 (its steady-state loop is cheap enough that the lost
> inlining was a large relative overhead); `orbit`/Philox/Xoshiro were
> effectively untouched by the fix, and moved by their normal run-to-run
> amount between measurement sessions. Full root-cause writeup:
> `experiments/2026-9-11_state-size-pareto-frontier/RESULTS.md` §9.

Measured 2026-09-12 with `experiments/2026-9-1_family-productionization/bench_release.c`
(same machine as above, pinned to one core with `taskset -c 0`,
`-O3 -march=native`, min-of-5..7-trials internally, plus best-of-8 outer
runs to average out session-level noise). Three numbers per mode, following
this repo's standing benchmark convention (always report `singleblock`
K=1/K=255 *and* `orbit`, never K=1 alone — a K=1-only table has been
misleading before):

| Mode | Init time (ns) | Generate/word, steady-state (ns/word) | Real throughput (ns/word) | Real throughput (MB/s) |
|---|---:|---:|---:|---:|
| `ra_core_singleblock` K=1 | 0.29 | 12.12 | 12.41 | 307.3 |
| `ra_core_singleblock` K=255 | 49.96 | ~0.00 | 0.191 | 20018.1 |
| `ra_core_orbit` --multistream K=1 | 75.51 | 9.75 | 85.26 | 44.7 |
| `ra_core_orbit` --multistream K=255 | 75.51 | 3.64 | 3.93 | 969.8 |
| `ra_core_orbit` --stream (N=20M) | 75.51 | 3.87 | 3.87 | 986.6 |
| Philox4x32-10 K=1 | 1.87 | 12.20 | 14.08 | 271.0 |
| Philox4x32-10 K=255 | 1.87 | 3.78 | 3.79 | 1007.9 |
| Philox4x32-10 stream | 1.87 | 3.89 | 3.89 | 981.3 |
| Xoshiro256\*\* K=1 | 1.92 | 5.09 | 7.00 | 544.8 |
| Xoshiro256\*\* K=255 | 1.92 | 1.29 | 1.30 | 2937.2 |
| Xoshiro256\*\* stream | 1.92 | 1.20 | 1.20 | 3171.8 |

**Real throughput** is the directly measured wall-clock number for that
usage pattern (reinit-every-call for `K=1`/`K=255`, one long call for
`--stream`) — the one to trust. **Init time** isolates just the
state-setup cost (`ra_init_state_orbit`/`ra_init_state_singleblock`,
verbatim-copied in `bench_release.c` since they're internal `static`
functions); **generate/word** is derived from the other two
(`(real_total_time − init_time) / K`), computed separately per K because
`singleblock`'s init is no longer constant-cost (variable-length, so K=1's
init only touches ~8 array entries instead of the full 256). For `orbit`,
init cost is K-independent (always a full 256+256-entry `L[]`/`M[]` init),
which is why it dominates at K=1 — `orbit` is designed for long continuous
streams, not frequent small reinits; use `singleblock` for that pattern
instead, within its 255-word-per-key limit.

**Cost of the `addr_hi` widening (honest delta, not noise)**: compared to
the pre-promotion 2026-09-04 numbers, `ra_core` rows regressed 5.6-9.9% at
K=1 (`singleblock` 371.6→335.4 MB/s, `orbit` multistream 66.7→65.4 MB/s —
the orbit figure looks small only because K=1 orbit was already
init-dominated) and ≤1.6% at K=255/stream, while the untouched Philox/
Xoshiro reference rows moved ≤1.3% (within normal run-to-run noise) over
the same remeasurement. That gap is real, not measurement noise: the extra
`fmix32` call and multiply per `L[]`/`M[]` term for `addr_hi` is fixed
per-init overhead, so it costs the most exactly where init dominates (K=1)
and amortizes away where it doesn't (K=255, long streams). See
`experiments/2026-9-6_orbit-address-widen-64bit-promotion/RESULTS.md` for
the full before/after table.

Reading the comparison: `singleblock` K=255 is **~19.9x faster than Philox
and ~6.8x faster than Xoshiro256\*\*** — its actual regime (batches up to
255 words per key/address, no reseed). This is the row the 2026-09-12
fairness fix above changed the most: before the fix, the same comparison
read 6.0x/2.25x — `ra_core` was never actually that close to the
baselines, the old numbers were measuring `ra_core` with its hot loop
artificially un-inlined while Philox/Xoshiro ran fully inlined. At K=1,
`singleblock` is **1.13x faster than Philox** and 1.77x slower than
Xoshiro256\*\* there (K=1 is init-dominated, so it barely touches the loop
the fix targeted — these ratios are within normal run-to-run noise of the
pre-fix numbers, not a real change). `orbit`, whose init isn't
variable-length, stays well behind both references at K=1 and is slightly
behind Philox (~0.96x) at K=255/stream, while still losing to
Xoshiro256\*\* by ~3.2x in that regime — `orbit`'s own functions weren't
touched by the fix (confirmed: adding the same `always_inline` treatment to
them empirically changes nothing, `orbit`'s per-word cost is dominated by
a data-dependent `L[]` swap the loop already does, not by call overhead),
so this row's small movement from the previous ~2.8x is ordinary
session-to-session measurement noise.

**Scope of this claim**: the speed numbers above are single-core CPU
throughput only. This repo does not yet have a GPU/SIMD implementation of
`ra_core` (Philox's usual home turf for the same addressable/counter-based
usage pattern, e.g. `jax.random.split`-style workloads), and the
statistical validation so far covers single-key output quality, not
cross-stream independence at scale for the addressable/multikey pattern
specifically. Treat `ra_core`'s current, verified advantage as raw
single-core speed in its designed K range — not yet a claim of matching
Philox's broader track record.

## Current research (`experiments/`)

Beyond the stable `ra_prng2`/`ra_prng3` line described in the CSAI2025
manuscript (accepted by reviewers, but never presented or published — see
Citation), this repo tracks an active line of research on **addressable /
orbit-style initialization**:
deriving a generator's starting state directly from a `key`, so a caller can
jump to an independent stream without a full reseed — in contrast to
Philox-style per-call point addressing. This work is exploratory, lives
entirely under `experiments/` (one dated subfolder per investigation, each
with its own `HANDOVER.md`), and is **not** part of the stable API described
above. A companion paper describing this direction,
`research/iccs2026_ra_prng/`, has been prepared for ICCS2026.

## Roadmap

* Further efficiency and speed improvements (see `experiments/` for current
  progress)
* Explicit SIMD optimizations and cross-platform distribution
* Formal write-up and productionization of the addressable-initialization /
  orbit-addressing research line

## License

This project is licensed under the CC BY-NC-SA 4.0 —
https://creativecommons.org/licenses/by-nc-sa/4.0

## Citation

I'm preparing the official citation format. In the meantime, please link to
this GitHub repository and include the project name and primary author.

Note: neither paper below has actually been presented or published — both
are manuscripts written for conference submission.

* **CSAI2025** (accepted by reviewers for oral presentation & publication,
  but never presented or published due to lack of funding) — "ra_prng2 and
  Beyond: An Array-Based PRNG Architecture for Efficient Random Generation" —
  `research/csai2025_ra_prng/`
* **ICCS2026** (prepared/submitted, not published) — "Array-Native
  Randomness: An Efficient Unified Permutation-Generation Architecture with
  Deterministic and High Sensitivity Properties" — `research/iccs2026_ra_prng/`

## Contribution

External contributions are not officially open yet. However, technical
feedback and discussion are very welcome via the discussion forum.

## Author

This project was created and maintained by **Hamas A. Rahman** (online /
GitHub handle: **[@hamzy-hams](https://github.com/hamzy-hams)**), an
independent researcher in random number generation, computer science, and
the Collatz Conjecture problem.

- **Author**: Hamas A. Rahman
- **GitHub**: [github.com/hamzy-hams](https://github.com/hamzy-hams)
- **Discord**: `hamzyhams`
- **Contact Email**: nexthamas95@gmail.com
