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

#include "shmem.h"
#include "shmem_internal.h"

/* FIXME need the other implementations */

#include "posh_heap.h"

/* Returns a pointer to a data object of a target PE.
* * The shmem_ptr() function only returns a non-NULL value on systems
* where ordinary memory loads and stores are used to implement OpenSHMEM
* put and get operations.
* * target must be the address of a symmetric data object.
* * pe must be a PE number. For more information about how PE numbers are
* assigned please refer to the Execution Model section.
* The shmem_ptr() routine returns an address that can be used to directly
* reference target on the target PE pe. The programmer must be able to assign
* this address to a pointer and perform ordinary loads and stores to this
* target address.
* When a sequence of loads (gets) and stores (puts) to a data object on a
* target PE does not match the access pattern provided in a OpenSHMEM data
* transfer routine like shmem_put32() or shmem_real_iget(), the shmem_ptr()
* function can provide an efficient means to accomplish the communication.
*/

void *shmem_ptr(void *target, int pe){

    void* buf;
    buf = _getRemoteAddr( target, pe );
    return buf;
}
