#include <omp.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "handle_error.h"

int main(int argc, char *argv[]) {
  double a[N], b[N], c[N];
  int retval;
  int native = 0x0;

  long long values[NUM_EVENTS];
  for (int eid = 0; eid < NUM_EVENTS; eid++)
    values[eid] = 0;

  printf("Start: %s\n", argv[0]);
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error!\n");
    exit(1);
  }
  for (int eid = 0; eid < NUM_EVENTS; eid++)
    chk(PAPI_event_name_to_code(event_str[eid], &native),
        "name to code failed");
  chk(PAPI_query_event(native), "zero not an event!");

  int event_set = PAPI_NULL;
  chk(PAPI_create_eventset(&event_set), "Couldn't create eventset.");
  chk(PAPI_add_event(event_set, native), "Couldn't add event.");
#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = (i + 3) % 11;
    b[i] = (3 * i + 2) % 29;
    c[i] = (5 * i + 1) % 13;
  }

  // BEGIN WORK
  chk(PAPI_start(event_set), "Coulnd't start event set.");
  double now = omp_get_wtime();
#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = a[i] * b[i] + c[i];
  }
  chk(PAPI_stop(event_set, values), "Couldn't stop event set.");
  printf("Time: %lf\n", omp_get_wtime() - now);
  // END WORK

  double sum = 0;
  for (int i = 0; i < N; i++) {
    sum += a[i];
  }

  for (int eid = 0; eid < NUM_EVENTS; eid++) {
    printf("%s: %lld\n", event_str[eid], values[eid]);
  }
  printf("result: %f\n", sum);
  printf("DONE!\n\n");
  return 0;
}
