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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/uio.h>
#include <poll.h>
#include <unistd.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <netinet/tcp.h>

#include "shmem_internal.h"
#include "posh_tcp.h"
#include "ked_tcp.h"
#include "posh_heap.h"

struct pollfd* fds;
int ncli;
pthread_mutex_t pollmtx = PTHREAD_MUTEX_INITIALIZER;

/* Prototypes of internal functions */ 

size_t recv_get( int, size_t, size_t );
size_t recv_put( int, size_t, size_t );
void   ked_insert_poll_socket( int );
int    ked_send_rank( int );
int    ked_recv_rank( int );
void   ked_insert_neighbor_new_conn( int, int );

/* put primitive */

size_t ked_put_tcp( int sock, void* dst_addr, const void* src_addr, size_t size ){
    ked_header_t put_header;
    struct iovec iov[2];
    unsigned int rc, data_len;
#ifdef _PROFILING
    struct timespec s_time, e_time;

    clock_gettime( CLOCK_REALTIME, &s_time );
#endif

    put_header.comm_type = PUT;
    put_header.address = _getOffset( dst_addr );
    put_header.size = size;
#if _TCP_DEBUG
    std::cout << "Offset: " << (int)put_header.address << " from address " << dst_addr << std::endl;
#endif

    iov[0].iov_len = sizeof( ked_header_t );
    iov[0].iov_base = (char*)&put_header;

    iov[1].iov_len = size;
    iov[1].iov_base = (char*)src_addr;

    rc = 0;
    while( rc < iov[0].iov_len + iov[1].iov_len ) {
        data_len = writev( sock, iov, 2 );
        if( 0 > data_len ) {
            perror("writev() 49" );
            exit(errno);
        }
        rc += data_len;
#if _TCP_DEBUG
        printf( "PUT sent %d bytes\n", rc );
#endif
    }

#ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &e_time );

    std::cout << __FUNCTION__ << " total time: " << TIMEDIFF( s_time, e_time ) << " ns " << std::endl;
#endif

    return (size_t)rc;
}

/* get primitive */

size_t ked_get_tcp( int sock, void* dst_addr, const void* src_addr, size_t size ){
    ked_header_t get_header;
    struct iovec iov_send, iov_recv;
    int rc, data_len;
 #ifdef _PROFILING
    struct timespec s_time, r_time, e_time;

    clock_gettime( CLOCK_REALTIME, &s_time );
#endif

    get_header.comm_type = GET;
    get_header.address = _getOffset( src_addr );
    get_header.size = size;

    iov_send.iov_len = sizeof( ked_header_t );
    iov_send.iov_base = (char*)&get_header;
    iov_recv.iov_len = size;
    iov_recv.iov_base = (char*)dst_addr;

    rc = 0;
    while( rc < iov_send.iov_len ) {
        data_len = writev(  sock, &iov_send, 1 );
        if( 0 > data_len ) {
            perror( "writev() 81" );
            exit(errno);
        }
        rc += data_len;
#if _TCP_DEBUG
        printf( "GET sent %d bytes, offset %d\n", rc, get_header.address );
#endif
    }

 #ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &r_time );
#endif
    
    rc = 0;
#if _TCP_DEBUG
    printf( "get: put the result at addr %p\n", dst_addr );
#endif
    while( rc < iov_recv.iov_len ) {
        data_len = readv(  sock, &iov_recv, 1 );
        if( 0 >= data_len ) {
            if( 0 == data_len ) {
                printf( "%d Remote peer disconnected\n", myInfo.getRank() );
            } else {
                perror("readv()");
            }
            exit(errno);
        }
        rc += data_len;
#if _TCP_DEBUG
        printf( "GET read %d bytes\n", rc );
#endif
    }
#if _TCP_DEBUG
    std::cout << "Get received " << *((int*)(iov_recv.iov_base)) << std::endl;
#endif

#ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &e_time );

    std::cout << __FUNCTION__ << " total time: " << TIMEDIFF( s_time, e_time ) << " ns ";
    std::cout << "- request receive time: " <<  TIMEDIFF( s_time, r_time ) << " ns ";
    std::cout << "- communication time: " <<  TIMEDIFF( r_time, e_time ) << " ns" << std::endl;
#endif

    return (size_t) rc;
}

