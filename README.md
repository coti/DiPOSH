# POSH
### Paris OpenSHMEM http://lipn.univ-paris13.fr/~coti/POSH


## What is it?

POSH (Paris OpenSHMEM) is an implementation of the OpenSHMEM standard [1] for 
shared-memory systems. It relies of the Boost library for inter-process 
communications. It is both a high-performance communication library, and a 
testbed for research on distributed algorithms on the communication model 
followed and implemented by OpenSHMEM.

[1] http://openshmem.org

## Contents

POSH is made of several components:
- the libshmem.so library
- the run-time environment: spawn
- the compiler: shmemcc

The shmemcc compiler first performs an initial parsing of the source code in 
order to insert global, static variables handling. Then it calls your regular 
C++ compiler in order to generate the source code and link the executable binary 
with the libshmem library.

```
  shmemcc -O3 -Wall -o mycode mycode.c
```

The executable binary must be executed by the run-time environment:
```
  spawm -n 4 ./mycode
```

## The Latest Version

Details of the latest version can be found on [2]. For details on this version, 
please see the file called NEWS.

[2] http://lipn.univ-paris13.fr/~coti/POSH

## Dependencies 

POSH was written en C++11. 

POSH relies on:
- flex
- perl
- some parts of Boost: Boost.Interprocess, Boost.Thread and Boost.System

## Installation

Please see the file called INSTALL.

## Example code

A small piece of OpenSHMEM code was written and placed in the example/ directory.
Please see the file called INSTALL for more information.

## Licensing

Please see the file called LICENSE.

## Contact and author

Please see the file called AUTHOR.