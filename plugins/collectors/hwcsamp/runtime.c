/*******************************************************************************
** Copyright (c) 2010 Krell Institute.  All Rights Reserved.
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
 * Declaration and definition of the HWC sampling collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"

#include "PapiAPI.h"
#include "blobs.h"

static int hwc_papi_init_done = 0;

/** Type defining the items stored in thread-local storage. */
typedef struct {

    OpenSS_DataHeader header;  /**< Header for following data blob. */

    hwcsamp_data data;          /**< Actual data blob. */
    OpenSS_HWCPCData buffer;      /**< PC sampling data buffer. */

    int EventSet;

} TLS;

static long_long evalues[6] = { 0, 0, 0, 0, 0, 0 };

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0x00001FF3;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif


#if defined (OPENSS_OFFLINE)
extern void offline_sent_data(int);

static void hwcsampTimerHandler(const ucontext_t* context);

void samp_start_timer()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    if (hwc_papi_init_done == 0) {
	return;
    }
    OpenSS_Start(tls->EventSet);

    OpenSS_Timer(tls->data.interval, hwcsampTimerHandler);

}

void samp_stop_timer()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    OpenSS_Timer(0, NULL);

    if (hwc_papi_init_done == 0 || tls == NULL)
        return;
    OpenSS_Stop(tls->EventSet, evalues);
}

#endif /*OFFLINE*/


static void send_samples(TLS *tls)
{
    tls->header.time_end = OpenSS_GetTime();
    tls->header.addr_begin = tls->buffer.addr_begin;
    tls->header.addr_end = tls->buffer.addr_end;
    tls->data.pc.pc_len = tls->buffer.length;
    tls->data.count.count_len = tls->buffer.length;

    tls->data.events.events_len = tls->buffer.length;

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
int bufsize = tls->buffer.length * sizeof(tls->buffer);
int pcsize =  tls->buffer.length * sizeof(tls->data.pc.pc_val);
int countsize =  tls->buffer.length * sizeof(tls->data.count.count_val);

fprintf(stderr,"send_samples: size of tls data is %d, buffer is %d\n",sizeof(tls->data), sizeof(tls->buffer));
fprintf(stderr,"send_samples: size of tls PC data is %d  %d, COUNT is %d  %d\n",tls->buffer.length , pcsize,tls->buffer.length , countsize);
fprintf(stderr,"send_samples: size of tls HASH is %d\n", sizeof(tls->buffer.hash_table));
fprintf(stderr,"send_samples: size of tls data pc buff is %d\n", sizeof(tls->data.pc.pc_val)*OpenSS_HWCPCBufferSize);
fprintf(stderr,"send_samples: size of tls data count buff is %d\n", sizeof(tls->data.count.count_val)*OpenSS_HWCPCBufferSize);
fprintf(stderr,"send_samples: size of tls hwccounts is %d\n", sizeof(tls->buffer.hwccounts));
fprintf(stderr,"send_samples: size of OpenSS_evcounts is %d\n", sizeof(OpenSS_evcounts)*OpenSS_HWCPCBufferSize);
fprintf(stderr,"send_samples: size of lon long is %d\n", sizeof(long long));
fprintf(stderr,"send_samples: size of uint64_t is %d\n", sizeof(uint64_t));
fprintf(stderr,"send_samples: size of OpenSS_HWCPCData is %d\n", sizeof(OpenSS_HWCPCData));
int eventssize =  tls->buffer.length * sizeof(tls->data.events.events_val);
fprintf(stderr,"send_samples: size of eventssize = %d\n",eventssize);
    }
#endif



#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"send_samples: sends data:\n");
        fprintf(stderr,"time_end(%#lu) addr range [%#lx, %#lx] pc_len(%d)\n",
            tls->header.time_end,tls->header.addr_begin,
            tls->header.addr_end,tls->data.pc.pc_len);
    }
#endif

    OpenSS_Send(&(tls->header),(xdrproc_t)xdr_hwcsamp_data,&(tls->data));

