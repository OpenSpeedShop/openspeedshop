/*******************************************************************************
** Copyright (c) 2007-2011 The Krell Institue. All Rights Reserved.
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
 * monitor_mpi_comm_size(), monitor_mpi_comm_rank(), *argc);
 * monitor_fini_mpi(void)
 * monitor_mpi_pcontrol(int level)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>
#include "monitor.h"
#include "OpenSS_Monitor.h"
#include "OpenSS_Offline.h"

extern void offline_start_sampling(const char* arguments);
extern void offline_stop_sampling(const char* arguments, const int finished);
extern void offline_pause_sampling();
extern void offline_resume_sampling();

/** Type defining the items stored in thread-local storage. */
typedef struct {
    int debug;
    int in_mpi_pre_init;
    OpenSS_Monitor_Status sampling_status;
    int process_is_terminating;
    int thread_is_terminating;
    pthread_t tid;
    pid_t pid;
    OpenSS_Monitor_Type OpenSS_monitor_type;

    oss_dlinfoList *oss_dllist_curr, *oss_dllist_head;
} TLS;

int OpenSS_in_mpi_startup = 0;

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

#if 0
    Assert(tls != NULL);
#else
    /* The assert above can cause libmonitor to hang with mpt.
     * We may have forked a process which then did not call
     * monitor_init_process and allocate TLS.
     */
    if (tls == NULL) {
       if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
         fprintf(stderr,"Warning. monitor_fini_process called with no TLS.\n");
       }
         return;
    }
#endif 


   if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
   } else {
	tls->debug=0;
   }
    /*collector stop_sampling does not use the arguments param */
    if (tls->debug) {
	fprintf(stderr,"monitor_fini_process FINISHED SAMPLING %d,%lu\n",
		tls->pid,tls->tid);
    }

    static int f = 0;
    if (f > 0)
      raise(SIGSEGV);
    f++;

    tls->sampling_status = OpenSS_Monitor_Finished;
    if(how == MONITOR_EXIT_EXEC) {
	tls->process_is_terminating = 1;
	offline_stop_sampling(NULL, 1);
    } else {
	tls->process_is_terminating = 1;
	offline_stop_sampling(NULL, 1);
    }
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

#if 0
	/* get the identifier of this thread */
	pthread_t (*f_pthread_self)();
	f_pthread_self = (pthread_t (*)())dlsym(RTLD_DEFAULT, "pthread_self");
	tls->tid = (f_pthread_self != NULL) ? (*f_pthread_self)() : 0;
#else
	tls->tid = 0;
#endif


    } else {
	tls->debug=0;
    }

    tls->pid = getpid();
    tls->oss_dllist_head = NULL;

    if (tls->debug) {
	fprintf(stderr,"monitor_init_process BEGIN SAMPLING %d,%lu\n",
		tls->pid,tls->tid);
    }

    if (OpenSS_in_mpi_startup || tls->in_mpi_pre_init == 1) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_init_process returns early due to in mpi init\n");
	}
	return;
    }

    tls->in_mpi_pre_init = 0;
    tls->OpenSS_monitor_type = OpenSS_Monitor_Proc;

    /* Start with gathering data disabled if environment variable is set */
    if ( (getenv("OPENSS_ENABLE_MPI_PCONTROL") != NULL) && !(getenv("OPENSS_START_ENABLED") != NULL)) {
      if (tls->debug) {
        fprintf(stderr,"monitor_init_process OPENSS_START_ENABLED was NOT set. Skip starting sampling at start-up time. \n");
      }
      tls->sampling_status = OpenSS_Monitor_Not_Started;
    } else if ( (getenv("OPENSS_ENABLE_MPI_PCONTROL") != NULL) && (getenv("OPENSS_START_ENABLED") != NULL)) {
      if (tls->debug) {
        fprintf(stderr,"monitor_init_process OPENSS_START_ENABLED was set.  Start gathering from beginning of program.\n");
      }
      tls->sampling_status = OpenSS_Monitor_Started;
      offline_start_sampling(NULL);
    } else {
      if (tls->debug) {
        fprintf(stderr,"monitor_init_process OPENSS_ENABLE_MPI_PCONTROL was NOT set.  Normal start-up path.\n");
      }
      tls->sampling_status = OpenSS_Monitor_Started;
      offline_start_sampling(NULL);
    }
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

