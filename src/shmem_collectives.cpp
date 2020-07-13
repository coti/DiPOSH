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

#include "shmem_internal.h"

#include "shmem_collectives.h"
#include "posh_collectives.h"
#include "posh_coll_flat.h"
#include "posh_coll_mpi.h"

#include "posh_collectives.tpp"

void MeMyselfAndI::setCollectiveType( unsigned char _type ) {
    this->collective->type = _type;
}

unsigned char MeMyselfAndI::getCollectiveType( ) {
    return this->collective->type;
}

void* MeMyselfAndI::allocCollectiveSpace( size_t _size ){
    this->collective->ptr = _shmallocFake( _size );
#if _DEBUG 
    this->collective->space = _size;
#endif
    return this->collective->ptr;
}

void* MeMyselfAndI::getCollectiveBuffer( ) {
    return this->collective->ptr;
}

void MeMyselfAndI::collectiveInit( char* coll_type ) {
    this->collective = (Collective_t*) _shmallocFake( sizeof( Collective_t ) );
    this->collective->ptr  = NULL;
    this->collective->cnt  = 0;
    this->collective->type = _SHMEM_COLL_NONE;
    this->collective->inProgress = false;
#if _DEBUG 
    this->collective->space = -1;
#endif

    /* Initialize the collectives I am going to use */

    if( NULL != coll_type ) { /* The user specified which collective communication module they want */
        if ( 0 == strcmp( coll_type, "MPI" ) ) {
#if 1 //def MPICHANNEL
            this->collectives = std::unique_ptr<Collectives_t>{ new Collectives_MPI_t };
#endif
        }
        else{
            std::cout << "Could not understand the collective type specified. Fallback: flat"<< std::endl;
            this->collectives = std::unique_ptr<Collectives_t>{ new Collectives_flat_t };
        }
    } else {
        /* The user did not specify which collective communication module they want.
           Which communication channel are we using? */    
        if( NULL == comm_channel ) {
#if 1 //def MPICHANNEL
            this->collectives = std::unique_ptr<Collectives_t>{ new Collectives_MPI_t };
        }
#elif  _WITH_TCP
            /***/
#endif
        else {
            if ( 0 == strcmp( comm_channel, "MPI" ) ) {            
#if 1 //def MPICHANNEL
                this->collectives = std::unique_ptr<Collectives_t>{ new Collectives_MPI_t };
#endif // MPICHANNEL
            }
            
            else {/* Last resort: the flat one */
                this->collectives = std::unique_ptr<Collectives_t>{ new Collectives_flat_t };
            }
        }
    }
    
    /* TODO check that all the processes use the same module */
    

    /* DOTO send this to the flat module */
    
    char* mutName;
    if( myInfo.getRank() == 0 ) {
        asprintf( &mutName, "mtx_shmem_gather_root" );
        boost::interprocess::named_mutex::remove( mutName ); // remove any pre-existent mutex
        free( mutName );

        asprintf( &mutName, "shmem_barrier_ROOT" );
        boost::interprocess::named_mutex::remove( mutName ); // remove any pre-existent mutex
        free( mutName );


    }

    asprintf( &mutName, "mtx_shmem_bcast_%d", myInfo.getRank() );
    boost::interprocess::named_mutex::remove( mutName ); // remove any pre-existent mutex
    free( mutName );


}

void MeMyselfAndI::collectiveReset() {
    this->collective->cnt  = 0;
    this->collective->type = _SHMEM_COLL_NONE;
    this->collective->inProgress = false;
#if _DEBUG 
    this->collective->space = -1;
#endif
}

Collective_t*  MeMyselfAndI::getCollective(){
    return this->collective;
}

/* Barriers */

void shmem_barrier(int PE_start, int logPE_stride, int PE_size, long *pSync ){
    myInfo.collectives->posh_barrier( PE_start, logPE_stride, PE_size, pSync );
}

void shmem_barrier_all(){
    myInfo.collectives->posh_barrier_all();
}

/* Broadcasts */

