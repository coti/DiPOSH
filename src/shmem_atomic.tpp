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

#include <boost/bind.hpp>
#include "shmem_internal.h"

/* FIXME need the other implementations */

#include "posh_heap.h"

/**********************************************************************/
/*       **************           SWAP          **************        */
/**********************************************************************/

template<class T> class AtomicRemoteSwap {
private:
    managed_shared_memory* _remoteHeap;
    T* _target;
    T* _value;
    T* _previous;

public:
    
    AtomicRemoteSwap( managed_shared_memory* remoteHeap, T* target, T& value, T& previous  ) {
        this->_remoteHeap = remoteHeap;
        this->_target = target;
        this->_value = &value;
        this->_previous = &previous;
  }
    
    
   void operator()( ) {
        T* buf;

        /* Get our hands in there */

        managed_shared_memory::handle_t myHandle = myHeap.myHeap.get_handle_from_address( this->_target );
        buf = static_cast<T *> (this->_remoteHeap->get_address_from_handle( myHandle ));


        /* pull the initial data */
        
        _shmem_memcpy( static_cast<void *>( this->_previous ), static_cast<void *>( buf ), sizeof( T ) );

        /* push the new value over there */
        
        _shmem_memcpy( static_cast<void *>( buf ), static_cast<void *>( this->_value ), sizeof( T ) );
        
   }
};

template<class T> T shmem_template_swap( T* target, T value, int pe ) {

    T previousValue;

#if 0 /* debug only: not atomic !!!! */
    previousValue = shmem_template_g( target, pe );
    shmem_template_p( target, value, pe );
#endif

    /* Open remote PE's symmetric heap */
    
    char* _name;
    _name = myHeap.buildHeapName( pe );
    while( false == _sharedMemEsists( _name ) ) {
        usleep( SPIN_TIMER );
    }
    managed_shared_memory remoteHeap(  open_only, _name );
    free( _name );
        
    /* Execute atomically */

    AtomicRemoteSwap<T> func( &remoteHeap, target, boost::ref( value ), boost::ref( previousValue )  );
    remoteHeap.atomic_func( func );

    return previousValue;
}


/**********************************************************************/
/*       **************          CSWAP          **************        */
/**********************************************************************/

template<class T> class AtomicRemoteCswap {
private:
    managed_shared_memory* _remoteHeap;
    T* _target;
    T* _result;
    T* _cond;
    T* _value;

public:
    
    AtomicRemoteCswap( managed_shared_memory* remoteHeap, T* target, T& cond, T& value, T& result  ) {
        this->_remoteHeap = remoteHeap;
        this->_target = target;
        this->_result = &result;
        this->_cond = &cond;
        this->_value = &value;
   }
    
    void operator()( ) {
        T* buf;

        /* Get our hands in there */

        managed_shared_memory::handle_t myHandle = myHeap.myHeap.get_handle_from_address( this->_target );
        buf = static_cast<T *> (this->_remoteHeap->get_address_from_handle( myHandle ));


        /* pull the initial data */
        
       _shmem_memcpy( static_cast<void *>( this->_result ), static_cast<void *>( buf ), sizeof( T ) );

        if( this->_cond != this->_result ) {
            return;
        }

        /* if the condition is validated: push the new value over there */
        
        _shmem_memcpy( static_cast<void *>( buf ), static_cast<void *>( this->_value ), sizeof( T ) );
        
   }
};

template<class T> T shmem_template_cswap( T* target, T cond, T value, int pe ) {

    T previousValue;

    /* Open remote PE's symmetric heap */
    
    char* _name;
    _name = myHeap.buildHeapName( pe );
    while( false == _sharedMemEsists( _name ) ) {
        usleep( SPIN_TIMER );
    }
    managed_shared_memory remoteHeap(  open_only, _name );
    free( _name );
        
    /* Execute atomically */

    AtomicRemoteCswap<T> func( &remoteHeap, target, boost::ref( value ), boost::ref( cond ), boost::ref( previousValue )  );
    remoteHeap.atomic_func( func );

    return previousValue;
}


/**********************************************************************/
/*       **************           FADD          **************        */
/**********************************************************************/

template<class T> class AtomicRemoteFadd {
private:
    managed_shared_memory* _remoteHeap;
    T* _target;
    T* _result;
    T _value;

public:
    
    AtomicRemoteFadd( managed_shared_memory* remoteHeap, T* target, T value, T& result  ) {
        this->_remoteHeap = remoteHeap;
        this->_target = target;
        this->_result = &result;
        this->_value = value;
   }
    
    void operator()( ) {
        T* buf;

        /* Get our hands in there */

        managed_shared_memory::handle_t myHandle = myHeap.myHeap.get_handle_from_address( this->_target );
        buf = static_cast<T *> (this->_remoteHeap->get_address_from_handle( myHandle ));


        /* pull the initial data */
        
        _shmem_memcpy( static_cast<void *>( this->_result ), static_cast<void *>( buf ), sizeof( T ) );

        /* Add the value */

        this->_value += *(this->_result);

        /* push the resulting value over there */
        
        _shmem_memcpy( static_cast<void *>( buf ), static_cast<void *>( &(this->_value) ), sizeof( T ) );
        
   }
};

