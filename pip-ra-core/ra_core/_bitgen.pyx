# distutils: language = c
# cython: language_level=3
#
# numpy.random.BitGenerator integration for ra_core.
#
# Pure ctypes could not do this: numpy.random.BitGenerator.capsule is a
# `cdef readonly` C-level attribute (see numpy/random/bit_generator.pxd) --
# writable only from Cython/C code within the class hierarchy, not from a
# pure-Python subclass (see experiments/2026-9-7 session notes: the ctypes
# attempt raised "attribute 'capsule' ... is not writable"). This is the
# numpy-documented way to extend BitGenerator, matching how numpy's own
# PCG64/MT19937/etc are implemented.
#
# The three next_* callbacks are plain `cdef ... noexcept nogil` C functions
# (function pointers assigned once, at construction) -- no per-call Python
# overhead, unlike the abandoned ctypes callback approach.
#
# The underlying ra_core_engine_t state itself is NOT reimplemented or
# duplicated here: this file only calls the already-validated C API in
# ra_core_engine.c/.h (ra_core_engine_init/next/struct_size), compiled as an
# extra source file into this same extension module (see setup.py).

from libc.stdint cimport uint32_t, uint64_t, uint8_t
from libc.stdlib cimport malloc, free
from cpython.pycapsule cimport PyCapsule_New

from numpy.random.bit_generator cimport BitGenerator, bitgen_t


cdef extern from "ra_core_engine.h" nogil:
    ctypedef struct ra_core_engine_t:
        pass
    size_t ra_core_engine_struct_size()
    void ra_core_engine_init(ra_core_engine_t *e, uint32_t key, uint32_t addr_hi)
    void ra_core_engine_next(ra_core_engine_t *e, size_t n, uint32_t *out)
    uint64_t ra_core_engine_position(const ra_core_engine_t *e)


cdef uint32_t _next_uint32(void *st) noexcept nogil:
    cdef uint32_t word
    ra_core_engine_next(<ra_core_engine_t *>st, 1, &word)
    return word


cdef uint64_t _next_uint64(void *st) noexcept nogil:
    cdef uint32_t lo, hi
    ra_core_engine_next(<ra_core_engine_t *>st, 1, &lo)
    ra_core_engine_next(<ra_core_engine_t *>st, 1, &hi)
    return (<uint64_t>hi << 32) | <uint64_t>lo


cdef double _next_double(void *st) noexcept nogil:
    return (_next_uint64(st) >> 11) * (1.0 / 9007199254740992.0)  # 2**-53


cdef uint64_t _next_raw(void *st) noexcept nogil:
    return _next_uint64(st)


cdef class RaCoreBitGenerator(BitGenerator):
    """numpy.random.BitGenerator backed by a continuous ra_core orbit
    stream at (key, addr_hi). Use with numpy.random.Generator:

        gen = numpy.random.Generator(RaCoreBitGenerator(key=42))
        gen.integers(0, 100, size=1_000_000)

    Compiled (Cython) next_* callbacks -- no per-word Python overhead.
    """
    cdef ra_core_engine_t *_state
    cdef readonly uint32_t key
    cdef readonly uint32_t addr_hi

    def __cinit__(self, key, addr_hi=0):
        BitGenerator.__init__(self, None)
        self.key = <uint32_t>(int(key) & 0xFFFFFFFF)
        self.addr_hi = <uint32_t>(int(addr_hi) & 0xFFFFFFFF)

        self._state = <ra_core_engine_t *>malloc(ra_core_engine_struct_size())
        if self._state == NULL:
            raise MemoryError("failed to allocate ra_core_engine_t state")
        ra_core_engine_init(self._state, self.key, self.addr_hi)

        self._bitgen.state = <void *>self._state
        self._bitgen.next_uint64 = &_next_uint64
        self._bitgen.next_uint32 = &_next_uint32
        self._bitgen.next_double = &_next_double
        self._bitgen.next_raw = &_next_raw

        self._ctypes = None
        self._cffi = None
        self.capsule = PyCapsule_New(<void *>&self._bitgen, "BitGenerator", NULL)

    def __init__(self, key, addr_hi=0):
        # __cinit__ (above) already did all real initialization, including
        # BitGenerator.__init__(self, None) for the base class's own
        # bookkeeping. This override only exists so Python doesn't fall
        # through to BitGenerator.__init__(self, seed=None) with our
        # (key, addr_hi) arguments, which don't match that signature.
        pass

    def __dealloc__(self):
        if self._state != NULL:
            free(self._state)
            self._state = NULL

    @property
    def position(self):
        """Total words this stream has emitted so far."""
        return ra_core_engine_position(self._state)

    def __repr__(self):
        return (f"RaCoreBitGenerator(key=0x{self.key:08x}, "
                f"addr_hi=0x{self.addr_hi:08x}, position={self.position})")
