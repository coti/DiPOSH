#include "shmem.h"
#include "shmem_internal.h"

/* This routine determines if a target PE is reachable from the calling PE.
 * This routine returns a value that indicates whether the calling PE is 
 * able to perform OpenSHMEM communication operations with the target PE. 
 * In C/C++, shmem_pe_accessible() returns 1 if the speciﬁed PE is a valid
 * target PE for OpenSHMEM functions; otherwise, it returns 0. 
 */

int shmem_pe_accessible( int pe ){

    char* name;
    int res;

    res = 1;

    /* Build the name of the remote PE's shared heap */

    name = myHeap.buildHeapName( pe );

    try { /* Can we touch it? */
        managed_shared_memory segment( open_only, name );
    } catch( int e ) {
        res = 0;  /* failed to open it: it does not exist */
        std::cerr << pe << " is not reachable" << std::endl;
    }

    free( name );
    return res;
}

/* This routine indicates if an address is accessible via OpenSHMEM operations
 * from the speciﬁed target PE.
 * In C/C++, shmem_addr_accessible() returns 1 if the data object at
 * address addr is in a symmetric segment and can be accessed via OpenSHMEM
 * functions; otherwise, it returns 0.
 */

int shmem_addr_accessible( void* addr, int pe ){

    int res;
    char* name;

    /* is the remote PE accessible? */
    
    /* Build the name of the remote PE's shared heap */

    name = myHeap.buildHeapName( pe );

    try { /* Can we touch it? */
        managed_shared_memory segment( open_only, name );
        if( true == segment.belongs_to_segment( addr ) ) {
            res = 0;
        } else {
            res = 1;
        }
    } catch( int e ) {
        /* failed to open it: it does not exist */
        res = 0;
    }

    /* does the address belong to the remote symmetric segment, ie, 
       can it belong to this address space? (CC: this is how I understand 
       the specs) */

    free( name );
    return res;
}
