#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <papi.h>

#define N 5000000
// #define N (1lu << 18)

char event_str[][PAPI_MAX_STR_LEN] = {
    "FP_SCALE_OPS_SPEC",    // 0x80c0 0
    "FP_FIXED_OPS_SPEC",    // 0x80c1 1
    "L2D_CACHE_REFILL",     // 0x0017 2
    "L2D_CACHE_WB",         // 0x0018 3
    "L2D_SWAP_DM",          // 0x0325 4
    "L2D_CACHE_MIBMCH_PRF", // 0x0326 5
    "EA_CORE",              // 0x01e0 6
    "EA_L2",                // 0x03e0 7
};
// "EA_MEMORY",            // 0x03e8 8
#define NUM_EVENTS (sizeof(event_str) / sizeof(*event_str))

#endif // _CONFIG_H_
