/*******************************************************************************
** Copyright (c) 2007 The Krell Institue. All Rights Reserved.
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

extern void offline_start_sampling(const char* arguments);
extern void offline_stop_sampling(const char* arguments);
extern void offline_record_dso(const char* dsoname);

#if defined(_MONITOR_H_)
void monitor_fini_process(int how, void *data)
#else
void monitor_fini_process(void)
#endif
{
    static int f = 0;
    if (f > 0)
      raise(SIGSEGV);
    f++;

    /*collector stop_sampling does not use the arguments param */
    offline_stop_sampling(NULL);
}

#if defined(_MONITOR_H_)
void *monitor_init_process(int *argc, char **argv, void *data)
#else
void monitor_init_process(char *process, int *argc, char **argv, unsigned pid)
#endif
{
    offline_start_sampling(NULL);
}

void monitor_init_library(void)
{
/* removed previous code that was specific to UTK monitor and
 * only used for debug error checking
 */
}

void monitor_fini_library(void)
{
    static int f = 0;
    if (f > 0)
      raise(SIGSEGV);
    f++;
}

void monitor_fini_thread(void *ptr)
{
    offline_stop_sampling(NULL);
}

#if defined(_MONITOR_H_)
void *monitor_init_thread(int tid, void *data)
#else
void *monitor_init_thread(const unsigned tid)
#endif
{
    void *retval = (void *)0xdeadbeef;
    offline_start_sampling(NULL);
    return(retval);
}

void monitor_init_thread_support(void)
{
}

/* The Rice version of libmonitor added the flags adn
 * handle arguments to monitor dlopen. Use _MONITOR_H_
 * as define by Rice to determine which libmonitor
 * is being used. The UTK monitor package defines libmonitor_h
 */
#if defined(_MONITOR_H_)
void monitor_dlopen(const char *library, int flags, void *handle)
#else
void monitor_dlopen(const char *library)
#endif
{
    /* TODO: if OpenSS_GetDLInfo does not handle errors do so here. */
    int retval = OpenSS_GetDLInfo(getpid(), library);
}

#if defined(_MONITOR_H_)
/* TODO */
/* Rice version of libmonitor.
 * callbacks for handling of fork.
 */
void * monitor_pre_fork(void)
{
    //fprintf(stderr,"OPENSS monitor_pre_fork callback:\n");
    return (NULL);
}

void monitor_post_fork(pid_t child, void *data)
{
    //fprintf(stderr,"OPENSS monitor_post_fork callback:\n");
}
#endif
