/*******************************************************************************
** Copyright (c) 2007 The Krell Institue. All Rights Reserved.
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

#ifdef __cplusplus
extern "C"
{
#endif

int OpenSS_GetDLInfo(pid_t pid, char *path);
void OpenSS_InitializeParameters (openss_expinfo *info);
extern int OpenSS_mpi_rank;

#ifdef  __cplusplus
}
#endif
#endif /*_OpenSpeedShop_Runtime_Offline_*/
