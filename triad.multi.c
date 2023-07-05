#include <linux/prctl.h>
#include <omp.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <unistd.h>

#include "common.h"
#include "config.h"

unsigned long omp_get_tid_wrapper(void) {
  return (unsigned long)omp_get_thread_num();
}

int main(int argc, char *argv[]) {
  struct workload_t workload;
  setup_workload(&workload, N);

  int num_threads = omp_get_max_threads();
  printf("Start: %s (num_threads: %d)\n", argv[0], num_threads);

  init_papi(NUM_EVENTS, event_str);

  chk(PAPI_thread_init(omp_get_tid_wrapper), "PAPI_thread_init() failed.\n");
  int *event_set = malloc(num_threads * sizeof(int));
  long long **values = alloc_values(num_threads, NUM_EVENTS);

  double now = omp_get_wtime();
#pragma omp parallel
  {
    int tid = omp_get_tid_wrapper();
    event_set[tid] = PAPI_NULL;
    chk(PAPI_create_eventset(&event_set[tid]), "Couldn't create eventset.");
    for (int eid = 0; eid < NUM_EVENTS; eid++)
      chk(PAPI_add_named_event(event_set[tid], event_str[eid]),
          "Couldn't add event.");
    chk(PAPI_start(event_set[tid]), "Coulnd't start event set.");
  }

  /* WORK DONE HERE */
#pragma omp parallel for
  for (int i = 0; i < workload.num_elements; i++) {
    workload.A[i] = workload.A[i] * workload.B[i] + workload.C[i];
  }

#pragma omp parallel
  {
    int tid = omp_get_tid_wrapper();
    chk(PAPI_stop(event_set[tid], values[tid]), "Couldn't stop event set.");
  }
  printf("Time: %lf\n", omp_get_wtime() - now);
  // sum up counters for all threads
  for (int eid = 0; eid < NUM_EVENTS; eid++) {
    long long total_values = 0;
    for (int tid = 0; tid < num_threads; tid++) {
      total_values += values[tid][eid];
    }
    printf("%s: %lld\n", event_str[eid], total_values);
  }

  // xls veclen bits (not bytes)="<<3"; "/128"=">>7"; total=">> 4"
  int veclen = (prctl(PR_SVE_GET_VL) & PR_SVE_VL_LEN_MASK) >> 4;
  long cache_line = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
  printf("cache line: %ld\n", cache_line);
  for (size_t tid = 0; tid < num_threads; tid++) {
    long long *v = values[tid];
    double flops = v[0] * veclen + v[1];
    double gflops = flops / 1e9;
    double tp = v[2] + v[3] - v[4] - v[5];
    double gtp = tp * cache_line / 1e9;
  }

  teardown_workload(&workload);
  free_values(values, num_threads);
  return 0;
}
