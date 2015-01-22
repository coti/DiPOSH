#include "shmem_atomic.h"
#include "shmem_bcast.h"

template<class T>void shmem_template_allgather_flat( T* buffer, const T* myelem, size_t nb ){
    int ROOT = 0;
    int i;

    int size = myInfo.getSize();

    shmem_template_gather_flat( buffer, myelem, 1, ROOT );

    if( myInfo.getRank() == ROOT ) {
        for( i = 0 ; i < myInfo.getSize() ; i++ ) {
            Process p( i, buffer[i] );
            processes.push_back( p );
#ifdef _DEBUG
            std::cout << "Process rank " << i << " is running under pid " << buffer[i] << std::endl;
#endif
        }
    }

    shmem_broadcast_template( buffer, buffer, size, ROOT, ROOT, 0, size, (char*)NULL );

}

template<class T>void shmem_template_gather_flat( T* buffer, const T* myelem, size_t nb, int root ){

    Collective_t* coll = myInfo.getCollective();

   if( myInfo.getRank() == root ) {

       /* Is there anythere here already? */

       boost::interprocess::named_mutex named_mtx( boost::interprocess::open_or_create, "mtx_shmem_gather"); 
       named_mtx.lock(); 
       if( true == coll->inProgress ) {
          
#if defined( _SAFE ) || defined( _DEBUG )

           /* Check if the collective that is already in progress 
              is matching the current collective type 
           */
           if( !( _SHMEM_COLL_GATHER == coll->type ) && !( _SHMEM_COLL_NONE == coll->type ) ) {
               std::cerr << "Wrong collective type: process rank " << myInfo.getRank();
               std::cerr << " attempted to participate to a gather operation ";
               std::cerr << "( code " << _SHMEM_COLL_GATHER <<" ) whereas another ";
               std::cerr << "collective operation is in progress (code ";
               std::cerr << coll->type << ")" << std::cerr;
               return;
           }
#endif

       } else {
           coll->inProgress = true;
           coll->type = _SHMEM_COLL_GATHER;
           coll->ptr = _shmallocFake( myInfo.getSize() * nb * sizeof( T ) );
#if defined( _SAFE ) || defined( _DEBUG )
           coll->space = myInfo.getSize() * nb * sizeof( T );
#endif
       }

        /* put my own element in the buffer */
        _shmem_memcpy( buffer, myelem, nb*sizeof( T ) );
        named_mtx.unlock(); 

        /* wait for all the other processes */
        while( myInfo.getCollective()->cnt != ( myInfo.getSize() - 1 ) ) {
            usleep( SPIN_TIMER );
            /*#ifdef _DEBUG
           std::cout << myInfo.getRank() << "] Recv " << myInfo.getCollective()->cnt << " sig coll" << std::endl;
#endif*/
        }

    } else {
        int beginIndex;

        /* Is the remote guy ready? */

        boost::interprocess::named_mutex named_mtx( boost::interprocess::open_or_create, "mtx_shmem_gather"); 

        named_mtx.lock(); 
        Collective_t* remote_coll = static_cast<Collective_t *>( _getRemoteAddr( coll, root ) );

        if( false == remote_coll->inProgress ) {
#ifdef _DEBUG
            std::cout << myInfo.getRank() << "] Root process " << root << " has not entered the collective operation yet." << std::endl;
#endif
            /* Bad luck: the remote guy has not entered the collective op yet.
               We need to prepare his collective space for him.
            */
            remote_coll->ptr = _remote_shmallocFake( root, myInfo.getSize() * nb * sizeof( T ) );
            remote_coll->type = _SHMEM_COLL_GATHER;
            remote_coll->cnt = 0;
            remote_coll->inProgress = true;
#if defined( _SAFE ) || defined( _DEBUG )
            remote_coll->space =  myInfo.getSize() * nb * sizeof( T );
#endif
        }
        named_mtx.unlock(); 

        shmem_int_finc( &(myInfo.getCollective()->cnt), root );

        beginIndex = myInfo.getRank() * nb;
        shmem_template_put( buffer+beginIndex, myelem, nb, root );
    }

   myInfo.collectiveReset();
}
