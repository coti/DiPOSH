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
#include <sstream>
#include <string>

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
#if 1 //def MPIHUBCHANNEL
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
    out << myInfo.neigh_ci[n.rank] << std::endl;
    //    for( auto i = n.neigh_ci.begin() ; i != n.neigh_ci.end() ; ++i ){
    //        out << *i;
    //        out << std::endl;
    //    }
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

Neighbor_t* MeMyselfAndI::getNeighbor( int rank ) {
    return &(this->neighbors[rank]);
}

std::vector<Neighbor_t>* MeMyselfAndI::getNeighbors( ) {
    return &(this->neighbors);
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
    //    this->neighbors.reserve( nb );
    // The allocation itself (push_back) is done later */
    /*   for( int i = 0 ; i < nb ; i++ ) {
        //  this->neighbors[i] = std::unique_ptr<Neighbor_t>( nullptr );
        }*/
}

/* In this function, initialize the *local* endpoints, ie how I can be contacted.
   All the endpoints we can be open are put in a vector: myEndpoints.
   If an endpoint cannot be initialized, it throws an exception. It is caught silently 
   and we don't put the endpoint in the vector of endpoints.
   If we want to use a specific endpoint, its name is given in comm_channel.
   At the end, if no endpoint has been initialized, the vector is empty -> return an error.
   This function is way too long and way too redundant -> TODO refactor.
*/

int MeMyselfAndI::communicationInit( char* comm_channel ){

    myHeap.setupSymmetricHeap(); // should be in the SM component

    /* No channel is provided -> open whatever I can open */
    
    if( NULL == comm_channel ) {
        try{ /* SM */
            myEndpoints.push_back( new Endpoint_SM_t );
            myEndpoints.back().init_end();
        } catch( int ex ){
            /* Should not happen */
        std::cerr << "Could not open the shared memory communication component" << std::endl;
        }
        
#if 1 //def MPICHANNEL
        try{ /* MPI */
            myEndpoints.push_back( new Endpoint_MPI_t );
            myEndpoints.back().init_end();
        } catch( int ex ){
            ;; /* silent */
        }
#endif // MPICHANNEL
        
#ifdef _WITH_TCP
        try{ /* TCP */
            myEndpoints.push_back( new TCPendpoint_t );
            myEndpoints.back().init_end();
            while( ! e.isReady() ){
                ;;
            }
        } catch( int ex ){
            ;; /* silent */
        }
#endif // _WITH_TCP
        
#ifdef _WITH_KNEM
        try{ /* KNEM */
            myEndpoints.push_back( new  Endpoint_KNEM_t);
            myEndpoints.back().init();       
        } catch( int ex ){
            ;; /* silent */
        }
#endif // _WITH_KNEM
        
#ifdef _WITH_NMAD
        try{ /* NMAD */
            myEndpoints.push_back( new  Endpoint_NMAD_t);
            myEndpoints.back().init_end();       
        } catch( int ex ){
            ;; /* silent */
        }
#endif // _WITH_NMAD
        
#ifdef MPIHUBCHANNEL
        try{ /* HUB */
            myEndpoints.push_back( new  Endpoint_hub_t);
            myEndpoints.back().init_end();
        } catch( int ex ){
            ;; /* silent */
        }
#endif // MPIHUBCHANNEL
        
    } else { /* if NULL == comm_channel */
        if( 0 == strcmp( comm_channel, "SM" ) ) {
            
            try{ /* SM */
                myEndpoints.push_back( new Endpoint_SM_t );
                myEndpoints.back().init_end();
            } catch( int ex ){
                /* Should not happen */
                std::cerr << "Could not open the shared memory communication component" << std::endl;
            }
            
        } else if ( 0 == strcmp( comm_channel, "MPI" ) ) {            
#if 1 //def MPICHANNEL
            try{ /* MPI */
                myEndpoints.push_back( new Endpoint_MPI_t );
                myEndpoints.back().init_end();
          } catch( int ex ){
                ;; /* silent */
            }
#endif // MPICHANNEL
            
        } else if ( 0 == strcmp( comm_channel, "TCP" ) ) {           
 #ifdef _WITH_TCP
           try{ /* TCP */
                myEndpoints.push_back( new TCPendpoint_t );
                myEndpoints.back().init_end();
                while( ! e.isReady() ){
                    ;;
                }
            } catch( int ex ){
                ;; /* silent */
            }
#endif // _WITH_TCP
            
        } else if ( 0 == strcmp( comm_channel, "KNEM" ) ) {           
#ifdef _WITH_KNEM
            try{ /* KNEM */
                myEndpoints.push_back( new  Endpoint_KNEM_t);
                myEndpoints.back().init_end();       
            } catch( int ex ){
                ;; /* silent */
            }
#endif // _WITH_KNEM
            
        } else if ( 0 == strcmp( comm_channel, "NMAD" ) ) {           
#ifdef _WITH_NMAD
            try{ /* NMAD */
                myEndpoints.push_back( new  Endpoint_NMAD_t);
                myEndpoints.back().init_end();       
            } catch( int ex ){
                ;; /* silent */
            }
#endif // _WITH_NMAD
            
        } else if ( 0 == strcmp( comm_channel, "HUB" ) ) {           
#ifdef MPIHUBCHANNEL
            try{ /* HUB */
                myEndpoints.push_back( new  Endpoint_hub_t);
                myEndpoints.back().init_end();
            } catch( int ex ){
                ;; /* silent */
            }
#endif // MPIHUBCHANNEL
        }        

    } /* if NULL == comm_channel */

    /* Did I open anything? */
    
    if( myEndpoints.size() == 0 ) {
        std::cerr << "Could not open any communication component. Abort." << std::endl;
        return -1;
    }

    return 0;

}