#if defined(OPENSS_OFFLINE)
    offline_sent_data(1);
#endif

    tls->header.time_begin = tls->header.time_end;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;

    tls->data.pc.pc_len = 0;
    tls->data.count.count_len = 0;
    memset(tls->buffer.hash_table, 0, sizeof(tls->buffer.hash_table));
    memset(tls->buffer.hwccounts, 0, sizeof(tls->buffer.hwccounts));
    tls->data.events.events_len = 0;
}


/**
 * Timer event handler.
 *
 * Called by the timer handler each time a sample is to be taken. Extracts the
 * program counter (PC) address from the signal context and places it into the
 * sample buffer. When the sample buffer is full, it is sent to the framework
 * for storage in the experiment's database.
 *
 * @param context    Thread context at timer interrupt.
 */
static void hwcsampTimerHandler(const ucontext_t* context)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    
    /* Obtain the program counter (PC) address from the thread context */
    uint64_t pc = OpenSS_GetPCFromContext(context);

    /* This is supposed to reset counters */
    OpenSS_HWCAccum(tls->EventSet, evalues);

    if(OpenSS_UpdateHWCPCData(pc, &tls->buffer,evalues)) {

#ifndef NDEBUG
        if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
            fprintf(stderr,"sampTimerHandler sends data:\n");
            fprintf(stderr,"time_end(%#lu) addr range [%#lx, %#lx] pc_len(%d) count_len(%d)\n",
                tls->header.time_end,tls->header.addr_begin,
		tls->header.addr_end,tls->data.pc.pc_len,
                tls->data.count.count_len);
        }
#endif

	send_samples(tls);
    }

    /* reset our values */
    memset(evalues,0,sizeof(evalues));

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
      int i;
      for (i = 0; i < 6; i++) {
	if (tls->buffer.hwccounts[tls->buffer.length-1][i] > 0) {
	    fprintf(stderr,"%#lx sampTimerHandler %d count %d is %ld\n",pc,tls->buffer.length-1,i, tls->buffer.hwccounts[tls->buffer.length-1][i]);
	}
      }
    }
#endif
}



/**
 * Start sampling.
 *
 * Starts program counter (PC) sampling for the thread executing this function.
 * Initializes the appropriate thread-local data structures and then enables the
 * sampling timer.
 *
 * @param arguments    Encoded function arguments.
 */
void hwcsamp_start_sampling(const char* arguments)
{
    hwcsamp_start_sampling_args args;

    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    OpenSS_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Decode the passed function arguments */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
			    (xdrproc_t)xdr_hwcsamp_start_sampling_args,
			    &args);
    
    /* 
     * Initialize the data blob's header
     *
     * Passing &tls->header to OpenSS_InitializeDataHeader() was found
     * to not be safe on IA64 systems. Hopefully the extra copy can be
     * removed eventually.
     */
    
    OpenSS_DataHeader local_data_header;
    OpenSS_InitializeDataHeader(args.experiment, args.collector,
				&local_data_header);
    memcpy(&tls->header, &local_data_header, sizeof(OpenSS_DataHeader));

    OpenSS_SetSendToFile(&(tls->header), "hwcsamp", "openss-data");
    tls->data.events.events_val = tls->buffer.hwccounts;
    
    /* Initialize the actual data blob */
    tls->data.interval = 
	(uint64_t)(1000000000) / (uint64_t)(args.sampling_rate);
    tls->data.pc.pc_val = tls->buffer.pc;
    tls->data.count.count_val = tls->buffer.count;

    /* Initialize the sampling buffer */
    tls->buffer.addr_begin = ~0;
    tls->buffer.addr_end = 0;
    tls->buffer.length = 0;
    memset(tls->buffer.hash_table, 0, sizeof(tls->buffer.hash_table));

    /* Begin sampling */
    tls->header.time_begin = OpenSS_GetTime();

    memset(tls->buffer.hwccounts, 0, sizeof(tls->buffer.hwccounts));
    if(hwc_papi_init_done == 0) {
        hwc_init_papi();
        tls->EventSet = PAPI_NULL;
        hwc_papi_init_done = 1;
    }

    memset(evalues,0,sizeof(evalues));


    OpenSS_Create_Eventset(&tls->EventSet);

    int rval = PAPI_OK;

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
       fprintf(stderr, "PAPI Version: %d.%d.%d.%d\n", PAPI_VERSION_MAJOR( PAPI_VERSION ),
                        PAPI_VERSION_MINOR( PAPI_VERSION ),
                        PAPI_VERSION_REVISION( PAPI_VERSION ),
                        PAPI_VERSION_INCREMENT( PAPI_VERSION ) );
       fprintf(stderr,"System has %d hardware counters.\n", PAPI_num_counters());
    }