template<class T> T shmem_template_fadd( T* target, T value, int pe ) {

    T previousValue;

    /* Open remote PE's symmetric heap */
    
    char* _name;
    _name = myHeap.buildHeapName( pe );
    while( false == _sharedMemEsists( _name ) ) {
        usleep( SPIN_TIMER );
    }
    managed_shared_memory remoteHeap(  open_only, _name );
    free( _name );
        
    /* Execute atomically */

    AtomicRemoteFadd<T> func( &remoteHeap, target, value, boost::ref( previousValue )  );
    remoteHeap.atomic_func( func );

    return previousValue;
}


/**********************************************************************/
/*       **************           FINC          **************        */
/**********************************************************************/

template<class T> class AtomicRemoteFinc {
private:
    managed_shared_memory* _remoteHeap;
    T* _target;
    T* _result;

public:
    
     AtomicRemoteFinc( managed_shared_memory* remoteHeap, T* target, T& result  ) {
        this->_remoteHeap = remoteHeap;
        this->_target = target;
        this->_result = &result;
   }
    
    
   void operator()( ) {
        T* buf;
        T value;

        /* Get our hands in there */

        managed_shared_memory::handle_t myHandle = myHeap.myHeap.get_handle_from_address( this->_target );
        buf = static_cast<T *> (this->_remoteHeap->get_address_from_handle( myHandle ));


        /* pull the initial data */
        
        _shmem_memcpy( static_cast<void *>( this->_result ), static_cast<void *>( buf ), sizeof( T ) );

        /* increment a copy */

        value = *(this->_result) + 1;

        /* push the new value over there */
        
        _shmem_memcpy( static_cast<void *>( buf ), static_cast<void *>( &value ), sizeof( T ) );
        
   }
};

template<class T> T shmem_template_finc( T* target, int pe ) {

    T ret;

    /* Open remote PE's symmetric heap */
    
    char* _name;
    _name = myHeap.buildHeapName( pe );
    while( false == _sharedMemEsists( _name ) ) {
        usleep( SPIN_TIMER );
    }
    managed_shared_memory remoteHeap(  open_only, _name );
    free( _name );
        
    /* Execute atomically */

    AtomicRemoteFinc<T> func( &remoteHeap, target, boost::ref( ret )  );
    remoteHeap.atomic_func( func );

    return ret;;
}


/**********************************************************************/
/*       **************            ADD          **************        */
/**********************************************************************/

template<class T> class AtomicRemoteAdd {
private:
    managed_shared_memory* _remoteHeap;
    T* _target;
    T _value;

public:
    
     AtomicRemoteAdd( managed_shared_memory* remoteHeap, T* target, T value  ) {
        this->_remoteHeap = remoteHeap;
        this->_target = target;
        this->_value = value;
   }
    
    
   void operator()( ) {
        T* buf;

        /* Get our hands in there */

        managed_shared_memory::handle_t myHandle = myHeap.myHeap.get_handle_from_address( this->_target );
        buf = static_cast<T *> (this->_remoteHeap->get_address_from_handle( myHandle ));


        /* perform the addition on the remote data */

        *buf += this->_value;
        
   }
};

template<class T> void shmem_template_add( T* target, T value, int pe ) {

    /* Open remote PE's symmetric heap */
    
    char* _name;
    _name = myHeap.buildHeapName( pe );
    while( false == _sharedMemEsists( _name ) ) {
        usleep( SPIN_TIMER );
    }
    managed_shared_memory remoteHeap(  open_only, _name );
    free( _name );
        
    /* Execute atomically */

    AtomicRemoteAdd<T> func( &remoteHeap, target, value );
    remoteHeap.atomic_func( func );

}


/**********************************************************************/
/*       **************            INC          **************        */
/**********************************************************************/

template<class T> class AtomicRemoteInc {
private:
    managed_shared_memory* _remoteHeap;
    T* _target;

public:
    
     AtomicRemoteInc( managed_shared_memory* remoteHeap, T* target  ) {
        this->_remoteHeap = remoteHeap;
        this->_target = target;
   }
    
    
   void operator()( ) {
        T* buf;

        /* Get our hands in there */

        managed_shared_memory::handle_t myHandle = myHeap.myHeap.get_handle_from_address( this->_target );
        buf = static_cast<T *> (this->_remoteHeap->get_address_from_handle( myHandle ));


        /* perform the addition on the remote data */

        *buf += 1;
   }
};

template<class T> void shmem_template_inc( T* target, int pe ) {

    /* Open remote PE's symmetric heap */
    
    char* _name;
    _name = myHeap.buildHeapName( pe );
    while( false == _sharedMemEsists( _name ) ) {
        usleep( SPIN_TIMER );
    }
    managed_shared_memory remoteHeap(  open_only, _name );
    free( _name );
        
    /* Execute atomically */

    AtomicRemoteInc<T> func( &remoteHeap, target );
    remoteHeap.atomic_func( func );

}

