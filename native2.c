#include <stdlib.h>
#include <stdio.h>
#include <papi.h>

main()
{
    int EventSet = PAPI_NULL;
    unsigned int native = 0x0;
    int retval, i;
    PAPI_event_info_t info;
    PAPI_event_info_t *infostructs;

    /* Initialize the library */
    retval = PAPI_library_init(PAPI_VER_CURRENT);

    if (retval != PAPI_VER_CURRENT) {
        fprintf(stderr,"PAPI library init error!\n");
        exit(1);
    }

    /* Check to see if the preset, PAPI_FP_OPS, exists */
    retval = PAPI_query_event( PAPI_FP_OPS );
    if (retval != PAPI_OK) {
        fprintf (stderr,"No instruction counter? How lame.\n");
        handle_error( retval );
    }

    /* Get details about the preset, PAPI_FP_OPS */
    retval = PAPI_get_event_info( PAPI_FP_OPS, &info );
    if ( retval != PAPI_OK ) {
        fprintf (stderr,"No event info? How lame.\n");
        handle_error( retval );
    }

    if ( info.count > 0 )
        printf ("This event is available on this hardware.\n");

    if ( strcmp( info.derived, "NOT_DERIVED" ))
        printf ( "This event is a derived event on this hardware.\n" );

    /* Count the number of available preset events between 
       PAPI_FP_OPS and the end of the preset list */
    retval = 0;
    i = PAPI_FP_OPS;

    while (PAPI_enum_event(&i, PAPI_ENUM_EVENTS) == PAPI_OK)
       retval++;
    printf ( "There are %d PRESET events after PAPI_FP_OPS.\n", retval );
    exit(0);
}
