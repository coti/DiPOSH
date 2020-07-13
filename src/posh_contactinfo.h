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

#ifndef _POSH_CONTACTINFO_H_
#define _POSH_CONTACTINFO_H_

#include <cstring>
#include <cstdint>
#include <ostream>


#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

namespace boost {
    namespace serialization{
        class access;
    }
}

typedef uint64_t knem_cookie_t;

enum  neighbor_comm_type_t: int {TYPE_SM, TYPE_TCP, TYPE_MPI, TYPE_KNEM, TYPE_NMAD, TYPE_HUB, NONE};

/* Will be more complex when we support several types of networks */
/* TODO: we need an interface here */

class ContactInfo {
 private:
    /*friend class boost::serialization::access;

        template<class Archive>
    virtual void serialize( ){
        ;;
        }*/
protected:
    int rank;
    bool ready;
    neighbor_comm_type_t type;
    std::string hostname;
    
    virtual std::ostream& doprint( std::ostream& ) const = 0;
    virtual std::istream& doinput( std::istream& ) = 0;

public:
    virtual ~ContactInfo() = default;

    /*    virtual void setHostname( int, char* ) = 0;
    virtual void setHostname( char* ) = 0;
    virtual char* getHostname( void ) = 0;
    virtual uintptr_t getBaseAddress() = 0;
    virtual void setBaseAddress( uintptr_t )= 0; */
                              
    int getRank( void ) { return this->rank; }
    void setRank( int rank ) { this->rank = rank; }
    bool getReady( void ) { return this->ready; }
    void setReady( bool ready ) { this->ready = ready; }
    neighbor_comm_type_t getType( void ) { return this->type; }
    void setType( neighbor_comm_type_t t ) { this->type = t; }
    /*std::atomic<bool>*/ bool isReady(){
        return this->ready;
    }
    std::string getHostname() {return this->hostname;}
    void setHostname( char* name) { this->hostname = std::string( name );}

    friend std::ostream& operator << ( std::ostream& os, const ContactInfo& b ) {
        return b.doprint( os ); // polymorphic print via reference
    }
    friend std::istream &operator>> ( std::istream& is, ContactInfo &c ) { 
        return c.doinput( is );
    }

};

// extern std::ostream& operator<< ( std::ostream&, ContactInfo& );



#endif // _POSH_CONTACTINFO_H_
