#include "shmem_internal.h"
#include "shmem.h"
#include "shmem_put.h"
#include "shmem_get.h"


template <class T, class V>void shmem_broadcast_template( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync ){

#ifdef SHMEM_COLL_FLAT
    //    shmem_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    //    shmem_broadcast_template_flat_get_blocking( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    shmem_broadcast_template_flat_put_blocking( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
#else
    #warning "No collective operation algorithm selected -- using flat by default"
    //shmem_broadcast_template_flat_put_blocking( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
shmem_broadcast_template_binomial_put_blocking( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );

#endif

}


bool _shmem_is_remote_process_in_coll( int rank );
void _shmem_wait_until_entered( int rank ); 
bool _shmem_part_of_coll( int rank, int PE_start, int PE_size, int logPE_stride );
int _shmem_binomial_binbase_size( int nb ) ;
int _shmem_binomial_myfather( int PE_root, int PE_start, int logPE_stride, int PE_size ) ;
int _shmem_binomial_children( int rank, int size );

/* Various implementations */

/* The difference between the blocking and non-blocking implementations 
   comes when a process has to write into the memory of a remote process 
   and this latter process has not entered the collective commnucation yet.
   - in the non-blocking case, the former performs a remote mem allocation
   and writes, hence anticipating the moment when the remote process will
   enter the call;
   - in the blocking case, it waits until the remote process is ready.
   Blocking implementations are much simpler.
 */

template <class T,class V>void shmem_broadcast_template_flat_put_blocking( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    int myrank = myInfo.getRank();
    int size = myInfo.getSize();
    bool rc;

    Collective_t* coll = myInfo.getCollective();

    myInfo.getCollective()->inProgress = true;

    if( _shmem_unlikely( myrank == PE_root ) ) {
        /* I am the root of the broadcast. Write... */

        int remoteRank, nb;
        nb = 0;
        remoteRank = PE_start;

        while( nb < PE_size ) {

            _shmem_wait_until_entered( remoteRank );

            /* TODO: Possible optimization: write everywhere it is possible, and put
               the ranks of those that are not ready in a vector. Then loop on this vector
               until it is empty.
             */

            if( _shmem_likely( remoteRank != myrank ) ) {
                shmem_template_put( target, source, nelems, remoteRank );
                shmem_int_finc( &(coll->cnt), remoteRank );
            } else {
                _shmem_memcpy( target, source, nelems*sizeof( T ) );
            }

            nb++;

            remoteRank += ( 0x01 << logPE_stride );
            if( _shmem_unlikely( remoteRank >= size ) ) {
                remoteRank = remoteRank % size;
            }
        }
        
    } else {
        
        /* All I have to do is wait until the root has written into my memory */
         /* Am I part of the subset of participating processes ? */

        if( _shmem_unlikely( _shmem_part_of_coll( myrank, PE_start, PE_size, logPE_stride ) ) ){
            return;
        }

       while( myInfo.getCollective()->cnt != 1 ) {
            usleep( SPIN_TIMER );
        }
        
    }
    myInfo.getCollective()->inProgress = false;
}

template <class T,class V>void shmem_broadcast_template_flat_get_blocking( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    int myrank = myInfo.getRank();
    int size = myInfo.getSize();
    bool rc;

    Collective_t* coll = myInfo.getCollective();
    coll->inProgress = true;
    coll->cnt = 0;

    if( _shmem_unlikely( myrank == PE_root ) ) {

        /* I am the root of the broadcast. Wait until everyone has fetched data */

        _shmem_memcpy( target, source, nelems*sizeof( T ) );

        while( myInfo.getCollective()->cnt != (PE_size - 1) ) {
            usleep( SPIN_TIMER );
        }

    } else {

        /* Am I part of the subset of participating processes ? */

        if(  _shmem_unlikely( _shmem_part_of_coll( myrank, PE_start, PE_size, logPE_stride ) )) {
            return;
        }

        /* Is the root process ready yet? */

        _shmem_wait_until_entered( PE_root );

        /* Get data from the root */

        shmem_template_get( target, source, nelems, PE_root );
        shmem_int_finc( &(coll->cnt), PE_root );
        
    }

    coll->inProgress = true;
    coll->cnt = 0;

}

template <class T,class V>void shmem_broadcast_template_binomial_get_blocking( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    /* TODO : mettre des locks */

    int myrank = myInfo.getRank();
    int size = myInfo.getSize();
    bool rc;

    /* Am I part of the subset of participating processes ? */
    
    if(  _shmem_unlikely( _shmem_part_of_coll( myrank, PE_start, PE_size, logPE_stride ) )) {
        return;
    }

    Collective_t* coll = myInfo.getCollective();
    myInfo.getCollective()->cnt = -1;
    myInfo.getCollective()->inProgress = true;

    /* Am I the root process? */

    if( _shmem_unlikely( myrank == PE_root ) ) {
        _shmem_memcpy( target, source, nelems*sizeof( T ) );
        myInfo.getCollective()->cnt = 0;

    } else {

        int myfather = _shmem_binomial_myfather( PE_root, PE_start, logPE_stride, PE_size );
        int translated_rank = ( myrank - PE_start ) / ( 0x1 << logPE_stride );
        int nb_of_children = _shmem_binomial_children( translated_rank, PE_size );

        /* Is my father ready to be accessed yet? */
    
        _shmem_wait_until_entered( myfather );

        /* Get data from my father */

        shmem_template_get( target, source, nelems, myfather );
        shmem_int_finc( &(coll->cnt), myfather );
        
        /* Wait until all my children have fetched data from me */

        while( myInfo.getCollective()->cnt != nb_of_children ) {
            usleep( SPIN_TIMER );
        }

    }

    myInfo.getCollective()->inProgress = false;

}

template <class T,class V>void shmem_broadcast_template_binomial_put_blocking( T* target, const T* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, V* pSync ){

    /* TODO : mettre des locks */

    int myrank = myInfo.getRank();
    int size = myInfo.getSize();
    bool rc;
    int child;

    /* Am I part of the subset of participating processes ? */
    
    if(  _shmem_unlikely( _shmem_part_of_coll( myrank, PE_start, PE_size, logPE_stride ) )) {
        return;
    }

    Collective_t* coll = myInfo.getCollective();
    myInfo.getCollective()->cnt = 0;
    myInfo.getCollective()->inProgress = true;

    /* Am I the root process? */

    if( _shmem_unlikely( myrank == PE_root ) ) {
        _shmem_memcpy( target, source, nelems*sizeof( T ) );
        myInfo.getCollective()->cnt ++;
    } 

    /* Wait until the data is available in my local target buffer */

    while( myInfo.getCollective()->cnt != 1 ) {
        usleep( SPIN_TIMER );
    }

    /* Compute my children's ranks */

    int translated_rank = ( myrank - PE_start ) / ( 0x1 << logPE_stride );
    int nb_of_children = _shmem_binomial_children( translated_rank, PE_size );
    int mychild;

    int offset = _shmem_binomial_binbase_size( translated_rank );
    for( child = 0 ; child < nb_of_children ; child++ ) {
        mychild = translated_rank | ( 0x1 << ( offset++ ) ) ;
        mychild = mychild * 0x1 << logPE_stride + PE_start;

        /* Put the data and increment their local counter */
        shmem_template_put( target, source, nelems, mychild );
        shmem_int_finc( &(coll->cnt), mychild );

    }

    myInfo.getCollective()->inProgress = false;
    myInfo.getCollective()->cnt = 0;

}
