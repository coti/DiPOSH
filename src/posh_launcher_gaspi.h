/*
 * Copyright (c) 2020 LIPN - Universite Sorbonne Paris Nord
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

#ifndef _POSH_LAUNCHER_GASPI_H_
#define _POSH_LAUNCHER_GASPI_H_

#include <cstdlib>
#include <GASPI.h>
#include <iostream>

#include "posh_launcher.h"

bool isGASPI( void );


class Launcher_GASPI : public Launcher_t {
    const rte_type_t type = RTE_GASPI;
public:
    Launcher_GASPI(){
        gaspi_proc_init( GASPI_BLOCK );
    }
    ~Launcher_GASPI(){
        gaspi_proc_term( GASPI_BLOCK );
    }
   int getRank(){
        gaspi_rank_t rank;
        gaspi_proc_rank( &rank );
        std::cout << "rank " << rank << std::endl;
        return (int)rank;
    }
    int getSize(){
        gaspi_rank_t size;
        gaspi_proc_num( &size );
        std::cout << "size " << size << std::endl;
        return (int) size;
    }
    rte_type_t getType( ) {
        return this->type;
    }
    bool isDistributed() { return true; }
};

#endif
