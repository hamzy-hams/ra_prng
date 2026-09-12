# ra-core

Pip binding for `ra_core` (orbit + singleblock), the addressable/orbit-
addressing PRNG successor in this repo's `src/ra_core/c/ra_core.c`. Headline
property: **addressable** - `generate(key, addr_hi, n)` produces output
directly from any `(key, addr_hi)` address with no sequential state to
carry, unlike a conventional stateful PRNG (closer to a counter-based
generator like Philox).

## Install

Requires a full checkout of this repo (the C source this package compiles
lives at `../src/ra_core/c/`, one level up from here - this is not yet
published as a standalone sdist/wheel on PyPI):

```bash
git clone <this-repo>
cd ra_prng
pip install ./pip-ra-core
```

Needs `gcc` on the machine you install on (compiles at install time, with
`-march=native` - install on the machine that will run it, don't copy a
wheel built here to a different CPU). The `numpy.random.BitGenerator`
integration additionally needs `Cython` and `numpy` **at build time** (both
pulled in automatically via `pyproject.toml`'s `build-system.requires`) -
nothing extra to install by hand.

## API

### `generate(key, addr_hi, n, *, mode="auto") -> bytes`

Random-access, stateless: `n` little-endian uint32 words as `bytes`, from
address `(key, addr_hi)`. Call it again with a different `addr_hi` to jump
anywhere, in any order - no state carried between calls.

```python
import ra_core
words = ra_core.generate(key=0xCAFEBABE, addr_hi=42, n=1000)
```

`mode="auto"` (default) picks `"singleblock"` for `n<=255` (faster
fixed-size path) and `"orbit"` otherwise; pass `mode="orbit"` or
`mode="singleblock"` to force one.

### `RaCoreEngine` - stateful streaming, fork, save/resume

For when you need to keep pulling from the *same* stream over time (not
just one-shot random access), rather than `generate()`:

```python
e = ra_core.RaCoreEngine(key=555, addr_hi=1)
chunk1 = e.next(1000)
chunk2 = e.next(500)          # continues where chunk1 left off
print(e.key, e.addr_hi, e.position)   # introspection: address + how far in

child = e.fork()              # child = one forced early reseed; e itself
                               # is completely untouched, keeps its own
                               # trajectory as if fork() never happened
e.save("checkpoint.bin")       # binary compact dump (not portable across
                               # CPU architectures/endianness)
resumed = ra_core.RaCoreEngine.load("checkpoint.bin")
```

### `spawn_streams(key, n, addr_hi_start=0) -> list[RaCoreEngine]`

`n` independent streams for the same key, `addr_hi = addr_hi_start,
addr_hi_start+1, ...` - safe for parallel/reproducible-per-worker
generation (each worker gets its own address, no overlap).

### `shuffle(seq, key) -> list`

Deterministic Fisher-Yates shuffle - same `(key, len(seq))` always gives the
same permutation. For `len(seq) > 255` this chains `ra_core_singleblock`
calls with a **key-increment** (Weyl `0x9E3779B9`, `addr_hi` fixed at 0) -
the pattern already validated clean down to K=1 by this repo's production
PractRand/dieharder battery. An `addr_hi`-increment chaining alternative was
tried first and failed PractRand catastrophically at the very first
checkpoint (256MB) - see
`experiments/2026-9-7_addr-hi-chaining-validation/RESULTS.md` in the main
repo if you want the details. Range reduction uses Lemire's fast
multiply-shift method, not modulo.

### `random_key() -> int`

A fresh 32-bit key from OS entropy (`os.urandom`), for callers that don't
want to hardcode one.

### `RaCoreBitGenerator` - `numpy.random.Generator` integration

```python
import numpy as np
from ra_core import RaCoreBitGenerator

gen = np.random.Generator(RaCoreBitGenerator(key=42, addr_hi=0))
gen.integers(0, 100, size=1_000_000)
```

Compiled (Cython) - the `next_uint32`/`next_uint64`/`next_double` callbacks
numpy calls internally are plain C function pointers assigned once at
construction, not a per-word Python/ctypes callback. A pure-ctypes
implementation was tried first and is not possible with current numpy:
`BitGenerator.capsule` is a `cdef readonly` attribute, writable only from
Cython/C code, not from a pure-Python subclass.

For pure bulk generation without needing the `numpy.random.Generator` API
surface (`.integers()`, `.normal()`, etc.), `generate()` (one C call for the
whole buffer) has less overhead than going through a `BitGenerator` at all,
Cython or not.

## Development

```bash
make build-ext-core       # repo root Makefile - dev-loop build of
                           # libra_core_engine.so without a full pip install
pip install -e '.[dev]'   # from pip-ra-core/, editable install + pytest
pytest pip-ra-core/tests/
```
