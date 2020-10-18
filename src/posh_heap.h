/*
 * Copyright (c) 2014-2019 LIPN - Universite Paris 13
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
 */

#ifndef _POSH_HEAP_H_
#define _POSH_HEAP_H_

#include "posh_sm.h"
#include "shmem_constants.h"
#include "shmem_internal.h"

namespace bip = boost::interprocess;

#define HEAPBASENAME "POSH_SymmetricHeap_"

class SymmetricHeap{
 private:
    char* heapBaseName;
    char* heapName;
    unsigned long long int heapSize;
    uintptr_t baseAddr;
    bool inuse;
    bip::managed_shared_memory::handle_t offset_handle;

 public:
    bip::managed_shared_memory myHeap;

    // TODO : a voir si avec un managed_heap_memory ou un shared_memory_object ca pourrait le faire

   public:
   // le constructeur et le destructeur devraient etre prives pour faire de cette classe un singleton

   /* Constructor and destructor */

    SymmetricHeap(  ) {
        asprintf( &( this->heapBaseName ), HEAPBASENAME );
        this->heapName = NULL;
        this->heapSize = _SHMEM_DEFAULT_HEAP_SIZE;
    }

    ~SymmetricHeap(){
        this->deleteSharedHeap();
        if( NULL != this->heapBaseName ) {
            free( this->heapBaseName );
            this->heapBaseName = NULL;
        }
        if( NULL != this->heapName ) {
            free( this->heapName );
            this->heapName = NULL;
        }
    }

   /* Methods */

    void setupSymmetricHeap( void );
    void createSharedHeap( unsigned long long int ) ;
    void deleteSharedHeap( void );
    char* buildHeapName( int );
    void* heapAlloc( size_t );
    void* shmemalign( size_t, size_t );
    void* shrealloc( void*, size_t );
    void shfree( void* );

    /* Accessors */

    void setSize( unsigned long long int );
    unsigned long long int getSize();

    void setHeapName();
    void setHeapName( int );

    char* getHeapName();

    uintptr_t getBaseAddr();
    bip::managed_shared_memory::handle_t getOffsetHandle(){ return this->offset_handle; }

    bool useSharedHeap(){ return this->inuse; }
    void setUseSharedHeap( bool b ) { this->inuse = b; }

    /* Operators */
};

extern SymmetricHeap myHeap;
extern MeMyselfAndI myInfo;

bool _sharedMemEsists( char* );

inline void* _getMyBaseAddress( ) {
    bip::managed_shared_memory::handle_t handle0 = 0;
    return myHeap.myHeap.get_address_from_handle( handle0 );
}

inline void* _getRemoteBaseAddress( int pe ) {
    
    bip::managed_shared_memory::handle_t handle0 = 0;
    bip::managed_shared_memory* remote;
    /* quite unsafe -- should return -1 if the remote process is a TCP neighbor */
    Communication_SM_t* sm = (Communication_SM_t*) myInfo.getNeighbor( pe )->communications;
    remote = &( sm->remoteHeap );
    
    //    remote = &(myInfo.getNeighbor( pe )->comm_channel.sm_channel.shared_mem_segment);
    return remote->get_address_from_handle( handle0 );
}

inline void* _getLocalAddr( const size_t offset ) {
    bip::managed_shared_memory::handle_t handle0 = 0;
    return (void*)( (char*)myHeap.myHeap.get_address_from_handle( handle0 ) + offset);
}

inline size_t _getOffset( const void* addr ) {
    size_t offset;
    bip::managed_shared_memory::handle_t myHandle = myHeap.myHeap.get_handle_from_address( (char*)addr );
    offset = ( (char*) myHeap.myHeap.get_address_from_handle( myHandle ) - (char*)_getMyBaseAddress() );
    // offset = (char*) addr - (char*)_getMyBaseAddress(); // est-ce que ça casse des choses ça ? 
   //   printf( "address %p handle %p base address %p offset %d\n", addr, _getMyBaseAddress(),  myHeap.myHeap.get_address_from_handle( myHandle ), offset );
    return offset;
}

inline void* _getSharedMemRemoteAddr( const void* addr, int pe ) {
    return (void*)((char*)_getRemoteBaseAddress( pe ) + _getOffset( addr ));
}

/* this is just an alias */
inline void* _getRemoteAddr( const void* addr, int pe ) {
    return _getSharedMemRemoteAddr( addr, pe );
}

inline bip::managed_shared_memory* _getMyHeap(){
    return &(myHeap.myHeap);
}

bool checkSize( size_t );

#endif // _POSH_HEAP_H_
