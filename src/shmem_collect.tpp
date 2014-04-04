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

#include "shmem_atomic.h"

#ifdef SHMEM_COLL_FLAT
#include "shmem_bcast.h"
#endif

#include "shmem_put.h"
#include "shmem_get.h"

#include "shmem_alloc.h"

template <class T, class V> void shmem_template_gather_flat( T*, const T*, size_t, int, int, int, V*, int );
template <class T, class V> void shmem_template_collect_flat( T*, const T*, size_t, int, int, int, V* );

template <class T, class V> void shmem_template_collect( T* target, const T* source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, V* pSync ){

    /* The active set of PEs is defined by the triple PE_start, logPE_stride and PE_size. 
     * The results of the reduction must be stored at address target on all PEs of the active set.
     *
     * As with all OpenSHMEM collective routines, each of these routines assumes that only PEs
     * in the active set call the routine. If a PE not in the active set calls a OpenSHMEM collective
     * routine, undefined behavior results.
     */

    int myrank = myInfo.getRank();

    if( _shmem_unlikely( ( myrank < PE_start ) ||
                        ( myrank > ( PE_start + ( PE_size * ( 0x1 << logPE_stride) ) ) ) ||
                        ( ( myrank - PE_start ) % ( 0x1 << logPE_stride ) ) != 0 ) ) {
#ifdef _DEBUG
        std::cout << "Collect: Rank " << myInfo.getRank() << " is not part of the set of participating PEs." << std::endl;
#endif
        return;
    }

#ifdef SHMEM_COLL_FLAT
    /* Flat collective algorithm: gather + bcast */
    shmem_template_collect_flat( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );

#else
    #warning "No collective operation algorithm selected -- using flat by default"
    shmem_template_collect_flat( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
#endif
    

}

template <class T, class V> void shmem_template_collect_flat( T* target, const T* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    /* Gather */

    int ROOT = PE_start;
    shmem_template_gather_flat( target, source, nelems, PE_start, logPE_stride, PE_size, pSync, ROOT );
#ifdef _DEBUG
    if( myInfo.getRank() == ROOT ) {
        std::cout << "Gathered buff: " << std::endl;
        for( int i = 0 ; i < myInfo.getSize() ; i++ ) {
            std::cout << target[i] << "  " ;
        }
        std::cout << std::endl;
    }
#endif
    
    /* In-place bradcast */

    shmem_broadcast_template( const_cast<T*>( target ), const_cast<const T*>( target ), nelems * PE_size, ROOT, PE_start, logPE_stride, PE_size, pSync );

}

template <class T, class V> void shmem_template_gather_flat( T* target, const T* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, V* pSync, int root ){

    int myrank = myInfo.getRank();
    int mysize = myInfo.getSize();
    Collective_t* coll = myInfo.getCollective();
    bool rc;

    if( _shmem_unlikely( ( myrank < PE_start ) ||
                         ( myrank >= ( PE_start + ( PE_size * ( 0x1 << logPE_stride) ) ) ) ||
                         ( ( myrank - PE_start ) % ( 0x1 << logPE_stride ) ) != 0 ) ) {
#ifdef _DEBUG
        std::cout << "Collect: Rank " << myInfo.getRank() << " is not part of the set of participating PEs." << std::endl;
#endif
        return;
    }

    char* mutName;
    asprintf( &mutName, "mtx_shmem_gather_root" );
    boost::interprocess::named_mutex named_mtx( boost::interprocess::open_or_create, mutName );


    if( root == myrank ) {
        
        named_mtx.lock(); 

        if( false == coll->inProgress ) {
            std::cout << "NOT in progress" << std::endl;

            /* Nobody has pushed any data inside of me yet */
            coll->ptr = target; 
            coll->cnt = 0;
            coll->inProgress = true;
            coll->type = _SHMEM_COLL_GATHER;
#if defined( _SAFE ) || defined( _DEBUG )
            coll->space = PE_size * nelems * sizeof( T );
#endif
        } else { 
            std::cout << "ALREADY in progress" << std::endl;

#if defined( _SAFE ) || defined( _DEBUG )
            // TODO : check que la taille est bonne
            // check que le type est le bon !
#endif

        }
        named_mtx.unlock(); 

        _shmem_memcpy( target, source, nelems * sizeof( T ) );
        while( coll->cnt != ( mysize - 1 ) ) {
            usleep( SPIN_TIMER );
#ifdef _DEBUG
            printf( "cnt = %d\n", coll->cnt );
#endif
        }

    } else {
        int offset = nelems * ( myrank - PE_start ) / ( 0x1 << logPE_stride );
#if _DEBUG
        std::cout << "[" << myrank << "] offset : " << offset << std::endl;
#endif
        
        coll->type = _SHMEM_COLL_GATHER;
        coll->inProgress = true;
        coll->ptr = target;

        Collective_t* remote_coll = static_cast<Collective_t *>( _getRemoteAddr( coll, root ) );
        
#if defined( _SAFE ) || defined( _DEBUG )
    
    /* Check if the collective that is already in progress 
       is matching the current collective type 
    */
        
        if( !( _SHMEM_COLL_GATHER == remote_coll->type ) && !( _SHMEM_COLL_NONE == remote_coll->type )  ) {
            std::cerr << "Wrong collective type: process rank " << myInfo.getRank();
            std::cerr << " attempted to participate to a reduce operation ";
            std::cerr << "( code " << _SHMEM_COLL_GATHER <<" ) whereas another ";
            std::cerr << "collective operation is in progress (code ";
            std::cerr << remote_coll->type << ")" << std::cerr;
            return;
        }
#endif
        
        named_mtx.lock();
        printf( "%d Locked\n", myrank );
        
        /* Is the remote guy ready? */
        if( false == remote_coll->inProgress ) {

#ifdef _DEBUG
            std::cout << "[" << myrank << "] the remote guy is not ready ; allocate some space for him" << std::endl;
#endif

            /* init the rest of the collective data structure */
 
            remote_coll->type = _SHMEM_COLL_GATHER;
            remote_coll->cnt = 0;
            remote_coll->inProgress = true;
#if defined( _SAFE ) || defined( _DEBUG )
            remote_coll->space =  PE_size * nelems * sizeof( T );
#endif

            //   std::cout << "[" << myrank << "] allocated " << PE_size * nelems * sizeof( T ) << " bytes at addr " << std::hex << ptr << std::endl;

        } 
        named_mtx.unlock();   
        
        /* put my data (no concurrency in my slice of the target -> no mutex necessary) */

        shmem_template_put( target + offset, source, nelems, root );

        int k = shmem_template_finc( &(coll->cnt), root );

#ifdef _DEBUG
        std::cout << myrank << "] done " << k+1 << std::endl;

        // TMP
        T* tmp = (T*) malloc( PE_size * nelems * sizeof( T ) );
        shmem_template_get( tmp, static_cast<const T*>( coll->ptr ), nelems * PE_size, root );

        for( unsigned int i = 0 ; i < PE_size * nelems ; i++ ) {
            std::cout << tmp[i] << " ";
        }
        std::cout << endl;
        free( tmp );
        // END TMP
#endif

    }

    free( mutName );

    myInfo.collectiveReset();


    printf( "[%d] finished gather\n", myrank );

}


template <class T, class V> void shmem_template_fcollect( T* target, const T* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, V* pSync ){
    shmem_template_collect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}
