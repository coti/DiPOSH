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

//#include "shmem_internal.h"


#ifndef _POSH_MPI_H_
#define _POSH_MPI_H_

int shmem_mpi_put( int, void*, const void*, size_t );
int shmem_mpi_get( int, void*, const void*, size_t );
void shmem_mpi_init( void );

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
        return os << "MPI -- rank " << rank;
    }

    /*
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
    */


};


/* Hardly anything here, but necessary for symmetry purpose */

class MPIendpoint : public Endpoint_t {
protected:
    ContactInfo_MPI ci;
public:

    MPIendpoint(){}
    
    void init_end( ){
        shmem_mpi_init();
    }
    ContactInfo* getMyContactInfo(){
        return &(this->ci);
    }
    int finalize(){ /* we don't need to call MPI_Finalize */ } 
};

class Communication_MPI_t : public Communication_t, public MPIendpoint {

 public:

    Communication_MPI_t(){}
    
    void init( int rank ){
        //        shmem_mpi_init(  );
    }
    void setContactInfo( ContactInfo& ci ) {
        /* nothing to do */
        //   this->ci = ci;
    }

    int posh__get(  void* target, const void* source, size_t size, int pe ){
        return shmem_mpi_get( pe, target, source, size );
    }
    int posh__put(  void* target, const void* source, size_t size, int pe ){
       return  shmem_mpi_put( pe, target, source, size );
    }

};

#endif // define _POSH_MPI_H_
