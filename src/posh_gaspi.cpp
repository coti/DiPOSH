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

#include "posh_gaspi.h"
#include "shmem_internal.h"

#include <iostream>

void Endpoint_Gaspi_t::init_end(){

  this->segment_id = 0;
  this->segment_size = GASPI_SEGMENT_SIZE;
  gaspi_rank_t rank;

  gaspi_proc_rank( &rank );
  
  /* Create a segment */

  //  std::cout << "Rank " << rank << " segment created" << std::endl;
  
  gaspi_segment_create( this->segment_id, 2*this->segment_size,
                        GASPI_GROUP_ALL, GASPI_BLOCK, GASPI_MEM_UNINITIALIZED );

  /* Get a pointer to the beginning of this segment */

  gaspi_segment_ptr( this->segment_id, &( this->segment_begin ) );
  printf( "init segment %d beginning %p\n", this->segment_id, this->segment_begin ) ;

  /* We need another subsegment for temporary buffers between the shared and the local memory */

  this->local_id = this->segment_id;
  this->local_begin = this->segment_begin + this->segment_size;
  printf( "init local segment %d beginning %p\n", this->local_id, this->local_begin ) ;
  
  this->ci.setRank( (int)rank );
  
  /* Create a queue */
  
  this->queue_id = 0;

  //  std::cout << "Rank " << rank << " done" << std::endl;

   //  return (int) rank;
}


void Communication_Gaspi_t::posh__get( void* target, const void* source, size_t size, int pe ){

  Endpoint_Gaspi_t* myEndpointGaspi = static_cast<Endpoint_Gaspi_t*>(  myInfo.myEndpoint );

  gaspi_offset_t offset_remote = 0; // (char*)target - (char*)(this->segment_begin);
  gaspi_offset_t offset_local = myEndpointGaspi->getSegmentSize(); // (char*)source - (char*)(this->segment_begin);

  //   std::cout << "toto" << std::endl;
  // printf( "copy %p to %p\n", target, myEndpointGaspi->getSegmentBegin() +  myEndpointGaspi->getSegmentSize());
  gaspi_wait( myEndpointGaspi->getQueue(), GASPI_BLOCK );

  gaspi_read( myEndpointGaspi->getLocalSegmentId(), offset_local, pe, myEndpointGaspi->getSegmentId(), offset_remote, size, myEndpointGaspi->getQueue(), GASPI_BLOCK );
  gaspi_wait( myEndpointGaspi->getQueue(), GASPI_BLOCK );

  /* copy locally */

  memcpy( target, myEndpointGaspi->getSegmentBegin() + offset_local, size );

  //  gaspi_barrier( GASPI_GROUP_ALL, GASPI_BLOCK );
}

void Communication_Gaspi_t::posh__put( void* target, const void* source, size_t size, int pe ){
  Endpoint_Gaspi_t* myEndpointGaspi = static_cast<Endpoint_Gaspi_t*>(  myInfo.myEndpoint );

  gaspi_offset_t offset_remote = 0; //(char*)target - (char*)(this->segment_begin);
  gaspi_offset_t offset_local = this->segment_size; // (char*)source - (char*)(this->segment_begin);

  //  std::cout << "offset " << offset_local << " " << offset_remote << std::endl;

  /* copy locally */

  //  printf( "copy %p to %p\n", source, myEndpointGaspi->getSegmentBegin() +  myEndpointGaspi->getSegmentSize());

   memcpy( myEndpointGaspi->getSegmentBegin()+  myEndpointGaspi->getSegmentSize(), source, size );

  /* Transfer */
  
  gaspi_write( myEndpointGaspi->getLocalSegmentId(), offset_local, pe, myEndpointGaspi->getSegmentId(), offset_remote, size, myEndpointGaspi->getQueue(), GASPI_BLOCK );
  gaspi_wait( myEndpointGaspi->getQueue(), GASPI_BLOCK );

}
