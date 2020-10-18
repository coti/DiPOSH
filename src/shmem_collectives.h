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

#ifndef _SHMEM_COLLECTIVES_H
#define _SHMEM_COLLECTIVES_H

template<class T>void shmem_template_allgather_flat( T*, const T*, size_t );
template<class T>void shmem_template_gather_flat( T*, const T*, size_t, int );

void shmem_int_allgather_flat( int*, const int*, size_t );
void shmem_int_gather_flat( int*, const int*, size_t, int );

bool _shmem_is_remote_process_in_coll( int rank );
void _shmem_wait_until_entered( int rank ); 
bool _shmem_part_of_coll( int rank, int PE_start, int PE_size, int logPE_stride );
int _shmem_binomial_binbase_size( int nb ) ;
int _shmem_binomial_myfather( int PE_root, int PE_start, int logPE_stride, int PE_size ) ;
int _shmem_binomial_children( int rank, int size );

typedef enum { COLL_MPI, COLL_GASPI, COLL_NULL, COLL_UNKNOWN } collective_types;
static std::map<std::string, collective_types> collective_typenames;

inline unsigned int getCollType( char* coll_type ){
    if( NULL == coll_type ) return COLL_NULL;
    if( collective_typenames.find( coll_type ) == collective_typenames.end() ) return COLL_UNKNOWN;
    return collective_typenames[ std::string( coll_type ) ];
}



#include "shmem_collectives.tpp"

#endif
