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
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <dmtcp.h>

#include "posh_checkpointing.h"
#include "shmem_internal.h"

void posh_take_local_checkpoint(){
    
    int dmtcp_enabled = dmtcp_is_enabled();
    int ckpt, generation;

    if( !dmtcp_enabled ) {
        std::cerr << "DMTCP is not enabled. It might be because the executable";
        std::cerr << "is not running under dmtcp_launch." << std::endl;
        std::cerr << "Checkpointing is ignored." << std::endl;
        return;
    }

#ifdef _DEBUG
    std::cout << "Hello!" << std::endl;
#endif

    /* Which generation (checkpoint wave number) is this */
    
    generation = dmtcp_get_generation();

    /* Close the communication stuff and synchronize */

    myInfo.enterCheckpointing();
    myInfo.posh_close_communication_channels();
    checkpointing.coordinate();

    /* Take the checkpoint
       All the processes connected to the coordinator take the checkpoint.
       Therefore, this function must be called by one process only.
    */

    if( CKPT_INITIATOR == myInfo.getRank() ) {
        ckpt = dmtcp_checkpoint();
    /* After the checkpoint */

    switch( ckpt ) {
    case DMTCP_AFTER_CHECKPOINT:
        // Wait long enough for checkpoint request to be written out.
        while( dmtcp_get_generation() == generation ) {
            usleep(100);
        }
        std::cout << "Checkpointing: done" << std::endl;
        break;
    case DMTCP_AFTER_RESTART:
        std::cout << "*** dmtcp_checkpoint: This program is now restarting." << std::endl;
        break;
    case DMTCP_NOT_PRESENT:
        std::cerr << " *** dmtcp_checkpoint: DMTCP is not running.  Skipping checkpoint." << std::endl;
        break;
    default:
        std::cerr << "Checkpointing with dmtcp_checkpoint() returned an unexpected value: " << ckpt << std::endl;
  }

    /*  printf("\n*** Process done executing.  Successfully exiting.\n");
  if( ckpt == DMTCP_AFTER_CHECKPOINT ) {
    printf("*** Execute ./dmtcp_restart_script.sh to restart.\n");
    }*/

    }


  /* Re-open the communication stuff */

  myInfo.posh_reopen_communication_channels();

}

