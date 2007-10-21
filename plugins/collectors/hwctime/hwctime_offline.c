/*******************************************************************************
** Copyright (c) The Krell Institute (2007). All Rights Reserved.
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
 * Declaration and definition of the HardWare Counter Time sampling collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "blobs.h"
#include "hwctime_offline.h"

pid_t create_OpenSS_exepath () {
    pid_t pid = getpid();
    char tmpname[PATH_MAX];

    /* find our exe path (read symlink for /proc/pid/exe) */
    OpenSS_Path_From_Pid(tmpname);
    OpenSS_exepath = strdup(tmpname);
    return pid;
}

void create_rawfile_prefix () {
    pid_t pid = create_OpenSS_exepath();
    char tmpname[PATH_MAX], *bname;

    bname = basename(OpenSS_exepath);
    sprintf(tmpname,"/tmp/hwc-%s-%d",bname,pid);
    rawprefix = strdup(tmpname);
}

void create_rawfile_name () {
    uint64_t (*realfunc)() = dlsym (RTLD_NEXT, "pthread_self");
    if ( (*realfunc) != NULL) {
	tid = (*realfunc)();
    }

    char tmpname[PATH_MAX];

    /* create pathname to write raw data for specific thread */
    if (tid > 0) {
        sprintf(tmpname,"%s-%lu.openss-raw",rawprefix,tid);
    }
    else {
        sprintf(tmpname,"%s.openss-raw",rawprefix);
    }

    /* create our openss-raw output filename */
    OpenSS_rawdata = strdup(tmpname);
#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_OFFLINE_COLLECTOR") != NULL) {
	fprintf(stderr,"create_rawfile_name: OpenSS_rawdata = %s\n",OpenSS_rawdata);
    }
#endif
}


void offline_start_sampling(const char* arguments)
{
    /* TODO: handle experiment parameters */
#if 0
    hwctime_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    args.collector=1;
    args.sampling_rate=100000;
    args.thread=getpid();
    /* how to handle
       Blob arguments(reinterpret_cast<xdrproc_t>(xdr_hwctime_start_sampling_args), &args);
    */
#endif
    hwctime_start_sampling(arguments);
}

void offline_stop_sampling(const char* arguments)
{

    /* call the collector specific stop sampling funcion. This writes any data */
    /* blobs to the specific openss-raw file (exp-collector-pid-tid.openss-raw). */
    hwctime_stop_sampling(arguments);

    /* Call OpenSS_GetDLInfo with a NULL dso pathname argument to get the list of */
    /* dso's opened for this process.  These will be recorded as */
    /* object blobs in the openss-raw file.  One dso per blob is written to openss-raw. */
    /* TODO: error checking for OpenSS_GetDLInfo. */
    pid_t pid = getpid();
    int retval = OpenSS_GetDLInfo(pid, NULL);
}

void offline_record_dso(const char* dsoname, uint64_t begin, uint64_t end)
{
    /* Create an OpenSS_Data header for this object. */
    /* These values must match values used for data headers. */
    /* Initialize the dso blob's header */
    /* Passing &(tls.header) to OpenSS_InitializeDataHeader was not safe on ia64 systems.
     */
    OpenSS_DataHeader local_dso_header;
    OpenSS_InitializeDataHeader(/*experiment*/ 0, /*collector*/ 1, &(local_dso_header));
    memcpy(&tlsobj.header, &local_dso_header, sizeof(OpenSS_DataHeader));

    /* create an OpenSS_Obj blob for the dso named dsoname. */
    /* record the full path and the address range as found in the /proc/<pid>/maps file */
    tlsobj.objs.objname=strdup(dsoname);
    tlsobj.objs.addr_begin=begin;
    tlsobj.objs.addr_end=end;


#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_OFFLINE_COLLECTOR") != NULL) {
	fprintf(stderr,"offline_record_dso: record %s to %s\n",dsoname,OpenSS_rawdata);
    }
#endif

    OpenSS_Send(&(tlsobj.header), (xdrproc_t)xdr_openss_objects, &(tlsobj.objs));
}
