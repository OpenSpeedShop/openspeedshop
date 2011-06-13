/*******************************************************************************
** Copyright (c) 2009 The Krell Institue. All Rights Reserved.
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
 * Declaration of Offline Monitor specifc utility functions
 *
 */

#ifndef _OpenSpeedShop_Monitor_
#define _OpenSpeedShop_Monitor_

#include "offline.h"



#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    OpenSS_Monitor_Proc,
    OpenSS_Monitor_Thread,
    OpenSS_Monitor_Default
} OpenSS_Monitor_Type;

typedef enum {
    OpenSS_Monitor_Started = 1,
    OpenSS_Monitor_Finished,
    OpenSS_Monitor_Paused,
    OpenSS_Monitor_Resumed,
    OpenSS_Monitor_Not_Started
} OpenSS_Monitor_Status;

#ifdef  __cplusplus
}
#endif
#endif /*_OpenSpeedShop_Monitor_*/
