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
 * functions for papi
 *
 */

#include "PapiAPI.h"

void OpenSS_Create_Eventset(int* EventSet)
{
    int rval = PAPI_create_eventset(EventSet);
    if (rval != PAPI_OK) {
        if (rval == PAPI_EINVAL ) {
           fprintf(stderr,"PAPI_create_eventset returns PAPI_EINVAL.\n");
        }
        if (rval == PAPI_ENOMEM ) {
           fprintf(stderr,"PAPI_create_eventset returns PAPI_ENOMEM.\n");
        }
        char error_str[PAPI_MAX_STR_LEN];
        PAPI_perror(rval,error_str,PAPI_MAX_STR_LEN);
        fprintf(stderr,"PAPI_error %d: %s\n",rval,error_str);
        fprintf(stderr,"PAPI_strerror %d: %s\n",rval,PAPI_strerror(rval));
        return;
    }
}

void OpenSS_AddEvent(int EventSet, int event)
{
    int rval = PAPI_add_event(EventSet,event);

    if ( rval != PAPI_OK) {
        fprintf(stderr,"PAPI_add_event FAILED!\n");
        return;
    }
}

void OpenSS_Overflow(int EventSet, int event, int threshold , void *hwcPAPIHandler)
{
    if (EventSet == PAPI_NULL) {
        fprintf(stderr,"OpenSS_Overflow:EventSet is PAPI_NULL!\n");
    }

    int rval = PAPI_overflow(EventSet,event,threshold, 0, hwcPAPIHandler);

    if (rval != PAPI_OK) {
        fprintf(stderr,"PAPI_overflow(...,...) FAILED!\n");
    }
}

void OpenSS_Start(int EventSet)
{
    if (EventSet == PAPI_NULL) {
        fprintf(stderr,"OpenSS_Start: EventSet is PAPI_NULL!\n");
    }

    int rval = PAPI_start(EventSet);

    if (rval != PAPI_OK) {
        fprintf(stderr,"PAPI_start(EventSet) FAILED!\n");
    }
}

void OpenSS_Stop(int EventSet)
{
    if (EventSet == PAPI_NULL) {
        fprintf(stderr,"OpenSS_Stop:Evenset is PAPI_NULL!\n");
        return;
    }

    int rval = PAPI_stop(EventSet,values);

    if (rval != PAPI_OK) {
        fprintf(stderr,"PAPI_stop(EventSet,values) FAILED!\n");
    }
}
