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

#ifndef _SHMEM_H
#define _SHMEM_H

//#include "shmem_internal.h"
#include "shmem_constants.h"

#include <cstring>
#include <cstddef>

/*** http://bongo.cs.uh.edu/site/sites/default/site_files/openshmem_specification-1.0.pdf ***/

/************************************/
/*** 8.1. Initialization Routines ***/
/************************************/

/* 8.1.1. Initializes OpenSHMEM. */

void start_pes( int ); // OK
int shmem_finalize( void ); // ADDED

/***************************/
/*** 8.2. Query Routines ***/
/***************************/

/* 8.2.1. Returns the number of processing elements (PEs) used to run the application. */

int _num_pes( void ); // OK
int shmem_n_pes( void ); // OK

/* 8.2.2 Returns the PE number of the calling PE. */

int _my_pe( void ); // OK
int shmem_my_pe( void ); // OK

/*****************************************/
/*** 8.3. Accessibility Query Routines ***/
/*****************************************/

/* 8.3.1. This routine determines if a target PE is reachable from the calling PE. */

int shmem_pe_accessible( int ); // ok

/* 8.3.2. This routine indicates if an address is accessible via OpenSHMEM operations from the specified target PE. */

int shmem_addr_accessible( void*, int ); // ecrit pas teste

/************************************/
/*** 8.4. Symmetric Heap Routines ***/
/************************************/

/* 8.4.1. This routine allocates a block of memory in the symmetric heap of the calling PE. */

void *shmalloc( size_t ); // OK

/* 8.4.2. This routine allocates a block from the symmetric heap with a byte alignment specified by the programmer. */

void *shmemalign( size_t, size_t ); // ecrit pas teste

/* 8.4.3. This routine expands or reduces the size of the block to which ptr points, depending on the provided size parameter. */

void *shrealloc( void*, size_t ); // bouchon

/* 8.4.4. Frees a memory block previously allocated in the symmetric heap. */

void shfree( void* );  // OK

/**************************************/
/*** 8.5. Remote Pointer Operations ***/
/**************************************/

/* 8.5.1. Returns a pointer to a data object of a target PE. */

void *shmem_ptr( void *, int ); // OK

/***********************************/
/*** 8.6. Elemental Put Routines ***/
/***********************************/

/* 8.6.1 These routines provide a low latency mechanism to write basic types (short, int, float, double, long, long long, long double) to symmetric data objects on target PEs */

void shmem_short_p(short*, short, int ); // a l'air OK
void shmem_int_p(int*, int, int ); // a l'air OK
void shmem_long_p(long*, long, int ); // a l'air OK
void shmem_float_p(float*, float, int ); // a l'air OK
void shmem_double_p(double*, double, int ); // a l'air OK
void shmem_longlong_p(long long*, long long, int ); // a l'air OK
void shmem_longdouble_p(long double*, long double, int ); // a l'air OK

/************************************/
/*** 8.7. Block Data Put Routines ***/
/************************************/

/* 8.7.1. These routines copy contiguous data from a local object to an object on the destination PE. */

void shmem_char_put( char*, const char*, size_t, int );  // OK
void shmem_short_put( short*, const short*, size_t, int ); // OK
void shmem_int_put( int*, const int*, size_t, int);  // OK
void shmem_long_put( long*, const long*, size_t, int ); // OK
void shmem_float_put( float*, const float*, size_t, int ); // OK
void shmem_double_put( double*, const double*, size_t, int ); // OK
void shmem_longlong_put( long long*, const long long*, size_t, int ); // OK
void shmem_longdouble_put( long double*, const long double*, size_t, int );  // OK
void shmem_put32( void*, const void*, size_t, int ); // OK
void shmem_put64( void*, const void*, size_t, int );  // OK
void shmem_put128( void*, const void*, size_t, int );  // OK
void shmem_putmem( void*, const void*, size_t, int );  // OK

/*********************************/
/*** 8.8. Strided Put Routines ***/
/*********************************/

/* 8.8.1. These routines copy strided data from the local PE to a strided data object on the destination PE. */

void shmem_short_iput( short*, const short*, ptrdiff_t, ptrdiff_t, size_t, int );  // KO
void shmem_int_iput( int*, const int*, ptrdiff_t, ptrdiff_t, size_t, int );  // KO
void shmem_float_iput( float*, const float*, ptrdiff_t, ptrdiff_t, size_t, int );   // KO
void shmem_long_iput( long*, const long*, ptrdiff_t, ptrdiff_t, size_t, int );   // KO
void shmem_double_iput( double*, const double*, ptrdiff_t, ptrdiff_t, size_t, int );  // KO
void shmem_longlong_iput( long long*, const long long*, ptrdiff_t, ptrdiff_t, size_t, int );  // KO
void shmem_longdouble_iput( long double*, const long double*, ptrdiff_t, ptrdiff_t, size_t, int );   // KO
void shmem_iput32( void*, const void*, ptrdiff_t,ptrdiff_t, size_t, int );  // KO
void shmem_iput64( void*, const void*, ptrdiff_t, ptrdiff_t, size_t, int );   // KO
void shmem_iput128( void*, const void*, ptrdiff_t, ptrdiff_t, size_t, int );   // KO

