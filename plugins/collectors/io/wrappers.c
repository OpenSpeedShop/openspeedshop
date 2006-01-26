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

/*
 * IO Wrapper Functions
 *
 * The following functions cannot be generated via a mkwrapper mechanism
 * because they have specialized implementations that don't fit a
 * common wrapper template.
 *
 */

/*
 * FIXME: Currently we can only wrap the global test symbol for
 * a syscall in glibc and not the weak symbol.
 * e.g. We wrap __libc_open rather than open.
 *
 */

/*

see /usr/include/bits/syscall.h
SYS_pread;
ssize_t pread(int fd, void *buf, size_t count, off_t  offset);
SYS_read;
ssize_t read(int fd, void *buf, size_t count);
SYS_readv;
ssize_t readv(int fd, const struct iovec *vector, int count);
SYS_pwrite;
ssize_t pwrite(int  fd,  const  void  *buf, size_t count, off_t offset);
SYS_write;
ssize_t write(int fd, const void *buf, size_t count);
SYS_writev;
ssize_t writev(int fd, const struct iovec *vector, int count);
SYS_lseek;
off_t lseek(int fildes, off_t offset, int whence);
SYS_creat;
int creat(const char *pathname, mode_t mode);
SYS_open;
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
SYS_close;
int close(int fd);
SYS_dup;
int dup(int oldfd);
int dup2(int oldfd, int newfd);
SYS_pipe;
int pipe(int filedes[2]);

*/


ssize_t io__libc_read(int fd, void *buf, size_t count) 
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
    retval = __libc_read(fd, buf, count);

    event.nbytes = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
    io_record_event(&event, OpenSS_GetAddressOfFunction(__libc_read));
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t io__libc_write(int fd, void *buf, size_t count) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_write;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) buf;
    event.sysargs[2] = (uint64_t) count;
#endif

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = __libc_write(fd, buf, count);

    event.nbytes = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
    io_record_event(&event, OpenSS_GetAddressOfFunction(__libc_write));
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

off_t io__libc_lseek(int fd, off_t offset, int whence) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_lseek;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) offset;
    event.sysargs[2] = (uint64_t) whence;
#endif

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = __libc_lseek(fd, offset, whence);

    event.nbytes = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
    io_record_event(&event, OpenSS_GetAddressOfFunction(__libc_lseek));
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int io__libc_open(const char *pathname, int flags, mode_t mode) 
{    
    int retval = 0;
    io_event event;

    io_start_event(&event);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_open;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) pathname;
    event.sysargs[1] = (uint64_t) flags;
    event.sysargs[2] = (uint64_t) mode;
#endif

    event.start_time = OpenSS_GetTime();

    extern int errno;
    int myerrno;

    /* Call the real IO function */
    retval = __libc_open(pathname, flags, mode);

    myerrno = errno;
    errno = 0;
    event.nbytes = retval;


    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
    io_record_event(&event, OpenSS_GetAddressOfFunction(__libc_open));
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int io__libc_close(int fd) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_close;
    event.nsysargs = 1;
    event.sysargs[0] = (uint64_t) fd;
#endif

    event.start_time = OpenSS_GetTime();

    extern int errno;
    errno = 0;

    /* Call the real IO function */
    retval = __libc_close(fd);

    event.nbytes = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
    io_record_event(&event, OpenSS_GetAddressOfFunction(__libc_close));
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int io__dup2(int oldfd, int newfd) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_dup2;
    event.nsysargs = 2;
    event.sysargs[0] = (uint64_t) oldfd;
    event.sysargs[1] = (uint64_t) newfd;
#endif

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = __dup2(oldfd,newfd);

    event.nbytes = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
    io_record_event(&event, OpenSS_GetAddressOfFunction(__dup2));
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if 0
int iocreat(char *pathname, mode_t mode) 
{
    int retval;
    io_event event;

    io_start_event(&event);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_creat;
    event.nsysargs = 1;
    event.sysargs[0] = (uint64_t) pathname;
    event.sysargs[1] = (uint64_t) mode;
#endif

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = creat(pathname,mode);

    event.nbytes = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
    io_record_event(&event, OpenSS_GetAddressOfFunction(creat));
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int io_pipe(int filedes[2]) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_pipe;
    event.nsysargs = 1;
    event.sysargs[0] = (uint64_t) filedes;
#endif

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = pipe(filedes);

    event.nbytes = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
    io_record_event(&event, OpenSS_GetAddressOfFunction(pipe));
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t iopread64(int fd, void *buf, size_t count, off_t offset) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_pread;
    event.nsysargs = 4;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) buf;
    event.sysargs[2] = (uint64_t) count;
    event.sysargs[3] = (uint64_t) offset;
#endif

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = pread64(fd, buf, count, offset);

    event.nbytes = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
    io_record_event(&event, OpenSS_GetAddressOfFunction(pread64));
    
    /* Return the real IO function's return value to the caller */
    return retval;
}


ssize_t io_readv(int fd, const struct iovec *vector, size_t count) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_readv;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) vector;
    event.sysargs[2] = (uint64_t) count;
#endif

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = readv(fd, vector, count);

    event.nbytes = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
    io_record_event(&event, OpenSS_GetAddressOfFunction(readv));
    
    /* Return the real IO function's return value to the caller */
    return retval;
}


ssize_t io_writev(int fd, const struct iovec *vector, size_t count) 
{    
    int retval;
    io_event event;

    io_start_event(&event);

#ifdef EXTENDEDIOTRACE
    event.syscallno = SYS_writev;
    event.nsysargs = 3;
    event.sysargs[0] = (uint64_t) fd;
    event.sysargs[1] = (uint64_t) vector;
    event.sysargs[2] = (uint64_t) count;
#endif

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = writev(fd, vector, count);

    event.nbytes = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
    io_record_event(&event, OpenSS_GetAddressOfFunction(writev));
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
#endif
