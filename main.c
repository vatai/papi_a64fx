#include "papi.h"

int main()
{
    int retval;

    retval = PAPI_hl_region_begin("computation");
    if ( retval != PAPI_OK )
        handle_error(1);

    /* Do some computation here */

    retval = PAPI_hl_region_end("computation");
    if ( retval != PAPI_OK )
        handle_error(1);
}
