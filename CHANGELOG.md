# Changelog

All notable changes to this project will be documented in this file.

---

## [0.1.0] — 2025

### Summary

First working implementation of the ra_prng2 algorithm as described in
*"Array-Native Randomness: An Efficient Unified Permutation-Generation
Architecture with Deterministic and High Sensitivity Properties"* (ICCS 2025).

This release is a direct, unoptimized translation of the algorithm from the
paper. The goal is correctness and traceability to the formal specification,
not performance. No structural separation of concerns, no API stability
guarantees, no tuning.

---

### Added — `src/c/ra_prng2.c`

Reference implementation of the ra_prng2 algorithm in C.

**State and initialization**
- Two 256-element `uint32_t` arrays `L[256]` and `M[256]`, aligned to 64 bytes
- `L` initialized with `i * 0x9e3779b7 + 0x9e3779b7` (golden ratio multiplier)
- `M` initialized with `i * 0x06a0dd9b + 0x06a0dd9b`
- Scalar `cons` initialized from caller-supplied seed (range 0 to 2^32 − 1)

**Core loop — `ra_core(seed, rng)`**
- Outer loop runs `iteration = rng / 255 + 1` times
  (implementation-defined approximation to manage total output count;
  not present in the paper's formal specification)
- Inner loop descends from `i = 255` down to `i = 1` (255 steps per outer
  iteration), matching the paper's Algorithm 2
- Per inner step: entropy accumulator `o` computed from 8 consecutive
  elements of `M` starting at `(i + e) mod 256`, each left-shifted by `e`
- Four-variable transform per step: `a`, `b`, `o`, `c` updated via
  `rot32`, XOR, shift, and add, matching the paper's equations exactly
- Output value `c` produced at each inner step (32-bit)
- Swap index `d` derived from `c` via Lemire's fast reduction:
  `d = (uint64_t)c * (i + 1) >> 32`
- In-place swap `L[i] ↔ L[d]` implements the internal permutation

**Reseeding — `ra_hash(M, out8)`**
- After inner loop: `M[i] ^= L[i]` for all 256 elements (entropy diffusion)
- `ra_hash` reduces `M[256]` to `out8[8]`:
  - For each of 8 output slots: XOR with `M[N[i] & 0xFF]` (nonlinear index),
    then XOR with 32 strided elements `M[j*8 + i]` for j in 0..31
  - Reads `M` without modifying it (correct behavior)
- New `cons` built by XORing `out8[e] << e` for e in 0..7

**Count management**
- `count` variable decrements each inner step; breaks early and returns
  current `cons` when `count <= 1`
- This is an implementation detail for controlling total output volume,
  not part of the paper's algorithm definition

**Entry point**
- `main()` runs `ra_core(seed=1, rng=200000)`, prints elapsed time
  and final `cons` value — used for basic timing verification

---

### Added — `src/python/ra_prng2.py`

Pure-Python reference implementation of ra_prng2. Intended for
readability and algorithm tracing, not performance.

**Matches `src/c/ra_prng2.c` in:**
- Array initialization constants (`0x9e3779b7`, `0x06a0dd9b`)
- Inner loop structure (255 down to 1), transform sequence, Lemire reduction
- Post-loop M mutation and reseeding via `ra_hash`

**Known discrepancy — operator precedence in `c` computation:**
- Line: `c = rot32((o + c << 14) ^ (b >> 13) ^ a, b)`
- Python evaluates this as `o + (c << 14)` due to precedence rules,
  which matches the paper's formula `o + (c ≪ 14)`
- The C version has the same expression; C's 32-bit unsigned overflow
  produces a numerically compatible result but the implementations
  are not guaranteed to produce identical bit sequences
- Will be made explicit with parentheses in a future release

**Additional note:**
- Contains `print()` statements throughout `ra_core` for step-by-step
  tracing — intended as a development/debug aid, not production output

---

### Added — `bindings/ra_prngmodule.c`

CPython C extension exposing `ra_prng2` core logic as a Python module
(`import ra_prng`).

**Internal state**
- `RA_PRNG` struct encapsulates `L[256]`, `M[256]`, `tmp8[8]`, `cons`,
  `outputs_generated` (outer iteration counter), and `remaining_count`
  (optional output-count limiter, 0 = unlimited)
- Arrays aligned to 64 bytes

**`ra_prng_init(seed, remaining_count=0)`**
- Initializes struct with same array constants as `src/c/ra_prng2.c`
- `remaining_count=0` runs indefinitely; any positive value limits total
  inner-loop iterations across all calls

**`ra_prng_next()`** → `int`
- Executes one full outer iteration (inner loop 255→1, M mutation, reseeding)
- Returns new `cons` after reseeding
- Increments `outputs_generated`
- If `remaining_count` reaches 1 mid-iteration, returns current `cons`
  immediately without completing the outer step

**`ra_prng_advance(iterations)`** → `int`
- Calls `ra_prng_next()` `iterations` times
- Returns last `cons` produced

**Python-facing API**
```python
rng = ra_prng.RA_PRNG(seed=1)   # initialize
rng.next()                       # one outer iteration → returns cons (uint32)
rng.advance(n)                   # n outer iterations → returns last cons
rng.cons                         # read current scalar state (property)
rng.outputs_generated            # read outer iteration count (property)
```

**Implementation note — safety comment vs code:**
- Source contains the comment "Small safety fix applied: ensure 'd' used as
  index is constrained to 0..(STATE_SIZE-1)" but no explicit bounds clamp
  is present in the code
- Lemire's reduction `(uint64_t)c * (i+1) >> 32` with `i` in range 1..255
  produces `d` in range 0..254, which is within bounds by construction
- The comment is misleading; bounds safety comes from Lemire's arithmetic
  properties, not from an added guard

**Build**
```
python setup.py build_ext --inplace
```

---

### Not yet implemented

- Shuffling of external data arrays (the `data[idx] ↔ data[count]` swap
  from Algorithm 3 in the paper)
- Range sampling without modulo bias
- State inspection / pretty-print
- pip-installable package
- ra_prng3 (64-bit extension)
- Any formal test harness connecting to NIST STS / PractRand output

---

### Known limitations in this release
The hash function in every code are inefficient, it can be merged into one clear function, resulting same output while reducing operations and variables.
| Location | Issue |
|---|---|
| `src/python/ra_prng2.py` | `print()` calls inside `ra_core` — not suitable for benchmarking |
| `src/c/ra_prng2.c` | `iteration = rng / 255 + 1` approximation not in paper spec |
| `bindings/ra_prngmodule.c` | Safety comment does not correspond to explicit guard in code |