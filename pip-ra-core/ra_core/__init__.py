"""ra_core: pip binding for the ra_core PRNG (orbit + singleblock).

Headline property: ra_core is *addressable* - generate(key, addr_hi, n)
produces output directly from any (key, addr_hi) address with no
sequential state to carry, unlike a conventional stateful PRNG. See
README.md for details and examples of every function/class below.
"""

import ctypes
import os
import struct
from pathlib import Path

from . import _engine

__all__ = [
    "generate",
    "spawn_streams",
    "shuffle",
    "random_key",
    "RaCoreEngine",
    "RaCoreBitGenerator",
]


def __getattr__(name):
    # Lazy: RaCoreBitGenerator lives in the compiled Cython extension
    # _bitgen (see _bitgen.pyx) - importing ra_core itself should not force
    # that extension to load if a caller never touches this name.
    if name == "RaCoreBitGenerator":
        from ._bitgen import RaCoreBitGenerator
        return RaCoreBitGenerator
    raise AttributeError(f"module {__name__!r} has no attribute {name!r}")

_SINGLEBLOCK_MAX = 255


def _mask32(x: int) -> int:
    return x & 0xFFFFFFFF


def generate(key: int, addr_hi: int, n: int, *, mode: str = "auto") -> bytes:
    """Random-access generate: n little-endian uint32 words (as bytes) from
    the address (key, addr_hi), with no state to carry between calls - call
    it again with a different addr_hi to jump anywhere, in any order.

    mode: "auto" (default) picks "singleblock" for n<=255 (faster fixed-size
    path) and "orbit" otherwise; pass "orbit"/"singleblock" to force one.
    """
    if n == 0:
        return b""
    if mode == "auto":
        mode = "singleblock" if n <= _SINGLEBLOCK_MAX else "orbit"

    lib = _engine._lib_handle()
    buf = (ctypes.c_uint32 * n)()
    if mode == "singleblock":
        if n > _SINGLEBLOCK_MAX:
            raise ValueError(f"mode='singleblock' only supports n<=255, got n={n}")
        lib.ra_core_singleblock_into(_mask32(key), _mask32(addr_hi), n, buf)
    elif mode == "orbit":
        lib.ra_core_orbit_into(_mask32(key), _mask32(addr_hi), n, buf)
    else:
        raise ValueError(f"unknown mode {mode!r}, expected 'auto', 'orbit' or 'singleblock'")
    return bytes(buf)


def spawn_streams(key: int, n: int, addr_hi_start: int = 0) -> list:
    """n independent RaCoreEngine streams for the same key, addr_hi =
    addr_hi_start, addr_hi_start+1, ... - safe for parallel/reproducible-
    per-worker generation (each worker gets its own address, no overlap)."""
    return [RaCoreEngine(key, addr_hi_start + i) for i in range(n)]


def shuffle(seq, key: int) -> list:
    """Deterministic Fisher-Yates shuffle of seq, driven by ra_core.

    Same (key, seq length) always produces the same permutation. For
    n>255 this chains ra_core_singleblock calls with a KEY-increment
    (addr_hi fixed at 0) - the pattern already validated clean down to K=1
    by this repo's production battery; an addr_hi-increment chaining
    alternative was tried first and failed PractRand catastrophically, see
    experiments/2026-9-7_addr-hi-chaining-validation/RESULTS.md in the
    source repo.
    """
    items = list(seq)
    n = len(items)
    if n < 2:
        return items
    idx = (ctypes.c_uint32 * n)()
    _engine._lib_handle().ra_core_shuffle_indices(_mask32(key), n, idx)
    return [items[i] for i in idx]


def random_key() -> int:
    """A fresh 32-bit key from OS entropy, for callers that don't want to
    hardcode one."""
    return struct.unpack("<I", os.urandom(4))[0]


class RaCoreEngine:
    """Stateful orbit-mode stream: continues across .next() calls, can be
    forked, saved and resumed. For one-shot/random-access generation from a
    known address, prefer the module-level generate() instead - this class
    is for when you need to keep pulling from the same stream over time.
    """

    def __init__(self, key: int, addr_hi: int = 0):
        self._raw, self._state = _engine.alloc_aligned_state()
        self._key = _mask32(key)
        self._addr_hi = _mask32(addr_hi)
        _engine._lib_handle().ra_core_engine_init(self._state, self._key, self._addr_hi)

    @classmethod
    def _blank(cls):
        self = cls.__new__(cls)
        self._raw, self._state = _engine.alloc_aligned_state()
        return self

    @property
    def key(self) -> int:
        return self._key

    @property
    def addr_hi(self) -> int:
        return self._addr_hi

    @property
    def position(self) -> int:
        """Total words emitted by this engine so far ("print address
        engine": combine with .key/.addr_hi to see exactly where a stream
        is)."""
        return _engine._lib_handle().ra_core_engine_position(self._state)

    def __repr__(self) -> str:
        return (f"RaCoreEngine(key=0x{self._key:08x}, addr_hi=0x{self._addr_hi:08x}, "
                f"position={self.position})")

    def next(self, n: int) -> bytes:
        """Generate n little-endian uint32 words (as bytes), continuing
        from this engine's current position."""
        if n == 0:
            return b""
        buf = (ctypes.c_uint32 * n)()
        _engine._lib_handle().ra_core_engine_next(self._state, n, buf)
        return bytes(buf)

    def fork(self) -> "RaCoreEngine":
        """A new, independent RaCoreEngine that has gone through one forced
        early reseed - this engine (self) is left completely untouched and
        keeps generating its own trajectory as if fork() had never been
        called."""
        child = RaCoreEngine._blank()
        child._key = self._key
        child._addr_hi = self._addr_hi
        _engine._lib_handle().ra_core_engine_fork(self._state, child._state)
        return child

    def save(self, path) -> None:
        """Dump this engine's full state to `path` as raw bytes (binary
        compact format - not portable across CPU architectures/endianness,
        matches the machine it was written on)."""
        size = _engine.engine_struct_size()
        buf = (ctypes.c_uint8 * size)()
        _engine._lib_handle().ra_core_engine_save(self._state, buf)
        Path(path).write_bytes(bytes(buf))

    @classmethod
    def load(cls, path) -> "RaCoreEngine":
        """Restore an engine previously written by .save(), resuming from
        exactly the position it was saved at (including any partially-
        consumed internal block)."""
        data = Path(path).read_bytes()
        expected = _engine.engine_struct_size()
        if len(data) != expected:
            raise ValueError(
                f"{path}: expected {expected} bytes of ra_core_engine_t state, got {len(data)} "
                "(wrong file, or saved by a different ra-core version)"
            )
        self = cls._blank()
        buf = (ctypes.c_uint8 * expected).from_buffer_copy(data)
        lib = _engine._lib_handle()
        lib.ra_core_engine_load(self._state, buf)
        self._key = lib.ra_core_engine_key(self._state)
        self._addr_hi = lib.ra_core_engine_addr_hi(self._state)
        return self
