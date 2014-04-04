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
#include "shmem.h"
#include "shmem_put.h"

template <class T, class V>void shmem_broadcast_template( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync ){

#ifdef SHMEM_COLL_FLAT
    shmem_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
#else
    #warning "No collective operation algorithm selected -- using flat by default"
    shmem_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
#endif

}


/* Various implementations */


template <class T,class V>void shmem_broadcast_template_flat( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    int myRank = myInfo.getRank();
    int size = myInfo.getSize();
    bool rc;

    Collective_t* coll = myInfo.getCollective();

    char* mutName;
    asprintf( &mutName, "mtx_shmem_bcast_%d", myRank );
    boost::interprocess::named_mutex named_mtx( boost::interprocess::open_or_create, mutName ); 

#ifdef _DEBUG
    std::cout << "Bcast : rank [" << myRank << "] has entered flat bcast" << std::endl;
#endif
    
    while( false == ( rc = named_mtx.try_lock() ) ) {
        usleep( SPIN_TIMER );
    }

    std::cout << "rank [" << myRank << "] locked" << std::endl;

    //        named_mtx.lock(); 
    if( false == coll->inProgress ) {
        /* Nobody has pushed any data inside of me yet */
        coll->cnt = 0;
        coll->inProgress = true;
        coll->type = _SHMEM_COLL_BCAST;
#if defined( _SAFE ) || defined( _DEBUG )
        coll->space = nelems * sizeof( T );
#endif
    } else { 
        /* the data has already been placed in the target buffer */
#if defined( _SAFE ) || defined( _DEBUG )
        // TODO : check que la taille est bonne
#endif
    }
    named_mtx.unlock(); 

    if( _shmem_unlikely( myRank == PE_root ) ) {
        int remoteRank, nb;
        nb = 0;
        remoteRank = PE_start;

        while( nb < PE_size ) {

            if( _shmem_likely( remoteRank != myRank ) ) {
                
                /* TODO : il va y avoir de la factorisation de code possible ici */
                
                Collective_t* remote_coll = static_cast<Collective_t *>( _getRemoteAddr( coll, remoteRank ) );
                
                char* remMutName;
                asprintf( &remMutName, "mtx_shmem_bcast_%d", remoteRank );
                boost::interprocess::named_mutex remote_mtx( boost::interprocess::open_or_create, remMutName ); 

#if defined( _SAFE ) || defined( _DEBUG )
                
                /* Check if the collective that is already in progress 
                   is matching the current collective type 
                */
                
                if( !( _SHMEM_COLL_BCAST == remote_coll->type ) && !( _SHMEM_COLL_NONE == remote_coll->type )  ) {
                    std::cerr << "Wrong collective type: process rank " << myInfo.getRank();
                    std::cerr << " attempted to participate to a Bcast operation ";
                    std::cerr << "( code " << _SHMEM_COLL_BCAST <<" ) whereas another ";
                    std::cerr << "collective operation is in progress (code ";
                    std::cerr << remote_coll->type << ")" << std::cerr;
                    return;
                }
#endif
                
                /* Is the remote guy ready? */
                while( false == ( rc = remote_mtx.try_lock() ) ) {
                    usleep( SPIN_TIMER );
                }
                if( false == remote_coll->inProgress ) {
                    remote_coll->type = _SHMEM_COLL_BCAST;
                    remote_coll->cnt = 0;
                    remote_coll->inProgress = true;
#if defined( _SAFE ) || defined( _DEBUG )
                    remote_coll->space =  nelems * sizeof( T );
#endif
                    
                }
                
                shmem_template_put( target, source, nelems, remoteRank );
                shmem_int_finc( &(myInfo.getCollective()->cnt), remoteRank );

                remote_mtx.unlock(); 


#ifdef _DEBUG
                std::cout << myInfo.getRank() << "] put buff into " << remoteRank << " ; " << nelems << " elements" << std::endl;
#endif
                free( remMutName );
            } else {
                if( _shmem_unlikely( target != source ) ) {
                    _shmem_memcpy( target, source, nelems*sizeof( T ) );
                }
            }
            nb++;

            remoteRank += ( 0x01 << logPE_stride );
            if( _shmem_unlikely( remoteRank >= size ) ) {
                remoteRank = remoteRank % size;
            }
        }

    } else { // TODO : prendre en compte le stride
        while( myInfo.getCollective()->cnt != 1 ) {
            usleep( SPIN_TIMER );
        }
        
        free( mutName );
    }
    std::cout << "Bcast : rank " << myRank << " done" << std::endl;

    myInfo.collectiveReset();
}


template <class T>void shmem_template_bcast_flat( T* target, const T* source, size_t nbElem, int root ) {

    int myRank = myInfo.getRank();
    Collective_t* coll;

    coll = myInfo.getCollective();
    coll->type = _SHMEM_COLL_BCAST;

    boost::interprocess::named_mutex named_mtx( boost::interprocess::open_or_create, "mtx_shmem_bcast"); 

    if( myRank == root ) {

        int remoteRank;
        
        for( std::vector<Process>::iterator it = processes.begin(); it != processes.end(); it++ ) {
            remoteRank = it->getRank() ;

            if( myRank != remoteRank ) {
                
                Collective_t* remote_coll = static_cast<Collective_t *>( _getRemoteAddr( coll, remoteRank ));
                
#if defined( _SAFE ) || defined( _DEBUG )
                
                /* Check if the collective that is already in progress 
                   is matching the current collective type 
                */

 
                if( !( _SHMEM_COLL_BCAST == remote_coll->type ) && !( _SHMEM_COLL_NONE == remote_coll->type ) ) {
                    std::cerr << "Wrong collective type: process rank " << myInfo.getRank();
                    std::cerr << " attempted to participate to a bcast operation ";
                    std::cerr << "( code " << _SHMEM_COLL_BCAST <<" ) whereas another ";
                    std::cerr << "collective operation is in progress (code ";
                    std::cerr << remote_coll->type << ")" << std::cerr;
                    return;
                }
#endif
                
                /* Is the remote guy ready? */

                named_mtx.lock(); 
                if( false == remote_coll->inProgress ) {
                    remote_coll->type = _SHMEM_COLL_BCAST;
                    remote_coll->cnt = 0;
                    remote_coll->inProgress = true;
#if defined( _SAFE ) || defined( _DEBUG )
                    remote_coll->space =  nbElem * sizeof( T );
#endif
                    
                }
                named_mtx.unlock(); 

                shmem_int_finc( &(myInfo.getCollective()->cnt), remoteRank );
                shmem_template_put( target, source, nbElem, remoteRank );
#ifdef _DEBUG
                std::cout << myInfo.getRank() << "] put buff into " << remoteRank << " ; " << nbElem << " elements" << std::endl;
#endif

            }
        }

    } else {
        named_mtx.lock(); 
        if( false == coll->inProgress ) {
            coll->cnt = 0;
            coll->inProgress = true;
#if defined( _SAFE ) || defined( _DEBUG )
            coll->space =  nbElem * sizeof( T );
#endif
        }
        named_mtx.unlock(); 

        while( myInfo.getCollective()->cnt != 1 ) {
            usleep( SPIN_TIMER );
        }
#ifdef _DEBUG
        std::cout << myInfo.getRank() << "] recv Bcast buff" <<  std::endl;
#endif

    }

    myInfo.collectiveReset();
}
