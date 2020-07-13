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

#ifndef _POSH_COLL_MPI_H_
#define _POSH_COLL_MPI_H_

#include <cstdint>
#include <unordered_map>

#include "posh_collectives.h"

//typedef std::tuple <int,int,int> key_t; /* start, stride, size */
//typedef std::unordered_map<const key_t, MPI_Comm> communicators_t;

class Collectives_MPI_t : public Collectives_t {

 private:
    
    /* Communicators I create once and keep */
    
    //  communicators_t mycommunicators;
    std::unordered_map< unsigned long, MPI_Op> operations;


 public:
    Collectives_MPI_t();
    
    void posh_barrier_all( ) {
        MPI_Barrier( MPI_COMM_WORLD );
    }
    void posh_barrier( int PE_start, int logPE_stride, int PE_size, long *pSync ){
        /* TODO */
        //        posh_barrier_flat( PE_start, logPE_stride, PE_size, pSync );
    }

    /* To all routines */

    /* Collect routines */

    /* Reduction routines */
    
    int posh_reduce(/*shmem_team_t team,*/ unsigned char *dest, const unsigned char *source, size_t nreduce, void(* operation)( unsigned char*, unsigned char*, unsigned char* ) );
    int posh_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce, void(* operation)( short*, short*, short* ) );
    int posh_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce, void(* operation)( unsigned short*, unsigned short*, unsigned short* ) );
    int posh_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce, void(* operation)( int*, int*, int* ) );
    int posh_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce, void(* operation)( unsigned int*, unsigned int*, unsigned int* ) );
    int posh_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce, void(* operation)( long*, long*, long* ) );
    int posh_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce, void(* operation)( unsigned long*, unsigned long*, unsigned long* ) );
    int posh_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce, void(* operation)( long long*, long long*, long long* ) );
    int posh_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce, void(* operation)( unsigned long long*, unsigned long long*, unsigned long long* ) );
    int posh_reduce(/*shmem_team_t team,*/ float *dest, const float *source, size_t nreduce, void(* operation)( float*, float*, float* ) ) ;
     int posh_reduce(/*shmem_team_t team,*/ double *dest, const double *source, size_t nreduce, void(* operation)( double*, double*, double* ) );
     int posh_reduce(/*shmem_team_t team,*/ long double *dest, const long double *source, size_t nreduce, void(* operation)( long double*, long double*, long double* ) );

    /* Broadcast routines */

    void posh_broadcast32( void* target, const void* source,  size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync );
    void posh_broadcast64(void* target, const void* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync );
    void posh_short_broadcast( short* target, const short* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, short* pSync );
    void posh_int_broadcast( int* target, const int* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, int* pSync );
    void posh_long_broadcast( long* target, const long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync );
    void posh_longlong_broadcast( long long* target, const long long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long long* pSync );
    void posh_float_broadcast( float* target, const float* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, float* pSync );
    void posh_double_broadcast( double* target, const double* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, double* pSync );
    /* Necessary because we cannot have a virtual template member in the interface */
    void posh_broadcast( uint32_t* target, const uint32_t* source,  size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync );
    void posh_broadcast(uint64_t* target, const uint64_t* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync );
    void posh_broadcast( short* target, const short* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, short* pSync );
    void posh_broadcast( int* target, const int* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, int* pSync );
    void posh_broadcast( long* target, const long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync );
    void posh_broadcast( long long* target, const long long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long long* pSync );
    void posh_broadcast( float* target, const float* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, float* pSync );
    void posh_broadcast( double* target, const double* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, double* pSync );
    void posh_broadcast( long double* target, const long double* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long double* pSync );
    void posh_broadcast( unsigned long long* target, const unsigned long long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, unsigned long long* pSync );
    void posh_broadcast( unsigned char* target, const unsigned char* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, unsigned char* pSync );
    void posh_broadcast( unsigned short* target, const unsigned short* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, unsigned short* pSync );

    
};


#endif //define _POSH_COLL_MPI_H_
