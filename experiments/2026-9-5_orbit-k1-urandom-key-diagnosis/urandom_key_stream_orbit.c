// urandom_key_stream_orbit.c -- diagnostic, this session only (2026-09-05).
//
// Question this answers: is the K=1+hashed-key defect in ra_core_orbit
// (FPF-14+6/16, see ../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md)
// caused by the ALGEBRAIC STRUCTURE of the SplitMix32 hash-chain key
// sequence (base+0, base+1, base+2, ... each hashed) resonating with
// ra_core's own init formula -- or is it intrinsic to ra_core_orbit's
// internal math, showing up for basically any sufficiently-random K=1 key
// stream?
//
// This driver replaces the key source with /dev/urandom (kernel CSPRNG,
// ChaCha20-based) instead of SplitMix32(index). /dev/urandom draws have no
// algebraic/index relationship to each other or to any of ra_core's or
// SplitMix32's constants -- a practical proxy for "TRNG" for this specific
// diagnostic purpose (what matters here is the absence of a formula linking
// consecutive keys, not physical entropy quality).
//
// ra_core.c itself is NOT modified -- linked as-is via nomain object,
// same pattern as ../2026-9-4_orbit-kmin-battery/diag_orbit_k1_hashed.c.
//
// Usage: ./urandom_key_stream_orbit <n_words>
//   Writes n_words raw little-endian uint32 words to stdout, each word
//   being ra_core_orbit(key, 1, ...) for a fresh /dev/urandom-drawn key
//   (K=1: one word per key, fresh reinit every block -- matches the
//   K=1+hashed lane's reseed cadence exactly).

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

extern uint32_t ra_core_orbit(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream);

#define URANDOM_BUFSZ (1 << 20) /* 1MB per refill -- avoid a syscall per key */

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <n_words>\n", argv[0]);
        return 1;
    }
    size_t n = (size_t)strtoull(argv[1], NULL, 0);

    FILE *urand = fopen("/dev/urandom", "rb");
    if (!urand) {
        fprintf(stderr, "error: cannot open /dev/urandom: %s\n", strerror(errno));
        return 2;
    }

    static uint8_t buf[URANDOM_BUFSZ];
    size_t buf_pos = URANDOM_BUFSZ; /* force refill on first key */

    for (size_t i = 0; i < n; ++i) {
        if (buf_pos + 4 > URANDOM_BUFSZ) {
            size_t got = fread(buf, 1, URANDOM_BUFSZ, urand);
            if (got < URANDOM_BUFSZ) {
                fprintf(stderr, "error: short read from /dev/urandom (%zu/%d)\n",
                        got, URANDOM_BUFSZ);
                return 3;
            }
            buf_pos = 0;
        }
        uint32_t key;
        memcpy(&key, buf + buf_pos, 4);
        buf_pos += 4;
        ra_core_orbit(key, 0u, 1, stdout);
    }

    fclose(urand);
    return 0;
}
