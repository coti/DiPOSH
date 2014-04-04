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

#include <stdio.h>
#include <stdlib.h>
#include "shmem.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define NBXP  20
#define DEFAULTSIZE 16

#define NBCOUNTERS       2
#ifdef WITHPAPI
#define PAPI             1
#else
#define PAPI             0
#endif

#if PAPI
#include <papi.h>
#else
#define long_long unsigned long long
#endif

double getTime();
void experimentPut( unsigned int, int, int, char*, char*, FILE* );
void experimentGet( unsigned int, int, int, char*, char*, FILE* );

int main( int argc, char** argv) {

    int rank, size, res, i, j, k;
    char* ptr;
    char* data;
    struct timeval tv_start, tv_end;
    double theTime;
    double** times;

    char* filename = NULL;
    FILE* fd = NULL;

#if PAPI
    int* events = NULL;
    int num_hwcntrs = 0;
    long long int* values = NULL;
    int error = 0;
#endif

    int nb = NBXP;
    int buffsize = DEFAULTSIZE;
    unsigned int it;

    int myBuddyLeft, myBuddyRight, myBuddy;

    int getOnly, putOnly;

    getOnly = 0 ; putOnly = 0;
    if( argc >= 4 ) {
        if( 0 == strncmp( argv[3], "g", 1 ) ) {
            getOnly = 1;
        }
        if( 0 == strncmp( argv[3], "p", 1 ) ) {
            putOnly = 1;
        }
        if( 0 == strncmp( argv[3], "o", 1 ) ) {
            asprintf( &filename, "%s.%d", "putget", getpid() );
        }
    }
    if( argc >= 5 ) {
        if( 0 == strncmp( argv[4], "o", 1 ) ) {
            asprintf( &filename, "%s.%d", "putget", getpid() );
        }
    }

    if( argc >= 3 )
        buffsize = atoi( argv[2] ) ;
    if( argc >= 2 )
        nb = atoi( argv[1] ) ;

#if PAPI
    /* Initialize the PAPI library and get the number of counters available */
    if( (num_hwcntrs = PAPI_num_counters()) <= PAPI_OK ){
        printf( "Error while retrieving counters info\n" );
        return EXIT_FAILURE;
    }

    printf( "This system has %d available counters.\n", num_hwcntrs );

    if( num_hwcntrs < NBCOUNTERS ) {
        printf( "Not enough counters available (%d available, I need %d)\n", num_hwcntrs, NBCOUNTERS );
        error = 1;
        goto finish;
    }

    num_hwcntrs = NBCOUNTERS;

    values = (long long int*) malloc( num_hwcntrs * sizeof( long long int ) );
    events = (int*) malloc( num_hwcntrs * sizeof( int ) );

    events[0] = PAPI_TOT_CYC;
    events[1] = PAPI_TOT_INS;

    /* Start counting events */
    int rc;
    rc = PAPI_start_counters( events, num_hwcntrs );
    if ( rc != PAPI_OK){
        printf( "error starting counting events\n" );
        switch( rc ){
        case PAPI_EINVAL:
            printf( "One or more of the arguments is invalid.\n" );
            break;
        case PAPI_EISRUN:
            printf( "Counters have already been started, you must call PAPI_stop_counters() before you call this function again.\n" );
            break;
        case PAPI_ESYS:
            printf( "A system or C library call failed inside PAPI, see the errno variable.\n" );
            break;
        case PAPI_ENOMEM:
            printf( "Insufficient memory to complete the operation.\n" );
            break;
        case PAPI_ECNFLCT:
            printf( "The underlying counter hardware cannot count this event and other events in the EventSet simultaneously.\n" );
            break;
        case PAPI_ENOEVNT:
            printf( "The PAPI preset is not available on the underlying hardware.\n" );
            break;
        }

        error = 1;
        goto finish;
    }
  
#endif

    start_pes( 0 );
    
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if( size < 2 ) {
        fprintf( stderr, "We need at least 2 processes. Exiting.\n" );
        return EXIT_SUCCESS;
    }

    if( rank == 0 || rank == 1  ) {
        if( NULL != filename ) {
            fd = fopen( filename, "w+" );
            if( NULL == fd ) {
                fprintf( stderr, "pb ouverture fichier sortie\n" );
                return EXIT_FAILURE;
            }
        }
        if( rank == 0 ) {
                printf( "buff size is %d, %d experiments\n", buffsize, nb );
            }
    }
    //printf( "Hello, I am process %d in %d \n", rank, size );

    //    shmem_barrier_all();
    sleep( 2 );
    
    /* check */

    myBuddyLeft = ( rank + 1 ) % size;
    myBuddyRight = ( rank + size - 1 ) % size;

    /*    res = shmem_pe_accessible( myBuddyLeft );
    if( 1 == res ) {
        printf( "[%d/%d] My buddy %d is reachable\n", rank, size, myBuddyLeft );
    } else {
        printf( "[%d/%d] My buddy %d is NOT reachable\n", rank, size, myBuddyLeft );
    }
    res = shmem_pe_accessible( myBuddyRight );
    if( 1 == res ) {
        printf( "[%d/%d] My buddy %d is reachable\n", rank, size, myBuddyRight );
    } else {
        printf( "[%d/%d] My buddy %d is NOT reachable\n", rank, size, myBuddyRight );
        }*/

    /* Allocate some memory */

    ptr = (char*)shmalloc( buffsize );
    data = (char*) malloc( buffsize );

    /* Put stuff there */

    memset( data, rank, buffsize );

    /* Remote read in my buddy's memory */

    if( rank % 2 ) {
        myBuddy = myBuddyRight;
    } else {
        myBuddy = myBuddyLeft;
    }

    shmem_barrier_all();

    if( getOnly != 1 ) {
        
        if( rank == 0 ) {
            
            if( NULL == fd ) {
                printf( "\n\n - * - * - * - * - PUT - * - * - * - * -\n" );
                printf( "[SIZE (B)]\t[BANDWIDTH (Gb/s)]\t[AVERAGE TIME (nsec)]\t[STD DEV]\n" );
            } else {
                fprintf( fd, "\n\n - * - * - * - * - PUT - * - * - * - * -\n" );
                fprintf( fd, "[SIZE (B)]\t[BANDWIDTH (Gb/s)]\t[AVERAGE TIME (nsec)]\t[STD DEV]\n" );
            }
        
            k = 1;
            it = 0;
            while( k <= buffsize ) {
                experimentPut( k, nb, myBuddy, data, ptr, fd );
                k = ( 0x01 << it ) + 3;
                if( k <= buffsize ) {
                    experimentPut( k, nb, myBuddy, data, ptr, fd );
                }
                k = ( 0x01 << it ) - 3;
                if( k > 0 ) {
                    experimentPut( k, nb, myBuddy, data, ptr, fd );
                }
                it++;
                k = 0x01 << it;
            }
            
        }
        
        shmem_barrier_all();
    }

    if( putOnly != 1 ) {
        if( rank == 1 ) {
            
            if( NULL == fd ) {
                printf( "\n\n - * - * - * - * - GET - * - * - * - * -\n" );
                printf( "[SIZE (B)]\t[BANDWIDTH (Gb/s)]\t[AVERAGE TIME (nsec)]\t[STD DEV]\n" );
            } else {
                fprintf( fd, "\n\n - * - * - * - * - GET - * - * - * - * -\n" );
                fprintf( fd, "[SIZE (B)]\t[BANDWIDTH (Gb/s)]\t[AVERAGE TIME (nsec)]\t[STD DEV]\n" );
            }

            k = 1;
            it = 0;
            while( k <= buffsize ) {
                experimentGet( k, nb, myBuddy, data, ptr, fd );
                k = ( 0x01 << it ) + 3;
                if( k <= buffsize ) {
                    experimentGet( k, nb, myBuddy, data, ptr, fd );
                }
                k = ( 0x01 << it ) - 3;
                if( k > 0 ) {
                    experimentGet( k, nb, myBuddy, data, ptr , fd);
                }
                it++;
                k = 0x01 << it;
            }
            
        }
        
        shmem_barrier_all();
    }


 finish:
    if( NULL != filename ) {
        free( filename );
    }
    if( NULL != fd ){
        fclose( fd );
    }
#if PAPI
    if( NULL != events ) {
        free( events );
    }
  if( NULL != values ) {
      free( values );
  }
#endif

if( rank == 0 ) {
    printf( "The end %d\n", EXIT_SUCCESS );
}

  // return EXIT_SUCCESS;
  exit( 0 ) ;
}

