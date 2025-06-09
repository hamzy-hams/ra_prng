# RA_PRNG: Robust Abstract Pseudorandom Number Generator Series

This repository contains the development and documentation of the RA_PRNG series — a custom-designed family of fast, high-entropy pseudorandom number generators developed by Zephyr (2024–2025). Each version shows a significant evolution in entropy distribution, throughput efficiency, and theoretical periodicity.

## Versions

- **ra_prng_v1**: First prototype. Weak entropy, small period. Used mainly for conceptual foundation.
- **ra_prng_v2**: Major upgrade. High throughput (up to 6GiB/s), passed NIST STS, Dieharder, and ENT tests. Applied in image encryption and key scrambling.
- **ra_prng_v3**: Current state-of-the-art. Achieves up to 2^6000 period growth compared to v2, stronger bias resistance, and passes PractRand up to 32 GiB. Actively undergoing real-world application testing.

## Features

- Portable C implementation with minimal dependencies.
- Custom mixing strategy using bitwise, permutation, and logical layers.
- Modular seed expansion logic.
- Designed with cryptographic research standards in mind.

## Benchmark Highlights

- **v2**: ~6 GiB/s throughput on standard laptop hardware.
- **v3**: Verified up to 32 GiB input size with 0 anomalies in PractRand.
- All versions documented with failure cases, entropy evaluation, and revision history.

## Applications

- Procedural encryption for images
- Scrambling index structures
- PRNG-based music generation
- Experimental chaotic compression

## Status

> Still in private research phase. Not recommended for cryptographic production until peer-reviewed.

## License

This project is licensed under the [Apache License 2.0](./LICENSE).
