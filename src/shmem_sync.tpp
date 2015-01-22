#include "shmem_constants.h"

#include <unistd.h>
#include <iostream>

template<class T> void shmem_template_wait( T* var, T value ){

    /* Spin-lock */

    while( *var != value ) {
        usleep( SPIN_TIMER );
    }
}

template<class T> void shmem_template_wait_until( T* var, int cmp, T value ){

    while( true ) {
        switch( cmp ) {
        case SHMEM_CMP_EQ:
            if( *var == value ) return; 
        case SHMEM_CMP_NE:
            if( *var != value ) return;
        case SHMEM_CMP_GT:
            if( *var > value ) return;
        case SHMEM_CMP_LE:
            if( *var <= value ) return;
        case SHMEM_CMP_LT:
            if( *var < value ) return;
        default:
            std::cerr << "Wrong compare operator: " << cmp << std::endl;
            return;
        }
        usleep( SPIN_TIMER );
    }
}
