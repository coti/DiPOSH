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

#ifndef _SHMEM_INTERNAL_H
#define _SHMEM_INTERNAL_H

#include <iostream>
#include <cstdlib>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp> 
#include <boost/thread.hpp> 
#include <signal.h>
#include <arpa/inet.h>
#include <atomic>

#include "shmem_constants.h"
#include "shmem_processinfo.h"
#include "shmem_utils.h"
#include "posh_communication.h"
#include "posh_contactinfo.h"
#include "posh_neighbor.h"

#include "posh_launcher.h"

#ifdef _WITH_TCP
#include "posh_tcp.h" // this will go away with the TCPendpoint_t
#endif // _WITH_TCP
//#include "posh_heap.h"
#ifdef _WITH_KNEM
#include "posh_knem.h"
#endif // _WITH_KNEM
#ifdef _WITH_NMAD
#include "posh_nmad.h"
#endif // _WITH_NMAD
#ifdef MPIHUBCHANNEL
#include "posh_hub.h"
#endif // _WITH_HUB
#ifdef MPICHANNEL
#include "posh_mpi.h"
#endif // _WITH_MPI

//#include <boost/stacktrace.hpp>

#ifndef MPICHANNEL
#define TCPCHANNEL
#endif

using namespace boost::interprocess;


#if defined( DISTRIBUTED_POSH ) && ( defined( MPICHANNEL ) || defined( MPIHUBCHANNEL ) )
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
using namespace boost::mpi;
namespace mpi = boost::mpi;
#endif

/*#include <setjmp.h>
  extern jmp_buf return_to_top_level;*/

struct Collective_t { 

    bool inProgress;      /* are we currently in the collective operation? */
    int type;             /* Type of collective operation */
    int cnt;              /* How many processes are taking part *locally* (ie, from my own point of view) */
    void* ptr;            /* pointer to the segment of data in the symmetric heap */
#if _DEBUG
    size_t space;         /* size of the allocated space */
#endif

};



class MeMyselfAndI {

 private:
    int shmem_rank;
    int shmem_size;
    int shmem_started;

    Collective_t* collective;

    //    managed_shared_memory* neighbors;
    Neighbor_t* neighbors; /* FIXME Joel said to use a vector */

#ifdef _WITH_TCP
    int tcp_port;
    int listen_socket;
#endif // _WITH_TCP
    std::string hostname;
    
    Launcher_t *rte;     /* Whoever started me is here */
    
    //   std::vector<ContactInfo> myContactInfo;// TODO
    ContactInfo* myContactInfo;

    Endpoint_t* myEndpoint; // should be able to have several endpoints there
#if _WITH_TCP
    TCPendpoint_t myEndpointTCP; // TODO put this behind an interface
#endif
#ifdef _WITH_KNEM
    KNEMendpoint_t myEndpointKNEM;
#endif
#ifdef _WITH_NMAD
    NMADendpoint_t myEndpointNMAD;
#endif
#ifdef MPIHUBCHANNEL
    Endpoint_hub_t myEndpointHub;
#endif
#ifdef MPICHANNEL
    Endpoint_MPI_t myEndpointMPI;
#endif

#ifdef CHANDYLAMPORT
    bool _checkpointing;
#endif
    
    std::map<long, boost::interprocess::named_mutex*> locks;

   public:

#if defined( MPICHANNEL ) || defined( MPIHUBCHANNEL )
    MPI_Win window;
    mpi::environment env;
    mpi::communicator world;
#endif

   // le constructeur et le destructeur devraient etre prives pour faire de cette classe un singleton
    
    /* Constructor and destructor */

