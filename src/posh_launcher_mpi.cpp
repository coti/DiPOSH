/*
 * Copyright (c) 2014-2020 LIPN - Universite Paris 13
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

#include <cstdlib>

#include "posh_launcher_mpi.h"

#define OMPI_ENV_VARIABLE "OMPI_UNIVERSE_SIZE"
#define MPICH_ENV_VARIABLE "MPIEXEC_UNIVERSE_SIZE"

bool isMPI( void ){

    char* size;
    
    /* OpenMPI */
    
    size = std::getenv( OMPI_ENV_VARIABLE );
    if( NULL != size ) {
#if _DEBUG
        std::cout << "Started with MPI" << std::endl;
#endif
        return true;
    }

    /* MPICH (not tested) */
    
    size = std::getenv( MPICH_ENV_VARIABLE );
    if( NULL != size ) {
#if _DEBUG
        std::cout << "Started with MPI" << std::endl;
#endif
        return true;
    }
    
    /* Nothing :( */

#if _DEBUG
    std::cout << "Not started with MPI" << std::endl;
#endif
    return false;
}
