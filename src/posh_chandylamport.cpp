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

void shmem_barrier_all( void );

void Checkpointing::initMemName(){
    asprintf( &( this->markermemname ), "%s_%d", this->markerbasename.c_str(), myInfo.getRank() );
}

/* Initialize the local marker, including creation of the segment of shared
   memory that will contain it. 
*/

/* FIXME: a lot of this code can be factorized with the creation of the shared heap */

void Checkpointing::initMarker(){

    int shmsize = std::max( sizeof( atomic<int> ), static_cast<long unsigned int>( POSH_MIN_SHM_SIZE ) );
    
    /* Open (and create) the segment of shared memory */

    this->initMemName();
    shared_memory_object::remove( this->markermemname );
    this->markermem = managed_shared_memory ( open_or_create, this->markermemname, shmsize );

    /* My marker is contained in the segment */
    
    this->marker = this->markermem.find_or_construct<std::atomic<int>>( "marker" )(0);
    *(this->marker) = 0;
}

/* Access the value of my local marker */

int Checkpointing::getMarker(){
    return *(this->marker);
}

/* Reset the value held by my local marker counter (0) */

void Checkpointing::resetMarker(){
    *(this->marker) = 0;
}

/* Increment a process's marker */

void Checkpointing::sendMarker( int rr ){
    char* name;
    asprintf( &name,  "%s_%d", this->markerbasename.c_str(), rr );
    managed_shared_memory remote( open_only, name );
    
    std::pair<std::atomic<int>*, std::size_t> value = remote.find<std::atomic<int>>( "marker" );
    std::atomic<int>* rmark = value.first;
    (*rmark)++;
    
    
    free( name );
}
 
/* Coordinate the processes */

void Checkpointing::coordinate( ){
#ifdef CL_MARKER
    /* Circulate the marker */
    for( int i = 0 ; i < myInfo.getSize() ; i++ ){
        if( i != myInfo.getRank() ) {
            checkpointing.sendMarker( i );
        }
    }
    while( myInfo.getSize() - 1 != checkpointing.getMarker() ){
        usleep( 10 );
    }
#endif // CL_MARKER
#ifdef CL_DOUBLEBARRIER
    shmem_barrier_all();
    shmem_barrier_all();
#endif // CL_DOUBLEBARRIER
}

 
/* I received a marker: the communication wave has been triggered
   by a communication.
*/

void Checkpointing::recvMarker( ){
    if( !myInfo.getCheckpointingState() ) {
        myInfo.enterCheckpointing();
    }
    this->coordinate();
    
    // Else: ignore. It means that we are already checkpointing and we are receiving a marker.... Should not arrive (I need to thing about it again) TODO
}
