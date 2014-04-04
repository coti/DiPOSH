/*
 *
 * Copyright (c) 2014 LIPN - Universite Paris 13
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
 *
 */

#include "shmem.h"
#include "shmem_internal.h"

#include <boost/interprocess/sync/named_mutex.hpp> 


/* Sets a mutual exclusion memory lock after it is no longer in use by another process. 
 * The shmem_set_lock() routine sets a mutual exclusion lock after waiting for the lock
 * to be freed by any other PE currently holding the lock. Waiting PEs are assured of
 * getting the lock in a first-come, first-served manner.
*/

void shmem_set_lock( long* _lock ){
    boost::interprocess::named_mutex* mtx = myInfo.getLock( *_lock );
    mtx->lock(); 
}


/* Releases a lock previously set by the calling PE. 
* The shmem_clear_lock() routine releases a lock previously set by shmem_set_lock()
* after ensuring that all local and remote stores initiated in the critical region are
* complete.*/

void shmem_clear_lock( long* _lock ){
    boost::interprocess::named_mutex* mtx = myInfo.getLock( *_lock );
    mtx->unlock(); 
}

/* Sets a mutual exclusion lock only if it is currently cleared. 
 * The shmem_test_lock() function sets a mutual exclusion lock only if it is currently
 * cleared. By using this function, a PE can avoid blocking on a set lock. If the lock
 * is currently set, the function returns without waiting.
 * Return Values:
 * * 0 The lock was originally cleared and this call was able to set the lock. 
 * * 1 The lock had been set and the call returned without waiting to set the lock.
 * Note to implementers: the lock variable is initialized to 0 everywhere. Once set,
 * the value of the lock should be treated as opaque to allow implementations freedom
 * to optimize lock structures, e.g. for specific hardware operations.
*/

int shmem_test_lock( long* _lock ){
    boost::interprocess::named_mutex* mtx = myInfo.getLock( *_lock );
    bool rc = mtx->try_lock(); 
    return ( ( rc == true ) ? 0 : 1 );
}
