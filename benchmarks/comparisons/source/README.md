# benchmarks/comparisons/source

C implementations used to produce the benchmark results in
`benchmarks/comparisons/` and `benchmarks/results/`: `chacha20*.c`, `pcg*.c`,
`philox*.c`, `xoshiro256*.c` (third-party comparison algorithms), plus
`ra_prng2.c` and `ra_prng3.c`.

## `ra_prng2.c` / `ra_prng3.c` here are NOT the canonical implementations

They look similar to `src/ra_prng2/c/ra_prng2.c` and `src/ra_prng3/c/ra_prng3.c`
(same core mixing ideas, same author) but are a **different, derived CLI
tool** - a file-based token-shuffling utility (`ZepFold`):

```
./ra_prng2 --input tokens.txt --output scrambled.txt [--seed S] [--multiplier_m MM] [--multiplier_l ML]
```

Differences from the canonical `src/` versions:
- Reads/writes token arrays from/to files instead of generating a raw output stream.
- Takes `--multiplier_m`/`--multiplier_l` as runtime parameters instead of fixed constants.
- Uses different permutation/index math (Lemire-style fast-reduction indexing
  into the token array) tailored to in-place shuffling rather than raw
  generation.

They are kept as separate files deliberately and are **not** merged with the
canonical `src/` implementations - doing so would risk silently changing the
behavior of either the published-paper generator or this shuffling
experiment. See `docs/STRUCTURE.md` for the full breakdown of every
`ra_prng2`/`ra_prng3` variant in this repository.

`tokens.txt` in this directory is the input corpus these tools were
benchmarked against (flagged by `graphify` as filename-sensitive due to the
word "tokens" - it is PRNG shuffle input data, not credentials).
