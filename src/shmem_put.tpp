#include <cstring> // memset() 

#include "shmem_internal.h"

#ifdef _DEBUG
#include "shmem_get.h"
#endif


template<class T> void shmem_template_p( T* addr, T value, int pe ){
    shmem_template_put( addr, &value, 1, pe );
}


template<class T> void shmem_template_put( T* addr, const T* value, size_t nb, int pe ){
    /* TODO: this is atomic. OpenSHMEM's specs say that it can be non-blocking 
       in order to be able to be overlapped by something else */
    T* buf = NULL;
    switch( myInfo.getRemoteCommType( pe ) ) {

        case SHARED_MEMORY_COMM : /* -> mémoire partagée */
            /* Open remote PE's symmetric heap */
            buf = (T*) _getRemoteAddr( addr, pe );
            /* Put our data over there */
            _shmem_memcpy( reinterpret_cast<void *>( buf ), reinterpret_cast<const void *>( value ), nb*sizeof( T ) );
            break;

        case OPEN_MX_COMM:  /* -> mémoire distribuée */
        //   ked_put( .... );
            break;

        default:
            std::cerr << "Unknown communication type " << myInfo.getRemoteCommType( pe )  <<std::endl;
    }
    
}


template<class T> void shmem_template_iput( T* target, const T* source,  ptrdiff_t tst, ptrdiff_t sst, size_t nb, int pe ){

    /* Open remote PE's symmetric heap */

     T* buf = (T*) _getRemoteAddr( target, pe );

    /* Put our data over there */

    size_t i;
    char* dst;
    const char* src;
    for( i = 0 ; i < nb ; i++ ) {
        dst = reinterpret_cast<char*>( buf );
        dst += tst * i * sizeof( T );

        src = reinterpret_cast<const char*>( source );
        src += sst * i * sizeof( T );

        _shmem_memcpy( reinterpret_cast<void *>( dst ), reinterpret_cast<const void*>( src ), sizeof( T ) );

        //        _shmem_memcpy( static_cast<void *>( &( buf [tst * i]) ), static_cast<const void*>( &( source[ sst * i ] ) ), sizeof( T ) );

#ifdef _DEBUG
        std::cout << "put " << ((const T*)(source))[ sst * i ] << " on " << pe << std::endl;
#endif
    }

#ifdef _DEBUG
    /* What did I just put over there? DEBUG */

    T* buf2 = (T*)malloc( sizeof( T ) ); ;

    for( i = 0 ; i < nb ; i++ ) {
        shmem_template_get( buf2, &(buf[ sst * i ]), 1, pe );
        std::cout << "--> " << myInfo.getRank() << " put " << *buf2 << " on " << pe << std::endl;
    }
#endif

}