void experimentPut( unsigned int size, int nb, int myBuddy, char* buffIn, char* buffOut, FILE* fd ) {

    int i;
    double t_start, t_end, timeSpent;
    double averageT, sumT, devT;
    double averageBw, sumBw, devBw;
    double bw;

    /* warm-up */

    shmem_char_put( buffOut, buffIn, size, myBuddy );

    /* True work */

    averageT = 0;  devT = 0; sumT = 0;
    averageBw = 0;  devBw = 0; sumBw = 0;

    for( i = 0 ; i < nb ; i++ ) {
        t_start = getTime();
        shmem_char_put( buffOut, buffIn, size, myBuddy );
        t_end = getTime();
        timeSpent = t_end - t_start;
        
        averageT += timeSpent;
        sumT += timeSpent * timeSpent;

        bw = size*8 / timeSpent;
        averageBw += bw;
        sumBw += bw * bw;
    }

     if( nb != 0 ) {
        averageT /= nb;
        devT = sumT / nb;
        devT -= (averageT * averageT);
        devT = sqrt(devT);

        averageBw /= nb;
        devBw = sumBw / nb;
        devBw -= (averageBw * averageBw);
        devBw = sqrt(devBw);

        if( NULL == fd ) {
            printf( "%d\t%.2lf\t%.2lf\t%.4lf\n", size, averageBw, averageT, devT );
        } else {
            fprintf( fd, "%d\t%.2lf\t%.2lf\t%.4lf\n", size, averageBw, averageT, devT );
        }

    }
}

