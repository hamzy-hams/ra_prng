// battery_wrapper_orbit.c
// Thin wrapper: exposes ra_core_param.c's ra_core_orbit (parametrized by
// -DRA_N=.. -DRA_RESEED_MODE=..) to tools/battery/*.py via
// driver_main_template.c's CORE_FN (single-core) convention. Gives a
// binary with the --single/--stream/--stream-hashed CLI contract that
// run_battery.py/practrand_gate.py/avalanche.py already speak, N-agnostic.
#define RA_CORE_PARAM_NO_MAIN
#include "ra_core_param.c"

#define CORE_FN ra_core_orbit
#include "../../tools/battery/driver_main_template.c"
