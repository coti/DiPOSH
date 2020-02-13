/*
 * Copyright (c) 2020 LIPN - Universite Paris 13
 *                         - Université Sorbonne Paris Nord
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
#include "posh_hub.h"
#include "posh_heap.h"
#include "posh_sm.h"

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/intercommunicator.hpp>
#include <boost/mpi/collectives.hpp>

#include <algorithm>

namespace mpi = boost::mpi;

mpi::communicator Endpoint_hub_t::local_comm; // communicator used to communicate with my hub

void Communication_hub_t::init( int rank ) {

    //    std::cout << "TODO" << std::endl;
    
    /* Initialize a shared heap */
    
    char* _name; 
    _name = myHeap.buildHeapName( rank );
    while( false == _sharedMemEsists( _name ) ) {
        usleep( SPIN_TIMER );
    }
    managed_shared_memory __heap(  open_only, _name );
    this->remoteHeap = std::move( __heap );
    free( _name );
    //        shmem_sm_init(  );


}

void Endpoint_hub_t::init_end( ) {
    
    MPI_Comm comm_dup;

    /* At this point, my local heap is already initialized */
    
    this->rank = myInfo.getRank();

    /* How many hubs am I going to spawn? Prepare a color (also used later) */

    std::hash<std::string> hash_fn;
    int color = hash_fn( mpi::environment::processor_name() );

    std::vector<int> colors;
    for( int i = 0 ; i < myInfo.getSize() ; i++ ) colors.push_back( 0 );
    
    mpi::all_gather( myInfo.world, color, colors );
    int nbhubs = 1;
    std::sort( colors.begin(), colors.end() );
    for( auto it = colors.begin()+1 ; it != colors.end() ; it++ ) {
        if( *it != *(it-1) ) {
            nbhubs++;
        }
    }
    
    //    std::cout << "Spawn " << nbhubs << " hubs" << std::endl;

    /* Spawn the hubs. The new global comm is new_world */
    
    //  char* args[2] = {"hub-mpi", NULL};
    // MPI_Comm_spawn( "valgrind", args, nbhubs,
    MPI_Comm_spawn( "hub-mpi", MPI_ARGV_NULL, nbhubs,
                    MPI_INFO_NULL, 0, myInfo.world, &comm_dup,
                    MPI_ERRCODES_IGNORE);
    mpi::intercommunicator intercomm( comm_dup, mpi::comm_duplicate );
    mpi::communicator new_world = intercomm.merge( false );

    //    std::cout << "New world size: " << new_world.size() << std::endl;

    /* Create a communicator for the hubs */

    color = 0;
    mpi::communicator processes = new_world.split( color );

    /* Create a local communicator, on each machine, including the hub */
    
    color = hash_fn( mpi::environment::processor_name() );
    this->local_comm = new_world.split( color );

    /* Declare myself to my hub (last process of the communicator) */
    
    this->local_comm.send( local_comm.size()-1, TAG_DECLARE, this->rank );
    //    std::cout << myInfo.getRank() << " local hub " << this->local_comm.size() - 1 << std::endl;
}

int Endpoint_hub_t::finalize( ) {
    /* Send a message to my hub */
    this->local_comm.send( local_comm.size()-1, TAG_FINAL, this->rank );

    /* We don't need to free the communicators, Boost will do it. */
    /* We don't need to call MPI_Finalize either. */
}

int Communication_hub_t::posh__put(  void* target, const void* source, size_t size, int pe ){

    /* We shouldn't have to deal with local communications
       bcause local neighbors are initialized with SM communications */
    
    managed_shared_memory::handle_t handle = myHeap.myHeap.get_handle_from_address( target );
    MPI_Aint disp = (MPI_Aint) handle;
    std::vector<long long int> couple;
    couple.push_back( disp );
    couple.push_back( pe );
    couple.push_back( size );

    mpi::communicator *comm = &(Endpoint_hub_t::local_comm);
    int hubrank = comm->size() - 1;
    
    /* send the data, address and destination to my hub */
    
    comm->send( hubrank, TAG_PUT, couple );
    comm->send( hubrank, TAG_DATA, (char*)source, size );
    
    /* wait for completion */

    comm->recv( hubrank, TAG_PUT_DONE );

    return 0;
}

int Communication_hub_t::posh__get(  void* local, const void* target, size_t size, int pe ){

    /* We shouldn't have to deal with local communications
       bcause local neighbors are initialized with SM communications */
    
    managed_shared_memory::handle_t handle = myHeap.myHeap.get_handle_from_address( target );
    MPI_Aint disp = (MPI_Aint) handle;
    std::vector<long long int> couple;
    couple.push_back( disp );
    couple.push_back( pe );
    couple.push_back( size );

    mpi::communicator *comm = &(Endpoint_hub_t::local_comm);
    int hubrank = comm->size() - 1;
    
    /* Send the address and destination to my hub, receive the data */
    
    comm->send( hubrank, TAG_GET, couple );
    comm->recv( hubrank, TAG_DATA, (char*)local, size );
    
    return 0;
}

