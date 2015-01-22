#ifndef _SHMEM_COLLECTIVES_H
#define _SHMEM_COLLECTIVES_H

template<class T>void shmem_template_allgather_flat( T*, const T*, size_t );
template<class T>void shmem_template_gather_flat( T*, const T*, size_t, int );

void shmem_int_allgather_flat( int*, const int*, size_t );
void shmem_int_gather_flat( int*, const int*, size_t, int );

#include "shmem_collectives.tpp"

#endif
