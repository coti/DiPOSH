/*
 * Copyright (c) 2020 LIPN - Universite Paris 13
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

#ifndef _POSH_HUB_H_
#define _POSH_HUB_H_

#include "posh_contactinfo.h"

#include <boost/mpi/communicator.hpp>
namespace mpi = boost::mpi;

#define TAG_DECLARE   1
#define TAG_PUT       2
#define TAG_PUT_DONE  3
#define TAG_GET       4
#define TAG_DATA      16

//int shmem_hub_put( int, void*, const void*, size_t );
//int shmem_hub_get( int, void*, const void*, size_t );
//void shmem_hub_init( void );

void shmem_hub_finalize( void );

class ContactInfo_hub  : public ContactInfo  {

public:
    ContactInfo_hub(){
        this->ready = false;
    }

    std::ostream& doprint( std::ostream& os ) const {
        return os << "HUB -- rank " << rank;
    }

    /*
    neighbor_comm_type_t getType(){
        return type;
    }
    void setType( neighbor_comm_type_t type ){
        this->type = type;
    }
    void setRank( int rank ){
        this->rank = rank;
    }
    int getRank(){
        return rank;
    }
    bool isReady() { return this->ready; }
    */
};

class HUBneighbor_t{
 public:
    managed_shared_memory remoteHeap;
};


class Endpoint_hub_t : public Endpoint_t {
protected:
    int rank;
    ContactInfo_hub ci; // unused
    static mpi::communicator local_comm; // class attribute

public:    
    /* TODO */
    void init_end();
    void init() {init_end(); }
    
    ContactInfo* getMyContactInfo(){
        return &(this->ci);
    }
};

class Communication_hub_t : public Communication_t, public HUBneighbor_t, public Endpoint_hub_t {

protected:
    int rank;
    ContactInfo_hub ci; // unused
    
 public:

    Communication_hub_t() {}
    
    void init( int );
    void setContactInfo( ContactInfo& ci ) {
        // nothing
    }
    int posh__get(  void* target, const void* source, size_t size, int pe );
    int posh__put(  void* target, const void* source, size_t size, int pe );

};

#endif // define _POSH_HUB_H_
