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

#ifndef _POSH_KNEM_H_
#define _POSH_KNEM_H_

#include "posh_contactinfo.h"
#include "posh_endpoint.h"

#define KNEM_LIMIT 1024 /* max size of an iovec */


class ContactInfo_KNEM : public ContactInfo {

    neighbor_comm_type_t type = TYPE_KNEM;
    
    int rank;
    knem_cookie_t cookie;
    /*std::atomic<bool>*/ bool ready; //{ false };

public:
    template<class Archive>
        void serialize( Archive & ar, const unsigned int version) {
        ar & type;
        ar & rank;
        ar & cookie;
        ar & ready;
    }

    ContactInfo_KNEM(){
        this->ready = false;
    }
    ContactInfo_KNEM( neighbor_comm_type_t  type, knem_cookie_t cookie ){
        this->cookie = cookie;
        this->type = type;
    }
    std::ostream& doprint( std::ostream& os ) const {
        return os << "KNEM -- rank " << rank << " cookie " << cookie << hostname;
    }

    std::istream& doinput( std::istream& is ) {
        std::string type, sep, sep2, sep3, host, _rank;
        knem_cookie_t _cookie;
        is >> type >> sep >> sep2 >> _rank >> sep3 >> _cookie >> host;
        this->rank = std::stoi( _rank );
        this->cookie = _cookie;
        this->hostname = host;
        return is;
    }

    int getRank(){
        return rank;
    }
    void setRank( int rank ){
        this->rank = rank;
    }
    
    knem_cookie_t getCookie(){
        return this->cookie;
    }
    void setCookie( knem_cookie_t c ){
        this->cookie = c;
    }
    neighbor_comm_type_t getType(){
        return type;
    }
    void setType( neighbor_comm_type_t type ){
        this->type = type;
    }
    bool isReady() { return this->ready; }

};                                   


class Endpoint_KNEM_t : public Endpoint_t{
protected:
    ContactInfo_KNEM ci;
    int knem_fd;
    
 public:
    Endpoint_KNEM_t(){}
    neighbor_comm_type_t getType() { return TYPE_KNEM; }
    
    void setMyContactInfo( int rank ){
        this->ci.setRank( rank );
    }
    void init_end( );
    knem_cookie_t getCookie(){
        return this->ci.getCookie();
    }
    int getSocket(){
        return this->knem_fd;
    }
    void setSocket( int sd ){
        this->knem_fd = sd;
    }
    ContactInfo* getMyContactInfo(){
        return &(this->ci);
    }
    int finalize(){ return 0; /* we don't need to call MPI_Finalize */ } 
};

class Communication_KNEM_t : public Communication_t, public Endpoint_KNEM_t {

 public:
    
    //   void init( int rank ){
        /* TODO */
    //    }
    void init_comm( int rank ){
        //        shmem_mpi_init(  );
    }
    void reopen(){ /*init(this->rank);*/ } // TODO
    void close( void ){} // TODO
    void setContactInfo( ContactInfo& ci ) {
        ContactInfo_KNEM* ck = static_cast<ContactInfo_KNEM*>( &ci );
        this->ci.setCookie( ck->getCookie() );
    }
    /*    void setContactInfo( int rank ) {
         this->ci.setRank( rank );
         }*/

    void posh__get(  void* target, const void* source, size_t size, int pe );
    void posh__put(  void* target, const void* source, size_t size, int pe );

};

#endif // define _POSH_KNEM_H_
