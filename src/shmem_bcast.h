#ifndef _SHMEM_BCAST_H
#define _SHMEM_BCAST_H

#include <stdlib.h>

void shmem_int_bcast_flat( int*, const int*, size_t, int );

template <class T, class V>void shmem_broadcast_template( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync );

template <class T,class V>void shmem_broadcast_template_flat_put_blocking( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync );
template <class T,class V>void shmem_broadcast_template_flat_get_blocking( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync );


#include "shmem_bcast.tpp"

#endif
