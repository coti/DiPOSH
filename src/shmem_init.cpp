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

#include "shmem.h"
#include "shmem_internal.h"
#include "shmem_constants.h"
#ifdef DISTRIBUTED_POSH
#if defined( MPICHANNEL ) || defined( MPIHUBCHANNEL )
#include "posh_mpi.h"
#else
#include "shmem_tcp.h"
#endif // MPICHANNEL || MPIHUBCHANNEL
#endif // DISTRIBUTED_POSH
//#include <pthread.h>

#include "perf.h"
#include "posh_heap.h"

#ifdef MPILAUNCHER
#include <boost/mpi.hpp>
#include <boost/mpi/communicator.hpp>
namespace mpi = boost::mpi;
#endif

/* Global variables */

char VAR_SMA_VERSION[] = "SMA_VERSION";
int shmem_sma_version[] = { 0, 0, 1 };
char VAR_SMA_INFO[] = "SMA_INFO";
char shmem_sma_info[] = "POSH, Paris OpenSHMEM, implementation of OpenSHMEM (c) LIPN 2012-2018\n";
char VAR_HEAP_SIZE[] = "SMA_SYMMETRIC_SIZE";
//unsigned long long int heap_size = 134217728; /* 128 MB */
char VAR_SMA_DEBUG[] = "SMA_DEBUG";
int shmem_sma_debug = 0;

char VAR_NUM_PE[]    = "SHMEM_NUM_PE";    /* Number of PEs */
char VAR_PE_NUM[]    = "SHMEM_PE_NUM";    /* My own PE number */ 
char VAR_RUN_DEBUG[] = "SHMEM_RUN_DEBUG"; /* Debug mode: start an infinite loop to attach a debugger */
char VAR_PID_ROOT[]  = "SHMEM_PID_ROOT";  /* pid of the root process (rank 0) */
char VAR_COMM_CHAN[] = "SHMEM_COMM_CHANNEL";  /* Force a communication channel */

//managed_shared_memory symmetric_heap;

SymmetricHeap myHeap;
MeMyselfAndI myInfo;
std::vector<Process> processes;

#ifdef CHANDYLAMPORT
Checkpointing checkpointing;
#endif

/* Initializes OpenSHMEM. The parameter is not used. 
 * The specs say it should be 0, but let's be tolerant and just ignore it.
 * Can be called multiple times with no effect.
 * Must be called before any other OpenSHMEM routine.
*/

void start_pes( int npes ) {

    int port, rc;
    char* comm_channel;

    /* Loop here in debug mode to be able to attach a debugger */

    if( NULL != getenv( VAR_RUN_DEBUG ) ) {
        std::cout << "debug" << std::endl;
        int spin_loop = 1;
        while( spin_loop == 1 ) {
            int zz = 1;
            usleep( SPIN_TIMER );
            zz+=getpid();
        }
    }

    /* Is the communication channel specified? */

    if( NULL != (comm_channel = getenv( VAR_COMM_CHAN ) ) ) {
        std::cout << "Use communication channel " << comm_channel << std::endl;
    }

    if( myInfo.getStarted() ) return;

    /* Initialize the interactions with the run-time environment */

    myInfo.initRte();

#ifdef MPILAUNCHER
    //    mpi::environment env( mpi::threading::funneled );
    std::cout << "thread level: " << myInfo.env.thread_level() << std::endl;
#endif // MPILAUNCHER

    /* Get my PE number and the number of PEs */
    /* TODO should go to RTE */
    
    myInfo.findAndSetMyRank();
    myInfo.findAndSetMySize();
    myInfo.findAndSetPidRoot();
    
    /* Set up the symmetric heap of this process element */

    myInfo.communicationInit( comm_channel );
    //    myHeap.setupSymmetricHeap();    
    myInfo.collectiveInit();
    myInfo.allocNeighbors( myInfo.getSize() );

#if CHANDYLAMPORT
    /* If required, set up things for checkpointing */
    checkpointing.initMarker();
#endif

    /* Start my communicaton thread */
    myInfo.initNeighbors( myInfo.getSize(), comm_channel );

#ifdef _DEBUG
    //    sleep( 1 );
    std::cout << "PE " << myInfo.getRank() << " / " << myInfo.getSize() << " up and running on proc " << getpid() << std::endl;
#endif

    /* Initialize the performance profiling stuff (if necessary) */
    
    POSH_PROF_INIT
        
    /* Start-up done! */
        
    myInfo.setStarted( true );

    /* TMP */
    /*
    nm_comm_t p_comm = NULL;
    p_comm = nm_comm_world("nm_example");
    nm_coll_barrier( p_comm, 1 );
    */
    /* END TMP */ 
   
}

/* Returns the number of PEs running the application, numbered in [0, N-1[
 */

int shmem_n_pes( ){
    return myInfo.getSize();;
}
int _num_pes( ){
    return shmem_n_pes();
}

/* Returns the PE number of the calling PE
 */

int shmem_my_pe( ){
    return myInfo.getRank();
}
int _my_pe( ){
    return shmem_my_pe();
}

/* Finalize the application 
 */

int shmem_finalize(){
    /* todo */
    /* exit thread */
    /* close sockets */

    myInfo.getMyEndpoint()->finalize();
    
    return 0;
}
