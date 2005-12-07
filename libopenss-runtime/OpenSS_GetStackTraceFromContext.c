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
 * Definition of the OpenSS_GetStackTraceFromContext() function.
 *
 */

#include "Assert.h"
#include "RuntimeAPI.h"

#include <libunwind.h>



/**
 * Get stack trace from a thread context.
 *
 * Returns the stack trace from a thread context. The current thread context is
 * obtained directly in almost all cases, with the one exception being described
 * in the note below. All frames up to the first signal frame can be skipped, as
 * can a fixed number of additional frames.
 *
 * @note    In theory libunwind could and should <em>always</em> be used to
 *          directly obtain the context for the stack trace - even in the case
 *          of signal contexts. However libunwind is unable to unwind past the
 *          signal frame on many IA32 and AMD64/EM64T systems. Our only recourse
 *          on these systems is to use the signal context directly for doing the
 *          unwind. Hopefully in the future this restriction can be removed and
 *          the signal_context argument to this function can be dropped.
 *
 * @param signal_context        Thread signal context from which to extract the
 *                              stack trace. Use null when obtaining stack trace
 *                              from a non-signal context.
 * @param skip_signal_frames    Flag indiciating if frames up to and including
 *                              the signal frame should be skipped.
 * @param skip_frames           Fixed number of additional frames to skip.
 * @param max_frames            Maximum number of frames to be stored.
 * @retval stacktrace_size      Actual size (in number of frames) of the stack
 *                              trace obtained from the current context.
 * @retval stacktrace           Stack trace obtained from the current context.
 */
void OpenSS_GetStackTraceFromContext(const ucontext_t* signal_context,
				     bool_t skip_signal_frames,
				     unsigned skip_frames,
				     unsigned max_frames,
				     unsigned* stacktrace_size,
				     uint64_t* stacktrace)
{
    unw_context_t context;
    unw_cursor_t cursor;
    int retval;
    unw_word_t pc;
    unsigned index = 0;

#if defined(__linux) && (defined(__i386) || defined(__x86_64))

    /*
     * Use the signal context if one was provided, otherwise get the current
     * thread context directly.
     */
    if(signal_context != NULL) {
	memcpy(&context, signal_context, sizeof(unw_context_t));
	skip_signal_frames = FALSE;
    }
    else
	Assert(unw_getcontext(&context) == 0);

#elif defined(__linux) && defined(__ia64)

    /* Get the current thread context */
    Assert(unw_getcontext(&context) == 0);
    
#else
#error "Platform/OS Combination Unsupported!"
#endif

    /* Initialize the unwind cursor from the context */
    Assert(unw_init_local(&cursor, &context) == 0);
	
    /* Iterate over each frame in the stack trace from this context */
    while(TRUE) {

	/* Are we still unwinding past skipped signal frames? */
	if(skip_signal_frames) {
	    
	    /* Stop skipping signal frames after we've encountered one */
	    retval = unw_is_signal_frame(&cursor);
	    Assert(retval >= 0);
	    if(retval > 0)		
		skip_signal_frames = FALSE;
	    
	}
	
	/* Are we still unwinding past skipped additional frames? */
	else if(skip_frames > 0) {
	    
	    /* Decrement the number of fixed additional frames to be skipped */
	    --skip_frames;
	    
	}
	
	/* Stop unwinding if the stack trace buffer is full */
	else if(index == max_frames)
	    break;
	
	/* Otherwise store the PC value from this frame in the stack trace */
	else {
	    Assert(unw_get_reg(&cursor, UNW_REG_IP, &pc) == 0);
	    stacktrace[index++] = (uint64_t)pc;	    
	}
	
	/* Unwind to the next frame, stopping after the last frame */
	retval = unw_step(&cursor);
	if(retval <= 0)
	    break;
	
    }
    
    /* Return the stack trace size to the caller */
    *stacktrace_size = index;
}
