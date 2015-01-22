#include <stdlib.h>

#include "shmem_collect.h"

/* The collect routines concatenate nelems elements from array source 
 * across all PEs in the active set, and store the result in array target. 
 * The fcollect routines require that nelems be the same value in all 
 * participating PEs, while the collect routines allow nelems to vary 
 * from PE to PE. 
 * As with all OpenSHMEM collective routines, each of these routines 
 * assumes that only PEs in the active set call the routine. If a PE not 
 * in the active set calls a OpenSHMEM collective routine, undefined 
 * behavior results.
 */

/* 8, 64 => 8 bytes, 64 bits -> cast en int64_t
   4, 32 => 4 bytes, 32 bits -> cast en int32_t
*/

void shmem_collect32( void *target, const void *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, long *pSync ){
    shmem_template_collect( (int32_t*)target, (int32_t*)source, nelems, PE_start, logPE_stride, PE_size, pSync );
}
/*void shmem_collect64( void *target, const void *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, long *pSync ){
    shmem_template_collect( (int64_t*)target, (int64_t*)source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_fcollect32(void *target, const void *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, long *pSync ) {
    shmem_template_fcollect( (int32_t*)target, (int32_t*)source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_fcollect64(void *target, const void *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, long *pSync ) {
    shmem_template_fcollect( (int64_t*)target, (int64_t*)source, nelems, PE_start, logPE_stride, PE_size, pSync );
    }*/

/* Not part of the standard */

/*void shmem_short_collect( short *target, const short *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, short *pSync ){
    shmem_template_collect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
    }*/

void shmem_int_collect( int *target, const int *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, int *pSync ){
    shmem_template_collect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

/*void shmem_long_collect( long *target, const long *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, long *pSync ){
    shmem_template_collect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_longlong_collect( long long *target, const long long *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, long long *pSync ){
    shmem_template_collect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_float_collect( float *target, const float *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, float *pSync ){
    shmem_template_collect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_double_collect( double* target, const double *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, double *pSync ){
    shmem_template_collect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_short_fcollect( short *target, const short *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, short *pSync ){
    shmem_template_collect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_int_fcollect( int *target, const int *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, int *pSync ){
    shmem_template_fcollect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_long_fcollect( long *target, const long *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, long *pSync ){
    shmem_template_fcollect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_longlong_fcollect( long long *target, const long long *source, size_t nelems, int PE_start, int logPE_stride, int PE_size, long long *pSync ){
    shmem_template_fcollect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_float_fcollect( float *target, const float *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, float *pSync ){
    shmem_template_fcollect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_double_fcollect( double* target, const double *source, size_t nelems, int PE_start, int logPE_stride, int	PE_size, float *pSync ) {
    shmem_template_fcollect( target, source, nelems, PE_start, logPE_stride, PE_size, pSync );
}

*/