/* Perform a remote memory allocation */

size_t ked_remote_fake_shmalloc_tcp( int sock, size_t size ) {
    ked_header_t shmalloc_header;
    struct iovec iov, iov_recv;
    unsigned int rc, data_len;
    char* recv;

    shmalloc_header.comm_type = SHMALLOC;
    shmalloc_header.address = 0;
    shmalloc_header.size = size;

    iov.iov_len = sizeof( ked_header_t );
    iov.iov_base = (char*)&shmalloc_header;
    iov_recv.iov_len = sizeof( char* );
    iov_recv.iov_base = &recv;

    /* Send the size that needs to be allocated on the remote peer */

    rc = 0;
    while( rc < iov.iov_len ) {
        data_len = writev(  sock, &iov, 1 );
        if( 0 > data_len ) {
            perror( "writev() 132" );
            exit(errno);
        }
        rc += data_len;
#if _TCP_DEBUG
        printf( "SHMALLOC sent %d bytes\n", rc );
#endif
    }

    /* Receive the offset */

    rc = 0;
    while( rc < iov_recv.iov_len ) {
        data_len = readv(  sock, &iov_recv, 1 );
        if( 0 >= data_len ) {
            if( 0 == data_len ) {
                printf( "%d Remote peer disconnected\n", myInfo.getRank() );
            } else {
                perror("readv()");
            }
            exit(errno);
        }
        rc += data_len;
    }


    return (size_t) rc;
}

/* Once a socket is connected, the client sends its rank to declare who it is */

int ked_send_rank( int sd ) {
    int rank, data_len, rc;
    struct iovec iov;
        
    rank = myInfo.getRank();
    iov.iov_base = (char*) &rank;
    iov.iov_len = sizeof( rank );
    
    rc = 0;
    while( rc < iov.iov_len ) {
        data_len = writev( sd, &iov, 1 );
        if( 0 > data_len ) {
            perror("writev() 216" );
            exit(errno);
        }
        rc += data_len;
#if _TCP_DEBUG
        printf( "%d sent my id\n", rank );
#endif
    }
    return rc;
}

/* ... and the server receives it */

int ked_recv_rank( int sd, int *rank ) {
    int data_len, rc;
    struct iovec iov;
    
    iov.iov_base = (char*) rank;
    iov.iov_len = sizeof( *rank );
    
    rc = 0;
    while( rc < iov.iov_len ) {
        data_len = readv( sd, &iov, 1 );
        if( 0 > data_len ) {
            perror("readv() 205" );
            exit(errno);
        }
        rc += data_len;
#if _TCP_DEBUG
        printf( "%d recv id %d\n", myInfo.getRank(), *rank );
#endif
    }
    return rc;
}

/* When a new client has establised a new connection, insert it 
   as the communication channel of a neighbor */

void ked_insert_neighbor_new_conn( int sd, int rank ){
    Neighbor_t* nb;
    nb = myInfo.getNeighbor( rank );
    nb->comm_type = TYPE_TCP;
    /* insert the socket */
    Communication_TCP_t* channel = (Communication_TCP_t*) nb->communications;
    channel->setSocket( sd );
    //    nb->communications->setSocket( sd );
    /* no need to set the address, will not be used */
}

/* Connect to a remote process */

