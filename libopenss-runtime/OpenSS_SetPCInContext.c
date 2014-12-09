/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the OpenSS_GetPCFromContext() function.
 *
 */

#include "Assert.h"
#include "RuntimeAPI.h"



/**
 * Set program counter (PC) in a thread context.
 *
 * Set the program counter value in the specified thread context as a single
 * 64-bit unsigned integer. Thread contexts are typically obtained from a
 * signal handler or from calling OpenSS_GetContext().
 *
 * @param value    the PC value.
 * @param context    Thread context in which to set the PC value.
 *
 * @ingroup RuntimeAPI
 */
void OpenSS_SetPCInContext(uint64_t value, ucontext_t* context)
{
    /* Check preconditions */
    Assert(context != NULL);

#if defined(__linux) && defined(__powerpc__)
    
    /* Return PC value from Linux/PPC32 thread context 
    FIXME: context->uc_mcontext.gregs[REG_EIP] = value;
*/

#elif defined(__linux) && defined(__powerpc64__)
    
    /* Return PC value from Linux/PPC64 thread context 
    FIXME: context->uc_mcontext.gregs[REG_EIP] = value;
*/

#elif defined(__linux) && defined(__i386)
    
    /* Return PC value from Linux/IA32 thread context */
    context->uc_mcontext.gregs[REG_EIP] = value;
    
#elif defined(__linux) && defined(__x86_64)

    /* Return PC value from Linux/X86-64 thread context */
#if __WORDSIZE == 64
    context->uc_mcontext.gregs[REG_RIP] = value;
#else  /* __WORDSIZE == 32 */
    context->uc_mcontext.gregs[REG_EIP] = value;
#endif

#elif defined(__linux) && defined(__ia64)
    
    /* Return PC value from Linux/IA64 thread context */
    context->uc_mcontext.sc_ip = value;
    
#elif defined(__linux) && defined(__aarch64__)

    context->uc_mcontext.pc = value;

#elif defined(__linux) && defined(__arm__)

    context->uc_mcontext.arm_pc = value;

#else
#error "Platform/OS Combination Unsupported!"
#endif
}