#ifdef HAVE_TARGET_POSIX_THREADS
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

    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
    } else {
	tls->debug=0;
    }

    if (tls->debug) {
	fprintf(stderr,"monitor_fini_thread FINISHED SAMPLING %d,%lu\n",
		tls->pid,tls->tid);
    }

    tls->sampling_status = OpenSS_Monitor_Finished;
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

#if 0
	/* get the identifier of this thread */
	pthread_t (*f_pthread_self)();
	f_pthread_self = (pthread_t (*)())dlsym(RTLD_DEFAULT, "pthread_self");
	tls->tid = (f_pthread_self != NULL) ? (*f_pthread_self)() : 0;
#else
	tls->tid = 0;
#endif


    } else {
	tls->debug=0;
    }


    if (OpenSS_in_mpi_startup || tls->in_mpi_pre_init == 1) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_init_thread returns early due to in mpi init\n");
	}
	return;
    }

    tls->pid = getpid();
    tls->oss_dllist_head = NULL;

    if (tls->debug) {
	fprintf(stderr,"monitor_init_thread BEGIN SAMPLING %d,%lu\n",
		tls->pid,tls->tid);
    }

    tls->in_mpi_pre_init = 0;
    tls->OpenSS_monitor_type = OpenSS_Monitor_Thread;


    /* Start with gathering data disabled if environment variable is set */
    if ( (getenv("OPENSS_ENABLE_MPI_PCONTROL") != NULL) && !(getenv("OPENSS_START_ENABLED") != NULL)) {
       if (tls->debug) {
         fprintf(stderr,"monitor_init_thread OPENSS_START_ENABLED was NOT set. Do not start gathering performance data. \n");
       }
       tls->sampling_status = OpenSS_Monitor_Not_Started;
    } else {
       if (tls->debug) {
          fprintf(stderr,"monitor_init_thread OPENSS_START_DISABLED was NOT set \n");
       }
       tls->sampling_status = OpenSS_Monitor_Started;
       offline_start_sampling(NULL);
    }

    return(data);
}

void monitor_init_thread_support(void)
{
}

#endif

