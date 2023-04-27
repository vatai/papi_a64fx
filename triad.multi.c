#include <omp.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

#include "handle_error.h"

#define N (1lu << 28)

unsigned long omp_get_thread_num_wrapper(void) {
  return (unsigned long)omp_get_thread_num();
}

int main(int argc, char *argv[]) {
  double a[N], b[N], c[N];
  int retval;
  // int native = 0x0;
  char event_str[PAPI_MAX_STR_LEN] = "FP_SCALE_OPS_SPEC";
  int num_threads = omp_get_max_threads();
  long long *values = malloc(num_threads * sizeof(long long));

  printf("Start: %s (num_threads: %d)\n", argv[0], num_threads);
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error!\n");
    exit(1);
  }
  // chk(PAPI_event_name_to_code(event_str, &native), "name to code failed");
  // chk(PAPI_query_event(native), "zero not an event!");

  chk(PAPI_thread_init(omp_get_thread_num_wrapper),
      "PAPI_thread_init() failed.\n");

#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = (i + 3) % 11;
    b[i] = (3 * i + 2) % 29;
    c[i] = (5 * i + 1) % 13;
  }

  // BEGIN WORK
  double now = omp_get_wtime();
#pragma omp parallel
  {
    int event_set = PAPI_NULL;
    chk(PAPI_create_eventset(&event_set), "Couldn't create eventset.");
    chk(PAPI_add_named_event(event_set, event_str), "Couldn't add event.");
    chk(PAPI_start(event_set), "Coulnd't start event set.");
#pragma omp for
    for (int i = 0; i < N; i++) {
      a[i] = a[i] * b[i] + c[i];
    }
    int tid = omp_get_thread_num_wrapper();
    chk(PAPI_stop(event_set, &values[tid]), "Couldn't stop event set.");
  }
  printf("Time: %lf\n", omp_get_wtime() - now);
  // END WORK

  double sum = 0;
  for (int i = 0; i < N; i++) {
    sum += a[i];
  }

  long long total_values = 0;
  for (int i = 0; i < num_threads; i++) {
    printf("counter[%d]: %lld\n", i, values[i]);
    total_values += values[i];
  }
  printf("total_counter: %lld\n", values[i]);
  printf("result: %f\n", sum);

  free(values);
  printf("DONE!\n\n");
  return 0;
}
