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


#include <knem_io.h>
#include <math.h>       /* ceil */

#include "shmem_internal.h"
#include "posh_heap.h"
#include "posh_contactinfo.h"

#include "posh_knem.h"

void Endpoint_KNEM_t::init_end( ){
     int rank = myInfo.getRank();
     this->init_end( rank );
}

void Endpoint_KNEM_t::init_end( int rank ){
    int knem_fd, err;
    knem_cookie_t mycookie;
     
     struct knem_cmd_create_region create;
     struct knem_cmd_param_iovec knem_iov[1];
     
     knem_fd = open( KNEM_DEVICE_FILENAME, O_RDWR );
     if( knem_fd < 0 ) {
         perror( "Opening KNEM fd" );
         return;
     }
     
     knem_iov[0].base = (uint64_t)myHeap.getBaseAddr();
     knem_iov[0].len = myHeap.getSize();
     
     create.iovec_array = (uintptr_t) &knem_iov[0];
     create.iovec_nr = 1;
     create.flags = KNEM_FLAG_ANY_USER_ACCESS;
     create.protection = PROT_READ | PROT_WRITE; /*  allow both remote readers and writers */
     err = ioctl( knem_fd, KNEM_CMD_CREATE_REGION, &create );

     if( 0 != err ) {
         perror( "ioctl (create region)" );
     }
     mycookie = create.cookie;

     this->ci.setCookie( mycookie );
     this->setSocket( knem_fd );
     this->ci.setRank( rank );
}


void Communication_KNEM_t::posh__get( void* target, const void* source, size_t size, int pe ){
  struct knem_cmd_inline_copy icopy;
  struct knem_cmd_param_iovec* knem_iov;
  struct knem_cmd_param_iovec  knem_iov_s[1];
  struct knem_cmd_param_iovec* knem_iov_d;
  int err, knem_fd, nb_iov, i, j;
  
  managed_shared_memory::handle_t handle = myHeap.myHeap.get_handle_from_address( source );
  Neighbor_t* nb = myInfo.getNeighbor( pe );
  knem_cookie_t cookie = ((Communication_KNEM_t*)(nb->communications))->getCookie();
        
  /* beyond a certain size, cut the message into several iovecs */

  if( _shmem_likely( size < KNEM_LIMIT ) ) {
      knem_iov = &( knem_iov_s[0] );
      nb_iov = 1;

      knem_iov[0].base = reinterpret_cast<uint64_t>( target );
      knem_iov[0].len = size;
  } else {
      nb_iov = ceil( (double)size / (double)KNEM_LIMIT );
      knem_iov_d = (struct knem_cmd_param_iovec*) malloc( nb_iov * sizeof( struct knem_cmd_param_iovec ) );
      i = 0; j = 0;
      while( i < ( size - KNEM_LIMIT ) ){
          knem_iov_d[j].base = reinterpret_cast<uint64_t>( target ) + i;
          knem_iov_d[j].len = KNEM_LIMIT;
          i += KNEM_LIMIT;
          j++;
      }
      knem_iov_d[j].base = reinterpret_cast<uint64_t>( target ) + i;
      knem_iov_d[j].len = size % KNEM_LIMIT;
      knem_iov = knem_iov_d;
  }
  
  icopy.local_iovec_array = reinterpret_cast<uintptr_t>( knem_iov );
  icopy.local_iovec_nr = nb_iov;
  icopy.remote_cookie = cookie;
  icopy.remote_offset = (int) handle;
  icopy.write = 0; /* read from the remote region into our local segments */
  icopy.flags = 0;

  knem_fd = this->getSocket();
  
  err = ioctl( knem_fd, KNEM_CMD_INLINE_COPY, &icopy );
  if( 0 != err ) {
      perror( "KNEM get" );
  }

  if (icopy.current_status != KNEM_STATUS_SUCCESS){
    printf("request failed\n");
    perror( "KNEM get" );
  }
  
  if( _shmem_unlikely( size >= KNEM_LIMIT ) ) {
      free( knem_iov_d );
  }
}


void Communication_KNEM_t::posh__put(  void* target, const void* source, size_t size, int pe ){
  struct knem_cmd_inline_copy icopy;
  struct knem_cmd_param_iovec* knem_iov;
  struct knem_cmd_param_iovec  knem_iov_s[1];
  struct knem_cmd_param_iovec* knem_iov_d;
  int err, knem_fd, nb_iov, i, j;

  managed_shared_memory::handle_t handle = myHeap.myHeap.get_handle_from_address( target );
  Neighbor_t* nb = myInfo.getNeighbor( pe );
  knem_cookie_t cookie = ((Communication_KNEM_t*)(nb->communications))->getCookie();
        
  /* beyond a certain size, cut the message into several iovecs */
  
  if( _shmem_likely( size < KNEM_LIMIT ) ) {
      knem_iov = &( knem_iov_s[0] );
      nb_iov = 1;

      knem_iov[0].base = reinterpret_cast<uint64_t>( target );
      knem_iov[0].len = size;
  } else {
      nb_iov = ceil( (double)size / (double)KNEM_LIMIT );
      knem_iov_d = (struct knem_cmd_param_iovec*) malloc( nb_iov * sizeof( struct knem_cmd_param_iovec ) );
      i = 0; j = 0;
      while( i < ( size - KNEM_LIMIT ) ){
          knem_iov_d[j].base = reinterpret_cast<uint64_t>( target ) + i;
          knem_iov_d[j].len = KNEM_LIMIT;
          i += KNEM_LIMIT;
          j++;
      }
      knem_iov_d[j].base = reinterpret_cast<uint64_t>( target ) + i;
      knem_iov_d[j].len = size % KNEM_LIMIT;
      knem_iov = knem_iov_d;
  }
  
  icopy.local_iovec_array = reinterpret_cast<uintptr_t>( knem_iov );
  icopy.local_iovec_nr = nb_iov;
  icopy.remote_cookie = cookie;
  icopy.remote_offset = (int) handle;
  icopy.write = 1; /* write mode */
  icopy.flags = 0;

  knem_fd = this->getSocket();
  
  err = ioctl( knem_fd, KNEM_CMD_INLINE_COPY, &icopy );
  if( 0 != err ) {
      perror( "KNEM put" );
  }
  
  if( icopy.current_status != KNEM_STATUS_SUCCESS ){
    printf( "request failed\n" );
    perror( "KNEM put" );
  }
  
  if( _shmem_unlikely( size >= KNEM_LIMIT ) ) {
      free( knem_iov_d );
  }
}
