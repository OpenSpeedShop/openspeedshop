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

#include "RuntimeAPI.h"
#include "blobs.h"
#include <syscall.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "runtime.h"
#include <dlfcn.h>

/*
 * IO Wrapper Functions
 *
 * The following functions cannot be generated via a mkwrapper mechanism
 * because they have specialized implementations that don't fit a
 * common wrapper template.
 *
 */

/*
The following IO SYS calls are traced by the IOCollector.
These calls are traced using their weak names
(e.g. "open" rather than "__libc_open").
To trace via the weak names requires a version dpcl that
creates FunctionObj's with the alt_name field set to
the alternate name returned by dyninst's findFunction.
Currently, dpcl-20051215-8 has the needed get_alt_name
call which the IOCollector depends on.

see /usr/include/bits/syscall.h for details.
SYS_pread;
SYS_read;
SYS_readv;
SYS_pwrite;
SYS_write;
SYS_writev;
SYS_lseek;
SYS_creat;
SYS_open;
SYS_close;
SYS_dup;
SYS_dup2;
SYS_pipe;
SYS_open64;
SYS_creat64;
SYS_pread64;
SYS_pwrite64;
SYS_lseek64;
*/


ssize_t ioread(int fd, void *buf, size_t count) 
{
    ssize_t retval;
    io_event event;

    io_start_event(&event);


    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = read(fd, buf, count);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_read;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) buf;
    event.sysargs[2] = (uint64_t) count;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "read");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(read));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t iowrite(int fd, void *buf, size_t count) 
{    
    ssize_t retval;
    io_event event;

    io_start_event(&event);


    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = write(fd, buf, count);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_write;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) buf;
    event.sysargs[2] = (uint64_t) count;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "write");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(write));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

off_t iolseek(int fd, off_t offset, int whence) 
{    
    off_t retval;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = lseek(fd, offset, whence);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_lseek;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) offset;
    event.sysargs[2] = (uint64_t) whence;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "lseek");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(lseek));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

off_t iolseek64(int fd, off_t offset, int whence) 
{    
    off_t retval;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = lseek64(fd, offset, whence);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_lseek64;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) offset;
    event.sysargs[2] = (uint64_t) whence;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "lseek64");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(lseek64));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int ioopen(const char *pathname, int flags, mode_t mode) 
{    
    int retval = 0;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = open(pathname, flags, mode);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_open;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) pathname;
    event.sysargs[1] = (uint64_t) flags;
    event.sysargs[2] = (uint64_t) mode;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "open");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(open));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int ioopen64(const char *pathname, int flags, mode_t mode) 
{    
    int retval = 0;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = open64(pathname, flags, mode);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_open64;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) pathname;
    event.sysargs[1] = (uint64_t) flags;
    event.sysargs[2] = (uint64_t) mode;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "open64");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(open64));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int ioclose(int fd) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = close(fd);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_close;
    event.nsysargs = 1;
    event.sysargs[0] = (uint64_t) fd;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "close");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(close));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int iodup(int oldfd) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = dup(oldfd);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_dup;
    event.nsysargs = 2;
    event.sysargs[0] = (uint64_t) oldfd;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "dup");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    /* defined(__i386) */
    io_record_event(&event, OpenSS_GetAddressOfFunction(dup));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int iodup2(int oldfd, int newfd) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = dup2(oldfd,newfd);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_dup2;
    event.nsysargs = 2;
    event.sysargs[0] = (uint64_t) oldfd;
    event.sysargs[1] = (uint64_t) newfd;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "dup2");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(dup2));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int iocreat(char *pathname, mode_t mode) 
{
    int retval;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = creat(pathname,mode);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_creat;
    event.nsysargs = 2;
    event.sysargs[0] = (uint64_t) pathname;
    event.sysargs[1] = (uint64_t) mode;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "creat");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(creat));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int iocreat64(char *pathname, mode_t mode) 
{
    int retval;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = creat64(pathname,mode);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_creat64;
    event.nsysargs = 2;
    event.sysargs[0] = (uint64_t) pathname;
    event.sysargs[1] = (uint64_t) mode;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "creat64");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(creat64));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int iopipe(int filedes[2]) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = pipe(filedes);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_pipe;
    event.nsysargs = 1;
    event.sysargs[0] = (uint64_t) filedes;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "pipe");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(pipe));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t iopread(int fd, void *buf, size_t count, off_t offset) 
{    
    ssize_t retval;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = pread(fd, buf, count, offset);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_pread;
    event.nsysargs = 4;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) buf;
    event.sysargs[2] = (uint64_t) count;
    event.sysargs[3] = (uint64_t) offset;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "pread");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(pread));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t iopread64(int fd, void *buf, size_t count, off_t offset) 
{    
    ssize_t retval;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = pread64(fd, buf, count, offset);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_pread64;
    event.nsysargs = 4;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) buf;
    event.sysargs[2] = (uint64_t) count;
    event.sysargs[3] = (uint64_t) offset;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "pread64");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(pread64));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t iopwrite(int fd, void *buf, size_t count, off_t offset) 
{    
    ssize_t retval;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = pwrite(fd, buf, count, offset);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_pwrite;
    event.nsysargs = 4;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) buf;
    event.sysargs[2] = (uint64_t) count;
    event.sysargs[3] = (uint64_t) offset;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "pwrite");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(pwrite));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t iopwrite64(int fd, void *buf, size_t count, off_t offset) 
{    
    ssize_t retval;
    io_event event;

    io_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = pwrite64(fd, buf, count, offset);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_pwrite64;
    event.nsysargs = 4;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) buf;
    event.sysargs[2] = (uint64_t) count;
    event.sysargs[3] = (uint64_t) offset;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "pwrite64");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(pwrite64));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t ioreadv(int fd, const struct iovec *vector, size_t count) 
{    
    ssize_t retval;
    io_event event;

    io_start_event(&event);


    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = readv(fd, vector, count);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_readv;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) vector;
    event.sysargs[2] = (uint64_t) count;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "readv");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(readv));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}


ssize_t iowritev(int fd, const struct iovec *vector, size_t count) 
{    
    ssize_t retval;
    io_event event;

    io_start_event(&event);


    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = writev(fd, vector, count);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_writev;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) vector;
    event.sysargs[2] = (uint64_t) count;
    event.retval = retval;
#endif

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "writev");
    io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    io_record_event(&event, OpenSS_GetAddressOfFunction(writev));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
