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

/* FIXME need the other implementations */

#include "posh_heap.h"

template <class T, class V>void posh_broadcast_template_binomial( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    posh_broadcast_template_binomial_put_blocking( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );


}



template <class T,class V>void posh_broadcast_template_binomial_put_blocking( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    /* TODO : mettre des locks */

    int myrank = myInfo.getRank();
    int size = myInfo.getSize();
    bool rc;
    int child;

    /* Am I part of the subset of participating processes ? */
    
    if(  _shmem_unlikely( _shmem_part_of_coll( myrank, PE_start, PE_size, logPE_stride ) )) {
        return;
    }

    Collective_t* coll = myInfo.getCollective();
    myInfo.getCollective()->cnt = 0;
    myInfo.getCollective()->inProgress = true;

    /* Am I the root process? */

    if( _shmem_unlikely( myrank == PE_root ) ) {
        _shmem_memcpy( target, source, nelems*sizeof( T ) );
        myInfo.getCollective()->cnt ++;
    } 

    /* Wait until the data is available in my local target buffer */

    while( myInfo.getCollective()->cnt != 1 ) {
        usleep( SPIN_TIMER );
    }

    /* Compute my children's ranks */

    int translated_rank = ( myrank - PE_start ) / ( 0x1 << logPE_stride );
    int nb_of_children = _shmem_binomial_children( translated_rank, PE_size );
    int mychild;

    int offset = _shmem_binomial_binbase_size( translated_rank );
    for( child = 0 ; child < nb_of_children ; child++ ) {
        mychild = translated_rank | ( 0x1 << ( offset++ ) ) ;
        mychild = mychild * 0x1 << ( logPE_stride + PE_start);

        /* Put the data and increment their local counter */
        shmem_template_put( target, source, nelems, mychild );
        shmem_int_finc( &(coll->cnt), mychild );

    }

    myInfo.getCollective()->inProgress = false;
    myInfo.getCollective()->cnt = 0;

}


/* Reduction */

