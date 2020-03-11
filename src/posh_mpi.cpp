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

//#include <mpi.h>
#include "shmem_internal.h"
#include "posh_mpi.h"
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
namespace mpi = boost::mpi;

#include "posh_heap.h" // FIXME should be split into two files

#ifdef _DEBUG
#include "posh_sm.h"
#endif

int shmem_mpi_put( int pe, void* target, const void* source, size_t size ){
    managed_shared_memory::handle_t handle = myHeap.myHeap.get_handle_from_address( target );
    MPI_Aint disp = (MPI_Aint) handle;

    MPI_Win_lock( MPI_LOCK_SHARED, pe, 0, myInfo.window );
    MPI_Put( source, size, MPI_CHAR, pe, disp, size, MPI_CHAR, myInfo.window );
    MPI_Win_unlock( pe, myInfo.window ); 


    return 0;
}

int shmem_mpi_get( int pe, void* target, const void* source, size_t size ){
    
    managed_shared_memory::handle_t handle = myHeap.myHeap.get_handle_from_address( source );
    MPI_Aint disp = (MPI_Aint) handle;

    MPI_Win_lock( MPI_LOCK_SHARED, pe, 0, myInfo.window );
    MPI_Get( target, size, MPI_CHAR, pe, disp, size, MPI_CHAR, myInfo.window );
    MPI_Win_unlock( pe, myInfo.window ); 

#ifdef _DEBUG
    std::cout << "Got " << *(int*)target << std::endl;

    /* Open remote PE's symmetric heap */
    const void* buf = const_cast<const void*>(_getRemoteAddr( source, pe ) );
    _shmem_memcpy( target, buf, size );
    
    std::cout << "Also got " << *(int*)target << std::endl;
#endif
    
    return 0;
}

int shmem_mpi_iput( int pe, void* target, const void* source, size_t size ){
    std::cout << __func__ << " not implemented yet" << std::endl;
    return 0;
}

int shmem_mpi_iget( int pe, void* target, const void* source, size_t size ){
    std::cout << __func__ << " not implemented yet" << std::endl;
    return 0;
}

void shmem_mpi_init( ){

    /* Create a shared window for my own shared heap */

    MPI_Win_create( _getMyBaseAddress(),  myHeap.getSize(), sizeof( char ), MPI_INFO_NULL, MPI_COMM_WORLD, &(myInfo.window) );
    int key, attr;

    int   win_flag = 0;
    int * win_model = NULL;
    MPI_Win_get_attr( myInfo.window, MPI_WIN_MODEL, &win_model, &win_flag);

    if( *win_model != MPI_WIN_UNIFIED ){
        std::cout << "The memory model must be unified" << std::endl;
    } else {
        std::cout << "Good: the memory model is unified" << std::endl;
    }

    
    /* Set a contact information */
    
    myInfo.setMyContactInfo(  myInfo.world.rank() );

#ifdef _DEBUG
    std::cout << "My contact info:" << std::endl;
    std::cout << *(myInfo.getMyContactInfoP()) << std::endl;
    std::cout << "My type: " << (myInfo.getMyContactInfoP())->getType() << std::endl;
#endif // _DEBUG
    MPI_Barrier( MPI_COMM_WORLD ); // TODO check if these is another one somewhere else
    
#ifdef _DEBUG    /* test */
    
    int pe = ( myInfo.getRank() + 1 ) % myInfo.getSize();

    int toto, aa;
    int* titi = (int*) myHeap.myHeap.allocate( sizeof( int ) );
    *titi = myInfo.getRank();
    
    managed_shared_memory::handle_t handle = myHeap.myHeap.get_handle_from_address( titi );
    MPI_Aint disp = handle;

    MPI_Win_lock( MPI_LOCK_SHARED, pe, 0, myInfo.window );
    MPI_Get( &toto, 1, MPI_INT, pe, disp, 1, MPI_INT, myInfo.window );
    MPI_Win_unlock( pe, myInfo.window ); 

    std::cout << myInfo.getRank() << " got " << toto << " from disp " << disp << std::endl;
#endif 
}


/* TODO call this from somewhere */

void shmem_mpi_finalize(  ){

    MPI_Win_detach( myInfo.window, _getMyBaseAddress() );
    
}

void shmem_mpi_exchange_ci( std::vector<ContactInfo*> &ci_all ){
    #if 0
    mpi::communicator world;
    ContactInfo* my_ci = getMyContactInfoP();
    mpi::all_gather( world, *my_ci, ci_all );


#ifdef _DEBUG
    std::cout << "My CI: " << *my_ci << std::endl;
#endif // _DEBUG

    /* Process what I have received */

    for( auto i = ci_all.begin() ; i != ci_all.end() ; ++i ){
#ifdef _DEBUG
        std::cout << *i << ' ';
#endif  // _DEBUG
        int id = i->getRank();
        ContactInfo ci( id, this->hostname.c_str(), i->getAddr(), i->getPort() );
        this->neighbors[id].rank = id;
        this->neighbors[id].neigh_ci.push_back( ci ); // TODO why not use *i ?
#ifdef _DEBUG
        std::cout << "CI: " << ci << std::endl;
#endif // _DEBUG
        
        if( NULL == comm_channel ) {

            /* Are we on the same node? */
#ifdef _DEBUG
            std::cout << "hostname: " << i->getHostname() << " - " << this->hostname.c_str() << std::endl;
#endif // _DEBUG
            if( 0 == strcmp( i->getHostname(), this->hostname.c_str() ) ) {
#ifdef _DEBUG
                std::cout << "neighbor is on the same host" << std::endl;
#endif // _DEBUG
               initNeighborSM( id );
            } else {         
                switch( i->getType() ) {
#if 0 //TODO refactor
                case TYPE_TCP:
                    initNeighborTCP( id, *i );
                    break;
#endif
                case TYPE_MPI:
                    initNeighborMPI( id );
                    break;
#ifdef _WITH_KNEM
                case TYPE_KNEM:
                    initNeighborKNEM( id, *i );
                    break;
#endif
                default:
                    /* TODO: if on the same node -> SM */
                    initNeighborNULL( id );
                    break;
                }
            }
        }
#if 0 //TODO refactor
else {
            if( 0 == strcmp( comm_channel, "TCP" ) ) {
#ifdef _DEBUG
                std::cout << "neighbor TCP" << std::endl;
#endif // _DEBUG
                initNeighborTCP( id, *i );
            }
            if( 0 == strcmp( comm_channel, "MPI" ) ) {
                initNeighborMPI( id );
            }
            if( 0 == strcmp( comm_channel, "SM" ) ) {
                initNeighborSM( id );
            }
#ifdef _WITH_KNEM
            if( 0 == strcmp( comm_channel, "KNEM" ) ) {
                initNeighborKNEM( id, *i );
            }
#endif // _WITH_KNEM
        }
#endif
#ifdef _DEBUG
        std::cout << "Init " << id << std::endl;
        std::cout << this->neighbors[id] << std::endl;
#endif // _DEBUG
    }
#endif
}
