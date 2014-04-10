lib_LTLIBRARIES = libshmem.la

libshmem_la_SOURCES = shmem_accessible.cpp   shmem_lock.cpp 		\
		shmem_alloc.cpp        shmem_MeMyselfAndI_implem.cpp    \
		shmem_atomic.cpp       shmem_collect.cpp		\
		shmem_barrier.cpp      shmem_ptr.cpp			\
		shmem_bcast.cpp        shmem_put.cpp			\
		shmem_collectives.cpp  shmem_SymmetricHeap_Implem.cpp	\
		shmem_get.cpp          shmem_sync.cpp			\
		shmem_init.cpp         shmem_reduce.cpp			\
		shmem_utils.cpp

libshmem_la_LDFLAGS = -version-info 0:0:1

bin_PROGRAMS = spawn shmem_s2sc 

spawn_SOURCES = spawn.cpp shmem_portability.cpp shmem_constants.h shmem_portability.h
shmem_s2sc_SOURCES = shmem_s2sc.flex


dist_bin_SCRIPTS = shmemcc

include_HEADERS=shmem.h

ACLOCAL_AMFLAGS=-I ../config 