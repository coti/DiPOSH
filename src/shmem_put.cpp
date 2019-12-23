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

#include "shmem_put.h"

/* These routines provide a low latency mechanism to write basic types
 * (short, int, float, double, long, long long, long double) to symmetric
 * data objects on target PEs
 * * addr must be the address of a symmetric data object.
 * * pe must be a PE number. For more information about how PE numbers are
 * assigned please refer to the Execution Model section.
 * The shmem_TYPE_p() routines write value to a symmetric array element or
 * scalar data object of the target PE indicated by the parameter pe. These
 * routines start the target transfer and may return before the data is delivered
 * to the target PE.
 */
void shmem_short_p( short *addr, short value, int pe ){
    shmem_template_p( addr, value, pe );
}
void shmem_int_p( int *addr, int value, int pe ){
    shmem_template_p( addr, value, pe );
}
void shmem_long_p( long *addr, long value, int pe ){
    shmem_template_p( addr, value, pe );
}
void shmem_float_p( float *addr, float value, int pe ){
    shmem_template_p( addr, value, pe );
}
void shmem_double_p( double *addr, double value, int pe ){
    shmem_template_p( addr, value, pe );
}
void shmem_longlong_p( long long *addr, long long value, int pe ){
    shmem_template_p( addr, value, pe );
}
void shmem_longdouble_p( long double *addr, long double value, int pe ){
    shmem_template_p( addr, value, pe );
}


/* These routines copy contiguous data from a local object to an object on the 
 * destination PE. 
 * * target must be the address of a symmetric data object.
 * * source must have the same type as target.
 * * nelems must be of type integer. If you are using Fortran, it must be a
 * constant, variable, or array element of default integer type.
 * * pe must be a PE number. For more information about how PE numbers are assigned
 * please refer to the Execution Model section.
 * These routines transfer nelems elements of the data object at address source
 * on the calling PE, to the data object at address target on the target PE pe.
 * These routines start the target transfer and return after the data has been
 * copied out of the source array on the local PE. However, this does not guarantee
 * delivery to the target PE. The delivery of data into the data object on the
 * destination PE from different put calls may occur in any order. Because of
 * this, two successive put operations may deliver data out of order unless a call
 * to shmem_fence() is introduced between the two calls.
*/

void shmem_char_put( char *target, const char *source, size_t nelems, int pe ) {
    shmem_template_put( target, source, nelems, pe );
}
void shmem_short_put( short *target, const short *source, size_t nelems, int pe ) {
    shmem_template_put( target, source, nelems, pe );
}
void shmem_int_put( int *target, const int *source, size_t nelems, int pe ) {
    shmem_template_put( target, source, nelems, pe );
}
void shmem_long_put( long *target, const long *source, size_t nelems, int pe ) {
    shmem_template_put( target, source, nelems, pe );
}
void shmem_float_put( float *target, const float *source, size_t nelems, int pe ) {
    shmem_template_put( target, source, nelems, pe );
}
void shmem_double_put( double *target, const double *source, size_t nelems, int pe ) {
    shmem_template_put( target, source, nelems, pe );
}
void shmem_longlong_put( long long *target, const long long *source, size_t nelems, int pe ) {
    shmem_template_put( target, source, nelems, pe );
}
void shmem_longdouble_put( long double *target, const long double *source, size_t nelems, int pe ) {
    shmem_template_put( target, source, nelems, pe );
}
void shmem_put32 (void *target, const void *source, size_t nelems, int pe ) {
    shmem_template_put( static_cast<char*>( target ), static_cast<const char*>( source ), nelems, pe ); 
}
void shmem_put64( void *target, const void *source, size_t nelems, int pe ) {
    shmem_template_put( static_cast<char*>( target ), static_cast<const char*>( source ), nelems, pe ); 
}
void shmem_put128( void *target, const void *source, size_t nelems, int pe ) {
    shmem_template_put( static_cast<char*>( target ), static_cast<const char*>( source ), nelems, pe ); 
}
void shmem_putmem( void *target, const void *source, size_t nelems, int pe ) {
    shmem_template_put( static_cast<char*>( target ), static_cast<const char*>( source ), nelems, pe ); 
}


/* These routines copy strided data from the local PE to a strided data object
 * on the destination PE.
 * * target must be the address of a symmetric data object.
 * * The strides tst and sst are scaled by the element size of the target and source
 * arrays respectably. A value of 1 indicates contiguous data. tst and sst must be of
 * type integer.
 * If you are using Fortran, it must be a default integer value.
 * * pe must be a PE number. For more information about how PE numbers are assigned
 * please refer to the Execution Model section.
 * The shmem_iput() routines read the elements of a local array (source) and write
 * them to a target array (target) on the PE indicated by pe. These routines return
 * when the data has been copied out of the source array on the local PE but not
 * necessarily before the data has been delivered to the target data object.
 */

void shmem_short_iput( short *target, const short *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iput( target, source, tst, sst, nelems, pe );
}
void shmem_int_iput(int *target, const int *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iput( target, source, tst, sst, nelems, pe );
}
void shmem_float_iput(float *target, const float *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){ 
    shmem_template_iput( target, source, tst, sst, nelems, pe );
}
void shmem_long_iput(long *target, const long *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iput( target, source, tst, sst, nelems, pe );
}
void shmem_double_iput(double *target, const double *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iput( target, source, tst, sst, nelems, pe );
}
void shmem_longlong_iput(long long *target, const long long *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){ 
    shmem_template_iput( target, source, tst, sst, nelems, pe );
}
void shmem_longdouble_iput(long double *target, const long double *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iput( target, source, tst, sst, nelems, pe );
}
void shmem_iput32(void *target, const void *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iput( static_cast<char*>( target ), static_cast<const char*>( source ), tst, sst, nelems, pe );
}
void shmem_iput64(void *target, const void *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iput( static_cast<char*>( target ), static_cast<const char*>( source ), tst, sst, nelems, pe );
}
void shmem_iput128(void *target, const void *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iput( static_cast<char*>( target ), static_cast<const char*>( source ), tst, sst, nelems, pe );
}
