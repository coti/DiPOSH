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

#include "shmem_internal.h"
#include "shmem_tcp.h"

#ifndef _POSH_TCP_H_
#define _POSH_TCP_H_

//class TCPendpoint_t;
/* Specific routines */

class ContactInfo_TCP : public ContactInfo  {

    char hostname[HOST_NAME_MAX + 1]; // unused
    uint32_t ip4_addr;
    uint16_t port;

public:
    template<class Archive>
    void serialize( Archive & ar, const unsigned int version) {
        ar & type;
        ar & hostname;
        ar & ip4_addr;
        ar & port;
        ar & rank;
        ar & ready;
    }

    ContactInfo_TCP(){
        this->ready = false;
    }
    ContactInfo_TCP( int rank, const char* hname, uint32_t addr, uint16_t port ){
        this->type = TYPE_TCP;
        this->ip4_addr = addr;
        this->port = port;
        this->rank = rank;
        this->ready = true;
    }
     ContactInfo_TCP( int rank, uint32_t addr, uint16_t port ){
        this->type = TYPE_TCP;
        this->ip4_addr = addr;
        this->port = port;
        this->rank = rank;
        this->ready = true;
    }
   std::ostream& doprint( std::ostream& os ) const {
        char ipAddress[INET6_ADDRSTRLEN];
        uint32_t ip_int = ip4_addr;      
        inet_ntop( AF_INET, (const void*)&ip_int, ipAddress, sizeof(ipAddress));      
        return os << "TCP -- rank " << rank << std::endl <<  ipAddress << " " << port;
    }

    /*    neighbor_comm_type_t getType(){
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

    void setHostname( int rank, char* hname ) {
        std::strcpy( this->hostname, hname );
    }

    void setAddr( uint32_t addr, uint16_t port ){
        this->ip4_addr = addr;
        this->port = port;
        this->ready = true;
    }
    void setHostname( char* hostname ) {
        std::strcpy( this->hostname, hostname );
    }
    char* getHostname(){
        return hostname;
    }
    uint32_t getAddr( ) {
        return ip4_addr;
    }
    uint16_t getPort(){
        return port;
    }

};

extern std::ostream& operator<< ( std::ostream&, ContactInfo_TCP& );

void shmem_tcp_init( void );

class TCPendpoint_t {

private:
    void shmem_tcp_init();
    
 protected:
    std::string hostname;
    struct sockaddr_in address;
    int socket;
    
    ContactInfo_TCP ci;

public:
    boost::thread ked_thread;
    
 public:
    void init(){
        shmem_tcp_init();
    }
    void init(int rank ){ // TODO really ?
        shmem_tcp_init();
    }

    int getmyTCPport( void );
    void setMyTCPthreadID( boost::thread& );
    boost::thread* getMyTCPthreadID( void );
    struct sockaddr_in getAddr(){
        return this->address;
    }
    void setAddr( struct sockaddr_in addr ) {
        memcpy( &this->address, &addr, sizeof( struct sockaddr_in ) );
    }
    int getSocket(){
        return this->socket;
    }
    void setSocket( int sd ){
        this->socket = sd;
    }
    ContactInfo* getMyContactInfo(){
        return &(this->ci);
    }
};


class Communication_TCP_t : public Communication_t, public TCPendpoint_t {

 public:
    Communication_TCP_t(){}

    /* Interface */
    
    void init( int rank ){
        //
    }

    // ca devrait degager c'est pas grave si c'est crade
    void setContactInfo( ContactInfo& __ci ){
        ContactInfo_TCP ci_tcp = ci;
        memset( &(this->address), 0, sizeof(struct sockaddr_in) );
        this->address.sin_family = AF_INET;
        this->address.sin_addr.s_addr = ci_tcp.getAddr();
        this->address.sin_port = ci_tcp.getPort(); 
        this->socket = -1;
    }
    int posh__get(  void* target, const void* source, size_t size, int pe ){
        return shmem_tcp_get( pe, target, source, size );
    }
    int posh__put(  void* target, const void* source, size_t size, int pe ){
        return shmem_tcp_put( pe, target, source, size );
    }

};

#endif // _POSH_TCP_H_
