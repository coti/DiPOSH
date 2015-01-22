#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shmem.h"

int main(){

    int rank;
    int* mavariable;

    start_pes( 0 );

    rank = _my_pe();
    printf( "Mon rang est %d\n", rank ); 

    mavariable = (int*) shmalloc( sizeof( int ) );

    if( 0 == rank ) {
        *mavariable = getpid();
    } else {
        *mavariable = 0;
    }

    printf( "%d] ma variable vaut %d\n", rank, *mavariable );

    shmem_barrier_all();

    if( 0 == rank ) {
        shmem_int_p( mavariable, *mavariable, 1 );
    }

    shmem_barrier_all();
    printf( "%d] ma variable vaut %d\n", rank, *mavariable );

    shmem_barrier_all();
    shfree( mavariable );

    return EXIT_SUCCESS;
}
