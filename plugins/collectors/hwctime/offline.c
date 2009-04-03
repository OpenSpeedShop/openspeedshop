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
#include "PapiAPI.h"
#include "blobs.h"

static uint64_t hwctime_time_started;

extern void hwctime_resume_papi();
extern void hwctime_suspend_papi();

/**
 * Start offline sampling.
 *
 * Starts program counter (PC) sampling for the thread executing this function.
 * Writes descriptive information for the thread to the appropriate file and
 * calls hwctime_start_sampling() with the environment-specified arguments.
 *
 * @param in_arguments    Encoded function arguments. Always null.
 */
void offline_start_sampling(const char* in_arguments)
{
    hwctime_start_sampling_args args;
    char arguments[3 * sizeof(hwctime_start_sampling_args)];

    /* Access the environment-specified arguments */
    const char* sampling_rate = getenv("OPENSS_HWCTIME_THRESHOLD");

    /* Encode those arguments for hwctime_start_sampling() */
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

    /* For some reason we can not init papi here and translate
     * the OPENSS_HWC_EVENT string to an event code without hosing
     * the papi initialization for the papi handler. FIXME.
     */

    args.collector = 1;
    args.experiment = 0;
    args.hwctime_event = PAPI_NULL;  /*hwc_start_sampling will set args.hwc_event. */
    OpenSS_EncodeParameters(&args,
			    (xdrproc_t)xdr_hwctime_start_sampling_args,
			    arguments);
    
        
    hwctime_time_started = OpenSS_GetTime();

    /* Start sampling */
    hwctime_start_sampling(arguments);
}



/**
 * Stop offline sampling.
 *
 * Stops program counter (PC) sampling for the thread executing this function. 
 * Calls hwctime_stop_sampling() and writes descriptive information for the
 * thread to the appropriate file.
 *
 * @param in_arguments    Encoded function arguments. Always null.
 */
void offline_stop_sampling(const char* in_arguments, const int finished)
{
    OpenSS_DataHeader header;
    openss_expinfo info;

    /* Stop sampling */
    hwctime_stop_sampling(NULL);

    if (!finished) {
	return;
    }

    /* Initialize the offline "info" blob's header */
    OpenSS_InitializeDataHeader(0, /* Experiment */
				1, /* Collector */
				&header);
    
    /* Initialize the offline "info" blob */
    OpenSS_InitializeParameters(&info);
    info.collector = strdup("hwctime");
    info.exename = strdup(OpenSS_GetExecutablePath());
    info.rank = monitor_mpi_comm_rank();

    /* Access the environment-specified arguments */
    const char* sampling_rate = getenv("OPENSS_HWCTIME_THRESHOLD");
    info.rate = (sampling_rate != NULL) ? atoi(sampling_rate) : 100;

    if (sampling_rate != NULL) {
        info.rate=atoi(sampling_rate);
    } else {
#if defined(linux)
        if (hw_info) {
            info.rate = (unsigned) hw_info->mhz*10000*2;
        } else {
            info.rate = THRESHOLD*2;
        }
#else
        info.rate = THRESHOLD*2;
#endif
    }
    
    const char* hwctime_event_param = getenv("OPENSS_HWCTIME_EVENT");
    if (hwctime_event_param != NULL) {
        info.event = strdup(hwctime_event_param);
    } else {
        info.event = strdup("PAPI_TOT_CYC");
    }

    /* Send the offline "info" blob */
    OpenSS_SetSendToFile("hwctime", "openss-info");
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
    if (is_dlopen)
        hwctime_suspend_papi();
    OpenSS_DataHeader header;
    openss_objects objects;
    
    /* Initialize the offline "dso" blob's header */
    OpenSS_InitializeDataHeader(0, /* Experiment */
				1, /* Collector */
				&header);

    if (is_dlopen) {
	header.time_begin = OpenSS_GetTime();
    } else {
	header.time_begin = hwctime_time_started;
    }
    header.time_end = is_dlopen ? -1ULL : OpenSS_GetTime();
    
    /* Initialize the offline "dso" blob */
    objects.objname = strdup(dsoname);
    objects.addr_begin = begin;
    objects.addr_end = end;
    objects.is_open = is_dlopen;

    /* Send the offline "dso" blob */
    OpenSS_SetSendToFile("hwctime", "openss-dsos");
    OpenSS_Send(&header, (xdrproc_t)xdr_openss_objects, &objects);
    if (is_dlopen)
        hwctime_resume_papi();
}
