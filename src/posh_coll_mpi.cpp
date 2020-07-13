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
 * You should have received a copy of the GNU General Public License
 * along with POSH.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <mpi.h>
#include <string.h> // for memcpy

#include "shmem_internal.h"
#include "posh_coll_mpi.h"

/* The arguments int PE_start, int logPE_stride, int PE_size are deprecated.
   TODO: handle teams
*/

template<class T>unsigned long nameToLong( void(* operation)( T*, T*, T* ) ) {
    auto a1 = operation;
    unsigned long b1 = (unsigned long) a1;
    return b1;
}

Collectives_MPI_t::Collectives_MPI_t(){
    
this->operations[ nameToLong( &_shmem_operation_and_unsigned_char ) ] = MPI_BAND;
this->operations[ nameToLong( &_shmem_operation_or_unsigned_char ) ] = MPI_BOR;
this->operations[ nameToLong( &_shmem_operation_xor_unsigned_char ) ] = MPI_BXOR;
this->operations[ nameToLong( &_shmem_operation_max_unsigned_char ) ] = MPI_MAX;
this->operations[ nameToLong( &_shmem_operation_min_unsigned_char ) ] = MPI_MIN;
this->operations[ nameToLong( &_shmem_operation_sum_unsigned_char ) ] = MPI_SUM;
this->operations[ nameToLong( &_shmem_operation_prod_unsigned_char ) ] = MPI_PROD;
this->operations[ nameToLong( &_shmem_operation_and_short ) ] = MPI_BAND;
this->operations[ nameToLong( &_shmem_operation_or_short ) ] = MPI_BOR;
this->operations[ nameToLong( &_shmem_operation_xor_short ) ] = MPI_BXOR;
this->operations[ nameToLong( &_shmem_operation_max_short ) ] = MPI_MAX;
this->operations[ nameToLong( &_shmem_operation_min_short ) ] = MPI_MIN;
this->operations[ nameToLong( &_shmem_operation_sum_short ) ] = MPI_SUM;
this->operations[ nameToLong( &_shmem_operation_prod_short ) ] = MPI_PROD;
this->operations[ nameToLong( &_shmem_operation_and_unsigned_short ) ] = MPI_BAND;
this->operations[ nameToLong( &_shmem_operation_or_unsigned_short ) ] = MPI_BOR;
this->operations[ nameToLong( &_shmem_operation_xor_unsigned_short ) ] = MPI_BXOR;
this->operations[ nameToLong( &_shmem_operation_max_unsigned_short ) ] = MPI_MAX;
this->operations[ nameToLong( &_shmem_operation_min_unsigned_short ) ] = MPI_MIN;
this->operations[ nameToLong( &_shmem_operation_sum_unsigned_short ) ] = MPI_SUM;
this->operations[ nameToLong( &_shmem_operation_prod_unsigned_short ) ] = MPI_PROD;
this->operations[ nameToLong( &_shmem_operation_and_int ) ] = MPI_BAND;
this->operations[ nameToLong( &_shmem_operation_or_int ) ] = MPI_BOR;
this->operations[ nameToLong( &_shmem_operation_xor_int ) ] = MPI_BXOR;
this->operations[ nameToLong( &_shmem_operation_max_int ) ] = MPI_MAX;
this->operations[ nameToLong( &_shmem_operation_min_int ) ] = MPI_MIN;
this->operations[ nameToLong( &_shmem_operation_sum_int ) ] = MPI_SUM;
this->operations[ nameToLong( &_shmem_operation_prod_int ) ] = MPI_PROD;
this->operations[ nameToLong( &_shmem_operation_and_unsigned_int ) ] = MPI_BAND;
this->operations[ nameToLong( &_shmem_operation_or_unsigned_int ) ] = MPI_BOR;
this->operations[ nameToLong( &_shmem_operation_xor_unsigned_int ) ] = MPI_BXOR;
this->operations[ nameToLong( &_shmem_operation_max_unsigned_int ) ] = MPI_MAX;
this->operations[ nameToLong( &_shmem_operation_min_unsigned_int ) ] = MPI_MIN;
this->operations[ nameToLong( &_shmem_operation_sum_unsigned_int ) ] = MPI_SUM;
this->operations[ nameToLong( &_shmem_operation_prod_unsigned_int ) ] = MPI_PROD;
this->operations[ nameToLong( &_shmem_operation_and_long ) ] = MPI_BAND;
this->operations[ nameToLong( &_shmem_operation_or_long ) ] = MPI_BOR;
this->operations[ nameToLong( &_shmem_operation_xor_long ) ] = MPI_BXOR;
this->operations[ nameToLong( &_shmem_operation_max_long ) ] = MPI_MAX;
this->operations[ nameToLong( &_shmem_operation_min_long ) ] = MPI_MIN;
this->operations[ nameToLong( &_shmem_operation_sum_long ) ] = MPI_SUM;
this->operations[ nameToLong( &_shmem_operation_prod_long ) ] = MPI_PROD;
this->operations[ nameToLong( &_shmem_operation_and_unsigned_long ) ] = MPI_BAND;
this->operations[ nameToLong( &_shmem_operation_or_unsigned_long ) ] = MPI_BOR;
this->operations[ nameToLong( &_shmem_operation_xor_unsigned_long ) ] = MPI_BXOR;
this->operations[ nameToLong( &_shmem_operation_max_unsigned_long ) ] = MPI_MAX;
this->operations[ nameToLong( &_shmem_operation_min_unsigned_long ) ] = MPI_MIN;
this->operations[ nameToLong( &_shmem_operation_sum_unsigned_long ) ] = MPI_SUM;
this->operations[ nameToLong( &_shmem_operation_prod_unsigned_long ) ] = MPI_PROD;
this->operations[ nameToLong( &_shmem_operation_and_long_long ) ] = MPI_BAND;
this->operations[ nameToLong( &_shmem_operation_or_long_long ) ] = MPI_BOR;
this->operations[ nameToLong( &_shmem_operation_xor_long_long ) ] = MPI_BXOR;
this->operations[ nameToLong( &_shmem_operation_max_long_long ) ] = MPI_MAX;
this->operations[ nameToLong( &_shmem_operation_min_long_long ) ] = MPI_MIN;
this->operations[ nameToLong( &_shmem_operation_sum_long_long ) ] = MPI_SUM;
this->operations[ nameToLong( &_shmem_operation_prod_long_long ) ] = MPI_PROD;
this->operations[ nameToLong( &_shmem_operation_and_unsigned_long_long ) ] = MPI_BAND;
this->operations[ nameToLong( &_shmem_operation_or_unsigned_long_long ) ] = MPI_BOR;
this->operations[ nameToLong( &_shmem_operation_xor_unsigned_long_long ) ] = MPI_BXOR;
this->operations[ nameToLong( &_shmem_operation_max_unsigned_long_long ) ] = MPI_MAX;
this->operations[ nameToLong( &_shmem_operation_min_unsigned_long_long ) ] = MPI_MIN;
this->operations[ nameToLong( &_shmem_operation_sum_unsigned_long_long ) ] = MPI_SUM;
this->operations[ nameToLong( &_shmem_operation_prod_unsigned_long_long ) ] = MPI_PROD;
this->operations[ nameToLong( &_shmem_operation_and_float ) ] = MPI_BAND;
this->operations[ nameToLong( &_shmem_operation_or_float ) ] = MPI_BOR;
this->operations[ nameToLong( &_shmem_operation_xor_float ) ] = MPI_BXOR;
this->operations[ nameToLong( &_shmem_operation_max_float ) ] = MPI_MAX;
this->operations[ nameToLong( &_shmem_operation_min_float ) ] = MPI_MIN;
this->operations[ nameToLong( &_shmem_operation_sum_float ) ] = MPI_SUM;
this->operations[ nameToLong( &_shmem_operation_prod_float ) ] = MPI_PROD;
this->operations[ nameToLong( &_shmem_operation_and_double ) ] = MPI_BAND;
this->operations[ nameToLong( &_shmem_operation_or_double ) ] = MPI_BOR;
this->operations[ nameToLong( &_shmem_operation_xor_double ) ] = MPI_BXOR;
this->operations[ nameToLong( &_shmem_operation_max_double ) ] = MPI_MAX;
this->operations[ nameToLong( &_shmem_operation_min_double ) ] = MPI_MIN;
this->operations[ nameToLong( &_shmem_operation_sum_double ) ] = MPI_SUM;
this->operations[ nameToLong( &_shmem_operation_prod_double ) ] = MPI_PROD;
this->operations[ nameToLong( &_shmem_operation_and_long_double ) ] = MPI_BAND;
this->operations[ nameToLong( &_shmem_operation_or_long_double ) ] = MPI_BOR;
this->operations[ nameToLong( &_shmem_operation_xor_long_double ) ] = MPI_BXOR;
this->operations[ nameToLong( &_shmem_operation_max_long_double ) ] = MPI_MAX;
this->operations[ nameToLong( &_shmem_operation_min_long_double ) ] = MPI_MIN;
this->operations[ nameToLong( &_shmem_operation_sum_long_double ) ] = MPI_SUM;
this->operations[ nameToLong( &_shmem_operation_prod_long_double ) ] = MPI_PROD;



}

