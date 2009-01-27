/*******************************************************************************
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
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
 * Definition of the openss_send() function.
 *
 */



/**
 * Send performance data.
 *
 * Sends performance data to the framework instance which loaded this runtime
 * library into the target process. Any header generation and data encoding is
 * performed by OpenSS_Send(). Here the data is treated purely as a buffer of
 * bytes to be sent.
 *
 * @param size    Size of the data to be sent (in bytes).
 * @param data    Pointer to the data to be sent.
 * @return        Integer "1" if succeeded or "0" if failed.
 */
int openss_send(const unsigned size, const void* data)
{
    /* Send the data and inform the caller whether we succeeded or not */
    return OpenSS_SendToFile(size, data);
}
