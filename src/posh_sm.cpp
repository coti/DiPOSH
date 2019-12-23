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

#include "shmem_internal.h"
#include "posh_sm.h"
#include "posh_heap.h"

int shmem_sm_put( int pe, void* target, const void* source, size_t size ) {
    void* buf =  _getRemoteAddr( target, pe );
    _shmem_memcpy( buf,  source, size );
}

int shmem_sm_get( int pe, void* target, const void* source, size_t size ) {
    /* Open remote PE's symmetric heap */
    const void* buf = const_cast<const void*>(_getRemoteAddr( source, pe ) );
    /* Pull the data here */
    _shmem_memcpy( reinterpret_cast<void *>( target ), buf, size );
}


void Communication_SM_t::init( int rank ) {
    this->rank = rank;
    char* _name; 
    _name = myHeap.buildHeapName( rank );
    while( false == _sharedMemEsists( _name ) ) {
        usleep( SPIN_TIMER );
    }
    managed_shared_memory __heap(  open_only, _name );
    this->remoteHeap = std::move( __heap );
    free( _name );
    //        shmem_sm_init(  );
}
