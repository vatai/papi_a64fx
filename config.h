#include <papi.h>

#define N 500000
// (1lu << 18)

char event_str[PAPI_MAX_STR_LEN][] = {"FP_SCALE_OPS_SPEC"};
// char event_str[PAPI_MAX_STR_LEN] = "PAPI_TOT_CYC";
#define NUM_EVENTS (sizeof(event_str) / sizeof(*event_str))
