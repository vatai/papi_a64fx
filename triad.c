#include <omp.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

#include "handle_error.h"

#define N (1lu << 28)

int main() {
  double a[N], b[N], c[N];
  int retval;
  int native = 0x0;
  int event_set = PAPI_NULL;
  long long values = 0;
  char event_str[PAPI_MAX_STR_LEN] = "FP_SCALE_OPS_SPEC";

  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error!\n");
    exit(1);
  }
  chk(PAPI_event_name_to_code(event_str, &native), "name to code failed");
  chk(PAPI_query_event(native), "zero not an event!");

#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = (i + 3) % 11;
    b[i] = (3 * i + 2) % 29;
    c[i] = (5 * i + 1) % 13;
  }

  // BEGIN WORK
  double now = omp_get_wtime();
  chk(PAPI_create_eventset(&event_set), "Couldn't create eventset.");
  chk(PAPI_add_event(event_set, native), "Couldn't add event.");
  chk(PAPI_start(event_set), "Coulnd't start event set.");
#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = a[i] * b[i] + c[i];
  }
  chk(PAPI_stop(event_set, &values), "Couldn't stop event set.");
  printf("Time: %lf\n", omp_get_wtime() - now);
  // END WORK

  double sum = 0;
  for (int i = 0; i < N; i++) {
    sum += a[i];
  }

  printf("counter: %lld\n", values);
  printf("result: %f\n", sum);

  printf("DONE!\n");
  return 0;
}
