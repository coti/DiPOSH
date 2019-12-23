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

#include <ostream>
#include <arpa/inet.h>

#include "posh_tcp.h"
#include "ked_tcp.h"
#include "posh_contactinfo.h"


std::ostream& operator<< ( std::ostream& out, Communication_TCP_t& tcp ){
    char ipAddress[INET6_ADDRSTRLEN];
    uint32_t ip_int = tcp.getAddr().sin_addr.s_addr;   
    inet_ntop( AF_INET, (const void*)&ip_int, ipAddress, sizeof(ipAddress));      
    out << ipAddress << " " ;
    out << (int)(tcp.getAddr().sin_port);
    out << " on socket " << tcp.getSocket();
    return out;
}

int TCPendpoint_t::getmyTCPport(){
    return this->address.sin_port;
}

void TCPendpoint_t::setMyTCPthreadID( boost::thread& tid ){
    this->ked_thread = std::move( tid );
}

boost::thread* TCPendpoint_t::getMyTCPthreadID( ){
    return &(this->ked_thread);
}

void TCPendpoint_t::shmem_tcp_init(  ) {
    
    boost::thread comm_thread( &ked_thread_comm );    
    this->setMyTCPthreadID( comm_thread );
    
/* FIXME TODO */
#if 0
           /* neighbor located on a different machine */
    std::cout << myInfo.getRank() << " init TCP on neighbor " << neighborrank << std::endl;
    //            this->neighbors[neighborrank].comm_channel.tcp_channel.hostname = std::string( this->neighbors[neighborrank].hostname );
    this->neighbors[neighborrank].comm_channel.tcp_channel.setAddr( this->neighbors->neigh_ci[0].getAddr(), this->neighbors->neigh_ci[0].getPort() ); // FIXME uh ???
    this->neighbors[neighborrank].comm_type = TYPE_TCP;
#endif
}
