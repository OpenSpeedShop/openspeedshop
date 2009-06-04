/*******************************************************************************
** Copyright (c) The Krell Institute (2007,2008,2009). All Rights Reserved.
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

/** Type defining the items stored in thread-local storage. */
typedef struct {

    uint64_t time_started;
    int is_tracing;
    int pid;

    OpenSS_DataHeader dso_header;   /**< Header for following dso blob. */
    OpenSS_DataHeader info_header;  /**< Header for following info blob. */
    offline_data data;              /**< Actual dso data blob. */

    struct {
        openss_objects objs[OpenSS_OBJBufferSize];
    } buffer;

    int  dsoname_len;
    int  finished;
    int  sent_data;

} TLS;

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */

static const uint32_t TLSKey = 0x0000FEF9;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif


void offline_sent_data(int sent_data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->sent_data = sent_data;
}

void offline_send_dsos(TLS *tls)
{
    OpenSS_SetSendToFile(&(tls->dso_header), "mpi", "openss-dsos");
    OpenSS_Send(&(tls->dso_header), (xdrproc_t)xdr_offline_data, &(tls->data));
    
    /* Send the offline "info" blob */
#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"offline_stop_sampling SENDS DSOS for HOST %s, PID %d, POSIX_TID %lu\n",
        tls->dso_header.host, tls->dso_header.pid, tls->dso_header.posix_tid);
    }
#endif
#if 0
    fprintf(stderr,"SEND DSOS\n");
    int i;
    for (i = 0; i < tls->data.objs.objs_len; i++) {
	fprintf(stderr,"TLS objname = %s\n",tls->buffer.objs[i].objname);
	fprintf(stderr,"TLS addresses = %#lx,%#lx\n",tls->buffer.objs[i].addr_begin, tls->buffer.objs[i].addr_end);
	fprintf(stderr,"TLS times = %lu,%lu\n",tls->buffer.objs[i].time_begin, tls->buffer.objs[i].time_end);
	fprintf(stderr,"TLS is_open = %d\n",tls->buffer.objs[i].is_open);
    }
    fprintf(stderr,"TLS tls->data.objs.objs_len = %d\n",tls->data.objs.objs_len);
    fprintf(stderr,"TLS tls->dsoname_len = %d\n",tls->dsoname_len);
    fprintf(stderr,"TLS BYTES %d\n",sizeof(char) * tls->dsoname_len + sizeof(openss_objects) * tls->data.objs.objs_len);
    fprintf(stderr,"(OpenSS_OBJBufferSize) = %d\n", OpenSS_OBJBufferSize);
#endif
    tls->data.objs.objs_len = 0;
    tls->dsoname_len = 0;
}

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
    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
    if (tls == NULL) {
        tls = malloc(sizeof(TLS));
        Assert(tls != NULL);
        OpenSS_SetTLS(TLSKey, tls);
	tls->is_tracing = 0;
    }
    
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (tls->is_tracing == 1 && tls->pid == getpid()) {
#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"offline_start_sampling ALREADY TRACING %d\n",tls->pid);
	}
#endif
	return;
    }

    if (tls->finished) {
#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"offline_start_sampling Already DONE TRACING %d\n",tls->pid);
	}
#endif
	return;
    }

    tls->is_tracing = 1;
    tls->pid = getpid();

    mpi_start_tracing_args args;
    char arguments[3 * sizeof(mpi_start_tracing_args)];

    args.collector = 1;
    args.experiment = 0;

    /* Encode those arguments for mpi_start_tracing() */
    OpenSS_EncodeParameters(&args,
			    (xdrproc_t)xdr_mpi_start_tracing_args,
			    arguments);
    
    tls->time_started = OpenSS_GetTime();

    tls->dsoname_len = 0;
    tls->data.objs.objs_len = 0;
    tls->data.objs.objs_val = tls->buffer.objs;

    /* Start sampling */
    mpi_start_tracing(arguments);
}


void offline_finish()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (!tls->finished) {
	return;
    }

    OpenSS_DataHeader header;
    openss_expinfo info;

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
#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"offline_stop_sampling SENDS INFO for HOST %s, PID %d, POSIX_TID %lu\n",
        header.host, header.pid, header.posix_tid);
    }
#endif

    OpenSS_SetSendToFile(&header, "mpi", "openss-info");
    OpenSS_Send(&header, (xdrproc_t)xdr_openss_expinfo, &info);

    /* Write the thread's initial address space to the appropriate file */
    OpenSS_GetDLInfo(getpid(), NULL);
    if(tls->data.objs.objs_len > 0) {
	offline_send_dsos(tls);
    }
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
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"offline_stop_sampling ENTERED for %d, is_tracing %d, finished %d\n",
	tls->pid, tls->is_tracing, finished);
    }
#endif

    if (!tls->is_tracing) {
#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"offline_stop_sampling NOT TRACING %d\n",tls->pid);
	}
#endif
	return;
    }

    /* Stop sampling */
    mpi_stop_tracing(NULL);

    tls->finished = finished;

    if (finished && tls->sent_data) {
#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"offline_stop_sampling FINISHED for %d\n",tls->pid);
	}
#endif
	offline_finish();
    }
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
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    //fprintf(stderr,"offline_record_dso called for %s, is_dlopen = %d\n",dsoname, is_dlopen);

    /* Initialize the offline "dso" blob's header */
    OpenSS_DataHeader local_header;
    OpenSS_InitializeDataHeader(0, /* Experiment */
				1, /* Collector */
				&local_header);
    memcpy(&tls->dso_header, &local_header, sizeof(OpenSS_DataHeader));

    openss_objects objects;

    if (is_dlopen) {
	objects.time_begin = tls->dso_header.time_begin = OpenSS_GetTime();
    } else {
	objects.time_begin = tls->dso_header.time_begin = tls->time_started;
    }
    objects.time_end = tls->dso_header.time_end = is_dlopen ? -1ULL : OpenSS_GetTime();
    

    /* Initialize the offline "dso" blob */
    objects.objname = strdup(dsoname);
    objects.addr_begin = begin;
    objects.addr_end = end;
    objects.is_open = is_dlopen;

    memcpy(&(tls->buffer.objs[tls->data.objs.objs_len]),
           &objects, sizeof(objects));
    tls->data.objs.objs_len++;
    tls->dsoname_len += strlen(dsoname);

    if(tls->data.objs.objs_len + tls->dsoname_len == OpenSS_OBJBufferSize) {
	offline_send_dsos(tls);
    }
}