#endif


#if (PAPI_VERSION_MAJOR(PAPI_VERSION)>=4)
    rval = PAPI_assign_eventset_component( tls->EventSet, 0 );
    if (rval != PAPI_OK) {
        OpenSS_PAPIerror(rval,"OpenSS_Create_Eventset assign_eventset_component");
        return;
    }
#endif

#if !defined(TARGET_OS_BGP) 
    rval = PAPI_set_multiplex( tls->EventSet );
    if ( rval == PAPI_ENOSUPP) {
        fprintf(stderr,"OpenSS_Create_Eventset: Multiplex not supported\n");
    } else if (rval != PAPI_OK)  {
        OpenSS_PAPIerror(rval,"OpenSS_Create_Eventset set_multiplex");
        return;
    }
#endif


#if defined (OPENSS_OFFLINE)
    const char* hwc_event_param = getenv("OPENSS_HWCSAMP_EVENTS");
    if (hwc_event_param != NULL) {
	char *tfptr, *saveptr, *tf_token;
	tfptr = strdup(hwc_event_param);
	int i;
	for (i = 1;  ; i++, tfptr = NULL) {
	    tf_token = strtok_r(tfptr, ":,", &saveptr);
	    if (tf_token == NULL) {
		break;
	    }
	    OpenSS_AddEvent(tls->EventSet, get_papi_eventcode(tf_token));
	    if (tfptr) free(tfptr);
	}
	
    } else {
	OpenSS_AddEvent(tls->EventSet, get_papi_eventcode("PAPI_TOT_CYC"));
    }
#else
    /* TODO: verify setting events via passed arguments for online */
    OpenSS_AddEvent(tls->EventSet, get_papi_eventcode("PAPI_TOT_CYC"));
#endif

    OpenSS_Start(tls->EventSet);

    OpenSS_Timer(tls->data.interval, hwcsampTimerHandler);
}



/**
 * Stop sampling.
 *
 * Stops program counter (PC) sampling for the thread executing this function.
 * Disables the sampling timer and sends any samples remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void hwcsamp_stop_sampling(const char* arguments)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = OpenSS_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    Assert(tls != NULL);

    /* Stop sampling */
    OpenSS_Timer(0, NULL);

    tls->header.time_end = OpenSS_GetTime();

    if (tls->EventSet == PAPI_NULL) {
	return;
    }
    OpenSS_Stop(tls->EventSet, evalues);

    /* Are there any unsent samples? */
    if(tls->buffer.length > 0) {

#ifndef NDEBUG
	if (getenv("OPENSS_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr, "samp_stop_sampling:\n");
	    fprintf(stderr, "time_end(%#lu) addr range[%#lx, %#lx] pc_len(%d) count_len(%d)\n",
		tls->header.time_end,tls->header.addr_begin,
		tls->header.addr_end,tls->data.pc.pc_len,
		tls->data.count.count_len);
	}
#endif

	send_samples(tls);

    }

    /* Destroy our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    free(tls);
    OpenSS_SetTLS(TLSKey, NULL);
#endif
}
