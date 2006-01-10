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
/** IO Wrapper Function for @FUNCTION@ */

#include "RuntimeAPI.h"
#include "blobs.h"
#include <syscall.h>
#include "runtime.h"

/*
 * Special-Purpose IO Wrapper Functions
 *
 * Most of the IO wrapper functions are generated automagically via the custom
 * `mkwrapper' tool and its associated template file `mkwrapper.template'. The
 * following functions cannot be generated via that mechanism because they have
 * specialized implementations that don't fit the template.
 *
 */

/*
pread
read
readv
pwrite
write
writev
create
open
close
dup
pipe
lseek
see /usr/include/bits/syscall.h
libssrt-io/ssio.c:      syscall_no = SYS_creat;
libssrt-io/ssio.c:      syscall_no = SYS_open;
libssrt-io/ssio.c:      syscall_no = SYS_read;
libssrt-io/ssio.c:      syscall_no = SYS_write;
libssrt-io/ssio.c:      syscall_no = SYS_close;
libssrt-io/ssio.c:      syscall_no = SYS_pipe;
libssrt-io/ssio.c:      syscall_no = SYS_dup;
libssrt-io/ssio.c:      syscall_no = SYS_lseek;
libssrt-io/ssio.c:      syscall_no = SYS_pread;
libssrt-io/ssio.c:      syscall_no = SYS_pwrite;
libssrt-io/ssio.c:      syscall_no = SYS_readv;
libssrt-io/ssio.c:      syscall_no = SYS_writev;
*/

extern ssize_t __read(int fd, void *buf, size_t count);
int io__libc_read(int fd, void *buf, size_t count) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_read;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) buf;
    event.sysargs[2] = (uint64_t) count;
#endif

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = __read(fd, buf, count);

    event.nbytes = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
    io_record_event(&event, OpenSS_GetAddressOfFunction(__read));
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
