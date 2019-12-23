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

#ifndef _LAUNCHER_MPI_H_
#define _LAUNCHER_MPI_H_

#include <mpi.h>
#include "posh_launcher.h"

bool isMPI( void );

class Launcher_MPI : public Launcher_t {
    const rte_type_t type = RTE_MPI;
 public:
    int getRank(){
        int rank;
        MPI_Comm_rank( MPI_COMM_WORLD, &rank );
        return rank;
    }
    int getSize(){
        int size;
        MPI_Comm_size( MPI_COMM_WORLD, &size );
        return size;
    }
    rte_type_t getType( ) {
        return this->type;
    }
    bool isDistributed() { return true; }
};

#endif //_LAUNCHER_MPI_H_
