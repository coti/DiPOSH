#ifndef _GNU_SOURCE

#include <stdarg.h>
#include <assert.h>
#include "shmem_portability.h"

int asprintf( char ** buf, const char * format, ... ) {

    va_list arg;
    int rv;
	
    va_start(arg, format);
    rv = vasprintf(buf, format, arg);
    va_end(arg);
	
    return rv;
}

int vasprintf(char **__restrict buf, const char * __restrict format,
              va_list arg) {

    va_list arg2;
    int rv;

    va_copy(arg2, arg);
    rv = vsnprintf(NULL, 0, format, arg2);
    va_end(arg2);
	
    *buf = NULL;
	
    if (rv >= 0) {
        if ((*buf = malloc(++rv)) != NULL) {
            if ((rv = vsnprintf(*buf, rv, format, arg)) < 0) {
                free(*buf);
                *buf = NULL;
            }
        }
    }
	
    assert(rv >= -1);
	
    return rv;
}

#endif
