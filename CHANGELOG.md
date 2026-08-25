# Changelog

Format based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

### Added
- `pyproject.toml`: installable packaging for the pure-Python `ra_prng2` module.
- `Makefile`: real `build-c` / `build-ext` / `install` / `dev-install` / `test` / `bench` / `clean` targets.
- `bindings/cpython/_ra_prng2.py`: reproducible ctypes binding over `ra_prng2_struct.c`'s
  `ra_prng_init`/`ra_prng_next`/`ra_prng_advance`, replacing the orphaned, source-less
  `.so` files that were scattered across the repo.
- `docs/STRUCTURE.md`, `docs/api/`, `CONTRIBUTING.md`: repository layout and build documentation.
- `.github/workflows/ci.yml`: build + smoke-test on push/PR.
- `graphify-out/GRAPH_REPORT.md`: `graphify` knowledge-graph scan of the repository
  (276 nodes / 375 edges / 32 communities; other `graphify-out/*` outputs are
  regenerable and gitignored).

### Fixed
- `.gitignore`: every pattern line used a trailing `# comment`, which git does not
  treat as a comment - the whole line (including the comment text) was the literal,
  never-matching pattern. `*.so`/`*.o`/`*.a`/`*.out` were consequently never actually
  ignored.
- `README.md`: stale build instructions (referenced `ra_prng2/src` and
  `ra_prng_thread.c`, which no longer match the current `src/ra_prng2/c/` layout).
