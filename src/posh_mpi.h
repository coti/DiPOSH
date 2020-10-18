/*
 * Copyright (c) 2014-2020 LIPN - Universite Paris 13
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

//#include "shmem_internal.h"


#ifndef _POSH_MPI_H_
#define _POSH_MPI_H_

#include <vector>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
using namespace boost::mpi;
namespace mpi = boost::mpi;

#include "posh_contactinfo.h"
#include "posh_endpoint.h"
#include "posh_communication.h"

int shmem_mpi_put( int, void*, const void*, size_t );
int shmem_mpi_get( int, void*, const void*, size_t );
int shmem_mpi_init( void );

void shmem_mpi_exchange_ci( std::vector<ContactInfo*> & );

class ContactInfo_MPI : public ContactInfo  {

public:
    template<class Archive>
    void serialize( Archive & ar, const unsigned int version) {
        ar & type;
        ar & rank;
        ar & ready;
    }

    ContactInfo_MPI(){
        this->ready = false;
    }
    ContactInfo_MPI( neighbor_comm_type_t  type, int rank ){
        this->rank = rank;
        this->type = type;
    }
    std::ostream& doprint( std::ostream& os ) const {
        return os << "MPI -- rank " << this->rank << " " << hostname;
    }
    std::istream& doinput( std::istream& is ) {
        std::string type, sep, sep2, host, _rank;
        is >> type >> sep >> sep2 >> _rank >> host;
        this->rank = std::stoi( _rank );
        this->hostname = host;
        return is;
    }

    int getRank(){
        return rank;
    }
    void setRank( int rank ){
        this->rank = rank;
    }

    neighbor_comm_type_t getType(){
        return type;
    }
    void setType( neighbor_comm_type_t type ){
        this->type = type;
    }
    bool isReady() { return this->ready; }

};


/* Hardly anything here, but necessary for symmetry purpose */

#if 1
class Endpoint_MPI_t : public Endpoint_t {
protected:
    ContactInfo_MPI ci;
public:

    Endpoint_MPI_t(){}

    void setMyContactInfo( int rank ){
        this->ci.setRank( rank );
    }
    neighbor_comm_type_t getType() { return TYPE_MPI; }
    
    /*    void init() {
              char hostname[HOST_NAME_MAX+1];
        gethostname( hostname, HOST_NAME_MAX );
        this->ci.setHostname( hostname );
        
        int rank = shmem_mpi_init();
        this->ci.setRank( rank );
        } */// what is the difference?
    void init_end( ){
        char hostname[HOST_NAME_MAX+1];
        gethostname( hostname, HOST_NAME_MAX );
        this->ci.setHostname( hostname );
        
        int rank = shmem_mpi_init();
        this->ci.setRank( rank );
    }
    ContactInfo* getMyContactInfo(){
        return &(this->ci);
    }
    int finalize(){ return 0; /* we don't need to call MPI_Finalize */ } 
    void* posh__shmalloc( size_t s ){
        std::cerr << "posh__shmalloc should not be called from an Endpoint_MPI_t object" << std::endl;
        return NULL;
    }
    void* posh__shmemalign( size_t a, size_t s ){
        std::cerr << "posh__shmemalign should not be called from an Endpoint_MPI_t object" << std::endl;
        return NULL;
    }
    void* posh__shrealloc( void* p, size_t s ){
        std::cerr << "posh__shrealloc should not be called from an Endpoint_MPI_t object" << std::endl;
        return NULL;
    }
    void posh__shfree( void* p ){
        std::cerr << "posh__shfree should not be called from an Endpoint_MPI_t object" << std::endl;
    }
};
#else
class Endpoint_MPI_t : public Endpoint_t {
protected:
    ContactInfo_MPI *ci;
public:

    Endpoint_MPI_t(){
        throw ENDPOINT_UNAVAILABLE;
    }

    void setMyContactInfo( int rank ){
        this->ci.setRank( rank );
    }
    
    void init() {}
    void init_end( ){}
    ContactInfo* getMyContactInfo(){ return NULL; }
    int finalize(){ return 0; /* we don't need to call MPI_Finalize */ } 
};
#endif

class Communication_MPI_t : public Communication_t, public Endpoint_MPI_t {

 public:

    Communication_MPI_t(){}
    
    void init_comm( int rank ){
        //        shmem_mpi_init(  );
    }
    /*void init( ){
        //        shmem_mpi_init(  );
	}*/
    void close(  ){
	// nothing
    }
    void reopen(  ){
	// nothing
    }
    void setContactInfo( ContactInfo& ci ) {
        /* nothing to do */
        //   this->ci = ci;
    }

    void posh__get(  void* target, const void* source, size_t size, int pe ){
        shmem_mpi_get( pe, target, source, size );
    }
    void posh__put(  void* target, const void* source, size_t size, int pe ){
        shmem_mpi_put( pe, target, source, size );
    }

};

#endif // define _POSH_MPI_H_
