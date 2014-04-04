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

#ifndef _SHMEM_REDUCE_H
#define _SHMEM_REDUCE_H

template <class T>void shmem_template_and_to_all( T*, T*, int,  int, int, int, T*, long* );
template <class T>void shmem_template_or_to_all( T*, T*, int,  int, int, int, T*, long* );
template <class T>void shmem_template_xor_to_all( T*, T*, int,  int, int, int, T*, long* );
template <class T>void shmem_template_max_to_all( T*, T*, int,  int, int, int, T*, long* );
template <class T>void shmem_template_min_to_all( T*, T*, int,  int, int, int, T*, long* );
template <class T, class V> void shmem_template_sum_to_all( T*, T*, int,  int, int, int, V*, long* );
template <class T, class V>void shmem_template_prod_to_all( T*, T*, int,  int, int, int, V*, long*);

template <class T, class V>void shmem_template_binarytree_reduce( T*, T*, int,  int, int, int, V*, long*, void(*)( T*, T*, T* ) );
template <class T, class V>void shmem_template_flat_reduce_put( T*, T*, int,  int, int, int, V*, long*, void(*)( T*, T*, T* ) );
template <class T, class V>void shmem_template_flat_reduce_get( T*, T*, int,  int, int, int, V*, long*, void(*)( T*, T*, T* ) );

template <class T>bool shmem_template_get_and_operation( T*, T*, int, int, T*, void(* )( T*, T*, T* ) );

template <class T>void shmem_template_and_to_all( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_and_flat_get( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_and_flat_put( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_and_binary( T*, T*, int,  int, int , int, T*, long* );

template <class T>void shmem_template_or_to_all( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_or_flat_get( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_or_flat_put( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_or_binary( T*, T*, int,  int, int , int, T*, long* );

template <class T>void shmem_template_xor_to_all( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_xor_flat_get( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_xor_flat_put( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_xor_binary( T*, T*, int,  int, int , int, T*, long* );

template <class T>void shmem_template_max_to_all( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_max_flat_get( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_max_flat_put( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_max_binary( T*, T*, int,  int, int , int, T*, long* );

template <class T>void shmem_template_min_to_all( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_min_flat_get( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_min_flat_put( T*, T*, int,  int, int , int, T*, long* );
template <class T>void shmem_template_min_binary( T*, T*, int,  int, int , int, T*, long* );

template <class T, class V> void shmem_template_sum_to_all( T*, T*, int,  int, int , int, V*, long* );
template <class T, class V>void shmem_template_sum_flat_get( T*, T*, int,  int, int , int, V*, long* );
template <class T, class V>void shmem_template_sum_flat_put( T*, T*, int,  int, int , int, V*, long* );
template <class T, class V>void shmem_template_sum_binary( T*, T*, int,  int, int , int, V*, long* );

template <class T, class V>void shmem_template_prod_to_all( T*, T*, int,  int, int , int, V*, long* );
template <class T, class V>void shmem_template_prod_flat_get( T*, T*, int,  int, int , int, V*, long* );
template <class T, class V>void shmem_template_prod_flat_put( T*, T*, int,  int, int , int, V*, long* );
template <class T, class V>void shmem_template_prod_binary( T*, T*, int,  int, int , int, V*, long* );

template <class T> void _shmem_operation_template_and( T*, T*, T* );
template <class T> void _shmem_operation_template_or( T*, T*, T* );
template <class T> void _shmem_operation_template_xor( T*, T*, T* );
template <class T> void _shmem_operation_template_max( T*, T*, T* );
template <class T> void _shmem_operation_template_min( T*, T*, T* );
template <class T> void _shmem_operation_template_sum( T*, T*, T* );
template <class T> void _shmem_operation_template_prod( T*, T*, T* );


#include "shmem_reduce.tpp"

#endif
