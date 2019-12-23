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

#ifndef _POSH_NEIGHBOR_H_
#define _POSH_NEIGHBOR_H_

#include <ostream>
#include <vector>
#include "posh_communication.h"
#include "posh_contactinfo.h"
#include "shmem_internal.h"

#include <boost/interprocess/managed_shared_memory.hpp>
using namespace boost::interprocess;


extern std::map< neighbor_comm_type_t, unsigned int> channel_priorities;

// typedef struct Neighbor_t {
class Neighbor_t {
 public: /* should be private etc but no time for this yet */
    std::string hostname;              /* name of the machine this neightbor is running on */
    int rank;

    //Communication_t* communications;
    /* List of all the communication channels I have with this neighbor */
    std::vector<Communication_t> channels;
    std::vector<ContactInfo> neigh_ci;

    /* My favorite one */
    Communication_t* communications;
    neighbor_comm_type_t comm_type; /* TODO ca va degager */
    
    /* methods */
    
    Neighbor_t(){
        this->hostname = "";
        //        this->comm_type = NONE;
    }
    ~Neighbor_t(){
        this->hostname.clear();
    }

};

extern std::ostream& operator<< ( std::ostream&, Neighbor_t& );

#endif // _POSH_NEIGHBOR_H_
