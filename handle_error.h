#ifndef _HANDLE_ERROR_H_
#define _HANDLE_ERROR_H_

#include <stdlib.h>

void handle_error(int retval);

long long **alloc_values(size_t num_threads, size_t num_events);

void free_values(long long **values, size_t num_threads);

void formula(long long **values, long long *cntvct, size_t num_threads,
             size_t num_events);

#define chk(X, msg)                                                            \
  {                                                                            \
    retval = X;                                                                \
    if (retval != PAPI_OK) {                                                   \
      fprintf(stderr, "%s\n", msg);                                            \
      handle_error(retval);                                                    \
    }                                                                          \
  }

#endif // _HANDLE_ERROR_H_
