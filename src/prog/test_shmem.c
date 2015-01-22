#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "shmem.h"

int main() {

    int rank, size;
    int res;
    int i;

    start_pes( 0 );

    rank = shmem_my_pe();
    size = shmem_n_pes();

    printf( "Hello, I am process %d in %d \n", rank, size );

    sleep( 2 );

    shmem_barrier_all();

    int* src;
    int* tgt;

    printf( "%d - go !\n", rank );

    src = (int *) shmalloc( sizeof( int ) );
    tgt = (int *) shmalloc( size * sizeof( int ) );


    *src = getpid();
    shmem_int_broadcast( &tgt[0], src, 1, 0, 0, 0, size, NULL ); 

    printf( "[%d] Bcast: %d\n", rank, tgt[0] );

    memcpy( src, &rank, sizeof( rank ) );

    // shmem_barrier_all();
    *src = getpid();
    sleep( 3 );

    shmem_collect32( (void*)tgt, (const void*)src, 1, 0, 0, size, NULL );

    sleep( 2 );
    
    printf( "%d | ", rank );
    for( i = 0 ; i < size ; i++ ) {
        printf( "%d ", (int) tgt[i] );
    }
    printf( "\n" );

    shmem_barrier_all();

    shfree( src );
    shfree( tgt );

    short* strided;
    const short loc[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    strided = (short *) shmalloc( 10 * sizeof( short ) );
    memset( strided, 0, sizeof( short ) );
    shmem_barrier_all();

    shmem_short_iput( strided, loc, 2, 1, 5, (rank + size + 1 ) % size );  // KO

    shmem_barrier_all();

    printf( "%d | ", rank );
    for( i = 0 ; i < 10 ; i++ ) {
        printf( "%d ", (int) strided[i] );
    }
    printf( "\n" );

    memset( strided, 0, sizeof( short ) );

    shmem_barrier_all();

#if 0

    shmem_short_iput( strided, loc, 2, 2, 5, (rank + size + 1 ) % size );  // KO

    shmem_barrier_all();

    printf( "%d | ", rank );
    for( i = 0 ; i < 10 ; i++ ) {
        printf( "%d ", (int) strided[i] );
    }
    printf( "\n" );

    memset( strided, 0, sizeof( short ) );

    shmem_barrier_all();

    shmem_short_iput( strided, loc, 1, 2, 5, (rank + size + 1 ) % size );  // KO

    shmem_barrier_all();

    printf( "%d | ", rank );
    for( i = 0 ; i < 10 ; i++ ) {
        printf( "%d ", (int) strided[i] );
    }
    printf( "\n" );
#endif

    return EXIT_SUCCESS;


#if 0
    if( 0 == rank ) {
        printf("     SUICIDE !!\n" );
        return -1;
    }

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

    /* Allocate some memory */

    /* ca marche */

    int* src;
    int* tgt;

    if( 0 == rank ) {
        printf( "  SUICIDE !!!\n" );
        exit( -1 );
    }

    src = (int *) shmalloc( 10 * sizeof( int ) );
    tgt = (int *) shmalloc( 10 * sizeof( int ) );
    int __toto__src[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    memcpy( src, __toto__src, sizeof( __toto__src ) );

    sleep( 3 );

    printf( "%d allocated at addr %p\n", rank, tgt );

	if( rank == 0 ) {
		int j;
		/* put 5 words into target on PE's [1 to (nProcs-1)] */
		for( j=1 ; j < size ; j++ ){
            shmem_iput32( tgt, src, 1, 2, 5, j );
            //shmem_int_put( tgt, src, 10, j );
        }
	}


    sleep( 3 );


	if( rank == 1 ) {
        int j;
        for( j = 0 ; j < 10 ; j++ ) {
            printf( "tgt[%d] = %d\n", j, tgt[j] );
        }
	}





    printf( " - - - - - - - - - - \n\n" );
    sleep( 2 );

    short* ptr;
    ptr = (short*)shmalloc( sizeof( short ) );

    /* Target addr? */

    printf( "%d -- local: %p, left: %p\n", rank, ptr, shmem_ptr( ptr, myBuddyLeft ) );

    long double* ptr2;
    ptr2 = (long double*)shmalloc( 5*sizeof( long double ) );
    const long double tab[5] = { 1.29713, 291.2312, 5234.2132, 21423.6332, 414.328};

    printf( "[%d/%d] allocated %p\n", rank, size, ptr );

    /* Put stuff there */

    long source[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    static long target[10];

    sleep( 2 );

    if (_my_pe() == 0) {
        /* put 10 elements into target on PE 1 */
        shmem_long_put(target, source, 10, 1);
    }

    shmem_barrier_all();  /* sync sender and receiver */
    sleep( 2 );
    if( rank == 0 ) printf( " - - - - - - - - - - - \n\n" );

    if (_my_pe() == 1) {
        if (0 != memcmp(source, target, sizeof(long) * 10)) {
            int i;
            fprintf(stderr,"[%d] Src & Target mismatch?\n",_my_pe());
            for (i = 0 ; i < 10 ; ++i) {
                printf("%ld,%ld ", source[i], target[i]);
            }
            printf("\n");
            return 1;
        }
    }


    shmem_short_p( ptr, (short) rank, rank );
    printf( "[%d/%d] put %d into %d\n", rank, size, rank, rank );
    //    shmem_longdouble_put( ptr2, tab, 5, myBuddyLeft );

    /* Read what I have just put over there */
    sleep( 2 );
    if( rank == 0 ) printf( " - - - - - - - - - - - \n\n" );
    short toto = shmem_short_g( ptr, rank);
    printf( "[%d/%d] read %d from %d\n", rank, size, toto, rank );

    /* Remote read */

    sleep( 2 );
    if( rank == 0 ) printf( " - - - - - - - - - - - \n\n" );
    sleep( 2 );

    toto = shmem_short_g( ptr, myBuddyRight);
    printf( "[%d/%d] g -- read %d from %d\n", rank, size, toto, myBuddyRight );

    shmem_barrier_all();

    sleep( 2 );
    if( rank == 0 ) printf( " - - - - - - - - - - - \n\n" );

    short titi = shmem_short_swap( ptr, (short)( 1 + 2 * rank ), myBuddyRight );

    printf( "[%d/%d] swap -- read %d from %d\n", rank, size, titi, myBuddyRight );

    sleep( 2 );
    if( rank == 0 ) printf( " - - - - - - - - - - - \n\n" );

    titi = shmem_short_swap( ptr, (short)( 1 + 3 * rank ), myBuddyRight );

    printf( "[%d/%d] swap -- read %d from %d\n", rank, size, titi, myBuddyRight );

    sleep( 2 );
    if( rank == 0 ) printf( " - - - - - - - - - - - \n\n" );

    toto = shmem_short_finc( ptr, myBuddyRight );
    printf( "[%d/%d] finc -- read %d from %d\n", rank, size, toto, myBuddyRight );

    sleep( 2 );
    if( rank == 0 ) printf( " - - - - - - - - - - - \n\n" );

    toto = shmem_short_g( ptr, myBuddyRight );
    printf( "[%d/%d] resultat du finc -- read %d from %d\n", rank, size, toto, myBuddyRight );


    /* Free it */

    shfree( ptr );
    shfree( ptr2 );
#endif

    return EXIT_SUCCESS;
}
