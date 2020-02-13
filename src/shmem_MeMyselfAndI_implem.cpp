/*
 * Copyright (c) 2014-2020 LIPN - Universite Paris 13
 *                              - Université Sorbonne Paris Nord
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
#include <fstream>

#ifdef CHANDYLAMPORT
#include <boost/filesystem.hpp>
#endif

#ifdef DISTRIBUTED_POSH
#include <limits.h>
//#include <boost/mpi.hpp>
//#include <boost/mpi/communicator.hpp>
//namespace mpi = boost::mpi;
//namespace boost { namespace mpi {
//        template <>
//        struct is_mpi_datatype<ContactInfo> : mpl::true_ { };
//    }
//}
#endif

#ifdef MPICHANNEL
#include "posh_mpi.h"
#endif
#ifdef MPIHUBCHANNEL
#include "posh_hub.h"
#endif
#include "posh_tcp.h"
#include "posh_heap.h"
#ifdef _WITH_KNEM
#include "posh_knem.h"
#endif // _WITH_KNEM
#if defined( MPILAUNCHER )
#include <boost/mpi/collectives.hpp>
#include "posh_launcher_mpi.h"
#endif

#ifdef PADICOLAUNCHER
extern "C" {
#include <nm_launcher_interface.h>
}
#include "posh_launcher_padico.h"
#endif // PADICOLAUNCHER

#include "posh_launcher_sm.h"

#ifdef DISTRIBUTED_POSH
#if 0  // replaced by doprint in each ContactInfo_* class
std::ostream& operator << ( std::ostream &out, ContactInfo_TCP& ci ){

    out << " " << ci.getRank() << " -- ";
    if( TYPE_TCP == ci.getType() ) {
        out << " " << ci.getHostname() << " ";
        
        char ipAddress[INET6_ADDRSTRLEN];
        uint32_t ip_int = ci.getAddr();      
        inet_ntop( AF_INET, (const void*)&ip_int, ipAddress, sizeof(ipAddress));      
        out << ipAddress << " " ;
        out << (int)ci.getPort();
    }
    return out;
}
#endif

std::ostream& operator<< ( std::ostream& out, Neighbor_t& n ){
    map<neighbor_comm_type_t, std::string> commtype;
    commtype[TYPE_TCP] = "TCP";
    commtype[TYPE_SM] = "Shared mem";
    commtype[TYPE_MPI] = "MPI";
    commtype[TYPE_NMAD] = "NMad";
    commtype[NONE] = "Unknown";

    out << "Neighbor " << n.rank;
    out << " - Type " << commtype[n.comm_type] << std::endl;
    out << "Contact information: " << std::endl;
    for( auto i = n.neigh_ci.begin() ; i != n.neigh_ci.end() ; ++i ){
        out << *i;
        out << std::endl;
    }
    return out;
}

#endif // DISTRIBUTED_POSH

std::map< neighbor_comm_type_t, unsigned int> channel_priorities = { { TYPE_SM, 100 },
                                                                            { TYPE_MPI, 50 },
                                                                            { TYPE_TCP, 5 },
                                                                            { TYPE_KNEM, 120 },
                                                                            { TYPE_NMAD, 80 } };
/* Accessors */

int MeMyselfAndI::getRank(){
    return this->shmem_rank;
}

void MeMyselfAndI::setRank( int _rank ){
    this->shmem_rank = _rank;
}

int MeMyselfAndI::getSize(){
    return this->shmem_size;
}

void MeMyselfAndI::setSize( int _size ){
    this->shmem_size = _size;
}

bool MeMyselfAndI::getStarted(){
    return this->shmem_started;
}

void MeMyselfAndI::setStarted( bool _started ){
    this->shmem_started = _started;
}

Neighbor_t* MeMyselfAndI::getNeighbors() {
    return this->neighbors;
}

Neighbor_t* MeMyselfAndI::getNeighbor( int rank ) {
    return &(this->neighbors[rank]);
}

