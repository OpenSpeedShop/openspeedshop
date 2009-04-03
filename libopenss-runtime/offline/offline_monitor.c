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

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "monitor.h"
#include "OpenSS_Monitor.h"

extern void offline_start_sampling(const char* arguments);
extern void offline_stop_sampling(const char* arguments, const int finished);
extern void offline_record_dso(const char* dsoname);
extern void offline_defer_sampling(const int flag);

int sampling_active = 0;
int process_is_terminating = 0;
int thread_is_terminating = 0;
OpenSS_Monitor_Type OpenSS_monitor_type = OpenSS_Monitor_Default;

#define OPENSS_HANDLE_UNWIND_SEGV 1

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
    /*collector stop_sampling does not use the arguments param */
    //fprintf(stderr,"monitor_fini_process STOP SAMPLING %d\n",getpid());
    static int f = 0;
    if (f > 0)
      raise(SIGSEGV);
    f++;
    sampling_active = 0;
    process_is_terminating = 1;
    offline_stop_sampling(NULL, 1);
}

void *monitor_init_process(int *argc, char **argv, void *data)
{
    //fprintf(stderr,"monitor_init_process START SAMPLING %d\n",getpid());
    OpenSS_monitor_type = OpenSS_Monitor_Proc;
    sampling_active = 1;
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
    //fprintf(stderr,"monitor_fini_thread STOP SAMPLING %d\n",getpid());
    sampling_active = 0;
    thread_is_terminating = 1;
    offline_stop_sampling(NULL,1);
}

void *monitor_init_thread(int tid, void *data)
{
    //fprintf(stderr,"monitor_init_thread START SAMPLING %d\n",getpid());
    OpenSS_monitor_type = OpenSS_Monitor_Thread;
    sampling_active = 1;
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
    /* TODO:
     * if OpenSS_GetDLInfo does not handle errors do so here.
     */
    int retval = OpenSS_GetDLInfo(getpid(), library);
    if (!sampling_active) {
	//fprintf(stderr,"monitor_dlopen RESUME SAMPLING %d\n",getpid());
	sampling_active = 1;
	offline_start_sampling(NULL);
    }
}

void
monitor_pre_dlopen(const char *path, int flags)
{
    if (sampling_active) {
	//fprintf(stderr,"monitor_pre_dlopen PAUSE SAMPLING %d\n",getpid());
	sampling_active = 0;
	offline_stop_sampling(NULL,0);
    }
}

void
monitor_dlclose(void *handle)
{
    if (!thread_is_terminating || !process_is_terminating) {
	if (sampling_active) {
	    //fprintf(stderr,"monitor_dlclose PAUSE SAMPLING %d\n",getpid());
	    sampling_active = 0;
	    offline_stop_sampling(NULL,0);
	}
    }
}

void
monitor_post_dlclose(void *handle, int ret)
{
    if (!thread_is_terminating || !process_is_terminating) {
	if (!sampling_active) {
	    //fprintf(stderr,"monitor_post_dlclose RESUME SAMPLING %d\n",getpid());
	    sampling_active = 1;
	    offline_start_sampling(NULL);
	}
    }
}

/* 
 * callbacks for handling of FORK.
 */
void * monitor_pre_fork(void)
{
    /* Stop sampling prior to real fork. */
    if (sampling_active) {
	//fprintf(stderr,"monitor_pre_fork PAUSE SAMPLING %d\n",getpid());
	sampling_active = 0;
	offline_stop_sampling(NULL,1);
    }
    return (NULL);
}

void monitor_post_fork(pid_t child, void *data)
{
    /* Resume/start sampling forked process. */
    if (!sampling_active) {
	//fprintf(stderr,"monitor_post_fork RESUME SAMPLING %d\n",getpid());
	OpenSS_monitor_type = OpenSS_Monitor_Proc;
	sampling_active = 1;
	offline_start_sampling(NULL);
    }
}

/*
 * callbacks for handling of MPI programs.
 */

void monitor_mpi_pre_init(void)
{
    if (sampling_active) {
	//fprintf(stderr,"monitor_mpi_pre_init PAUSE SAMPLING %d\n",getpid());
	sampling_active = 0;
	offline_stop_sampling(NULL,0);
    }
}

void
monitor_init_mpi(int *argc, char ***argv)
{
    //fprintf(stderr,"ENTER monitor_init_mpi RANK %s\n",monitor_mpi_comm_rank());
    if (!sampling_active) {
	sampling_active = 1;
	//fprintf(stderr,"monitor_init_mpi RESUME SAMPLING %d\n",getpid());
	OpenSS_monitor_type = OpenSS_Monitor_Proc;
	offline_start_sampling(NULL);
    }
}

void monitor_fini_mpi(void)
{
}

void OpenSS_AdjustStackTrace(int index, unsigned* stacktrace_size,
			     uint64_t* stacktrace)
{
    /*
     * Remove details on libmonitor from stracktraces. 
     * Libmonitor adds two frames to all processes.
     * and one frame for threads started via clone.
     * If we find other cases we can add a new OpenSS_Monitor_Type
     * to deal with them.  OpenSS_Monitor_Default means no change.
     */

    if (OpenSS_monitor_type == OpenSS_Monitor_Proc) {
	/* PROCESS CASE */
        stacktrace[index-4] = stacktrace[index-3];
        stacktrace[index-3] = stacktrace[index-1];
        stacktrace[index-2] = stacktrace[index-1] = 0;
        *stacktrace_size = index - 2;
    } else if (OpenSS_monitor_type == OpenSS_Monitor_Thread) {
	/* THREAD CASE */
        stacktrace[index-3] = stacktrace[index-2];
        stacktrace[index-2] = stacktrace[index-1];
        stacktrace[index-1] = 0;
        *stacktrace_size = index - 1;
    }
}
