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

#ifndef _POSH_COMMUNICATION_H_
#define _POSH_COMMUNICATION_H_

#include "posh_contactinfo.h"

class Endpoint_t {
public:
    virtual void init_end( void ) = 0;
    virtual void init( void ) = 0;
    virtual ContactInfo* getMyContactInfo( void ) = 0;
};

class Communication_t {
 public:
    virtual void init( int ) = 0;
    virtual void setContactInfo( ContactInfo& ) = 0;
    virtual int posh__get(  void* , const void* , size_t , int  ) = 0;
    virtual int posh__put(  void* , const void* , size_t , int  ) = 0;
};


#endif // _POSH_COMMUNICATION_H_
