#include "shmem.h"
#include "shmem_internal.h"

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
