#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <omp.h>

void handle_error (int retval);

#define chk(X, msg) { retval = X; if (retval != PAPI_OK) { fprintf(stderr, "%s\n", msg); handle_error(retval); } }
#define N (1lu << 28)

double work() {
  double a[N], b[N], c[N];
  #pragma omp parallel for
  for (int i = 0; i < N; i++) { a[i] = (i+3) % 11; b[i] = (3*i +2) % 29; c[i] = (5*i+1) % 13; }
  double now = omp_get_wtime();
  #pragma omp parallel for
  for (int i = 0; i < N; i++) { a[i] = a[i] * b[i] + c[i]; }
  printf("Time: %lf\n", omp_get_wtime() - now);
  double sum = 0;
  for (int i = 0; i < N; i++) { sum += a[i]; }
  return sum;
}

int main() {
  int retval;
  int native = 0x0;
  int event_set = PAPI_NULL;
  long long values = 0;
  char event_str[PAPI_MAX_STR_LEN] = "FP_SCALE_OPS_SPEC";
  double result;

  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) { fprintf(stderr, "PAPI library init error!\n"); exit(1); }
  chk(PAPI_event_name_to_code(event_str, &native), "name to code failed");
  chk(PAPI_query_event(native), "zero not an event!");
  chk(PAPI_create_eventset(&event_set), "Couldn't create event.");
  chk(PAPI_add_event(event_set, native), "Couldn't add event.");

  chk(PAPI_start(event_set), "Coulnd't start event set.");
  result = work();
  chk(PAPI_stop(event_set, &values), "Couldn't stop event set.");
  printf("counter: %lld\n", values);
  printf("result: %f\n", result);

  printf("DONE!\n");
  return 0;
}