int ked_connect( Neighbor_t* nb ) {
    int sd, rc;
    struct hostent *lh, *hp;
    ContactInfo* ci;

    sd = socket( AF_INET, SOCK_STREAM, 0 );
    if( 0 > sd ) {
        perror("socket() 175");
        exit(errno);
    }

    int flags = 1; //SO_RCVBUF;
    // TCP_NODELAY
    // TCP_QUICKACK
    if( setsockopt( sd, SOL_SOCKET, SO_BUSY_POLL, (void *)&flags, sizeof(flags))) {
#if _TCP_DEBUG
        std::cout << "Failed to set SO_BUSY_POLL option on the socket" << std::endl;
#endif
        if( errno != EPERM ) { /* Operation not permitted */
            perror("ERROR: setsocketopt()");
            exit(0);
        }
    }

    ci = &(nb->neigh_ci.front());     /* Take the first contact info you find (will change later) */

    // TODO : move this somewhere else
     struct sockaddr_in addr;
    Communication_TCP_t* channel = (Communication_TCP_t*) nb->communications;
    
    /*memset( &(addr), 0, sizeof(struct sockaddr_in) );
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ci->getAddr();
    addr.sin_port = ci->getPort();
    //nb->communications->setAddr( addr );
    channel->setAddr( addr );
    // end move
    */
    
    addr = channel->getAddr();
    
#if _TCP_DEBUG    
    std::cout << myInfo.getRank();
    std::cout << " connect to " << *ci << " on peer " << nb->rank << std::endl;
    
    std::cout << "Connect to " << nb->rank << " on addr " << addr.sin_addr.s_addr << ":" << addr.sin_port << std::endl;
#endif

    /* Connect to the remote process */

    rc = connect( sd, (struct sockaddr*) (&addr ), sizeof( struct sockaddr_in ) );
    if( 0 > rc ) {
        perror("connect()");
        exit(errno);
    }

    /* Insert the socket into the polling structure */

    ked_insert_poll_socket( sd );

    /* Send my id */

    rc = ked_send_rank( sd );
    
    return sd;
}

/* Executed by the communication thread */

size_t recv_get( int sd, size_t src_addr, size_t size ){
    struct iovec iov;
    int rc, data_len;
    int i;
#ifdef _PROFILING
    struct timespec s_time, e_time;

    clock_gettime( CLOCK_REALTIME, &s_time );
#endif

    /* the src addr is the offset in the shared heap */

    iov.iov_base = _getLocalAddr( src_addr );
    iov.iov_len = size;

#if _TCP_DEBUG
    printf( "Ready to send %d bytes from %p offset %d\n", (int)size, iov.iov_base, src_addr );
#endif
    rc = 0;
    while( rc < iov.iov_len ) {
        data_len = writev(  sd, &iov, 1 );
        if( 0 > data_len ) {
            perror("writev 219" );
            exit(errno);
        }
        rc += data_len;
    }
#if _TCP_DEBUG
    printf( "%d sent %d bytes for a get\n", myInfo.getRank(), rc );
    std::cout << "Sent " << *((int*)(iov.iov_base)) << std::endl;
#endif
    
#ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &e_time );

    std::cout << __FUNCTION__ << " total time: " << TIMEDIFF( s_time, e_time ) << " ns " << std::endl;
#endif

    return (size_t) rc;
}

/* Executed by the communication thread */

size_t recv_put( int sd, size_t dst_add, size_t size ){
    struct iovec iov;
    int rc, data_len;
#ifdef _PROFILING
    struct timespec s_time, e_time;

    clock_gettime( CLOCK_REALTIME, &s_time );
#endif

    /* the dst addr is the offset in the shared heap */

    iov.iov_len = size;
    iov.iov_base = _getLocalAddr( dst_add );

#if _TCP_DEBUG
   printf( "Ready to receive %d bytes at address %p\n", (int)size, iov.iov_base );
#endif
    rc = 0;
    while( rc < iov.iov_len ) {
        data_len = readv(  sd, &iov, 1 );
        if( 0 >= data_len ) {
            if( 0 == data_len ) {
                printf( "%d Remote peer disconnected\n", myInfo.getRank() );
            } else {
                perror("readv()");
            }
            exit(errno);
        }
        rc += data_len;
    }
    //    printf( "received %s\n",(char*) iov.iov_base );
#if _TCP_DEBUG
    std::cout << myInfo.getRank() << " Received " << (int)rc << " bytes" << std::endl;
#endif
    
#ifdef _PROFILING
    clock_gettime( CLOCK_REALTIME, &e_time );

    std::cout << __FUNCTION__ << " total time: " << TIMEDIFF( s_time, e_time ) << " ns " << std::endl;
#endif

    return (size_t) rc;
}

