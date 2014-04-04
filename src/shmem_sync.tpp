/*
 *
 * Copyright (c) 2014 LIPN - Universite Paris 13
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
 *
 */

#include "shmem_constants.h"

#include <unistd.h>
#include <iostream>

template<class T> void shmem_template_wait( T* var, T value ){

    /* Spin-lock */

    while( *var != value ) {
        usleep( SPIN_TIMER );
    }
}

template<class T> void shmem_template_wait_until( T* var, int cmp, T value ){

    while( true ) {
        switch( cmp ) {
        case SHMEM_CMP_EQ:
            if( *var == value ) return; 
        case SHMEM_CMP_NE:
            if( *var != value ) return;
        case SHMEM_CMP_GT:
            if( *var > value ) return;
        case SHMEM_CMP_LE:
            if( *var <= value ) return;
        case SHMEM_CMP_LT:
            if( *var < value ) return;
        default:
            std::cerr << "Wrong compare operator: " << cmp << std::endl;
            return;
        }
        usleep( SPIN_TIMER );
    }
}
