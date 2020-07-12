# Collective communications in DiPOSH

## Design of the collective communication system

All the routines are in a class derived from ```Collectives_t```. This is a pure virtual class (an interface). In this class, we have all the calls to the SHMEM collective communication routines.

The initialization opens a class derived from ```Collectives_t``` in ```MeMyselfAndI::collectiveInit```. A ```std::unique_ptr``` to this class is present in the big global data structure ```myInfo```.

Therefore, a collective communication call is performed by, for example:
```
myInfo.collectives->posh_long_broadcast( ... )
```

How do we choose which class we use:
* based on what the user asks for: using the environment variable ```SHMEM_COLL_TYPE```
* based on which communication channel the user asks for: using the environment variable ```SHMEM_COMM_CHAN```
* based on which communication channel we are using on all the processes (TODO)
* fallback: use the flat one

## Implementation of the collective communication system

(Di)POSH relies a lot on C++ templates. Problem: we cannot define a virtual template method in the ```Collectives_t``` interface. Solution: use method overloading. Down side: it means we have a lot of methods to write, losing the interest of templates. As a consequence, the ```Collectives_t``` interface and its derived classes contain a lot of methods.