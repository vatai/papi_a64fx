#include <omp.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "handle_error.h"

int main(int argc, char *argv[]) {
  struct workload_t workload;
  setup_workload(&workload, N);

  size_t num_threads = 1;
  printf("Start: %s\n", argv[0]);

  init_papi(NUM_EVENTS, event_str);

  long long **values = alloc_values(num_threads, NUM_EVENTS);

  int event_set = PAPI_NULL;
  chk(PAPI_create_eventset(&event_set), "Couldn't create eventset.");
  for (int eid = 0; eid < NUM_EVENTS; eid++) {
    chk(PAPI_add_named_event(event_set, event_str[eid]), "Couldn't add event.");
  }
  chk(PAPI_start(event_set), "Coulnd't start event set.");
  double now = omp_get_wtime();

  /* WORK DONE HERE */
  for (int i = 0; i < workload.num_elements; i++) {
    workload.A[i] = workload.A[i] * workload.B[i] + workload.C[i];
  }

  chk(PAPI_stop(event_set, values[0]), "Couldn't stop event set.");

  teardown_workload(&workload);

  printf("Time: %lf\n", omp_get_wtime() - now);
  for (int eid = 0; eid < NUM_EVENTS; eid++) {
    printf("%s: %lld\n", event_str[eid], values[0][eid]);
  }

  free_values(values, num_threads);
  return 0;
}
