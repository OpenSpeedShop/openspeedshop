/*******************************************************************************
 * ** Copyright (c) 2012 The Krell Institue. All Rights Reserved.
 * **
 * ** This library is free software; you can redistribute it and/or modify it under
 * ** the terms of the GNU Lesser General Public License as published by the Free
 * ** Software Foundation; either version 2.1 of the License, or (at your option)
 * ** any later version.
 * **
 * ** This library is distributed in the hope that it will be useful, but WITHOUT
 * ** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * ** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * ** details.
 * **
 * ** You should have received a copy of the GNU Lesser General Public License
 * ** along with this library; if not, write to the Free Software Foundation, Inc.,
 * ** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * *******************************************************************************/

//#define _GNU_SOURCE
//#define __USE_GNU
#include <elf.h>
#include <link.h>

/* Never seen more than 3 load segments in practice. 16 should be fine. */
#define MAX_LOAD_SEGMENTS 16
typedef struct {
   ElfW(Addr) mem_addr;
   size_t mem_size;
   ElfW(Off) file_offset;
   size_t file_size;
   unsigned int premissions; /* in rwx format */
} mem_region;

extern const char *monlibs_vdso_name; /* = "[vdso]" */
extern const char *monlibs_exe_name;  /* = "[exe]" */

/**
 * Call monlibs_getLibraries and it will report all library changes since the last call 
 * via its library_cb callback.
 * 
 * Each library loaded/unloaded will be reported with a call to library_cb
 *  - base_address is the library's base address.  It can be added to symbol
 *    offsets to get symbol addresses.
 *  - name is the filename of the library.  The memory pointed to by name will
 *    remain valid until immediately after the callback that reports the library
 *    unload returns.
 *    Note that instead of reporting names for the executable and linux VDSO,
 *    monlibs respectively returns the monlibs_vsdo_name and monlibs_exe_name
 *    pointers.
 *  - regions is an array of mem_region structs.  Each mem_region describes 
 *    a block of memory the library occupies, what parts of the file on disk
 *    that memory was mapped from, and the premissions of the pages.  The memory
 *    of regions array is invalidated after the callback returns.
 *  - num_regions is the size of the regions array.
 *  - is_load is '1' if this callback is reporting a library load, and '0' if it's
 *    reporting a library unload.
 **/
typedef void(*library_cb)(ElfW(Addr) base_address,
	     const char *name, mem_region *regions, unsigned int num_regions,
	     int is_load, uint64_t b_time, uint64_t e_time);
void monlibs_getLibraries(library_cb cb, uint64_t b_time, uint64_t e_time);
