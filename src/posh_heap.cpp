/*
 * Copyright (c) 2020      LIPN - Universite Sorbonne Paris Nord
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

#include <cstddef>

#include "shmem_internal.h"
#include "posh_heap.h"
#include "shmem.h" // for shmem_barrier

void* SymmetricHeap::heapAlloc( size_t size ){
    if( ! checkSize( size ) ) return NULL;
    void* ptr = NULL;
    try { 
        ptr = this->myHeap.allocate( size );
    } catch (boost::interprocess::bad_alloc &ex) {
        std::cerr << ex.what() << std::endl; 
    } 
    return ptr;
}

void* SymmetricHeap::shmemalign( size_t alignment, size_t size ){
    if( ! checkSize( size ) ) return NULL;
    void* ptr = NULL;
    try { 
        ptr = this->myHeap.allocate_aligned( size, alignment );
    } catch (boost::interprocess::bad_alloc &ex) {
        std::cerr << ex.what() << std::endl; 
    } 
    return ptr;
}

void* SymmetricHeap::shrealloc( void* ptr, size_t size ){
    if( ! checkSize( size ) ) return NULL;
    if( size < 0 ) {
        std::cerr << "shrealloc: the size parameter must be greater or equal to 0" << std::endl;
        return NULL;
    }

    /* TODO */
    return NULL;
}

void SymmetricHeap::shfree( void* ptr ){

    if( NULL != ptr ) {
        try{
            this->myHeap.deallocate( ptr );
        } catch( int e ) {
            ;
        }
    }

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

