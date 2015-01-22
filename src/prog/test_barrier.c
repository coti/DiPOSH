#include <stdio.h>
#include <stdlib.h>
#include "shmem.h"

#include <time.h>

static int32_t token;
static int result;

int main() {

    int rank, size;
    int res;

    start_pes( 0 );

    rank = shmem_my_pe();
    size = shmem_n_pes();

    printf( "Hello, I am process %d in %d \n", rank, size );

    sleep( 2 );
    /* can I reach a buddy? */

    int myBuddyLeft = ( rank + 1 ) % size;
    int myBuddyRight = ( rank + size - 1 ) % size;
    res = shmem_pe_accessible( myBuddyLeft );
    if( 1 == res ) {
        printf( "[%d/%d] My buddy %d is reachable\n", rank, size, myBuddyLeft );
    } else {
        printf( "[%d/%d] My buddy %d is NOT reachable\n", rank, size, myBuddyLeft );
    }
    res = shmem_pe_accessible( myBuddyRight );
    if( 1 == res ) {
        printf( "[%d/%d] My buddy %d is reachable\n", rank, size, myBuddyRight );
    } else {
        printf( "[%d/%d] My buddy %d is NOT reachable\n", rank, size, myBuddyRight );
    }

    if( rank == 0 ) printf( " - - - - - - - - - - \n\n" );
    shmem_barrier_all();  /* sync sender and receiver */
    printf( "[%d] - - - - - FIRST BARRIER PASSED - - - - - \n\n", rank );

    sleep( 1 );

    token = getpid(); 
#if 0
    shmem_broadcast32( &token, &token, 1, 0, 0, 0, size, NULL );
    printf( "[%d] bcast done\n", rank );

    sleep( 1 );
    shmem_barrier_all();  /* sync sender and receiver */
    printf( "[%d] - - - - - SECOND BARRIER PASSED - - - - - \n\n", rank );

    sleep( 1 );
#endif

    token = getpid() + time( NULL );

    shmem_int_max_to_all( &result, &token, 1, 0, 0, size, NULL, NULL );
    
    printf( "%d] token: %d ; result: %d\n", rank, token, result );

    return EXIT_SUCCESS;
}
