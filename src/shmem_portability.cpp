/*
 * Copyright (c) 2014-2019 LIPN - Universite Paris 13
 *                    All rights reserved.
 *
 * This file is part of POSH.
 * 
 * POSH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * POSH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with POSH.  If not, see <http://www.gnu.org/licenses/>.
 */

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
