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

#ifndef _LAUNCHER_H_
#define _LAUNCHER_H_

typedef enum { RTE_SM, RTE_MPI, RTE_PADICO } rte_type_t ;

class Launcher_t {
 public:
    virtual rte_type_t getType( void ) = 0;
    virtual int getRank( void ) = 0;
    virtual int getSize( void ) = 0;
    virtual bool isDistributed( void ) = 0;
};

#endif // _LAUNCHER_H_
