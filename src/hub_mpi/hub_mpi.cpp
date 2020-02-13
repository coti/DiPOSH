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

#include <cstdlib>

#include "../shmem_internal.h"
#include "../posh_heap.h"

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/collectives/all_gather.hpp>
#include <boost/mpi/collectives/all_gatherv.hpp>
#include <boost/mpi/intercommunicator.hpp>

//#include <boost/interprocess/managed_shared_memory.hpp>

namespace mpi = boost::mpi;
namespace bip = boost::interprocess;

void getLocalRanks( std::vector<std::pair<int,int> >&, mpi::communicator );
mpi::communicator buildMergedComm( void );
int initCommunicators( mpi::communicator&, mpi::communicator&, mpi::communicator& );
std::vector<int> buildProcessMap( mpi::communicator, mpi::communicator, std::vector<std::pair<int,int> >, int& );
void openLocalHeaps( std::vector<std::pair<int,int> >&, std::map<int, bip::managed_shared_memory*>&, mpi::communicator, std::map<int, MPI_Win>&, int );
void waitForComm( mpi::communicator&, mpi::communicator&, std::vector<int>&, std::map<int, MPI_Win>& );
void performPut( int, std::vector<long long int>, std::vector<int>&, std::vector<int>& pmap, std::map<int, MPI_Win>& , mpi::communicator);

int main( int argc,char **argv ) {
    
    mpi::environment env( argc, argv );
    MPI_Comm parent;
    mpi::communicator new_world, local_comm, hubs;
    std::vector<std::pair<int,int> > myranks;
    std::map<int, bip::managed_shared_memory*> localHeaps;
    std::map<int, MPI_Win> windows;
    int rank, size, max;
    int rc;
    
    /* Build the communicators. */

    rc = initCommunicators( new_world, local_comm, hubs );
    if( EXIT_FAILURE == rc ) {
        std::cerr << "Error building the communicators" << std::endl;
        return EXIT_FAILURE;
    }

    /* Receive the ranks of the processes that will communicate through me */
    
    getLocalRanks( myranks, local_comm );

    /* How can I contact each process? */
    
    std::vector<int> pmap = buildProcessMap( hubs, local_comm,  myranks, max );
    
    /* Open my local processes' shared heaps */
    
    openLocalHeaps( myranks, localHeaps, hubs, windows, max );

    /* Wait for communications */

    waitForComm( local_comm, hubs, pmap, windows );
    
  return EXIT_SUCCESS;
}

/* each process is stored using <local rank, global rank> */

void getLocalRanks( std::vector<std::pair<int,int> >& ranks, mpi::communicator comm ) {
    int name, size, i;
    mpi::status stat;

    size = comm.size();

    for( i = 0 ; i < size - 1 ; i++ ) {
        stat = comm.recv( mpi::any_source, TAG_DECLARE, name );
        // std::cout << "Source: " << stat.source() << " (" << name << ")" << std::endl;
        ranks.push_back( std::pair<int, int>( stat.source(), name ) );
    }
}

/* Build the local communicator */

mpi::communicator buildMergedComm( ) {
    MPI_Comm parent;
    mpi::communicator comm;
    
    MPI_Comm_get_parent( &parent );
    mpi::intercommunicator intercomm( parent, mpi::comm_duplicate );
    comm = intercomm.merge( true );
    
    if( parent == MPI_COMM_NULL){
        std::cerr << "ERROR: Intercommunicator on the MPI HUB is null" << std::endl;
    } 
    return comm;
}

/* Build the communicators:
 * - new_world: contains all the processes including the hubs
 * - local_comm: contains the processes located on the node, including the hub
 * - hubs: all the hubs
 */

int initCommunicators( mpi::communicator& new_world, mpi::communicator& local_comm, mpi::communicator& hubs ){
    int rank;
    int color;
    
    new_world = buildMergedComm();
    if( false == new_world ) {
        return EXIT_FAILURE;
    }
    
    rank = new_world.rank();

    /* Create a communicator for the hubs */
    
    //    std::cout << "New world size (hub): " << new_world.size() << std::endl;

    color = 1;
    hubs = new_world.split( color );
    //    std::cout << "Hubs size (hub): " << hubs.size() << std::endl;
    
    /* Create a local communicator, on each machine, including the hub */
    
    std::hash<std::string> hash_fn;
    color = hash_fn( mpi::environment::processor_name() );
    local_comm = new_world.split( color );

    // std::cout << "hub " << hubs.rank() << " I am local rank " << local_comm.rank() << " in " << local_comm.size() << std::endl;

    return EXIT_SUCCESS;
}

/* Build a process map, ie which hub to send the message to when we want
 * to reach a given process. Returns a vector that, for each rank, gives the rank
 * of the hub to contact.
*/

