#include "shmem_internal.h"
#include <fstream>

/* Accessors */

int MeMyselfAndI::getRank(){
    return this->shmem_rank;
}

void MeMyselfAndI::setRank( int _rank ){
    this->shmem_rank = _rank;
}

int MeMyselfAndI::getSize(){
    return this->shmem_size;
}

void MeMyselfAndI::setSize( int _size ){
    this->shmem_size = _size;
}

bool MeMyselfAndI::getStarted(){
    return this->shmem_started;
}

void MeMyselfAndI::setStarted( bool _started ){
    this->shmem_started = _started;
}

RemoteProcess_t* MeMyselfAndI::getNeighbors() {
    return this->neighbors;
}

RemoteProcess_t* MeMyselfAndI::getNeighbor( int rank ) {
    return &(this->neighbors[rank]);
}

boost::interprocess::named_mutex* MeMyselfAndI::getLock( long _lock ){
    boost::interprocess::named_mutex* mut =  this->locks[ _lock ];
    if( _shmem_unlikely( NULL == mut ) ) {
        char* name;
        asprintf( &name, "shmem_lock_%ld", _lock );
        boost::interprocess::named_mutex mtx(boost::interprocess::open_or_create, name );
        free( name );
        mut = &mtx;
        this->setLock( _lock, &mtx );
    }
    return mut;
}
void MeMyselfAndI::setLock( long _lock, boost::interprocess::named_mutex *_mtx ){
    this->locks[ _lock ] = _mtx;
}

/* how do I communicate with this guy? */

communication_type_t MeMyselfAndI::getRemoteCommType( int pe ) {
    return this->getNeighbor( pe )->type;
}

/* Methods */

void MeMyselfAndI::findAndSetMyRank( ){
    if( -1 != this->shmem_rank ) {
        return;
    } else {
        char* myNum;
        myNum = getenv( VAR_PE_NUM );
        if( NULL == myNum ) {
            std::cerr << "Could not retrieve my PE number" << std::endl;
            return;
        }
        this->setRank( atoi( myNum ) );
    }
}

void MeMyselfAndI::findAndSetMySize () {
    if( -1 != this->shmem_size ) {
        return;
    } else {
        char* numPE;
        numPE = getenv( VAR_NUM_PE );
        if( NULL == numPE ) {
            std::cerr << "Could not retrieve the number of PEs in the system" << std::endl;
            return;
        }
        this->setSize( atoi( numPE ) );
    }
}

void MeMyselfAndI::findAndSetPidRoot () {
    char* rootpid;
    if( 0 == myInfo.getRank() ) {
        asprintf( &rootpid, "%d", getpid() );
    } else {
        rootpid = getenv( VAR_PID_ROOT );
        if( NULL == rootpid ) {
            std::cerr << "Could not retrieve the process id of the root process" << std::endl;
            return;
        }
    }

    this->setRootPid( atoi( rootpid ) );
#if _DEBUG
    std::cout << myInfo.getRank() << "] my father has pid " << atoi( rootpid ) << std::endl;
#endif

    if( 0 == myInfo.getRank() ) {
        free( rootpid );
    }
}

void MeMyselfAndI::setRootPid( int pid ) {
    Process pRoot( 0, pid );
    processes.push_back( pRoot );
}


void MeMyselfAndI::allocNeighbors( int nb ) {
    this->neighbors = ( RemoteProcess_t*) malloc( nb * sizeof( RemoteProcess_t ) );
//new managed_shared_memory[nb];
}

void MeMyselfAndI::initSharedMemNeighbor( int pe ) {
    char* _name; 
    _name = myHeap.buildHeapName( pe );
    while( false == _sharedMemEsists( _name ) ) {
        usleep( SPIN_TIMER );
    }
    managed_shared_memory remoteHeap(  open_only, _name );
    this->neighbors[pe].comm.sm_segment = std::move( remoteHeap );
    
    //        this->neighbors[i]( open_only, _name );
    free( _name );
}

void MeMyselfAndI::initOpenMxNeighbor( int pe ) {
    omx_return_t ret;

    this->neighbors[pe].comm.omx_endpoint.connected = false;

    ret = omx_hostname_to_nic_id( this->neighbors[pe].hostname, &(this->neighbors[pe].comm.omx_endpoint.dest_addr) );
    if (ret != OMX_SUCCESS) {
      fprintf(stderr, "Cannot find peer name %s\n", this->neighbors[pe].hostname);
      goto out;
    }

    return;

 out:
    this->neighbors[pe].type = NO_COMM;
}

