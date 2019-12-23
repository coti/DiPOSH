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

#ifndef _POSH_CHECKPOINTING_H_
#define  _POSH_CHECKPOINTING_H_

#ifdef _DEBUG
#define _POSH_ADDITIONAL_CL_ARGUMENTS 5
#else
#define _POSH_ADDITIONAL_CL_ARGUMENTS 6 // +1 for -q (quiet)
#endif

#define _POSH_DMTCPLAUNCH "/usr/local/bin/dmtcp_launch"
#define _POSH_DMTCPCOORD  "--join-coordinator"
#define _POSH_DMTCP_OPTION_SM "--allow-file-overwrite"
#define _POSH_DMTCP_PLUGIN1  "--with-plugin"
#define _POSH_DMTCP_PLUGIN2   "/home/figatellix/POSH/src/ckpt/libposh_cl.so"
#define _POSH_DMTCP_OPTION_QUIET "-q"

#define _POSH_DMTCPSTARTCOORD   "/usr/local/bin/dmtcp_coordinator"

#define CKPT_INITIATOR 0

void cl_start_coordinator( void );
void posh_take_local_checkpoint( void );

#endif // ifndef _POSH_CHECKPOINTING_H_
