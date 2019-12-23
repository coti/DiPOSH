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

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <boost/thread.hpp>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <boost/program_options.hpp>

#include "shmem_processinfo.h"
#include "shmem_constants.h"

#include <sys/time.h>
#include <sys/resource.h>

#include <cerrno>

#ifdef MPILAUNCHER
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
namespace mpi = boost::mpi;
#endif

#ifdef CHANDYLAMPORT
#include "posh_checkpointing.h"
#endif

namespace po = boost::program_options;

extern char** environ;

boost::condition_variable running_cond;
boost::mutex mut_cond;

std::vector<Process> processes;

typedef enum { NOT_STARTED_YET, STARTING, RUNNING, CLEANING, FINISHED } job_state_t;
static job_state_t poshState;

unsigned long long int heap_size = 134217728; ; // TODO : se debarasser de ca des que possible
char VAR_SMA_VERSION[] = "SMA_VERSION";
int shmem_sma_version[] = { 0, 0, 1 };
char VAR_SMA_INFO[] = "SMA_INFO";
char shmem_sma_info[] = "pSHMEM, Paris SHMEM, implementation of OpenSHMEM (c) LIPN 2012";

char VAR_HEAP_SIZE[] = "SMA_SYMMETRIC_SIZE";
//unsigned long long int heap_size = 134217728; /* 128 MB */
char VAR_SMA_DEBUG[] = "SMA_DEBUG";
int shmem_sma_debug = 0;

char VAR_NUM_PE[]    = "SHMEM_NUM_PE";    /* Number of PEs */
char VAR_PE_NUM[]    = "SHMEM_PE_NUM";    /* My own PE number */ 
char VAR_RUN_DEBUG[] = "SHMEM_RUN_DEBUG"; /* Debug mode: start an infinite loop to attach a debugger */
char VAR_PID_ROOT[]   = "SHMEM_PID_ROOT";  /* pid of the root process (rank 0) */

#define MIN( a, b ) do { a < b ? a : b} while( 0 );
#define MAX( a, b ) do { a > b ? a : b} while( 0 );

void deleteProcess( int );
void killProcesses( std::vector<Process> );
void sigchldHandler( int, siginfo_t *, void * );


unsigned int getNbCores( ) {
    unsigned int nb;
    char* varNbCores;

    varNbCores = getenv( "SHMEM_NB_THREADS" );

    if( NULL == varNbCores ) {
        nb = boost::thread::hardware_concurrency();
    } else  {
        nb = atoi( varNbCores );
    }

#ifdef _DEBUG
    std::cout << nb << " cores available" << std::endl;
#endif
    return nb;
}

void setHandler( ){
    //  signal( SIGCHLD, &sigchldHandler );
	struct sigaction act;

	memset( &act, '\0', sizeof( act ) );
 	act.sa_sigaction = &sigchldHandler;
 
	act.sa_flags = SA_SIGINFO;
 
	if( sigaction( SIGCHLD, &act, NULL ) < 0 ) {
        std::perror( "sigaction" );
    }

}

void error_arguments( char* name ) {
    std::cerr << "Argument error" << std::endl ;
    std::cerr << "Correct usage is:" << std::endl;
    std::cerr << "\t"<< name << " [options] program [arguments]" << std::endl;
    std::cerr << "Options : " << std::endl;
    std::cerr << "\t -n NPE : number of process elements " << std::endl;
    std::cerr << "\t -d     : debug mode " << std::endl;
    std::cerr << "\t -t     : print traces and stats " << std::endl;
    std::cerr << "\t -f FPE : rank of the first process" << std::endl;
    std::cerr << "\t -h HPE : number of PEs to spawn here" << std::endl;
#ifdef CHECKPOINTING
    std::cerr << "\t -r     : restarting from a checkpoint" << std::endl;
#endif
    std::cerr << "Useful environment variables:" << std::endl;
    std::cerr << "\tSMA_SYMMETRIC_SIZE : size of the symmetric heap" << std::endl;
    std::cerr << "\tSMA_DEBUG          : enable debug mode (verbose, equivalent to -d)" << std::endl;

}