    MeMyselfAndI(){

        char mybighostname[1024];

        this->shmem_rank = -1;
        this->shmem_size = -1;
        this->shmem_started = false;
        this->collective = NULL;

        mybighostname[1023] = '\0';
        gethostname( mybighostname, 1024 );
        this->hostname = std::string( mybighostname );
#ifdef _WITH_TCP
        this->listen_socket = -1;
#endif // _WITH_TCP
        
#if defined( MPICHANNEL ) || defined( MPIHUBCHANNEL )
        mpi::environment env( mpi::threading::funneled );
        std::cout << "thread level: " << this->env.thread_level() << std::endl;
#endif
        
    }
    ~MeMyselfAndI(){
        // delete[] neighbors;
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
    Neighbor_t* getNeighbors( void );
    Neighbor_t* getNeighbor( int );
    boost::interprocess::named_mutex* getLock( long );
    void setLock( long, boost::interprocess::named_mutex* );
#ifdef CHANDYLAMPORT
    bool getCheckpointingState( void ) { return this->_checkpointing; }
#endif
    
    /* Methods */

    void initRte( void );
    
    void findAndSetMyRank( void );
    void findAndSetMySize ( void ); 
    void findAndSetPidRoot( void );
    void setRootPid( int );
    Launcher_t * getRTE( void) { return this->rte ; }

    unsigned char getCollectiveType( void );
    void setCollectiveType( unsigned char );
    void* allocCollectiveSpace( size_t );
    void* getCollectiveBuffer( void );
    void collectiveInit( void );
    void collectiveReset( void );

    void allocNeighbors( int );
    void initNeighbors( int, char* );
    void* getRemoteHeapBaseAddr( int );
    void* getRemoteHeapLocalBaseAddr( int );

    void communicationInit( char* );

    void initNeighborSM( int );
    std::string getmyhostname( void );
#ifdef _WITH_TCP
    void initNeighborTCP( int, ContactInfo_TCP& );
    int getmyTCPport( void );
    void setMyTCPthreadID( boost::thread& );
    boost::thread* getMyTCPthreadID( void );
#endif // _WITH_TCP
#ifdef _WITH_KNEM /* FIXME: use an object */
    void initNeighborKNEM( int, ContactInfo_KNEM& );
    KNEMendpoint_t* getMyEndpointKNEM();
#endif
    void initNeighborNULL( int );
#ifdef MPICHANNEL
    void initNeighborMPI( int );
#endif
#ifdef MPIHUBCHANNEL
    void initNeighborHub( int );
    Endpoint_hub_t* getMyEndpointHub();
#endif
#ifdef _WITH_NMAD
    void initNeighborNMAD( int, ContactInfo_NMAD& );
    NMADendpoint_t* getMyEndpointNMAD();
#endif

    Endpoint_t* getMyEndpoint() { return this->myEndpoint; }
    
#ifdef DISTRIBUTED_POSH
    void setMyContactInfo( int, uint32_t, uint16_t );
    void setMyContactInfo( int );
    void setMyContactInfo( knem_cookie_t );
    ContactInfo* getMyContactInfoP( void );
#endif

#ifdef CHANDYLAMPORT
    void posh_close_communication_channels( void );
    void posh_reopen_communication_channels( void );
    void closeNeighbor( int );
    void enterCheckpointing( void );
    void exitCheckpointing( void );
#endif
};


extern MeMyselfAndI myInfo;

/* Internal functions */

void* _shmallocFake( size_t );
void* _remote_shmallocFake( int, size_t );

#if 0
void sigBarrierHandler( int );
void sigBcastHandler( int );
void _shmem_bcastSignal( int );
#endif


/* FIXME WTF?? */

inline neighbor_comm_type_t _getNeighborComm( int pe ){
    return myInfo.getNeighbor( pe )->comm_type;
}

#define TIMEDIFF( ts, te )       (te.tv_sec - ts.tv_sec)*1000000000 + (te.tv_nsec - ts.tv_nsec)

#ifdef CHANDYLAMPORT

/* Put the marker in a dedicated segment of shared memory */

class Checkpointing {

 private:
    string markerbasename = "POSH_marker";
    char* markermemname = NULL;
    managed_shared_memory markermem;
    std::atomic<int>* marker;

 public:

    /* Constructor and destructor */

    Checkpointing(){
        initMemName();
    }
    ~Checkpointing(){
        shared_memory_object::remove( markermemname );
        if( NULL != markermemname ){
            free( markermemname );
        }
    }
    
    /* Methods: local */
 private:
    void initMemName();
 public:
    void initMarker();
    int getMarker();
    void resetMarker();
    
    /* Methods: remote */
 private:
    void initMemName( int );
 public:
    void sendMarker( int );
    void coordinate( void );
    void recvMarker( void );
};
    
extern Checkpointing checkpointing;
    
#endif // CHANDYLAMPORT


#endif
