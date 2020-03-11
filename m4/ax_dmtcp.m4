#
#
# Copyright (c) 2020 LIPN - Universite Sorbonne Paris Nord
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

AC_DEFUN([AX_DMTCP_HOME],[
    AC_ARG_WITH([dmtcp],
                AS_HELP_STRING([--with-dmtcp@<:@=DIR@:>@], [DMTCP root directory]),
                [DMTCP_HOME="$withval"
                 AS_IF([test "$DMTCP_HOME" = "yes"],
                  AC_CHECK_HEADER(dmtcp.h,[DMTCP_HOME="/usr"],AC_MSG_ERROR([could not find dmtcp.h])),
                  AC_CHECK_HEADER([$DMTCP_HOME/include/dmtcp.h],
                               [ CPPFLAGS+="-I$DMTCP_HOME/include" ; LDFLAGS+="-L$DMTCP_HOME/lib"  ],
                               AC_MSG_ERROR([could not find $DMTCP_HOME/include/dmtcp.h])
                              )
                )],
                [DMTCP_HOME=""]
               )
    AC_SUBST([DMTCP_HOME])
])