#ifdef HAVE_TARGET_DLOPEN
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
    if (tls == NULL || tls && tls->sampling_status == 0 ) {
	return;
    }

    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
    } else {
	tls->debug=0;
    }

    if (OpenSS_in_mpi_startup || tls->in_mpi_pre_init == 1) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_dlopen returns early due to in mpi init\n");
	}
	return;
    }

    if (library == NULL) {
	if (tls->debug) {
	    fprintf(stderr,"monitor_dlopen ignores null library name\n");
	}
	return;
    }

    if (tls->debug) {
	fprintf(stderr,"monitor_dlopen called with %s , handle %p, for %d,%lu\n",
	    library, handle, tls->pid,tls->tid);
    }

    tls->oss_dllist_curr = (oss_dlinfoList*)malloc(sizeof(oss_dlinfoList));
    tls->oss_dllist_curr->oss_dlinfo_entry.load_time = OpenSS_GetTime();
    tls->oss_dllist_curr->oss_dlinfo_entry.unload_time = OpenSS_GetTime() + 1;
    tls->oss_dllist_curr->oss_dlinfo_entry.name = strdup(library);
    tls->oss_dllist_curr->oss_dlinfo_entry.handle = handle;
    tls->oss_dllist_curr->oss_dlinfo_next = tls->oss_dllist_head;
    tls->oss_dllist_head = tls->oss_dllist_curr;

    if (tls->sampling_status == OpenSS_Monitor_Paused && !tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_dlopen RESUME SAMPLING %d,%lu\n",
		tls->pid,tls->tid);
        }
	tls->sampling_status = OpenSS_Monitor_Resumed;
	offline_resume_sampling();
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
    if (tls == NULL || tls && tls->sampling_status == 0 ) {
	return;
    }

    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
    } else {
	tls->debug=0;
    }

    if (tls->in_mpi_pre_init == 1) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_pre_dlopen returns early due to in mpi init\n");
	}
	return;
    }

    if (path == NULL) {
	if (tls->debug) {
	    fprintf(stderr,"monitor_pre_dlopen ignores null path\n");
	}
	return;
    }

    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
    } else {
	tls->debug=0;
    }

    if (tls->debug) {
	fprintf(stderr,"monitor_pre_dlopen %s for %d,%lu\n",
		path, tls->pid,tls->tid);
    }

    if ((tls->sampling_status == OpenSS_Monitor_Started ||
       /*  tls->sampling_status == OpenSS_Monitor_Not_Started || */
	 tls->sampling_status == OpenSS_Monitor_Resumed) && !tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_pre_dlopen PAUSE SAMPLING %d,%lu\n",
		tls->pid,tls->tid);
        }
	tls->sampling_status = OpenSS_Monitor_Paused;
	offline_pause_sampling();
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


    if (tls == NULL || tls && tls->sampling_status == 0 ) {
	return;
    }

    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
    } else {
	tls->debug=0;
    }

    if (tls->in_mpi_pre_init == 1) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_dlclose returns early due to in mpi init\n");
	}
	return;
    }

    while (tls->oss_dllist_curr) {
	if (tls->oss_dllist_curr->oss_dlinfo_entry.handle == handle) {
	   tls->oss_dllist_curr->oss_dlinfo_entry.unload_time = OpenSS_GetTime();

            if (tls->debug) {
	        fprintf(stderr,"FOUND %p %s\n",handle, tls->oss_dllist_curr->oss_dlinfo_entry.name);
	        fprintf(stderr,"loaded at %d, unloaded at %d\n",
		               tls->oss_dllist_curr->oss_dlinfo_entry.load_time,
		               tls->oss_dllist_curr->oss_dlinfo_entry.unload_time);
	    }

	   /* On some systems (NASA) it appears that dlopen can be called
	    * before monitor_init_process (or even monitor_early_init).
	    * So we need to use getpid() directly here.
	    */ 

	   int retval = OpenSS_GetDLInfo(getpid(),
					 tls->oss_dllist_curr->oss_dlinfo_entry.name,
					 tls->oss_dllist_curr->oss_dlinfo_entry.load_time,
					 tls->oss_dllist_curr->oss_dlinfo_entry.unload_time
					);
	   break;
	}
	tls->oss_dllist_curr = tls->oss_dllist_curr->oss_dlinfo_next;
    }

    if (!tls->thread_is_terminating || !tls->process_is_terminating) {
	if ((tls->sampling_status == OpenSS_Monitor_Started ||
        /*     tls->sampling_status == OpenSS_Monitor_Not_Started || */
	     tls->sampling_status == OpenSS_Monitor_Resumed) && !tls->in_mpi_pre_init) {
            if (tls->debug) {
	        fprintf(stderr,"monitor_dlclose PAUSE SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
            }
	    tls->sampling_status = OpenSS_Monitor_Paused;
	    offline_pause_sampling();
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

    if (tls == NULL || tls && tls->sampling_status == 0 ) {
	return;
    }

    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
    } else {
	tls->debug=0;
    }

    if (tls->in_mpi_pre_init == 1) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_post_dlclose returns early due to in mpi init\n");
	}
	return;
    }

    if (!tls->thread_is_terminating || !tls->process_is_terminating) {
	if (tls->sampling_status == OpenSS_Monitor_Paused && !tls->in_mpi_pre_init) {
            if (tls->debug) {
	        fprintf(stderr,"monitor_post_dlclose RESUME SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
            }
	    tls->sampling_status = OpenSS_Monitor_Resumed;
	    offline_resume_sampling();
	}
    }
}

#endif

#ifdef HAVE_TARGET_FORK
/* 
 * callbacks for handling of FORK.
 */
void * monitor_pre_fork(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    /* The sgi MPT mpi startup apparently can fork
     * a process such that monitor does not go
     * through its normal path and our tls is not
     * allocated as we expect.
     */
    TLS* tls = OpenSS_GetTLS(TLSKey);
    if (tls == NULL) {
	tls = malloc(sizeof(TLS));
    }
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

    if (OpenSS_in_mpi_startup || tls->in_mpi_pre_init == 1) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_pre_fork returns early due to in mpi init\n");
	}
	return;
    }

    /* Stop sampling prior to real fork. */
    if (tls->sampling_status == OpenSS_Monitor_Paused ||
	tls->sampling_status == OpenSS_Monitor_Started) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_pre_fork FINISHED SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = OpenSS_Monitor_Finished;
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


    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
    } else {
	tls->debug=0;
    }
    if (OpenSS_in_mpi_startup || tls->in_mpi_pre_init == 1) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_post_fork returns early due to in mpi init\n");
	}
	return;
    }

    /* Resume/start sampling forked process. */
    if (tls->sampling_status == OpenSS_Monitor_Paused ||
	tls->sampling_status == OpenSS_Monitor_Finished) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_post_fork BEGIN SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->OpenSS_monitor_type = OpenSS_Monitor_Proc;
	tls->sampling_status = 1;
	offline_start_sampling(NULL);
    }
}
#endif

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
    OpenSS_in_mpi_startup = 1;

    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
    } else {
	tls->debug=0;
    }

    if (tls->sampling_status == OpenSS_Monitor_Started) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_mpi_pre_init PAUSE SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = OpenSS_Monitor_Paused;
	offline_pause_sampling();
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

    if (tls->sampling_status == OpenSS_Monitor_Paused) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_init_mpi RESUME SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = OpenSS_Monitor_Resumed;
	tls->OpenSS_monitor_type = OpenSS_Monitor_Proc;
	offline_resume_sampling();
    }

    tls->in_mpi_pre_init = 0;
    OpenSS_in_mpi_startup = 0;
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

    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
    } else {
	tls->debug=0;
    }

    if (tls->debug) {
	fprintf(stderr,"monitor_fini_mpi CALLED %d,%lu\n",
		tls->pid,tls->tid);
    }

    if (tls->sampling_status == OpenSS_Monitor_Started ||
	tls->sampling_status == OpenSS_Monitor_Resumed) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_fini_mpi PAUSE SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = OpenSS_Monitor_Paused;
	offline_pause_sampling();
    }
}

