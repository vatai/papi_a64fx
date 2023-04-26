#ifndef _HANDLE_ERROR_H_
#define _HANDLE_ERROR_H_

void handle_error(int retval);

#define chk(X, msg)                                                            \
  {                                                                            \
    retval = X;                                                                \
    if (retval != PAPI_OK) {                                                   \
      fprintf(stderr, "%s\n", msg);                                            \
      handle_error(retval);                                                    \
    }                                                                          \
  }

#endif // _HANDLE_ERROR_H_
