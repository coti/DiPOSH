#ifndef _SHMEM_SYNC_H
#define _SHMEM_SYNC_H

template<class T> void shmem_template_wait( T*, T );
template<class T> void shmem_template_wait_until( T*, int, T );

#include "shmem_sync.tpp"

#endif
