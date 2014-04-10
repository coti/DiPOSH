#
#
# Copyright (c) 2014 LIPN - Universite Paris 13
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