/* Executed by the communication thread */

size_t recv_shmalloc( int sd, size_t size ) {
    struct iovec iov;
    unsigned int rc, data_len;
    void* ptr;
    size_t offset;

    /* Received the size -> perform the allocation */
    
    ptr = myHeap.myHeap.allocate( size );
    
    /* Send back the offset */

    offset = _getOffset( ptr );

    iov.iov_base = ptr;
    iov.iov_len = sizeof( char* );

#if _TCP_DEBUG
    printf( "Ready to send %d bytes: %p\n", (int)size, ptr );
#endif
    rc = 0;
    while( rc < iov.iov_len ) {
        data_len = writev(  sd, &iov, 1 );
        if( 0 > data_len ) {
            perror("writev() 281" );
            exit(errno);
        }
        rc += data_len;
    }
#if _TCP_DEBUG
    printf( "sent %s\n", (char*)iov.iov_base );
#endif
    
    return (size_t) size;
}

void ked_insert_poll_socket( int sd ) {

    pthread_mutex_lock( &pollmtx );
    ncli++;
    fds[ncli].fd = sd;
    fds[ncli].events = POLLIN;
    pthread_mutex_unlock( &pollmtx );

}

/* Executed by the communication thread */

size_t recv_data( int sd ) {
    unsigned int rc, data_len;
    struct iovec iov;
    ked_header_t myheader;

    iov.iov_base = (char*) &myheader;
    iov.iov_len = sizeof( ked_header_t );
    
    /* read the first iovec, containing the header */
    
    rc = 0;
    while( rc < iov.iov_len ) {
        data_len = readv(  sd, &iov, 1 );
         if( 0 >= data_len ) {
            if( 0 == data_len ) {
                printf( "%d Remote peer disconnected\n", myInfo.getRank() );
            } else {
                perror("readv()");
            }
            exit(errno);
         }
        rc += data_len;
    }

    /* proceed accordingly */

    switch( myheader.comm_type ) {
    case PUT:
#if _TCP_DEBUG
        printf( "Recv put %d bytes of data at addr %p\n", (int)myheader.size, (char*)myheader.address );
#endif
        rc = recv_put( sd, myheader.address, myheader.size );
        break;
    case GET:
#if _TCP_DEBUG
        printf( "Recv get %d bytes of data from addr %p\n", (int)myheader.size, (char*)myheader.address );
#endif
        rc = recv_get( sd, myheader.address, myheader.size );
        break;
    case SHMALLOC:
#if _TCP_DEBUG
        printf( "Recv shmalloc %d bytes in my heap\n", (int)myheader.size );
#endif
        rc = recv_shmalloc( sd, myheader.size );
        break;
    default:
        printf( "Recv something else " );
        break;
    }

    return (size_t) rc;
}

/* Communication thread */

