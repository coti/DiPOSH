#
#
# Copyright (c) 2014-2020 LIPN - Universite Paris 13
#                              - Universite Sorbonne Paris Nord
#                    All rights reserved.
#
# This file is part of POSH.
# 
# POSH is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# POSH is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with POSH.  If not, see <http://www.gnu.org/licenses/>.
#
#

#################################################################################
# Library
#################################################################################

lib_LTLIBRARIES = libshmem.la

#################################################################################
# Sources
#################################################################################

libshmem_la_SOURCES = shmem_accessible.cpp   shmem_lock.cpp 		\
		shmem_alloc.cpp        shmem_MeMyselfAndI_implem.cpp    \
		shmem_atomic.cpp       shmem_collect.cpp		\
		shmem_barrier.cpp      shmem_ptr.cpp			\
		shmem_put.cpp	       shmem_init.cpp 	         	\
		shmem_collectives.cpp  shmem_SymmetricHeap_Implem.cpp	\
		shmem_get.cpp          shmem_sync.cpp			\
		shmem_utils.cpp

# shmem_bcast.cpp shmem_reduce.cpp	

#################################################################################
# RTE, interface with the Launcher
#################################################################################

libshmem_la_SOURCES += posh_launcher.h
if ENABLE_LAUNCHERLOCAL
libshmem_la_SOURCES += posh_launcher_sm.cpp posh_launcher_sm.h
endif
if ENABLE_LAUNCHERMPI
libshmem_la_SOURCES += posh_launcher_mpi.cpp posh_launcher_mpi.h
endif
if ENABLE_LAUNCHERPADICO
libshmem_la_SOURCES += posh_launcher_padico.cpp posh_launcher_padico.h
endif
if ENABLE_LAUNCHERGASPI
libshmem_la_SOURCES += posh_launcher_gaspi.cpp posh_launcher_gaspi.h
endif

#################################################################################
# Communication channel
#################################################################################

libshmem_la_SOURCES += posh_neighbor.h posh_heap.cpp posh_heap.h posh_communication.h
libshmem_la_SOURCES += posh_sm.cpp posh_sm.h

if ENABLE_COMM_TCP
libshmem_la_SOURCES += ked_tcp.cpp ked_tcp.h posh_tcp.cpp posh_tcp.h shmem_tcp.cpp shmem_tcp.h
endif
if ENABLE_COMM_MPI
libshmem_la_SOURCES += posh_mpi.cpp posh_mpi.h
endif
if ENABLE_COMM_NMAD
libshmem_la_SOURCES += posh_nmad.cpp posh_nmad.h
endif
if ENABLE_COMM_KNEM
libshmem_la_SOURCES += posh_knem.cpp posh_knem.h
endif
if ENABLE_COMM_GASPI
libshmem_la_SOURCES += posh_gaspi.cpp posh_gaspi.h
endif
if ENABLE_COMM_HUB_MPI
libshmem_la_SOURCES += posh_hub.cpp posh_hub.h
endif

#################################################################################
# Collective communications
#################################################################################

libshmem_la_SOURCES +=  posh_coll_flat.cpp posh_coll_flat.h \
			posh_coll_binomial.cpp posh_coll_binomial.tpp posh_coll_binomial.h \
			posh_collectives.cpp posh_collectives.tpp posh_collectives.h
if ENABLE_COMM_MPI
libshmem_la_SOURCES += posh_coll_mpi.cpp posh_coll_mpi.h
endif
if ENABLE_COMM_GASPI
libshmem_la_SOURCES += posh_coll_gaspi.cpp posh_coll_gaspi.h
endif

#################################################################################
# Fault tolerance
#################################################################################

if ENABLE_FT
libshmem_la_SOURCES += posh_checkpointing.cpp posh_chandylamport.cpp
endif

#################################################################################
# Performance profiling
#################################################################################

#################################################################################
# Compilation information
#################################################################################

libshmem_la_LDFLAGS = -version-info 0:2:0

#################################################################################
# Binaries: local launcher and pre-compiler
#################################################################################

bin_PROGRAMS = shmem_s2sc 

#spawn_SOURCES = spawn.cpp shmem_portability.cpp shmem_constants.h shmem_portability.h

#if ENABLE_FT
#spawn_SOURCES += posh_ft_coordinator.cpp
#endif

shmem_s2sc_SOURCES = shmem_s2sc.ll

shmem_s2sc_LDADD = @LEXLIB@ 
shmem_s2sc_LDFLAGS = -no-undefined

dist_bin_SCRIPTS = shmemcc

include_HEADERS=shmem.h shmem_constants.h

ACLOCAL_AMFLAGS=-I ../config 
