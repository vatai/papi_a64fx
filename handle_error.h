#ifndef _HANDLE_ERROR_H_
#define _HANDLE_ERROR_H_

#include <papi.h>
#include <stdlib.h>

long long **alloc_values(size_t num_threads, size_t num_events);

void free_values(long long **values, size_t num_threads);

#define chk(X, msg)                                                            \
  {                                                                            \
    retval = X;                                                                \
    if (retval != PAPI_OK) {                                                   \
      fprintf(stderr, "%s\n", msg);                                            \
      printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));            \
      exit(1);                                                                 \
    }                                                                          \
  }

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

#endif // _HANDLE_ERROR_H_
