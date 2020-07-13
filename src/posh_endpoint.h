
#ifndef _POSH_ENDPOINT_H_
#define _POSH_ENDPOINT_H_

#include "posh_neighbor.h"

#define ENDPOINT_OK           0
#define ENDPOINT_UNAVAILABLE -1

class Endpoint_t {
public:
    virtual ~Endpoint_t() = default;

    
    virtual void init_end( void ) = 0;
    //    virtual void init( void ) = 0; 
    virtual int finalize( void ) = 0;
    virtual ContactInfo* getMyContactInfo( void ) = 0;
    virtual void setMyContactInfo( int ) = 0;
    virtual neighbor_comm_type_t getType( void ) = 0; 

    friend  std::ostream& operator << ( std::ostream& os, const Endpoint_t& b ) {
        return os << "endpoint" << std::endl;
    }
};

#endif // _POSH_ENDPOINT_H_
