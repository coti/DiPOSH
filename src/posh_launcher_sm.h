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

#ifndef _LAUNCHER_SM_H_
#define _LAUNCHER_SM_H_

#include <cstdlib>
#include <iostream>
#include "posh_launcher.h"

extern char VAR_PE_NUM[];
extern char VAR_NUM_PE[];

bool isLocal( void );

class Launcher_SM : public Launcher_t {
    const rte_type_t type = RTE_SM;
 public:
    int getRank(){
        char* myNum;
        myNum = std::getenv( VAR_PE_NUM );
        if( NULL == myNum ) {
            std::cerr << "Could not retrieve my PE number" << std::endl;
            return -1;
        }
        return atoi( myNum );
    }
    int getSize(){
        char* myNum;
        myNum = std::getenv( VAR_NUM_PE );
        if( NULL == myNum ) {
            std::cerr << "Could not retrieve my PE number" << std::endl;
            return -1;
        }
        return atoi( myNum );
    }
    rte_type_t getType( ) {
        return this->type;
    }
    bool isDistributed() { return false; }
};

#endif //_LAUNCHER_SM_H_
