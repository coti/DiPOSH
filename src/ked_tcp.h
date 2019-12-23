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

#ifndef _KED_TCP_H_
#define _KED_TCP_H_

#include "posh_neighbor.h"

/* Constants */

#define _MY_PORT_ 5000
#define BACKLOG 10
#define NP 16

/* Structures required by the mini-protocol */

typedef enum {GET, PUT, SHMALLOC} comm_type_t;

typedef struct {
    comm_type_t comm_type;
    size_t address;
    size_t size;
} ked_header_t;

/* Prototypes */

size_t ked_put_tcp( int, void*, const void*, size_t );
size_t ked_get_tcp( int, void*, const void*, size_t );
size_t ked_remote_fake_shmalloc_tcp( int, size_t );
void* ked_thread_comm( void );
int ked_connect( Neighbor_t* );

#endif
