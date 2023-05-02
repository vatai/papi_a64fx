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

  double now = omp_get_wtime();
  // begin

#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = a[i] * b[i] + c[i];
  }
  double sum = 0;
  for (int i = 0; i < N; i++) {
    sum += a[i];
  }
  printf("result: %f\n", sum);

  printf("Time: %lf\n", omp_get_wtime() - now);
  free_values(values, num_threads);
  return 0;
}
