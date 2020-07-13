/*
 * Copyright (c) 2020 LIPN - Universite Sorbonne Paris Nord
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

#ifndef _POSH_COLLECTIVES_TPP_
#define _POSH_COLLECTIVES_TPP_

/**********************************************************************/
/*                                                                    */
/*       **************        OPERATIONS       **************        */
/*                                                                    */
/**********************************************************************/

//void(* operation)( T*, T*, T* )

template <class T> void _shmem_operation_template_and( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 & *buff2 );
 }

 template <class T> void _shmem_operation_template_or( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 | *buff2 );
 }

 template <class T> void _shmem_operation_template_xor( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 ^ *buff2 );
 }

 template <class T> void _shmem_operation_template_max( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }

 template <class T> void _shmem_operation_template_min( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }

 template <class T> void _shmem_operation_template_sum( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 + *buff2);
 }

 template <class T> void _shmem_operation_template_prod( T* res, T* buff1, T* buff2 ) {
     *res = ( *buff1 * *buff2);
 }

#endif // _POSH_COLLECTIVES_TPP_
