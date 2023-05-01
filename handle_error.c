#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

void handle_error(int retval) {
  printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
  exit(1);
}

long long **alloc_values(size_t num_threads, size_t num_events) {
  long long **values = malloc(num_threads * sizeof(long long *));
  for (int tid = 0; tid < num_threads; tid++) {
    values[tid] = malloc(num_events * sizeof(long long));
    for (int eid = 0; eid < num_events; eid++)
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
