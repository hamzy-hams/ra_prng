"""ctypes binding for the ra_core engine (src/ra_core/c/ra_core_engine.c).

This is glue only - it marshals calls into the C functions already
implemented and reviewed in ra_core_engine.c (which itself only reuses
ra_core.c's validated statics; no PRNG formula lives here). Requires the
shared library built by `pip install .` (packaged install) or
`make build-ext-core` (dev checkout, -> build/lib/libra_core_engine.so).
"""

import ctypes
from pathlib import Path

_MODULE_DIR = Path(__file__).resolve().parent
_INSTALLED_LIB_PATH = _MODULE_DIR / "libra_core_engine.so"  # pip install case
_REPO_ROOT = _MODULE_DIR.parents[2]  # pip-ra-core/ra_core/ -> pip-ra-core/ -> repo root
_DEV_LIB_PATH = _REPO_ROOT / "build" / "lib" / "libra_core_engine.so"  # make build-ext-core case


def _find_lib_path() -> Path:
    if _INSTALLED_LIB_PATH.exists():
        return _INSTALLED_LIB_PATH
    if _DEV_LIB_PATH.exists():
        return _DEV_LIB_PATH
    raise FileNotFoundError(
        f"libra_core_engine.so not found at {_INSTALLED_LIB_PATH} or {_DEV_LIB_PATH}. "
        "In a dev checkout, run `make build-ext-core`. If this package was "
        "installed via pip, gcc was likely missing at install time - "
        "reinstall after installing gcc."
    )


_LIB_PATH = _find_lib_path()

_ALIGNMENT = 64  # matches alignas(64) on ra_core_engine_t's L/M fields


def _load_library() -> ctypes.CDLL:
    lib = ctypes.CDLL(str(_LIB_PATH))

    lib.ra_core_orbit_into.argtypes = [
        ctypes.c_uint32, ctypes.c_uint32, ctypes.c_size_t, ctypes.POINTER(ctypes.c_uint32),
    ]
    lib.ra_core_orbit_into.restype = None

    lib.ra_core_singleblock_into.argtypes = [
        ctypes.c_uint32, ctypes.c_uint32, ctypes.c_size_t, ctypes.POINTER(ctypes.c_uint32),
    ]
    lib.ra_core_singleblock_into.restype = None

    lib.ra_core_engine_struct_size.argtypes = []
    lib.ra_core_engine_struct_size.restype = ctypes.c_size_t

    lib.ra_core_engine_init.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32]
    lib.ra_core_engine_init.restype = None

    lib.ra_core_engine_next.argtypes = [
        ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_uint32),
    ]
    lib.ra_core_engine_next.restype = None

    lib.ra_core_engine_position.argtypes = [ctypes.c_void_p]
    lib.ra_core_engine_position.restype = ctypes.c_uint64

    lib.ra_core_engine_key.argtypes = [ctypes.c_void_p]
    lib.ra_core_engine_key.restype = ctypes.c_uint32

    lib.ra_core_engine_addr_hi.argtypes = [ctypes.c_void_p]
    lib.ra_core_engine_addr_hi.restype = ctypes.c_uint32

    lib.ra_core_engine_fork.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
    lib.ra_core_engine_fork.restype = None

    lib.ra_core_engine_save.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint8)]
    lib.ra_core_engine_save.restype = None

    lib.ra_core_engine_load.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint8)]
    lib.ra_core_engine_load.restype = None

    lib.ra_core_shuffle_indices.argtypes = [
        ctypes.c_uint32, ctypes.c_size_t, ctypes.POINTER(ctypes.c_uint32),
    ]
    lib.ra_core_shuffle_indices.restype = None

    return lib


_lib = None


def _lib_handle() -> ctypes.CDLL:
    global _lib
    if _lib is None:
        _lib = _load_library()
    return _lib


_engine_struct_size = None


def engine_struct_size() -> int:
    global _engine_struct_size
    if _engine_struct_size is None:
        _engine_struct_size = _lib_handle().ra_core_engine_struct_size()
    return _engine_struct_size


def alloc_aligned_state() -> tuple:
    """Over-allocate and hand-align a buffer to _ALIGNMENT bytes, sized to
    fit ra_core_engine_t exactly (queried from C, not hardcoded). Returns
    (raw_buffer, aligned_void_p) - `raw_buffer` must be kept referenced for
    the object's lifetime so it is never garbage-collected while
    `aligned_void_p` still points into it (same pattern as
    bindings/cpython/_ra_prng2.py's RaPrng2)."""
    size = engine_struct_size()
    raw = ctypes.create_string_buffer(size + _ALIGNMENT)
    raw_addr = ctypes.addressof(raw)
    aligned_addr = (raw_addr + _ALIGNMENT - 1) & ~(_ALIGNMENT - 1)
    return raw, ctypes.c_void_p(aligned_addr)
