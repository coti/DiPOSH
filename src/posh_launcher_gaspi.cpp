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

#include "posh_launcher_gaspi.h"

#define GPI_ENV_VARIABLE "GASPI_NRANKS"

bool isGASPI(){

  char* size;
  size = std::getenv( GPI_ENV_VARIABLE );
  if( NULL != size ){
    return true;
  }

  return false;
}
