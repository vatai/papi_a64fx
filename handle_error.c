#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

void handle_error(int retval) {
  printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
  exit(1);
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

void formula(long long **values, long long *cntvct, size_t num_threads,
             size_t num_events) {
  long long veclen = 512;
  long long cntfrq = 100000000;
  for (size_t tid = 0; tid < num_threads; tid++) {
    long long *val = values[tid];
    double numer = val[0] * veclen + val[1];
    double denum = cntvct[tid] / cntfrq;
    double result_tid = numer / 1e9 / denum;
    for (size_t eid = 0; eid < num_events; eid++) {
      printf("%8lld; ", val[eid]);
    }
    printf("GFLOPS(%d): %llf\n", tid, result_tid);
  }
}
