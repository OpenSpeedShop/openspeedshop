/*******************************************************************************
** Copyright (c) The Krell Institute (2007). All Rights Reserved.
** Copyright (c) 2008 William Hachfeld. All Rights Reserved.
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
 * Declaration and definition of the PC sampling collector's offline runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "OpenSS_Offline.h"
#include "blobs.h"
#include "MPITraceableFunctions.h"

int OpenSS_mpi_rank = -1;

static uint64_t mpi_time_started;
static int is_tracing = 0;

/**
 * Start offline sampling.
 *
 * Starts mpi tracing for the thread executing this function.
 * Writes descriptive information for the thread to the appropriate file and
 * calls mpi_start_sampling() with the environment-specified arguments.
 *
 * @param in_arguments    Encoded function arguments. Always null.
 */
void offline_start_sampling(const char* in_arguments)
{
    if (is_tracing) {
	return;
    }
    is_tracing = 1;

    mpi_start_tracing_args args;
    char arguments[3 * sizeof(mpi_start_tracing_args)];

    args.collector = 1;
    args.experiment = 0;

    /* Encode those arguments for mpi_start_tracing() */
    OpenSS_EncodeParameters(&args,
			    (xdrproc_t)xdr_mpi_start_tracing_args,
			    arguments);
    
    mpi_time_started = OpenSS_GetTime();

    /* Start sampling */
    mpi_start_tracing(arguments);
}



/**
 * Stop offline sampling.
 *
 * Stops program counter (PC) sampling for the thread executing this function. 
 * Calls mpi_stop_sampling() and writes descriptive information for the
 * thread to the appropriate file.
 *
 * @param in_arguments    Encoded function arguments. Always null.
 */
void offline_stop_sampling(const char* in_arguments, const int finished)
{
    OpenSS_DataHeader header;
    openss_expinfo info;

    if (!is_tracing) {
	return;
    }

    /* Stop sampling */
    mpi_stop_tracing(NULL);

    if (!finished) {
	return;
    }

    /* Initialize the offline "info" blob's header */
    OpenSS_InitializeDataHeader(0, /* Experiment */
				1, /* Collector */
				&header);
    
    /* Initialize the offline "info" blob */
    OpenSS_InitializeParameters(&info);
    info.collector = strdup("mpi");
    info.exename = strdup(OpenSS_GetExecutablePath());
    info.rank = OpenSS_mpi_rank;

    /* Access the environment-specified arguments */
    const char* mpi_traced = getenv("OPENSS_MPI_TRACED");

    if (mpi_traced != NULL && strcmp(mpi_traced,"") != 0) {
	info.traced = strdup(mpi_traced);
    } else {
	info.traced = strdup(all);
    }
    
    /* Send the offline "info" blob */
    OpenSS_SetSendToFile("mpi", "openss-info");
    OpenSS_Send(&header, (xdrproc_t)xdr_openss_expinfo, &info);

    /* Write the thread's initial address space to the appropriate file */
    OpenSS_GetDLInfo(getpid(), NULL);
}



/**
 * Record a DSO operation.
 *
 * Writes information regarding a DSO being loaded or unloaded in the thread
 * to the appropriate file.
 *
 * @param dsoname      Name of the DSO's file.
 * @param begin        Beginning address at which this DSO was loaded.
 * @param end          Ending address at which this DSO was loaded.
 * @param is_dlopen    Boolean "true" if this DSO was just opened,
 *                     or "false" if it was just closed.
 */
void offline_record_dso(const char* dsoname,
			uint64_t begin, uint64_t end,
			uint8_t is_dlopen)
{
    OpenSS_DataHeader header;
    openss_objects objects;
    
    /* Initialize the offline "dso" blob's header */
    OpenSS_InitializeDataHeader(0, /* Experiment */
				1, /* Collector */
				&header);

    if (is_dlopen) {
	header.time_begin = OpenSS_GetTime();
    } else {
	header.time_begin = mpi_time_started;
    }
    header.time_end = is_dlopen ? -1ULL : OpenSS_GetTime();
    
    /* Initialize the offline "dso" blob */
    objects.objname = strdup(dsoname);
    objects.addr_begin = begin;
    objects.addr_end = end;
    objects.is_open = is_dlopen;

    /* Send the offline "dso" blob */
    OpenSS_SetSendToFile("mpi", "openss-dsos");
    OpenSS_Send(&header, (xdrproc_t)xdr_openss_objects, &objects);
}
