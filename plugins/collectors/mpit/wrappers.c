/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * MPI function wrappers for the MPI extended event tracing collector.
 *
 */

#include "RuntimeAPI.h"
#include "runtime.h"

#include <mpi.h>



/*
 * MPI_Irecv
 */

int mpit_PMPI_Irecv(void* buf, int count, MPI_Datatype datatype, int source, 
		    int tag, MPI_Comm comm, MPI_Request* request)
{
    int retval, datatype_size;
    mpit_event event;
    
    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();
    retval = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
    event.stop_time = OpenSS_GetTime();
    event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = comm;
    event.datatype = datatype;
    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Irecv));
    return retval;
}



/*
 * MPI_Isend
 */

int mpit_PMPI_Isend(void* buf, int count, MPI_Datatype datatype, int dest,
		    int tag, MPI_Comm comm, MPI_Request* request)
{
    int retval, datatype_size;
    mpit_event event;
    
    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();
    retval = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
    event.stop_time = OpenSS_GetTime();
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    event.destination = dest;
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = comm;
    event.datatype = datatype;
    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Isend));
    return retval;
}