void killProcesses( std::vector<Process> pList ) {

    int pid;
    int sig;

    sig = SIGKILL;

    for( std::vector<Process>::iterator it = processes.begin(); it != processes.end(); it++ ) {
        pid = it->getPid();
#ifdef _DEBUG
        std::cout << "send SIGKILL signal to process " << pid << std::endl;
#endif
        kill( pid, sig );
    }
    
}

void deleteProcess( int _pid ) {

#ifdef _DEBUG
    std::cout << "Deleting process " << _pid << " from my list." << std::endl;
#endif

    for( std::vector<Process>::iterator it = processes.begin(); it != processes.end(); it++ ) {
        if( _pid == it->getPid() ) {
            processes.erase( it );
            if( processes.empty() ) {
                poshState = FINISHED;
            }
            return;
        }
    }
}

void sigchldHandler( int sig, siginfo_t *siginfo, void *context ) {
    int status, exitStat;
    pid_t pid;

    pid = wait( &status );

    if( WIFEXITED(status) ) {
        std::cout << "Process " << pid << " exited normally" << std::endl;
    }
    exitStat = WEXITSTATUS(status);
    std::cout << "Process " << pid << " exited with status " << exitStat << std::endl;

    if( -1 == pid ) {
        int exitStat;
        printf( "errno : %d\n", errno );
        exitStat = WEXITSTATUS(status);
        printf("exit status: %d\n", exitStat );
        
        std::cerr << strerror(errno) << std::endl;
        
    }
    if( -1 != pid && errno != ECHILD ) {
        deleteProcess( pid );
    }

    if( ( exitStat != 0 ) && ( poshState != CLEANING ) ) {
        poshState = CLEANING;
        killProcesses( processes );
    } 

#ifdef _DEBUG
    std::cout << "Process " << pid << " exited with status " << exitStat << std::endl;
#endif

}

char* joinArgv( char** argv ) {
    int i, len;
    char* res;

    len = 0;

    i = 0;
    while( NULL != argv[i] ) {
        len += strlen( argv[i] );
        i++;
    }

    res = (char*) malloc( len + i + 1 );
    i = 0;
    len = 0;
    while( NULL != argv[i] ) {
        memcpy( res + len, argv[i], strlen( argv[i] ) );
        len += strlen( argv[i] ) + 1;
        res[len] = ' ';
        i++;
    }
    res[len] = '\0';

    return res;
}

/* Do not use strtok */

char** splitArgv( char* str, char sep ) {
    int nb, i, j;
    char** out;

    /* Count the nb of tokens */
    nb = 0;
    i = 0;
    while( '\0' != str[i] ) {
        if( sep == str[i] && '\0' != str[i+1]) {
            nb++;
        }
        i++;
    }

    out = (char**) malloc( (nb+1) *sizeof( char* ) );

    i = 0; j = 0;
    nb = 0;
    while( '\0' != str[i] ) {
        if( sep == str[i] && '\0' != str[i+1] ) {
            out[nb] = (char*) malloc( ( i - j + 1 ) * sizeof( char ) );
            memcpy( out[nb], &str[j], ( i - j ) * sizeof( char ) );
            out[nb][i-j] = '\0';
            i++;
            j = i;
            nb++;
        }
        i++;
    }
    if( i != j+1 ) {
        out[nb] = (char*) malloc( ( i - j + 1 ) * sizeof( char ) );
        memcpy( out[nb], &str[j], ( i - j ) * sizeof( char ) );
        out[nb][i-j] = '\0';
    }
    out[nb+1] = NULL;

    return out;
}

int getArgLen( char** arg ) {
    int len;
    len = 0;
    while( arg[len] != NULL ) {
        len++;
    }
    return len;
}

