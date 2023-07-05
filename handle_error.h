#ifndef _HANDLE_ERROR_H_
#define _HANDLE_ERROR_H_

#include <papi.h>
#include <stdlib.h>

struct workload_t {
  size_t num_elements;
  double *A;
  double *B;
  double *C;
};

void setup_workload(struct workload_t *workload, size_t num_elements);

void teardown_workload(struct workload_t *workload);

void init_papi(size_t num_events, char event_str[][PAPI_MAX_STR_LEN]);

long long **alloc_values(size_t num_threads, size_t num_events);

void free_values(long long **values, size_t num_threads);

#define chk(X, msg)                                                            \
  {                                                                            \
    int retval = X;                                                            \
    if (retval != PAPI_OK) {                                                   \
      fprintf(stderr, "%s\n", msg);                                            \
      printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));            \
      exit(1);                                                                 \
    }                                                                          \
  }

#endif // _HANDLE_ERROR_H_
