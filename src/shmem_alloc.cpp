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

#include "shmem.h"
#include "shmem_internal.h"

#include "shmem_tcp.h"
#include "posh_heap.h"

bool checkSize( size_t );


/* This routine allocates a block of memory in the symmetric heap
 * of the calling PE
 * * All PEs must call this routine at the same point of the execution 
 * path; otherwise, undefined behavior results.
 * * All PEs must call this routine with the same size value; otherwise,
 * undeﬁned behavior results.
 * * The parameter size must be less than or equal to the amount of
 * symmetric heap space available for the calling PE; otherwise shmalloc
 * returns NULL.
 * The shmalloc() routine allocates a block of memory of at least size
 * bytes from the symmetric heap of the calling PE, and returns a pointer
 * to the allocated block.
 * shmalloc() calls shmem_barrier_all() before returning to ensure that
 * all the PEs participate. This guarantees symmetric allocation, and that
 * the memory on all the PEs is available to the other PEs.
 * The shmalloc() routine returns a pointer to the allocated block;
 * otherwise a null pointer is returned if no block could be allocated.
 */

void *shmalloc( size_t size ){
    if( ! checkSize( size ) ) return NULL;
    void* ptr = NULL;
    try { 
        ptr = myHeap.myHeap.allocate( size );
    } catch (boost::interprocess::bad_alloc &ex) {
        std::cerr << ex.what() << std::endl; 
    } 
    shmem_barrier_all();
    return ptr;
}

/* This routine allocates a block from the symmetric heap with a byte
 * alignment speciﬁed by the programmer.
 * * Values for parameters alignment and size must be positive integer values.
 * * alignment is a power of 2; power >= 3.
 * * The parameter size must be less than or equal to the amount of 
 * symmetric heap space available for the calling PE; otherwise shmemalign
 * returns NULL
 * The shmemalign() routine allocates a memory block of size bytes in the
 * symmetric heap, with an alignment of alignment bytes.
 * shmemalign() calls shmem_barrier_all() before returning to ensure that
 * all the PEs participate. This guarantees symmetric allocation, and that
 * the target memory on all the PEs is available to the other PEs.
 * This routine returns a pointer to the allocated block of memory;
 * otherwise a null pointer is returned.
*/

void *shmemalign( size_t alignment, size_t size ){
    if( ! checkSize( size ) ) return NULL;
    void* ptr = NULL;
    try { 
        ptr = myHeap.myHeap.allocate_aligned( size, alignment );
    } catch (boost::interprocess::bad_alloc &ex) {
        std::cerr << ex.what() << std::endl; 
    } 
    shmem_barrier_all();
    return ptr;
}

/* This routine expands or reduces the size of the block to which ptr
 * points, depending on the provided size parameter.
 * * All PEs must call this routine at the same point of the execution
 * path; otherwise, undeﬁned behavior results.
 * * All PEs must call this routine with the same parameters; otherwise
 * different symmetric heap addresses may be returned to each PE.
 * * The parameter size must be an integer greater than or equal to zero.
 * * The parameter size must be lesser than or equal to the amount of
 * symmetric heap space available for the calling PE; otherwise shrealloc
 * returns NULL.
 * The shrealloc() routine changes the size of the memory block to which
 * ptr points to, and returns a pointer to the memory block. The contents
 * of the memory block are preserved up to the lesser of the new and old
 * sizes. If the new size is larger, the value of the newly allocated
 * portion of the block is indeterminate. In case shrealloc() is unable
 * to extend the size of the memory block at its current location, the
 * routine may move the block elsewhere in the symmetric heap while
 * ensuring that contents of the block are preserved. In case of
 * relocation, shrealloc() will return a pointer to the new location.
 * If ptr is null, shrealloc() behaves exactly like shmalloc(). If size
 * is 0 and ptr is not null, the memory block is deallocated. Otherwise,
 * if ptr does not match a pointer earlier returned by a symmetric heap
 * function, or if the space has already been deallocated, shrealloc() will
 * return a null pointer. If the space cannot be allocated, the block to
 * which ptr points to is unchanged.
 * shrealloc() calls shmem_barrier_all() before returning to ensure that
 * all the PEs participate. This guarantees symmetric allocation, and that
 * the target memory on all the PEs is available to the other PEs.
 */

void *shrealloc( void* ptr, size_t size ){
    if( ! checkSize( size ) ) return NULL;
    if( size < 0 ) {
        std::cerr << "shrealloc: the size parameter must be greater or equal to 0" << std::endl;
        return NULL;
    }

    /* TODO */
    shmem_barrier_all();
    return NULL;
}

/* Frees a memory block previously allocated in the symmetric heap
 * This routine causes the block to which ptr points to, to be
 * deallocated; that is, made available for further allocation.
 * If ptr is a null pointer, no action occurs; otherwise, if the
 * argument does not match a pointer earlier returned by a symmetric
 * heap function, or if the space has already been deallocated, shfree()
 * returns.
 * shfree() calls shmem_barrier_all() before returning to ensure that all
 * the PEs participate. This guarantees symmetric deallocation within the
 * heap.
 */

void shfree( void* ptr ){

    if( NULL != ptr ) {
        try{
            myHeap.myHeap.deallocate( ptr );
        } catch( int e ) {
            ;
        }
    }

    shmem_barrier_all();
}




/* internal */

bool checkSize( size_t size ) {
    managed_shared_memory::size_type free_memory = myHeap.myHeap.get_free_memory();
    if( free_memory < size ) {
        std::cerr << "Warning: could not allocate all the requested space -- " << size << " Bytes asked, " << free_memory << " Bytes left" << std::endl;
        return false;
    }
    return true;
}

/* Fake shmalloc, for internal use only.
   Allocates space in the symmetric heap without calling shmem_barrier_all 
   at the end.

   FIXME: should get away
*/

void* _shmallocFake( size_t size ){
    if( ! checkSize( size ) ) return NULL;
    void* ptr = NULL;
    try { 
        ptr = myHeap.myHeap.allocate( size );
    } catch (boost::interprocess::bad_alloc &ex) {
        std::cerr << ex.what() << std::endl; 
    } 
    return ptr;
}

#if 0 // TODO degager
void* _remote_shmallocFake( int pe, size_t size ){
    if( ! checkSize( size ) ) return NULL;
    void* ptr = NULL;
    neighbor_comm_type_t neighbor_type = _getNeighborComm( pe );
    if( _shmem_likely( TYPE_SM == neighbor_type ) ) {
        try { 
            managed_shared_memory* remoteHeap;
            remoteHeap = &(myInfo.getNeighbor( pe )->comm_channel.sm_channel.shared_mem_segment);
            
            ptr = remoteHeap->allocate( size );
        } catch (boost::interprocess::bad_alloc &ex) {
            std::cerr << ex.what() << std::endl; 
        } 
    } else {
#ifdef DISTRIBUTED_POSH
#ifdef TCPCHANNEL
        size_t offset = shmem_remote_fake_shmalloc_tcp( pe, size );
        ptr = _getLocalAddr( offset );
#endif // TCPCHANNEL
        
#else
        std::cerr << "Non-distributed POSH without shared memory? I cannot put anything." << std::endl;
#endif // DISTRIBUTED
    }
    return ptr;
}

#endif
