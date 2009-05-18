/*******************************************************************************
** Copyright (c) 2007,2008,2009 The Krell Institue. All Rights Reserved.
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
 * Declaration and definition of the offline libmonitor callbacks
 * that we will override.
 *
 */

/*
 * The Rice libmonitor package defines _MONITOR_H_
 * and these callbacks we can use to monitor a process.
 *
 * monitor_init_library(void)
 * monitor_fini_library(void)
 * monitor_pre_fork(void)
 * monitor_post_fork(pid_t child, void *data)
 * monitor_init_process(int *argc, char **argv, void *data)  Openss callback.
 * monitor_fini_process(int how, void *data)  Openss callback.
 * monitor_thread_pre_create(void)
 * monitor_thread_post_create(void *data)
 * monitor_init_thread_support(void)
 * monitor_init_thread(int tid, void *data)
 * monitor_fini_thread(void *data)  Openss callback.
 * monitor_dlopen(const char *path, int flags, void *handle)  Openss callback.
 * monitor_dlclose(void *handle)
 * monitor_init_mpi(int *argc, char ***argv)
                  monitor_mpi_comm_size(), monitor_mpi_comm_rank(), *argc);
 * monitor_fini_mpi(void)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "monitor.h"
#include "OpenSS_Monitor.h"

extern void offline_start_sampling(const char* arguments);
extern void offline_stop_sampling(const char* arguments, const int finished);
extern void offline_record_dso(const char* dsoname);
extern void offline_defer_sampling(const int flag);

/** Type defining the items stored in thread-local storage. */
typedef struct {
    int debug;
    int in_mpi_pre_init;
    int sampling_active;
    int process_is_terminating;
    int thread_is_terminating;
    OpenSS_Monitor_Type OpenSS_monitor_type;
} TLS;

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0x0000FAB1;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif


/* #define OPENSS_HANDLE_UNWIND_SEGV 1 */

#if defined(OPENSS_HANDLE_UNWIND_SEGV)
#include <setjmp.h>
sigjmp_buf unwind_jmp;
volatile int OpenSS_numsegv;
volatile int OpenSS_unwinding;

int
OpenSS_SEGVhandler(int sig, siginfo_t *siginfo, void *context)
{
    if (OpenSS_unwinding) {
        OpenSS_numsegv++;
        siglongjmp(unwind_jmp,9);
    }
    return 1;
}

int OpenSS_SetSEGVhandler(void)
{
    int rval = monitor_sigaction(SIGSEGV, &OpenSS_SEGVhandler, 0, NULL);

    if (rval != 0) {
        fprintf(stderr,"Unable to install SIGSEGV handler", __FILE__, __LINE__);
    }

    return rval;
}
#endif

/*
 * callbacks for handling of PROCESS
 */
void monitor_fini_process(int how, void *data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /*collector stop_sampling does not use the arguments param */
    if (tls->debug) {
	fprintf(stderr,"monitor_fini_process FINISHED SAMPLING %d\n",getpid());
    }

    static int f = 0;
    if (f > 0)
      raise(SIGSEGV);
    f++;

    tls->sampling_active = 0;
    tls->process_is_terminating = 1;
    offline_stop_sampling(NULL, 1);
}

void *monitor_init_process(int *argc, char **argv, void *data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    OpenSS_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
    } else {
	tls->debug=0;
    }

    if (tls->debug) {
	fprintf(stderr,"monitor_init_process BEGIN SAMPLING %d\n",getpid());
    }

    tls->in_mpi_pre_init = 0;
    tls->OpenSS_monitor_type = OpenSS_Monitor_Proc;
    tls->sampling_active = 1;
    offline_start_sampling(NULL);
    return (data);
}

/*
 * callbacks for handling of monitor init
 */
void monitor_init_library(void)
{
}

void monitor_fini_library(void)
{
    static int f = 0;
    if (f > 0)
      raise(SIGSEGV);
    f++;
}

/*
 * callbacks for handling of THREADS
 */
void monitor_fini_thread(void *ptr)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (tls->debug) {
	fprintf(stderr,"monitor_init_thread FINISHED SAMPLING %d\n",getpid());
    }

    tls->sampling_active = 0;
    tls->thread_is_terminating = 1;
    offline_stop_sampling(NULL,1);
}

