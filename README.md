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
| `bindings/cpython/` | Real, installable `pip install .` package — a `ctypes` wrapper calling straight into `ra_prng2_struct.c`, no logic reimplemented in Python | Stable |
| `benchmarks/comparisons/source/` | A *derived* file-shuffling CLI tool (`--input`/`--output`/`--seed`/...) built on the same mixing ideas but with its own permutation/index math — not the paper algorithm itself | Separate tool |
| `benchmarks/results/`, `benchmarks/comparisons/` | Precomputed statistical-test output and throughput/entropy comparisons vs xoshiro256\*\*, PCG32, Philox4x32, ChaCha20, `/dev/urandom` | Reference data |
| `experiments/` | Exploratory research (new addressing/initialization schemes, cycle-operation search, cross-family benchmarking, etc.), one dated subfolder per investigation | Active research, not part of the stable API |
| `sub-projects/` | Downstream applications (`image_encrypting`, `fingerprinting_tools`) | Placeholders, not yet implemented |
| `research/` | The published paper + acceptance letter (CSAI2025), plus a companion paper draft (ICCS2026) | See Citation |

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

### `ra_core` (experimental, not yet in `src/`)

`experiments/2026-9-1_family-productionization/ra_core.c` is the unified,
addressable-init successor under active research (`ra_core_orbit` for
unbounded continuous streams, `ra_core_singleblock` for single-key batches
of up to 255 words with no reseed). It has passed its full statistical
battery (dieharder 0 FAILED, PractRand 16GB+ clean, avalanche/collision-scan
PASS, both cores, full valid K range) but **has not been promoted to `src/`
yet** — no package/API decision has been made for it, so treat it as
research code, not a stable dependency.

Measured 2026-09-04 with `experiments/2026-9-1_family-productionization/bench_release.c`
(same machine as above, pinned to one core with `taskset -c 0`, GCC 16.1.1
`-O3 -march=native`, min-of-5..7-trials) after that session's two bit-exact
speed optimizations (rolling-register wide-`o` computation, and — for
`singleblock` only — variable-length `M[]` initialization). Three numbers
per mode, following this repo's standing benchmark convention (always
report `singleblock` K=1/K=255 *and* `orbit`, never K=1 alone — a K=1-only
table has been misleading before):

| Mode | Init time (ns) | Generate/word, steady-state (ns/word) | Real throughput (ns/word) | Real throughput (MB/s) |
|---|---:|---:|---:|---:|
| `ra_core_singleblock` K=1 | 0.25 | 10.02 | 10.27 | 371.6 |
| `ra_core_singleblock` K=255 | 31.69 | 0.33 | 0.457 | 8351.1 |
| `ra_core_orbit` --multistream K=1 | 48.54 | 8.64 | 57.18 | 66.7 |
| `ra_core_orbit` --multistream K=255 | 48.54 | 2.74 | 2.93 | 1300.0 |
| `ra_core_orbit` --stream (N=20M) | 48.54 | 2.80 | 2.80 | 1362.4 |
| Philox4x32-10 K=1 | 1.87 | 10.40 | 12.27 | 311.0 |
| Philox4x32-10 K=255 | 1.87 | 2.77 | 2.78 | 1374.4 |
| Philox4x32-10 stream | 1.87 | 2.77 | 2.77 | 1378.3 |
| Xoshiro256\*\* K=1 | 1.79 | 4.78 | 6.58 | 580.2 |
| Xoshiro256\*\* K=255 | 1.79 | 1.03 | 1.04 | 3673.5 |
| Xoshiro256\*\* stream | 1.79 | 0.97 | 0.97 | 3934.9 |

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

Reading the comparison: `singleblock` K=255 is **6.1x faster than Philox
and 2.27x faster than Xoshiro256\*\*** — its actual regime (batches up to
255 words per key/address, no reseed). Yesterday's variable-length-init
optimization also flipped the K=1 picture against Philox specifically:
`singleblock` K=1 is now **1.19x faster than Philox** (previously slower),
though still 1.56x slower than Xoshiro256\*\* there. `orbit`, whose init
isn't variable-length, stays well behind both references at K=1 and is now
roughly at parity with Philox (not clearly ahead, unlike an earlier,
less-controlled measurement suggested) at K=255/stream, while still losing
to Xoshiro256\*\* by ~2.8x in that regime.

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

Beyond the stable, paper-published `ra_prng2`/`ra_prng3`, this repo tracks an
active line of research on **addressable / orbit-style initialization**:
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

* **CSAI2025** (accepted, oral presentation) — "ra_prng2 and Beyond: An
  Array-Based PRNG Architecture for Efficient Random Generation" —
  `research/csai2025_ra_prng/`
* **ICCS2026** (prepared/submitted) — "Array-Native Randomness: An Efficient
  Unified Permutation-Generation Architecture with Deterministic and High
  Sensitivity Properties" — `research/iccs2026_ra_prng/`

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