int spawnPE( int id, int nbPE, bool debug_mode, char** arg ){

    int fd[2];
    int pid;
    int i;
    char** myEnv;

#if 0 // def DISTRIBUTED_POSH
    int size;
    mpi::communicator world;
    std::cout << "Process " << world.rank() << " ";
#endif

#ifdef _DEBUG
    std::cout << "Spawn " << std::endl;
    for( i = 0; arg[i] != NULL ; i++ ) 
        std::cout << arg[i] << " " << std::endl;
    std::cout << " on thread " << id << " in " << nbPE << std::endl;
#endif

   /* Spawn the process */


    /* Set useful environment variables */

    for( i = 0; environ[i] != NULL ; i++ ) 
        ;;
    i--;
 
    /*   myEnv = (char**) malloc( ( i + 2 ) * sizeof( char* ) );
    memcpy( myEnv, environ, i * sizeof( char* ) );
    asprintf( &myEnv[i], "%s=%d", VAR_PE_NUM, id );
    myEnv[i+1] = NULL;*/

    myEnv = (char**) malloc( ( i + 3 ) * sizeof( char* ) );
    memcpy( myEnv, environ, i * sizeof( char* ) );
    asprintf( &myEnv[i], "%s=%d", VAR_PE_NUM, id );

    pid = -1;
    if( 0 != id ) {
        for( std::vector<Process>::iterator it = processes.begin(); it != processes.end(); it++ ) {
            if( 0 == it->getRank() ) { 
                pid = it->getPid();
            }
        }
    }
    asprintf( &myEnv[i+1], "%s=%d", VAR_PID_ROOT, pid );
    myEnv[i+2] = NULL;

    
    /* TODO: using open would be nicer */

    /* if( 0 != pipe( fd ) ) {
        perror( "error opening a pipe for I/O redirections" );
        return EXIT_FAILURE;
        }*/

    switch( pid = fork() ) {
    case -1:
        perror( "POSH launch failed in fork: " );
        running_cond.notify_one();
        return EXIT_FAILURE;

    case 0:
        /* redirect I/Os */
        /*  close( fd[0 ] );
        if( dup2( fd[1], STDOUT_FILENO ) < 0 ) {
            perror( "pSHMEM launch failed in dup2/stdout: " );
            running_cond.notify_one();
            return EXIT_FAILURE;
        }
        if( dup2( fd[1], STDERR_FILENO ) < 0 ) {
            perror( "pSHMEM launch failed in dup2/stderr: " );
            running_cond.notify_one();
            return EXIT_FAILURE;
            }*/
     /* execvpe is not available on BSD systems (including on my Mac) */
        //        execvp( arg[0], arg );
        execve( arg[0], arg, myEnv );

#ifdef _DEBUG
        std::cerr << "execve returned. errno = " << errno << std::endl;
        std::cerr << std::strerror( errno ) << std::endl;
#endif        

        running_cond.notify_one();
        return errno;

    default:
        /*   close( fd[1] );
             dup2( fd[0], STDIN_FILENO );*/
        break;
    }

     /* father: keep the pid and wake up the master */

    Process p( id, pid );
    processes.push_back( p );
#ifdef _DEBUG
    std::cout << "Process rank " << id << " is running under pid " << pid << std::endl;
#endif

    running_cond.notify_one();

    return EXIT_SUCCESS;
} 

