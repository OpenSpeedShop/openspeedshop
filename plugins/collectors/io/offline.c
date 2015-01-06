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
#include "IOTraceableFunctions.h"

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

static const uint32_t TLSKey = 0x0000FEF7;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

extern void defer_trace(int);

void offline_finish();

void offline_pause_sampling()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->is_tracing = 0;
    defer_trace(0);
}

void offline_resume_sampling()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->is_tracing = 1;
    defer_trace(1);
}

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
    OpenSS_SetSendToFile(&(tls->dso_header), "io", "openss-dsos");
    OpenSS_Send(&(tls->dso_header), (xdrproc_t)xdr_offline_data, &(tls->data));
    
    /* Send the offline "info" blob */
#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"offline_send_dsos SENDS DSOS for HOST %s, PID %lld, POSIX_TID %llu\n",
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
 * Starts io tracing for the thread executing this function.
 * Writes descriptive information for the thread to the appropriate file and
 * calls io_start_sampling() with the environment-specified arguments.
 *
 * @param in_arguments    Encoded function arguments. Always null.
 */
void offline_start_sampling(const char* in_arguments)
{
    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls =  malloc(sizeof(TLS));
    Assert(tls != NULL);
    OpenSS_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    io_start_tracing_args args;
    char arguments[3 * sizeof(io_start_tracing_args)];

    args.collector = 1;
    args.experiment = 0;

    /* Encode those arguments for io_start_tracing() */
    OpenSS_EncodeParameters(&args,
			    (xdrproc_t)xdr_io_start_tracing_args,
			    arguments);
    
    tls->time_started = OpenSS_GetTime();

    tls->dsoname_len = 0;
    tls->data.objs.objs_len = 0;
    tls->data.objs.objs_val = tls->buffer.objs;

    /* Start sampling */
    io_start_tracing(arguments);
}

/**
 * Stop offline sampling.
 *
 * Stops program counter (PC) sampling for the thread executing this function. 
 * Calls io_stop_sampling() and writes descriptive information for the
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

    if (!tls) {
#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"warn: offline_stop_sampling has no TLS for %d\n",getpid());
	}
#endif
	return;
    }

    defer_trace(0);

    /* Stop sampling */
    io_stop_tracing(NULL);

    tls->finished = finished;

    if (finished && tls->sent_data) {
#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"offline_stop_sampling FINISHED for %d\n",getpid());
	}
#endif
	offline_finish();
    }
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
    info.collector = strdup("io");
    info.exename = strdup(OpenSS_GetExecutablePath());
    info.rank = monitor_mpi_comm_rank();

    /* Access the environment-specified arguments */
    const char* io_traced = getenv("OPENSS_IO_TRACED");

    if (io_traced != NULL && strcmp(io_traced,"") != 0) {
	info.traced = strdup(io_traced);
    } else {
	info.traced = strdup(traceable);
    }
    
    /* Send the offline "info" blob */
#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"offline_stop_sampling SENDS INFO for HOST %s, PID %lld, POSIX_TID %llu\n",
        header.host, header.pid, header.posix_tid);
    }
#endif

    OpenSS_SetSendToFile(&header, "io", "openss-info");
    OpenSS_Send(&header, (xdrproc_t)xdr_openss_expinfo, &info);

    /* Write the thread's initial address space to the appropriate file */
    OpenSS_GetDLInfo(getpid(), NULL, tls->time_started, OpenSS_GetTime());
    if(tls->data.objs.objs_len > 0) {
	offline_send_dsos(tls);
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

    if (is_dlopen) {
	defer_trace(0);
    }

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

    int dsoname_len = strlen(dsoname);
    int newsize = (tls->data.objs.objs_len * sizeof(objects)) +
		  (tls->dsoname_len + dsoname_len);


    if(newsize > OpenSS_OBJBufferSize) {
#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
            fprintf(stderr,"offline_record_dso SENDS OBJS for HOST %s, PID %lld, POSIX_TID %llu\n",
        	   tls->dso_header.host, tls->dso_header.pid, tls->dso_header.posix_tid);
	}
#endif
	offline_send_dsos(tls);
    }

    memcpy(&(tls->buffer.objs[tls->data.objs.objs_len]),
           &objects, sizeof(objects));
    tls->data.objs.objs_len++;
    tls->dsoname_len += dsoname_len;

    if (is_dlopen) {
	defer_trace(1);
    }
}


void offline_record_dlopen(const char* dsoname,
			uint64_t a_begin, uint64_t a_end,
			uint64_t t_begin, uint64_t t_end)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    defer_trace(0);

    /* Initialize the offline "dso" blob's header */
    OpenSS_DataHeader local_header;
    OpenSS_InitializeDataHeader(0, /* Experiment */
				1, /* Collector */
				&local_header);
    memcpy(&tls->dso_header, &local_header, sizeof(OpenSS_DataHeader));

    openss_objects objects;

    /* Initialize the offline "dso" blob */
    objects.objname = strdup(dsoname);
    objects.time_begin = t_begin;
    objects.time_end = t_end;
    objects.addr_begin = a_begin;
    objects.addr_end = a_end;
    objects.is_open = 1;

    int dsoname_len = strlen(dsoname);
    int newsize = (tls->data.objs.objs_len * sizeof(objects)) +
		  (tls->dsoname_len + dsoname_len);


    if(newsize > OpenSS_OBJBufferSize) {
#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
            fprintf(stderr,"offline_record_dlopen SENDS OBJS for HOST %s, PID %lld, POSIX_TID %llu\n",
        	   tls->dso_header.host, tls->dso_header.pid, tls->dso_header.posix_tid);
	}
#endif
	offline_send_dsos(tls);
    }

    memcpy(&(tls->buffer.objs[tls->data.objs.objs_len]),
           &objects, sizeof(objects));
    tls->data.objs.objs_len++;
    tls->dsoname_len += dsoname_len;

    defer_trace(1);
}