/****************************************/
/*** 8.9. Elemental Data Get Routines ***/
/****************************************/

/* 8.9.1. These routines provide a low latency mechanism to retrieve basic types (short, int, float, double, long) from symmetric data objects on target PEs. */

short shmem_short_g(short*, int );  // OK
int shmem_int_g(int*, int );  // OK
long shmem_long_g(long*, int );  // OK
float shmem_float_g(float*, int );  // OK
double shmem_double_g(double*, int );  // OK
long long shmem_longlong_g(long long*, int );  // OK
long double shmem_longdouble_g(long double*, int );  // OK

/*************************************/
/*** 8.10. Block Data Get Routines ***/
/*************************************/

/* 8.10.1. These routines retrieve data from a contiguous data object on a target PE. */

void shmem_char_get( char*, const char*, size_t, int );  // OK
void shmem_short_get( short*, const short*, size_t, int );  // OK
void shmem_int_get( int*, const int*, size_t, int );  // OK
void shmem_long_get( long*, const long*, size_t, int );  // OK
void shmem_float_get( float*, const float*, size_t, int );  // OK
void shmem_double_get( double*, const double*, size_t, int );  // OK
void shmem_longlong_get( long long*, const long long*, size_t, int );  // OK
void shmem_longdouble_get( long double*, const long double*, size_t, int );  // OK
void shmem_get32( void*, const void*, size_t, int );  // OK
void shmem_get64( void*, const void*, size_t, int );  // OK
void shmem_get128( void*, const void*, size_t, int );  // OK
void shmem_getmem( void*, const void*, size_t, int );  // OK

/**********************************/
/*** 8.11. Strided Get Routines ***/
/**********************************/

/* 8.11.1. The strided get routines copy strided data located on a target PE to a local strided data object. */

void shmem_iget32( void*, const void*, ptrdiff_t, ptrdiff_t, size_t, int );
void shmem_iget64( void*, const void*, ptrdiff_t, ptrdiff_t, size_t, int );
void shmem_iget128( void*, const void*, ptrdiff_t, ptrdiff_t, size_t, int );
void shmem_short_iget( short*, const short*, ptrdiff_t, ptrdiff_t, size_t, int );
void shmem_int_iget( int*, const int*, ptrdiff_t, ptrdiff_t, size_t, int );
void shmem_long_iget( long*, const long*, ptrdiff_t, ptrdiff_t, size_t, int );
void shmem_double_iget( double*, const double*, ptrdiff_t, ptrdiff_t, size_t, int );
void shmem_float_iget( float*, const float*, ptrdiff_t, ptrdiff_t, size_t, int );
void shmem_longlong_iget( long long*, const long long*, ptrdiff_t, ptrdiff_t, size_t, int );
void shmem_longdouble_iget( long double*, const long double*, ptrdiff_t, ptrdiff_t, size_t, int );

/******************************************************/
/*** 8.12. Atomic Memory fetch-and-operate Routines ***/
/******************************************************/

/* 8.12.1. Performs an atomic swap operation. */

short shmem_short_swap( short*, short, int ); /* This one is not in the standard */  // OK
int shmem_int_swap( int*, int, int );  // OK
long shmem_long_swap( long*, long, int );  // OK
long shmem_swap( long*, long, int );  // OK
long long shmem_longlong_swap( long long*, long long, int );  // OK
float shmem_float_swap( float*, float, int );  // OK
double shmem_double_swap( double*, double, int );  // OK

/* 8.12.2. Update a target data object on an arbitrary processing element (PE) and return the prior contents of the data object in one atomic oration. */

short shmem_short_cswap( short*, short, short, int ); /* This one is not in the standard */   // OK
int shmem_int_cswap( int*, int, int, int );  // OK
long shmem_long_cswap( long*, long, long, int );  // OK
long long shmem_longlong_cswap( long long*, long long, long long, int );  // OK

/* 8.12.3. These routines rform an atomic fetch-and-add oration. */

short shmem_short_fadd( short*, short, int ); /* This one is not in the standard */  // OK
int shmem_int_fadd( int*, int, int );  // OK
long shmem_long_fadd( long*, long, int );  // OK
long long shmem_longlong_fadd( long long*, long long, int );  // OK

/* 8.12.4. These routines rform a fetch-and-increment oration. */

