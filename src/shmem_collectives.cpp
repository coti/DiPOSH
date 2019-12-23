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

#include "shmem_collectives.h"


void MeMyselfAndI::setCollectiveType( unsigned char _type ) {
    this->collective->type = _type;
}

unsigned char MeMyselfAndI::getCollectiveType( ) {
    return this->collective->type;
}

void* MeMyselfAndI::allocCollectiveSpace( size_t _size ){
    this->collective->ptr = _shmallocFake( _size );
#if _DEBUG 
    this->collective->space = _size;
#endif
    return this->collective->ptr;
}

void* MeMyselfAndI::getCollectiveBuffer( ) {
    return this->collective->ptr;
}

void MeMyselfAndI::collectiveInit() {
    this->collective = (Collective_t*) _shmallocFake( sizeof( Collective_t ) );
    this->collective->ptr  = NULL;
    this->collective->cnt  = 0;
    this->collective->type = _SHMEM_COLL_NONE;
    this->collective->inProgress = false;
#if _DEBUG 
    this->collective->space = -1;
#endif

    char* mutName;
    if( myInfo.getRank() == 0 ) {
        asprintf( &mutName, "mtx_shmem_gather_root" );
        boost::interprocess::named_mutex::remove( mutName ); // remove any pre-existent mutex
        free( mutName );

        asprintf( &mutName, "shmem_barrier_ROOT" );
        boost::interprocess::named_mutex::remove( mutName ); // remove any pre-existent mutex
        free( mutName );


    }

    asprintf( &mutName, "mtx_shmem_bcast_%d", myInfo.getRank() );
    boost::interprocess::named_mutex::remove( mutName ); // remove any pre-existent mutex
    free( mutName );


}

void MeMyselfAndI::collectiveReset() {
    this->collective->cnt  = 0;
    this->collective->type = _SHMEM_COLL_NONE;
    this->collective->inProgress = false;
#if _DEBUG 
    this->collective->space = -1;
#endif
}

Collective_t*  MeMyselfAndI::getCollective(){
    return this->collective;
}



/** internal functions **/

extern std::vector<Process> processes;


/* "bad" allgather: all the processes send their buffer to the root process,
 * then the aforementionned root process broadcasts the resulting buffer
 * to all the other processes.
 * - buffer is the resulting buffer, allocated in the symmetric heap
 * - myelem is the local buffer that will be put into the resulting buffer
 * - nb is the number of elements in each local buffer
 */

void shmem_int_allgather_flat( int* buffer, const int* myelem, size_t nb ) {
    shmem_template_allgather_flat( buffer, myelem, nb );
}

void shmem_int_gather_flat( int* buffer, const int* myelem, size_t nb, int root ) {
    shmem_template_gather_flat( buffer, myelem, nb, root );
}

bool _shmem_is_remote_process_in_coll( int rank ) {
    return shmem_template_g( &(myInfo.getCollective()->inProgress), rank );
}

void _shmem_wait_until_entered( int rank ) {

    while( false == _shmem_is_remote_process_in_coll( rank ) ){
        usleep( SPIN_TIMER );
    }
}

bool _shmem_part_of_coll( int rank, int PE_start, int PE_size, int logPE_stride ) {
    return ( ( rank < PE_start ) ||
             ( rank > ( PE_start + ( PE_size * ( 0x1 << logPE_stride) ) ) ) ||
             ( ( rank - PE_start ) % ( 0x1 << logPE_stride ) ) != 0 );
}

/* Returns the length of the prefix
 * e.g., if nb = 0x001101, returns 4
 */

int _shmem_binomial_binbase_size( int nb ) {
    int len;
    int b = 0x1;

    len = 0;
    while( b <= nb ) {
        b = b << 1;
        len++;
    }
    return len;
}

/* Father's rank
 * My own rank minus the heaviest bit.
 */

int _shmem_binomial_myfather( int PE_root, int PE_start, int logPE_stride, int PE_size ) {
    int rank = myInfo.getRank();
    rank -= PE_start;
    int log = _shmem_binomial_binbase_size( rank );
    int b = 0x1 << (log-1);
    int father = rank ^ b;
    father = rank - ( ( rank - father ) * 0x1 << logPE_stride );
    return father;
}

/* Number of children
 */

int _shmem_binomial_children( int rank, int size ) {
    int log_rank = _shmem_binomial_binbase_size( rank );

    int n = 0;
    int child = rank;
    while( child < size ) {
        child = rank | ( 0x1 << ( log_rank + n++ ) ) ;
    }
    return n - 1;
}


