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

#include "RuntimeAPI.h"
#include "PapiAPI.h"

static void OpenSS_PAPIerror (int rval, const char *where)
{
	char error_str[PAPI_MAX_STR_LEN];
	PAPI_perror(rval,error_str,PAPI_MAX_STR_LEN);
	unsigned long mytid = PAPI_thread_id();
	fprintf(stderr,"OpenSS_PAPIerror:%s, %d in %lu: %s\n",where,rval,mytid,error_str);
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
	    fprintf(stderr,"The event %#x does not seem to be supported\n",
		event);
	    OpenSS_PAPIerror(rval,"OpenSS_event_exists");
	    return false;
	}

	PAPI_event_info_t info;

	rval = PAPI_get_event_info(event,&info);
	if (rval != PAPI_OK) {
	    fprintf(stderr,"The event %#x does not seem to be supported\n",
		event);
	    OpenSS_PAPIerror(rval,"OpenSS_event_exists");
	    return false;
	}

#if 0
	if (info.count > 0) {
	    fprintf (stderr, "%s (%s) is available on this hardware.\n",
		info.symbol ? info.symbol : "",
		info.short_descr ? info.short_descr : "");
	}
#endif

	if (info.count > 1) {
	    fprintf (stderr,"%s is a derived event on this hardware.\n",
		info.symbol ? info.symbol : "");
	}
}

void OpenSS_Create_Eventset(int *EventSet)
{
    int rval = PAPI_OK;
    if (*EventSet != PAPI_NULL) {
#if 0
	/* Remove all events in the eventset */
	rval = PAPI_cleanup_eventset(*EventSet);
	if (rval != PAPI_OK) {
	    OpenSS_PAPIerror(rval,"OpenSS_Create_Eventset");
	}
	rval = PAPI_destroy_eventset(EventSet);
	if (rval != PAPI_OK) {
	    OpenSS_PAPIerror(rval,"OpenSS_Create_Eventset");
	}
#endif
	*EventSet = PAPI_NULL;
    }

    rval = PAPI_create_eventset(EventSet);
    if (rval != PAPI_OK) {
	OpenSS_PAPIerror(rval,"OpenSS_Create_Eventset");
        return;
    }
}

void OpenSS_AddEvent(int EventSet, int event)
{
    OpenSS_event_exists(event);

    int rval = PAPI_add_event(EventSet,event);

    if ( rval != PAPI_OK) {
	OpenSS_PAPIerror(rval,"OpenSS_AddEvent");
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
        fprintf(stderr,"PAPI_overflow() for event %#x FAILED!\n", event);
	OpenSS_PAPIerror(rval,"OpenSS_Overflow");
    }
}

void OpenSS_Start(int EventSet)
{
    int rval = PAPI_start(EventSet);

    if (rval != PAPI_OK) {
	OpenSS_PAPIerror(rval,"OpenSS_Start");
    }
}

/* if PAPI_ENOTRUN then just return. may want to return rval here...*/
void OpenSS_Stop(int EventSet, long_long* evalues)
{
    if (EventSet == PAPI_NULL) {
#if 0
        fprintf(stderr,"OpenSS_Stop:Evenset is PAPI_NULL!\n");
#endif
        return;
    }

    int rval = PAPI_stop(EventSet,evalues);

    if (rval == PAPI_ENOTRUN) {
	OpenSS_PAPIerror(rval,"OpenSS_Stop");
	return;
    }

    if (rval != PAPI_OK) {
	OpenSS_PAPIerror(rval,"OpenSS_Stop");
    }
}

/* if PAPI_ENOTRUN then just return. may want to return rval here...*/
void OpenSS_HWCAccum(int EventSet, long_long* evalues)
{
    if (EventSet == PAPI_NULL) {
        return;
    }

    int rval = PAPI_accum(EventSet,evalues);

    if (rval == PAPI_ENOTRUN) {
	OpenSS_PAPIerror(rval,"OpenSS_HWCAccum");
	return;
    }

    if (rval != PAPI_OK) {
	OpenSS_PAPIerror(rval,"OpenSS_HWCAccum");
    }
}

/* if PAPI_ENOTRUN then just return. may want to return rval here...*/
void OpenSS_HWCRead(int EventSet, long_long* evalues)
{
    if (EventSet == PAPI_NULL) {
        return;
    }

    int rval = PAPI_read(EventSet,evalues);

    if (rval == PAPI_ENOTRUN) {
	OpenSS_PAPIerror(rval,"OpenSS_HWCRead");
	return;
    }

    if (rval != PAPI_OK) {
	OpenSS_PAPIerror(rval,"OpenSS_HWCRead");
    }
}