void *monitor_init_thread(int tid, void *data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    OpenSS_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
    } else {
	tls->debug=0;
    }

    if (tls->debug) {
	fprintf(stderr,"monitor_init_thread BEGIN SAMPLING %d\n",getpid());
    }

    tls->in_mpi_pre_init = 0;
    tls->OpenSS_monitor_type = OpenSS_Monitor_Thread;
    tls->sampling_active = 1;
    offline_start_sampling(NULL);
    return(data);
}

void monitor_init_thread_support(void)
{
}

/*
 * callbacks for handling of DLOPEN/DLCLOSE.
 */
void monitor_dlopen(const char *library, int flags, void *handle)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* TODO:
     * if OpenSS_GetDLInfo does not handle errors do so here.
     */
    int retval = OpenSS_GetDLInfo(getpid(), library);
    if (!tls->sampling_active && !tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_dlopen RESUME SAMPLING %d\n",getpid());
        }
	tls->sampling_active = 1;
	offline_start_sampling(NULL);
    }
}

void
monitor_pre_dlopen(const char *path, int flags)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (tls->sampling_active && !tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_pre_dlopen PAUSE SAMPLING %d\n",getpid());
        }
	tls->sampling_active = 0;
	offline_stop_sampling(NULL,0);
    }
}

void
monitor_dlclose(void *handle)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (!tls->thread_is_terminating || !tls->process_is_terminating) {
	if (tls->sampling_active && !tls->in_mpi_pre_init) {
            if (tls->debug) {
	        fprintf(stderr,"monitor_dlclose PAUSE SAMPLING %d\n",getpid());
            }
	    tls->sampling_active = 0;
	    offline_stop_sampling(NULL,0);
	}
    }
}

void
monitor_post_dlclose(void *handle, int ret)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (!tls->thread_is_terminating || !tls->process_is_terminating) {
	if (!tls->sampling_active && !tls->in_mpi_pre_init) {
            if (tls->debug) {
	        fprintf(stderr,"monitor_dlclose RESUME SAMPLING %d\n",getpid());
            }
	    tls->sampling_active = 1;
	    offline_start_sampling(NULL);
	}
    }
}

/* 
 * callbacks for handling of FORK.
 */
void * monitor_pre_fork(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Stop sampling prior to real fork. */
    if (tls->sampling_active) {
        if (tls->debug) {
	     fprintf(stderr,"monitor_pre_fork FINISHED SAMPLING %d\n",getpid());
        }
	tls->sampling_active = 0;
	offline_stop_sampling(NULL,1);
    }
    return (NULL);
}

void monitor_post_fork(pid_t child, void *data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Resume/start sampling forked process. */
    if (!tls->sampling_active) {
        if (tls->debug) {
	     fprintf(stderr,"monitor_post_fork STARTS SAMPLING %d\n",getpid());
        }
	tls->OpenSS_monitor_type = OpenSS_Monitor_Proc;
	tls->sampling_active = 1;
	offline_start_sampling(NULL);
    }
}

/*
 * callbacks for handling of MPI programs.
 */

void monitor_mpi_pre_init(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->in_mpi_pre_init = 1;

    if (tls->sampling_active) {
        if (tls->debug) {
	     fprintf(stderr,"monitor_mpi_pre_init PAUSE SAMPLING %d\n",getpid());
        }
	tls->sampling_active = 0;
	offline_stop_sampling(NULL,0);
    }
}

void
monitor_init_mpi(int *argc, char ***argv)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (!tls->sampling_active) {
        if (tls->debug) {
	     fprintf(stderr,"monitor_init_mpi RESUME SAMPLING %d\n",getpid());
        }
	tls->sampling_active = 1;
	tls->OpenSS_monitor_type = OpenSS_Monitor_Proc;
	offline_start_sampling(NULL);
    }

    tls->in_mpi_pre_init = 0;
}

void monitor_fini_mpi(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    if (tls->debug) {
	fprintf(stderr,"monitor_fini_mpi CALLED %d\n",getpid());
    }
}
