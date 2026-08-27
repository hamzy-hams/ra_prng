# API Reference: `ra_prng2`

## Python (`src/ra_prng2/python/ra_prng2.py`)

Pure-Python reference implementation. Installed as the `ra_prng2` module via
`pip install -e .` (see `pyproject.toml`).

| Function | Signature | Description |
|---|---|---|
| `rot32` | `rot32(n: int, r: int) -> int` | Rotate a 32-bit value `n` left by `r` bits. |
| `ra_hash` | `ra_hash(N: list[int]) -> list[int]` | Mix a 256-element state array down to 8 output words. |
| `ra_core` | `ra_core(seed, iteration, verbose=False) -> int` | Run the core PRNG loop for `iteration` outer steps; returns the final `cons`. Pass `verbose=True` to print intermediate state (debug only). |

## C - canonical generator

Three variants of the same algorithm in `src/ra_prng2/c/`, see
`docs/STRUCTURE.md` for how they differ. Build with `make build-c` / `make build-ext`.

### `ra_prng_init` / `ra_prng_next` / `ra_prng_advance` (`ra_prng2_struct.c`)

```c
void     ra_prng_init(RA_PRNG *r, uint32_t seed, uint64_t remaining_count /*0 = unlimited*/);
uint32_t ra_prng_next(RA_PRNG *r);                       // one output
uint32_t ra_prng_advance(RA_PRNG *r, size_t iterations);  // run N outputs, return the last
```

`RA_PRNG` holds the full internal state (two 256-entry `uint32_t` arrays plus
bookkeeping); allocate one instance per independent stream. This is the API
consumed by the Python binding below.

## Python binding (`bindings/cpython/_ra_prng2.py`)

```python
from bindings.cpython._ra_prng2 import RaPrng2

r = RaPrng2(seed=42)
r.next()        # -> int, one 32-bit output
r.advance(10)   # -> int, last output after 10 steps
```

`RaPrng2` is a `ctypes` wrapper calling directly into `ra_prng_init`/`next`/
`advance` above (requires `make build-ext` first). See
`bindings/cpython/README.md` for why this replaces the previously-committed,
source-less `.so` files.
