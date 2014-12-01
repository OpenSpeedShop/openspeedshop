/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2010 The Krell Institute. All Rights Reserved.
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
 * Definition of the OpenSS_GetInstrLength() function.
 *
 */

#include "Assert.h"
#include "RuntimeAPI.h"

/* The disassembly is only needed for x86 and x86_64. We need
   to decode the instruction at the exception address to
   determine it's length.
*/
#if defined(__linux) && (defined(__i386) || defined(__x86_64) || defined(__powerpc__) || defined(__powerpc64__))

#include <bfd.h>
#include <stdint.h>
#include <dis-asm.h>

static bfd  *abfd;
static int init_done = 0;

/* dummy print function for opcodes disassembler.
   We do not need to print a disassembly.
*/
static int dummyprint () {
//    fprintf(stderr,"ENTERED libopenss-runtime:dummyprint\n");
    return 0;
}

/* use the bfd for the process exe to initialize disassembler.
   FIXME: Invetigate removing this dependency.
*/
static void initdisassembler() {
//fprintf(stderr, "ENTERED initdisassembler\n");
  char myexepath[PATH_MAX];
  char *target = NULL;

  if (init_done == 1)  return;

  /* find the victim executable */
  int mypid = getpid();
  sprintf(myexepath, "/proc/%d/exe", mypid);

  bfd_init();

  abfd = bfd_openr(myexepath,target);

  if (!abfd) {
    /*error no bfd to disassemble */
    fprintf(stderr, "Could not load bfd for %s\n",myexepath);
    return;
  }

  /* make sure it's an object file */
  if (!bfd_check_format (abfd, bfd_object)) {
    fprintf(stderr, "The bfd %s is not an object file\n",myexepath);
    return;
  }

  init_done = 1;
}
#endif

/**
 * Get length of instruction at the current PC.
 *
 * @param value      the PC value.
 *
 * @return  length of instruction at PC address.
 *
 * @ingroup RuntimeAPI
 */
int OpenSS_GetInstrLength(uint64_t pcvalue)
{
//fprintf(stderr, "ENTERED OpenSS_GetInstrLength\n");
#if defined(__linux) && (defined(__i386) || defined(__x86_64))
    
    disassemble_info   disinfo;
    disassembler_ftype disassemble_fn;

    initdisassembler();

//fprintf(stderr, "OpenSS_GetInstrLength CALLS INIT_DISASSEMBLE_INFO\n");
    INIT_DISASSEMBLE_INFO(disinfo, stdout, fprintf);

    disinfo.flavour = bfd_get_flavour (abfd);
    disinfo.arch = bfd_get_arch (abfd);
    //disinfo.mach = bfd_get_mach (abfd);
    disinfo.mach = bfd_mach_i386_i386;
    disinfo.octets_per_byte = bfd_octets_per_byte (abfd);
    if (bfd_big_endian (abfd))
      disinfo.display_endian = disinfo.endian = BFD_ENDIAN_BIG;
    else if (bfd_little_endian (abfd))
      disinfo.display_endian = disinfo.endian = BFD_ENDIAN_LITTLE;
    else
      disinfo.endian = BFD_ENDIAN_UNKNOWN;
    disinfo.buffer_length = 8;

    unsigned char* ptr = (unsigned char*) pcvalue;

    disinfo.buffer_vma = pcvalue;
    disinfo.buffer = ptr;
    disinfo.fprintf_func=(fprintf_ftype) dummyprint;

//fprintf(stderr, "OpenSS_GetInstrLength CALLS print_insn_i386 with addr %#lx\n",pcvalue);
    int insbytes = print_insn_i386(pcvalue, &disinfo);

//fprintf(stderr,"libopenss-runtime:OpenSS_GetInstrLength returns length %d\n",
//	insbytes);
    return insbytes;

#elif defined(__linux) && defined(__powerpc__)

    /*
       FROM: http://www.ibm.com/developerworks/library/l-ppc/
       All PowerPCs (including 64-bit implementations) use fixed-length 32-bit instructions
    */
    return 0x4;


#elif defined(__linux) && defined(__powerpc64__)
    /*
       FROM: http://www.ibm.com/developerworks/library/l-ppc/
       All PowerPCs (including 64-bit implementations) use fixed-length 32-bit instructions
    */
    return 0x4;


#elif defined(__linux) && defined(__ia64)

    /* IA64 bundles are 128 bits. The caller must find the address
       of the bundle that contains the exception address and
       add this return value to it prior to updating the context PC.
    */
    return 0x10;

#elif defined(__linux) && defined(__aarch64__)
    /*
       FROM: Arm architecture wiki page
       Armv8 (and also including 64-bit implementations) use fixed-length 32-bit instructions
    */
    return 0x4;

#elif defined(__linux) && defined(__arm__)
    /*
       FROM: Arm architecture wiki page
       Armv7 (and also including 64-bit implementations) use fixed-length 32-bit instructions
    */
    return 0x4;

#else
#error "Platform/OS Combination Unsupported!"
#endif
}
