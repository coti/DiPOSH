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

#ifndef _POSH_COLL_FLAT_H_
#define _POSH_COLL_FLAT_H_

#define TAG_BARRIER 0x1

#include "posh_coll_flat.tpp"
#include "posh_collectives.tpp"

void posh_barrier_all_flat( void );
void posh_barrier_flat( int, int, int, long * );

template <class T, class V>void shmem_broadcast_template_flat( T*, const T*, size_t, int, int, int, int, V* );
template <class T>void shmem_template_flat_reduce_get( T* target, T* source, int nelems, void(* operation)( T*, T*, T* ) );


class Collectives_flat_t : public Collectives_t {

 public:
    Collectives_flat_t(){
    }

    void posh_barrier_all( ) {
        posh_barrier_all_flat();
    }
    void posh_barrier( int PE_start, int logPE_stride, int PE_size, long *pSync ){
        posh_barrier_flat( PE_start, logPE_stride, PE_size, pSync );
    }

    /* To all routines */

    /* Collect routines */

    /* Reduction routines */

    int posh_reduce(/*shmem_team_t team,*/ unsigned char *dest, const unsigned char *source, size_t nreduce, void(* operation)( unsigned char*, unsigned char*, unsigned char* ) ) {
        shmem_template_flat_reduce_get( dest, source, nreduce, operation );
        return 0;
    }
    int posh_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce, void(* operation)( short*, short*, short* ) ) {
        shmem_template_flat_reduce_get( dest, source, nreduce, operation );
        return 0;
    }
    int posh_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce, void(* operation)( unsigned short*, unsigned short*, unsigned short* ) ) {
        shmem_template_flat_reduce_get( dest, source, nreduce, operation );
        return 0;
    }
    int posh_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce, void(* operation)( int*, int*, int* ) ) {
        shmem_template_flat_reduce_get( dest, source, nreduce, operation );
        return 0;
    }
    int posh_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce, void(* operation)( unsigned int*, unsigned int*, unsigned int* ) ) {
        shmem_template_flat_reduce_get( dest, source, nreduce, operation );
        return 0;
    }
    int posh_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce, void(* operation)( long*, long*, long* ) ) {
        shmem_template_flat_reduce_get( dest, source, nreduce, operation );
        return 0;
    }
    int posh_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce, void(* operation)( unsigned long*, unsigned long*, unsigned long* ) ) {
        shmem_template_flat_reduce_get( dest, source, nreduce, operation );
        return 0;
    }
    int posh_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce, void(* operation)( long long*, long long*, long long* ) ) {
        shmem_template_flat_reduce_get( dest, source, nreduce, operation );
        return 0;
    }
    int posh_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce, void(* operation)( unsigned long long*, unsigned long long*, unsigned long long* ) ) {
        shmem_template_flat_reduce_get( dest, source, nreduce, operation );
        return 0;
    }
     int posh_reduce(/*shmem_team_t team,*/ float *dest, const float *source, size_t nreduce, void(* operation)( float*, float*, float* ) ) {
         shmem_template_flat_reduce_get( dest, source, nreduce, operation );
        return 0;
     }
     int posh_reduce(/*shmem_team_t team,*/ double *dest, const double *source, size_t nreduce, void(* operation)( double*, double*, double* ) ) {
         shmem_template_flat_reduce_get( dest, source, nreduce, operation );
         return 0;
    }
     int posh_reduce(/*shmem_team_t team,*/ long double *dest, const long double *source, size_t nreduce, void(* operation)( long double*, long double*, long double* ) ) {
         shmem_template_flat_reduce_get( dest, source, nreduce, operation );
        return 0;
     }


    /* Broadcast routines */

    void posh_broadcast32( void* target, const void* source,  size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
        posh_broadcast_template_flat( (int32_t*) target, (const int32_t*) source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
        
    void posh_broadcast64(void* target, const void* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
        posh_broadcast_template_flat( (int64_t*) target, (const int64_t*) source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
    
    void posh_short_broadcast( short* target, const short* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, short* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
    
    void posh_int_broadcast( int* target, const int* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, int* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
    
    void posh_long_broadcast( long* target, const long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
    
    void posh_longlong_broadcast( long long* target, const long long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long long* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
    
    void posh_float_broadcast( float* target, const float* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, float* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
    
    void posh_double_broadcast( double* target, const double* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, double* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    } 

    /* Necessary because we cannot have a template member in the interface */

    void posh_broadcast( uint32_t* target, const uint32_t* source,  size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
        posh_broadcast_template_flat( (int32_t*) target, (const int32_t*) source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
        
    void posh_broadcast(uint64_t* target, const uint64_t* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
        posh_broadcast_template_flat( (int64_t*) target, (const int64_t*) source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
    
    void posh_broadcast( short* target, const short* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, short* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
    
    void posh_broadcast( int* target, const int* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, int* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
    
    void posh_broadcast( long* target, const long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
    
    void posh_broadcast( long long* target, const long long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long long* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
    
    void posh_broadcast( float* target, const float* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, float* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    }
    
    void posh_broadcast( double* target, const double* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, double* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    } 
    void posh_broadcast( long double* target, const long double* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long double* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    } 
    void posh_broadcast( unsigned long long* target, const unsigned long long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, unsigned long long* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    } 
    void posh_broadcast( unsigned char* target, const unsigned char* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, unsigned char* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    } 
    void posh_broadcast( unsigned short* target, const unsigned short* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, unsigned short* pSync ){
        posh_broadcast_template_flat( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
    } 




};


#endif //define _POSH_COLL_FLAT_H_
