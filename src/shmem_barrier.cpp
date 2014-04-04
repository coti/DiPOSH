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

#include "shmem.h"
#include "shmem_internal.h"

/* Suspends the execution of the calling PE until all other PEs issue a call to this
 * particular shmem_barrier_all() statement.
 * The shmem_barrier_all() routine does not return until all other PEs have entered 
 * this routine at the same point of the execution path.
 * Prior to synchronizing with other PEs, shmem_barrier_all() ensures completion of all
 * previously issued local memory stores and target memory updates issued via OpenSHMEM
 * functions such as shmem_put32.
 */
void shmem_barrier_all( ){
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

/* Performs a barrier operation on a subset of processing elements (PEs).
 * The shmem_barrier() routine does not return until the subset of PEs specified by
 * PE_start, logPE_stride and PE_size, have entered this routine at the same point of
 * the execution path.
 * If the pSync array is initialized at run time, be sure to use some type of
 * synchronization, for example, a call to shmem_barrier_all before calling shmem_barrier
 * for the first time.
 * If the active set does not change, shmem_barrier can be called repeatedly with the same
 * pSync array. No additional synchronization beyond that implied by shmem_barrier itself
 * is necessary in this case.
 */

void shmem_barrier( int PE_start, int logPE_stride, int PE_size, long *pSync ){
    ;;
}

/* Provides a separate ordering on the sequence of puts issued by this PE to each
 * destination PE.
 * The shmem_fence() routine provides an ordering on the put operations issued by
 * the calling PE prior to the call to shmem_fence() relative to the put operations
 * issued by the calling PE following the call to shmem_fence(). It guarantees that
 * all such prior put operations issued to a particular destination PE are fully
 * written to the symmetric memory of that destination PE, before any such following
 * put operations to that same destination PE are written to the symmetric memory of
 * that destination PE.
 * Note that the ordering is provided separately on the sequences of puts from the
 * calling PE to each distinct destination PE. The shmem_quiet() routine should be
 * used instead if ordering of puts is required when multiple destination PEs are
 * involved.
 */

void shmem_fence( ){
    ;;
}

/* Provides an ordering on the sequence of puts issued by this PE across all
 * destination PEs. 
 * The shmem_quiet() routine provides an ordering on the put operations issued by
 * the calling PE prior to the call to shmem_quiet() relative to memory load, memory
 * store, put, get or synchronization operations issued by the calling PE following
 * the call to shmem_quiet(). It guarantees that all such prior put operations are fully
 * written to the symmetric memory of the destination PE and visible to all other PEs,
 * before any such following memory load, memory store, put, get or synchronization
 * operations are visible to any PE.
*/

void shmem_quiet( ){
    ;;
}