std::vector<int> buildProcessMap( mpi::communicator hubs, mpi::communicator local_comm, std::vector<std::pair<int,int> > myranks, int& max ){

    std::vector<int> rk_sizes;
    std::vector<int> rk, ranks;
    std::vector<int> pmap;
    int j, k, size;

    /* Exchange the ranks we are in charge with */

    mpi::all_gather( hubs, local_comm.size() - 1, rk_sizes ); // do not count the hub
    max = 0;
    for( auto it: rk_sizes ){
        if( it > max ) max = it;
        //        std::cout << it << " " ;
    }
    //    std::cout << std::endl;

    for( auto it: myranks )
        rk.push_back( it.second );
    //    std::cout << hubs.rank() << " my ranks: " << rk.size() << std::endl;
    mpi::all_gatherv( hubs, rk, ranks, rk_sizes );

    /* Initialize the process map */
    
    size = 0;
    for( auto it: rk_sizes ) size += it;
    for( int i = 0 ; i < size ; i++ ) pmap.push_back( -1 );
    
    j = 0; k = 0;
    for( int it: rk_sizes ) {
        for( int i = 0 ; i < it ; i++ ) {
            pmap[ ranks[j] ] = k;
            j++;
        }
        k++;
    }
    //    for( auto i: pmap )
        //        std::cout << i << " ";
    //    std::cout << std::endl;
    return pmap;
}

/* Open a shared heap and a window for each local neighbor */

void openLocalHeaps( std::vector<std::pair<int,int> >& myranks, std::map<int, bip::managed_shared_memory*>& localHeaps, mpi::communicator hubs, std::map<int, MPI_Win>& windows, int max ){
    int nb = 0;
    for( auto r: myranks ){
        //        std::cout << "my ranks: " << r.first << " " << r.second << std::endl;
        char* _name;
        asprintf( &_name, "%s_%d", HEAPBASENAME, r.second );
        localHeaps[ r.second ] = new bip::managed_shared_memory( bip::open_only, _name );
        free( _name );
        
        /* Prepare a window */

        /* This is a collective call. So we need to call it as many times 
           as we have local heaps. Problem: not all the hubs are managing 
           the same number of processes. Then we will perform bogus calls. */
        
        MPI_Win win;

        void* base_addr = localHeaps[ r.second ]->get_address_from_handle( 0 );
        MPI_Win_create( base_addr,  localHeaps[ r.second ]->get_size(), sizeof( char ), MPI_INFO_NULL, hubs, &win );
        
        /* We can discuss the number of windows, wrt to concurrent communications. 
           Ranks on this window. */

        std::vector<int> ranks;
        mpi::all_gather( hubs, r.second, ranks );

        for( auto rk: ranks ){
            if( rk != -1 )  windows[rk] = win;
        }
        nb++;
    }
    
    /* Bogus calls */

    while( nb < max ) {
        MPI_Win win;
        int toto;
        void* base_addr = &toto;
        MPI_Win_create( base_addr, 1, sizeof( char ), MPI_INFO_NULL, hubs, &win );
        nb++;

        std::vector<int> ranks;
        toto = -1;
        mpi::all_gather( hubs, toto, ranks );

    }
}

void performPut( int origin, std::vector<long long int> info, std::map<int, MPI_Win>& windows, mpi::communicator& local_comm ) {

    long long int disp = info[0];
    long long int dest = info[1];
    long long int size = info[2];

    /* Receive the data */

    char* buff = (char*) malloc( size );
    local_comm.recv( origin, TAG_DATA, buff, size );

    /* Find the window */

    MPI_Win win = windows[dest];

    /* Perform the communication */
    
    MPI_Win_lock( MPI_LOCK_SHARED, dest, 0, win );
    MPI_Put( buff, size, MPI_CHAR, dest, disp, size, MPI_CHAR, win );
    MPI_Win_unlock( dest, win ); 

    free( buff );
}

void performGet( int origin, std::vector<long long int> info, std::map<int, MPI_Win>& windows, mpi::communicator& local_comm ) {

    long long int disp = info[0];
    long long int dest = info[1];
    long long int size = info[2];

    char* buff = (char*) malloc( size );

    /* Find the window */

    MPI_Win win = windows[dest];

    /* Perform the communication */
    
    MPI_Win_lock( MPI_LOCK_SHARED, dest, 0, win );
    MPI_Get( buff, size, MPI_CHAR, dest, disp, size, MPI_CHAR, win );
    MPI_Win_unlock( dest, win ); 

    /* Send the data */

    local_comm.send( origin, TAG_DATA, buff, size );

    free( buff );
}


void waitForComm( mpi::communicator& local_comm, mpi::communicator& hubs, std::vector<int>& pmap, std::map<int, MPI_Win>& windows ){

    mpi::status stat;
    MPI_Aint disp;
    std::vector<long long int> info; // src, disp, destination, size

    while( true ){ /* wait for communication requests from my local processes */

        stat = local_comm.recv( mpi::any_source, mpi::any_tag, info );

        /* Perform the requested communication */

        if( stat.tag() == TAG_PUT ) {
            performPut( stat.source(), info, windows, local_comm );
        }

        if( stat.tag() == TAG_GET ) {
            performGet( stat.source(), info, windows, local_comm );
        }
        
        if( stat.tag() == TAG_FINAL ) {
            std::cout << "Recv finalize" << std::endl;
            break;
        }
        
        /* Send completion notification */
        
        local_comm.send( stat.source(), TAG_PUT_DONE );
    }

    /* Finished. */
    /* We don't need to free the communicators, Boost will do it. */
    
}
