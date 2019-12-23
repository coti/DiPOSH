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

#include "shmem_internal.h"

#include "posh_heap.h"

/* Methods */

void SymmetricHeap::setupSymmetricHeap( ){
    setHeapName();
    createSharedHeap( this->heapSize );
}

void SymmetricHeap::createSharedHeap( unsigned long long int size ) {
    this->heapSize = size;
    if( NULL == this->heapName ) setHeapName();
    this->deleteSharedHeap(  );
    shared_memory_object::remove( this->heapName ); 
    /*    boost::interprocess::permissions perm( );
    perm.set_unrestricted( ); */
    this->myHeap = managed_shared_memory ( /* open_or_create */ create_only, this->heapName, this->heapSize /*, &perm*/ );


#ifdef _DEBUG
    int* buf = (int*) this->myHeap.get_address_from_handle( 0 );
    try { 
        int* buf = static_cast<int*>(this->myHeap.allocate( this->heapSize / 8 ) );
        std::memset( buf, 0xDEADBEEF, this->heapSize / 8 );
        this->myHeap.deallocate( buf );
    } catch (boost::interprocess::bad_alloc &ex) {
        std::cerr << ex.what() << std::endl; 
    } 
#endif

    /* Init base addr */

    this->offset_handle = 0;
    this->baseAddr =  reinterpret_cast<uintptr_t>( this->myHeap.get_address_from_handle( 0 ) );

}

void SymmetricHeap::deleteSharedHeap( ){
    shared_memory_object::remove( this->heapName );
}

char* SymmetricHeap::buildHeapName( int _rank ) {
    char* _name;
    asprintf( &_name, "%s_%d", this->heapBaseName, _rank );
    return _name;
}

/* Accessors */

void SymmetricHeap::setSize( unsigned long long int size ) {
    this->heapSize = size;
}

unsigned long long int SymmetricHeap::getSize() {
    return this->heapSize;
}

void SymmetricHeap::setHeapName() {
    int rank;
    
    if( NULL != this->heapName ) return;
    
    rank = myInfo.getRank( );
    if( -1 != myInfo.getRank( ) ) {
        myInfo.findAndSetMyRank();
        rank = myInfo.getRank( );
    }
    this->setHeapName( rank );
}

void SymmetricHeap::setHeapName( int rank ) {
    this->heapName = buildHeapName( rank );
}

char* SymmetricHeap::getHeapName(){
    return this->heapName;
}

uintptr_t SymmetricHeap::getBaseAddr(){
    return this->baseAddr ;
}

/* Internal tools */

bool _sharedMemEsists( char* name ) {
    try {
        managed_shared_memory shm( open_only, name );
    } catch( ... ) {
        return false;
    }
    return true;
}
