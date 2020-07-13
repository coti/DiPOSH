/*
 * Copyright (c) 2014-2020 LIPN - Universite Sorbonne Paris Nord
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
#include "shmem.h"
#include "shmem_put.h"
#include "shmem_get.h"
#include "shmem_collectives.h"

/* FIXME need the other implementations */

#include "posh_heap.h"


template <class T,class V>void posh_broadcast_template_flat_put_blocking( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync );
template <class T,class V>void posh_broadcast_template_flat_get_blocking( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync );



template <class T, class V>void posh_broadcast_template_flat( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    //    shmem_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    //    shmem_broadcast_template_flat_get_blocking( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    posh_broadcast_template_flat_put_blocking( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );

}

/* Various implementations */

/* The difference between the blocking and non-blocking implementations 
   comes when a process has to write into the memory of a remote process 
   and this latter process has not entered the collective commnucation yet.
   - in the non-blocking case, the former performs a remote mem allocation
   and writes, hence anticipating the moment when the remote process will
   enter the call;
   - in the blocking case, it waits until the remote process is ready.
   Blocking implementations are much simpler.
 */

template <class T,class V>void posh_broadcast_template_flat_put_blocking( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    int myrank = myInfo.getRank();
    int size = myInfo.getSize();
    bool rc;

    Collective_t* coll = myInfo.getCollective();

    myInfo.getCollective()->inProgress = true;

    if( _shmem_unlikely( myrank == PE_root ) ) {
        /* I am the root of the broadcast. Write... */

        int remoteRank, nb;
        nb = 0;
        remoteRank = PE_start;

        while( nb < PE_size ) {

            _shmem_wait_until_entered( remoteRank );

            /* TODO: Possible optimization: write everywhere it is possible, and put
               the ranks of those that are not ready in a vector. Then loop on this vector
               until it is empty.
             */

            if( _shmem_likely( remoteRank != myrank ) ) {
                shmem_template_put( target, source, nelems, remoteRank );
                shmem_int_finc( &(coll->cnt), remoteRank );
            } else {
                _shmem_memcpy( target, source, nelems*sizeof( T ) );
            }

            nb++;

            remoteRank += ( 0x01 << logPE_stride );
            if( _shmem_unlikely( remoteRank >= size ) ) {
                remoteRank = remoteRank % size;
            }
        }
        
    } else {
        
        /* All I have to do is wait until the root has written into my memory */
         /* Am I part of the subset of participating processes ? */

        if( _shmem_unlikely( _shmem_part_of_coll( myrank, PE_start, PE_size, logPE_stride ) ) ){
            return;
        }

       while( myInfo.getCollective()->cnt != 1 ) {
            usleep( SPIN_TIMER );
        }
        
    }
    myInfo.getCollective()->inProgress = false;
}

template <class T,class V>void posh_broadcast_template_flat_get_blocking( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    int myrank = myInfo.getRank();
    int size = myInfo.getSize();
    bool rc;

    Collective_t* coll = myInfo.getCollective();
    coll->inProgress = true;
    coll->cnt = 0;

    if( _shmem_unlikely( myrank == PE_root ) ) {

        /* I am the root of the broadcast. Wait until everyone has fetched data */

        _shmem_memcpy( target, source, nelems*sizeof( T ) );

        while( myInfo.getCollective()->cnt != (PE_size - 1) ) {
            usleep( SPIN_TIMER );
        }

    } else {

        /* Am I part of the subset of participating processes ? */

        if(  _shmem_unlikely( _shmem_part_of_coll( myrank, PE_start, PE_size, logPE_stride ) )) {
            return;
        }

        /* Is the root process ready yet? */

        _shmem_wait_until_entered( PE_root );

        /* Get data from the root */

        shmem_template_get( target, source, nelems, PE_root );
        shmem_int_finc( &(coll->cnt), PE_root );
        
    }

    coll->inProgress = true;
    coll->cnt = 0;

}

/* Reductions */

template <class T, class V>void shmem_template_flat_reduce_put( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, V* pWrk, long* pSync,  void(* operation)( T*, T*, T* ) ) {

    // TODO : prendre en compte le stride

    int myRank = myInfo.getRank();
    int size = myInfo.getSize();
    bool rc;
    int ROOT = 0;

    Collective_t* coll = myInfo.getCollective();
    coll->type = _SHMEM_COLL_REDUCE;

    int myBuddy, nblevel, toto, level;

    char* mutName;
    asprintf( &mutName, "mtx_shmem_reduce_%d", myRank );
    boost::interprocess::named_mutex named_mtx( boost::interprocess::open_or_create, mutName ); 

#ifdef _DEBUG
    std::cout << "Reduce : rank [" << myRank << "] has entered reduce" << std::endl;
#endif

    if( _shmem_unlikely( ROOT == myRank ) ) {

        /* Everybody will write into me */


    } else {


        /* TODO */


    } 



    std::cout << myInfo.getRank() << "] Flat reduction done" << std::endl;
    if( myInfo.getRank() == ROOT ) std::cout << "result: " << *target << std::endl;
    free( mutName );

    myInfo.collectiveReset();
}