void experimentGet( unsigned int size, int nb, int myBuddy, char* buffOut, char* buffIn,  FILE* fd  ) {

    int i;
    double t_start, t_end, timeSpent;
    double averageT, sumT, devT;
    double averageBw, sumBw, devBw;
    double bw;

    /* warm-up */

    shmem_char_get( buffOut, buffIn, size, myBuddy );

    /* True work */

    averageT = 0;  devT = 0; sumT = 0;
    averageBw = 0;  devBw = 0; sumBw = 0;

    for( i = 0 ; i < nb ; i++ ) {
        t_start = getTime();
        shmem_char_get( buffOut, buffIn, size, myBuddy );
        t_end = getTime();
        timeSpent = t_end - t_start;
        
        averageT += timeSpent;
        sumT += timeSpent * timeSpent;

        bw = size*8 / timeSpent;
        averageBw += bw;
        sumBw += bw * bw;
    }

    if( nb != 0 ) {
        averageT /= nb;
        devT = sumT / nb;
        devT -= (averageT * averageT);
        devT = sqrt(devT);

        averageBw /= nb;
        devBw = sumBw / nb;
        devBw -= (averageBw * averageBw);
        devBw = sqrt(devBw);

        if( NULL == fd ) {
            printf( "%d\t%.2lf\t%.2lf\t%.4lf\n", size, averageBw, averageT, devT );
        } else {
            fprintf( fd, "%d\t%.2lf\t%.2lf\t%.4lf\n", size, averageBw, averageT, devT );
        }
    }

}


double getTime(){
#if PAPI
    return PAPI_get_real_usec();
#else
    struct timespec ts;
    clock_gettime( CLOCK_REALTIME, &ts );
    return ( ts.tv_nsec + ts.tv_sec * 1000000000 );

    /*    struct timeval tv;
    gettimeofday( &tv, NULL );
    return ( tv.tv_sec * 1000000 + tv.tv_usec );*/
#endif
}
