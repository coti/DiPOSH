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

/* Extensions of the OpenSHMEM standard proposed by POSH.
   The function delared in this file are *not* part of the OpenSHMEM standard
*/


/* User-triggered coordinated checkpointing.
   This is a collective operation: it includes a barrier.

 */

#include <sys/types.h>

void shmem_coord_checkpoint();

/* This is necessary because DMTCP returns a virual pid */

#ifdef CHANDYLAMPORT
#include <dmtcp.h>
#endif

//#define _posh_getpid getpid
inline __pid_t _posh_getpid() {
#ifdef CHANDYLAMPORT
    return dmtcp_virtual_to_real_pid( getpid() );
#else
    return getpid();
#endif
}

#include "perf.h"
