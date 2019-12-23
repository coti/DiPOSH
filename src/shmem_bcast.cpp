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

#include "shmem_bcast.h"


/*
The broadcast routines write the data at address source of the PE specified by PE_root to address target on all other PEs in the active set. The active set of PEs is defined by the triple PE_start, logPE_stride and PE_size. The data is not copied to the target address on the PE specified by PE_root.
Before returning, the broadcast routines ensure that the elements of the pSync array are restored to their initial values.
As with all OpenSHMEM collective routines, each of these routines assumes that only PEs in the active set call the routine. If a PE not in the active set calls a OpenSHMEM collective routine, undefined behavior results.
* Parameters
* target Address of the symmetric data object in which to store the data.
* source Address of the symmetric data object that contains data to be copied.
* nelems The number of elements in the target and source arrays.
* PE_root The PE from which data will be copied.
* PE_start The lowest PE number of the active set of PEs.
* logPE_stride The log (base 2) of the stride between consecutive PE numbers in the active
* set.
* PE_size The number of PEs in the active set.
* pSync A symmetric work array.
* Constraints
* * target array on all PEs in the active set must be ready to accept the results of the broadcast.
* * source and target must be the addresses of symmetric data objects.
* * Data types for target are as follows:
* * - For shmem_broadcast8() and shmem_broadcast64(), target may be of any non character type that has an element size of 64 bits. No Fortran derived types or C/C++ structures are allowed.
* * - For shmem_broadcast32(), target may be of any non character type that has an element size of 32 bits. No Fortran derived types or C/C++ structures are allowed.
* * - For shmem_broadcast4(), target may be of any non character type that has an element size of 32 bits.
* * source must be the same type as target.
* * source and target may be the same array, but they must not be overlapping arrays.
* * If using C/C++, nelems must be of type integer. If you are using Fortran, it must be a default integer value.
* * PE_root is a zero-based integer count into the active set: 0 <= P E_root < P E_size. If you are using Fortran, it must be a default integer value.
* * If using C/C++, PE_start must be of type integer. If you are using Fortran, it must be a default integer value. Its value must be greater than or equal to zero.
* * If using C/C++, logPE_stride must be of type integer. If you are using Fortran, it must be a default integer value.
* * If using C/C++, PE_size must be of type integer. If you are using Fortran, it must be a default integer value. Its value must be less than or equal to the total number of PEs minus one.
* * In C/C++, pSync must be of type long and size _SHMEM_BCAST_SYNC_SIZE. In Fortran, pSync must be of type integer and size SHMEM_BCAST_SYNC_SIZE. Every
element of this array must be initialized with the value _SHMEM_SYNC_VALUE (in
C/C++) or SHMEM_SYNC_VALUE (in Fortran) before any of the PEs in the active set enter the broadcast.
* * The pSync array on all PEs in the active set must not be in use from a prior call to a collective OpenSHMEM routine.
* * This routine must be called by all the PEs in the active set at the same point of the execution path; otherwise undefined behavior results.
*/

/* 8, 64 => 8 bytes, 64 bits -> cast en int64_t
   4, 32 => 4 bytes, 32 bits -> cast en int32_t
*/

void shmem_broadcast4( void* target, const void* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    shmem_broadcast_template( (int32_t*) target, (const int32_t*) source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );

}

void shmem_broadcast32( void* target, const void* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    shmem_broadcast_template( (int32_t*) target, (const int32_t*) source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_broadcast8( void* target, const void* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    shmem_broadcast_template( (int64_t*) target, (const int64_t*) source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_broadcast64(void* target, const void* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    shmem_broadcast_template( (int64_t*) target, (const int64_t*) source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}


/* not part of the standard */

void shmem_short_broadcast( short* target, const short* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, short* pSync ){
    shmem_broadcast_template( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}

void shmem_int_broadcast( int* target, const int* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, int* pSync ){
    shmem_broadcast_template( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}
void shmem_long_broadcast( long* target, const long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long* pSync ){
    shmem_broadcast_template( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}
void shmem_longlong_broadcast( long long* target, const long long* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, long long* pSync ){
    shmem_broadcast_template( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}
void shmem_float_broadcast( float* target, const float* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, float* pSync ){
    shmem_broadcast_template( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}
void shmem_double_broadcast( double* target, const double* source, size_t nelems, int PE_root, int PE_start, int logPE_stride, int PE_size, double* pSync ){
    shmem_broadcast_template( target, source, nelems, PE_root, PE_start, logPE_stride, PE_size, pSync );
}


/* internal fonctions (not part of the standard) */

/* flat algo */

void shmem_int_bcast_flat( int* target, const int* source, size_t nbElem, int root ) {
    int PE_size = myInfo.getRank();
    shmem_broadcast_template( target, source, nbElem, root, root, 0, PE_size, (char*)NULL );
}