short shmem_short_finc( short*, int ); /* This one is not in the standard */  // OK
int shmem_int_finc( int*, int );  // OK
long shmem_long_finc( long*, int );  // OK
long long shmem_longlong_finc( long long*, int );  // OK

/**********************************************/
/*** 8.13. Atomic Memory Operation Routines ***/
/**********************************************/

/* 8.13.1. These routines perform an atomic add operation. */

void shmem_short_add( short*, short, int ); /* This one is not in the standard */  // OK
void shmem_int_add( int*, int, int );  // OK
void shmem_long_add( long*, long, int );  // OK
void shmem_longlong_add( long long*, long long, int );  // OK

/* 8.13.2. These routines perform an atomic increment operation on a target data object. */

void shmem_short_inc( short*, int ); /* This one is not in the standard */  // OK
void shmem_int_inc( int*, int );  // OK
void shmem_long_inc( long*, int );  // OK
void shmem_longlong_inc( long long*, int );  // OK

/*****************************************************/
/*** 8.14. Point-to-Point Synchronization Routines ***/
/*****************************************************/

/* 8.14.1. These routines force the calling PE to wait until var is no longer equal to value. */

void shmem_short_wait( short*, short ); /* Fait avec de l'attente active en attendant mieux... */
void shmem_int_wait( int*, int );
void shmem_long_wait( long*, long );
void shmem_longlong_wait( long long*, long long );
void shmem_wait( long*, long );

/* 8.14.2. These routines force the calling PE to wait until the condition indicated by cond and  is satisfied. */

void shmem_short_wait_until( short*, int, short ); /* Fait avec de l'attente active en attendant mieux... */
void shmem_int_wait_until( int*, int, int );
void shmem_long_wait_until( long*, int, long );
void shmem_longlong_wait_until( long long*, int, long long );
void shmem_wait_until( long*, int, long );

/**********************************************/
/*** 8.15. Barrier Synchronization Routines ***/
/**********************************************/

/* 8.15.1. Suspends the execution of the calling PE until all other PEs issue a call to this particular shmem_barrier_all() statement. */

void shmem_barrier_all( void ); // ok

/* 8.15.2. Performs a barrier operation on a subset of processing elements (PEs). */

void shmem_barrier( int, int, int, long* );

/* 8.15.3. Provides a separate ordering on the sequence of puts issued by this PE to each destination PE. */

void shmem_fence( void );

/* 8.15.4. Provides an ordering on the sequence of puts issued by this PE across all destination PEs. */

void shmem_quiet(void);


/********************************/
/*** 8.16. Reduction Routines ***/
/********************************/

void shmem_short_and_to_all( short*, short*, int, int, int, int, short*, long* );
void shmem_int_and_to_all( int*, int*, int, int, int, int, int*, long* );
void shmem_long_and_to_all( long*, long*, int, int, int, int, long*, long* );
void shmem_longlong_and_to_all( long long*, long long*, int, int, int, int, long long*, long* );

void shmem_short_or_to_all( short*, short*, int, int, int, int, short*, long* );
void shmem_int_or_to_all( int*, int*, int, int, int, int, int*, long* );
void shmem_long_or_to_all( long*, long*, int, int, int, int, long*, long* );
void shmem_longlong_or_to_all( long long*, long long*, int, int, int, int, long long*, long* );

void shmem_short_xor_to_all( short*, short*, int, int, int, int, short*, long* );
void shmem_int_xor_to_all( int*, int*, int, int, int, int, int*, long* );
void shmem_long_xor_to_all( long*, long*, int, int, int, int, long*, long* );
void shmem_longlong_xor_to_all( long long*, long long*, int, int, int, int, long long*, long* );

void shmem_short_max_to_all( short*, short*, int, int, int, int, short*, long* );
void shmem_int_max_to_all( int*, int*, int, int, int, int, int*, long* );
void shmem_long_max_to_all( long*, long*, int, int, int, int, long*, long* );
void shmem_float_max_to_all( float*, float*, int, int, int, int, float*, long* );
void shmem_double_max_to_all( double*, double*, int, int, int, int, double*, long* );
void shmem_longlong_max_to_all( long long*, long long*, int, int, int, int, long long*, long* );
void shmem_longdouble_max_to_all( long double*, long double*, int, int, int, int, long double*, long* );

void shmem_short_min_to_all( short*, short*, int, int, int, int, short*, long* );
void shmem_int_min_to_all( int*, int*, int, int, int, int, int*, long* );
void shmem_long_min_to_all( long*, long*, int, int, int, int, long*, long* );
void shmem_float_min_to_all( float*, float*, int, int, int, int, float*, long* );
void shmem_double_min_to_all( double*, double*, int, int, int, int, double*, long* );
void shmem_longlong_min_to_all( long long*, long long*, int, int, int, int, long long*, long* );
void shmem_longdouble_min_to_all( long double*, long double*, int, int, int, int, long double*, long* );

