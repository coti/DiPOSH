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

# https://www.lrde.epita.fr/~adl/dl/autotools.pdf

# $ vim configure.ac
# $ libtoolize --force
# $ aclocal
# $ autoheader
# $ automake --force-missing --add-missing
# $ autoconf

# VARIOUS INITIALIZATIONS

AC_INIT([POSH: Paris OpenSHMEM],[0.2],[camille.coti@lipn.univ-paris13.fr])
AM_INIT_AUTOMAKE([-Wall])
AC_CONFIG_MACRO_DIR([config])
AM_PROG_AR
LT_INIT

# CHECK FOR PROGRAMS

AC_PROG_CXX

# SANITY CHECK

AC_CONFIG_SRCDIR([src/shmem.h])
AC_PREREQ([2.61]) # CC: not sure about the exact minimal number

# CHECK FOR EXTERNAL LIBRARIES

LIBS="$LIBS"

# Boost

AC_ARG_WITH(
    [boost],
    AS_HELP_STRING(
	[--with-boost=FULLPATH],
        [set full path to your Boost installation]
    ),
    [BOOSTPATH="$withval"]
)

if test -n "$BOOSTPATH"; then
       AC_MSG_CHECKING([user-supplied Boost installation])
       CPPFLAGS="-I${BOOSTPATH} ${CPPFLAGS}"
       LDFLAGS="-L${BOOSTPATH} ${LDFLAGS}"
fi

AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([boost/interprocess/managed_shared_memory.hpp], [],
    [AC_MSG_ERROR(You need the Boost Interprocess libraries.)])
AC_CHECK_HEADERS([boost/thread.hpp], [],
    [AC_MSG_ERROR(You need the Boost Thread libraries.)])
AC_CHECK_HEADERS([boost/bind.hpp], [],
    [AC_MSG_ERROR(You need the Boost Bind libraries.)])
AC_CHECK_HEADERS([boost/system/error_code.hpp], [],
    [AC_MSG_ERROR(You need the Boost System libraries.)])
AC_LANG_POP([C++])

CPPFLAGS="${CPPFLAGS} -Wall -Wno-unused-variable -Wno-deprecated-declarations -fPIC -D_GNU_SOURCE -std=gnu++11 -Wno-unused-function"
LDFLAGS="${LDFLAGS} -lboost_thread -lboost_system -lpthread"

# flex

AC_PROG_LEX

# gnu++11
AX_CXX_COMPILE_STDCXX_11
CPPFLAGS="${CPPFLAGS} -std=gnu++11"

# DEBUGGING MODE

AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])

if test $debug = true ; then
  CPPFLAGS="-O0 -g -D_DEBUG ${CPPFLAGS}"
  LDFLAGS="-g -Wall -O0 ${LDFLAGS}"
else
  CPPFLAGS=" -O3 ${CPPFLAGS}"
  LDFLAGS=" -O3 ${LDFLAGS}"
fi

# SAFE MODE

AC_ARG_ENABLE(safe,
AS_HELP_STRING([--enable-safe],
               [enable safe mode, default: no]),
[case "${enableval}" in
             yes) safe=true ;;
             no)  safe=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-safe]) ;;
esac],
[safe=false])

if test $safe = true ; then
  CPPFLAGS="-D_SAFE ${CPPFLAGS}"
fi

# CHOICE OF MEMCPY

MEMCPYTYPE=_USE_MEMCPY_ # Default

AC_ARG_WITH([memcpy],
    AS_HELP_STRING([--without-memcpy], [Use default memcpy implementation (stock memcpy() )]))

AC_ARG_WITH(
    memcpy,
    AS_HELP_STRING(
	[--with-memcpy=MEMCPY-TYPE],
	[Use memcpy implementation: memcpy, kernel, vectorized, ptrvec, SSE, MMX or  MMX2 ]
    ),
    [ memcpy="$withval" ]
)

AS_IF( [test "x$memcpy" = "xmemcpy"],
       [ MEMCPYTYPE=_USE_MEMCPY_ ])
AS_IF( [test "x$memcpy" = "xvectorized"],
       [ MEMCPYTYPE=_USE_VECTOR_COPY_ ])
AS_IF( [test "x$memcpy" = "xptrvec"],
       [ MEMCPYTYPE=_USE_PTR_VEC_COPY_ ])
AS_IF( [test "x$memcpy" = "xkernel"],
       [ MEMCPYTYPE=_USE_KER_COPY_ ])
AS_IF( [test "x$memcpy" = "xsse"],
       [ MEMCPYTYPE=_USE_SSE_COPY_ ])
AS_IF( [test "x$memcpy" = "xmmx"],
       [ MEMCPYTYPE=_USE_MMX_COPY_ ])
AS_IF( [test "x$memcpy" = "xmmx2"],
       [ MEMCPYTYPE=_USE_MMX2_COPY_ ])

CPPFLAGS=" -D${MEMCPYTYPE} ${CPPFLAGS}"

# CHOICE OF COLLECTIVE COMMUNICATIONS

COLLTYPE=SHMEM_COLL_BINARY  # Default

AC_ARG_WITH([collective],
    AS_HELP_STRING([--without-collective], [Use default collective communication algorithms]))

AC_ARG_WITH(
    collective,
    AS_HELP_STRING(
	[--with-collective=COLL_TYPE],
	[Use collective communication algorithms: flat or binary]
    ),
    [ colltype="$withval" ]
)

AS_IF( [test "x$colltype" = "xbinary"],
       [ COLLTYPE=SHMEM_COLL_BINARY ])
AS_IF( [test "x$colltype" = "xflat"],
       [ COLLTYPE=SHMEM_COLL_FLAT ] )

CPPFLAGS=" -D${COLLTYPE} ${CPPFLAGS}"

# END

AC_PROG_INSTALL
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
        Makefile
        src/Makefile
])
AC_OUTPUT

# MYCFLAGS = -Wall  -I$(TOOLS)/include  -Wno-unused-variable -Wno-deprecated-declarations -fPIC -D_GNU_SOURCE -std=gnu++11 -Wno-unused-function #-fno-builtin 
# MYLDFLAGS = -Wall #-fno-builtin
# ifeq ($(MODE),DEBUG)
#   CFLAGS=-O0 -g -D_DEBUG $(MYCFLAGS)
#   LDFLAGS=-g -Wall -O0 $(MYLDFLAGS)
# else
#   CFLAGS= -O3 $(MYCFLAGS)
#   LDFLAGS= -O3 $(MYLDFLAGS)
# endif
# ifeq ($(MODE),DEBUG)
#   SHCCFLAGS=-g -O0
# else
#   SHCCFLAGS=-g -O3
# endif
#
# COLLTYPE = SHMEM_COLL_BINARY # SHMEM_COLL_FLAT # 
# MEMCPYTYPE =  _USE_MEMCPY_  # _USE_SSE_MEMCPY_ #
#
# CFLAGS += -D$(COLLTYPE) -D$(MEMCPYTYPE) #-D_USE_BLOCK_MEMCPY
#
## Possible flags:
# _DEBUG : debug mode
# _SAFE : safe mode (activates run-time checks)
# NB: _DEBUG activates most of the features enabled by the _SAFE flag
# MULTITHREADED_SPAWN

# LIBS=-L/usr/local/lib -L$(TOOLS)/lib -lboost_thread -lboost_system -lpthread
