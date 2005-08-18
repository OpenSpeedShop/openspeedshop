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

static void OpenSS_PAPIerror (int rval)
{
	char error_str[PAPI_MAX_STR_LEN];
	PAPI_perror(rval,error_str,PAPI_MAX_STR_LEN);
	fprintf(stderr,"PAPI_error %d: %s\n",rval,error_str);
}

typedef int oss_boolean;
#undef false
#undef true
#define false 0
#define true 1
static oss_boolean OpenSS_event_exists (int event)
{
	int rval = PAPI_query_event(event);
	if (rval != PAPI_OK) {
	    fprintf(stderr,"The event %s does not seem to be supported\n",
		get_papi_name(event));
	    OpenSS_PAPIerror(rval);
	    return false;
	}

	PAPI_event_info_t info;

	rval = PAPI_get_event_info(event,&info);
	if (rval != PAPI_OK) {
	    fprintf(stderr,"The event %s does not seem to be supported\n",
		get_papi_name(event));
	    OpenSS_PAPIerror(rval);
	    return false;
	}

	if (info.count > 0) {
	    fprintf (stderr, "%s (%s) is available on this hardware.\n",
		get_papi_name(event), info.short_descr ? info.short_descr : "");
	}

	if (info.count > 1) {
	    fprintf (stderr,"%s is a derived event on this hardware.\n",
		get_papi_name(event));
	}
}

void OpenSS_Create_Eventset(int *EventSet)
{
    int rval = PAPI_OK;
    if (*EventSet != PAPI_NULL) {
	/* Remove all events in the eventset */
	rval = PAPI_cleanup_eventset(*EventSet);
	if (rval != PAPI_OK) {
	    OpenSS_PAPIerror(rval);
	}
	rval = PAPI_destroy_eventset(EventSet);
	if (rval != PAPI_OK) {
	    OpenSS_PAPIerror(rval);
	}
	*EventSet = PAPI_NULL;
    }

    rval = PAPI_create_eventset(EventSet);
    if (rval != PAPI_OK) {
	OpenSS_PAPIerror(rval);
        return;
    }
}

void OpenSS_AddEvent(int EventSet, int event)
{
    OpenSS_event_exists(event);

    int rval = PAPI_add_event(EventSet,event);

    if ( rval != PAPI_OK) {
        fprintf(stderr,"PAPI_add_event FAILED!\n");
	OpenSS_PAPIerror(rval);
        return;
    }
}

void OpenSS_Overflow(int EventSet, int event, int threshold , void *hwcPAPIHandler)
{
    if (EventSet == PAPI_NULL) {
        fprintf(stderr,"OpenSS_Overflow:EventSet is PAPI_NULL!\n");
    }

#if 0
fprintf(stderr,"OpenSS_Overflow: call PAPI_overflow for event %s, with threshold %d\n",get_papi_name(event),threshold);
#endif

    int rval = PAPI_overflow(EventSet,event,threshold, 0, hwcPAPIHandler);

    if (rval != PAPI_OK) {
        fprintf(stderr,"PAPI_overflow() for event %s FAILED!\n", get_papi_name(event));
	OpenSS_PAPIerror(rval);
    }
}

void OpenSS_Start(int EventSet)
{
    int rval = PAPI_start(EventSet);

    if (rval != PAPI_OK) {
        fprintf(stderr,"PAPI_start(EventSet) FAILED!\n");
	OpenSS_PAPIerror(rval);
    }
}

/* if PAPI_ENOTRUN then just return. may want to return rval here...*/
void OpenSS_Stop(int EventSet)
{
    if (EventSet == PAPI_NULL) {
#if 0
        fprintf(stderr,"OpenSS_Stop:Evenset is PAPI_NULL!\n");
#endif
        return;
    }

    int rval = PAPI_stop(EventSet,values);

    if (rval == PAPI_ENOTRUN) {
	OpenSS_PAPIerror(rval);
	return;
    }

    if (rval != PAPI_OK) {
        fprintf(stderr,"PAPI_stop(EventSet,values) FAILED!\n");
	OpenSS_PAPIerror(rval);
    }
}
