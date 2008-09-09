/*******************************************************************************
** Copyright (c) 2008 The Krell Institue.
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
 * Definition of the OpenSS_InitializeDataHeader() function.
 *
 */

#include "Assert.h"
#include "offline.h"


/**
 * Initialize performance data header.
 *
 * Zeroes a performance data header and fills in the experiment and collector
 * identifiers as well as identifying information for the calling thread.
 *
 * @param experiment    Identifier of experiment to contain the data.
 * @param collector     Identifier of collector gathering data.
 * @param header        Performance data header to be initialized.
 */
void OpenSS_InitializeParameters (openss_expinfo *info)
{
    /* Check assertions */
    Assert(info != NULL);

    /* Zero the header */
    memset(info, 0, sizeof(openss_expinfo));

    /* Fill in the specified experiment and collector identifiers */
#if 1
    info->collector = "";
    info->exename = "";
    info->traced = "";
    info->event = "";
    info->rank = 0;
    info->omptid = 0;
    info->rate = 0;
#endif
}
