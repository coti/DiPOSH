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

#ifndef _SHMEM_PUT_H
#define _SHMEM_PUT_H

#include <stdlib.h>
#include <cstddef> // ptrdiff_t

template<class T> void shmem_template_p( T*, T, int );
template<class T> void shmem_template_put( T*, const T*, size_t, int );
template<class T> void shmem_template_iput( T*, const T*, ptrdiff_t, ptrdiff_t, size_t, int );

#include "shmem_put.tpp"

#endif
