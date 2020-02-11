#
#
# Copyright (c) 2014-2019 LIPN - Universite Paris 13
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

AC_INIT([POSH: Paris OpenSHMEM],[0.2],[camille.coti@lipn.univ-paris13.fr])
AM_INIT_AUTOMAKE([-Wall])
AC_CONFIG_MACRO_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AM_PROG_AR
LT_INIT
AC_ENABLE_SHARED

# CHECK FOR PROGRAMS

AC_PROG_CXX

# SANITY CHECK

AC_CONFIG_SRCDIR([src/shmem.h])
AC_PREREQ([2.61]) # CC: not sure about the exact minimal number
AC_LANG([C++])

# CHECK FOR EXTERNAL LIBRARIES

LIBS="$LIBS"

# CHECK FOR FLEX (required to compile the source-to-source precompiler)

AC_PROG_LEX
if test "x$LEX" != xflex; then
   LEX="$SHELL $missing_dir/missing flex"
   AC_SUBST([LEX_OUTPUT_ROOT], [lex.yy])
   AC_SUBST([LEXLIB], [''])
fi

#################################################################################
# Boost
#################################################################################

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
       CPPFLAGS="-I${BOOSTPATH}/include ${CPPFLAGS}"
       LDFLAGS="-L${BOOSTPATH}/lib ${LDFLAGS}"
fi

#AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([boost/interprocess/managed_shared_memory.hpp], [],
    [AC_MSG_ERROR(You need the Boost Interprocess libraries.)])
AC_CHECK_HEADERS([boost/thread.hpp], [],
    [AC_MSG_ERROR(You need the Boost Thread libraries.)])
AC_CHECK_HEADERS([boost/bind.hpp], [],
    [AC_MSG_ERROR(You need the Boost Bind libraries.)])
AC_CHECK_HEADERS([boost/system/error_code.hpp], [],
    [AC_MSG_ERROR(You need the Boost System libraries.)])
#AC_LANG_POP([C++])

AX_PTHREAD([], [AC_MSG_ERROR([Missing pthread library])])

# Check only boost_thread, assume optimistically that the other ones will be with it
AX_CXX_CHECK_LIB(boost_thread, boost::thread, [], [
    AX_CXX_CHECK_LIB(boost_thread-mt, boost::thread, [], [
        echo "Error: POSH needs some Boost libraries: boost_thread or boost_thread-mt, boost_system and boost_program_options. Check the path with --with-boost"
	exit -1
    ])
])

CPPFLAGS=" ${CPPFLAGS} -Wall -Wno-unused-variable -Wno-deprecated-declarations -fPIC -D_GNU_SOURCE -Wno-unused-function  -Wno-deprecated-register"
LDFLAGS=" ${LDFLAGS}  -lboost_system -lpthread  -lboost_program_options "

#################################################################################
# flex
#################################################################################

#AC_CHECK_PROG([FLEX], [flex lex], [flex])
#AM_MISSING_PROG([FLEX], [$FLEX])
AC_PROG_LEX

# gnu++11
AX_CXX_COMPILE_STDCXX_11
CPPFLAGS="${CPPFLAGS} -std=gnu++11 "

#################################################################################
# DEBUGGING MODE
#################################################################################

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
  CPPFLAGS=" ${CPPFLAGS}"
  LDFLAGS=" ${LDFLAGS}"
fi

#################################################################################
# SAFE MODE
#################################################################################

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
  CPPFLAGS="-D_SAFE ${CPPFLAGS} "
fi

#################################################################################
# WHICH RTE
#################################################################################

## TODO disable it

AC_ARG_ENABLE([launcher-local],
	AS_HELP_STRING([--enable-launcher-local], [Enable local, single-node launcher [default=yes]]),
	[ AM_CONDITIONAL(ENABLE_LAUNCHERLOCAL, true)
       	CPPFLAGS+=" -DENABLE_LAUNCHER_LOCAL "],
	[ AM_CONDITIONAL(ENABLE_LAUNCHERLOCAL, true) ] )

AC_ARG_ENABLE([launcher-mpi],
	AS_HELP_STRING([--enable-launcher-mpi], [Enable MPI launcher [default=disabled]]),
	[ AM_CONDITIONAL(ENABLE_LAUNCHERMPI, true)
          CPPFLAGS+="-DMPILAUNCHER  -DDISTRIBUTED_POSH "
	  ACX_MPI( [ CC=mpicc ; CXX=mpic++ ; LD=mpic++ ],
		   [ AC_MSG_FAILURE( MPI is required ) ] )
      	],
	[ AM_CONDITIONAL(ENABLE_LAUNCHERMPI, false) ] )

# TODO check the presence of PadicoTM

