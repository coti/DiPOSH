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

MODE=NODEBUG

CC=g++
LD=g++

TOOLS=/usr/local

MYCFLAGS = -Wall  -I$(TOOLS)/include  -Wno-unused-variable -Wno-deprecated-declarations -fPIC -D_GNU_SOURCE -std=gnu++11 -Wno-unused-function #-fno-builtin 
MYLDFLAGS = -Wall #-fno-builtin
ifeq ($(MODE),DEBUG)
  CFLAGS=-O0 -g -D_DEBUG $(MYCFLAGS)
  LDFLAGS=-g -Wall -O0 $(MYLDFLAGS)
else
  CFLAGS= -O3 $(MYCFLAGS)
  LDFLAGS= -O3 $(MYLDFLAGS)
endif

ifeq ($(MODE),DEBUG)
  SHCCFLAGS=-g -O0
else
  SHCCFLAGS=-g -O3
endif

COLLTYPE = SHMEM_COLL_FLAT 
MEMCPYTYPE =  _USE_MEMCPY_  # _USE_SSE_MEMCPY_ #

CFLAGS += -D$(COLLTYPE) -D$(MEMCPYTYPE) #-D_USE_BLOCK_MEMCPY

## Possible flags:
# _DEBUG : debug mode
# _SAFE : safe mode (activates run-time checks)
# NB: _DEBUG activates most of the features enabled by the _SAFE flag
# MULTITHREADED_SPAWN

#LIBS=-L/opt/local/lib -lboost_thread-mt
LIBS=-L/usr/local/lib -L$(TOOLS)/lib -lboost_thread -lboost_system -lpthread

SOURCES_LIB =   shmem_accessible.cpp   shmem_lock.cpp 			\
		shmem_alloc.cpp        shmem_MeMyselfAndI_implem.cpp    \
		shmem_atomic.cpp       shmem_collect.cpp		\
		shmem_barrier.cpp      shmem_ptr.cpp			\
		shmem_bcast.cpp        shmem_put.cpp			\
		shmem_collectives.cpp  shmem_SymmetricHeap_Implem.cpp	\
		shmem_get.cpp          shmem_sync.cpp			\
		shmem_init.cpp         shmem_reduce.cpp			\
		shmem_utils.cpp


TEMPLATE_FILES = shmem_atomic.tpp  shmem_collectives.tpp     \
                 shmem_put.tpp     shmem_sync.tpp            \
                 shmem_bcast.tpp   shmem_get.tpp             \
                 shmem_reduce.tpp  shmem_collect.tpp

TEMPLATE_HEADERS = $(TEMPLATE_FILES:.tpp=.h)

HEADERS_LIB = shmem.h shmem_constants.h shmem_internal.h shmem_processinfo.h shmem_utils.h $(TEMPLATE_FILES) $(TEMPLATE_HEADERS)
OBJECTS_LIB = $(SOURCES_LIB:.cpp=.o)

SOURCES_EXEC = spawn.cpp shmem_portability.cpp
HEADERS_EXEC = shmem_constants.h shmem_portability.h
OBJECTS_EXEC = $(SOURCES_EXEC:.cpp=.o)

PARSER = shmem_s2sc

LIBSHMEM = libshmem.so
EXEC = spawn
COMPILER = shmemcc

TESTDIR  = ../example
BENCHPROG = bench_putget

all: $(LIBSHMEM) $(EXEC) $(PARSER)

# POSH RTE

$(EXEC): $(OBJECTS_EXEC) $(PARSER) $(HEADERS_EXEC)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS_EXEC) $(LIBS)

# lib shmem

$(LIBSHMEM): $(OBJECTS_LIB) $(HEADERS_LIB) $(TEMPLATE_FILES) $(TEMPLATE_HEADERS)
	$(LD) $(LDFLAGS) -shared -o $@ $(OBJECTS_LIB) $(LIBS) 

.cpp.o:
	$(CC) $(CFLAGS) -c $<

# source-to-source compiler

$(PARSER): $(PARSER).o 
	gcc $(FLAGS) -o $(PARSER) $(PARSER).o -lfl

$(PARSER).o: $(PARSER).c
	gcc $(FLAGS) -c $(PARSER).c

$(PARSER).c: $(PARSER).flex
	flex -o $(PARSER).c $(PARSER).flex 

# actions

clean:
	@rm -f $(EXEC) $(LIBSHMEM) $(OBJECTS_LIB) $(OBJECTS_EXEC) $(TESTDIR)/$(BENCHPROG) $(PARSER) $(PARSER).c $(PARSER).o
	@find . -name '*.dSYM' -exec @rm -Rf {} \;

bench: $(EXEC) $(LIBSHMEM)  $(TESTDIR)/$(BENCHPROG).c
	./$(COMPILER) $(SHCCFLAGS) -o $(TESTDIR)/$(BENCHPROG) $(TESTDIR)/$(BENCHPROG).c -lrt -lm
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:. ; ./$(EXEC) -n 2 $(TESTDIR)/$(BENCHPROG) 20 33554432

