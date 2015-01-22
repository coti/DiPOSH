#ifndef _SHMEM_ATOMIC_H
#define _SHMEM_ATOMIC_H

template<class T> T shmem_template_swap( T*, T, int );
template<class T> T shmem_template_cswap( T*, T, T, int );
template<class T> T shmem_template_fadd( T*, T, int );
template<class T> T shmem_template_finc( T*, int );
template<class T> void shmem_template_add( T*, T, int );
template<class T> void shmem_template_inc( T*, int );

#include "shmem_atomic.tpp"

#endif