template <class T, class V>void shmem_template_binarytree_reduce( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, V* pWrk, long* pSync,  void(* operation)( T*, T*, T* ) ) {

    // TODO : prendre en compte le stride

    int myRank = myInfo.getRank();
    int size = myInfo.getSize();
    bool rc; 

    Collective_t* coll = myInfo.getCollective();
    coll->type = _SHMEM_COLL_REDUCE;

#define ROOT PE_start

    int myBuddy, nblevel, toto, level;

    char* mutName;
    asprintf( &mutName, "mtx_shmem_reduce_%d", myRank );
    boost::interprocess::named_mutex named_mtx( boost::interprocess::open_or_create, mutName ); 

#ifdef _DEBUG
    std::cout << "Reduce : rank [" << myRank << "] has entered reduce" << std::endl;
#endif

    /* compute the number of level we will need (from Julien) */
    nblevel = 1;
    toto = 1;
    while ( size > toto ) { toto = toto*2; nblevel++; }

    while( false == ( rc = named_mtx.try_lock() ) ) {
#ifdef _DEBUG
        std::cout << "[" << myRank << "] lock " << mutName  << " is locked line " << __LINE__ << std::endl;
#endif
        usleep( SPIN_TIMER );
    }
    //    named_mtx.lock(); 

    if( false == coll->inProgress ) {
        
        /* Nobody has pushed any data inside of me yet */
        coll->ptr = target; // _shmallocFake( nelems * sizeof( T ) );
        coll->cnt = 0;
        coll->inProgress = true;
        coll->type = _SHMEM_COLL_REDUCE;
#if defined( _SAFE ) || defined( _DEBUG )
        coll->space = nelems * sizeof( T );
#endif
    } else { 
        /* the data has already been placed into some temp'ly allocated space */
        void* ptr = coll->ptr;
        coll->ptr = target; 
        _shmem_memcpy( coll->ptr, ptr, nelems * sizeof( T ) );
        free( ptr );
#if defined( _SAFE ) || defined( _DEBUG )
        if( coll->space != nelems * sizeof( T ) ) {
            std::cerr << "Wrong size in the collective buffer in process rank " << myInfo.getRank() << ":" << std::endl;
            std::cerr << "Expecting " <<  nelems * sizeof( T ) << " bytes, found " << coll->space << " bytes" << std::endl;
        }
        if( _SHMEM_COLL_REDUCE != coll->type ) {
            std::cerr << "Wrong collective type: process rank " << myInfo.getRank();
            std::cerr << " attempted to participate to a reduce operation ";
            std::cerr << "( code " << _SHMEM_COLL_REDUCE <<" ) whereas another ";
            std::cerr << "collective operation is in progress (code ";
            std::cerr << coll->type << ")" << std::endl;
            return;
        }
#endif
    }
    
    named_mtx.unlock();

    toto = 1;
    for ( level = 1 ; level < nblevel ; level++ ) {
        std::cout << myRank << "] level " << level << std::endl;
        if ( myRank % toto == 0 ) {   
            if( myRank % ( toto * 2 ) == 0 ){
                if ( myRank + toto < size ) {        
                    myBuddy = myRank + toto;
                    std::cout << myRank << "] my buddy " << myBuddy << " will write inside of me" << std::endl;

                    while( true ) {
                        usleep( SPIN_TIMER );
                        named_mtx.lock(); 
                        if( 1 == coll->cnt ) {
                            break;
                        }
                        named_mtx.unlock(); 
                    } 

                    coll->cnt = 0;                   
                    operation( target, target,  (T*)(coll->ptr) );
                    named_mtx.unlock(); 
    
#ifdef _DEBUG
        std::cout << myInfo.getRank() << "] recv Reduce buff" <<  std::endl;
#endif
                }
            } else {
                myBuddy = myRank - toto; 
#ifdef _DEBUG
                std::cout << myRank << "] write inside of my buddy " << myBuddy << std::endl;
#endif

                Collective_t* remote_coll = static_cast<Collective_t *>( _getRemoteAddr( coll, myBuddy ) );

                char* remMutName;
                asprintf( &remMutName, "mtx_shmem_reduce_%d", myBuddy );
                boost::interprocess::named_mutex remote_mtx( boost::interprocess::open_or_create, remMutName ); 

#if defined( _SAFE ) || defined( _DEBUG )
                
                /* Check if the collective that is already in progress 
                   is matching the current collective type 
                */
                
                if( !( _SHMEM_COLL_REDUCE == remote_coll->type ) && !( _SHMEM_COLL_NONE == remote_coll->type )  ) {
                    std::cerr << "Wrong collective type: process rank " << myInfo.getRank();
                    std::cerr << " attempted to participate to a reduce operation ";
                    std::cerr << "( code " << _SHMEM_COLL_REDUCE <<" ) whereas another ";
                    std::cerr << "collective operation is in progress (code ";
                    std::cerr << remote_coll->type << ")" << std::endl;
                    return;
                }
#endif
                
                /* Is the remote guy ready? */
                //                remote_mtx.lock(); 
                while( false == ( rc = remote_mtx.try_lock() ) ) {
#ifdef _DEBUG
                    std::cout << "[" << myRank << "] lock " << remMutName  << " is locked line " << __LINE__ << std::endl;
#endif
                    usleep( SPIN_TIMER );
                }
#if 0 // should not happen
                if( false == remote_coll->inProgress ) {
                    remote_coll->ptr = _remote_shmallocFake( myBuddy, nelems * sizeof( T ) );
                    remote_coll->type = _SHMEM_COLL_REDUCE;
                    remote_coll->cnt = 0;
                    remote_coll->inProgress = true;
#if defined( _SAFE ) || defined( _DEBUG )
                    remote_coll->space =  nelems * sizeof( T );
#endif
                }
#endif

                /* has anyone written over there already? */
                int cnt;
                while( true ) {
                    cnt = shmem_int_g( &(coll->cnt), myBuddy );
                    if( cnt == 0 ) {
                        break;
                    }
                    
                    remote_mtx.unlock(); 
                    usleep( SPIN_TIMER );
                    remote_mtx.lock(); 
                }
                                              
                shmem_template_finc( &(coll->cnt), myBuddy );
                shmem_template_put( target, source, nelems, myBuddy );

                remote_mtx.unlock(); 

#ifdef _DEBUG
                std::cout << myInfo.getRank() << "] put buff into " << myBuddy << " ; " << nelems << " elements" << std::endl;
#endif
                free( remMutName );

            } /* end else ( myRank % ( toto * 2 ) == 0 ) -> I am the reader/writer */
        }
        toto = toto*2;
    }

#ifdef _DEBUG
    std::cout << myInfo.getRank() << "] Reduction tree done" << std::endl;
    if( myInfo.getRank() == ROOT ) std::cout << "result: " << *target << std::endl;
#endif
    free( mutName );

    myInfo.collectiveReset();
}

