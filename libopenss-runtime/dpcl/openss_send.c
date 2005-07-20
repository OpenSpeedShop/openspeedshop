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
 * Definition of the openss_send() function and its supporting code.
 *
 */

#include "Assert.h"

#include <dpclExt.h>
#include <stdio.h>



/**
 * Message handle.
 *
 * Message handle used to send data back to the framework instance which loaded
 * this runtime library into the target process. Messages cannot be sent until
 * this is set to something other than null.
 */
static AisPointer MsgHandle = NULL;



/**
 * Set the message handle.
 *
 * Simply makes the specified message handle the one to be used for sending data
 * back to the proper framework instance.
 *
 * @todo    Do we need locking on MsgHandle here?
 *
 * @param msg_handle    Message handle to be used.
 */
void openss_set_msg_handle(AisPointer msg_handle)
{
    MsgHandle = msg_handle;
}



/**
 * Send performance data.
 *
 * Sends performance data to the framework instance which loaded this runtime
 * library into the target process. Any header generation and data encoding is
 * performed by OpenSS_Send(). Here the data is treated purely as a buffer of
 * bytest to be sent.
 *
 * @param size    Size of the data to be sent (in bytes).
 * @param data    Pointer to the data to be sent.
 * @return        Integer "1" if succeeded or "0" if failed.
 */
int openss_send(const unsigned size, const void* data)
{
    /* Check assertions */
    Assert(MsgHandle != NULL);
    
    /* Send the data and inform the caller whether we succeeded or not */
    return (Ais_send(MsgHandle, (void*)data, size) == 0) ? 1 : 0;
}
