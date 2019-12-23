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

#include <strings.h>

#include "shmem_internal.h"
#include "posh_tcp.h"
#include "ked_tcp.h"

bool shmem_tcp_is_connected( int );
unsigned int shmem_tcp_get_socket( int );
unsigned int shmem_tcp_connect( int );

int shmem_tcp_put( int pe, void* dst_addr, const void* src_addr, size_t size ){
    int rc;
    Neighbor_t* nb;
    unsigned int sd;

    /* Do I have an existing connection with the remote process? */

    if( _shmem_unlikely( false == shmem_tcp_is_connected( pe ) ) ){
        rc = shmem_tcp_connect( pe );
        if( rc < 0 ){
            return -1;
        }
    }

    sd = shmem_tcp_get_socket( pe );

    /* Send the data */

    rc = ked_put_tcp( sd, dst_addr, src_addr, size );

    return rc;
}

int shmem_tcp_get( int pe, void* dst_addr, const void* src_addr, size_t size ){
    int rc;
    Neighbor_t* nb;
    unsigned int sd;

    /* Do I have an existing connection with the remote process? */

    if( _shmem_unlikely( false == shmem_tcp_is_connected( pe ) ) ){
        rc = shmem_tcp_connect( pe );
        if( rc < 0 ){
            return -1;
        }
    }

    sd = shmem_tcp_get_socket( pe );

    /* Fetch the data */

    rc = ked_get_tcp( sd, dst_addr, src_addr, size );

    return rc;
}


/* Eventually, this should disappear */

size_t shmem_remote_fake_shmalloc_tcp( int pe, size_t size ) {
    int rc;
    int sd;
    if( _shmem_unlikely( false == shmem_tcp_is_connected( pe ) ) ){
        rc = shmem_tcp_connect( pe );
        if( rc < 0 ){
            return -1;
        }
    }

    sd = shmem_tcp_get_socket( pe );
    return  ked_remote_fake_shmalloc_tcp( sd, size );
}

/* internal functions */

bool shmem_tcp_is_connected( int pe ) {
    int sd;
    sd = shmem_tcp_get_socket( pe );
    return ( sd > 0 );
}

unsigned int shmem_tcp_get_socket( int pe ) {
    Neighbor_t* nb;
    int sd;
    nb = myInfo.getNeighbor( pe );
    // FIXME pourquoi j'ai fait ce test ??    
    /*    if( _shmem_unlikely( 0 == nb->hostname.compare( std::string("") ) ) ){
        std::cerr << " Error -- remote peer " << pe << " is not initalized" << std::endl;
        return -1;
        }*/
    if( _shmem_unlikely( TYPE_TCP != nb->comm_type ) ) {
        std::cerr << " Error -- remote peer " << pe << " is not a TCP peer" << std::endl;
        return -1;
    }
    Communication_TCP_t* channel = (Communication_TCP_t*) nb->communications;
    sd = channel->getSocket();
    //    sd = nb->communications->getSocket();

    return sd;
}

unsigned int shmem_tcp_connect( int pe ) {
    Neighbor_t* nb;
    int sd;
    unsigned int rc;

    if( _shmem_unlikely( false == shmem_tcp_is_connected( pe ) ) ) {
        nb = myInfo.getNeighbor( pe );
        sd = ked_connect( nb );
        Communication_TCP_t* channel = (Communication_TCP_t*) nb->communications;
        channel->setSocket( sd );
        //        nb->communications->setSocket( sd );
    }

    return sd;
}
