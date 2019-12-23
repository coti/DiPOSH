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

#include "posh_launcher_sm.h"

extern char VAR_PID_ROOT[];

bool isLocal( void ){

    char* size = getenv( VAR_PID_ROOT );
    if( NULL != size ) {
#if _DEBUG
        std::cout << "Started with a local fork" << std::endl;
#endif
        return true;
    }
    
#if _DEBUG
    std::cout << "Not started with a local fork" << std::endl;
#endif
    return false;
}
