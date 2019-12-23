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

#ifndef _SHMEM_CONSTANTS_H
#define _SHMEM_CONSTANTS_H

/* Compare operators */

static const int SHMEM_CMP_EQ = 1; // Equal 
static const int SHMEM_CMP_NE = 2; // Not equal 
static const int SHMEM_CMP_GT = 3; // Greater than 
static const int SHMEM_CMP_LE = 4; // Less than or equal to 
static const int SHMEM_CMP_LT = 5; // Less than

/* mandatory environment variables and default values */

extern char VAR_SMA_VERSION[];
extern int shmem_sma_version[];
extern char VAR_SMA_INFO[];
extern char shmem_sma_info[];

extern char VAR_HEAP_SIZE[];
//extern unsigned long long int heap_size; // TODO faudrait s'en passer un jour 
extern char VAR_SMA_DEBUG[];
extern int shmem_sma_debug;

/* environment variables */

extern char VAR_NUM_PE[];
extern char VAR_PE_NUM[];
extern char VAR_RUN_DEBUG[];
extern char VAR_PID_ROOT[];

//extern char heapNameBase[];

/* return values */

#define _SHMEM_SUCCESS        0

/* collectives */

#define _SHMEM_COLL_NONE      0
#define _SHMEM_COLL_BARRIER   1
#define _SHMEM_COLL_ALLGATHER 2
#define _SHMEM_COLL_BCAST     3
#define _SHMEM_COLL_GATHER    4
#define _SHMEM_COLL_REDUCE    5

/* default values */

#define _SHMEM_DEFAULT_HEAP_SIZE  134217728 /* 128 MB */
//#define _SHMEM_DEFAULT_HEAP_SIZE  1048576 /* 1 MB */


/* internal constants */

#ifndef _DEBUG
#define SPIN_TIMER 100
#else
#define SPIN_TIMER 100000
#endif

/* Boost.interprocess cannot create segments of shared memory
   smaller than a certain size. It throws a 
   boost::interprocess_exception::library_error if we are asking for
   something too small. */
#define POSH_MIN_SHM_SIZE 512

/* a definir :
_SHMEM_REDUCE_MIN_WRKDATA_SIZE
_SHMEM_REDUCE_SYNC_SIZE

 */

/* This is necessary because DMTCP returns a virual pid */
#ifdef CHANDYLAMPORT
//#define _posh_getpid getpid
#include <sys/types.h>
inline __pid_t _posh_getpid( void );

#endif


#endif
