#include <stdio.h>
#include <stdlib.h>
#include "shmem.h"
#include <sys/time.h>

#define NBXP  20


int main( int argc, char** argv) {

  int rank, size, res, i, j, k;
    int* ptr;
    int* data;
    struct timeval tv_start, tv_end;
    double theTime;
    double** times;

    int nb = 1;

    if( argc > 1 ) {
        nb = atoi( argv[1] ) ;
    }

    start_pes( 0 );

    rank = shmem_my_pe();
    size = shmem_n_pes();

    printf( "Hello, I am process %d in %d \n", rank, size );

    /* Determine the nb of xps */

    k = 0; i = 1;
    while( i < nb ) {
      i = 2 * i;
      k++;
      if( i > 2 ) { // pour k - 3
	k++;
      }
      if( i < nb - 3 ) { // pour k + 3
	k++;
      }
    }
    times = (double**) malloc( k * sizeof( double* ) );
    for( i = 0 ; i < k ; i++ ) {
      times[i] = (double*) malloc( 3 * sizeof( double ) );
    }

    k = -1; i = 1;
    while( i < nb ) {

      i = 2 * i;
      k++;
      times[k][0] = i;
      times[k][1] = 0;
      times[k][2] = 0;
      if( i > 2 ) {
	k++;
	times[k][0] = i - 3;
	times[k][1] = 0;
	times[k][2] = 0;
      }
      if( i < nb - 3 ) { 
	k++;
	times[k][0] = i + 3;
	times[k][1] = 0;
	times[k][2] = 0;
      }
    }

    sleep( 2 );

    /* check */

    int myBuddyLeft = ( rank + 1 ) % size;
    res = shmem_pe_accessible( myBuddyLeft );
    if( 1 == res ) {
        printf( "[%d/%d] My buddy %d is reachable\n", rank, size, myBuddyLeft );
    } else {
        printf( "[%d/%d] My buddy %d is NOT reachable\n", rank, size, myBuddyLeft );
    }

    /* Allocate some memory */

    ptr = (int*)shmalloc( nb * sizeof( int ) );
    data = (int*) malloc( nb * sizeof( int ) );

    /* init */

    srandom( getpid() );
    for( i = 0 ; i < nb ; i++ ) {
        data[i] = random();
    }

    /* warm-up */

    shmem_int_put( ptr, &rank, 1, rank );
    shmem_int_get( &res, ptr, 1 , myBuddyLeft );

    printf( "[%d/%d] read %d from %d\n", rank, size, res, myBuddyLeft );


    /* Put stuff there */

    shmem_int_put( ptr, data, nb, rank );
    sleep( 2 );

    /*** FIXED BUDDY ***/

    /* Remote read in my buddy's memory */
    printf( "[%d/%d] read data from %d\n", rank, size, myBuddyLeft );
  
    for( i = 0 ; i < k ; i++ ) {
        double average, sum, dev;
        average = 0;  dev = 0; sum = 0;
        for( j = 0 ; j < NBXP ; j++ ) {
            
            gettimeofday( &tv_start, NULL );
            shmem_int_get( data, ptr, times[i][0] , myBuddyLeft );
            gettimeofday( &tv_end, NULL );
            
            theTime = ( tv_end.tv_sec - tv_start.tv_sec ) * 1000000 + tv_end.tv_usec - tv_start.tv_usec;
            
            average += theTime;
            sum += theTime * theTime;
            
        }
        if( j != 0 ) {
            average /= j;
            dev = sum / j;
            dev -= (average * average);
            dev = sqrt(dev);
            times[i][1] = average;
            times[i][2] = dev;
        }
        
        printf( "[%d/%d -> %d] %d %f %f %f\n", rank, size, myBuddyLeft, times[i][0], times[i][1], times[i][2] );
	//        printf( "[%d/%d -> %d] %d %.0lf\n", rank, size, myBuddyLeft, i, theTime );
    }

    /*** CHANGING BUDDY ***/
    
    myBuddyLeft = ( myBuddyLeft + 1 ) % size;
    
    for( i = 1 ; i < nb ; i++ ) {
        gettimeofday( &tv_start, NULL );
        shmem_int_get( data, ptr, i , myBuddyLeft );
        gettimeofday( &tv_end, NULL );
        
        theTime = ( tv_end.tv_sec - tv_start.tv_sec ) * 1000000 + tv_end.tv_usec - tv_start.tv_usec;
        
        printf( "[%d/%d -> %d] %d %.0lf\n", rank, size, myBuddyLeft, i, theTime );
    }


    /* Free it */

    shfree( ptr );
    free( data );

    return EXIT_SUCCESS;
}
