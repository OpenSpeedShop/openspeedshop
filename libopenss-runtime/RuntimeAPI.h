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
 * Declaration of the Runtime API.
 *
 */

#ifndef _OpenSpeedShop_Runtime_RuntimeAPI_
#define _OpenSpeedShop_Runtime_RuntimeAPI_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.h"
#include "OpenSS_DataHeader.h"

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <rpc/rpc.h>
#include <ucontext.h>

/** Number of entries in the sample buffer. */
#define OpenSS_PCBufferSize 1024

/** Number of entries in the hash table. */
#define OpenSS_PCHashTableSize (OpenSS_PCBufferSize + (OpenSS_PCBufferSize / 4))

/** struct to record the pc buffer, count buffer, hash table, address range */
 
typedef struct {

    /** Sample buffer. */
    struct {
	/**< Program counter (PC) addresses. */
        uint64_t pc[OpenSS_PCBufferSize];

	/**< Sample count at each address. */
        uint8_t count[OpenSS_PCBufferSize];
    } buffer;

    /** Hash table mapping PC addresses to their sample buffer entries. */
    unsigned hash_table[OpenSS_PCHashTableSize];

    /** pc buffer length*/
    uint8_t pc_len;
    /** count buffer length*/
    uint8_t count_len;

    /** Address range for the PC addresses */
    uint64_t addr_begin;
    uint64_t addr_end;
} OpenSS_PCData;


/** Type representing a function pointer to a timer event handler. */
typedef void (*OpenSS_TimerEventHandler)(const ucontext_t*);


void OpenSS_DecodeParameters(const char*, const xdrproc_t, void*);
uint64_t OpenSS_GetAddressOfFunction(const void*);
uint64_t OpenSS_GetPCFromContext(const ucontext_t*);
bool_t   OpenSS_UpdatePCData(uint64_t pc, OpenSS_PCData *pcdata);

uint64_t OpenSS_GetTime();
void OpenSS_Send(const OpenSS_DataHeader*, const xdrproc_t, const void*);
void OpenSS_Timer(uint64_t, OpenSS_TimerEventHandler);

#ifdef HAVE_LIBUNWIND
void OpenSS_GetStackTraceFromContext(const ucontext_t*,
				     bool_t, unsigned, unsigned,
				     unsigned*, uint64_t*);
#endif

#ifdef HAVE_PAPI
void OpenSS_Create_Eventset(int*);
void OpenSS_AddEvent(int, int);
void OpenSS_Overflow(int, int, int, void*);
void OpenSS_Start(int);
void OpenSS_Stop(int);
#endif



/**
 * Get the current context.
 *
 * Returns the current context of the calling thread.
 *
 * @retval context    Current context.
 *
 * @ingroup RuntimeAPI
 */
#define OpenSS_GetContext(context)    \
    Assert(getcontext(context) == 0);



#endif
