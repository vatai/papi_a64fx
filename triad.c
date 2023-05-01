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
  long long cntvct;
  size_t num_threads = 1;
  long long **values = alloc_values(num_threads, NUM_EVENTS);

  printf("Start: %s\n", argv[0]);
#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = (i + 3) % 11;
    b[i] = (3 * i + 2) % 29;
    c[i] = (5 * i + 1) % 13;
  }

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

  // BEGIN WORK
  chk(PAPI_start(event_set), "Coulnd't start event set.");
  cntvct = PAPI_get_virt_cyc();
  double now = omp_get_wtime();
  // #pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = a[i] * b[i] + c[i];
  }
  cntvct = PAPI_get_virt_cyc() - cntvct;
  chk(PAPI_stop(event_set, values[0]), "Couldn't stop event set.");
  printf("Time: %lf\n", omp_get_wtime() - now);
  // END WORK

  for (int eid = 0; eid < NUM_EVENTS; eid++) {
    printf("%s: %lld\n", event_str[eid], values[0][eid]);
  }

  double sum = 0;
  for (int i = 0; i < N; i++) {
    sum += a[i];
  }
  printf("result: %f\n", sum);

  free_values(values, num_threads);
  return 0;
}
