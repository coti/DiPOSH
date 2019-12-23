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

#include "perf.h"
#include <dirent.h>
#include <ostream>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

profile_functions_t function_prof_info;

/* routines called by the user */

void posh_output_profiling_info( void ){
    function_prof_info.output();
}


/* internal routines */

std::ostream& operator << ( std::ostream &out, stats_all_t& stats ){
    /* std::cout << "numa hit         " << stats.numa_hit << std::endl;
    std::cout << "numa miss        " << stats.numa_miss << std::endl;
    std::cout << "numa foreign     " << stats.numa_foreign << std::endl;
    std::cout << "local node       " << stats.local_node << std::endl;
    std::cout << "other node       " << stats.other_node << std::endl;
    std::cout << "interleave hit   " << stats.interleave_hit << std::endl;

    std::cout << " -- in the map:"  << std::endl;*/
    
    for( auto& kv : stats.statsdico ) {
        std::cout <<  std::setw ( 20 )  << kv.first << " : " << std::setw ( 20 )  << kv.second << std::endl;
    }
    
    return out;
}

std::ostream& operator<<( std::ostream& out, profile_functions_t& stats ){
    for( auto& kv : stats.functionsdico ) {
        std::cout <<  std::setw ( 20 )  << kv.first << " : " << std::setw ( 20 )  << kv.second << std::endl;
    }   
    return out;
}

stats_all_t operator-( stats_all_t left, const stats_all_t& right){
    for( auto& kv : right.statsdico ) {
        left.statsdico[ kv.first ] -= kv.second;
    }
    left.numa_hit -= right.numa_hit;
    left.numa_miss -= right.numa_miss;
    left.numa_foreign -= right.numa_foreign;
    left.local_node -= right.local_node;
    left.other_node -= right.other_node;
    left.interleave_hit -= right.interleave_hit;
    return left;
}



/* What follows is very inspired from the numastat source code */

int node_and_digits(const struct dirent *dptr) {
	char *p = (char *)(dptr->d_name);
	if (*p++ != 'n') return 0;
	if (*p++ != 'o') return 0;
	if (*p++ != 'd') return 0;
	if (*p++ != 'e') return 0;
	do {
		if (!isdigit(*p++)) return 0;
	} while (*p != '\0');
	return 1;
}

#define NUMASTATFILE "numastat"
#define SMALL_BUF_SIZE 64

int posh_get_machine_numa_stats( stats_all_t& stats ){

    int nn;
    int num_nodes;
    char fname[64];
    char buf[SMALL_BUF_SIZE];
    std::string fieldname;
    long long fieldvalue;
    
	struct dirent **namelist;
    
    num_nodes = scandir("/sys/devices/system/node", &namelist, node_and_digits, NULL );
	if( num_nodes < 1 ) {
        perror("Couldn't open /sys/devices/system/node");
        return -1;
    }
    
    /* open the info file from all the nodes */

    for( nn = 0 ; nn < num_nodes ; nn++ ) {
        snprintf( fname, sizeof(fname), "/sys/devices/system/node/node%d/%s", nn, NUMASTATFILE );
        std::string line;
        std::ifstream myfile( fname );
        if( myfile.is_open() ) {
            while ( std::getline( myfile, line ) ) {
                fieldname =  line.substr( 0, line.find_first_of(" ") );
                fieldvalue = std::stoll( line.substr( line.find_first_of(" ") + 1 ) );
                if( stats.statsdico.count( fieldname ) > 0 ) {
                    stats.statsdico[fieldname] += fieldvalue;
                } else {
                    stats.statsdico[fieldname] = fieldvalue;
                }
            }
            myfile.close();
        } else {
            sprintf(buf, "cannot open %s", fname);
            perror(buf);
            return -1;
        }
    }

    return 0;
}
