/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
#include <papi.h>

/* default papi threshold */
#define THRESHOLD   1000000
#define OpenSS_NUMCOUNTERS 6

#ifdef __cplusplus
extern "C" {
#endif

const PAPI_hw_info_t *hw_info ;       /* PAPI hardware information */
static int papithreshold = THRESHOLD;

static long_long values[2] = { 0, 0 };
extern long_long **allocate_test_space(int , int);
extern void get_papi_name(int,char*);
extern int get_papi_eventcode (char* eventname);
extern void print_papi_error(int);

extern void hwc_init_papi();
extern void print_hw_info();
extern void get_papi_exe_info();
extern void print_papi_events();
extern void get_papi_available_presets();
extern bool query_papi_event(int);

#ifdef __cplusplus
}
#endif

#include <string>
#include <vector>

typedef std::pair<std::string, std::string> papi_event;
typedef std::vector<papi_event> papi_available_presets;
papi_available_presets OpenSS_papi_available_presets();

#endif
