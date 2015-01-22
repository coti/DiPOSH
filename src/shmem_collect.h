#ifndef _SHMEM_COLLECT_H
#define _SHMEM_COLLECT_H

template <class T, class V> void shmem_template_collect( T*, const T*, size_t, int, int, int, V* );
template <class T, class V> void shmem_template_fcollect( T*, const T*, size_t, int, int, int, V* );

#include "shmem_collect.tpp"

#endif
