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
 * Declaration and definition of the PC sampling collector's runtime.
 *
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
extern char *OpenSS_dsopath;

void monitor_fini_process(void)
{
    static int f = 0;
    if (f > 0)
      raise(SIGSEGV);
    f++;

    /*collector stop_sampling does not use the arguments param */
    offline_stop_sampling(NULL);
}

void monitor_init_process(char *process, int *argc, char **argv, unsigned pid)
{
#if 0
    pcsamp_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    args.collector=1;
    args.sampling_rate=100000;
    args.thread=pid;
    /* how to handle
       Blob arguments(reinterpret_cast<xdrproc_t>(xdr_pcsamp_start_sampling_args), &args);
    */
#endif
    offline_start_sampling(NULL);
}

void monitor_init_library(void)
{
    monitor_opt_error = 0;

#ifndef PROCESS_ONLY
    /* Second test library doesn't monitor errors. */
    monitor_opt_error = MONITOR_NONZERO_EXIT | MONITOR_SIGINT | MONITOR_SIGABRT;
#endif
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

void *monitor_init_thread(const unsigned tid)
{
    void *retval = (void *)0xdeadbeef;
    offline_start_sampling(NULL);
    return(retval);
}

void monitor_init_thread_support(void)
{
}

void monitor_dlopen(const char *library)
{
    offline_record_dso(library);
}
