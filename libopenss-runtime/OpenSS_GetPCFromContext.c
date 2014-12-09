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
 * Definition of the OpenSS_GetPCFromContext() function.
 *
 */

#define __GNU_SOURCE
#include "Assert.h"
#include "RuntimeAPI.h"



/**
 * Get program counter (PC) from a thread context.
 *
 * Returns the program counter value from the specified thread context as a
 * single 64-bit unsigned integer. Thread contexts are typically obtained from
 * a signal handler or from calling OpenSS_GetContext().
 *
 * @param context    Thread context from which to extract the PC value.
 * @return           PC value from this context.
 *
 * @ingroup RuntimeAPI
 */
uint64_t OpenSS_GetPCFromContext(const ucontext_t* context)
{
    /* Check preconditions */
    Assert(context != NULL);

#if defined(__linux) && defined(__i386)
    
    /* Return PC value from Linux/IA32 thread context */
    return (uint64_t)(context->uc_mcontext.gregs[REG_EIP]);

#elif defined(__linux) &&  defined(__powerpc__) 

  return (uint64_t *) (((struct pt_regs *) ((&(context->uc_mcontext))->regs))->nip);

#elif defined(__linux) &&  defined(__powerpc64__)

  return (uint64_t *) (((struct pt_regs *) ((&(context->uc_mcontext))->regs))->nip);

#elif defined(__linux) && defined(__x86_64) 

    /* Return PC value from Linux/X86-64 thread context */
#if __WORDSIZE == 64
    return (uint64_t)(context->uc_mcontext.gregs[REG_RIP]);
#else  /* __WORDSIZE == 32 */
    return (uint64_t)(context->uc_mcontext.gregs[REG_EIP]);
#endif

#elif defined(__linux) && defined(__ia64)
    
    /* Return PC value from Linux/IA64 thread context */
    return (uint64_t)(context->uc_mcontext.sc_ip);
    
#elif defined(__linux) && defined(__aarch64__)
    
    /* Return PC value from Linux/arm thread context */
    return (uint64_t)(context->uc_mcontext.pc);

#elif defined(__linux) && defined(__arm__)
    
    /* Return PC value from Linux/arm thread context */
    return (uint64_t)(context->uc_mcontext.arm_pc);

#else
#error "Platform/OS Combination Unsupported!"
#endif
}
