#include "shmem_get.h"

/* These routines provide a low latency mechanism to retrieve basic types (short,
 * int, float, double, long) from symmetric data objects on target PEs. 
 * * addr must be the address of a symmetric data object
 * Retrieves the value at the symmetric address addr of the target PE pe.
*/

short shmem_short_g( short *addr, int pe ){
    return shmem_template_g( addr, pe );
}
int shmem_int_g( int *addr, int pe ){
    return shmem_template_g( addr, pe );
}
long shmem_long_g( long *addr, int pe ){
    return shmem_template_g( addr, pe );
}
float shmem_float_g( float *addr, int pe ){
    return shmem_template_g( addr, pe );
}
double shmem_double_g( double *addr, int pe ){
    return shmem_template_g( addr, pe );
}
long long shmem_longlong_g( long long *addr, int pe ){
    return shmem_template_g( addr, pe );
}
long double shmem_longdouble_g( long double *addr, int pe ){
    return shmem_template_g( addr, pe );
}


/* These routines retrieve data from a contiguous data object on a target PE. 
* source must be the address of a symmetric data object.
* * In C/C++ nelems must be of type integer. If you are using Fortran, it must
* be a constant, variable, or array element of default integer type.
* The shmem_get() routines transfer nelems elements of the data object at address
* source on the target PE (pe), to the data object at address target on the local
* PE. These routines return after the data has been copied to address target on the
* local pe.
* After successful completion, the retrieved data will be available at address target.
*/

void shmem_char_get( char *target, const char *source, size_t nelems, int pe ){
    shmem_template_get( target, source, nelems, pe );
}
void shmem_short_get( short *target, const short *source, size_t nelems, int pe ){
     shmem_template_get( target, source, nelems, pe );
}
void shmem_int_get( int *target, const int *source, size_t nelems, int pe ){
    shmem_template_get( target, source, nelems, pe );
}
void shmem_long_get( long *target, const long *source, size_t nelems, int pe ){
    shmem_template_get( target, source, nelems, pe );
}
 void shmem_float_get( float *target, const float *source, size_t nelems, int pe ){
    shmem_template_get( target, source, nelems, pe );
}
void shmem_double_get( double *target, const double *source, size_t nelems, int pe ){
    shmem_template_get( target, source, nelems, pe );
}
void shmem_longlong_get( long long *target, const long long *source, size_t nelems, int pe ){
    shmem_template_get( target, source, nelems, pe );
}
void shmem_longdouble_get( long double *target, const long double *source, size_t nelems, int pe ){
    shmem_template_get( target, source, nelems, pe );
}
void shmem_get32( void *target, const void *source, size_t nelems, int pe ){
    shmem_template_get( static_cast<char*>( target ), static_cast<const char*>( source ), nelems, pe ); 
}
void shmem_get64( void *target, const void *source, size_t nelems, int pe ){
    shmem_template_get( static_cast<char*>( target ), static_cast<const char*>( source ), nelems, pe ); 
}
void shmem_get128( void *target, const void *source, size_t nelems, int pe ){
    shmem_template_get( static_cast<char*>( target ), static_cast<const char*>( source ), nelems, pe ); 
}
void shmem_getmem( void *target, const void *source, size_t nelems, int pe ){
    shmem_template_get( static_cast<char*>( target ), static_cast<const char*>( source ), nelems, pe ); 
}

void shmem_intptr_get( intptr_t *target, const intptr_t *source, size_t nelems, int pe ){
    shmem_template_get( target, source, nelems, pe ); 
}

void shmem_uintptr_get( uintptr_t *target, const uintptr_t *source, size_t nelems, int pe ){
    shmem_template_get( target, source, nelems, pe ); 
}



/* The strided get routines copy strided data located on a target PE to a local
 * strided data object. 
 * * source must be the address of a symmetric data object.
 * * source must have the same type as target.
 * * In C/C++ tst and sst must be of type integer. If you are using Fortran, they
 * must be a default integer value.
 * * The strides tst and sst are scaled by the element size of the target and
 * source arrays respectably. A value of 1 indicates contiguous data. tst and sst
 * must be of type integer. If you are using Fortran, it must be a default integer
 * value.
 * * In C/C++ nelems must be of type integer. If you are using Fortran, it must be
 * a constant, variable, or array element of default integer type.
 * * Depending on the routine being called, source and target must conform to the 
 * following typing constraints:
 * - In shmem_iget32() and shmem_iget4() they can only have a non character type
 * that has a storage size equal to 32 bits.
 * - In shmem_iget64() and shmem_iget8() they can only have a non character type
 * that has a storage size equal to 64 bits.
 * - In shmem_iget128() they can only have a non character type that has a storage
 * size equal to 128 bits.
 * In shmem_short_iget() they must be of type short. 
 * - In shmem_int_iget() they must be of type int.
 * - In shmem_float_iget() they must be of type float.
 * - In shmem_double_iget() they must be of type double.
 * - In shmem_long_iget() they must be of type long.
 * - In shmem_longlong_iget() they must be of type long long.
 * - In shmem_longdouble_iget() they must be of type long double.
 * - In SHMEM_COMPLEX_IGET() they must be of type complex of default size.
 * - In SHMEM_DOUBLE_IGET() they must be of type double precision.
 * - In SHMEM_INTEGER_IGET() they must be of type integer.
 * - In SHMEM_LOGICAL_IGET() they must be of type logical.
 * - In SHMEM_REAL_IGET() they must be of type real.
 * The strided get routines retrieve array data available at address source
 * on target PE (pe). The elements of the source array are separated by a stride
 * sst. Once the data is received, it is stored at the local memory address target,
 * separated by stride tst. The routines return when the data has been copied into
 * the local target array.
 * Return Values Upon return of this routine, the data object at address target 
 * will contain the data retrieved from the target memory address source.
 */


void shmem_iget32( void *target, const void *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iget( static_cast<char*>( target ), static_cast<const char*>( source ), tst, sst, nelems, pe );
}
void shmem_iget64( void *target, const void *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iget( static_cast<char*>( target ), static_cast<const char*>( source ), tst, sst, nelems, pe );
}
void shmem_iget128( void *target, const void *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iget( static_cast<char*>( target ), static_cast<const char*>( source ), tst, sst, nelems, pe );
}
void shmem_short_iget( short *target, const short *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iget( target, source, tst, sst, nelems, pe );
}
void shmem_int_iget( int *target, const int *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iget( target, source, tst, sst, nelems, pe );
}
void shmem_long_iget( long *target, const long *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iget( target, source, tst, sst, nelems, pe );
}
void shmem_double_iget( double *target, const double *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iget( target, source, tst, sst, nelems, pe );
}
void shmem_float_iget( float *target, const float *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iget( target, source, tst, sst, nelems, pe );
}
void shmem_longlong_iget( long long *target, const long long *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iget( target, source, tst, sst, nelems, pe );
    }
void shmem_longdouble_iget( long double *target, const long double *source, ptrdiff_t tst, ptrdiff_t sst, size_t nelems, int pe ){
    shmem_template_iget( target, source, tst, sst, nelems, pe );
}

