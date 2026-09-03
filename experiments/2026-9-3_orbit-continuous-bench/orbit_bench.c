// orbit_bench.c
// Continuous-stream real speed harness for ra_core_orbit
// (../2026-9-1_family-productionization/ra_core.c). Scope: user asked
// specifically for orbit's continuous-stream MB/s (the metric singleblock's
// RESULTS.md showed sits close to K=1, NOT midway between K=1 and K=255 --
// see this folder's RESULTS.md for the motivating discussion). K=1/K=255 for
// singleblock already exist in
// ../2026-9-3_combo-winner-pareto-selection/RESULTS.md and are cited there,
// not re-measured here. K=1/K=255 for orbit itself are out of scope too.
//
// Linking strategy: EXTERN-LINK against the actual compiled ra_core.c
// object, not copy-verbatim. Precedent:
// ../2026-9-1_family-productionization/bench_ra_core.c already does this
// exact trick for ra_core_orbit/ra_core_singleblock via
// `-Dmain=<other name> -c ra_core.c -o ra_core_nomain.o`. This means
// ra_core_orbit here IS the production function (same object code), not a
// hand-transcribed copy -- stronger correctness guarantee than
// winners_bench.c's copy-verbatim-plus-KAT approach (that approach was
// needed there because winners_bench.c's 11 other candidates ARE
// hand-transcribed DSL variants with no separate authoritative source).
// bench_ra_core.c itself is NOT reused/modified because it only has
// `throughput`/`reinit-sweep` (Philox-comparison format) -- no `--stream`
// or `--multistream`. Those two modes are ported here from
// ../2026-9-3_combo-winner-pareto-selection/winners_bench.c, verbatim in
// logic, so this binary is a drop-in CLI-compatible sibling of
// winners_bench for the one core it knows about ("orbit").
//
// ra_core.c is NOT modified -- read-only compile input only.
//
// Compile (two steps, flags identical to ra_core.c/winners_bench.c so speed
// numbers are comparable):
//   gcc -O3 -march=native -std=gnu17 -include stdalign.h
//       -Dmain=ra_core_unused_main
//       -c ../2026-9-1_family-productionization/ra_core.c
//       -o ra_core_nomain.o -Wall -Wextra
//   gcc -O3 -march=native -std=gnu17 -include stdalign.h
//       orbit_bench.c ra_core_nomain.o -o orbit_bench -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

extern uint32_t ra_core_orbit(uint32_t key, size_t rng, FILE *raw_stream);

typedef uint32_t (*core_fn_t)(uint32_t key, size_t rng, FILE *raw_stream);
typedef struct { const char *name; core_fn_t fn; long max_rng; } core_entry_t;

static const core_entry_t CORES[] = {
    { "orbit", ra_core_orbit, -1 },  // -1 == unbounded, matches ra_core.c's own CORES[]
};
#define NUM_CORES (sizeof(CORES) / sizeof(CORES[0]))

static const core_entry_t *find_core(const char *name) {
    for (size_t i = 0; i < NUM_CORES; ++i) if (strcmp(CORES[i].name, name) == 0) return &CORES[i];
    fprintf(stderr, "Unknown core '%s'. Known:", name);
    for (size_t i = 0; i < NUM_CORES; ++i) fprintf(stderr, " %s", CORES[i].name);
    fprintf(stderr, "\n");
    exit(1);
}

// ---------------------------------------------------------------------
// --multistream: continuous-stream mode, ported verbatim (logic) from
// winners_bench.c's mode_multistream(). Chunked multikey pattern -- Weyl
// key increment between blocks of K words each, same block_idx*0x9E3779B9u
// scheme used throughout this research line.
// ---------------------------------------------------------------------

static void mode_multistream(const core_entry_t *core, uint32_t base_key, size_t n, size_t K, FILE *out) {
    if (K == 0 || (core->max_rng >= 0 && (long)K > core->max_rng)) {
        fprintf(stderr, "--multistream: K=%zu invalid for core '%s' (max_rng=%ld)\n", K, core->name, core->max_rng);
        exit(1);
    }
    uint64_t block_idx = 0;
    size_t remaining = n;
    while (remaining > 0) {
        size_t block_len = remaining < K ? remaining : K;
        uint32_t key = base_key + (uint32_t)(block_idx * 0x9E3779B9u);
        core->fn(key, block_len, out);
        remaining -= block_len;
        ++block_idx;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage:\n"
            "  %s --stream <core> <key> <rng>\n"
            "  %s --multistream <core> <base_key> <n> <K>\n"
            "Cores: orbit\n",
            argv[0], argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "--stream") == 0 && argc >= 5) {
        const core_entry_t *core = find_core(argv[2]);
        uint32_t key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t rng = (size_t)strtoull(argv[4], NULL, 0);
        core->fn(key, rng, stdout);
        return 0;
    }
    if (strcmp(argv[1], "--multistream") == 0 && argc >= 6) {
        const core_entry_t *core = find_core(argv[2]);
        uint32_t base_key = (uint32_t)strtoul(argv[3], NULL, 0);
        size_t n = (size_t)strtoull(argv[4], NULL, 0);
        size_t K = (size_t)strtoull(argv[5], NULL, 0);
        mode_multistream(core, base_key, n, K, stdout);
        return 0;
    }
    fprintf(stderr, "Unknown mode or missing args: %s\n", argv[1]);
    return 1;
}