void* ked_thread_comm( ){

    int rc, i, rank;
    int port;
    unsigned int len;
    int sock;
    int sock_cli;
    struct sockaddr_in listen_addr;
    struct sockaddr_in remote_addr;
    char* data;
    int nfds, polled;
    socklen_t socklen;

    ked_header_t put_header;

    len = sizeof( struct sockaddr_in );

    /* Find my IP address */

    struct sockaddr_in myaddr;
    struct ifaddrs *interfaces = NULL;
    rc = getifaddrs( &interfaces );
    
    for ( auto i = interfaces; i != NULL; i = i->ifa_next) {
        if( i->ifa_addr->sa_family == AF_INET ) {
            std::string ipAddress = inet_ntoa(((struct sockaddr_in*)i->ifa_addr)->sin_addr);
            if( ipAddress != "127.0.0.1" ){
                //                std::cout << "IP: " << ipAddress << std::endl;
                std::memcpy( &myaddr, i->ifa_addr, sizeof(struct sockaddr_in) );
                break;
            }
        }
    }

    /* Creation of a listen socket */

    sock = socket( AF_INET, SOCK_STREAM, 0 );
    if( 0 > sock ) {
        perror("socket()");
        exit(errno);
    }

    memset( &listen_addr, 0, sizeof(struct sockaddr_in) );
    listen_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons( 0 );
    rc = bind( sock, (struct sockaddr*)&listen_addr, len );
    if( 0 > rc ) {
        perror("bind()");
        exit(errno);
    }
    rc = getsockname( sock, (struct sockaddr*)&listen_addr, &len );
    myInfo.setMyContactInfo( myInfo.getRank(), myaddr.sin_addr.s_addr, (uint16_t)listen_addr.sin_port );

    rc = listen( sock, BACKLOG );
    if( 0 > rc ) {
        perror("listen()");
        exit(errno);
    }
    
    /* preparation of a polling structure */

    nfds = myInfo.getSize();
    pthread_mutex_lock( &pollmtx );
    fds = (struct pollfd*) malloc( (nfds+1) * sizeof( struct pollfd ) );
    memset( fds, 0, sizeof( fds ) );
    fds[0].fd = sock;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    ncli = 0;
    pthread_mutex_unlock( &pollmtx );
    socklen = sizeof( struct sockaddr* );

    while( 1 ) {

        polled = poll( fds, ncli+1 /* nfds */, -1 );
        //        printf(" poll\n" );
        /* TODO : gerer en cas d'interruption */

        for( i = 0 ; i < ncli+1 ; i++ ) {
            if( 0 == fds[i].revents ) {      /* nothing happened here. */
                continue;
            }
            if( POLLIN != fds[i].revents ) { /* unexpected event. This is an error */
                fprintf( stderr, "Error on socket %d\n", fds[i].fd );
                break;
            }
            if( sock == fds[i].fd ) {        /* accept incoming connexions */
                sock_cli = accept( sock, (struct sockaddr*) &remote_addr, &socklen );
#if _DEBUG
                printf( "%d accept new connection\n", myInfo.getRank() );
#endif
                ked_insert_poll_socket( sock_cli );
                /* Receive the other process's rank */
                rc = ked_recv_rank( sock_cli, &rank );
                /* Insert it in the appropriate neighbor */
                ked_insert_neighbor_new_conn( sock_cli, rank );
                
            } else {
                rc = recv_data( fds[i].fd );
                if( 0 == rc ) {
                    printf( "%d disconnected\n", fds[i].fd );
                    close( fds[i].fd );
                    fds[i].events = POLLNVAL;
                    fds[i].fd = -1;
                }
            }
            
        }

    }
 exit:
    close( sock );
}

#if 0
int main(){

    int rc;
    pthread_t tid;

    int port;
    char* buff = "je sers la science et c est ma joie";
    int len, data_len;

    ked_header_t put_header;
    struct iovec *iov;

    port = _MY_PORT_;
    rc = pthread_create( &tid, NULL, thread_comm, (void*) &port );
    if( 0 != rc ) {
        printf("Error in thread creation: %s\n", strerror(errno));
    }

    /* Creation of a communication socket */

    int sock = socket( AF_INET, SOCK_STREAM, 0 );
    if( 0 > sock ) {
        perror("socket()");
        exit(errno);
    }

    struct sockaddr_in addr;
    memset( &addr, 0, sizeof(struct sockaddr_in) );
    addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
    addr.sin_family = AF_INET;
    addr.sin_port = htons( _MY_PORT_ );
    
    /* connect to the server */

    sleep( 2 );

    rc = connect( sock, (struct sockaddr*) &addr, sizeof( struct sockaddr_in ) );
    if( 0 > rc ) {
        perror("connect()");
        exit(errno);
    }

    /* write something */

    len = strlen( buff );
    rc = put_tcp( sock, NULL, buff, len );
    printf( "put 0 %d bytes\n", rc );

    len = strlen( buff );
    rc = get_tcp( sock, buff, NULL, 8 );
    printf( "get 0 %d bytes\n", rc );

    close(  sock );

    rc = pthread_join( tid, NULL );
    if( 0 != rc ) {
        printf("Error in pthread_join: %s\n", strerror(errno));
    }

    return EXIT_SUCCESS;
}
#endif
