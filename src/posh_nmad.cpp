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

#include "posh_nmad.h"
#include "shmem_internal.h"
#include "posh_heap.h"  // for _getOffset
#include <iostream>

/* Computes the local remote address, ie where the remote peer
   has addressed the element */

uintptr_t getRemoteLocalAddr( const void* addr, Neighbor_t* nb ) {
    /* compute the local offset */
    size_t offset = _getOffset( addr );
    /* add the remote base address */
    return offset + ((Communication_NMAD_t*) nb->communications)->getBaseAddr();
} 

int shmem_nmad_put( int pe, void* target, const void* source, size_t size ){
    struct nm_data_s data;
    nm_onesided_request_t req;
    Neighbor_t* nb;

 #ifdef _PROFILING
    struct timespec s_time, build_time, info_time, put_time, e_time;
    clock_gettime( CLOCK_REALTIME, &s_time );
#endif

    nm_data_contiguous_build( &data, const_cast<void*>( source ), size /* sizeof( source )*/ );
    
#ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &build_time );
#endif

    nb = myInfo.getNeighbor( pe );
    if( _shmem_unlikely( TYPE_NMAD != nb->comm_type ) ) {
        std::cerr << " Error -- remote peer " << pe << " is not a NewMadeleine peer" << std::endl;
        return -1;
    }
    Communication_NMAD_t* channel = (Communication_NMAD_t*) nb->communications;
    nm_gate_t gate;
    nm_launcher_get_gate( pe, &gate );
    nm_onesided_t os_session = myInfo.getMyEndpointNMAD()->getOsSession();
    
    const char* addrTarget = reinterpret_cast<const char*>( getRemoteLocalAddr( reinterpret_cast<void* >( target ), nb ) );

 #ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &info_time );
#endif
   
    /*      std::cout << "Put at address " << reinterpret_cast<uintptr_t>( addrTarget ) << " on peer " << pe << std::endl;*/
    //    printf( "addr %d\n", (uintptr_t) addrTarget );
    
    /* on envoie la sauce */
    
    //    nm_onesided_iput( os_session, gate, &data, reinterpret_cast<uintptr_t>( addrTarget ) /*(uintptr_t)target*/, &req );
    //    nm_onesided_req_wait( &req );
    nm_onesided_put( os_session, gate, &data, reinterpret_cast<uintptr_t>( addrTarget ) /*(uintptr_t)target*/ );
#ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &put_time );
#endif

    nm_onesided_fence( os_session, gate );
    
 #ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &e_time );
    std::cout << __FUNCTION__ << " total time: " << TIMEDIFF( s_time, e_time ) << " ns " << std::endl;
    std::cout << __FUNCTION__ << " build time: " << TIMEDIFF( s_time, build_time ) <<" ns " << std::endl;
    std::cout << __FUNCTION__ << " info time: " << TIMEDIFF( build_time, info_time ) <<" ns " << std::endl;
    std::cout << __FUNCTION__ << " put time: " << TIMEDIFF( info_time, put_time ) <<" ns " << std::endl;
    std::cout << __FUNCTION__ << " fence time: " << TIMEDIFF( put_time, e_time ) <<" ns " << std::endl;
#endif
    
    return 0; // tmp
}

int shmem_nmad_get( int pe, void* dest, const void* source, size_t size ){

    struct nm_data_s data;
    nm_onesided_request_t req;
    Neighbor_t* nb;

  #ifdef _PROFILING
    struct timespec s_time, build_time, info_time, get_time, e_time;
    clock_gettime( CLOCK_REALTIME, &s_time );
#endif

    nm_data_contiguous_build( &data, const_cast<void*>( dest ), size /* sizeof( source )*/ );

 #ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &build_time );
#endif

    nb = myInfo.getNeighbor( pe );
    if( _shmem_unlikely( TYPE_NMAD != nb->comm_type ) ) {
        std::cerr << " Error -- remote peer " << pe << " is not a NewMadeleine peer" << std::endl;
        return -1;
    }
    Communication_NMAD_t* channel = (Communication_NMAD_t*) nb->communications;
    nm_gate_t gate;
    nm_launcher_get_gate( pe, &gate );
    nm_onesided_t os_session = myInfo.getMyEndpointNMAD()->getOsSession();
    
    const char* addrDest = reinterpret_cast<const char*>( getRemoteLocalAddr( const_cast<void* >( source ), nb ) );
    
 #ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &info_time );
#endif
   
    //std::cout << "Get from address " << reinterpret_cast<uintptr_t>( addrDest ) << " on peer " << pe << std::endl;
    //    printf( "addr %d\n", (uintptr_t) addrDest );
    
    /* on envoie la sauce */
    
    //    nm_onesided_iget( os_session, gate, &data, reinterpret_cast<uintptr_t>( addrDest ) /*(uintptr_t)dest*/, &req );
    //    nm_onesided_req_wait( &req );
    nm_onesided_get( os_session, gate, &data, reinterpret_cast<uintptr_t>( addrDest ) /*(uintptr_t)dest*/ );
#ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &get_time );
#endif

    nm_onesided_fence( os_session, gate );
    
#ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &e_time );
    std::cout << __FUNCTION__ << " total time: " << TIMEDIFF( s_time, e_time ) << " ns " << std::endl;
    std::cout << __FUNCTION__ << " build time: " << TIMEDIFF( s_time, build_time ) <<" ns " << std::endl;
    std::cout << __FUNCTION__ << " info time: " << TIMEDIFF( build_time, info_time ) <<" ns " << std::endl;
    std::cout << __FUNCTION__ << " get time: " << TIMEDIFF( info_time, get_time ) <<" ns " << std::endl;
    std::cout << __FUNCTION__ << " fence time: " << TIMEDIFF( get_time, e_time ) <<" ns " << std::endl;
#endif
    
    return 0; // tmp
}

/* CI-related routines */

#include "posh_launcher_padico.h"

void shmem_nmad_exchange_ci( void ){

    /* NMad works with remote local adresses */
    
    uintptr_t* addresses = (uintptr_t*) malloc( myInfo.getSize() * sizeof( int* ) );
    int root = 0;
    int i;
   
    uintptr_t myBaseAddr = myHeap.getBaseAddr();
    ContactInfo ci;

    nm_comm_t p_comm = ((Launcher_Padico*)(myInfo.getRTE()))->comm_world;
    
    /* Gather + Bcast because there is no allgather in NMad */

    nm_coll_gather( p_comm, root, &myBaseAddr, sizeof( int* ), addresses, sizeof( int* ), INIT_TAG );
    nm_coll_bcast( p_comm, 0, addresses, myInfo.getSize()*sizeof( int* ), INIT_TAG );
    
    
    // std::cout << myInfo.getRank() << " My base addr: " << myBaseAddr << std::endl;
    for(  i = 0 ; i < myInfo.getSize(); i++ ){
        ci.setBaseAddress( addresses[i] );

        //        myInfo.myEndpointNMAD.getGate( i );
        myInfo.initNeighborNMAD( i, ci );
        std::cout << myInfo.getRank() << " base addr: " << addresses[i] << " for rank " << i << std::endl;

      }

    
}