boost::interprocess::named_mutex* MeMyselfAndI::getLock( long _lock ){
    boost::interprocess::named_mutex* mut =  this->locks[ _lock ];
    if( _shmem_unlikely( NULL == mut ) ) {
        char* name;
        asprintf( &name, "shmem_lock_%ld", _lock );
        boost::interprocess::named_mutex mtx(boost::interprocess::open_or_create, name );
        free( name );
        mut = &mtx;
        this->setLock( _lock, &mtx );
    }
    return mut;
}
void MeMyselfAndI::setLock( long _lock, boost::interprocess::named_mutex *_mtx ){
    this->locks[ _lock ] = _mtx;
}

/* Methods */

/* Initialize the interactions with the run-time environment */

void MeMyselfAndI::initRte( ){

    /* Who started me? */
    
    if( isLocal() ) {
        myInfo.rte = new Launcher_SM();
        return;
    }

#ifdef PADICOLAUNCHER
    if( isPadico() ) {
        myInfo.rte = new Launcher_Padico();
        return;
      }
#endif // PADICOLAUNCHER

#ifdef MPILAUNCHER
    if( isMPI() ) {
        myInfo.rte = new Launcher_MPI();
        return;
    }
#endif // MPILAUNCHER
        
}

void MeMyselfAndI::findAndSetMyRank( ){
    int rank;
    if( -1 != this->shmem_rank ) {
        return;
    } else {
        /* TODO use an object there */
        this->shmem_rank = rte->getRank();
        //      this->myContactInfo.setRank( rte->getRank() ); // FIXME do this later
    }
}

void MeMyselfAndI::findAndSetMySize () {
    if( -1 != this->shmem_size ) {
        return;
    } else {
        this->shmem_size = rte->getSize();
    }
}

void MeMyselfAndI::findAndSetPidRoot () {
    char* rootpid;
    if( 0 == myInfo.getRank() ) {
        asprintf( &rootpid, "%d", getpid() );
    } else {
        rootpid = getenv( VAR_PID_ROOT );
        if( NULL == rootpid ) {
            /* Not necessarily a problem (only in non-distributed mode) */
            //            std::cerr << "Could not retrieve the process id of the root process" << std::endl;
            return;
        }
    }

    this->setRootPid( atoi( rootpid ) );
#if _DEBUG
    std::cout << myInfo.getRank() << "] my father has pid " << atoi( rootpid ) << std::endl;
#endif

    if( 0 == myInfo.getRank() ) {
        free( rootpid );
    }
}

void MeMyselfAndI::setRootPid( int pid ) {
    Process pRoot( 0, pid );
    processes.push_back( pRoot );
}

std::string MeMyselfAndI::getmyhostname(){
    return this->hostname;
}

void MeMyselfAndI::allocNeighbors( int nb ) {
    this->neighbors = new Neighbor_t[nb];
}

/* This should go behind an interface */
void MeMyselfAndI::initNeighborNULL( int neighborrank ) {

    /* FIXME */
    
    this->neighbors[neighborrank].comm_type = NONE;
    std::cerr << "Unknown communication channel type received for " << neighborrank << std::endl;
}

void MeMyselfAndI::initNeighborSM( int neighborrank ) {
    this->neighbors[neighborrank].comm_type = TYPE_SM;
    this->neighbors[neighborrank].communications = new Communication_SM_t();    
    this->neighbors[neighborrank].communications->init( neighborrank );
#ifdef _DEBUG 
    std::cout << "Init neighbor  " << neighborrank << " SM" << std::endl;
#endif // _DEBUG
    
    /*    char* _name; 
    _name = myHeap.buildHeapName( neighborrank );
    while( false == _sharedMemEsists( _name ) ) {
        usleep( SPIN_TIMER );
    }
    managed_shared_memory remoteHeap(  open_only, _name );
    this->neighbors[neighborrank].comm_channel.sm_channel.shared_mem_segment = std::move( remoteHeap );
    this->neighbors[i]( open_only, _name );
    free( _name ); */
}

#ifdef MPICHANNEL

