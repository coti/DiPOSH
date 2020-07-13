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

#ifndef _POSH_COLLECTIVES_H_
#define _POSH_COLLECTIVES_H_

class Collectives_t {

 public:

    /* Barrier routines: 9.9.1 and 9.9.2 */
    
    virtual void posh_barrier_all( void ) = 0;
    virtual void posh_barrier( int, int, int, long* ) = 0;

    /* To all routines */
    /* TODO: 9.9.3 to 9.9.6 */

    /* Collect routines: 9.9.8 */

    /* Reduction routines: 9.9.9 */

    virtual  int posh_reduce(/*shmem_team_t team,*/ unsigned char *dest, const unsigned char *source, size_t nreduce, void(* operation)( unsigned char*, unsigned char*, unsigned char* ) ) = 0;
   virtual  int posh_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce, void(* operation)( short*, short*, short* ) ) = 0;
    virtual int posh_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce, void(* operation)( unsigned short*, unsigned short*, unsigned short* ) ) = 0;
    virtual int posh_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce, void(* operation)( int*, int*, int* ) ) = 0;
    virtual int posh_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce, void(* operation)( unsigned int*, unsigned int*, unsigned int* ) ) = 0;
    virtual int posh_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce, void(* operation)( long*, long*, long* ) ) = 0;
    virtual int posh_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce, void(* operation)( unsigned long*, unsigned long*, unsigned long* ) ) = 0;
    virtual int posh_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce, void(* operation)( long long*, long long*, long long* ) ) = 0;
    virtual int posh_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce, void(* operation)( unsigned long long*, unsigned long long*, unsigned long long* ) ) = 0;
    virtual  int posh_reduce(/*shmem_team_t team,*/ float *dest, const float *source, size_t nreduce, void(* operation)( float*, float*, float* ) ) = 0;
    virtual  int posh_reduce(/*shmem_team_t team,*/ double *dest, const double *source, size_t nreduce, void(* operation)( double*, double*, double* ) ) = 0;
    virtual  int posh_reduce(/*shmem_team_t team,*/ long double *dest, const long double *source, size_t nreduce, void(* operation)( long double*, long double*, long double* ) ) = 0;
    
    /* Broadcast routines: 9.9.7 */

    virtual void posh_broadcast32( void*, const void*, size_t, int, int, int, int, long* ) = 0;
    virtual void posh_broadcast64(void*, const void*, size_t, int, int, int, int, long* ) = 0;
    virtual void posh_short_broadcast( short*, const short*, size_t, int, int, int, int, short* ) = 0; 
    virtual void posh_int_broadcast( int*, const int*, size_t, int, int, int, int, int* ) = 0; 
    virtual void posh_long_broadcast( long*, const long*, size_t, int, int, int, int, long* ) = 0; 
    virtual void posh_longlong_broadcast( long long*, const long long*, size_t, int, int, int, int, long long* ) = 0; 
    virtual void posh_float_broadcast( float*, const float*, size_t, int, int, int, int, float* ) = 0; 
    virtual void posh_double_broadcast( double*, const double*, size_t, int, int, int, int, double* ) = 0; 

    //virtual void posh_broadcast_template( T*, const T*, size_t, int, int, int, int, V* ) = 0;
    /* Necessary because we cannot have a virtual template member in the interface */

    virtual void posh_broadcast( uint32_t*, const uint32_t*, size_t, int, int, int, int, long* ) = 0;
    virtual void posh_broadcast( uint64_t*, const uint64_t*, size_t, int, int, int, int, long* ) = 0;
    virtual void posh_broadcast( short*, const short*, size_t, int, int, int, int, short* ) = 0; 
    virtual void posh_broadcast( int*, const int*, size_t, int, int, int, int, int* ) = 0; 
    virtual void posh_broadcast( long*, const long*, size_t, int, int, int, int, long* ) = 0; 
    virtual void posh_broadcast( long long*, const long long*, size_t, int, int, int, int, long long* ) = 0; 
    virtual void posh_broadcast( float*, const float*, size_t, int, int, int, int, float* ) = 0; 
    virtual void posh_broadcast( double*, const double*, size_t, int, int, int, int, double* ) = 0; 
    virtual void posh_broadcast( long double*, const long double*, size_t, int, int, int, int, long double* ) = 0;
    
    virtual void posh_broadcast( unsigned long long*, const unsigned long long*, size_t, int, int, int, int, unsigned long long* ) = 0; 
    virtual void posh_broadcast( unsigned char*, const unsigned char*, size_t, int, int, int, int, unsigned char* ) = 0; 
    virtual void posh_broadcast( unsigned short*, const unsigned short*, size_t, int, int, int, int, unsigned short* ) = 0; 

    
    virtual ~Collectives_t() = default;
};




