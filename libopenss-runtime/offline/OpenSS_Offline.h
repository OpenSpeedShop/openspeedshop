/*******************************************************************************
** Copyright (c) 2007,2008,2009 The Krell Institue. All Rights Reserved.
** Copyright (c) 2008 William Hachfeld. All Rights Reserved.
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
 * Declaration of Offline utility functions
 *
 */

#ifndef _OpenSpeedShop_Runtime_Offline_
#define _OpenSpeedShop_Runtime_Offline_

#include "offline.h"


typedef struct oss_dlinfo_t oss_dlinfo;
typedef struct oss_dlinfoList_t oss_dlinfoList;

struct oss_dlinfo_t {
    uint64_t load_time;
    uint64_t unload_time;
    uint64_t addr_begin;
    uint64_t addr_end;
    char *name;
    void *handle;
};

struct oss_dlinfoList_t {
    oss_dlinfo oss_dlinfo_entry;
    oss_dlinfoList *oss_dlinfo_next;
};


#ifdef __cplusplus
extern "C"
{
#endif

/* Size of buffer (dso blob) to hold the dsos loaded into victim addressspace.
 * Computed in collector offline code as:
 * number of dso objects * sizeof dso object + the string lengths of the dso paths.
 * The collectors will send the current dsos buffer (blob) when ever
 * OpenSS_OBJBufferSize is exceeded and start a new buffer.
 */
#define OpenSS_OBJBufferSize (8*1024)

int OpenSS_GetDLInfo(pid_t pid, char *path, uint64_t b_time, uint64_t e_time);
void OpenSS_InitializeParameters (openss_expinfo *info);

#ifdef  __cplusplus
}
#endif
#endif /*_OpenSpeedShop_Runtime_Offline_*/
