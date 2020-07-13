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

#ifndef _POSH_SM_H_
#define _POSH_SM_H_

#include "posh_contactinfo.h"
#include "posh_endpoint.h"
#include "posh_communication.h"

#include <boost/interprocess/managed_shared_memory.hpp>
namespace bip = boost::interprocess;

int shmem_sm_put( int, void*, const void*, size_t );
int shmem_sm_get( int, void*, const void*, size_t );
//void shmem_sm_init( void );

class ContactInfo_SM  : public ContactInfo  {

public:
    ContactInfo_SM(){
        this->ready = false;
    }

    std::ostream& doprint( std::ostream& os ) const {
        return os << "SM -- rank " << rank << " " << hostname;
    }
    std::istream& doinput( std::istream& is ) {
        std::string sep, sep2, type, host, _rank;
        is >> type >> sep >> sep2 >>  _rank >> host;
        this->rank = std::stoi( _rank );
        this->hostname = host;
       return is;
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

class SMneighbor_t{
 public:
    bip::managed_shared_memory remoteHeap;
};

class Endpoint_SM_t : public Endpoint_t {
protected:
    int rank;
    ContactInfo_SM ci;
    
public:    
    /* TODO */
    void init_end(){
        char hostname[HOST_NAME_MAX+1];
        gethostname( hostname, HOST_NAME_MAX );
        this->ci.setHostname( hostname );
        
        int rank = myInfo.getRank();
        this->ci.setRank( rank );
   }
    /*    void init(){
        char hostname[HOST_NAME_MAX+1];
        gethostname( hostname, HOST_NAME_MAX );
        this->ci.setHostname( hostname );
        
        int rank = myInfo.getRank();
        this->ci.setRank( rank );
        }*/
    int finalize(){ return 0; }
    neighbor_comm_type_t getType() { return TYPE_SM; }
    
    ContactInfo* getMyContactInfo(){
        return &(this->ci);
    }
    void setMyContactInfo( int rank ) {
        this->rank = rank; 
    }
};

class Communication_SM_t : public Communication_t, public SMneighbor_t, public Endpoint_SM_t {

protected:
    int rank;
    ContactInfo_SM ci;
    
 public:

    Communication_SM_t() {}
    
    void init_comm( int );
    void setContactInfo( ContactInfo& ci ) {
        // nothing
    }
    void reopen(){ init_end( ); }
    void close( void ){
	// TODO
	//	    auto* ptr = &(this->neighbors[rank].comm_channel.sm_channel.shared_mem_segment);
    //    ptr->~managed_shared_memory();    /* FIXME there is a memory leak here */
    /* auto* ptr = &(this->neighbors[rank].comm_channel.sm_channel);
       ptr->~sm_comm_channel_t();*/

    }
    void posh__get(  void* target, const void* source, size_t size, int pe ){
        shmem_sm_get( pe, target, source, size );
    }
    void posh__put(  void* target, const void* source, size_t size, int pe ){
        shmem_sm_put( pe, target, source, size );
    }

};

#endif // define _POSH_SM_H_
