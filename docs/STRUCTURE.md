# Repository Structure

`ra_prng` is research code for an array-based deterministic PRNG
(`ra_prng2`/`ra_prng3`, published at CSAI2025). This document describes what
each top-level directory is for.

```
.
├── src/                    canonical algorithm implementations
│   ├── ra_prng2/            32-bit version - stable, central to the paper
│   │   ├── c/                three C variants (see below) + CHANGELOG.md
│   │   └── python/            pure-Python reference implementation (ra_prng2.py)
│   └── ra_prng3/            64-bit experimental version ("do not use")
│       ├── c/                single-file C implementation + CHANGELOG.md
│       └── python/            empty - no Python port exists yet
├── bindings/cpython/        Python <-> C binding, a real installable package
│                              (see bindings/cpython/README.md)
├── tests/                   c/ and python/ test suites (currently empty scaffolding)
├── benchmarks/
│   ├── results/              precomputed statistical-test output (Dieharder,
│   │                          NIST STS, TestU01, PractRand) per algorithm version
│   └── comparisons/          speed/entropy comparisons vs xoshiro256**, PCG32,
│       ├── source/            Philox4x32, ChaCha20, /dev/urandom - includes
│       │                      *derived* CLI variants of ra_prng2/ra_prng3, see
│       │                      benchmarks/comparisons/source/README.md
│       ├── RNGing_speed/, entropy/, scrambling_speed/   raw result .txt files
├── experiments/              exploratory work not part of the stable API
│   ├── 2025-10-5_scramble-design/   token-scrambling design experiment
│   └── others/                standalone analysis scripts
├── sub-projects/             downstream applications built on ra_prng
│   ├── image_encrypting/      placeholder - not yet implemented
│   └── fingerprinting_tools/  placeholder - reserved for a future sub-project
├── research/csai2025_ra_prng/  the published paper + acceptance letter (PDFs)
├── docs/                     this file, docs/api/ (module reference)
├── graphify-out/              generated knowledge-graph scan (see README.md)
├── pyproject.toml, Makefile   build - see CONTRIBUTING.md
└── .github/workflows/ci.yml   build + smoke-test on push/PR
```

## `ra_prng2` vs `ra_prng3` vs the `benchmarks/comparisons/source/` variants

These are **not duplicates of each other** - each serves a different purpose:

- `src/ra_prng2/c/ra_prng2.c` - the canonical 32-bit generator: seed in,
  stream of `cons` values out (`ra_core`). This is what the CSAI2025 paper
  describes.
- `src/ra_prng2/c/ra_prng2_thread.c` - the same generator, OpenMP-enabled for
  multi-threaded throughput benchmarking.
- `src/ra_prng2/c/ra_prng2_struct.c` - the same generator refactored into a
  reusable `RA_PRNG` struct with `ra_prng_init`/`ra_prng_next`/`ra_prng_advance`.
  This is the basis for the ctypes binding in `bindings/cpython/`.
- `src/ra_prng3/c/ra_prng3.c` - the 64-bit experimental successor (`ZepFold`
  core, larger state, longer period). Explicitly marked "do not use" in the
  main README - still under active research.
- `benchmarks/comparisons/source/ra_prng2.c` and `ra_prng3.c` - a **different,
  derived CLI tool** (`ZepFold` file-shuffling utility with `--input`/
  `--output`/`--seed`/`--multiplier_m`/`--multiplier_l` flags), built on the
  same core mixing ideas but with different permutation/index math and its
  own parameters. It is kept separate deliberately - see
  `benchmarks/comparisons/source/README.md`.

## Build

See `CONTRIBUTING.md` and the root `Makefile` / `pyproject.toml`.
