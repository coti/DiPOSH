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

#ifndef _SHMEM_COLLECTIVES_H
#define _SHMEM_COLLECTIVES_H

template<class T>void shmem_template_allgather_flat( T*, const T*, size_t );
template<class T>void shmem_template_gather_flat( T*, const T*, size_t, int );

void shmem_int_allgather_flat( int*, const int*, size_t );
void shmem_int_gather_flat( int*, const int*, size_t, int );

#include "shmem_collectives.tpp"

#endif
