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

#ifndef _SHMEM_INTERNAL_H
#define _SHMEM_INTERNAL_H

#include <iostream>
#include <cstdlib>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp> 
#include <signal.h>

#include "shmem_constants.h"
#include "shmem_processinfo.h"
#include "shmem_utils.h"

using namespace boost::interprocess;


/*#include <setjmp.h>
  extern jmp_buf return_to_top_level;*/

typedef struct Collective_t { 

    bool inProgress;      /* are we currently in the collective operation? */
    int type;             /* Type of collective operation */
    int cnt;              /* How many processes are taking part *locally* (ie, from my own point of view) */
    void* ptr;            /* pointer to the segment of data in the symmetric heap */
#if _DEBUG
    size_t space;         /* size of the allocated space */
#endif

} Collective_t;


class MeMyselfAndI {

 private:
    int shmem_rank;
    int shmem_size;
    int shmem_started;

    Collective_t* collective;

    managed_shared_memory* neighbors;
    std::map<long, boost::interprocess::named_mutex*> locks;

   public:
    
   // le constructeur et le destructeur devraient etre prives pour faire de cette classe un singleton
    
    /* Constructor and destructor */

    MeMyselfAndI(){
        this->shmem_rank = -1;
        this->shmem_size = -1;
        this->shmem_started = false;
        this->collective = NULL;
    }
    ~MeMyselfAndI(){
        delete[] neighbors;
    }
 
 public:

    /* Accessors */

    int getRank( void );
    void setRank( int );
    int getSize( void );
    void setSize( int );
    bool getStarted( void );
    void setStarted( bool );
    Collective_t* getCollective( void );
    managed_shared_memory* getNeighbors( void );
    managed_shared_memory* getNeighbor( int );
    boost::interprocess::named_mutex* getLock( long );
    void setLock( long, boost::interprocess::named_mutex* );

    /* Methods */

    void findAndSetMyRank( void );
    void findAndSetMySize ( void ); 
    void findAndSetPidRoot( void );
    void setRootPid( int );

    unsigned char getCollectiveType( void );
    void setCollectiveType( unsigned char );
    void* allocCollectiveSpace( size_t );
    void* getCollectiveBuffer( void );
    void collectiveInit( void );
    void collectiveReset( void );

    void allocNeighbors( int nb );
    void* getRemoteHeapBaseAddr( int );
    void* getRemoteHeapLocalBaseAddr( int );

};

extern MeMyselfAndI myInfo;


class SymmetricHeap{
 private:
    char* heapBaseName;
    char* heapName;
    unsigned long long int heapSize;
    uintptr_t baseAddr;
    managed_shared_memory::handle_t offset_handle;

 public:
    managed_shared_memory myHeap;

    // TODO : a voir si avec un managed_heap_memory ou un shared_memory_object ca pourrait le faire

   public:
   // le constructeur et le destructeur devraient etre prives pour faire de cette classe un singleton

   /* Constructor and destructor */

    SymmetricHeap(  ) {
        asprintf( &( this->heapBaseName ), "pSHMEM_SymmetricHeap_" );
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

    /* Accessors */

    void setSize( unsigned long long int );
    unsigned long long int getSize();

    void setHeapName();
    void setHeapName( int );

    char* getHeapName();

    uintptr_t getBaseAddr();
    managed_shared_memory::handle_t getOffsetHandle(){ return this->offset_handle; }

    /* Operators */
};

extern SymmetricHeap myHeap;

/* Internal functions */

void* _shmallocFake( size_t );
void* _remote_shmallocFake( int, size_t );

#if 0
void sigBarrierHandler( int );
void sigBcastHandler( int );
void _shmem_bcastSignal( int );
#endif

bool _sharedMemEsists( char* );


inline void* _getRemoteAddr( const void* addr, int pe ) {

    managed_shared_memory* remote;
    remote = myInfo.getNeighbor( pe );
    
    managed_shared_memory::handle_t myHandle = myHeap.myHeap.get_handle_from_address( addr );
    return remote->get_address_from_handle( myHandle );
}


#endif