void MeMyselfAndI::initNeighbor( int pe ) {
    switch( this->neighbors[pe].type ) {
    case SHARED_MEMORY_COMM:
        this->initSharedMemNeighbor( pe );
        break;
    case OPEN_MX_COMM:
        this->initOpenMxNeighbor( pe );
        break;
    default:
        std::cerr << "Unknown communication type " << this->neighbors[pe].type <<std::endl;
    }

}

void MeMyselfAndI::initNeighbors( int nb ) {

    int len = 128;
    char* machinefile;
    char* taktukrank;
    bool use_openmx;
    int i;

    /* Am I using Open-MX ? */

    taktukrank = getenv( "TAKTUK_RANK" );
    if( NULL != taktukrank ){ /* yes: initialize it */
        use_openmx = true;
    } else {
        use_openmx = false;
    }
    free( taktukrank );

    if( use_openmx ) {

        char myhostname[len];

        gethostname( myhostname, len );

       /* Read the machinefile */

        machinefile = getenv( "POSH_MACHINEFILE" );

        std::ifstream in( machinefile );
        char str[len];
        i = 0;
        while( in && i < nb ) {
            in.getline( str, len );
            if( in ){ 
                std::memcpy( this->neighbors[i].hostname, str, sizeof( str ) );
#ifdef _DEBUG
                std::cout << "Process " << i << " will be running on " << str << std::endl;
#endif
                i++;
            }
        }
        
        in.close();

        /* Who is running on the same machine as me? */

        for( i = 0 ; i < nb ; i++ ) {
            if( 0 == strncmp( "localhost", this->neighbors[i].hostname, strlen( "localhost" ) )
                || 0 == strncmp( myhostname, this->neighbors[i].hostname, std::min( strlen( myhostname), strlen( this->neighbors[i].hostname ) ) ) ) {
                this->neighbors[i].type = SHARED_MEMORY_COMM;
            } else {
                this->neighbors[i].type = OPEN_MX_COMM;
            }
        }

    } else {
        /* Everyone is local */
        for( i = 0 ; i < nb ; i++ ) {
            std::memcpy( this->neighbors[i].hostname, "localhost", sizeof( "localhost" ) );
            this->neighbors[i].type = SHARED_MEMORY_COMM;
        }
    }

    /* Initialize everyone */

    for( i = 0 ; i < nb ; i++ ) {
        this->initNeighbor( i );
    }

}

/* How I see the remote guy's base address */

void* MeMyselfAndI::getRemoteHeapLocalBaseAddr( int rank ) {
    managed_shared_memory* myNeighbor = &(this->getNeighbor( rank )->comm.sm_segment);
    managed_shared_memory::handle_t handle = myHeap.getOffsetHandle();
    return myNeighbor->get_address_from_handle( handle );
}

/* How the remote guy sees his own base address */

void* MeMyselfAndI::getRemoteHeapBaseAddr( int rank ) {
    uintptr_t* ptr = (uintptr_t*)this->getRemoteHeapLocalBaseAddr( rank );
    return (void*)(*ptr);
}

/* Initialize Open-MX */

void MeMyselfAndI::initOpenMX( ) {

    char my_hostname[OMX_HOSTNAMELEN_MAX];
    char my_ifacename[OMX_BOARD_ADDR_STRLEN];
    omx_return_t ret;
    unsigned eid = OMX_ANY_ENDPOINT;
    unsigned bid = 0;

    ret = omx_init();
    if (ret != OMX_SUCCESS) {
        fprintf(stderr, "Failed to initialize (%s)\n", omx_strerror(ret) );
        return;
    }

    ret = omx_open_endpoint( bid, eid, 0x12345678, NULL, 0, &( this -> omx_ep ) );
    if (ret != OMX_SUCCESS) {
        fprintf(stderr, "Failed to open endpoint (%s)\n", omx_strerror(ret));
        return;
    }
    
#ifdef _DEBUG
    ret = omx_get_info( this -> omx_ep, OMX_INFO_BOARD_HOSTNAME, NULL, 0, my_hostname, sizeof(my_hostname));
    if (ret != OMX_SUCCESS) {
        fprintf(stderr, "Failed to get endpoint hostname (%s)\n", omx_strerror(ret));
        return;
    }
    
    ret = omx_get_info( this -> omx_ep, OMX_INFO_BOARD_IFACENAME, NULL, 0,  my_ifacename, sizeof(my_ifacename));
    if (ret != OMX_SUCCESS) {
        fprintf(stderr, "Failed to get endpoint iface name (%s)\n", omx_strerror(ret));
        return;
    }
    
    printf("Successfully open any endpoint for hostname '%s' iface '%s'\n",  my_hostname, my_ifacename);
#endif
}
