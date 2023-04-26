#include <omp.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

#include "handle_error.h"

#define N (1lu << 28)

int main(int argc, char *argv[]) {
  double a[N], b[N], c[N];
  int retval;
  // int native = 0x0;
  char event_str[PAPI_MAX_STR_LEN] = "FP_SCALE_OPS_SPEC";
  int num_threads = omp_get_max_threads();
  long long *values = malloc(num_threads * sizeof(*values));

  printf("Start: %s\n", argv[0]);
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error!\n");
    exit(1);
  }
  // chk(PAPI_event_name_to_code(event_str, &native), "name to code failed");
  // chk(PAPI_query_event(native), "zero not an event!");

  chk(PAPI_thread_init(omp_get_thread_num), "PAPI_thread_init() failed.\n");

#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = (i + 3) % 11;
    b[i] = (3 * i + 2) % 29;
    c[i] = (5 * i + 1) % 13;
  }

  // BEGIN WORK
  double now = omp_get_wtime();
#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    int event_set = PAPI_NULL;
    chk(PAPI_create_eventset(&event_set), "Couldn't create eventset.");
    chk(PAPI_add_named_event(event_set, event_str), "Couldn't add event.");
    chk(PAPI_start(event_set), "Coulnd't start event set.");
    a[i] = a[i] * b[i] + c[i];
    size_t tid = omp_get_thread_num();
    chk(PAPI_stop(event_set, values + tid), "Couldn't stop event set.");
  }
  printf("Time: %lf\n", omp_get_wtime() - now);
  // END WORK

  double sum = 0;
  for (int i = 0; i < N; i++) {
    sum += a[i];
  }

  printf("counter: %lld\n", values);
  printf("result: %f\n", sum);

  free(values);
  printf("DONE!\n");
  return 0;
}
