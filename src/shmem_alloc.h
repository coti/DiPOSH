#ifndef _SHMEM_ALLOC_H_
#define _SHMEM_ALLOC_H_
#ifndef _SHMEM_H

void *shmalloc( size_t );
void *shmemalign( size_t, size_t );
void *shrealloc( void*, size_t );
void shfree( void* );

#endif
#endif
