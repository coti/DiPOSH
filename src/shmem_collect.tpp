#include "shmem_internal.h"

#include "shmem_atomic.h"

#ifdef SHMEM_COLL_FLAT
#include "shmem_bcast.h"
#endif

#include "shmem_put.h"
#include "shmem_get.h"

#include "shmem_alloc.h"

template <class T, class V> void shmem_template_gather_flat_put( T*, const T*, size_t, int, int, int, V*, int );
template <class T, class V> void shmem_template_collect_flat( T*, const T*, size_t, int, int, int, V* );
template <class T, class V> void shmem_template_collect_bruck_get( T*, const T*, size_t, int, int, int, V* );
template <class T, class V> void shmem_template_collect_bruck_put( T*, const T*, size_t, int, int, int, V* );

int shmem_int_finc( int*, int ); 

template <class T, class V> void shmem_template_collect( T* target, const T* source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, V* pSync ){

    /* The active set of PEs is defined by the triple PE_start, logPE_stride and PE_size. 
     * The results of the reduction must be stored at address target on all PEs of the active set.
     *
     * As with all OpenSHMEM collective routines, each of these routines assumes that only PEs
     * in the active set call the routine. If a PE not in the active set calls a OpenSHMEM collective
     * routine, undefined behavior results.
     */

    int myrank = myInfo.getRank();

    if( _shmem_unlikely( ( myrank < PE_start ) ||
                        ( myrank > ( PE_start + ( PE_size * ( 0x1 << logPE_stride) ) ) ) ||
                        ( ( myrank - PE_start ) % ( 0x1 << logPE_stride ) ) != 0 ) ) {
#ifdef _DEBUG
        std::cout << "Collect: Rank " << myInfo.getRank() << " is not part of the set of participating PEs." << std::endl;
#endif
        return;
    }

#ifdef SHMEM_COLL_FLAT
    /* Flat collective algorithm: gather + bcast */
    //  shmem_template_collect_flat( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
    shmem_template_collect_bruck_get( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );

#else
#ifdef SHMEM_COLL_BINARY
    /* Bruck */
    shmem_template_collect_bruck_put( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );

#else
    #warning "No collective operation algorithm selected -- using flat by default"
    shmem_template_collect_flat( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
#endif
#endif
    

}

template <class T, class V> void shmem_template_collect_flat( T* target, const T* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    /* Gather */

    int ROOT = PE_start;
    shmem_template_gather_flat_put( target, source, nelems, PE_start, logPE_stride, PE_size, pSync, ROOT );
#ifdef _DEBUG
    if( myInfo.getRank() == ROOT ) {
        std::cout << "Gathered buff: " << std::endl;
        for( int i = 0 ; i < myInfo.getSize() ; i++ ) {
            std::cout << target[i] << "  " ;
        }
        std::cout << std::endl;
    }
#endif
    
    /* In-place bradcast */

    shmem_broadcast_template( const_cast<T*>( target ), const_cast<const T*>( target ), nelems * PE_size, ROOT, PE_start, logPE_stride, PE_size, pSync );

#ifdef _DEBUG
    if( myInfo.getRank() == ROOT ) {
        std::cout << "Gathered buff: " << std::endl;
        for( int i = 0 ; i < myInfo.getSize() ; i++ ) {
            std::cout << target[i] << "  " ;
        }
        std::cout << std::endl;
    }
#endif
    
}

/* At each k-th step, read from rank r + 2^k
   If the number of PEs is odd, the last read contains only the necessary parts
   At the end: local shift
 */

template <class T, class V> void shmem_template_collect_bruck_get( T* target, const T* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, V* pSync ){



    int size = myInfo.getSize();
    int rank = myInfo.getRank();
    
    if( _shmem_unlikely( ( rank < PE_start ) ||
                         ( rank >= ( PE_start + ( PE_size * ( 0x1 << logPE_stride) ) ) ) ||
                         ( ( rank - PE_start ) % ( 0x1 << logPE_stride ) ) != 0 ) ) {
#ifdef _DEBUG
        std::cout << "Collect: Rank " << myInfo.getRank() << " is not part of the set of participating PEs." << std::endl;
#endif
        return;
    }

    Collective_t* coll = myInfo.getCollective();
    Collective_t* remote_coll;

    int distance;
    int buddy;
    size_t write_offset;
    int nb;
    int i;

    int stride = 0x1 << logPE_stride;

    int* the_end = (int*) shmalloc( sizeof( int ) );
    *the_end = 0;

    /* okay, I entered the collective */

    coll->inProgress = true;
    coll->cnt = 0;

    _shmem_memcpy( target, source, PE_size * nelems * sizeof( T ) );

    /* Now comes the real business */

    nb = 0x1;
    write_offset =  nelems * nb; 

    /* write my step number */

    coll->cnt = 0;
    buddy = 0;

    for( distance = 1 ; distance < PE_size ; distance <<= 1 ) {

        buddy = ( rank - PE_start + ( distance * stride ) ) % ( PE_size * stride ) + PE_start;
        std::cout << rank << "] my cnt:" << coll->cnt << std::endl;
        std::cout << rank << "] my buddy:" << buddy <<  " - distance: " << distance <<  "/" << size << std::endl;

        remote_coll = static_cast<Collective_t *>( _getRemoteAddr( coll, buddy ) );
        
        /* Is the remote guy ready? If not, wait.*/
            
        while( false == remote_coll->inProgress ) {
            usleep( SPIN_TIMER );
        }
        
        /* has the remote guy reached this step yet? */
        
        while( coll->cnt > remote_coll->cnt ) {
            usleep( SPIN_TIMER );
        }
        
        shmem_template_get( target + write_offset, target, nelems * nb, buddy );
        
#ifdef _DEBUG

        printf( "%d | GOT [ ", rank );
        for( int i = write_offset ; i < write_offset + nb * nelems ; i++ ) {
            printf( "%d ", (int) target[i] );
        }
        printf( "] - (fetched from %d at %d)\n", buddy, write_offset );


        printf( "%d | [ ", rank );
        for( int i = 0 ; i < PE_size * nelems ; i++ ) {
            printf( "%d ", (int) target[i] );
        }
        printf( "] - (step %d)\n", nb );
        
#endif

        nb <<= 1;
        coll->cnt =  nb;
        std::cout << rank << "] cnt is now:" << nb << std::endl;
        
        write_offset = nelems * nb;
        
    }

    /* write on my first buddy that I am done with it */

    //  buddy =  ( rank - PE_start + ( 1 * stride ) ) % ( PE_size * stride ) + PE_start;

    std::cout << rank << "] tell " << buddy << " I am done." << std::endl;
    shmem_template_p( the_end, (int)1, buddy );

    while( *the_end == 0 ) {
        usleep( SPIN_TIMER );
        std::cout << rank << "] the end " << *the_end << std::endl;

    }

    /* local shift */
    
    std::rotate( target, target+(size-rank)*nelems, target+((size+1)*nelems-1) );

    // myInfo.collectiveReset();
    // shfree( the_end );
    printf( "%d | [ ", rank );
    for( int i = 0 ; i < PE_size * nelems ; i++ ) {
            printf( "%d ", (int) target[i] );
    }
    printf( "] - (END)\n" );
    std::cout << rank << "] done." << std::endl;

    coll->cnt = 0;
    coll->inProgress = false;
}

/* put-based implemenation of Bruck's algorithm
 * - put stuff in remote processes' buffer
 * - increment their lcoal counter
 * - as soon as this is done and our own local counter has reached the 
 * proper value, we are done.
 */

template <class T, class V> void shmem_template_collect_bruck_put( T* target, const T* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    int size = myInfo.getSize();
    int rank = myInfo.getRank();
    
    if( _shmem_unlikely( ( rank < PE_start ) ||
                         ( rank >= ( PE_start + ( PE_size * ( 0x1 << logPE_stride) ) ) ) ||
                         ( ( rank - PE_start ) % ( 0x1 << logPE_stride ) ) != 0 ) ) {
#ifdef _DEBUG
        std::cout << "Collect: Rank " << myInfo.getRank() << " is not part of the set of participating PEs." << std::endl;
#endif
        return;
    }

    Collective_t* coll = myInfo.getCollective();
    Collective_t* remote_coll;

    int distance;
    int buddy;
    size_t write_offset;
    int nb;
    int i;
    int nbsteps;
    int rc;

    int stride = 0x1 << logPE_stride;

    /* okay, I entered the collective */

    coll->cnt = 0;
    coll->inProgress = true;

    _shmem_memcpy( target, source, PE_size * nelems * sizeof( T ) );


    /* Now comes the real business */

    buddy = 0;
    nb = 0x1;
    write_offset =  nelems * nb; 
    nbsteps = 0;

    for( distance = 1 ; distance < PE_size ; distance <<= 1 ) {

        buddy = ( rank - PE_start + ( distance * stride ) ) % ( PE_size * stride ) + PE_start;
        std::cout << rank << "] my cnt:" << coll->cnt << std::endl;
        std::cout << rank << "] my buddy:" << buddy <<  " - distance: " << distance <<  "/" << size << std::endl;

        remote_coll = static_cast<Collective_t *>( _getRemoteAddr( coll, buddy ) );
        
       /* Am I ready for this guy? */

        if( nbsteps >= coll->cnt ) {
            /* no */
            usleep( SPIN_TIMER );
        }

        /* Push data into the remote process's memory */

        shmem_template_put( target + write_offset, source, nelems * nb, buddy );
        rc = shmem_int_finc( &(coll->cnt), buddy );

        std::cout << rank << "] incremented cnt to: " << rc+1 << " on " << buddy << std::endl;
        std::cout << rank << "] step "<< nbsteps  << ": write  " << nelems*nb << " elements on " << buddy << " at " << write_offset  << std::endl;

#ifdef _DEBUG
        printf( "%d | [ ", rank );
        for( int i = 0 ; i < nb * nelems ; i++ ) {
            printf( "%d ", (int) target[i] );
        }
        printf( "] - (written into %d at %d)\n", buddy, write_offset );

        printf( "%d | [ ", rank );
        for( int i = 0 ; i < PE_size * nelems ; i++ ) {
            printf( "%d ", (int) target[i] );
        }
        printf( "] - (total)\n" );
#endif

        nbsteps++;

        nb <<= 1;
        write_offset = nelems * nb;

    }

    /* Now we are done writing into our buddys' memory, how many processes
       have written into our own memory? */

    std::cout << rank << "] nb steps: " << nbsteps  << std::endl;
    std::cout << rank << "] cnt: " << coll->cnt << std::endl;

    while( coll->cnt < nbsteps ){
        usleep( SPIN_TIMER );
    }

    /* local shift */
    
    std::rotate( target, target+(size-rank)*nelems, target+((size+1)*nelems-1) );

    /* Done! */

    printf( "%d | [ ", rank );
    for( int i = 0 ; i < PE_size * nelems ; i++ ) {
            printf( "%d ", (int) target[i] );
    }
    printf( "] - (END)\n" );
    std::cout << rank << "] done." << std::endl;

    coll->cnt = 0;
    coll->inProgress = false;
}


template <class T, class V> void shmem_template_gather_flat_put( T* target, const T* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, V* pSync, int root ){

    int myrank = myInfo.getRank();
    int mysize = myInfo.getSize();
    Collective_t* coll = myInfo.getCollective();
    bool rc;

    if( _shmem_unlikely( ( myrank < PE_start ) ||
                         ( myrank >= ( PE_start + ( PE_size * ( 0x1 << logPE_stride) ) ) ) ||
                         ( ( myrank - PE_start ) % ( 0x1 << logPE_stride ) ) != 0 ) ) {
#ifdef _DEBUG
        std::cout << "Collect: Rank " << myInfo.getRank() << " is not part of the set of participating PEs." << std::endl;
#endif
        return;
    }

    coll->inProgress = true;
    coll->cnt = 0;

    if( root == myrank ) {
        
        _shmem_memcpy( target, source, nelems * sizeof( T ) );

        while( coll->cnt != ( mysize - 1 ) ) {
            usleep( SPIN_TIMER );
#ifdef _DEBUG
            printf( "cnt = %d\n", coll->cnt );
#endif
        }

    } else {
        int offset = nelems * ( myrank - PE_start ) / ( 0x1 << logPE_stride );
#if _DEBUG
        std::cout << "[" << myrank << "] offset : " << offset << std::endl;
#endif
              
        /* put my data (no concurrency in my slice of the target -> no mutex necessary) */

        shmem_template_put( target + offset, source, nelems, root );
        int k = shmem_template_finc( &(coll->cnt), root );

    }

    coll->inProgress = false;
}


template <class T, class V> void shmem_template_fcollect( T* target, const T* source, size_t nelems, int PE_start, int logPE_stride, int PE_size, V* pSync ){
    shmem_template_collect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}
