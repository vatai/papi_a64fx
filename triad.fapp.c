#include <omp.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "config.h"
#include "fj_tool/fapp.h"

unsigned long omp_get_tid_wrapper(void) {
  return (unsigned long)omp_get_thread_num();
}

int main(int argc, char *argv[]) {
  struct workload_t workload;
  setup_workload(&workload, N);

  size_t num_threads = omp_get_max_threads();
  printf("Start (fapp): %s (num_threads: %d)\n", argv[0], num_threads);

  double now = omp_get_wtime();
  fapp_start("roi", 1, 0);

  /* WORK DONE HERE */
#pragma omp parallel for
  for (int i = 0; i < workload.num_elements; i++) {
    workload.A[i] = workload.A[i] * workload.B[i] + workload.C[i];
  }

  fapp_stop("roi", 1, 0);
  printf("Time: %lf\n", omp_get_wtime() - now);

  teardown_workload(&workload);
  return 0;
}
