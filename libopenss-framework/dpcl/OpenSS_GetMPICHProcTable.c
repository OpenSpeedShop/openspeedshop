/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the OpenSS_GetMPICHProcTable function.
 *
 */

#include "Assert.h"
#include "OpenSS_Job.h"

#include <dpclExt.h>



/** Structure holding host name, executable name, and pid for a process. */
typedef struct {
    char* host_name;        /**< Something we can pass to inet_addr. */
    char* executable_name;  /**< The name of the image. */
    int pid;                /**< The pid of the process. */    
} MPIR_PROCDESC;



/**
 * Get the MPICH process table.
 * 
 * Gets the current value of the MPICH process table within this process. The
 * contents of that table are placed into an OpenSS_Job structure, XDR encoded,
 * and then sent to the caller via the specified DPCL message handle.
 *
 * @todo    DPCL currently imposes a maximum (around 16Kb) on the length of
 *          individual messages sent using Ais_send(). In the future this limit
 *          should be removed. For now the table is broken up, when required,
 *          into multiple OpenSS_Job structures that are sent individually.
 *
 * @sa    http://www-unix.mcs.anl.gov/mpi/mpi-debug/
 *
 * @param msg_handle        DPCL message handle for returning the table.
 * @param proctable_size    Address of MPIR_proctable_size (the number 
 *                          of entries in MPIR_proctable).
 * @param proctable         Address of MPIR_proctable (an array of 
 *                          MPIR_PROCDESC structures.
 */
void OpenSS_GetMPICHProcTable(AisPointer msg_handle, 
			      int* proctable_size, MPIR_PROCDESC** proctable)
{
    const size_t EncodingBufferSize = 15 * 1024;
    unsigned i, estimated_size, entry_size, actual_size;
    char* buffer = NULL;
    OpenSS_Job job;
    XDR xdrs;

    /* Allocate the encoding buffer */
    buffer = alloca(EncodingBufferSize);

    /*
     * Allocate an array of host/pid pairs in the job description large enough
     * to hold the entire process table (even though we may not use all of it)
     */
    job.processes.processes_val = 
	alloca(*proctable_size * sizeof(OpenSS_JobEntry));

    /* Zero the host/pid pair array length in the job description */
    job.processes.processes_len = 0;
    
    /* Initialize the estimated encoding size */
    estimated_size = sizeof(job);
    
    /* Iterate over each host/pid pair in the process table */
    for(i = 0; i < *proctable_size; ++i) {
	
	/* Compute the estimated encoding size of this entry */
	entry_size =

	    /* Host name */
	    (((*proctable)[i].host_name != NULL) ?
	     strlen((*proctable)[i].host_name) : 
	     0) +

	    /* Terminating zero for host name */
	    1 +
	    
	    /* Process identifier */
	    sizeof(uint32_t) +
	    
	    /* Fudge-factor */
	    8;

	/* Will adding this entry exceed the encoding buffer size? */
	if((estimated_size + entry_size) > EncodingBufferSize) {

	    /* Indicate this list DOES NOT complete the job description */
	    job.is_last = FALSE;

	    /* Create an XDR stream using the encoding buffer */
	    xdrmem_create(&xdrs, buffer, EncodingBufferSize, XDR_ENCODE);
	    
	    /* Encode the job description to this stream */
	    Assert(xdr_OpenSS_Job(&xdrs, &job) == TRUE);
	    
	    /* Get the actual encoded size */
	    actual_size = xdr_getpos(&xdrs);

	    /* Close the XDR stream */
	    xdr_destroy(&xdrs);
	    
	    /* Send the data */
	    Assert(Ais_send(msg_handle, (void*)buffer, (int)actual_size) == 0);

	    /* Zero the host/pid pair array length in the job description */
	    job.processes.processes_len = 0;
    
	    /* Initialize the estimated encoding size */
	    estimated_size = sizeof(job);

	}

	/* Increment the estimated encoding size by this entry's size */
	estimated_size += entry_size;
	
	/* Add this host/pid pair to the job description */
	job.processes.processes_val[job.processes.processes_len].host = 
	    (*proctable)[i].host_name;
	job.processes.processes_val[job.processes.processes_len].pid = 
	    (*proctable)[i].pid;
	job.processes.processes_len++;
	
	/* Is this the last host/pid pair in the process table? */
	if(i == (*proctable_size - 1)) {

	    /* Indicate this list DOES complete the job description */
	    job.is_last = TRUE;

	    /* Create an XDR stream using the encoding buffer */
	    xdrmem_create(&xdrs, buffer, EncodingBufferSize, XDR_ENCODE);
	    
	    /* Encode the job description to this stream */
	    Assert(xdr_OpenSS_Job(&xdrs, &job) == TRUE);
	    
	    /* Get the actual encoded size */
	    actual_size = xdr_getpos(&xdrs);

	    /* Close the XDR stream */
	    xdr_destroy(&xdrs);
	    
	    /* Send the data */
	    Assert(Ais_send(msg_handle, (void*)buffer, (int)actual_size) == 0);

	}

    }
}
