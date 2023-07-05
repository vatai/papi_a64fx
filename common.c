#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

void setup_workload(struct workload_t *workload, size_t num_elements) {
  workload->num_elements = num_elements;
  workload->A = malloc(num_elements * sizeof(double));
  workload->B = malloc(num_elements * sizeof(double));
  workload->C = malloc(num_elements * sizeof(double));

#pragma omp parallel for
  for (int i = 0; i < num_elements; i++) {
    workload->A[i] = (i + 3) % 11;
    workload->B[i] = (3 * i + 2) % 29;
    workload->C[i] = (5 * i + 1) % 13;
  }
}

void teardown_workload(struct workload_t *workload) {
  double sum = 0;
  for (int i = 0; i < workload->num_elements; i++) {
    sum += workload->A[i];
  }
  printf("result: %f\n", sum);
  free(workload->A);
  free(workload->B);
  free(workload->C);
}

void init_papi(size_t num_events, char event_str[][PAPI_MAX_STR_LEN]) {
  int retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error!\n");
    exit(1);
  }
  for (int eid = 0; eid < num_events; eid++) {
    chk(PAPI_query_named_event(event_str[eid]), "zero not an event!");
  }
}

long long **alloc_values(size_t num_threads, size_t num_events) {
  long long **values = malloc(num_threads * sizeof(long long *));
  for (size_t tid = 0; tid < num_threads; tid++) {
    values[tid] = malloc(num_events * sizeof(long long));
    for (size_t eid = 0; eid < num_events; eid++)
      values[tid][eid] = 0;
  }
  return values;
}

void free_values(long long **values, size_t num_threads) {
  for (size_t tid = 0; tid < num_threads; tid++) {
    free(values[tid]);
  }
  free(values);
}
