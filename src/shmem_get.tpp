#include <string.h>

#include "shmem_internal.h"

template<class T> T shmem_template_g( T* addr, int pe ){
    T result;
    shmem_template_get( &result, addr, 1, pe );
    return result;
}


template<class T> void shmem_template_get( T* target, const T* source, size_t nb, int pe ){

    const void* buf = NULL;

    switch( myInfo.getRemoteCommType( pe )) {

        case SHARED_MEMORY_COMM : /* -> mémoire partagée */
            /* Open remote PE's symmetric heap */
            buf = const_cast<const void*>(_getRemoteAddr( reinterpret_cast<void*>( const_cast<T*>( source ) ), pe ) );
            /* Pull the data here */
            _shmem_memcpy( reinterpret_cast<void *>( target ), buf, nb * sizeof( T ) );
            break;

        case OPEN_MX_COMM:  /* -> mémoire distribuée */
        //   ked_get( .... );
            break;

        default:
        std::cerr << "Unknown communication type " << myInfo.getRemoteCommType( pe ) << std::endl;
    }
    
}


template<class T> void shmem_template_iget( T* target, const T* source,  ptrdiff_t tst, ptrdiff_t sst, size_t nb, int pe ){

    /* Open remote PE's symmetric heap */

    const void* buf = const_cast<const void*>(_getRemoteAddr( reinterpret_cast<void*>( const_cast<T*>( source ) ), pe ) );

    /* Pull the data */

    size_t i;
    char* dst;
    const char* src;
    for( i = 0 ; i < nb ; i++ ) {

        dst = reinterpret_cast<char*>( target );
        dst += tst * i * sizeof( T );

        src = reinterpret_cast<const char*>( buf );
        src += sst * i * sizeof( T );

        _shmem_memcpy( reinterpret_cast<void *>( dst ), static_cast<const void*>( src ), sizeof( T ) );
    }

}
