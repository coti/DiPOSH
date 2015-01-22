#include "shmem_reduce.h"

/* This routine returns the result of performing a bitwise AND operation on the source data
 * object of every PE in the active set. The active set of PEs is defined by the triple PE_start,
 * logPE_stride and PE_size.
 * As with all OpenSHMEM collective routines, each of these routines assumes that only PEs
 * in the active set call the routine. If a PE not in the active set calls a OpenSHMEM collective
 * routine, undefined behavior results.
*/



void shmem_short_and_to_all( short *target, short *source, int nreduce, int PE_start, int logPE_stride, int PE_size, short *pWrk, long *pSync ){
    shmem_template_and_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_int_and_to_all( int *target, int *source, int nreduce, int PE_start, int logPE_stride, int PE_size, int *pWrk, long *pSync ){
    shmem_template_and_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_long_and_to_all( long *target, long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long *pWrk, long *pSync ){
    shmem_template_and_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_longlong_and_to_all( long long *target, long long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long long *pWrk, long *pSync ){
    shmem_template_and_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}


void shmem_short_or_to_all( short *target, short *source, int nreduce, int PE_start, int logPE_stride, int PE_size, short *pWrk, long *pSync ){
    shmem_template_or_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_int_or_to_all( int *target, int *source, int nreduce, int PE_start, int logPE_stride, int PE_size, int *pWrk, long *pSync ){
    shmem_template_or_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_long_or_to_all( long *target, long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long *pWrk, long *pSync ){
    shmem_template_or_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_longlong_or_to_all( long long *target, long long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long long *pWrk, long *pSync ){
    shmem_template_or_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}


void shmem_short_xor_to_all( short *target, short *source, int nreduce, int PE_start, int logPE_stride, int PE_size, short *pWrk, long *pSync ){
    shmem_template_xor_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_int_xor_to_all( int *target, int *source, int nreduce, int PE_start, int logPE_stride, int PE_size, int *pWrk, long *pSync ){
    shmem_template_xor_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_long_xor_to_all( long *target, long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long *pWrk, long *pSync ){
    shmem_template_xor_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_longlong_xor_to_all( long long *target, long long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long long *pWrk, long *pSync ){
    shmem_template_xor_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}




void shmem_short_max_to_all( short *target, short *source, int nreduce, int PE_start, int logPE_stride, int PE_size, short *pWrk, long *pSync ){
    shmem_template_max_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_int_max_to_all( int *target, int *source, int nreduce, int PE_start, int logPE_stride, int PE_size, int *pWrk, long *pSync ){
    shmem_template_max_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_long_max_to_all( long *target, long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long *pWrk, long *pSync ){
    shmem_template_max_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_float_max_to_all( float *target, float *source, int nreduce, int PE_start, int logPE_stride, int PE_size, float *pWrk, long *pSync ){
    shmem_template_max_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_double_max_to_all( double *target, double *source, int nreduce, int PE_start, int logPE_stride, int PE_size, double *pWrk, long *pSync ){
    shmem_template_max_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_longlong_max_to_all( long long *target, long long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long long *pWrk, long *pSync ){
    shmem_template_max_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_longdouble_max_to_all( long double *target, long double *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long double *pWrk, long *pSync ){
    shmem_template_max_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}


void shmem_short_min_to_all( short *target, short *source, int nreduce, int PE_start, int logPE_stride, int PE_size, short *pWrk, long *pSync ){
    shmem_template_min_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_int_min_to_all( int *target, int *source, int nreduce, int PE_start, int logPE_stride, int PE_size, int *pWrk, long *pSync ){
    shmem_template_min_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_long_min_to_all( long *target, long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long *pWrk, long *pSync ){
    shmem_template_min_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_float_min_to_all( float *target, float *source, int nreduce, int PE_start, int logPE_stride, int PE_size, float *pWrk, long *pSync ){
    shmem_template_min_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_double_min_to_all( double *target, double *source, int nreduce, int PE_start, int logPE_stride, int PE_size, double *pWrk, long *pSync ){
    shmem_template_min_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_longlong_min_to_all( long long *target, long long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long long *pWrk, long *pSync ){
    shmem_template_min_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_longdouble_min_to_all( long double *target, long double *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long double *pWrk, long *pSync ){
    shmem_template_min_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}


void shmem_short_sum_to_all( short *target, short *source, int nreduce, int PE_start, int logPE_stride, int PE_size, short *pWrk, long *pSync ){
    shmem_template_sum_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_int_sum_to_all( int *target, int *source, int nreduce, int PE_start, int logPE_stride, int PE_size, int *pWrk, long *pSync ){
    shmem_template_sum_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_long_sum_to_all( long *target, long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long *pWrk, long *pSync ){
    shmem_template_sum_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_float_sum_to_all( float *target, float *source, int nreduce, int PE_start, int logPE_stride, int PE_size, float *pWrk, long *pSync ){
    shmem_template_sum_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_double_sum_to_all( double *target, double *source, int nreduce, int PE_start, int logPE_stride, int PE_size, double *pWrk, long *pSync ){
    shmem_template_sum_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_longlong_sum_to_all( long long *target, long long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long long *pWrk, long *pSync ){
    shmem_template_sum_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_longdouble_sum_to_all( long double *target, long double *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long double *pWrk, long *pSync ){
    shmem_template_sum_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
/*void shmem_complexf_sum_to_all( float complex *target, float complex *source, int nreduce, int PE_start, int logPE_stride, int PE_size, float complex *pWrk, long *pSync ){
    shmem_template_sum_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_complexd_sum_to_all( double complex *target, double complex *source, int nreduce, int PE_start, int logPE_stride, int PE_size, double complex *pWrk, long *pSync ){
    shmem_template_sum_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
    }*/


void shmem_short_prod_to_all( short *target, short *source, int nreduce, int PE_start, int logPE_stride, int PE_size, int *pWrk, long *pSync ){
    shmem_template_prod_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_int_prod_to_all( int *target, int *source, int nreduce, int PE_start, int logPE_stride, int PE_size, int *pWrk, long *pSync ){
    shmem_template_prod_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_long_prod_to_all( long *target, long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long *pWrk, long *pSync ){
    shmem_template_prod_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_float_prod_to_all( float *target, float *source, int nreduce, int PE_start, int logPE_stride, int PE_size, float *pWrk, long *pSync ){
    shmem_template_prod_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_double_prod_to_all( double *target, double *source, int nreduce, int PE_start, int logPE_stride, int PE_size, double *pWrk, long *pSync ){
    shmem_template_prod_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_longlong_prod_to_all( long long *target, long long *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long long *pWrk, long *pSync ){
    shmem_template_prod_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_longdouble_prod_to_all( long double *target, long double *source, int nreduce, int PE_start, int logPE_stride, int PE_size, long double *pWrk, long *pSync ){
    shmem_template_prod_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
/*void shmem_complexf_prod_to_all( float complex *target, float complex *source, int nreduce, int PE_start, int logPE_stride, int PE_size, float complex *pWrk, long *pSync ){
    shmem_template_prod_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
}
void shmem_complexd_prod_to_all( double complex *target, double complex *source, int nreduce, int PE_start, int logPE_stride, int PE_size, double complex *pWrk, long *pSync ){
    shmem_template_prod_to_all( target, source, nreduce, PE_start, logPE_stride, PE_size, pWrk, pSync );
    }*/


