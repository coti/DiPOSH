/*
 * Copyright (c) 2020 LIPN - Universite Sorbonne Paris Nord
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

#ifndef _POSH_GASPI_H_
#define _POSH_GASPI_H_

#include "posh_contactinfo.h"
#include "posh_endpoint.h"
#include "posh_communication.h"
#include <GASPI.h>
#include <iostream>

#define GASPI_SEGMENT_SIZE  0x1 << 10

class ContactInfo_Gaspi : public ContactInfo {

  neighbor_comm_type_t type = TYPE_GASPI;
  int rank;
  bool ready;

 public:
  ContactInfo_Gaspi(){
    this->ready = true;
  }
  ContactInfo_Gaspi( neighbor_comm_type_t type, int rank ) {
    this->ready = true;
    this->type = type;
    this->rank = rank;
  }
    std::ostream& doprint( std::ostream& os ) const {
        return os << "GASPI -- rank " << this->rank << " " << hostname;
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

class Endpoint_Gaspi_t : public Endpoint_t {

protected:
    ContactInfo_Gaspi ci;
    gaspi_segment_id_t segment_id;
    gaspi_size_t segment_size;
    gaspi_pointer_t segment_begin;
    gaspi_queue_id_t queue_id = 0;

public:
    void init_end( void );

    int finalize(){
        //gaspi_proc_term( GASPI_BLOCK );
        return 0;
    }
    ContactInfo* getMyContactInfo(){
        return &(this->ci);
    }
    void setMyContactInfo( int rank ) {
      this->ci.setRank( rank ); 
    }
    neighbor_comm_type_t getType(){ return TYPE_GASPI; }

    gaspi_segment_id_t getSegmentId() { return this->segment_id; }
    gaspi_size_t getSegmentSize(){ return segment_size; }
    gaspi_pointer_t getSegmentBegin(){ return segment_begin; }
    gaspi_queue_id_t getQueue(){ return queue_id; }


    void* posh__shmalloc( size_t s ){
        std::cout << "GASPI Shmalloc" << std::endl;
        gaspi_pointer_t array;
        gaspi_segment_ptr( this->segment_id, &array );
        /* TODO */
        return (void *)(array);;
    }
    void* posh__shmemalign( size_t a, size_t s ){
        std::cout << "GASPI Shmalloc" << std::endl;
        /* TODO */
        return NULL;
    }
    void* posh__shrealloc( void* p, size_t s ){
        std::cout << "GASPI Shmalloc" << std::endl;
        /* TODO */
        return NULL;
    }
    void posh__shfree( void* p ){
        std::cout << "GASPI Shmalloc" << std::endl;
        /* TODO */
    }
};


class Communication_Gaspi_t : public  Communication_t, public Endpoint_Gaspi_t {

 public:
    /*  Communication_Gaspi_t(){  }*/

  void init_comm( int rank ){
    
    
  }
  void reopen( void ){

  }
  void close( void ){

  }
  void setContactInfo( ContactInfo& ci ) {

  }
  void posh__get(  void* target, const void* source, size_t size, int pe );
  void posh__put(  void* target, const void* source, size_t size, int pe );


};

#endif

