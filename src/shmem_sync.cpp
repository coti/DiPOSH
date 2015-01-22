#include "shmem_sync.h"

/* These routines force the calling PE to wait until var is no longer equal to value. 
 * A call to any shmem_wait() routine does not return until some other processor makes
 * the value at address var not equal to value.
 */

void shmem_short_wait( short *var, short value ){
    shmem_template_wait( var, value );
}
void shmem_int_wait( int *var, int value ){
    shmem_template_wait( var, value );
}
void shmem_long_wait( long *var, long value ){
    shmem_template_wait( var, value );
}
void shmem_longlong_wait( long long *var, long long value ){
    shmem_template_wait( var, value );
}
void shmem_wait( long *ivar, long value ){
    shmem_template_wait( ivar, value );
}

/* These routines force the calling PE to wait until the condition indicated by cond
 * and value is satisfied. 
 * A call to any shmem_wait_until() routine does not return until some other processor
 * changes the value at address var to satisfy the condition implied by cmp and value.
 */

void shmem_short_wait_until( short *var, int cmp, short value ){
    shmem_template_wait_until( var, cmp, value );
}
void shmem_int_wait_until( int *var, int cmp, int value ){
    shmem_template_wait_until( var, cmp, value );
}
void shmem_long_wait_until( long *var, int cmp, long value ){
    shmem_template_wait_until( var, cmp, value );
}
void shmem_longlong_wait_until( long long *var, int cmp, long long value ){
    shmem_template_wait_until( var, cmp, value );
}
void shmem_wait_until( long *ivar, int cmp, long value ){
    shmem_template_wait_until( ivar, cmp, value );
}

