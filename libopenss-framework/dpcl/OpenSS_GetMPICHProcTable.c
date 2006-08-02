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



/** Number of entries in MPIR_proctable. */
extern int MPIR_proctable_size;

/** Structure holding host name, executable name, and pid for a process. */
typedef struct {
    char* host_name;        /**< Something we can pass to inet_addr. */
    char* executable_name;  /**< The name of the image. */
    int pid;                /**< The pid of the process. */    
} MPIR_PROCDESC;

/** Array of MPIR_PROCDESC structures. */
extern MPIR_PROCDESC* MPIR_proctable;



/**
 * Get the MPICH process table.
 * 
 * Gets the current value of the MPICH process table within this process. ...
 *
 * @sa    http://www-unix.mcs.anl.gov/mpi/mpi-debug/
 *
 * @param msg_handle    ...
 */
void OpenSS_GetMPICHProcTable(AisPointer msg_handle)
{
    unsigned i, buffer_size, encoded_size;
    OpenSS_Job job;
    char* buffer = NULL;
    XDR xdrs;
    
    /* Allocate the array of host/pid pairs in the job description */
    job.processes.processes_len = MPIR_proctable_size;
    job.processes.processes_val = 
	alloca(job.processes.processes_len * sizeof(OpenSS_JobEntry));

    /* Fill the host/pid pairs into the job description */
    for(i = 0; i < job.processes.processes_len; ++i) {
	job.processes.processes_val[i].host = MPIR_proctable[i].host_name;
	job.processes.processes_val[i].pid = MPIR_proctable[i].pid;
    }
    
    /* Estimate the required encoding buffer size */
    buffer_size = sizeof(job.processes.processes_len);
    for(i = 0; i < job.processes.processes_len; ++i) {
	if(job.processes.processes_val[i].host != NULL)
	    buffer_size += strlen(job.processes.processes_val[i].host);
	buffer_size += 1;  /* Terminating zero for host name string */
	buffer_size += sizeof(job.processes.processes_val[i].pid);
    }
    buffer_size += 256;  /* Fudge-factor */
    
    /* Allocate the encoding buffer */
    buffer = alloca(buffer_size);

    /* Create an XDR stream using the encoding buffer */
    xdrmem_create(&xdrs, buffer, buffer_size, XDR_ENCODE);

    /* Encode the job description to this stream */
    Assert(xdr_OpenSS_Job(&xdrs, &job) == TRUE);

    /* Get the encoded size */
    encoded_size = xdr_getpos(&xdrs);

    /* Close the XDR stream */
    xdr_destroy(&xdrs);

    /* Send the data */
    Assert(Ais_send(msg_handle, (void*)buffer, (int)encoded_size) == 0);

    /* 
     * Note: Since the list of host/pid pairs is stack allocated, and copies
     *       are not made of the host names, it would be incorrect to make a
     *       call to xdr_free() here.
     */
}
