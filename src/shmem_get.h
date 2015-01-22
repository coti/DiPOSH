#ifndef _SHMEM_GET_H
#define _SHMEM_GET_H

#include <stdlib.h>
#include <cstddef> // ptrdiff_t
#include <cstdint> // *intptr_t

template<class T> T shmem_template_g( T*, int );
template<class T> void shmem_template_get( T*, const T*, size_t, int);
template<class T> void shmem_template_iget( T*, const T*,  ptrdiff_t, ptrdiff_t, size_t, int );

void shmem_intptr_get( intptr_t*, const intptr_t*, size_t, int );
void shmem_uintptr_get( uintptr_t*, const uintptr_t*, size_t, int );


#include "shmem_get.tpp"

#endif
