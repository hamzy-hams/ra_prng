# ra_prng3 — Experimental High-Performance Random Number Generator

**This project is currently in experimental development stage.**
Several files and folders may not be fully synchronized. If you're looking for the most up-to-date implementation or benchmark, please refer to the **most recently edited source file**.

## Overview

**ra_prng3** is the third-generation member of the `ra_prng` family. It builds upon the principles of its predecessor (`ra_prng2`), with a focus on:

- **Larger internal state**: ~4 KB using `uint64_t` arrays
- **Higher theoretical period**: up to $2^{32832}$
- **Faster throughput**: exceeding Philox4x32 and approaching xoshiro256**
- Determinism and parallelizability
