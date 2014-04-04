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

#include <string.h>

#include "shmem_internal.h"

template<class T> T shmem_template_g( T* addr, int pe ){
    T result;
    shmem_template_get( &result, addr, 1, pe );
    return result;
}


template<class T> void shmem_template_get( T* target, const T* source, size_t nb, int pe ){

    /* Open remote PE's symmetric heap */

    const void* buf = const_cast<const void*>(_getRemoteAddr( reinterpret_cast<void*>( const_cast<T*>( source ) ), pe ) );

#ifdef _DEBUG
    std::cout << "--> "<< myInfo.getRank() << " mapped buff addr " << buf << " into " << target << std::endl;
#endif

    /* Pull the data here */

#ifdef _DEBUG
    void* rc;
    rc = 
#endif

    _shmem_memcpy( reinterpret_cast<void *>( target ), buf, nb * sizeof( T ) );

#ifdef _DEBUG
    std::cout << "--> "<< myInfo.getRank() << " cp addr " << buf << " into " << target << " with rc = " << rc << std::endl;
    std::cout  << "--> "<< myInfo.getRank() << " get " << (T)*target << " from " << buf << std::endl;
#endif
    
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
