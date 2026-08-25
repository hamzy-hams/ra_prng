# ra_prng

**ra_prng** is a deterministic array-native PRNG that unifies random number
generation and data shuffling into a single internal process. Unlike
conventional PRNGs that generate values first and shuffle separately,
ra_prng treats permutation as the generative mechanism itself — making
shuffling a structural property of the generator, not an external application.

The design is documented in a peer-reviewed paper presented at ICCS 2025:
*"Array-Native Randomness: An Efficient Unified Permutation-Generation
Architecture with Deterministic and High Sensitivity Properties"*

---

## Why ra_prng

Most PRNGs are scalar systems: they produce a value, then that value drives
an external transformation. This separation is a foundational assumption of
conventional PRNG design — the generator is unaware of the data being shuffled,
and the shuffler is unaware of the generator's internal state.

ra_prng inverts this. Its internal state (two 256-element arrays) evolves
*through* continuous in-place permutation. Randomness is an emergent property
of the permutation process, not an input to it.

Three properties follow from this design that are not available in combination
anywhere else:

**Unified generation and shuffling**
A single execution loop handles both random output and data permutation.
There is no call boundary between generator and shuffler, and no separate
Fisher–Yates overhead.

**Non-invertible state evolution**
Each reseeding step compresses the full array state into a 32-bit scalar
through a hash-like reduction. Multiple internal states can produce the same
successor — the mapping is many-to-one. This means: given the output stream,
recovering the seed is not feasible. Mersenne Twister, by contrast, is fully
invertible from 624 consecutive outputs.

**Content-sensitive fingerprinting**
Because the internal state evolves based on the data being shuffled, two
datasets shuffled with the same seed will produce divergent transformation
trajectories if the datasets differ. This is not possible with any
scalar PRNG: their output depends only on seed and sequence position,
never on the data being processed.

---

## Statistical quality

Tested on 1 GiB of output:

| Test suite | Result |
|---|---|
| NIST STS (188 subtests) | 99.4% pass, mean p-value 0.503 ± 0.287 |
| BigCrush (160 tests) | 100% pass |
| PractRand | No failure up to 128 GB |
| Dieharder (114 subtests) | 112 pass, 2 weak, 0 fail |
| Entropy (ent) | 7.99999 bits/byte |
| Avalanche sensitivity | 49.9% bit-flip rate |

Theoretical period: **2^4969.65** (ra_prng2), extended to **2^16447.65** in ra_prng3.
State size: **2 KB** — fits within typical L1 cache.

> ra_prng is a general-purpose PRNG. It has not been evaluated against
> cryptographic attack models and should not be used as a cryptographic primitive.

---

## Shuffling benchmark

1,000,000 items, Fisher–Yates, 30 runs each, measured with `perf stat -r 30`.
All algorithms in per-call (no-buffer) configuration for a fair comparison.

| Algorithm | Time (s) | Deviation | IPC |
|---|---:|---:|---:|
| PCG32 | 0.0599 | 1.60% | 1.59 |
| xoshiro256 | 0.0610 | 2.81% | 1.60 |
| Philox4x32 | 0.0662 | 3.75% | 1.43 |
| **ra_prng2** | **0.1216** | **1.36%** | **3.46** |
| ChaCha20 | 0.1965 | 2.09% | 1.95 |

ra_prng2 has the **lowest run-to-run deviation** of all evaluated algorithms
(1.36%) and the **highest IPC** (3.46), reflecting its branchless design and
L1-resident state. The ~2× slower wall-clock time compared to PCG32 is
attributable to denser per-element computation; the gap narrows significantly
relative to raw throughput benchmarks because shuffling performance is
bounded by memory access, not generation speed alone.

Note: ChaCha20 drops to 0.064s with a 16-word buffer — a 3× improvement.
ra_prng excludes buffering by design: its state mutation and output extraction
share the same loop, making separate buffering structurally redundant.

---

## How it works

ra_prng2 maintains two internal arrays:

- **L[256]** — permutation array, shuffled in-place each iteration
- **M[256]** — entropy reservoir, mixed with L after each full sweep

Each outer iteration runs an inner loop from index 255 down to 0. At each
inner step, four intermediate values evolve through bitwise rotation, XOR,
and shift operations drawing from both arrays and the current scalar state.
The output value `c` is produced at each inner step, then used immediately
to compute a swap index via Lemire's fast reduction — permuting L[i] with
L[d] in the same pass.

After the inner loop completes, M is updated by XORing it element-wise with
the permuted L. A hash-like reduction over M produces the new scalar constant
for the next outer iteration. This reseeding step is the non-invertible
component: it compresses 256 × 32 bits of state into 32 bits.

For the full formal specification see the paper, or the source in `src/`.

---

## Quick start

```python
import ra_prng

rng = ra_prng.RA_PRNG(seed=42)

# Generate a 32-bit value
value = rng.next()

# Check current step position
pos = rng.advance()
```

A pip package is not yet available. Build from source using the CPython
binding in `bindings/cpython/`.

---

## Repository layout
ra_prng/
├── src/                  C implementation of ra_prng2 and ra_prng3
├── bindings/
│   └── cpython/          Python extension module
├── benchmarks/           Shuffle timing, avalanche analysis, PRNG comparison
└── tests/

---

## Planned

- Deterministic shuffle with verifiable seed certificate
- Range sampling without modulo bias
- State inspection / debug output
- ra_prng3 stable release (64-bit, period 2^16447)
- Drop-in `random.shuffle()` replacement with audit log

---

## Reference

Rahman, *"Array-Native Randomness: An Efficient Unified Permutation-Generation
Architecture with Deterministic and High Sensitivity Properties"*,
ICCS 2025. Source and benchmarks: this repository.

---

## License

This repository uses two separate licenses:

- **Research papers & written materials** (`research/`): Creative Commons
  Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0).
  See `LICENSE`.
- **Source code** (`src/`, `bindings/`, `tests/`, `benchmarks/`,
  `sub-projects/`, `experiments/`): Proprietary, all rights reserved.
  See `LICENSE-CODE`.