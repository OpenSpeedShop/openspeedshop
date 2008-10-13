/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007 William Hachfeld. All Rights Reserved.
** Copyright (c) 2007 Krell Institute.  All Rights Reserved.
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
 * Declaration and definition of the HWC sampling collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "blobs.h"
#include "PapiAPI.h"

#if defined (OPENSS_OFFLINE)
#include "hwc_offline.h"
#include "OpenSS_Offline.h"
#endif

#if defined (OPENSS_USE_FILEIO)
#include "OpenSS_FileIO.h"
#endif


/*
 * NOTE: For some reason GCC doesn't like it when the following two macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

/** Thread-local storage. */
static __thread struct {

    OpenSS_DataHeader header;  /**< Header for following data blob. */
    hwc_data data;             /**< Actual data blob. */

    OpenSS_PCData buffer;      /**< PC sampling data buffer. */

} tls;

/** PAPI event set. */
static __thread int EventSet = PAPI_NULL;



/**
 * PAPI event handler.
 *
 * Called by PAPI each time a sample is to be taken. Takes the program counter
 * (PC) address passed by PAPI and places it into the sample buffer. When the
 * sample buffer is full, it is sent to the framework for storage in the
 * experiment's database.
 *
 * @param context    Thread context at timer interrupt.
 */
static void hwcPAPIHandler(int EventSet, void* pc, 
			   long_long overflow_vector, void* context)
{
    /* Update the sampling buffer and check if it has been filled */
    if(OpenSS_UpdatePCData((uint64_t)pc, &tls.buffer)) {
	
	/* Send these samples */
	tls.header.time_end = OpenSS_GetTime();	
	tls.header.addr_begin = tls.buffer.addr_begin;
	tls.header.addr_end = tls.buffer.addr_end;
	tls.data.pc.pc_len = tls.buffer.length;
	tls.data.count.count_len = tls.buffer.length;

#ifndef NDEBUG
        if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
            fprintf(stderr,"hwcPAPIHandler sends data:\n");
            fprintf(stderr,"time_end(%#lu) addr range [%#lx, %#lx] pc_len(%d) count_len(%d)\n",
                tls.header.time_end,tls.header.addr_begin,
		tls.header.addr_end,tls.data.pc.pc_len,
                tls.data.count.count_len);
        }
#endif

#if defined (OPENSS_USE_FILEIO)
	/* Create the openss-data file name for this exe-collector-pid-tid */
	/* Default is to create openss-data files in /tmp */
	OpenSS_CreateOutfile("openss-data");
#endif
	OpenSS_Send(&(tls.header), (xdrproc_t)xdr_hwc_data, &(tls.data));

	/* Re-initialize the data blob's header */
	tls.header.time_begin = tls.header.time_end;

	/* Re-initialize the sampling buffer */
	tls.buffer.addr_begin = ~0;
	tls.buffer.addr_end = 0;
	tls.buffer.length = 0;
	memset(tls.buffer.hash_table, 0, sizeof(tls.buffer.hash_table));
	
    }
}



/**
 * Start sampling.
 *
 * Starts hardware counter (HWC) sampling for the thread executing this
 * function. Initializes the appropriate thread-local data structures and
 * then enables the sampling counter.
 *
 * @param arguments    Encoded function arguments.
 */
void hwc_start_sampling(const char* arguments)
{
    hwc_start_sampling_args args;

#if defined (OPENSS_USE_FILEIO)
    /* Create the rawdata output file prefix. */
    /* fpe_stop_tracing will append */
    /* a tid as needed for the actuall .openss-xdrtype filename */
    OpenSS_CreateFilePrefix("hwc");
#endif

#if defined (OPENSS_OFFLINE)

    /* TODO: need to handle arguments for offline collectors */
    args.collector=1;
    args.experiment=0; /* DataQueues index start at 0.*/

    if(EventSet == PAPI_NULL) {
	hwc_init_papi();
    }

    /* Initialize the info blob's header */
    /* Passing &(tls.header) to OpenSS_InitializeDataHeader was */
    /* not safe on ia64 systems. */
    OpenSS_DataHeader local_info_header;
    OpenSS_InitializeDataHeader(args.experiment, args.collector, &(local_info_header));
    memcpy(&tlsinfo.header, &local_info_header, sizeof(OpenSS_DataHeader));

    tlsinfo.header.time_begin = OpenSS_GetTime();

    openss_expinfo local_info;
    OpenSS_InitializeParameters(&(local_info));
    memcpy(&tlsinfo.info, &local_info, sizeof(openss_expinfo));
    tlsinfo.info.collector = "hwc";
    tlsinfo.info.exename = strdup(OpenSS_exepath);

    char* sampling_rate = getenv("OPENSS_HWC_THRESHOLD");

    if (sampling_rate != NULL) {
	args.sampling_rate=atoi(sampling_rate);
    } else {
#if defined(linux)
	if (hw_info) {
	    args.sampling_rate = (unsigned) hw_info->mhz*10000*2;
	} else {
	    args.sampling_rate = THRESHOLD*2;
	}
#else
	args.sampling_rate = THRESHOLD*2;
#endif
    }

    char* hwc_event_param = getenv("OPENSS_HWC_EVENT");
    if (hwc_event_param != NULL) {
	args.hwc_event=get_papi_eventcode(hwc_event_param);
	tlsinfo.info.event = strdup(hwc_event_param);
    } else {
	args.hwc_event=get_papi_eventcode("PAPI_TOT_CYC");
	tlsinfo.info.event = strdup("PAPI_TOT_CYC");
    }

    tlsinfo.info.rate = args.sampling_rate;

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"hwc_start_sampling sends tlsinfo:\n");
        fprintf(stderr,"collector=%s, hostname=%s, pid =%d, OpenSS_rawtid=%lx\n",
            tlsinfo.info.collector,tlsinfo.header.host,
	    tlsinfo.header.pid,tlsinfo.header.posix_tid);
    }
