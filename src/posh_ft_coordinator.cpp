/*
 * Copyright (c) 2014-2019 LIPN - Universite Paris 13
 *                    All rights reserved.
 *
 * This file is part of POSH.
 * 
 * POSH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * POSH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with POSH.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstring>


#include "posh_checkpointing.h"

extern char **environ;

void cl_start_coordinator() {

    int pid;
#ifdef _DEBUG
        std::cout << "Starting the coordinator" << std::endl;
#endif

    switch( pid = fork() ) {
    case -1:
        std::perror( "Failed to start the checkpoint coordinator: " );
        break;
    case 0:
        char* arg[3];
        arg[0] = _POSH_DMTCPSTARTCOORD;
        arg[1] = _POSH_DMTCP_OPTION_QUIET;
        arg[2] = NULL;
        execve( arg[0], arg, environ );
#ifdef _DEBUG
        std::cerr << "Starting the coordinator: execve returned. errno = " << errno << std::endl;
        std::cerr << std::strerror( errno ) << std::endl;
#endif        
        break;
    default:
        break;
    }
            
#ifdef _DEBUG
        std::cout << "Coordinator spawned" << std::endl;
#endif
}

