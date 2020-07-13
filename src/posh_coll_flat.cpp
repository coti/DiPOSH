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

#include "shmem.h"
#include "shmem_internal.h"
#include "shmem_barrier.h"

#include "posh_coll_flat.h"

#ifdef _WITH_NMAD
nm_comm_t p_comm = NULL; 
#endif // _WITH_NMAD

#if 0
void posh_barrier_all_flat( ){
#if !defined( MPICHANNEL ) && !defined( MPIHUBCHANNEL )
#ifdef _WITH_NMAD
    if( NULL == p_comm )
        p_comm = nm_comm_world( "nm_posh_barrier" );
    nm_coll_barrier( p_comm, TAG_BARRIER );
#else
    shmem_barrier_naive();
#endif // _WITH_NMAD
#else
    MPI_Barrier( myInfo.world );
#endif 
}
#endif


void posh_barrier_all_flat(){
    int ROOT = 0;
    Collective_t* coll = myInfo.getCollective();
    int myrank = myInfo.getRank();
    int mysize = myInfo.getSize();
    bool rc;
    
#ifdef _DEBUG
    if( shmem_my_pe() == ROOT ) std::cout << " - - - - - - - - - - - " << std::endl;
#endif  

    boost::interprocess::named_mutex mtx(boost::interprocess::open_or_create, "shmem_barrier_ROOT" );

   /* algo de merde : reduce + bcast */

    if( _shmem_unlikely( myrank == ROOT ) ) {
        mtx.lock( );   
        while( coll->cnt != ( mysize - 1 ) ) {
            mtx.unlock( );
            usleep( SPIN_TIMER );
            mtx.lock( );
        }
        for( int i = 0 ; i < mysize ; i++ ) {
            if( _shmem_likely( i != myrank ) ) {
                shmem_int_finc( &(coll->cnt), i );
            }
        }
    } else {

        /* send a signal to the root process:
           increment its collective counter */
        mtx.lock( );
        shmem_int_finc( &(coll->cnt), ROOT );
        mtx.unlock( );

        /* wait until my local counter has been incremented by the root process */
        while( coll->cnt == 0 ) {
            usleep( SPIN_TIMER );
        }
    }

    /* reset collective data structure */

    myInfo.collectiveReset();
    if( _shmem_unlikely( myrank == ROOT ) ) {
          mtx.unlock( );
    }
}

void posh_barrier_flat( int PE_start, int logPE_stride, int PE_size, long *pSync ){
    ;;
    /* TODO */
}
