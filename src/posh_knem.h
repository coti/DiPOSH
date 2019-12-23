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

#define KNEM_LIMIT 1024 /* max size of an iovec */

int shmem_knem_put( int, void*, const void*, size_t );
int shmem_knem_get( int, void*, const void*, size_t );

class ContactInfo_KNEM : public ContactInfo {

    neighbor_comm_type_t type = TYPE_KNEM;
    
    int rank;
    knem_cookie_t cookie;
    /*std::atomic<bool>*/ bool ready; //{ false };

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
        return os << "KNEM -- rank " << rank << " cookie " << cookie;
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
    bool isReady() { return this->ready; }

};                                   


class KNEMendpoint_t{
 protected:
    knem_cookie_t cookie;
    int knem_fd;
    
 public:
    knem_cookie_t getCookie(){
        return this->cookie;
    }
    int getSocket(){
        return this->knem_fd;
    }
    void setSocket( int sd ){
        this->knem_fd = sd;
    }
    void shmem_knem_init( void );
};

class Communication_KNEM_t : public Communication_t, public KNEMendpoint_t {

 public:
    
    void init( int rank ){
        /* TODO */
    }
    void setContactInfo( ContactInfo_KNEM& ci ) {
        this->cookie = ci.getCookie();
    }

    int posh__get(  void* target, const void* source, size_t size, int pe ){
        return shmem_knem_get( pe, target, source, size );
    }
    int posh__put(  void* target, const void* source, size_t size, int pe ){
        return shmem_knem_put( pe, target, source, size );
    }

};

#endif // define _POSH_KNEM_H_
