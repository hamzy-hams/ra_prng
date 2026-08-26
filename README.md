# ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation

`ra_prng` is a new architecture for deterministic random number generation relying on techniques such as array index shuffling, bitwise rotation, bit shifting, XORing, and nonlinear indexing. This algorithm is explicitly designed for high efficiency and large entropy, with an internal array-based structure that enables long periods without sacrificing performance and output quality.

Unlike traditional PRNGs which separate scrambling and entropy, `ra_prng` makes the array-shuffling process the core of its design. This makes it especially suitable for applications like data shuffling in AI/ML model training, neural network weight initialization, deterministic simulations, statistical experiments, or can be used for general purpose.

> **Note:** Some parts of the code are still in Indonesian and have not yet been translated to English.
> This PRNG is **not designed or intended for cryptographic use!!!**.

## Project Components

This repository consists of several main components:

* `ra_prng2/` — The 32-bit version central to the main paper, stable and deterministic.  
* `ra_prng3/` — Experimental 64-bit version with a larger internal state, higher throughput, and a theoretical period up to $2^{32832}$. (experimental version, do not use)
* `comparisons/` — Benchmark data and comparative evaluations against algorithms like xoshiro256\*\*, PCG32, Philox4x32, ChaCha20, and `/dev/urandom`.

## Repository Layout

See `docs/STRUCTURE.md` for what each top-level directory is for, and how
`ra_prng2`, `ra_prng3`, and the CLI variants under
`benchmarks/comparisons/source/` relate to each other.

## Compilation and Usage

```bash
make build-c      # -> build/bin/{ra_prng2, ra_prng2_thread, ra_prng3}
make build-ext     # -> build/lib/libra_prng2.so (used by bindings/cpython/_ra_prng2.py)
```

Or manually:

```bash
# Standard version
gcc -O3 -march=native -std=gnu17 -include stdalign.h src/ra_prng2/c/ra_prng2.c -o ra_prng2

# OpenMP-enabled version
gcc -O3 -march=native -std=gnu17 -include stdalign.h -fopenmp src/ra_prng2/c/ra_prng2_thread.c -o ra_prng2_thread
```
The default output is either random numbers or the result of the folding process, depending on the implementation.

See `CONTRIBUTING.md` for the full build/test workflow.

## Benchmarking and Evaluation
All benchmark results are available in the benchmark/ folder within each algorithm’s subdirectory. For fair comparison, tests were run in the same environment using:
* CPU: Intel Core i3-1115G4
* OS: Arch Linux 6.8.7
* Compiler: GCC 13.2.0
Summary highlights:
* ra_prng2 outperforms ChaCha20 and `/dev/urandom` in throughput
* ra_prng3 even surpasses Philox4x32 in RNG speed (experimental)
* Full evaluation in the comparisons directory
## Statistical Test Results
* Passed all Dieharder and NIST STS tests
* No fatal failures in any subtests
* A few WEAK flags appeared, which is normal and also seen in other popular algorithms (Xoshiro, PCG, ChaCha20)
* It passed all 160 tests in BigCrush (TestU01 v1.2.3) with no failures, indicating robust statistical behavior across collision tests, autocorrelation, Hamming weight dependencies, spectral analysis, and other domains.
* Additionally, it passed PractRand tests up to 128 GB of output with no failures reported, further validating the uniformity and unpredictability of its output at scale.
## Roadmap
* Further efficiency and speed improvements
* Explicit SIMD optimizations and cross-platform distribution
* A second paper to be published for ra_prng3

## License
This project is licensed under the CC BY-NC-SA 4.0 — https://creativecommons.org/licenses/by-nc-sa/4.0

## Citation
I'm preparing the official citation format. In the meantime, please link to this GitHub repository and include the project name and primary author.

## Contribution
External contributions are not officially open yet. However, technical feedback and discussion are very welcome via the discussion forum.

## Author
This project was created and maintained by **Hamas A. Rahman** (online / GitHub handle: **[@hamzy-hams](https://github.com/hamzy-hams)**), an independent researcher in random number generation, computer science, and Collatz Conjecture problem.
- **Author**: Hamas A. Rahman
- **GitHub**: [github.com/hamzy-hams](https://github.com/hamzy-hams)
- **Discord**: `hamzyhams`
- **Contact Email**: nexthamas95@gmail.com

