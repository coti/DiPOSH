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

#include <iostream>

void _shmem_operation_and_unsigned_char( unsigned char* res, unsigned char* buff1, unsigned char* buff2 ) {
     *res = ( *buff1 & *buff2 );
 }
void _shmem_operation_or_unsigned_char( unsigned char* res, unsigned char* buff1, unsigned char* buff2 ) {
     *res = ( *buff1 | *buff2 );
 }
void _shmem_operation_xor_unsigned_char( unsigned char* res, unsigned char* buff1, unsigned char* buff2 ) {
     *res = ( *buff1 ^ *buff2 );
 }
void _shmem_operation_max_unsigned_char( unsigned char* res, unsigned char* buff1, unsigned char* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_min_unsigned_char( unsigned char* res, unsigned char* buff1, unsigned char* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_sum_unsigned_char( unsigned char* res, unsigned char* buff1, unsigned char* buff2 ) {
     *res = ( *buff1 + *buff2);
 }
void _shmem_operation_prod_unsigned_char( unsigned char* res, unsigned char* buff1, unsigned char* buff2 ) {
     *res = ( *buff1 * *buff2);
 }
void _shmem_operation_and_short( short* res, short* buff1, short* buff2 ) {
     *res = ( *buff1 & *buff2 );
 }
void _shmem_operation_or_short( short* res, short* buff1, short* buff2 ) {
     *res = ( *buff1 | *buff2 );
 }
void _shmem_operation_xor_short( short* res, short* buff1, short* buff2 ) {
     *res = ( *buff1 ^ *buff2 );
 }
void _shmem_operation_max_short( short* res, short* buff1, short* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_min_short( short* res, short* buff1, short* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_sum_short( short* res, short* buff1, short* buff2 ) {
     *res = ( *buff1 + *buff2);
 }
void _shmem_operation_prod_short( short* res, short* buff1, short* buff2 ) {
     *res = ( *buff1 * *buff2);
 }
void _shmem_operation_and_unsigned_short( unsigned short* res, unsigned short* buff1, unsigned short* buff2 ) {
     *res = ( *buff1 & *buff2 );
 }
void _shmem_operation_or_unsigned_short( unsigned short* res, unsigned short* buff1, unsigned short* buff2 ) {
     *res = ( *buff1 | *buff2 );
 }
void _shmem_operation_xor_unsigned_short( unsigned short* res, unsigned short* buff1, unsigned short* buff2 ) {
     *res = ( *buff1 ^ *buff2 );
 }
void _shmem_operation_max_unsigned_short( unsigned short* res, unsigned short* buff1, unsigned short* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_min_unsigned_short( unsigned short* res, unsigned short* buff1, unsigned short* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_sum_unsigned_short( unsigned short* res, unsigned short* buff1, unsigned short* buff2 ) {
     *res = ( *buff1 + *buff2);
 }
void _shmem_operation_prod_unsigned_short( unsigned short* res, unsigned short* buff1, unsigned short* buff2 ) {
     *res = ( *buff1 * *buff2);
 }
void _shmem_operation_and_int( int* res, int* buff1, int* buff2 ) {
     *res = ( *buff1 & *buff2 );
 }
void _shmem_operation_or_int( int* res, int* buff1, int* buff2 ) {
     *res = ( *buff1 | *buff2 );
 }
void _shmem_operation_xor_int( int* res, int* buff1, int* buff2 ) {
     *res = ( *buff1 ^ *buff2 );
 }
void _shmem_operation_max_int( int* res, int* buff1, int* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_min_int( int* res, int* buff1, int* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_sum_int( int* res, int* buff1, int* buff2 ) {
     *res = ( *buff1 + *buff2);
 }
void _shmem_operation_prod_int( int* res, int* buff1, int* buff2 ) {
     *res = ( *buff1 * *buff2);
 }
void _shmem_operation_and_unsigned_int( unsigned int* res, unsigned int* buff1, unsigned int* buff2 ) {
     *res = ( *buff1 & *buff2 );
 }
void _shmem_operation_or_unsigned_int( unsigned int* res, unsigned int* buff1, unsigned int* buff2 ) {
     *res = ( *buff1 | *buff2 );
 }
void _shmem_operation_xor_unsigned_int( unsigned int* res, unsigned int* buff1, unsigned int* buff2 ) {
     *res = ( *buff1 ^ *buff2 );
 }
void _shmem_operation_max_unsigned_int( unsigned int* res, unsigned int* buff1, unsigned int* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_min_unsigned_int( unsigned int* res, unsigned int* buff1, unsigned int* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_sum_unsigned_int( unsigned int* res, unsigned int* buff1, unsigned int* buff2 ) {
     *res = ( *buff1 + *buff2);
 }
void _shmem_operation_prod_unsigned_int( unsigned int* res, unsigned int* buff1, unsigned int* buff2 ) {
     *res = ( *buff1 * *buff2);
 }
void _shmem_operation_and_long( long* res, long* buff1, long* buff2 ) {
     *res = ( *buff1 & *buff2 );
 }
void _shmem_operation_or_long( long* res, long* buff1, long* buff2 ) {
     *res = ( *buff1 | *buff2 );
 }
void _shmem_operation_xor_long( long* res, long* buff1, long* buff2 ) {
     *res = ( *buff1 ^ *buff2 );
 }
void _shmem_operation_max_long( long* res, long* buff1, long* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_min_long( long* res, long* buff1, long* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_sum_long( long* res, long* buff1, long* buff2 ) {
     *res = ( *buff1 + *buff2);
 }
void _shmem_operation_prod_long( long* res, long* buff1, long* buff2 ) {
     *res = ( *buff1 * *buff2);
 }
void _shmem_operation_and_unsigned_long( unsigned long* res, unsigned long* buff1, unsigned long* buff2 ) {
     *res = ( *buff1 & *buff2 );
 }
void _shmem_operation_or_unsigned_long( unsigned long* res, unsigned long* buff1, unsigned long* buff2 ) {
     *res = ( *buff1 | *buff2 );
 }
void _shmem_operation_xor_unsigned_long( unsigned long* res, unsigned long* buff1, unsigned long* buff2 ) {
     *res = ( *buff1 ^ *buff2 );
 }
void _shmem_operation_max_unsigned_long( unsigned long* res, unsigned long* buff1, unsigned long* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_min_unsigned_long( unsigned long* res, unsigned long* buff1, unsigned long* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_sum_unsigned_long( unsigned long* res, unsigned long* buff1, unsigned long* buff2 ) {
     *res = ( *buff1 + *buff2);
 }
void _shmem_operation_prod_unsigned_long( unsigned long* res, unsigned long* buff1, unsigned long* buff2 ) {
     *res = ( *buff1 * *buff2);
 }
void _shmem_operation_and_long_long( long long* res, long long* buff1, long long* buff2 ) {
     *res = ( *buff1 & *buff2 );
 }
void _shmem_operation_or_long_long( long long* res, long long* buff1, long long* buff2 ) {
     *res = ( *buff1 | *buff2 );
 }
void _shmem_operation_xor_long_long( long long* res, long long* buff1, long long* buff2 ) {
     *res = ( *buff1 ^ *buff2 );
 }
void _shmem_operation_max_long_long( long long* res, long long* buff1, long long* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_min_long_long( long long* res, long long* buff1, long long* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_sum_long_long( long long* res, long long* buff1, long long* buff2 ) {
     *res = ( *buff1 + *buff2);
 }
void _shmem_operation_prod_long_long( long long* res, long long* buff1, long long* buff2 ) {
     *res = ( *buff1 * *buff2);
 }
void _shmem_operation_and_unsigned_long_long( unsigned long long* res, unsigned long long* buff1, unsigned long long* buff2 ) {
     *res = ( *buff1 & *buff2 );
 }
void _shmem_operation_or_unsigned_long_long( unsigned long long* res, unsigned long long* buff1, unsigned long long* buff2 ) {
     *res = ( *buff1 | *buff2 );
 }
void _shmem_operation_xor_unsigned_long_long( unsigned long long* res, unsigned long long* buff1, unsigned long long* buff2 ) {
     *res = ( *buff1 ^ *buff2 );
 }
void _shmem_operation_max_unsigned_long_long( unsigned long long* res, unsigned long long* buff1, unsigned long long* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_min_unsigned_long_long( unsigned long long* res, unsigned long long* buff1, unsigned long long* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_sum_unsigned_long_long( unsigned long long* res, unsigned long long* buff1, unsigned long long* buff2 ) {
     *res = ( *buff1 + *buff2);
 }
void _shmem_operation_prod_unsigned_long_long( unsigned long long* res, unsigned long long* buff1, unsigned long long* buff2 ) {
     *res = ( *buff1 * *buff2);
 }
void _shmem_operation_and_float( float* res, float* buff1, float* buff2 ) {
    /* Invalid */
    std::cerr << "ERROR - called AND on floats, this is not part of the standard" << std::endl;
     *res = 0;
 }
void _shmem_operation_or_float( float* res, float* buff1, float* buff2 ) {
    /* Invalid */
    std::cerr << "ERROR - called OR on floats, this is not part of the standard" << std::endl;
    *res = 0;
 }
void _shmem_operation_xor_float( float* res, float* buff1, float* buff2 ) {
    /* Invalid */
    std::cerr << "ERROR - called XOR on floats, this is not part of the standard" << std::endl;
    *res = 0;
 }
void _shmem_operation_max_float( float* res, float* buff1, float* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_min_float( float* res, float* buff1, float* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_sum_float( float* res, float* buff1, float* buff2 ) {
     *res = ( *buff1 + *buff2);
 }
void _shmem_operation_prod_float( float* res, float* buff1, float* buff2 ) {
     *res = ( *buff1 * *buff2);
 }
void _shmem_operation_and_double( double* res, double* buff1, double* buff2 ) {
    /* Invalid */
    std::cerr << "ERROR - called AND on doubles, this is not part of the standard" << std::endl;
     *res = 0;
 }
void _shmem_operation_or_double( double* res, double* buff1, double* buff2 ) {
    /* Invalid */
    std::cerr << "ERROR - called OR on doubles, this is not part of the standard" << std::endl;
     *res = 0;
 }
void _shmem_operation_xor_double( double* res, double* buff1, double* buff2 ) {
    /* Invalid */
    std::cerr << "ERROR - called XOR on doubles, this is not part of the standard" << std::endl;
     *res = 0;
 }
void _shmem_operation_max_double( double* res, double* buff1, double* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_min_double( double* res, double* buff1, double* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_sum_double( double* res, double* buff1, double* buff2 ) {
     *res = ( *buff1 + *buff2);
 }
void _shmem_operation_prod_double( double* res, double* buff1, double* buff2 ) {
     *res = ( *buff1 * *buff2);
 }
void _shmem_operation_and_long_double( long double* res, long double* buff1, long double* buff2 ) {
    /* Invalid */
    std::cerr << "ERROR - called AND on long doubles, this is not part of the standard" << std::endl;
     *res = 0;
 }
void _shmem_operation_or_long_double( long double* res, long double* buff1, long double* buff2 ) {
    /* Invalid */
    std::cerr << "ERROR - called OR on long doubles, this is not part of the standard" << std::endl;
     *res = 0;
 }
void _shmem_operation_xor_long_double( long double* res, long double* buff1, long double* buff2 ) {
    /* Invalid */
    std::cerr << "ERROR - called XOR on long doubles, this is not part of the standard" << std::endl;
     *res = 0;
 }
void _shmem_operation_max_long_double( long double* res, long double* buff1, long double* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_min_long_double( long double* res, long double* buff1, long double* buff2 ) {
     *res = ( *buff1 > *buff2 ? *buff1 : *buff2 );
 }
void _shmem_operation_sum_long_double( long double* res, long double* buff1, long double* buff2 ) {
     *res = ( *buff1 + *buff2);
 }
void _shmem_operation_prod_long_double( long double* res, long double* buff1, long double* buff2 ) {
     *res = ( *buff1 * *buff2);
 }

