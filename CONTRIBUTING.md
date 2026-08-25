# Contributing

External contributions are not officially open yet (per README.md), but
technical feedback and discussion are welcome via the discussion forum, or
by contacting the author (see README.md).

## Build

```bash
make build-c      # compile the CLI programs -> build/bin/{ra_prng2,ra_prng2_thread,ra_prng3}
make build-ext     # compile the shared library -> build/lib/libra_prng2.so
make install        # editable install of the pure-Python ra_prng2 module (pip install -e .)
make dev-install     # same, plus dev dependencies (pytest, build)
make test             # run tests/ (currently an empty scaffold - see tests/)
make clean             # remove build/
```

All build output goes to `build/` (gitignored). Do not compile in-place into
`src/`/`bindings/` directories - that produces stray untracked binaries.

## Coding conventions

- C sources compile with `gcc -std=gnu17 -include stdalign.h` (see `Makefile`)
  - GNU extensions are relied on (`clock_gettime`), and `-include stdalign.h`
    supplies the `alignas` macro the sources use without requiring C23.
- Some comments and identifiers are still in Indonesian and have not yet been
  translated to English (noted in README.md) - this is expected, not a bug.
- This PRNG is **not** designed or intended for cryptographic use.

## Benchmarks

`benchmarks/results/` holds precomputed statistical-test output (Dieharder,
NIST STS, TestU01, PractRand) produced manually against the environment
described in README.md ("Benchmarking and Evaluation"). There is currently no
automated benchmark runner (`make bench` prints a pointer to the raw files);
adding one is future work, not part of this refactor.

## Repository layout

See `docs/STRUCTURE.md`.