void shmem_broadcast32( void* target, const void* source,  size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    myInfo.collectives->posh_broadcast32( (int32_t*) target, (const int32_t*) source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_broadcast64(void* target, const void* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    myInfo.collectives->posh_broadcast32( (int64_t*) target, (const int64_t*) source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}
    
void shmem_short_broadcast( short* target, const short* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, short* pSync ){
     myInfo.collectives->posh_short_broadcast( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}
    
void shmem_int_broadcast( int* target, const int* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, int* pSync ){
    myInfo.collectives->posh_int_broadcast( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_long_broadcast( long* target, const long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    myInfo.collectives->posh_long_broadcast( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}
    
void shmem_longlong_broadcast( long long* target, const long long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long long* pSync ){
    myInfo.collectives->posh_longlong_broadcast( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}
    
void shmem_float_broadcast( float* target, const float* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, float* pSync ){
    myInfo.collectives->posh_float_broadcast( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}
    
void shmem_double_broadcast( double* target, const double* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, double* pSync ){
    myInfo.collectives->posh_double_broadcast( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
} 

/* Reductions */


   // AND
    
int shmem_uchar_and_reduce(/*shmem_team_t team,*/ unsigned char *dest, const unsigned char *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_short_and_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_ushort_and_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_int_and_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_uint_and_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_long_and_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_ulong_and_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_longlong_and_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_ulonglong_and_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}

int shmem_and_reduce(/*shmem_team_t team,*/ unsigned char *dest, const unsigned char *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_and_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_and_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_and_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
    int shmem_and_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_and_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_and_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_and_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}
int shmem_and_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_and );
}

// OR

int shmem_uchar_or_reduce(/*shmem_team_t team,*/ unsigned char *dest, const unsigned char *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_short_or_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_ushort_or_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_int_or_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_uint_or_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_long_or_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_ulong_or_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_longlong_or_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_ulonglong_or_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}

int shmem_or_reduce(/*shmem_team_t team,*/ unsigned char *dest, const unsigned char *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_or_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_or_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_or_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_or_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_or_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_or_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_or_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
int shmem_or_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_or );
}
    
// XOR

int shmem_uchar_xor_reduce(/*shmem_team_t team,*/ unsigned char *dest, const unsigned char *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_short_xor_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_ushort_xor_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_int_xor_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_uint_xor_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_long_xor_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_ulong_xor_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_longlong_xor_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_ulonglong_xor_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}

int shmem_xor_reduce(/*shmem_team_t team,*/ unsigned char *dest, const unsigned char *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_xor_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_xor_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_xor_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_xor_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_xor_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_xor_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_xor_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}
int shmem_xor_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_xor );
}

// MAX

int shmem_short_max_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_ushort_max_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_int_max_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_uint_max_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_long_max_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_ulong_max_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_longlong_max_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_ulonglong_max_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}

int shmem_max_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_max_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_max_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_max_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_max_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_max_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_max_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_max_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_max_reduce(/*shmem_team_t team,*/ float *dest, const float *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_max_reduce(/*shmem_team_t team,*/ double *dest, const double *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}
int shmem_max_reduce(/*shmem_team_t team,*/ long double *dest, const long double *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_max );
}

// MIN

int shmem_short_min_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_ushort_min_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_int_min_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_uint_min_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_long_min_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_ulong_min_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_longlong_min_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_ulonglong_min_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}

int shmem_min_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_min_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_min_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_min_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_min_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_min_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_min_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_min_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_min_reduce(/*shmem_team_t team,*/ float *dest, const float *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_min_reduce(/*shmem_team_t team,*/ double *dest, const double *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}
int shmem_min_reduce(/*shmem_team_t team,*/ long double *dest, const long double *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_min );
}

// SUM

int shmem_short_sum_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_ushort_sum_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_int_sum_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_uint_sum_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_long_sum_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_ulong_sum_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_longlong_sum_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_ulonglong_sum_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}

int shmem_sum_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_sum_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_sum_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_sum_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_sum_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_sum_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_sum_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_sum_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_sum_reduce(/*shmem_team_t team,*/ float *dest, const float *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_sum_reduce(/*shmem_team_t team,*/ double *dest, const double *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}
int shmem_sum_reduce(/*shmem_team_t team,*/ long double *dest, const long double *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_sum );
}

// TODO: complex

// PROD

int shmem_short_prod_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_ushort_prod_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_int_prod_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_uint_prod_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_long_prod_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_ulong_prod_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_longlong_prod_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_ulonglong_prod_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}

int shmem_prod_reduce(/*shmem_team_t team,*/ short *dest, const short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_prod_reduce(/*shmem_team_t team,*/ unsigned short *dest, const unsigned short *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_prod_reduce(/*shmem_team_t team,*/ int *dest, const int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_prod_reduce(/*shmem_team_t team,*/ unsigned int *dest, const unsigned int *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_prod_reduce(/*shmem_team_t team,*/ long *dest, const long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_prod_reduce(/*shmem_team_t team,*/ unsigned long *dest, const unsigned long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_prod_reduce(/*shmem_team_t team,*/ long long *dest, const long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_prod_reduce(/*shmem_team_t team,*/ unsigned long long *dest, const unsigned long long *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_prod_reduce(/*shmem_team_t team,*/ float *dest, const float *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_prod_reduce(/*shmem_team_t team,*/ double *dest, const double *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
int shmem_prod_reduce(/*shmem_team_t team,*/ long double *dest, const long double *source, size_t nreduce) {
    return myInfo.collectives->posh_reduce( dest, source, nreduce,  &_shmem_operation_template_prod );
}
// TODO: complex
    



/** internal functions **/

extern std::vector<Process> processes;


/* "bad" allgather: all the processes send their buffer to the root process,
 * then the aforementionned root process broadcasts the resulting buffer
 * to all the other processes.
 * - buffer is the resulting buffer, allocated in the symmetric heap
 * - myelem is the local buffer that will be put into the resulting buffer
 * - nb is the number of elements in each local buffer
 */

void shmem_int_allgather_flat( int* buffer, const int* myelem, size_t nb ) {
    shmem_template_allgather_flat( buffer, myelem, nb );
}

void shmem_int_gather_flat( int* buffer, const int* myelem, size_t nb, int root ) {
    shmem_template_gather_flat( buffer, myelem, nb, root );
}

bool _shmem_is_remote_process_in_coll( int rank ) {
    return shmem_template_g( &(myInfo.getCollective()->inProgress), rank );
}

void _shmem_wait_until_entered( int rank ) {

    while( false == _shmem_is_remote_process_in_coll( rank ) ){
        usleep( SPIN_TIMER );
    }
}

bool _shmem_part_of_coll( int rank, int PE_start, int PE_size, int logPE_stride ) {
    return ( ( rank < PE_start ) ||
             ( rank > ( PE_start + ( PE_size * ( 0x1 << logPE_stride) ) ) ) ||
             ( ( rank - PE_start ) % ( 0x1 << logPE_stride ) ) != 0 );
}

/* Returns the length of the prefix
 * e.g., if nb = 0x001101, returns 4
 */

int _shmem_binomial_binbase_size( int nb ) {
    int len;
    int b = 0x1;

    len = 0;
    while( b <= nb ) {
        b = b << 1;
        len++;
    }
    return len;
}

/* Father's rank
 * My own rank minus the heaviest bit.
 */

int _shmem_binomial_myfather( int PE_root, int PE_start, int logPE_stride, int PE_size ) {
    int rank = myInfo.getRank();
    rank -= PE_start;
    int log = _shmem_binomial_binbase_size( rank );
    int b = 0x1 << (log-1);
    int father = rank ^ b;
    father = rank - ( ( rank - father ) * 0x1 << logPE_stride );
    return father;
}

/* Number of children
 */

int _shmem_binomial_children( int rank, int size ) {
    int log_rank = _shmem_binomial_binbase_size( rank );

    int n = 0;
    int child = rank;
    while( child < size ) {
        child = rank | ( 0x1 << ( log_rank + n++ ) ) ;
    }
    return n - 1;
}


