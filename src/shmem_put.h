#ifndef _SHMEM_PUT_H
#define _SHMEM_PUT_H

#include <stdlib.h>
#include <cstddef> // ptrdiff_t

template<class T> void shmem_template_p( T*, T, int );
template<class T> void shmem_template_put( T*, const T*, size_t, int );
template<class T> void shmem_template_iput( T*, const T*, ptrdiff_t, ptrdiff_t, size_t, int );

#include "shmem_put.tpp"

#endif
