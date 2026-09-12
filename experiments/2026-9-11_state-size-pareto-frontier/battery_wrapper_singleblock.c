// battery_wrapper_singleblock.c
// Same as battery_wrapper_orbit.c but for ra_core_singleblock. RA_RESEED_MODE
// is accepted but irrelevant here (ra_reseed is dead code for singleblock,
// same as production ra_core.c) -- kept only so BUILD.sh can pass a uniform
// -D set across both cores.
#define RA_CORE_PARAM_NO_MAIN
#include "ra_core_param.c"

#define CORE_FN ra_core_singleblock
#include "../../tools/battery/driver_main_template.c"
