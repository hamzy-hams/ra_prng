# ra_prng2 and Beyond: A Novel Lightweight PRNG Architecture for High-Performance Random Data Generation

`ra_prng` is a new architecture for deterministic random number generation relying on techniques such as array index shuffling, bitwise rotation, bit shifting, XORing, and nonlinear indexing. This algorithm is explicitly designed for high efficiency and large entropy, with an internal array-based structure that enables long periods without sacrificing performance and output quality.

Unlike traditional PRNGs which separate scrambling and entropy, `ra_prng` makes the array-shuffling process the core of its design. This makes it especially suitable for applications like data shuffling in AI/ML model training, neural network weight initialization, deterministic simulations, statistical experiments, or can be used for general purpose.

> **Note:** Some parts of the code are still in Indonesian and have not yet been translated to English.
> This PRNG is **not designed or intended for cryptographic use!!!**.

## Project Components

This repository consists of several main components:

* `ra_prng2/` — The 32-bit version central to the main paper, stable and deterministic.  
* `ra_prng3/` — Experimental 64-bit version with a larger internal state, higher throughput, and a theoretical period up to $2^{32832}$. (experimental version, do not use)
* `comparisons/` — Benchmark data and comparative evaluations against algorithms like xoshiro256\*\*, PCG32, Philox4x32, ChaCha20, and `/dev/urandom`.

## Compilation and Usage

To compile:

```bash
# Standard version
cd ra_prng2/src
gcc -O3 -march=native ra_prng2.c -o ra_prng2

# OpenMP-enabled version
gcc -O3 -march=native -fopenmp ra_prng_thread.c -o ra_prng2_thread
```
The default output is either random numbers or the result of the folding process, depending on the implementation.

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
This project was created by Hamas A. Rahman, an independent researcher in random number generation, computer science, and Collatz Conjecture problem.
Contact email: nexthamas95@gmail.com
Discord ID: hamzyhams