#endif

#else

    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
			    (xdrproc_t)xdr_hwc_start_sampling_args,
			    &args);
#endif
    

    /* Initialize the data blob's header */
    /* Passing &(tls.header) to OpenSS_InitializeDataHeader was not safe on ia64 systems.
     */
    OpenSS_DataHeader local_data_header;
    OpenSS_InitializeDataHeader(args.experiment, args.collector, &(local_data_header));
    memcpy(&tls.header, &local_data_header, sizeof(OpenSS_DataHeader));
    
    /* Initialize the actual data blob */
    tls.data.interval = (uint64_t)(args.sampling_rate);
    tls.data.pc.pc_val = tls.buffer.pc;
    tls.data.count.count_val = tls.buffer.count;

    /* Initialize the sampling buffer */
    tls.buffer.addr_begin = ~0;
    tls.buffer.addr_end = 0;
    tls.buffer.length = 0;
    memset(tls.buffer.hash_table, 0, sizeof(tls.buffer.hash_table));

#if defined (OPENSS_OFFLINE)
    tlsobj.objs.objname = NULL;
    tlsobj.objs.addr_begin = ~0;
    tlsobj.objs.addr_end = 0;
#endif

    /* Begin sampling */
    tls.header.time_begin = OpenSS_GetTime();
    if(EventSet == PAPI_NULL) {
	hwc_init_papi();
    }
    OpenSS_Create_Eventset(&EventSet);
    OpenSS_AddEvent(EventSet, args.hwc_event);
    OpenSS_Overflow(EventSet, args.hwc_event, tls.data.interval,
		    hwcPAPIHandler);
    OpenSS_Start(EventSet);
}



/**
 * Stop sampling.
 *
 * Stops hardware counter (HWC) sampling for the thread executing this function.
 * Disables the sampling counter and sends any samples remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void hwc_stop_sampling(const char* arguments)
{
    /* Stop sampling */
    OpenSS_Stop(EventSet);
    tls.header.time_end = OpenSS_GetTime();

    /* For MPT add this check because we were hanging becasuse this is a SGI MPT daemon process */
    /* and not a ranked process.  So there is no data */
    if(tls.buffer.length > 0) {

#if defined (OPENSS_OFFLINE)

    tlsinfo.info.rank = OpenSS_mpi_rank;

    /* create the openss-info data and send it */
#if defined (OPENSS_USE_FILEIO)
    OpenSS_CreateOutfile("openss-info");
#endif
    OpenSS_Send(&(tlsinfo.header), (xdrproc_t)xdr_openss_expinfo, &(tlsinfo.info));
#endif

    }

    /* Are there any unsent samples? */
    if(tls.buffer.length > 0) {
	
	/* Send these samples */
	tls.header.addr_begin = tls.buffer.addr_begin;
	tls.header.addr_end = tls.buffer.addr_end;
	tls.data.pc.pc_len = tls.buffer.length;
	tls.data.count.count_len = tls.buffer.length;

#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr, "hwc_stop_sampling:\n");
	    fprintf(stderr, "time_end(%#lu) addr range[%#lx, %#lx] pc_len(%d) count_len(%d)\n",
		tls.header.time_end,tls.header.addr_begin,
		tls.header.addr_end,tls.data.pc.pc_len,
		tls.data.count.count_len);
	}
#endif

#if defined (OPENSS_USE_FILEIO)
	/* Create the rawdata output file prefix. */
	/* hwc_stop_sampling will append a tid as */
	/* needed for the actuall .openss-xdrtype filename */
	OpenSS_CreateFilePrefix("hwc");

        /* Create the openss-raw file name for this exe-collector-pid-tid */
        /* Default is to create openss-data files in /tmp */
        OpenSS_CreateOutfile("openss-data");

#endif /* defined OPENSS_USE_FILEIO */

	OpenSS_Send(&(tls.header), (xdrproc_t)xdr_hwc_data, &(tls.data));
	
    }
}
