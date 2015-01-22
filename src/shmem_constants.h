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


/* a definir :
_SHMEM_REDUCE_MIN_WRKDATA_SIZE
_SHMEM_REDUCE_SYNC_SIZE

 */

#endif
