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

#include <stdlib.h>
#include <string.h>
#include "shmem_atomic.h"
#include "shmem_put.h"
#include "shmem_get.h"
#include "shmem_internal.h"
#include "shmem_bcast.h"

/**********************************************************************/
/*       **************           AND           **************        */
/**********************************************************************/

template <class T>void shmem_template_and_to_all( T* target, T* source, int nreduce,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {

#ifdef SHMEM_COLL_BINARY
    shmem_template_and_binary( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#if defined( SHMEM_COLL_FLAT_GET ) || defined( SHMEM_COLL_FLAT )
    shmem_template_and_flat_get( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#ifdef SHMEM_COLL_FLAT_PUT
    shmem_template_and_flat_put( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
    #warning "No collective operation algorithm selected -- using flat by default"
    shmem_template_and_flat(target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#endif
#endif
#endif
}

/* various implementations */

template <class T>void shmem_template_and_flat_get( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_flat_reduce_get(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_and );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T>void shmem_template_and_flat_put( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_flat_reduce_put(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_and );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T>void shmem_template_and_binary( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_binarytree_reduce(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_and );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}


/**********************************************************************/
/*       **************            OR           **************        */
/**********************************************************************/

template <class T>void shmem_template_or_to_all( T* target, T* source, int nreduce,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {

#ifdef SHMEM_COLL_BINARY
    shmem_template_or_binary( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#if defined( SHMEM_COLL_FLAT_GET ) || defined( SHMEM_COLL_FLAT )
    shmem_template_or_flat_get( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#ifdef SHMEM_COLL_FLAT_PUT
    shmem_template_or_flat_put( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
    #warning "No collective operation algorithm selected -- using flat by default"
    shmem_template_or_flat(target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#endif
#endif
#endif
}

/* various implementations */

template <class T>void shmem_template_or_flat_get( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_flat_reduce_get(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_or );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T>void shmem_template_or_flat_put( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_flat_reduce_put(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_or );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T>void shmem_template_or_binary( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_binarytree_reduce(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_or );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}


/**********************************************************************/
/*       **************           XOR           **************        */
/**********************************************************************/

template <class T>void shmem_template_xor_to_all( T* target, T* source, int nreduce,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {

   #ifdef SHMEM_COLL_BINARY
    shmem_template_xor_binary( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#if defined( SHMEM_COLL_FLAT_GET ) || defined( SHMEM_COLL_FLAT )
    shmem_template_xor_flat_get( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#ifdef SHMEM_COLL_FLAT_PUT
    shmem_template_xor_flat_put( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
    #warning "No collective operation algorithm selected -- using flat by default"
    shmem_template_xor_flat(target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#endif
#endif
#endif
}

/* various implementations */

template <class T>void shmem_template_xor_flat_get( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_flat_reduce_get(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_xor );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T>void shmem_template_xor_flat_put( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_flat_reduce_put(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_xor );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T>void shmem_template_xor_binary( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_binarytree_reduce(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_xor );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

/**********************************************************************/
/*       **************           MAX           **************        */
/**********************************************************************/

template <class T>void shmem_template_max_to_all( T* target, T* source, int nreduce,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
#ifdef SHMEM_COLL_BINARY
    shmem_template_max_binary( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#if defined( SHMEM_COLL_FLAT_GET ) || defined( SHMEM_COLL_FLAT )
    shmem_template_max_flat_get( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#ifdef SHMEM_COLL_FLAT_PUT
    shmem_template_max_flat_put( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
    #warning "No collective operation algorithm selected -- using flat by default"
    shmem_template_max_flat(target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#endif
#endif
#endif
}

/* various implementations */

template <class T>void shmem_template_max_flat_get( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_flat_reduce_get(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_max );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T>void shmem_template_max_flat_put( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_flat_reduce_put(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_max );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T>void shmem_template_max_binary( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_binarytree_reduce(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_max );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

/**********************************************************************/
/*       **************           MIN           **************        */
/**********************************************************************/

template <class T>void shmem_template_min_to_all( T* target, T* source, int nreduce,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
#ifdef SHMEM_COLL_BINARY
    shmem_template_min_binary( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#if defined( SHMEM_COLL_FLAT_GET ) || defined( SHMEM_COLL_FLAT )
    shmem_template_min_flat_get( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#ifdef SHMEM_COLL_FLAT_PUT
    shmem_template_min_flat_put( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
    #warning "No collective operation algorithm selected -- using flat by default"
    shmem_template_min_flat(target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#endif
#endif
#endif
}

/* various implementations */

template <class T>void shmem_template_min_flat_get( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_flat_reduce_get(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_min );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T>void shmem_template_min_flat_put( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_flat_reduce_put(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_min );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T>void shmem_template_min_binary( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_binarytree_reduce(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_min );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}


/**********************************************************************/
/*       **************           SUM           **************        */
/**********************************************************************/

template <class T, class V> void shmem_template_sum_to_all( T* target, T* source, int nreduce,  int PE_start, int logPE_stride, int PE_size, V* pWrk, long* pSync) {
#ifdef SHMEM_COLL_BINARY
    shmem_template_sum_binary( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#if defined( SHMEM_COLL_FLAT_GET ) || defined( SHMEM_COLL_FLAT )
    shmem_template_sum_flat_get( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#ifdef SHMEM_COLL_FLAT_PUT
    shmem_template_sum_flat_put( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
    #warning "No collective operation algorithm selected -- using flat by default"
    shmem_template_sum_flat(target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#endif
#endif
#endif

}

/* various implementations */

template <class T, class V>void shmem_template_sum_flat_get( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, V* pWrk, long* pSync) {
    shmem_template_flat_reduce_get(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_sum );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T, class V>void shmem_template_sum_flat_put( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, T* pWrk, long* pSync) {
    shmem_template_flat_reduce_put(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_sum );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}


template <class T, class V>void shmem_template_sum_binary( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, V* pWrk, long* pSync) {
    shmem_template_binarytree_reduce(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_sum );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}


/**********************************************************************/
/*       **************          PROD           **************        */
/**********************************************************************/

template <class T, class V>void shmem_template_prod_to_all( T* target, T* source, int nreduce,  int PE_start, int logPE_stride, int PE_size, V* pWrk, long* pSync) {
#ifdef SHMEM_COLL_BINARY
    shmem_template_prod_binary( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#if defined( SHMEM_COLL_FLAT_GET ) || defined( SHMEM_COLL_FLAT )
    shmem_template_prod_flat_get( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
#ifdef SHMEM_COLL_FLAT_PUT
    shmem_template_prod_flat_put( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#else
    #warning "No collective operation algorithm selected -- using flat by default"
    shmem_template_prod_flat(target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
#endif
#endif
#endif
}

/* various implementations */

template <class T, class V>void shmem_template_prod_flat_get( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, V* pWrk, long* pSync) {
    shmem_template_flat_reduce_get(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_prod );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T, class V>void shmem_template_prod_flat_put( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, V* pWrk, long* pSync) {
    shmem_template_flat_reduce_put(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_prod );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

template <class T, class V>void shmem_template_prod_binary( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, V* pWrk, long* pSync) {
    shmem_template_binarytree_reduce(target, source, nelems, PE_start, logPE_stride, PE_size, pWrk, pSync, &_shmem_operation_template_prod );
    shmem_broadcast_template( target, target, nelems, PE_start, PE_start, logPE_stride, PE_size, /* pSync */ pWrk );
}

/**********************************************************************/
/*                                                                    */
/*       **************        OPERATIONS       **************        */
/*                                                                    */
/**********************************************************************/

template <class T> void _shmem_operation_template_and( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 & *buff2 );
 }

 template <class T> void _shmem_operation_template_or( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 | *buff2 );
 }

 template <class T> void _shmem_operation_template_xor( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 ^ *buff2 );
 }

 template <class T> void _shmem_operation_template_max( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }

 template <class T> void _shmem_operation_template_min( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }

 template <class T> void _shmem_operation_template_sum( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 + *buff2);
 }

 template <class T> void _shmem_operation_template_prod( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 * *buff2);
 }

/**********************************************************************/
/*                                                                    */
/*       **************        TOPOLOGIES       **************        */
/*                                                                    */
/**********************************************************************/

template <class T, class V>void shmem_template_flat_reduce_put( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, V* pWrk, long* pSync,  void(* operation)( T*, T*, T* ) ) {

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

    if( _shmem_unlikely( ROOT == myRank ) ) {

        /* Everybody will write into me */


    } else {





    } 



    std::cout << myInfo.getRank() << "] Flat reduction done" << std::endl;
    if( myInfo.getRank() == ROOT ) std::cout << "result: " << *target << std::endl;
    free( mutName );

    myInfo.collectiveReset();
}

template <class T> bool shmem_template_get_and_operation( T* target, T* source, int nelems, int remoterank, T* local, void(* operation)( T*, T*, T* ) ) {

     Collective_t* coll = myInfo.getCollective();

   /* Try to fetch the data.
       If the remote guy is not ready yet, enqueue its rank in the vector
       and eventually poll the vector's guys */
    
     Collective_t* remote_coll = static_cast<Collective_t *>( _getRemoteAddr( coll, remoterank ) );
    
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
        return false;
    }
#endif
    
    /* Is the remote guy ready? */
    
    if( false == remote_coll->inProgress ) { /* no */
        return false;
    } else { /* yes */
        shmem_template_get( local, source, nelems, remoterank );
        operation( target, target,  local );
    }
    
    /* increment the remote guy's counter */
    
    shmem_template_finc( &(coll->cnt), remoterank );

    return true;
}

template <class T, class V>void shmem_template_flat_reduce_get( T* target, T* source, int nelems,  int PE_start, int logPE_stride, int PE_size, V* pWrk, long* pSync,  void(* operation)( T*, T*, T* ) ) {

    int myRank = myInfo.getRank();
    int size = myInfo.getSize();
    bool rc; 

    Collective_t* coll = myInfo.getCollective();
    coll->type = _SHMEM_COLL_REDUCE;

#define ROOT PE_start

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

        /* I get data from everybody */
        std::vector<int> ranks;
        T* local;

        local = (T*) malloc( nelems * sizeof( T ) );
        _shmem_memcpy( target, source, nelems*sizeof( T ) );

        for( int rr = PE_start ; rr < PE_start + PE_size ; rr += ( 0x1 << logPE_stride ) ) {

            if( _shmem_likely( myRank != rr ) ) {
                bool rc;
                rc = shmem_template_get_and_operation( target, source, nelems, rr, local, operation );

                if( false == rc ) {
                    ranks.push_back( rr );
#ifdef _DEBUG
                    std::cout << "Flat reduction: " << rr << " will be tried again" << std::endl;
#endif
                }
#ifdef _DEBUG
                else {
                    std::cout << "Flat reduction: " << rr << " done" << std::endl;
                }
#endif
            }

        }

        while( 0 > ranks.size() ) {

            for( std::vector<int>::iterator it = ranks.begin(); it != ranks.end(); it++ ) {
                bool rc;
                rc = shmem_template_get_and_operation( target, source, nelems, *it,local,  operation );
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

#ifdef _DEBUG
    std::cout << myInfo.getRank() << "] Flat reduction done" << std::endl;
    if( myInfo.getRank() == ROOT ) std::cout << "result: " << *target << std::endl;
#endif

    myInfo.collectiveReset();
#ifdef _DEBUG
    std::cout << "[" << myRank << "] reset collective data structure" << std::endl;
#endif
}


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

