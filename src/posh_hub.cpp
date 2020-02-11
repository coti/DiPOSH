/*
 * Copyright (c) 2020 LIPN - Universite Paris 13
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

#include "posh_hub.h"

void Communication_hub_t::init( int rank ) {
    this->rank = rank;

    /* Initialize a shared heap */
    
    char* _name; 
    _name = myHeap.buildHeapName( rank );
    while( false == _sharedMemEsists( _name ) ) {
        usleep( SPIN_TIMER );
    }
    managed_shared_memory __heap(  open_only, _name );
    this->remoteHeap = std::move( __heap );
    free( _name );
    //        shmem_sm_init(  );

    /* Initialize a local communicator */

    std::hash<std::string> hash_fn;
    size_t str_hash = hash_fn( mpi::processor_name() );

    int color = str_hash;
    std::cout << color << std::endl;
    
}