int main( int argc, char** argv ) {

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ( "help", "produce help message")
        ( "number,n", po::value<int>(), "Number of process elements" )
        ( "debug,d", "Debug mode" )
        ( "traces,t", "Print traces and stats" )
        ( "first,f", po::value<int>(), "Rank of the first process" )
        ( "number_here,h", po::value<int>(), "Number of PEs to spawn here" )
        ( "executable", po::value< vector<string> >(), "Executable")
        ( "restart,r", "Restart from a checkpoint wave")
        ;
    po::positional_options_description p;
    p.add("executable", -1);  // TODO: is this correct?

    po::variables_map vm;
    po::store( po::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
    po::notify(vm);    

    int np, i, k, first_process, number_here;
    bool debug, trace, distributed;
    char* env_var;
    char* value;
    vector<string> execut;
    char** str_exec;
  
#if 0 //def DISTRIBUTED_POSH
  mpi::environment env;
  mpi::communicator world;
#endif

   /* initialize default values */

    np = getNbCores();
    debug = trace = false;
    poshState = NOT_STARTED_YET;
    first_process = 0;
    number_here = -1;
    distributed = false;

    /* Parse arguments */

    if( vm.count( "help" ) ) {
        cout << desc << "\n";
        return 1;
    }

    if( vm.count( "number" ) ) {
        np = vm[ "number" ].as<int>();
    }
    if( vm.count( "debug" ) ) {
        debug = true;
    }
    if( vm.count( "traces" ) ) {
        trace = true;
    }
#if CHECKPOINTING
     if( vm.count( "restart" ) ) {
        restart = true;
    }
#endif
   if( vm.count( "first" ) ) {
        first_process = vm[ "first" ].as<int>();
        distributed = true;
    } else {
        first_process = 0;
    }
    if( vm.count( "number_here" ) ) {
        number_here = vm[ "number_here" ].as<int>();
    } else {
        number_here = np;
    }

    /* If we want to use DMTCP for checkpointing, we need to start one coordinator per node */

#ifdef CHANDYLAMPORT
    cl_start_coordinator();
#endif

    str_exec = NULL;
    if ( vm.count("executable") ){
        execut = vm["executable"].as<std::vector<std::string> >();
        int shiftarg;
#ifdef CHANDYLAMPORT
        shiftarg = _POSH_ADDITIONAL_CL_ARGUMENTS;
#else
        shiftarg = 0;
#endif
        str_exec = (char**) malloc( ( execut.size() + 1 + shiftarg ) * sizeof( char* ) );
        
#ifdef CHANDYLAMPORT
        /* TODO: other option -q, --quiet (or set environment variable DMTCP_QUIET = 0, 1, or 2) */

        str_exec[0] =  _POSH_DMTCPLAUNCH; 
        str_exec[1] =  _POSH_DMTCPCOORD;
        str_exec[2] =  _POSH_DMTCP_OPTION_SM;
        str_exec[3] =  _POSH_DMTCP_PLUGIN1;
        str_exec[4] =  _POSH_DMTCP_PLUGIN2;
#ifndef _DEBUG
        str_exec[5] =  _POSH_DMTCP_OPTION_QUIET;
#endif
#endif // CHANDYLAMPORT

        for( i = 0 ; i < execut.size() ; i++ ) {
             str_exec[i + shiftarg] = const_cast<char*>( execut[i].c_str() );
        }
        
        str_exec[ execut.size() + shiftarg] = NULL;
#ifdef _DEBUG
        std::cout << "I will spawn ";
        for( i = 0 ; i < execut.size() + shiftarg ; i ++ )
            //    for( i = 0; str_exec[i] != NULL ; i++ ) 
            std::cout << str_exec[i] << " " << std::endl;
        
        
        for( std::vector<std::string>::iterator it = execut.begin(); it != execut.end(); ++it) {
            std::cout << *it << " " ;
        }
        std::cout << " on " << np << " threads"  << std::endl;
#ifdef DISTRIBUTED_POSH
        /*        std::cout << "I am process " << world.rank() << " of " << world.size()
                  << "." << std::endl;*/
#endif // DISTRIBUTED_POSH
#endif // _DEBUG
    } else {
        std::cout << "No exec provided" << std::endl;
        return EXIT_SUCCESS;
    }


    /* Get some environment variables */

    env_var = getenv( VAR_HEAP_SIZE );
    if( NULL != env_var ) {
        heap_size = atoll( env_var );
    }
    env_var = getenv( VAR_SMA_DEBUG );
    if( NULL != env_var ) {
        shmem_sma_debug = atoll( env_var );
        debug = shmem_sma_debug;
    }

    /* Put some into the environment */

    asprintf( &value, "%d", np );
    setenv( VAR_NUM_PE, value, 1 );

    if( debug ) {
        sprintf( value, "%d", 1 );
        setenv( VAR_RUN_DEBUG, value, 1 );
    }

    free( value );

    asprintf( &value, "%d.%d.%d", shmem_sma_version[0], shmem_sma_version[1], shmem_sma_version[2] );
    setenv( VAR_SMA_VERSION, value, 1 );
    setenv( VAR_SMA_INFO, shmem_sma_info, 1 );
    if( shmem_sma_debug ) {
        sprintf( value, "%d", shmem_sma_debug );
        setenv( VAR_SMA_DEBUG, value, 1 );
    }
    free( value );
    asprintf( &value, "%lld", heap_size );
    setenv( VAR_HEAP_SIZE, value, 1 );
    free( value );


    poshState = STARTING;

    /* Set sigchld handler */

    setHandler();

    /* Spawn the threads that will spawn the processes */

    //    command = joinArgv( &argv[k] );

#ifdef MULTITHREADED_SPAWN
    boost::thread_group process0;
    process0.create_thread( boost::bind( &spawnPE, 0, np, debug, str_exec ));
    process0.join_all();
#else    
    spawnPE( first_process, np, debug, str_exec );
#endif

#if _DEBUG
    int pid;
    for( std::vector<Process>::iterator it = processes.begin(); it != processes.end(); it++ ) {
        if( 0 == it->getRank() ) { 
            pid = it->getPid();
        }
    }
    std::cout << "Process 0 has pid " << pid << std::endl;
#endif

#ifdef MULTITHREADED_SPAWN
    boost::thread_group workers;
#endif
        std::cout << "spawn " << first_process << " to " << first_process + number_here << std::endl;
    for( i = first_process + 1 ; i < first_process + number_here ; i++ ) {
        std::cout << "spawn " << i << " / " << first_process + number_here << std::endl;
#ifdef MULTITHREADED_SPAWN
        workers.create_thread( boost::bind( &spawnPE, i, np, debug, str_exec ));
#else
        spawnPE( i, np, debug, str_exec );
#endif
    }

    // sleep until one worker is done, then wait for all of them to complete
#ifdef MULTITHREADED_SPAWN
    boost::this_thread::yield();
    boost::unique_lock<boost::mutex> running_lock( mut_cond );
    running_cond.wait( running_lock ); 
#endif
    poshState = RUNNING;
    
    /* reset default signal handler */
    signal( SIGCHLD, SIG_DFL );

#ifdef MULTITHREADED_SPAWN
     workers.join_all();
#endif

     free( str_exec );

    /* Wait for everybody to complete */
    for( i = 0 ; i < np ; i++ ) {
        pid_t mypid;
        int exitStat;
        siginfo_t infop;
        int rc;

        rc = waitid( P_ALL, 0, &infop, WEXITED | WSTOPPED );

        mypid = infop.si_pid;
        exitStat = infop.si_status; 
        if( exitStat != 0 ) {
            std::cout << "Exit status : " << infop.si_status ;
            std::cout << " - Errno : " << infop.si_errno << " - code : "<< infop.si_code << std::endl;
        }

        if( -1 == rc || -1 == mypid ) {
            printf( "errno : %d\n", errno );
            printf("exit status: %d\n", exitStat );
            
            std::cerr << strerror(errno) << std::endl;
            
        }    

        if( -1 != mypid && errno != ECHILD ) {
            deleteProcess( mypid );
        }
        
        if( exitStat != 0 && poshState != CLEANING ) {
            std::cerr << "A process (pid " << mypid << ") ended abnormally with status code " << exitStat << ". Killing all the other processes." << std::endl;
            poshState = CLEANING;
            killProcesses( processes );
        }
        
        if( trace ) {
#if __linux
            std::cout << "Process " << mypid << " exited with status " << exitStat << " ; resource usage: " << infop.si_utime << " (user time) " << infop.si_stime << " (system time)" << std::endl;
#else
            std::cout << "Process " << mypid << " exited with status " << exitStat << std::endl;
#endif
        }
    }

    if( debug ) std::cout << "All done" << std::endl;

    return EXIT_SUCCESS;
}
