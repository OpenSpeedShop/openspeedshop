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

#include "OpenSS_DataHeader.h"

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <rpc/rpc.h>
#include <ucontext.h>



/** Type representing a function pointer to a timer event handler. */
typedef void (*OpenSS_TimerEventHandler)(ucontext_t* context);



void OpenSS_DecodeParameters(const char*, const xdrproc_t, void*);
uint64_t OpenSS_GetPCFromContext(ucontext_t*);
uint64_t OpenSS_GetTime();
void OpenSS_Send(const OpenSS_DataHeader*, const xdrproc_t, const void*);
void OpenSS_Timer(uint64_t, OpenSS_TimerEventHandler);

#ifdef HAVE_PAPI
#include "PapiAPI.h"
void OpenSS_Create_Eventset(int*);
void OpenSS_AddEvent(int, int);
void OpenSS_Overflow(int, int, int, void*);
void OpenSS_Start(int);
void OpenSS_Stop(int);
#endif



#endif
