#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <boost/thread.hpp>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>

#include "shmem_processinfo.h"
#include "shmem_constants.h"

#include <sys/time.h>
#include <sys/resource.h>

#include <cerrno>

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
		perror( "sigaction" );
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

int spawnPE( int id, int nbPE, bool debug_mode, char** arg ){

    int fd[2];
    int pid;
    int i;
    char** myEnv;

#ifdef _DEBUG
    std::cout << "Spawn " << arg[0] << " on thread " << id << " in " << nbPE << std::endl;
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
        perror( "pSHMEM launch failed in fork: " );
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

#ifdef _DEBG
        std::cerr << "execve returned. errno = " << errno << std::endl;
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

    int np, i, k, nthreads; // np = total nb of processes, nthreads = number on this machine
    bool debug, trace;
    char* env_var;
    char* value;
    char* taktukrank;

    /* initialize default values */

    np = getNbCores();
    debug = trace = false;
    poshState = NOT_STARTED_YET;

    /* Parse arguments */

    k = 1;
    while( k < argc ) {
        if( 0 == strncmp( "-n", argv[k], std::min( strlen( "-n" ), strlen( argv[k] ) ) ) ) {
            if( NULL != argv[k+1] ) {
                k++;
                np = atoi( argv[k] );
            } else {
                error_arguments( argv[0] );
                return EXIT_FAILURE;
            }
        } else if( 0 == strncmp( "-d", argv[k], std::min( strlen( "-n" ), strlen( argv[k] ) ) ) ) {
            debug = true;
        } else if( 0 == strncmp( "-t", argv[k], std::min( strlen( "-n" ), strlen( argv[k] ) ) ) ) {
            trace = true;
        } else {
            break;
        }
        k++;
    }

    /* Have I been deployed by a TakTuk job? */

    taktukrank = getenv( "TAKTUK_RANK" );
    if( NULL != taktukrank ){
        std::cout << "I am distributed process " << taktukrank << " in " << getenv( "TAKTUK_COUNT" ) << std::endl;
        nthreads = 1;
    } else {
        nthreads = np;
    }

#ifdef _DEBUG
    std::cout << "I will spawn ";
    for( i = k ; i < argc ; i++ ) {
        std::cout << argv[i] << " " ;
    }
    std::cout << " on " << nthreads << " threads"  << std::endl;
#endif

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
    process0.create_thread( boost::bind( &spawnPE, 0, np, debug, &argv[k] ));
    process0.join_all();
#else    
    spawnPE( 0, nthreads, debug, &argv[k] );
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
    for( i = 1 ; i < nthreads ; i++ ) {
#ifdef MULTITHREADED_SPAWN
        workers.create_thread( boost::bind( &spawnPE, i, np, debug, &argv[k] ));
#else
        spawnPE( i, np, debug, &argv[k] );
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

    /* Wait for everybody to complete */
    for( i = 0 ; i < nthreads ; i++ ) {
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
