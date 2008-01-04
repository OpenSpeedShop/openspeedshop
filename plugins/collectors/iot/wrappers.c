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
/* Start part 1 of 2 for Hack to get around inconsistent syscall definitions */
/* #include <syscall.h> */
/* End part 1 of 2 for Hack to get around inconsistent syscall definitions */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "runtime.h"
#include <dlfcn.h>

/* Start part 2 of 2 for Hack to get around inconsistent syscall definitions */
#include <sys/syscall.h>
#ifdef __NR_pread64  /* Newer kernels renamed but it's the same.  */
# ifndef __NR_pread
# define __NR_pread __NR_pread64
# endif
#endif

#ifdef __NR_pwrite64  /* Newer kernels renamed but it's the same.  */
# ifndef __NR_pwrite
#  define __NR_pwrite __NR_pwrite64
# endif
#endif
/* End part 2 of 2 for Hack to get around inconsistent syscall definitions */

/*
 * IOT Wrapper Functions
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

/* used by iot_record_event to record pathnames into pathnames buffer */
/* currentpathname must be set prior to calling iot_record_event. */
/* iot_record_event sets pathindex and so we must set event.pathindex */
/* after the call to iot_record_event in each wrapper. */
/* Currently we record a pathname that is passed as an argument. */
/* TODO: On linux, we can examine /proc/"mypid"/fd after an syscall is made */
/* that uses a file descriptor (e.g. read). This is one way to map a */
/* file descriptor to a pathname. */

extern char currentpathname[PATH_MAX];

ssize_t iotread(int fd, void *buf, size_t count) 
{
    ssize_t retval;
    iot_event event;

    iot_start_event(&event);


    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = read(fd, buf, count);

    event.syscallno = SYS_read;
    event.nsysargs = 3;
    event.sysargs[0] = fd;
    event.sysargs[1] = (long) buf;
    event.sysargs[2] = count;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "read");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(read));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t iotwrite(int fd, void *buf, size_t count) 
{    
    ssize_t retval;
    iot_event event;

    iot_start_event(&event);


    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = write(fd, buf, count);

    event.syscallno = SYS_write;
    event.nsysargs = 3;
    event.sysargs[0] = fd;
    event.sysargs[1] = (long) buf;
    event.sysargs[2] = count;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "write");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(write));
#endif
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

off_t iotlseek(int fd, off_t offset, int whence) 
{    
    off_t retval;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = lseek(fd, offset, whence);

    event.syscallno = SYS_lseek;
    event.nsysargs = 3;
    event.sysargs[0] = fd;
    event.sysargs[1] = offset;
    event.sysargs[2] = whence;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "lseek");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(lseek));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

off_t iotlseek64(int fd, off_t offset, int whence) 
{    
    off_t retval;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = lseek64(fd, offset, whence);

    event.syscallno = SYS_lseek;
    event.nsysargs = 3;
    event.sysargs[0] = fd;
    event.sysargs[1] = offset;
    event.sysargs[2] = whence;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "lseek64");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(lseek64));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int iotopen(const char *pathname, int flags, mode_t mode) 
{    
    int retval = 0;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = open(pathname, flags, mode);

    event.retval = retval;
    event.syscallno = SYS_open;
    event.nsysargs = 3;
    event.sysargs[0] = (long) pathname;
    event.sysargs[1] = flags;
    event.sysargs[2] = mode;

    event.stop_time = OpenSS_GetTime();

    strncpy(currentpathname,pathname,strlen(pathname));

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "open");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(open));
#endif
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

int iotopen64(const char *pathname, int flags, mode_t mode) 
{    
    int retval = 0;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = open64(pathname, flags, mode);

    event.syscallno = SYS_open;
    event.nsysargs = 3;
    event.sysargs[0] = (long) pathname;
    event.sysargs[1] = flags;
    event.sysargs[2] = mode;
    event.retval = retval;
    strncpy(currentpathname,pathname,strlen(pathname));

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "open64");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(open64));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int iotclose(int fd) 
{    
    int retval;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = close(fd);

    event.syscallno = SYS_close;
    event.nsysargs = 1;
    event.sysargs[0] = fd;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "close");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(close));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int iotdup(int oldfd) 
{    
    int retval;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = dup(oldfd);

    event.syscallno = SYS_dup;
    event.nsysargs = 1;
    event.sysargs[0] = oldfd;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "dup");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    /* defined(__i386) */
    iot_record_event(&event, OpenSS_GetAddressOfFunction(dup));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int iotdup2(int oldfd, int newfd) 
{
    int retval;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = dup2(oldfd,newfd);

    event.syscallno = SYS_dup2;
    event.nsysargs = 2;
    event.sysargs[0] = oldfd;
    event.sysargs[1] = newfd;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "dup2");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(dup2));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

