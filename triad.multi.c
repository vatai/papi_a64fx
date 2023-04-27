#include <omp.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "handle_error.h"

unsigned long omp_get_thread_num_wrapper(void) {
  return (unsigned long)omp_get_thread_num();
}

int main(int argc, char *argv[]) {
  double a[N], b[N], c[N];
  int retval;
  // int native = 0x0;
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
  int *event_set = malloc(num_threads * sizeof(int));
#pragma omp parallel
  {
    int tid = omp_get_thread_num_wrapper();
    event_set[tid] = PAPI_NULL;
    chk(PAPI_create_eventset(&event_set[tid]), "Couldn't create eventset.");
    chk(PAPI_add_named_event(event_set[tid], event_str), "Couldn't add event.");
    chk(PAPI_start(event_set[tid]), "Coulnd't start event set.");
  }
  usleep(1000000);
#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = a[i] * b[i] + c[i];
  }
  usleep(1000000);
#pragma omp parallel
  {
    int tid = omp_get_thread_num_wrapper();
    chk(PAPI_stop(event_set[tid], &values[tid]), "Couldn't stop event set.");
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
  printf("total_counter: %lld\n", total_values);
  printf("result: %f\n", sum);

  free(values);
  printf("DONE!\n\n");
  return 0;
}
