# bindings/cpython

Python binding for the `ra_prng2` C engine.

## `_ra_prng2.py` (current)

A `ctypes` wrapper over `ra_prng_init` / `ra_prng_next` / `ra_prng_advance`
from `src/ra_prng2/c/ra_prng2_struct.c`. It calls into the existing C
functions only — no PRNG logic is reimplemented in Python.

Build the shared library it depends on, then use it:

```bash
make build-ext          # -> build/lib/libra_prng2.so
```

Alternatively, `pip install .` (or `pip install <sdist>.tar.gz`) compiles the
same C source via `gcc` at install time and bundles `libra_prng2.so` inside
the installed `bindings.cpython` package automatically - no separate
`make build-ext` step needed in that case. Requires `gcc` on the target
machine. Because compilation uses `-march=native`, install packages this way
**on the machine they'll run on** - do not copy a wheel built on one machine
to another with a different CPU.

```python
from bindings.cpython._ra_prng2 import RaPrng2

r = RaPrng2(seed=42)
r.next()       # one output
r.advance(10)  # advance 10 outputs, return the last
```

## `ra_prng.cpython-314-x86_64-linux-gnu.so` (orphaned, not reproducible)

Three identical copies of a compiled CPython extension (`.so`) predate this
refactor, scattered across `bindings/cpython/`, `sub-projects/image_encrypting/`,
and `tests/`. No `.c`/`.cpp` source implementing a CPython C-API or pybind11
module exists anywhere in the repository or its history, so this binary
cannot be rebuilt or verified. It is left in place on disk (now correctly
covered by `.gitignore`) but is superseded by `_ra_prng2.py` above, which
*is* reproducible from source. If the original binding source turns up, it
should replace this note; otherwise these `.so` files can eventually be
deleted.
