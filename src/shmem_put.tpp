/*
 *
 * Copyright (c) 2014 LIPN - Universite Paris 13
 *                    All rights reserved.
 *
 * This file is part of POSH.
 * 
 * POSH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * POSH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with POSH.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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

    /* Open remote PE's symmetric heap */

    T* buf = (T*) _getRemoteAddr( addr, pe );

    /* #ifdef _DEBUG
    std::cout << myInfo.getRank() << " put " << (T)*value << " into " << buf << std::endl;
    #endif */
    
    /* Put our data over there */

    _shmem_memcpy( reinterpret_cast<void *>( buf ), reinterpret_cast<const void *>( value ), nb*sizeof( T ) );

    /* #ifdef _DEBUG */
    /* What did I just put over there? DEBUG */

    /* T* buf2 = (T*)malloc( sizeof( T ) ); ;

    shmem_template_get( buf2, addr, nb, pe );
    std::cout << "--> " << myInfo.getRank() << " put " << *buf2 << " on " << pe << std::endl;
#endif */

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
