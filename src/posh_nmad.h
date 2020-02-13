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

#ifndef _POSH_NMAD_H_
#define _POSH_NMAD_H_

#include <iostream>
#include "posh_communication.h"

extern "C" {
#include <nm_public.h>
#include <nm_launcher_interface.h>
#include <nm_coll_interface.h>  /* for nm_comm_t */
#include <nm_onesided_interface.h>
}

int shmem_nmad_put( int, void*, const void*, size_t );
int shmem_nmad_get( int, void*, const void*, size_t );

void shmem_nmad_exchange_ci( void );

class ContactInfo_NMAD : public ContactInfo {
    
private:
    neighbor_comm_type_t type = TYPE_NMAD;
    
    int rank;
    uintptr_t base_addr;
    
public:
    /*std::atomic<bool>*/ bool ready; //{ false };

    template<class Archive>
        void serialize( Archive & ar, const unsigned int version) {
        ar & type;
        ar & rank;
        ar & base_addr;
        ar & ready;
    }

    ContactInfo_NMAD(){
        this->ready = false;
    }
    ContactInfo_NMAD( neighbor_comm_type_t  type, int rank ){
        this->rank = rank;
        this->type = type;
    }
    std::ostream& doprint( std::ostream& os ) const {
        return os << "NMAD -- rank " << rank << " base addr " << base_addr;
    }


    uintptr_t getBaseAddress(){
        return this->base_addr;
    }
    void setBaseAddress( uintptr_t addr ) {
        this->base_addr = addr;
    }
    bool isReady() { return this->ready; }

};

extern std::ostream& operator<< ( std::ostream&, ContactInfo_NMAD& );

class NMADendpoint_t : public Endpoint_t {
private:
    nm_gate_t p_gate = NULL;
    nm_session_t nm_session = NULL;
    nm_onesided_t os_session = NULL;

 protected:
    uintptr_t base_addr = 0;
    
 public:
    void shmem_nmad_init( void ){
        ;;
    }
    int finalize(){} // TODO
    
    nm_gate_t getGate( void ) { return this->p_gate; }
    nm_gate_t getGate( int& peer ) {   nm_gate_t g;   nm_launcher_get_gate( peer, &g ); return g; }
    void setGate( nm_gate_t& g ) { this->p_gate = g; }
    void setGate( int& peer ) {  nm_gate_t g;   nm_launcher_get_gate( peer, &g ); this->p_gate = g; }
    void setSession( nm_session_t& s ) { this->nm_session = s; }
    nm_session_t setSession( void ) { return this->nm_session; }
    void setOsSession( nm_onesided_t& s ) {this->os_session = s;}
    nm_onesided_t getOsSession( void ) {return this->os_session;}
    void setBaseAddr( uintptr_t addr ) { this->base_addr = addr; }
    uintptr_t getBaseAddr( ) { return this->base_addr; }

};

class Communication_NMAD_t : public Communication_t, public NMADendpoint_t {
    
 public:
    
    void init( int rank ){
        setGate( rank );
    }
    void setContactInfo( ContactInfo& ci ){
        this->base_addr = ci.getBaseAddress();
    }
    int posh__get(  void* target, const void* source, size_t size, int pe ){
        return shmem_nmad_get( pe, target, source, size );
    }
    int posh__put(  void* target, const void* source, size_t size, int pe ){
        return shmem_nmad_put( pe, target, source, size );
    }

};




#endif // define _POSH_NMAD_H_