void Collectives_MPI_t::posh_broadcast32( void* target, const void* source,  size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*4 );
    }
    /* Don't trust that MPI_INT will be on 32 bits (surprises happen) */
    MPI_Bcast( target, nelems*4, MPI_CHAR, PE_root, comm );
}
        
void Collectives_MPI_t::posh_broadcast64(void* target, const void* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*8 );
    }
    MPI_Bcast( target, nelems*8, MPI_CHAR, PE_root, comm );
}
    
void Collectives_MPI_t::posh_short_broadcast( short* target, const short* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, short* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( short ) );
    }
    MPI_Bcast( target, nelems, MPI_SHORT, PE_root, comm );
}
    
void Collectives_MPI_t::posh_int_broadcast( int* target, const int* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, int* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( int ) );
    }
    MPI_Bcast( target, nelems, MPI_INT, PE_root, comm );
}
    
void Collectives_MPI_t::posh_long_broadcast( long* target, const long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( long ) );
    }
    MPI_Bcast( target, nelems, MPI_LONG, PE_root, comm );
}
    
void Collectives_MPI_t::posh_longlong_broadcast( long long* target, const long long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long long* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( long long ) );
    }
    MPI_Bcast( target, nelems, MPI_LONG_LONG_INT, PE_root, comm );
}
    
