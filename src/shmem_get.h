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

#ifndef _SHMEM_GET_H
#define _SHMEM_GET_H

#include <stdlib.h>
#include <cstddef> // ptrdiff_t
#include <cstdint> // *intptr_t

template<class T> T shmem_template_g( T*, int );
template<class T> void shmem_template_get( T*, const T*, size_t, int);
template<class T> void shmem_template_iget( T*, const T*,  ptrdiff_t, ptrdiff_t, size_t, int );

void shmem_intptr_get( intptr_t*, const intptr_t*, size_t, int );
void shmem_uintptr_get( uintptr_t*, const uintptr_t*, size_t, int );


#include "shmem_get.tpp"

#endif