/* This is necessary because the number of endpoints might not be the same on all 
   the neighbors -> Allgatherv 
   TODO: support Padico
*/

void gatherContactInfo(){

    int nb, size, total;
    int i, j;
    size = myInfo.getSize();
    
    /* How many endpoints do I have? */

    nb = myInfo.myEndpoints.size();
    //    std::cout << "I have " << nb << " endpoints" << std::endl;

    /* How many endpoints do the other ones have? */

    int nb_all[ size ];
    MPI_Allgather( &nb, 1, MPI_INT, &nb_all[0], 1, MPI_INT, MPI_COMM_WORLD );

    /* Which type? */

    int displ[size];
    std::vector<neighbor_comm_type_t> myep; // more convenient in a vector
    for( i = 0 ; i < nb ; i++ ) {
        myep.push_back( myInfo.myEndpoints[i].getType() );
    }
    total = 0;
    for( i = 0 ; i < size ; i++ ){
        displ[i] = total;
        total += nb_all[i];
    }
    neighbor_comm_type_t alltypes[total];    

    MPI_Allgatherv( &myep[0], nb, MPI_INT,
                    &alltypes[0], nb_all, displ, MPI_INT, MPI_COMM_WORLD );

    /* The contact info are sent as strings -> receive their sizes */

    int mylen[nb];
    int alllen[total];    
    std::stringstream myci;
    for( i = 0 ; i < nb ; i++ ){
        std::stringstream tmp;
        //        std::cout <<  *(myInfo.myEndpoints[i].getMyContactInfo()) << std::endl;
        tmp << *(myInfo.myEndpoints[i].getMyContactInfo());
        myci << tmp.str();
        mylen[i] = tmp.str().size();
    }

    MPI_Allgatherv( &mylen[0], nb, MPI_INT,
                    &alllen[0], nb_all, displ, MPI_INT, MPI_COMM_WORLD );

    int mytotallen = myci.str().size();
    int displ2[size];
    int alltotallen[size];
    int totaltotallen = 0;
    for( i = 0 ; i < size ; i++ ){
        alltotallen[i] = 0;
        for( j = 0 ; j < nb_all[i] ; j++ ) {
            alltotallen[i] += alllen[ displ[i] + j];
        }
        displ2[i] = totaltotallen;
        totaltotallen += alltotallen[i];
    }

    /* Receive them */

    char allci[totaltotallen];    
    MPI_Allgatherv( myci.str().c_str(), mytotallen, MPI_CHAR,
                    &allci[0], alltotallen, displ2, MPI_CHAR, MPI_COMM_WORLD );

    std::string myhostname =  myInfo.myEndpoints[0].getMyContactInfo()->getHostname();

    /* Find what I have in common */

    for( i = 0 ; i < size ; i++ ){
        
        Neighbor_t n;

        if( i != myInfo.getRank() ){
            neighbor_comm_type_t* ptr_type = alltypes + displ[i];
            char* ptr_ci = allci + displ2[i];

            //            std::cout << myInfo.getRank() << " Rank " << i << ": " << nb_all[i] << " interfaces" << std::endl;
            
            /* Take the interface in common that has the highest priority */

            int maxpriority = 0;
            neighbor_comm_type_t comm_type = NONE;

            /* Select the highest priority */
            
            for( j = 0 ; j < nb_all[i] ; j++ ) {

                /* We need to read the CI in order to have the hostname;
                   necessary for the SM component */
                
                std::stringstream s;
                char toto[512];
                
                memcpy( toto, ptr_ci, alllen[ displ[i] + j] );
                toto[alllen[ displ[i] + j]] = '\0';
                s << toto;
                
                ptr_ci += alllen[ displ[i] + j];

                /*    for( auto &e: myep ){
                    std::cout << e  << " : " << ptr_type[j]<< " " << (e == ptr_type[j]) << std::endl;
                    }*/
                
                /* Do I have this type of interface? */
                bool found = false;
                for( auto &e: myep ){
                    if( e == ptr_type[j] ) found = true;
                }

                //  if( std::find( myep.begin(), myep.end(), ptr_type[j] ) != myep.end() ){
                if( found ==  true ) {
                    //  std::cout << "Max priority: " << maxpriority;

                    if( maxpriority < channel_priorities[ ptr_type[j] ] ){
                        //                    std::cout << " I have " << channel_priorities[ ptr_type[j] ] << " with " << ptr_type[j] << std::endl;

                        if( ptr_type[j] == TYPE_SM ) {
                            ContactInfo_SM ci;
                            s >> ci;
                            /* Keep the SM interface only if we have the same hostname */
                            if( 0 != myhostname.compare( ci.getHostname() ) ){
                                continue;
                            }
                        }

                        //                        std::cout << myInfo.getRank() << " take " << ptr_type[j] << std::endl;
                        
                        maxpriority = channel_priorities[ ptr_type[j] ];
                        comm_type = ptr_type[j];
                    }

                }
            }

            if( comm_type == NONE ) {
                std::cerr << myInfo.getRank() << " Could not find any common interface to communicate with " << i << std::endl;
                exit( -1 );
            }
            
            /* Plug the contact info and the neighbor */
            ptr_ci = allci + displ2[i];

            for( j = 0 ; j < nb_all[i] ; j++ ) {
                
                std::stringstream s;
                char toto[512];

                //std::cout << "Type: " << ptr_type[j] << std::endl;

                memcpy( toto, ptr_ci, alllen[ displ[i] + j] );
                toto[alllen[ displ[i] + j]] = '\0';
                //                std::cout << "CI: " <<  toto << std::endl;
                s << toto;
                
                ptr_ci += alllen[ displ[i] + j];

                /* Do I take this one? */
                
                if( ptr_type[j] == comm_type ) {
                                      
                    if( ptr_type[j] == TYPE_SM ) {
                        ContactInfo_SM ci;
                        s >> ci;
                        //    n.neigh_ci.push_back( &ci );
                        myInfo.neigh_ci.push_back( &ci );
                        
                        n.comm_type = TYPE_SM;
                        n.communications = new Communication_SM_t();    
                        n.communications->init_comm( i );
                        n.communications->setContactInfo( ci );

                        //                        std::cout << myInfo.getRank() << " Init comm " << ci << " from " << s.str() <<  " with " << i << std::endl;
                    } else if ( ptr_type[j] == TYPE_MPI ) {
                        ContactInfo_MPI ci;
                        s >> ci;
                        myInfo.neigh_ci.push_back( &ci );
                        //                       n.neigh_ci.push_back( &ci );

                        n.communications = new Communication_MPI_t();
                        n.comm_type = TYPE_MPI;
                        n.communications->init_comm( i );
                        n.communications->setContactInfo( ci );
                        
                        //         std::cout << myInfo.getRank() << " Push back " << ci << " from " << s.str() << std::endl;
#ifdef _WITH_TCP
                    } else if ( ptr_type[j] == TYPE_TCP ) {
                        ContactInfo_TCP ci;
                        s >> ci;
                        myInfo.neigh_ci.push_back( &ci );
                        //                       n.neigh_ci.push_back( &ci );
                        n.comm_type = TYPE_TCP;
                        n.communications = new Communication_TCP_t();
                        n.communications->setContactInfo( ci );
                        n.communications->init_comm( i );

                        // std::cout << myInfo.getRank() << " Push back " << ci << std::endl;
#endif // _WITH_TCP

#ifdef _WITH_KNEM
                    } else if ( ptr_type[j] == TYPE_KNEM ) {
                        ContactInfo_KNEM ci;
                        s >> ci;
                        n.neigh_ci.push_back( &ci );

                        n.comm_type = TYPE_KNEMM;
                        n.communications = new Communication_KNEM_t();
                        n.communications->setContactInfo( ci );
                        n.communications->init_comm( i );

                        
#endif // _WITH_KNEM
#ifdef _WITH_NMAD
                   } else if ( ptr_type[j] == TYPE_NMAD ) {
                        ContactInfo_NMAD ci;
                        s >> ci;
                        n.neigh_ci.push_back( &ci );
                        n.comm_type = TYPE_NMAD;
                        n.communications = new Communication_NMAD_t();
                        n.communications->setContactInfo( ci );
                        n.communications->init_comm( i );
#endif // _WITH_NMAD
#ifdef MPIHUBCHANNEL
                    } else if ( ptr_type[j] == TYPE_HUB ) {
                        ContactInfo_hub ci;
                        s >> ci;
                        myInfo.neigh_ci.push_back( &ci );
                        n.comm_type = TYPE_HUB;
                        n.communications = new Communication_hub_t();
                        n.communications->setContactInfo( ci );
                        n.communications->init_comm( i );
#endif // MPIHUBCHANNEL
                    }
                    break;
                }
            }
        } else {
            /* Communicate with myself on the segment of SM */
            
            nb = myInfo.myEndpoints.size();
            for( j = 0 ; j < nb ; j++ ) {
                if( TYPE_MPI == myInfo.myEndpoints[j].getType() ) {
                    //     ContactInfo ci =  myInfo.myEndpoints[j].getMyContactInfo(); 
                    myInfo.neigh_ci.push_back( myInfo.myEndpoints[j].getMyContactInfo() );
                    
                    n.comm_type = TYPE_SM;
                    n.communications = new Communication_SM_t();    
                    n.communications->init_comm( i );
                    //   n.communications->setContactInfo( ci );
                    
                }
            }
        }
        myInfo.getNeighbors()->push_back( n );

    }
    
}

/* Initialize the neighbors, ie how I can communicate with each one of them 
 */

void MeMyselfAndI::initNeighbors( int nb, char* comm_channel ) {

    /* Get the other processes' contact information and initialize the communications */

    gatherContactInfo();

#if 0
    Trucs qui peuvent être utiles:
    
        shmem_nmad_exchange_ci( );
    
    std::vector<ContactInfo*> ci_all( world.size() );
        shmem_mpi_exchange_ci( ci_all );

#endif
        
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


#ifdef CHANDYLAMPORT

void MeMyselfAndI::posh_close_communication_channels( ){
    int i;
    int nb = myInfo.getSize();
    
    for( i = 0 ; i < nb ; i++ ) {
        if( myInfo.getRank() != i ) {
	    this->neighbors[i].communications->close();
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
	    this->neighbors[i].communications->reopen();
        }
    }
    
    /* TODO */
}

/* FIXME : à ranger dans les utilitaires */

void MeMyselfAndI::closeNeighbor( int rank ) {
    //    auto* ptr = &(this->neighbors[rank].comm_channel.sm_channel.shared_mem_segment);
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