int iotcreat(char *pathname, mode_t mode) 
{
    int retval = 0;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = creat(pathname,mode);

    event.retval = retval;
    event.syscallno = SYS_creat;
    event.nsysargs = 2;
    event.sysargs[0] = (long) pathname;
    event.sysargs[1] = mode;

    event.stop_time = OpenSS_GetTime();

    strncpy(currentpathname,pathname,strlen(pathname));

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "creat");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(creat));
#endif
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

int iotcreat64(char *pathname, mode_t mode) 
{
    int retval;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = creat64(pathname,mode);

    event.syscallno = SYS_creat;
    event.nsysargs = 2;
    event.sysargs[0] = (long) pathname;
    event.sysargs[1] = mode;
    event.retval = retval;
    strncpy(currentpathname,pathname,strlen(pathname));

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "creat64");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(creat64));
#endif
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

int iotpipe(int filedes[2]) 
{
    int retval;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = pipe(filedes);

    event.syscallno = SYS_pipe;
    event.nsysargs = 1;
    event.sysargs[0] = (long) filedes;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "pipe");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(pipe));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t iotpread(int fd, void *buf, size_t count, off_t offset) 
{
    ssize_t retval;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = pread(fd, buf, count, offset);

#if   defined(__linux) && defined(SYS_pread)
    event.syscallno = SYS_pread;
#elif defined(__linux) && defined(SYS_pread64)
    event.syscallno = SYS_pread64;
#else
#error "SYS_pread or SYS_pread64 is not defined"
#endif
    event.nsysargs = 4;
    event.sysargs[0] = fd;
    event.sysargs[1] = (long) buf;
    event.sysargs[2] = count;
    event.sysargs[3] = offset;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "pread");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(pread));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t iotpread64(int fd, void *buf, size_t count, off_t offset) 
{
    ssize_t retval;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = pread64(fd, buf, count, offset);

#if   defined(__linux) && defined(SYS_pread)
    event.syscallno = SYS_pread;
#elif defined(__linux) && defined(SYS_pread64)
    event.syscallno = SYS_pread64;
#else
#error "SYS_pread or SYS_pread64 is not defined"
#endif
    event.nsysargs = 4;
    event.sysargs[0] = fd;
    event.sysargs[1] = (long) buf;
    event.sysargs[2] = count;
    event.sysargs[3] = offset;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "pread64");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(pread64));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t iotpwrite(int fd, void *buf, size_t count, off_t offset) 
{
    ssize_t retval;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = pwrite(fd, buf, count, offset);

#if   defined(__linux) && defined(SYS_pwrite)
    event.syscallno = SYS_pwrite;
#elif defined(__linux) && defined(SYS_pwrite64)
    event.syscallno = SYS_pwrite64;
#else
#error "SYS_pwrite or SYS_pwrite64 is not defined"
#endif
    event.nsysargs = 4;
    event.sysargs[0] = fd;
    event.sysargs[1] = (long) buf;
    event.sysargs[2] = count;
    event.sysargs[3] = offset;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "pwrite");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(pwrite));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t iotpwrite64(int fd, void *buf, size_t count, off_t offset) 
{
    ssize_t retval;
    iot_event event;

    iot_start_event(&event);

    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = pwrite64(fd, buf, count, offset);

#if   defined(__linux) && defined(SYS_pwrite)
    event.syscallno = SYS_pwrite;
#elif defined(__linux) && defined(SYS_pwrite64)
    event.syscallno = SYS_pwrite64;
#else
#error "SYS_pwrite or SYS_pwrite64 is not defined"
#endif
    event.nsysargs = 4;
    event.sysargs[0] = fd;
    event.sysargs[1] = (long) buf;
    event.sysargs[2] = count;
    event.sysargs[3] = offset;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "pwrite64");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(pwrite64));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

ssize_t iotreadv(int fd, const struct iovec *vector, size_t count) 
{
    ssize_t retval;
    iot_event event;

    iot_start_event(&event);


    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = readv(fd, vector, count);

    event.syscallno = SYS_readv;
    event.nsysargs = 3;
    event.sysargs[0] = fd;
    event.sysargs[1] = (long) vector;
    event.sysargs[2] = count;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "readv");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(readv));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}


ssize_t iotwritev(int fd, const struct iovec *vector, size_t count) 
{
    ssize_t retval;
    iot_event event;

    iot_start_event(&event);


    event.start_time = OpenSS_GetTime();

    /* Call the real IO function */
    retval = writev(fd, vector, count);

    event.syscallno = SYS_writev;
    event.nsysargs = 3;
    event.sysargs[0] = fd;
    event.sysargs[1] = (long) vector;
    event.sysargs[2] = count;
    event.retval = retval;

    event.stop_time = OpenSS_GetTime();

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    void (*realfunc)() = dlsym (RTLD_NEXT, "writev");
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction(writev));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
