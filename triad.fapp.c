#include <omp.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "fj_tool/fapp.h"
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
  size_t num_threads = omp_get_max_threads();
  printf("Start (fapp): %s (num_threads: %d)\n", argv[0], num_threads);

  // START begin
  double now = omp_get_wtime();
  fapp_start("roi", 1, 0);
  // START end
#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = a[i] * b[i] + c[i];
  }
  double sum = 0;
  for (int i = 0; i < N; i++) {
    sum += a[i];
  }
  printf("result: %f\n", sum);
  // STOP begin
  fapp_stop("roi", 1, 0);
  // STOP end

  printf("Time: %lf\n", omp_get_wtime() - now);
  return 0;
}