void Collectives_MPI_t::posh_float_broadcast( float* target, const float* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, float* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( float ) );
    }
    MPI_Bcast( target, nelems, MPI_FLOAT, PE_root, comm );
}
    
void Collectives_MPI_t::posh_double_broadcast( double* target, const double* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, double* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( double ) );
    }
    MPI_Bcast( target, nelems, MPI_DOUBLE, PE_root, comm );
} 

void Collectives_MPI_t::posh_broadcast( uint32_t* target, const uint32_t* source,  size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*4 );
    }
    /* Don't trust that MPI_INT will be on 32 bits (surprises happen) */
    MPI_Bcast( target, nelems*4, MPI_CHAR, PE_root, comm );
}

void Collectives_MPI_t::posh_broadcast(uint64_t* target, const uint64_t* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*8 );
    }
    MPI_Bcast( target, nelems*8, MPI_CHAR, PE_root, comm );
}
    
void Collectives_MPI_t::posh_broadcast( short* target, const short* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, short* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( short ) );
    }
    MPI_Bcast( target, nelems, MPI_SHORT, PE_root, comm );
}
    
void Collectives_MPI_t::posh_broadcast( int* target, const int* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, int* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( int ) );
    }
    MPI_Bcast( target, nelems, MPI_INT, PE_root, comm );
}
    
void Collectives_MPI_t::posh_broadcast( long* target, const long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( long ) );
    }
    MPI_Bcast( target, nelems, MPI_LONG, PE_root, comm );
}
    
void Collectives_MPI_t::posh_broadcast( long long* target, const long long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long long* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( long long ) );
    }
    MPI_Bcast( target, nelems, MPI_LONG_LONG_INT, PE_root, comm );
}
    
void Collectives_MPI_t::posh_broadcast( float* target, const float* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, float* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( float ) );
    }
    MPI_Bcast( target, nelems, MPI_FLOAT, PE_root, comm );
}
    
void Collectives_MPI_t::posh_broadcast( double* target, const double* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, double* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( double ) );
    }
    MPI_Bcast( target, nelems, MPI_DOUBLE, PE_root, comm );
} 

void Collectives_MPI_t::posh_broadcast( long double* target, const long double* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long double* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( long double ) );
    }
    MPI_Bcast( target, nelems, MPI_LONG_DOUBLE, PE_root, comm );
} 

void Collectives_MPI_t::posh_broadcast( unsigned short* target, const unsigned short* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, unsigned short* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( unsigned short ) );
    }
    MPI_Bcast( target, nelems, MPI_UNSIGNED_SHORT, PE_root, comm );
}