void MeMyselfAndI::initNeighborMPI( int neighborrank ) {


    if( channel_priorities[ this->neighbors[neighborrank].comm_type ] < channel_priorities[ TYPE_MPI ] ) {
        delete this->neighbors[neighborrank].communications;
        this->neighbors[neighborrank].communications = new Communication_MPI_t();
        this->neighbors[neighborrank].comm_type = TYPE_MPI;
    }



    /*
    this->neighbors[neighborrank].comm_type = TYPE_MPI;
    this->neighbors[neighborrank].communications = new Communication_MPI_t();
    */
    

    
#ifdef _DEBUG
    std::cout << "Init neighbor  " << neighborrank << " MPI" << std::endl;
#endif // _DEBUG
    /* Nothing to do */
}
#endif // MPICHANNEL


#ifdef _WITH_TCP
void MeMyselfAndI::initNeighborTCP( int neighborrank, ContactInfo_TCP& ci ) {

    //    this->neighbors[neighborrank].channels.push_back( Communication_TCP_t() );
    // TOOD: fill the list

    if( channel_priorities[ this->neighbors[neighborrank].comm_type ] < channel_priorities[ TYPE_TCP ] ) {
        delete this->neighbors[neighborrank].communications;
        this->neighbors[neighborrank].communications = new Communication_TCP_t();
        this->neighbors[neighborrank].communications->setContactInfo( ci );
        this->neighbors[neighborrank].comm_type = TYPE_TCP;
#ifdef _DEBUG
        std::cout << "Init neighbor " << neighborrank << " TCP" << std::endl;
        std::cout << " with CI " << ci << std::endl;
#endif // _DEBUG
    }

 /*
    this->neighbors[neighborrank].comm_type = TYPE_TCP;
    // this->neighbors[neighborrank].comm_channel.tcp_channel.socket = -1;;
    this->neighbors[neighborrank].communications = new Communication_TCP_t();
    this->neighbors[neighborrank].communications->setContactInfo( ci );
#ifdef _DEBUG
    std::cout << "Init neighbor " << neighborrank << " TCP" << std::endl;
    std::cout << " with CI " << ci << std::endl;
#endif // _DEBUG
    */
}
#endif // _WITH_TCP

#ifdef _WITH_KNEM
void MeMyselfAndI::initNeighborKNEM( int neighborrank, ContactInfo_KNEM& ci ) {
    TODO

    /*
    this->neighbors[neighborrank].comm_type = TYPE_KNEM;
    this->neighbors[neighborrank].communications = new Communication_KNEM_t();
    this->neighbors[neighborrank].communications->setContactInfo( ci );*/
}

KNEMendpoint_t* MeMyselfAndI::getMyEndpointKNEM(){
    return &(this->myEndpointKNEM);
}
#endif // _WITH_KNEM

#ifdef _WITH_NMAD
void MeMyselfAndI::initNeighborNMAD( int neighborrank, ContactInfo_NMAD& ci ) {

    TODO
    
    /*    this->neighbors[neighborrank].comm_type = TYPE_NMAD;
    this->neighbors[neighborrank].communications = new Communication_NMAD_t();
    this->neighbors[neighborrank].communications->setContactInfo( ci );*/
    
}
NMADendpoint_t* MeMyselfAndI::getMyEndpointNMAD(){
    return &(this->myEndpointNMAD);
}

#endif // _WITH_NMAD

#ifdef MPIHUBCHANNEL

void MeMyselfAndI::initNeighborHub( int neighborrank ) {

    /* is this guy on the same node as me? */

    char* _name;
    _name = myHeap.buildHeapName( neighborrank );
    if( /* false */ true == _sharedMemEsists( _name ) ) { // uncomment for debug DEBUG
        this->neighbors[neighborrank].comm_type = TYPE_SM;
        this->neighbors[neighborrank].communications = new Communication_SM_t();    
        this->neighbors[neighborrank].communications->init( neighborrank );
#ifdef _DEBUG 
        std::cout << "Init neighbor  " << neighborrank << " SM" << std::endl;
#endif // _DEBUG        
    } else { 
        this->neighbors[neighborrank].comm_type = TYPE_HUB;
        this->neighbors[neighborrank].communications = new Communication_hub_t();
        //    std::cout << "Init HUB neighbor " << neighborrank << std::endl;
#ifdef _DEBUG 
        std::cout << "Init neighbor  " << neighborrank << " HUB" << std::endl;
#endif // _DEBUG
    }
    free( _name );
}

