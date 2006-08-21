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
 * Stack trace function (uses libunwind).
 *
 */

#include "RuntimeAPI.h"
#include "libunwind.h"

bool_t cmp_samplebuffer(int framecount,
		      int *stackindex,
		      int countbuflen,
		      uint64_t *databuf,
		      uint64_t *countbuf,
		      uint64_t *framebuf)
{
    int i,j;
    bool_t found = FALSE;
    /* search individual stacks via count/indexing array */
    for (i = 0; i < countbuflen ; i++ )
    {
        /* a count > 0 indexes the top of stack in the data buffer. */
        /* a count == 255 indicates this stack is at the count limit. */
        if (countbuf[i] == 0) {
            continue;
        }
        if (countbuf[i] == 255) {
            found = FALSE;
            continue;
        }

        /* see if the stack addresses match */
        for (j = 0; j < framecount ; j++ )
        {
            if ( databuf[i+j] != framebuf[j] ) {
                   found = FALSE;
                   break;
            }
        }

        if ( j == framecount) {
            found = TRUE;
            *stackindex = i;
            return found;
        }
    }
    return found;
}

static void update_samplebuffer(int framecount,
				int *databuflen,
				int *countbuflen,
				uint64_t *hdr_addr_begin,
				uint64_t *hdr_addr_end,
				uint64_t *framebuf,
				uint64_t *countbuf,
				uint64_t *databuf)
{
        /* add frames to sample buffer, compute addresss range */
        int i;
        for (i = 0; i < framecount ; i++)
        {
            /* always add address to buffer bt */
            databuf[*databuflen] = framebuf[i];

            /* top of stack indicated by a positive count. */
            /* all other elements are 0 */
            if (i > 0 ) {
                countbuf[*countbuflen] = 0;
            } else {
                countbuf[*countbuflen] = 1;
            }

            if (framebuf[i] < *hdr_addr_begin ) {
                *hdr_addr_begin = framebuf[i];
            }
            if (framebuf[i] > *hdr_addr_end ) {
                *hdr_addr_end = framebuf[i];
            }
            *databuflen++;
            *countbuflen++;
        }
}

void OpenSS_UpdateStackTraceBuffer(int framecount,
				   int bufsize,
				   int *stackindex,
				   int *databuflen,
				   int *countbuflen,
				   uint64_t *hdr_addr_begin,
				   uint64_t *hdr_addr_end,
				   uint64_t *framebuf,
				   uint64_t *countbuf,
				   uint64_t *databuf)
{
    bool_t stack_already_exists =
		cmp_samplebuffer(framecount,stackindex,*countbuflen,databuf,countbuf,framebuf);

    /* if the stack already exisits in the buffer, update its count
     * and return. If the stack is already at the count limit, cmp_samplebuffer
     * will return false so a new stack entry will be created even though
     * the stack may already exist in the buffer.
     * Otherwise, add the new stack to the buffer
    */
    if (stack_already_exists && countbuf[*stackindex] < 255 ) {
        /* update count for this stack */
        countbuf[*stackindex] = countbuf[*stackindex] + 1;
        return;
    }

    /* Will this stack trace fit into the sample buffer? */
    /* update_samplebuffer adds the 0 stack termination marker and */
    /* updates tls.data.bt.bt_len. */

    int buflen = *databuflen + framecount;
    if ( buflen <= bufsize) {
        update_samplebuffer(framecount,
			    databuflen,
                            countbuflen,
                            hdr_addr_begin,
                            hdr_addr_end,
                            framebuf,
                            countbuf,
                            databuf);
    } else {
        /* sample buffer has no room for these stack frames.*/
        /* send the current sample buffer. (will init a new buffer) */
        send_samples();

        update_samplebuffer(framecount,
			    databuflen,
                            countbuflen,
                            hdr_addr_begin,
                            hdr_addr_end,
                            framebuf,
                            countbuf,
                            databuf);
    }
}