void Collectives_MPI_t::posh_broadcast( unsigned char* target, const unsigned char* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, unsigned char* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( unsigned char ) );
    }
    MPI_Bcast( target, nelems, MPI_UNSIGNED_CHAR, PE_root, comm );
} 

void Collectives_MPI_t::posh_broadcast( unsigned long long* target, const unsigned long long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, unsigned long long* pSync ){
    MPI_Comm comm = MPI_COMM_WORLD;
    if( myInfo.getRank() == PE_root ){
        memcpy( target, source, nelems*sizeof( long long ) );
    }
    MPI_Bcast( target, nelems, MPI_UNSIGNED_LONG_LONG, PE_root, comm );
}
    

    /* Reduction routines */
    
int Collectives_MPI_t::posh_reduce(/*shmem_team_t team,*/ unsigned char *dest, const unsigned char *source, size_t nreduce, void(* operation)( unsigned char*, unsigned char*, unsigned char* ) ) {
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Op op = this->operations[nameToLong( operation )];
    MPI_Allreduce( source, dest, nreduce, MPI_UNSIGNED_CHAR, op, comm);
    return 0;
}
int Collectives_MPI_t::posh_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce, void(* operation)( short*, short*, short* ) ) {
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Op op = this->operations[nameToLong( operation )];
    MPI_Allreduce( source, dest, nreduce, MPI_SHORT, op, comm);
    return 0;
}
int Collectives_MPI_t::posh_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce, void(* operation)( unsigned short*, unsigned short*, unsigned short* ) ) {
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Op op = this->operations[nameToLong( operation )];
    MPI_Allreduce( source, dest, nreduce, MPI_UNSIGNED_SHORT, op, comm);
    return 0;
}
int Collectives_MPI_t::posh_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce, void(* operation)( int*, int*, int* ) ) {
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Op op = this->operations[nameToLong( operation )];
    MPI_Allreduce( source, dest, nreduce, MPI_INT, op, comm);
    return 0;
}
int Collectives_MPI_t::posh_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce, void(* operation)( unsigned int*, unsigned int*, unsigned int* ) ) {
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Op op = this->operations[nameToLong( operation )];
    MPI_Allreduce( source, dest, nreduce, MPI_UNSIGNED, op, comm);
    return 0;
}
int Collectives_MPI_t::posh_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce, void(* operation)( long*, long*, long* ) ) {
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Op op = this->operations[nameToLong( operation )];
    MPI_Allreduce( source, dest, nreduce, MPI_LONG, op, comm);
    return 0;
}
int Collectives_MPI_t::posh_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce, void(* operation)( unsigned long*, unsigned long*, unsigned long* ) ) {
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Op op = this->operations[nameToLong( operation )];
    MPI_Allreduce( source, dest, nreduce, MPI_UNSIGNED_LONG, op, comm);
    return 0;
}
int Collectives_MPI_t::posh_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce, void(* operation)( long long*, long long*, long long* ) ) {
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Op op = this->operations[nameToLong( operation )];
    MPI_Allreduce( source, dest, nreduce, MPI_LONG_LONG, op, comm);
    return 0;
}
int Collectives_MPI_t::posh_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce, void(* operation)( unsigned long long*, unsigned long long*, unsigned long long* ) ) {
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Op op = this->operations[nameToLong( operation )];
    MPI_Allreduce( source, dest, nreduce, MPI_UNSIGNED_LONG_LONG, op, comm);
    return 0;
}
int Collectives_MPI_t::posh_reduce(/*shmem_team_t team,*/ float *dest, const float *source, size_t nreduce, void(* operation)( float*, float*, float* ) ) {
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Op op = this->operations[nameToLong( operation )];
    MPI_Allreduce( source, dest, nreduce, MPI_FLOAT, op, comm);
    return 0;
}
int Collectives_MPI_t::posh_reduce(/*shmem_team_t team,*/ double *dest, const double *source, size_t nreduce, void(* operation)( double*, double*, double* ) ) {
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Op op = this->operations[nameToLong( operation )];
    MPI_Allreduce( source, dest, nreduce, MPI_DOUBLE, op, comm);
    return 0;
}
int Collectives_MPI_t::posh_reduce(/*shmem_team_t team,*/ long double *dest, const long double *source, size_t nreduce, void(* operation)( long double*, long double*, long double* ) ) {
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Op op = this->operations[nameToLong( operation )];
    MPI_Allreduce( source, dest, nreduce, MPI_LONG_DOUBLE, op, comm);
    return 0;
}


