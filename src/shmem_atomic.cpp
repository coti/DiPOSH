#include "shmem_atomic.h"


/* Performs an atomic swap operation.
 * The atomic swap routines write value to address target on PE pe, and return 
 * the previous contents of target. The operation must be completed without the
 * possibility of another process updating target between the time of the fetch
 * and the update.
 * Returns the previous value of target.
 */

short shmem_short_swap( short *target, short value, int pe ){
    return shmem_template_swap( target, value, pe );
}
int shmem_int_swap( int *target, int value, int pe ){
    return shmem_template_swap( target, value, pe );
}
long shmem_long_swap( long *target, long value, int pe ){
    return shmem_template_swap( target, value, pe );
}
long shmem_swap( long *target, long value, int pe ){
    return shmem_template_swap( target, value, pe );
}
long long shmem_longlong_swap( long long *target, long long value, int pe ){
    return shmem_template_swap( target, value, pe );
}
float shmem_float_swap( float *target, float value, int pe ){
    return shmem_template_swap( target, value, pe );
}
double shmem_double_swap( double *target, double value, int pe ){
    return shmem_template_swap( target, value, pe );
}


/* The conditional swap routines write value to address target on PE pe, and
 * return the previous contents of target. The replacement must occur only if
 * cond is equal to target; otherwise target is left unchanged. In either case,
 * the routine must return the initial value of target. The operation must be
 * completed without the possibility of another process updating target between
 * the time of the fetch and the update.
 * Return Values Returns the initial value of target.
 */

short shmem_int_cswap( short *target, short cond, short value, int pe ){
    return shmem_template_cswap( target, cond, value, pe );
}
int shmem_int_cswap( int *target, int cond, int value, int pe ){
    return shmem_template_cswap( target, cond, value, pe );
}
long shmem_long_cswap( long *target, long cond, long value, int pe ){
    return shmem_template_cswap( target, cond, value, pe );
}
long long shmem_longlong_cswap( long long *target, long long cond, long long value, int pe ){
    return shmem_template_cswap( target, cond, value, pe );
}


/* These routines perform an atomic fetch-and-add operation.
 * The fetch and add routines retrieve the value at address target on PE pe,
 * and update target with the result of adding value to the retrieved value.
 * The operation must be completed without the possibility of another process
 * updating target between the time of the fetch and the update.
 * Returns the initial value of target.
 */

short shmem_short_fadd( short *target, short value, int pe ){
    return shmem_template_fadd( target, value, pe );
}
int shmem_int_fadd( int *target, int value, int pe ){
    return shmem_template_fadd( target, value, pe );
}
long shmem_long_fadd( long *target, long value, int pe ){
    return shmem_template_fadd( target, value, pe );
}
long long shmem_longlong_fadd( long long *target, long long value, int pe ){
    return shmem_template_fadd( target, value, pe );
}


/* These routines perform a fetch-and-increment operation.
 * The fetch and increment routines retrieve the value at address target on PE pe,
 * and update target with the result of incrementing the retrieved value by one.
 * The operation must be completed without the possibility of another process
 * updating target between the time of the fetch and the update.
 * Returns the initial value of target.
 */

short shmem_short_finc( short *target, int pe ){
    return shmem_template_finc( target, pe );
}
int shmem_int_finc( int *target, int pe ){
    return shmem_template_finc( target, pe );
}
long shmem_long_finc( long *target, int pe ){
    return shmem_template_finc( target, pe );
}
long long shmem_longlong_finc( long long *target, int pe ){
    return shmem_template_finc( target, pe );
}


/* These routines perform an atomic add operation.
 * The atomic add routines add value to the data at address target on PE pe.
 * The operation must be completed without the possibility of another process
 * updating target between the time of the fetch and the update.
 */

void shmem_short_add( short *target, short value, int pe ){
    shmem_template_add( target, value, pe );
}
void shmem_int_add( int *target, int value, int pe ){
    shmem_template_add( target, value, pe );
}
void shmem_long_add( long *target, long value, int pe ){
    shmem_template_add( target, value, pe );
}
void shmem_longlong_add( long long *target, long long value, int pe ){
    shmem_template_add( target, value, pe );
}


/* These routines perform an atomic increment operation on a target data object. 
 * The atomic increment routines replace the value of target with its value incremented
* by one. The operation must be completed without the possibility of another process
* updating target between the time of the fetch and the update.
*/

void shmem_short_inc( short *target, int pe ){
    shmem_template_inc( target, pe );
}
void shmem_int_inc( int *target, int pe ){
    shmem_template_inc( target, pe );
}
void shmem_long_inc( long *target, int pe ){
    shmem_template_inc( target, pe );
}
void shmem_longlong_inc( long long *target, int pe ){
    shmem_template_inc( target, pe );
}