void shmem_short_sum_to_all( short*, short*, int, int, int, int, int*, long* );
void shmem_int_sum_to_all( int*, int*, int, int, int, int, int*, long* );
void shmem_long_sum_to_all( long*, long*, int, int, int, int, long*, long* );
void shmem_float_sum_to_all( float*, float*, int, int, int, int, float*, long* );
void shmem_double_sum_to_all( double*, double*, int, int, int, int, double*, long* );
void shmem_longlong_sum_to_all( long long*, long long*, int, int, int, int, long long*, long* );
void shmem_longdouble_sum_to_all( long double*, long double*, int, int, int, int, long double*, long* );
//void shmem_complexf_sum_to_all( float complex*, float complex*, int, int, int, int, float complex*, long* );
//void shmem_complexd_sum_to_all( double complex*, double complex*, int, int, int, int, double complex*, long* );

void shmem_short_prod_to_all( short*, short*, int, int, int, int, int*, long* );
void shmem_int_prod_to_all( int*, int*, int, int, int, int, int*, long* );
void shmem_long_prod_to_all( long*, long*, int, int, int, int, long*, long* );
void shmem_float_prod_to_all( float*, float*, int, int, int, int, float*, long* );
void shmem_double_prod_to_all( double*, double*, int, int, int, int, double*, long* ); 
void shmem_longlong_prod_to_all( long long*, long long*, int, int, int, int, long long*, long* );
void shmem_longdouble_prod_to_all( long double*, long double*, int, int, int, int, long double*, long* ); 
//void shmem_complexf_prod_to_all( float complex*, float complex*, int, int, int, int, float complex*, long* );
//void shmem_complexd_prod_to_all( double complex*, double complex*, int, int, int, int, double complex*, long* );

/******************************/
/*** 8.17. Collect Routines ***/
/******************************/

void shmem_collect32( void*, const void*, size_t, int, int, int, long* ); // ok
void shmem_collect64( void*, const void*, size_t, int, int, int, long* );
/* The following ones are not part of the standard */
void shmem_short_collect( short*, const short*, size_t, int, int, int, short* );
void shmem_int_collect( int*, const int*, size_t, int, int, int, int* );
void shmem_long_collect( long*, const long*, size_t, int, int, int, long* );
void shmem_longlong_collect( long long*, const long long*, size_t, int, int, int, long long* );
void shmem_float_collect( float*, const float*, size_t, int, int, int, float* );
void shmem_double_collect( double*, const double*, size_t, int, int, int, double* );

void shmem_fcollect32( void*, const void*, size_t, int, int, int, long* ); // diff w/ collect routines?
void shmem_fcollect64( void*, const void*, size_t, int, int, int, long* );
/* The following ones are not part of the standard */
void shmem_short_fcollect( short*, const short*, size_t, int, int, int, short* );
void shmem_int_fcollect( int*, const int*, size_t, int, int, int, int* );
void shmem_long_fcollect( long*, const long*, size_t, int, int, int, long* );
void shmem_longlong_fcollect( long long*, const long long*, size_t, int, int, int, long long* );
void shmem_float_fcollect( float*, const float*, size_t, int, int, int, float* );
void shmem_double_fcollect( double*, const double*, size_t, int, int, int, double* );

/********************************/
/*** 8.18. Broadcast Routines ***/
/********************************/

void shmem_broadcast32( void*, const void*, size_t, int, int, int, int, long* ); // ok
void shmem_broadcast64(void*, const void*, size_t, int, int, int, int, long* );
/* The following ones are not part of the standard */
void shmem_short_broadcast( short*, const short*, size_t, int, int, int, int, short* ); 
void shmem_int_broadcast( int*, const int*, size_t, int, int, int, int, int* ); 
void shmem_long_broadcast( long*, const long*, size_t, int, int, int, int, long* ); 
void shmem_longlong_broadcast( long long*, const long long*, size_t, int, int, int, int, long long* ); 
void shmem_float_broadcast( float*, const float*, size_t, int, int, int, int, float* ); 
void shmem_double_broadcast( double*, const double*, size_t, int, int, int, int, double* ); 

/***************************/
/*** 8.19. Lock Routines ***/
/***************************/

/* 8.19.1. Sets a mutual exclusion memory lock after it is no longer in use by another process. */

void shmem_set_lock( long* ); // OK

/* 8.19.2. Releases a lock previously set by the calling PE. */

void shmem_clear_lock( long* ); // OK

/* 8.19.3. Sets a mutual exclusion lock only if it is currently cleared. */

int shmem_test_lock( long* ); // OK

#endif