void monitor_mpi_post_fini(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
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
	fprintf(stderr,"monitor_mpi_post_fini CALLED %d,%lu\n",
		tls->pid,tls->tid);
    }

    if (tls->sampling_status == OpenSS_Monitor_Paused) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_mpi_post_fini RESUME SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = OpenSS_Monitor_Resumed;
	offline_resume_sampling();
    }
}

/* monitor_mpi_pcontrol is reponsible for starting and stopping data
 * collection based on the user coding:
 *
 *     MPI_Pcontrol(0) to disable collection
 *     and
 *     MPI_Pcontrol(1) to reenable data collection
 *
*/
void monitor_mpi_pcontrol(int level)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
  TLS* tls = OpenSS_GetTLS(TLSKey);
#else
  TLS* tls = &the_tls;
#endif
  Assert(tls != NULL);

  if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
	tls->debug=1;
  } else {
	tls->debug=0;
  }

  if ( (getenv("OPENSS_ENABLE_MPI_PCONTROL") != NULL)) {

    if (tls->debug) {
	fprintf(stderr,"monitor_mpi_pcontrol CALLED %d,%lu\n", tls->pid,tls->tid);
    }


    if (level == 0) {
       if ((tls->sampling_status == OpenSS_Monitor_Started ||
   	    tls->sampling_status == OpenSS_Monitor_Resumed) && !tls->in_mpi_pre_init) {

	   tls->sampling_status = OpenSS_Monitor_Paused;
	   offline_pause_sampling();

           if (tls->debug) {
	       fprintf(stderr,"monitor_mpi_pcontrol PAUSE SAMPLING %d,%lu\n", tls->pid,tls->tid);
           }
       }
    } else if  (level == 1) {
       if (tls->sampling_status == OpenSS_Monitor_Not_Started ) {

           if (tls->debug) {
	      fprintf(stderr,"monitor_mpi_pcontrol RESUME SAMPLING with start_sampling call %d,%lu\n", tls->pid,tls->tid);
           }
           tls->OpenSS_monitor_type = OpenSS_Monitor_Proc;
           tls->sampling_status = OpenSS_Monitor_Started;
           offline_start_sampling(NULL);

       } else if (tls->sampling_status == OpenSS_Monitor_Paused && !tls->in_mpi_pre_init) {

	   tls->sampling_status = OpenSS_Monitor_Resumed;
	   offline_resume_sampling();

           if (tls->debug) {
	       fprintf(stderr,"monitor_mpi_pcontrol RESUME SAMPLING %d,%lu\n", tls->pid,tls->tid);
           }
       }
    } else if  (level == 2) {
	fprintf(stderr,"monitor_mpi_pcontrol CALLED with unsupported level=%d\n", level);
    } else {
	fprintf(stderr,"monitor_mpi_pcontrol CALLED with unsupported level=%d\n", level);
    }
  } else {
      /* early return - do not honor mpi_pcontrol */
#if 0
      if (tls->debug) {
  	fprintf(stderr,"monitor_mpi_pcontrol CALLED OPENSS_ENABLE_MPI_PCONTROL **NOT** SET IGNORING MPI_PCONTROL CALL %d,%lu\n", tls->pid,tls->tid);
      }
#endif
      return;
 }

}