Endpoint_hub_t* MeMyselfAndI::getMyEndpointHub(){
    return &(this->myEndpointHub);
}


#endif // MPIHUBCHANNEL

void MeMyselfAndI::communicationInit( char* comm_channel ){

    myHeap.setupSymmetricHeap(); // should be in the SM component

#ifdef DISTRIBUTED_POSH
    if( NULL == comm_channel ) {
        // TODO improve this!!
        this->myEndpoint = &( this->myEndpointHub );
        
        this->myEndpoint->init_end();
#ifdef _WITH_TCP
        /* This must be done only once the communication thread is up and initialized */
        while( ! this->getMyContactInfoP()->isReady() ){
            ;;
        }
#endif // _WITH_TCP
    }
    else { /* FIXME: use an object to stop using dirty things like this */
#ifdef _WITH_TCP
        if( 0 == strcmp( comm_channel, "TCP" ) ) {
            this->myEndpoint.init( );        
            /* This must be done only once the communication thread is up and initialized */
            while( ! this->getMyContactInfoP()->isReady() ){
                ;;
            }
        }
#endif // _WITH_TCP
#ifdef MPICHANNEL
        if( 0 == strcmp( comm_channel, "MPI" ) ) {
            this->myEndpoint = new MPIendpoint();
            this->myEndpoint->init_end();
      }
#endif // MPICHANNEL
#ifdef _WITH_KNEM
        if( 0 == strcmp( comm_channel, "KNEM" ) ) {
            this->myEndpoint->init();
        }
#endif // _WITH_KNEM
#ifdef _WITH_NMAD
        if( 0 == strcmp( comm_channel, "NMAD" ) ) {
            this->myEndpoint->init();
        }
#endif // _WITH_NMAD
#ifdef MPIHUBCHANNEL
        if( 0 == strcmp( comm_channel, "HUB" ) ) {
            std::cout << "INIT HUB" << std::endl;
            this->myEndpoint = new Endpoint_hub_t();
            this->myEndpoint->init_end();
        }
#endif // _WITH_NMAD
    }
#endif // DISTRIBUTED_POSH
}

void MeMyselfAndI::initNeighbors( int nb, char* comm_channel ) {

    char* filename;
    std::string machine; 
    int i;
    bool local = true ; // ??
    bool distributed = myInfo.getRTE()->isDistributed();

    /* Get the other processes' contact information */

    /* ça c'est pssible uniquement si je suis sur du MPI.
       Avec NewMadeleine pas besoin */

#ifdef DISTRIBUTED_POSH 
  
    /* Pack my CI, shared it with the other processes */

#ifdef _WITH_NMAD // FIXME TMP
    if( distributed )
        shmem_nmad_exchange_ci( );
#endif // _WITH_NMAD
    
    
#ifdef MPICHANNEL
    if( distributed ) {
        std::vector<ContactInfo*> ci_all( world.size() );
        shmem_mpi_exchange_ci( ci_all );
    }
#endif // MPICHANNEL
    
#ifdef MPIHUBCHANNEL
    if( distributed ) {
        //        std::cout << "INIT HUB NEIGHBORS" << std::endl;
        for( i = 0 ; i < myInfo.getSize() ; i++ ) {
            if( myInfo.getRank() != i ) {
                initNeighborHub( i );
            }
        }
    }
#endif // MPIHUBCHANNEL

#endif // DISTRIBUTED_POSH 

    /* TODO: non-distributed */

    if( !distributed ) {
        for( i = 0 ; i < myInfo.getSize() ; i++ ) {
            if( myInfo.getRank() != i ) {
                initNeighborSM( i );
            }
        }
    }
    
#ifdef _DEBUG
    std::cout << myInfo.getRank() << " init done" << std::endl;
#endif

}

/* How I see the remote guy's base address */

