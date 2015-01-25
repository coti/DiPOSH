#include "shmem.h"
#include "shmem_internal.h"
#include "shmem_constants.h"

/* Global variables */

char VAR_SMA_VERSION[] = "SMA_VERSION";
int shmem_sma_version[] = { 0, 0, 1 };
char VAR_SMA_INFO[] = "SMA_INFO";
char shmem_sma_info[] = "pSHMEM, Paris SHMEM, implementation of OpenSHMEM (c) LIPN 2012\n";

char VAR_HEAP_SIZE[] = "SMA_SYMMETRIC_SIZE";
//unsigned long long int heap_size = 134217728; /* 128 MB */
char VAR_SMA_DEBUG[] = "SMA_DEBUG";
int shmem_sma_debug = 0;

char VAR_NUM_PE[]    = "SHMEM_NUM_PE";    /* Number of PEs */
char VAR_PE_NUM[]    = "SHMEM_PE_NUM";    /* My own PE number */ 
char VAR_RUN_DEBUG[] = "SHMEM_RUN_DEBUG"; /* Debug mode: start an infinite loop to attach a debugger */
char VAR_PID_ROOT[]  = "SHMEM_PID_ROOT";  /* pid of the root process (rank 0) */


//managed_shared_memory symmetric_heap;

SymmetricHeap myHeap;
MeMyselfAndI myInfo;
std::vector<Process> processes;

/* Initializes OpenSHMEM. The parameter is not used. 
 * The specs say it should be 0, but let's be tolerant and just ignore it.
 * Can be called multiple times with no effect.
 * Must be called before any other OpenSHMEM routine.
*/

void start_pes( int npes ) {

    char* taktukrank;

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
    
    if( myInfo.getStarted() ) return;

    /* Get my PE number and the number of PEs */

    myInfo.findAndSetMyRank();
    myInfo.findAndSetMySize();
    myInfo.findAndSetPidRoot();

    /* Am I using Open-MX ? */

    taktukrank = getenv( "TAKTUK_RANK" );
    if( NULL != taktukrank ){ /* yes: initialize it */
        myInfo.initOpenMX();
    }
    //    free( taktukrank );

    /* Set up the symmetric heap of this process element */
    myHeap.setupSymmetricHeap();    
    myInfo.collectiveInit();
    myInfo.allocNeighbors( myInfo.getSize() );
    myInfo.initNeighbors( myInfo.getSize() );

    /* Allgather all the pids */

    /*  int* procs = (int*)_shmallocFake( myInfo.getSize() * sizeof( int ) );
    int mypid = getpid();
    std::cout << myInfo.getRank() << "] enter allgather" << std::endl;
    shmem_int_allgather_flat( procs, &mypid, 1 );

    if( myInfo.getRank() == 1 ) {
        int i;
        for( i = 0 ; i < myInfo.getSize() ; i++ ) {
            std::cout << i << " | " << procs[i] << std::endl;
        }
        }*/

#ifdef _DEBUG
    sleep( 1 );
    std::cout << "PE " << myInfo.getRank() << " / " << myInfo.getSize() << " up and running on proc " << getpid() << std::endl;
#endif

    /* Start-up done! */

    myInfo.setStarted( true );
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

