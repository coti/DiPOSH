/*
 *
 * Copyright (c) 2014 LIPN - Universite Paris 13
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
 *
 */

#include "shmem_internal.h"

/* Accessors */

int MeMyselfAndI::getRank(){
    return this->shmem_rank;
}

void MeMyselfAndI::setRank( int _rank ){
    this->shmem_rank = _rank;
}

int MeMyselfAndI::getSize(){
    return this->shmem_size;
}

void MeMyselfAndI::setSize( int _size ){
    this->shmem_size = _size;
}

bool MeMyselfAndI::getStarted(){
    return this->shmem_started;
}

void MeMyselfAndI::setStarted( bool _started ){
    this->shmem_started = _started;
}

managed_shared_memory* MeMyselfAndI::getNeighbors() {
    return this->neighbors;
}

managed_shared_memory* MeMyselfAndI::getNeighbor( int rank ) {
    return &(this->neighbors[rank]);
}

boost::interprocess::named_mutex* MeMyselfAndI::getLock( long _lock ){
    boost::interprocess::named_mutex* mut =  this->locks[ _lock ];
    if( _shmem_unlikely( NULL == mut ) ) {
        char* name;
        asprintf( &name, "shmem_lock_%ld", _lock );
        boost::interprocess::named_mutex mtx(boost::interprocess::open_or_create, name );
        free( name );
        mut = &mtx;
        this->setLock( _lock, &mtx );
    }
    return mut;
}
void MeMyselfAndI::setLock( long _lock, boost::interprocess::named_mutex *_mtx ){
    this->locks[ _lock ] = _mtx;
}

/* Methods */

void MeMyselfAndI::findAndSetMyRank( ){
    if( -1 != this->shmem_rank ) {
        return;
    } else {
        char* myNum;
        myNum = getenv( VAR_PE_NUM );
        if( NULL == myNum ) {
            std::cerr << "Could not retrieve my PE number" << std::endl;
            return;
        }
        this->setRank( atoi( myNum ) );
    }
}

void MeMyselfAndI::findAndSetMySize () {
    if( -1 != this->shmem_size ) {
        return;
    } else {
        char* numPE;
        numPE = getenv( VAR_NUM_PE );
        if( NULL == numPE ) {
            std::cerr << "Could not retrieve the number of PEs in the system" << std::endl;
            return;
        }
        this->setSize( atoi( numPE ) );
    }
}

void MeMyselfAndI::findAndSetPidRoot () {
    char* rootpid;
    if( 0 == myInfo.getRank() ) {
        asprintf( &rootpid, "%d", getpid() );
    } else {
        rootpid = getenv( VAR_PID_ROOT );
        if( NULL == rootpid ) {
            std::cerr << "Could not retrieve the process id of the root process" << std::endl;
            return;
        }
    }

    this->setRootPid( atoi( rootpid ) );
#if _DEBUG
    std::cout << myInfo.getRank() << "] my father has pid " << atoi( rootpid ) << std::endl;
#endif

    if( 0 == myInfo.getRank() ) {
        free( rootpid );
    }
}

void MeMyselfAndI::setRootPid( int pid ) {
    Process pRoot( 0, pid );
    processes.push_back( pRoot );
}


void MeMyselfAndI::allocNeighbors( int nb ) {
    this->neighbors = new managed_shared_memory[nb];
    char* _name;
    for( int i = 0 ; i < nb ; i++ ) {
        _name = myHeap.buildHeapName( i );
        while( false == _sharedMemEsists( _name ) ) {
            usleep( SPIN_TIMER );
        }
        managed_shared_memory remoteHeap(  open_only, _name );
        this->neighbors[i] = std::move( remoteHeap );

        //        this->neighbors[i]( open_only, _name );
        free( _name );
    }
}

/* How I see the remote guy's base address */

void* MeMyselfAndI::getRemoteHeapLocalBaseAddr( int rank ) {
    managed_shared_memory* myNeighbor = this->getNeighbor( rank );
    managed_shared_memory::handle_t handle = myHeap.getOffsetHandle();
    return myNeighbor->get_address_from_handle( handle );
}

/* How the remote guy sees his own base address */

void* MeMyselfAndI::getRemoteHeapBaseAddr( int rank ) {
    uintptr_t* ptr = (uintptr_t*)this->getRemoteHeapLocalBaseAddr( rank );
    return (void*)(*ptr);
}
