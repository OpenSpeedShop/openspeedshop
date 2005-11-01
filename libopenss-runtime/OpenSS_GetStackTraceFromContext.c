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

/*
   Obtain maxframes callstack addresses for the current context.
   There may be 3 frames of overhead we need to remove
   (4 frames for papi).
   The address buffer returned is by framebuf is the current callstack
   without any overhead frames introduced by openss.
*/
void OpenSS_GetStackTraceFromContext (const ucontext_t* context,
				      int overhead,
				      int maxframes,
				      int *framecount,
				      uint64_t *framebuf)
{
    /* Obtain the program counter (PC) address from the thread context */
    /* We will test passedpc against the first stack frame address */
    /* to see if we have to skip any signal handler overhead. */
    /* Suse and SLES may not have the signal handler overhead. */
    uint64_t passedpc;
    passedpc = OpenSS_GetPCFromContext(context);
    unw_word_t ip;      /* current stack trace pc address */

    /* Obtain the program counter current address from the thread context */
    /* and unwind the stack from there. */
    /* Libunwind provides it's own get context routines and the libunwind */
    /* documentation suggests that we use that. */


    unw_cursor_t cursor;          /* libunwind stack cursor (pointer) */
    unw_context_t uc;             /* libunwind context */

#if UNW_TARGET_IA64
    unw_getcontext (&uc);         /* get the libunwind context */
#else
    /* copy passed context to a libunwind context */
    memcpy(&uc, context, sizeof(ucontext_t));
#endif

    /* FIXME: handle errors from libunwind. */
    if (unw_init_local (&cursor, &uc) < 0) {
        /* handle error if we get a negative stack pointer */
        /*panic ("unw_init_local failed!\n"); */
    }

    /*
     * Loop through stack address and add to the sample buffer
     * (i.e. the current fame pc value at the stack trace cursor).
     * We may incur 3 or 4 frames of overhead for the signal handler.
     * The caller provides the number of possible frames to skip.
     * If overhead is 0, all frames including signal handler frames
     * are included in the stack trace.
     *
    */

    int overhead_marker = 0;  /* marker to count signal handler overhead*/
    int i = 0;     /* framebuf index */
    int rval = 0;  /* return value from libunwind calls */

    *framecount = 0;

    do
    {
        /* get current stack address pointed to by cursor */
        unw_get_reg (&cursor, UNW_REG_IP, &ip);

        /* are we already past the signal handler frames overhead? */
	/* there are 3 frames of overhead due to openspeedshop.
	   there is one additional frame of overhead due to papi
	   for collectors that use papi. Caller passes the overhead value.
	*/
        if (overhead_marker == 0 && passedpc == ip) {
            overhead_marker = overhead; /* we started past the overhead */
        }

        /* add frame address if we are past any signal handler overhead */
        if (overhead_marker > (overhead - 1) ) {

            /* add frame address to framebuf */
            framebuf[i] = (uint64_t) ip;
            i++;

        } else {

            /* increment past signal handler overhead */
            overhead_marker++;
        }

        /* step (unwind) the cursor to the preceding frame */
        rval = unw_step (&cursor);
        if (rval < 0) {
            unw_get_reg (&cursor, UNW_REG_IP, &ip);
            printf ("FAILURE: unw_step() returned %d for ip=%lx\n",
                  rval, (uint64_t) ip);
        }

        /* Is the sample framebuf full? */
        if(i == (maxframes - 1) ) {
	    /* we have maxframes, need to return this stack trace.
	    */
	    break;
        }
    }
    while (rval > 0);
    *framecount = i;
}