void* MeMyselfAndI::getRemoteHeapLocalBaseAddr( int rank ) {
    /*    managed_shared_memory* myNeighbor = this->getNeighbor( rank );
    managed_shared_memory::handle_t handle = myHeap.getOffsetHandle();
    return myNeighbor->get_address_from_handle( handle );*/
    return _getRemoteBaseAddress( rank );
}

/* How the remote guy sees his own base address */
/*
void* MeMyselfAndI::getRemoteHeapBaseAddr( int rank ) {
    uintptr_t* ptr = (uintptr_t*)this->getRemoteHeapLocalBaseAddr( rank );
    return (void*)(*ptr);
    }*/

std::string packMyContactInformation(){

    /* TMP */
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);
    return std::string( hostname );
}

#ifdef DISTRIBUTED_POSH

#ifdef _WITH_TCP
void MeMyselfAndI::setMyContactInfo( int rank, uint32_t addr, uint16_t port ){
    /* The hostname MUST be initialized somewhere else */

    ContactInfo_TCP ci( rank, addr, port );
    myContactInfo = &ci;
    
    /*    myContactInfo->setRank( rank );
    myContactInfo->setType( TYPE_TCP );
    myContactInfo->setAddr( addr, port );*/
    

#ifdef _DEBUG
    std::cout << "My contact info: " << myContactInfo << std::endl;
    std::cout << "My port: " << port << std::endl;
#endif // _DEBUG
}
#endif // _WITH_TCP

void MeMyselfAndI::setMyContactInfo( int rank ){
    /* MPI version */
    myContactInfo->setType( TYPE_MPI );
}

#ifdef _WITH_KNEM
void MeMyselfAndI::setMyContactInfo( knem_cookie_t cookie ){
    /* MPI version */
    myContactInfo->setType( TYPE_KNEM );
    myContactInfo->setCookie( cookie );
}
#endif //_WITH_KNEM

/* Returns the *pointer* */
ContactInfo* MeMyselfAndI::getMyContactInfoP( ){
    return myContactInfo;
}
//#endif // !MPICHANNEL

#endif // if DISTRIBUTED_POSH

#ifdef CHANDYLAMPORT

void MeMyselfAndI::posh_close_communication_channels( ){
    int i;
    int nb = myInfo.getSize();
    
    for( i = 0 ; i < nb ; i++ ) {
        if( myInfo.getRank() != i ) {
            
            /* TODO: handle the distributed case */
            
            if( this->neighbors[i].comm_type == SM ) {
                closeNeighbor( i );
            }

        }
    }
    
    /* TODO */

    /* Make mine unaccessible */
    /* Implement in shmem_accessible.cpp */
    
}

void MeMyselfAndI::posh_reopen_communication_channels( ){
    int i;
    int nb = myInfo.getSize();
    
    for( i = 0 ; i < nb ; i++ ) {
        if( myInfo.getRank() != i ) {
            
            /* TODO: handle the distributed case */
            
            if( this->neighbors[i].comm_type == SM ) {
                char* _name; 
                _name = myHeap.buildHeapName( i );
                while( false == _sharedMemEsists( _name ) ) {
                    usleep( SPIN_TIMER );
                }
                managed_shared_memory remoteHeap(  open_only, _name );
                this->neighbors[i].comm_channel.sm_channel.shared_mem_segment = std::move( remoteHeap );
                free( _name );
            }
        }
    }
    
    /* TODO */
}

/* FIXME : à ranger dans les utilitaires */

void MeMyselfAndI::closeNeighbor( int rank ) {
    auto* ptr = &(this->neighbors[rank].comm_channel.sm_channel.shared_mem_segment);
    //    ptr->~managed_shared_memory();    /* FIXME there is a memory leak here */
    /* auto* ptr = &(this->neighbors[rank].comm_channel.sm_channel);
       ptr->~sm_comm_channel_t();*/
}

void MeMyselfAndI::enterCheckpointing(){
    this->_checkpointing = true;
    // TODO make my heap unreachable if I am on distributed memory
}

void MeMyselfAndI::exitCheckpointing(){
    this->_checkpointing = false;
    checkpointing.resetMarker();
    // TODO make my heap unreachable if I am on distributed memory
}


#endif // if CHANDYLAMPORT