void _shmem_operation_and_unsigned_char( unsigned char*, unsigned char* , unsigned char*  );
void _shmem_operation_or_unsigned_char( unsigned char*, unsigned char* , unsigned char*  );
void _shmem_operation_xor_unsigned_char( unsigned char*, unsigned char* , unsigned char*  );
void _shmem_operation_max_unsigned_char( unsigned char*, unsigned char* , unsigned char*  );
void _shmem_operation_min_unsigned_char( unsigned char*, unsigned char* , unsigned char*  );
void _shmem_operation_sum_unsigned_char( unsigned char*, unsigned char*, unsigned char* );
void _shmem_operation_prod_unsigned_char( unsigned char*, unsigned char*, unsigned char* );
void _shmem_operation_and_short( short*, short* , short*  );
void _shmem_operation_or_short( short*, short* , short*  );
void _shmem_operation_xor_short( short*, short* , short*  );
void _shmem_operation_max_short( short*, short* , short*  );
void _shmem_operation_min_short( short*, short* , short*  );
void _shmem_operation_sum_short( short*, short*, short* );
void _shmem_operation_prod_short( short*, short*, short* );
void _shmem_operation_and_unsigned_short( unsigned short*, unsigned short* , unsigned short*  );
void _shmem_operation_or_unsigned_short( unsigned short*, unsigned short* , unsigned short*  );
void _shmem_operation_xor_unsigned_short( unsigned short*, unsigned short* , unsigned short*  );
void _shmem_operation_max_unsigned_short( unsigned short*, unsigned short* , unsigned short*  );
void _shmem_operation_min_unsigned_short( unsigned short*, unsigned short* , unsigned short*  );
void _shmem_operation_sum_unsigned_short( unsigned short*, unsigned short*, unsigned short* );
void _shmem_operation_prod_unsigned_short( unsigned short*, unsigned short*, unsigned short* );
void _shmem_operation_and_int( int*, int* , int*  );
void _shmem_operation_or_int( int*, int* , int*  );
void _shmem_operation_xor_int( int*, int* , int*  );
void _shmem_operation_max_int( int*, int* , int*  );
void _shmem_operation_min_int( int*, int* , int*  );
void _shmem_operation_sum_int( int*, int*, int* );
void _shmem_operation_prod_int( int*, int*, int* );
void _shmem_operation_and_unsigned_int( unsigned int*, unsigned int* , unsigned int*  );
void _shmem_operation_or_unsigned_int( unsigned int*, unsigned int* , unsigned int*  );
void _shmem_operation_xor_unsigned_int( unsigned int*, unsigned int* , unsigned int*  );
void _shmem_operation_max_unsigned_int( unsigned int*, unsigned int* , unsigned int*  );
void _shmem_operation_min_unsigned_int( unsigned int*, unsigned int* , unsigned int*  );
void _shmem_operation_sum_unsigned_int( unsigned int*, unsigned int*, unsigned int* );
void _shmem_operation_prod_unsigned_int( unsigned int*, unsigned int*, unsigned int* );
void _shmem_operation_and_long( long*, long* , long*  );
void _shmem_operation_or_long( long*, long* , long*  );
void _shmem_operation_xor_long( long*, long* , long*  );
void _shmem_operation_max_long( long*, long* , long*  );
void _shmem_operation_min_long( long*, long* , long*  );
void _shmem_operation_sum_long( long*, long*, long* );
void _shmem_operation_prod_long( long*, long*, long* );
void _shmem_operation_and_unsigned_long( unsigned long*, unsigned long* , unsigned long*  );
void _shmem_operation_or_unsigned_long( unsigned long*, unsigned long* , unsigned long*  );
void _shmem_operation_xor_unsigned_long( unsigned long*, unsigned long* , unsigned long*  );
void _shmem_operation_max_unsigned_long( unsigned long*, unsigned long* , unsigned long*  );
void _shmem_operation_min_unsigned_long( unsigned long*, unsigned long* , unsigned long*  );
void _shmem_operation_sum_unsigned_long( unsigned long*, unsigned long*, unsigned long* );
void _shmem_operation_prod_unsigned_long( unsigned long*, unsigned long*, unsigned long* );
void _shmem_operation_and_long_long( long long*, long long* , long long*  );
void _shmem_operation_or_long_long( long long*, long long* , long long*  );
void _shmem_operation_xor_long_long( long long*, long long* , long long*  );
void _shmem_operation_max_long_long( long long*, long long* , long long*  );
void _shmem_operation_min_long_long( long long*, long long* , long long*  );
void _shmem_operation_sum_long_long( long long*, long long*, long long* );
void _shmem_operation_prod_long_long( long long*, long long*, long long* );
void _shmem_operation_and_unsigned_long_long( unsigned long long*, unsigned long long* , unsigned long long*  );
void _shmem_operation_or_unsigned_long_long( unsigned long long*, unsigned long long* , unsigned long long*  );
void _shmem_operation_xor_unsigned_long_long( unsigned long long*, unsigned long long* , unsigned long long*  );
void _shmem_operation_max_unsigned_long_long( unsigned long long*, unsigned long long* , unsigned long long*  );
void _shmem_operation_min_unsigned_long_long( unsigned long long*, unsigned long long* , unsigned long long*  );
void _shmem_operation_sum_unsigned_long_long( unsigned long long*, unsigned long long*, unsigned long long* );
void _shmem_operation_prod_unsigned_long_long( unsigned long long*, unsigned long long*, unsigned long long* );
void _shmem_operation_and_float( float*, float* , float*  );
void _shmem_operation_or_float( float*, float* , float*  );
void _shmem_operation_xor_float( float*, float* , float*  );
void _shmem_operation_max_float( float*, float* , float*  );
void _shmem_operation_min_float( float*, float* , float*  );
void _shmem_operation_sum_float( float*, float*, float* );
void _shmem_operation_prod_float( float*, float*, float* );
void _shmem_operation_and_double( double*, double* , double*  );
void _shmem_operation_or_double( double*, double* , double*  );
void _shmem_operation_xor_double( double*, double* , double*  );
void _shmem_operation_max_double( double*, double* , double*  );
void _shmem_operation_min_double( double*, double* , double*  );
void _shmem_operation_sum_double( double*, double*, double* );
void _shmem_operation_prod_double( double*, double*, double* );
void _shmem_operation_and_long_double( long double*, long double* , long double*  );
void _shmem_operation_or_long_double( long double*, long double* , long double*  );
void _shmem_operation_xor_long_double( long double*, long double* , long double*  );
void _shmem_operation_max_long_double( long double*, long double* , long double*  );
void _shmem_operation_min_long_double( long double*, long double* , long double*  );
void _shmem_operation_sum_long_double( long double*, long double*, long double* );
void _shmem_operation_prod_long_double( long double*, long double*, long double* );

#endif //define _POSH_COLLECTIVES_H_