template <class T> bool shmem_template_get_and_operation( T* target, const T* source, int nelems, int remoterank, T* local, void(* operation)( T*, T*, T* ) ) {

    Collective_t* coll = myInfo.getCollective();

   /* Try to fetch the data.
       If the remote guy is not ready yet, enqueue its rank in the vector
       and eventually poll the vector's guys */
    
    Collective_t* remote_coll = static_cast<Collective_t *>( _getRemoteAddr( coll, remoterank ) );
   /* Is the remote guy ready? */
    
    if( false == remote_coll->inProgress ) { /* no */
        return false;
    } else { /* yes */
        
        /* Fetch the data. */
        
        myInfo.getNeighbor( remoterank )->communications->posh__get( static_cast<void*>( local ), static_cast<const void*>( source ), nelems*sizeof( T ), remoterank );
        operation( target, target,  local );
    }
    
    /* increment the remote guy's counter */
    /* TODO atomic!! */
    
    //    myInfo.getNeighbor( remoterank )->communications->finc( &(coll->cnt), remoterank );
    
    shmem_int_finc( &(coll->cnt), remoterank );

    return true;
}

template <class T>void shmem_template_flat_reduce_get( T* target, const T* source, int nelems, void(* operation)( T*, T*, T* ) ) {

    int myRank = myInfo.getRank();
    int size = myInfo.getSize();
    bool rc;
    int ROOT = 0;

    Collective_t* coll = myInfo.getCollective();
    coll->type = _SHMEM_COLL_REDUCE;

    int myBuddy, nblevel, toto, level;

#ifdef _DEBUG
    std::cout << "Reduce : rank [" << myRank << "] has entered reduce" << std::endl;
#endif

    coll->ptr = target; // _shmallocFake( nelems * sizeof( T ) );
    coll->cnt = 0;
    coll->inProgress = true;
    coll->type = _SHMEM_COLL_REDUCE;
#if defined( _SAFE ) || defined( _DEBUG )
    coll->space = nelems * sizeof( T );
#endif

    if( _shmem_unlikely( ROOT == myRank ) ) {

        std::vector<int> ranks;
        T* local;

        local = (T*) malloc( nelems * sizeof( T ) );
        _shmem_memcpy( target, source, nelems*sizeof( T ) );

        /* I get data from everybody */

        for( int rr = 0 ; rr < myInfo.getSize() ; rr++ ) {

            if( _shmem_likely( myRank != rr ) ) {
                bool rc;
                rc = shmem_template_get_and_operation( target, source, nelems, rr, local, operation );
                if( false == rc ) {
                    ranks.push_back( rr );
                }
            }

        }

        /* Get the ones that were not ready */
        
        while( 0 > ranks.size() ) {
            
            for( std::vector<int>::iterator it = ranks.begin(); it != ranks.end(); it++ ) {
                bool rc;
                rc = shmem_template_get_and_operation( target, source, nelems, *it, local,  operation );
                if( true == rc ) {
                    ranks.erase( it );
#ifdef _DEBUG
                    std::cout << "Flat reduction: " << *it << " done. " << ranks.size() << " left." << std::endl;
#endif
                }
#ifdef _DEBUG
                else {
                    std::cout << "Flat reduction: " << *it << " will be tried again" << std::endl;
                }
#endif

            }

        }

        free( local );
    } else {
        while( 0 == coll->cnt ) {
            usleep( SPIN_TIMER );
        }
    }

    /* Now broadcast the result */

    myInfo.collectives->posh_broadcast( target, target, nelems, ROOT, 0, 0, 0, NULL );
    

#ifdef _DEBUG
    std::cout << myInfo.getRank() << "] Flat reduction done" << std::endl;
    if( myInfo.getRank() == ROOT ) std::cout << "result: " << *target << std::endl;
#endif
    
    myInfo.collectiveReset();
#ifdef _DEBUG
    std::cout << "[" << myRank << "] reset collective data structure" << std::endl;
#endif
}


