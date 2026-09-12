// ra_core_engine.h
// Public C prototypes for ra_core_engine.c, for consumers that link against
// it from a separate translation unit (the Cython BitGenerator extension in
// pip-ra-core/ra_core/_bitgen.pyx). The ctypes layer (ra_core/_engine.py)
// does not need this header -- it declares its own argtypes/restype at
// runtime and only ever passes ra_core_engine_t around as an opaque
// ctypes.c_void_p.
//
// ra_core_engine_t itself stays an OPAQUE type here (forward-declared, no
// member list) -- nothing outside ra_core_engine.c needs its layout, only a
// pointer to pass back into these functions.
#ifndef RA_CORE_ENGINE_H
#define RA_CORE_ENGINE_H

#include <stddef.h>
#include <stdint.h>

typedef struct ra_core_engine_s ra_core_engine_t;

size_t ra_core_engine_struct_size(void);

void ra_core_engine_init(ra_core_engine_t *e, uint32_t key, uint32_t addr_hi);
void ra_core_engine_next(ra_core_engine_t *e, size_t n, uint32_t *out);
uint64_t ra_core_engine_position(const ra_core_engine_t *e);
void ra_core_engine_fork(const ra_core_engine_t *src, ra_core_engine_t *child);
void ra_core_engine_save(const ra_core_engine_t *e, uint8_t *out);
void ra_core_engine_load(ra_core_engine_t *e, const uint8_t *in);
uint32_t ra_core_engine_key(const ra_core_engine_t *e);
uint32_t ra_core_engine_addr_hi(const ra_core_engine_t *e);

void ra_core_orbit_into(uint32_t key, uint32_t addr_hi, size_t rng, uint32_t *out);
void ra_core_singleblock_into(uint32_t key, uint32_t addr_hi, size_t rng, uint32_t *out);
void ra_core_shuffle_indices(uint32_t key, size_t n, uint32_t *out_indices);

#endif
