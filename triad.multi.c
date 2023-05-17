#include <linux/prctl.h>
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
#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = (i + 3) % 11;
    b[i] = (3 * i + 2) % 29;
    c[i] = (5 * i + 1) % 13;
  }
  int num_threads = omp_get_max_threads();
  printf("Start: %s (num_threads: %d)\n", argv[0], num_threads);

  char event_str[][PAPI_MAX_STR_LEN] = {
      "FP_SCALE_OPS_SPEC",    // 0x80c0 0
      "FP_FIXED_OPS_SPEC",    // 0x80c1 1
      "L2D_CACHE_REFILL",     // 0x0017 2
      "L2D_CACHE_WB",         // 0x0018 3
      "L2D_SWAP_DM",          // 0x0325 4
      "L2D_CACHE_MIBMCH_PRF", // 0x0326 5
  };
#define NUM_EVENTS (sizeof(event_str) / sizeof(*event_str))
  int retval;
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error %x!\n", PAPI_VER_CURRENT);
    exit(1);
  }
  for (int eid = 0; eid < NUM_EVENTS; eid++) {
    int native = 0x0;
    chk(PAPI_event_name_to_code(event_str[eid], &native),
        "name to code failed");
    chk(PAPI_query_event(native), "zero not an event!");
  }
  chk(PAPI_thread_init(omp_get_tid_wrapper), "PAPI_thread_init() failed.\n");
  long long veclen = prctl(PR_SVE_GET_VL);
  double now = omp_get_wtime();
  int *event_set = malloc(num_threads * sizeof(int));
  long long *cntvct = malloc(num_threads * sizeof(long long));
  long long **values = alloc_values(num_threads, NUM_EVENTS);
#pragma omp parallel
  {
    int tid = omp_get_tid_wrapper();
    event_set[tid] = PAPI_NULL;
    chk(PAPI_create_eventset(&event_set[tid]), "Couldn't create eventset.");
    for (int eid = 0; eid < NUM_EVENTS; eid++)
      chk(PAPI_add_named_event(event_set[tid], event_str[eid]),
          "Couldn't add event.");
    chk(PAPI_start(event_set[tid]), "Coulnd't start event set.");
    cntvct[tid] = PAPI_get_real_cyc();
  }

#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = a[i] * b[i] + c[i];
  }

#pragma omp parallel
  {
    int tid = omp_get_tid_wrapper();
    cntvct[tid] = PAPI_get_real_cyc() - cntvct[tid];
    chk(PAPI_stop(event_set[tid], values[tid]), "Couldn't stop event set.");
  }
  printf("Time: %lf\n", omp_get_wtime() - now);
  for (int eid = 0; eid < NUM_EVENTS; eid++) {
    long long total_values = 0;
    for (int tid = 0; tid < num_threads; tid++) {
      total_values += values[tid][eid];
    }
    printf("%s: %lld\n", event_str[eid], total_values);
  }
  for (size_t tid = 0; tid < num_threads; tid++) {
    long long *val = values[tid];
    double numer = val[0] * veclen + val[1];
    double denum = cntvct[tid]; // cntfrq;
    double gflops = numer / (long long)(1e9) / denum;
    printf("cntvct: %10lld; ", cntvct[tid]);
    for (size_t eid = 0; eid < NUM_EVENTS; eid++) {
      printf("v[%d]: %6lld; ", eid, val[eid]);
    }
    printf("GFLOPS(%d): %f\n", tid, gflops);
  }
  free_values(values, num_threads);
  free(cntvct);
  double sum = 0;
  for (int i = 0; i < N; i++) {
    sum += a[i];
  }
  printf("result: %f\n", sum);
  return 0;
}
