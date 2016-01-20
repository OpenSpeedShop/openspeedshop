/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2016 Krell Institute All Rights Reserved.
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
 * Definition of the OpenSS_GetAddressOfFunction() function.
 *
 */

#include "Assert.h"
#include "RuntimeAPI.h"



/**
 * Get the address of a function.
 *
 * Returns the address of a function given its function pointer. Some platforms
 * (e.g. IA64) don't return the address of the first instruction in a function
 * as its function pointer. They return a pointer to a function "descriptor"
 * instead.
 *
 * @sa    http://www.intel.com/design/itanium/downloads/24537003.pdf
 * @sa    http://www.linuxbase.org/spec/book/ELF-IA64/ELF-IA64.html
 *
 * @param pointer    Function pointer from which to extract the address.
 * @return           Address of this function.
 *
 * @ingroup RuntimeAPI
 */
uint64_t OpenSS_GetAddressOfFunction(const void* pointer)
{
    /* Check assertions */   
    Assert(pointer != NULL);
#if defined(RUNTIME_PLATFORM_BGQ)
    
    /* Return the function pointer from the descriptor on Linux and BG/Q */
    return *((uint64_t*)((uintptr_t)pointer));

#elif defined(__linux) && (defined __powerpc64__ )

    /* Return the function pointer directly on Linux/IA32 and Linux/X86-64 */
    return (uint64_t)((uintptr_t)pointer);

#elif defined(__linux) && (defined __powerpc__ )

    /* Return the function pointer directly on Linux/IA32 and Linux/X86-64 */
    return (uint64_t)((uintptr_t)pointer);

#elif defined(__linux) && (defined(__i386) || defined(__x86_64) )

    /* Return the function pointer directly on Linux/IA32 and Linux/X86-64 */
    return (uint64_t)((uintptr_t)pointer);
    
#elif defined(__linux) && defined(__ia64)
    
    /* Return the function pointer from the descriptor on Linux/IA64 */
    return *((uint64_t*)((uintptr_t)pointer));
    
#elif defined(__linux) && (defined(__aarch64__))

    /* Return the function pointer directly on Linux/arm and Linux/aarch64 */
    return (uint64_t)((uintptr_t)pointer);
    
#elif defined(__linux) && (defined(__arm__))

    /* Return the function pointer directly on Linux/arm and Linux/aarch64 */
    return (uint64_t)((uintptr_t)pointer);
    
#else
#error "Platform/OS Combination Unsupported!"
#endif
}
