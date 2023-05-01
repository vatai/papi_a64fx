#include <omp.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "handle_error.h"

unsigned long omp_get_tid_wrapper(void) {
  return (unsigned long)omp_get_thread_num();
}

int main(int argc, char *argv[]) {
  double a[N], b[N], c[N];
  int retval;
  int native = 0x0;
  int num_threads = omp_get_max_threads();
  long long **values = alloc_values(num_threads, NUM_EVENTS);
  printf("Start: %s (num_threads: %d)\n", argv[0], num_threads);
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
  for (int eid = 0; eid < NUM_EVENTS; eid++) {
    chk(PAPI_event_name_to_code(event_str[eid], &native),
        "name to code failed");
    chk(PAPI_query_event(native), "zero not an event!");
  }
  chk(PAPI_thread_init(omp_get_tid_wrapper), "PAPI_thread_init() failed.\n");

  // BEGIN WORK
  double now = omp_get_wtime();
  int *event_set = malloc(num_threads * sizeof(int));
  long long *cntvct = malloc(num_threads * sizeof(long long));
#pragma omp parallel
  {
    int tid = omp_get_tid_wrapper();
    event_set[tid] = PAPI_NULL;
    chk(PAPI_create_eventset(&event_set[tid]), "Couldn't create eventset.");
    for (int eid = 0; eid < NUM_EVENTS; eid++)
      chk(PAPI_add_named_event(event_set[tid], event_str[eid]),
          "Couldn't add event.");
    chk(PAPI_start(event_set[tid]), "Coulnd't start event set.");
    cntvct[tid] = PAPI_get_virt_cyc();
  }
  // usleep(1000000);
#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = a[i] * b[i] + c[i];
  }
  // usleep(1000000);
#pragma omp parallel
  {
    int tid = omp_get_tid_wrapper();
    cntvct[tid] = PAPI_get_virt_cyc() - cntvct[tid];
    chk(PAPI_stop(event_set[tid], values[tid]), "Couldn't stop event set.");
  }
  printf("Time: %lf\n", omp_get_wtime() - now);
  // END WORK

  for (int eid = 0; eid < NUM_EVENTS; eid++) {
    long long total_values = 0;
    for (int tid = 0; tid < num_threads; tid++) {
      total_values += values[tid][eid];
    }
    printf("%s: %lld\n", event_str[eid], total_values);
  }

  double sum = 0;
  for (int i = 0; i < N; i++) {
    sum += a[i];
  }
  printf("result: %f\n", sum);

  free_values(values, num_threads);
  return 0;
}
