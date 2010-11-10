/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2010 The Krell INstitute. All Rights Reserved.
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
 * utility functions for papi
 *
 */

#ifndef _hwc_papi_
#define _hwc_papi_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <papi.h>

/* default papi threshold */
#define THRESHOLD   10000000

#ifdef __cplusplus
extern "C" {
#endif

const PAPI_hw_info_t *hw_info ;       /* PAPI hardware information */
static int papithreshold = THRESHOLD;

static long_long values[2] = { 0, 0 };
extern long_long **allocate_test_space(int , int);
extern void get_papi_name(int, char*);
extern int get_papi_eventcode (char* eventname);
extern void print_papi_error(int);

extern void hwc_init_papi();
extern void print_hw_info();
extern void get_papi_exe_info();
extern void print_papi_events();

void OpenSS_Create_Eventset(int*);
void OpenSS_AddEvent(int, int);
void OpenSS_Overflow(int, int, int, void*);
void OpenSS_Start(int);
void OpenSS_Stop(int, long long *);
void OpenSS_HWCAccum(int,long long *);
void OpenSS_HWCRead(int, long long *);
void OpenSS_PAPIerror (int , const char *);

#ifdef __cplusplus
}
#endif

#endif