AC_ARG_ENABLE([launcher-padico],
	AS_HELP_STRING([--enable-launcher-padico], [Enable PadicoTM launcher [default=disabled]]),
	[ AM_CONDITIONAL(ENABLE_LAUNCHERPADICO, true)
          CPPFLAGS+="-DPADICOLAUNCHER  -DDISTRIBUTED_POSH "
       	],
	[ AM_CONDITIONAL(ENABLE_LAUNCHERPADICO, false) ] )

#################################################################################
# CHOICE OF MEMCPY
#################################################################################

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

CPPFLAGS=" -D${MEMCPYTYPE} ${CPPFLAGS} "

#################################################################################
# CHOICE OF COMMUNICATION CHANNEL
#################################################################################

# TODO
# verifier que les dependances sont la
# verifier qu'on n'a pas d'incoherence entre RTE et comm channel

#AC_ARG_ENABLE([commsm],
#	AS_HELP_STRING([--enable-sm-comm], [Enable local shared memory communications (strongly recommanded) [default=yes]]),
#	[],
#	[enable_commsm=yes] )

AC_ARG_ENABLE([knem-comm],
	AS_HELP_STRING([--enable-knem-comm], [Enable local KNEM communications [default=no]]),
	[ AM_CONDITIONAL(ENABLE_COMM_KNEM, true)
       	  CPPFLAGS+=" -D_WITH_KNEM "],
	[ AM_CONDITIONAL(ENABLE_COMM_KNEM, false) ] )

AC_ARG_ENABLE([tcp-comm],
	AS_HELP_STRING([--enable-tcp-comm], [Enable TCP communications (slow) [default=no]]),
	[ AM_CONDITIONAL(ENABLE_COMM_TCP, true)
       	  CPPFLAGS+=" -DDISTRIBUTED_POSH  "],
	[ AM_CONDITIONAL(ENABLE_COMM_TCP, false) ] )

AC_ARG_ENABLE([mpi-comm],
	AS_HELP_STRING([--enable-mpi-comm], [Enable MPI communications [default=no]]),
	[ AM_CONDITIONAL(ENABLE_COMM_MPI, true)
       	  CPPFLAGS+=" -DMPICHANNEL -DDISTRIBUTED_POSH "
	  ACX_MPI( [ CC=mpicc ; CXX=mpic++ ; LD=mpic++ ],
		   [ AC_MSG_FAILURE( MPI is required ) ] )
          ],
	[ AM_CONDITIONAL(ENABLE_COMM_MPI, false) ] )

AC_ARG_ENABLE([nmad-comm],
	AS_HELP_STRING([--enable-nmad-comm], [Enable NewMadeleine communications [default=no]]),
	[ AM_CONDITIONAL(ENABLE_COMM_NMAD, true)
       	  CPPFLAGS+="-DNMADCHANNEL `pkg-config --cflags nmad` -D_WITH_NMAD  -DDISTRIBUTED_POSH " 
          LDFLAGS += `pkg-config --libs nmad` ],
	[ AM_CONDITIONAL(ENABLE_COMM_NMAD, false) ] )

AC_ARG_ENABLE([mpi-hub],
	AS_HELP_STRING([--enable-mpi-hub], [Enable hub communications using MPI [default=no]]),
	[ AM_CONDITIONAL(ENABLE_COMM_HUB_MPI, true)
      	  CPPFLAGS+=" -DMPIHUBCHANNEL -DDISTRIBUTED_POSH "
	  ACX_MPI( [ CC=mpicc ; CXX=mpic++ ; LD=mpic++ ],
 		   [ AC_MSG_FAILURE( MPI is required ) ] )
          ],
	AM_CONDITIONAL([ENABLE_COMM_HUB_MPI], [test "x$enable_mpi_hub" != "xno"]))

#################################################################################
# CHOICE OF COLLECTIVE COMMUNICATIONS
#################################################################################

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

CPPFLAGS=" -D${COLLTYPE} ${CPPFLAGS} "

#################################################################################
# FAULT TOLERANCE
#################################################################################

# TODO: path to the checkpointing library

AC_ARG_WITH(
    faulttolerance,
    AS_HELP_STRING(
	[--with-faulttolerance=ALGORITHM],
	[Use fault tolerance: Chandy-Lamport (ALGORITHM=marker), blocking with a double barrier (ALGORITHM=doublebarrier)]
    ),
    [ faulttolerance="$withval" ]
)

AS_IF( [test "x$faulttolerance" = "xmarker"],
       [ CPPFLAGS+=" -DCHANDYLAMPORT -DCL_MARKER " ])
AS_IF( [test "x$faulttolerance" = "xdoublebarrier"],
       [ CPPFLAGS+=" -DCHANDYLAMPORT -DCL_DOUBLEBARRIER " ])


#################################################################################
# PERFORMANCE PROFILING
#################################################################################

# TODO

#################################################################################
# END
#################################################################################

AC_PROG_INSTALL
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
        Makefile
        src/Makefile
])
AC_OUTPUT

