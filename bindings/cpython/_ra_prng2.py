"""ctypes binding for the ra_prng2 C engine.

This is glue only - it marshals calls into ra_prng_init/ra_prng_next/
ra_prng_advance, already implemented and reviewed in
src/ra_prng2/c/ra_prng2_struct.c. No PRNG algorithm logic lives here.

Requires the shared library built by `make build-ext`
(-> build/lib/libra_prng2.so).
"""

import ctypes
from pathlib import Path

_MODULE_DIR = Path(__file__).resolve().parent
_INSTALLED_LIB_PATH = _MODULE_DIR / "libra_prng2.so"  # pip install case
_REPO_ROOT = _MODULE_DIR.parents[1]
_DEV_LIB_PATH = _REPO_ROOT / "build" / "lib" / "libra_prng2.so"  # make build-ext case


def _find_lib_path() -> Path:
    if _INSTALLED_LIB_PATH.exists():
        return _INSTALLED_LIB_PATH
    if _DEV_LIB_PATH.exists():
        return _DEV_LIB_PATH
    raise FileNotFoundError(
        f"libra_prng2.so not found at {_INSTALLED_LIB_PATH} or {_DEV_LIB_PATH}. "
        "In a dev checkout, run `make build-ext`. If this package was "
        "installed via pip, gcc was likely missing at install time - "
        "reinstall after installing gcc."
    )


_LIB_PATH = _find_lib_path()

# The RA_PRNG struct layout (src/ra_prng2/c/ra_prng2_struct.c) is not
# replicated here. Instead we hand the C side a large-enough, correctly
# aligned block of raw memory and let it own every field exclusively.
_STATE_BYTES = 4096  # sizeof(RA_PRNG) is 2112 bytes as of this writing; padded generously
_ALIGNMENT = 64  # matches __attribute__((aligned(64))) on RA_PRNG.L / RA_PRNG.M


def _load_library() -> ctypes.CDLL:
    lib = ctypes.CDLL(str(_LIB_PATH))
    lib.ra_prng_init.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint64]
    lib.ra_prng_init.restype = None
    lib.ra_prng_next.argtypes = [ctypes.c_void_p]
    lib.ra_prng_next.restype = ctypes.c_uint32
    lib.ra_prng_advance.argtypes = [ctypes.c_void_p, ctypes.c_size_t]
    lib.ra_prng_advance.restype = ctypes.c_uint32
    return lib


_lib = None


def _lib_handle() -> ctypes.CDLL:
    global _lib
    if _lib is None:
        _lib = _load_library()
    return _lib


class RaPrng2:
    """Thin wrapper around one RA_PRNG instance."""

    def __init__(self, seed: int, remaining_count: int = 0):
        lib = _lib_handle()
        # Over-allocate and hand-align to a 64-byte boundary ourselves; keep
        # `_raw` referenced for the object's lifetime so it is never
        # garbage-collected while `_state` still points into it.
        self._raw = ctypes.create_string_buffer(_STATE_BYTES + _ALIGNMENT)
        raw_addr = ctypes.addressof(self._raw)
        aligned_addr = (raw_addr + _ALIGNMENT - 1) & ~(_ALIGNMENT - 1)
        self._state = ctypes.c_void_p(aligned_addr)
        lib.ra_prng_init(self._state, ctypes.c_uint32(seed), ctypes.c_uint64(remaining_count))

    def next(self) -> int:
        return _lib_handle().ra_prng_next(self._state)

    def advance(self, iterations: int) -> int:
        return _lib_handle().ra_prng_advance(self._state, ctypes.c_size_t(iterations))
