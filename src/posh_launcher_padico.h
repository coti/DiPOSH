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

#ifndef _LAUNCHER_PADICO_H_
#define _LAUNCHER_PADICO_H_

#include "shmem_utils.h" // for getArguments
extern "C" {
#include <nm_launcher_interface.h>
}

#define INIT_TAG 0x8

/* Have I been launched by Padico */

bool isPadico( void );

class Launcher_Padico : public Launcher_t {
const rte_type_t type = RTE_PADICO;
nm_session_t p_session = NULL;
nm_onesided_t os_session = NULL;

 public:
    nm_comm_t comm_world;
    
    Launcher_Padico(){
        int __myargc;
        char **__myargv;
        getArguments( &__myargc, &__myargv );                   
        nm_launcher_init( &__myargc, __myargv );
        this->comm_world = nm_comm_world("init");
        nm_launcher_session_open( &(this->p_session), "nm_posh" );
        nm_onesided_init( this->p_session, &(this->os_session) );

        myInfo.getMyEndpointNMAD()->setOsSession( this->os_session );
    }
    int getRank(){
        int rank;
        nm_launcher_get_rank( &rank );
        return rank;
    }
    int getSize(){
        int size;
        nm_launcher_get_size( &size );
        return size;
    }
    rte_type_t getType( ) {
        return this->type;
}
bool isDistributed(){ return true; }
};

#endif //_LAUNCHER_Padico_H_
