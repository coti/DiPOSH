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

#include <sys/types.h>
#include <fstream>

#include "shmem_utils.h"

/* Get the command-line arguments from anywhere 
   Necessary because OpenSHMEM's initialization does not take them as arguments
*/

void getArguments( int* argc, char*** argv ) {
    int pid = getpid();
    char filename[256];
    snprintf( filename, sizeof( filename ), "/proc/%d/cmdline", pid );
    std::string line;
    std::ifstream myfile( filename );
    std::vector<std::string> result; 
    
    if( myfile.is_open() ) {
        while ( std::getline( myfile, line, '\0' ) ) {
            result.push_back( line );
        }
        myfile.close();
    }
    *argc = result.size();
    *argv = (char**) malloc( ( *argc + 1 ) * sizeof( char* ) );
    for( unsigned int i = 0; i < result.size(); i++) {
        asprintf( &((*argv)[i]), "%s", result[i].c_str() );
                                     
    }
    (*argv)[*argc] = NULL;
    
}
