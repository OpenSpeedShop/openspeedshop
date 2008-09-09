/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2008 The Krell Institute. All Rights Reserved.
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

#include <dlfcn.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>

#include "runtime.h"


/*
 * IO Wrapper Functions
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

#if defined(OPENSS_OFFLINE)
#ifdef read
#undef read
#endif
ssize_t read(int fd, void *buf, size_t count) 
#else
ssize_t ioread(int fd, void *buf, size_t count) 
#endif
{
    ssize_t retval;
    io_event event;

    bool_t dotrace = io_do_trace("read");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "read");


    /* Call the real IO function */
    retval = (*realfunc)(fd, buf, count);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
ssize_t write(int fd, __const void *buf, size_t count) 
#else
ssize_t iowrite(int fd, void *buf, size_t count) 
#endif
{    
    ssize_t retval;
    io_event event;

    bool_t dotrace = io_do_trace("write");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "write");


    /* Call the real IO function */
    retval = (*realfunc)(fd, buf, count);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
off_t lseek(int fd, off_t offset, int whence) 
#else
off_t iolseek(int fd, off_t offset, int whence) 
#endif
{    
    off_t retval;
    io_event event;

    bool_t dotrace = io_do_trace("lseek");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    off_t (*realfunc)() = dlsym (RTLD_NEXT, "lseek");


    /* Call the real IO function */
    retval = (*realfunc)(fd, offset, whence);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
__off64_t lseek64(int fd, __off64_t offset, int whence) 
#else
__off_t iolseek64(int fd, __off_t offset, int whence) 
#endif
{    
    off_t retval;
    io_event event;

    bool_t dotrace = io_do_trace("lseek64");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    off_t (*realfunc)() = dlsym (RTLD_NEXT, "lseek64");


    /* Call the real IO function */
    retval = (*realfunc)(fd, offset, whence);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int open(const char *pathname, int flags, mode_t mode) 
#else
int ioopen(const char *pathname, int flags, mode_t mode) 
#endif
{    
    int retval = 0;
    io_event event;

    bool_t dotrace = io_do_trace("open");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    int (*realfunc)() = dlsym (RTLD_NEXT, "open");


    /* Call the real IO function */
    retval = (*realfunc)(pathname, flags, mode);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int open64(const char *pathname, int flags, mode_t mode) 
#else
int ioopen64(const char *pathname, int flags, mode_t mode) 
#endif
{    
    int retval = 0;
    io_event event;

    bool_t dotrace = io_do_trace("open64");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    int (*realfunc)() = dlsym (RTLD_NEXT, "open64");

    /* Call the real IO function */
    retval = (*realfunc)(pathname, flags, mode);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int close(int fd) 
#else
int ioclose(int fd) 
#endif
{    
    int retval;
    io_event event;

    bool_t dotrace = io_do_trace("close");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    int (*realfunc)() = dlsym (RTLD_NEXT, "close");


    /* Call the real IO function */
    retval = (*realfunc)(fd);

    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int dup(int oldfd) 
#else
int iodup(int oldfd) 
#endif
{    
    int retval;
    io_event event;

    bool_t dotrace = io_do_trace("dup");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    int (*realfunc)() = dlsym (RTLD_NEXT, "dup");

    /* Call the real IO function */
    retval = (*realfunc)(oldfd);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    /* defined(__i386) */
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int dup2(int oldfd, int newfd) 
#else
int iodup2(int oldfd, int newfd) 
#endif
{    
    int retval;
    io_event event;

    bool_t dotrace = io_do_trace("dup2");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    int (*realfunc)() = dlsym (RTLD_NEXT, "dup2");

    /* Call the real IO function */
    retval = (*realfunc)(oldfd,newfd);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int creat(char *pathname, mode_t mode) 
#else
int iocreat(char *pathname, mode_t mode) 
#endif
{
    int retval = 0;
    io_event event;

    bool_t dotrace = io_do_trace("creat");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    int (*realfunc)() = dlsym (RTLD_NEXT, "creat");

    /* Call the real IO function */
    retval = (*realfunc)(pathname,mode);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }    

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int creat64(char *pathname, mode_t mode) 
#else
int iocreat64(char *pathname, mode_t mode) 
#endif
{
    int retval;
    io_event event;

    bool_t dotrace = io_do_trace("creat64");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    int (*realfunc)() = dlsym (RTLD_NEXT, "creat64");

    /* Call the real IO function */
    retval = (*realfunc)(pathname,mode);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int pipe(int filedes[2]) 
#else
int iopipe(int filedes[2]) 
#endif
{    
    int retval;
    io_event event;

    bool_t dotrace = io_do_trace("pipe");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    int (*realfunc)() = dlsym (RTLD_NEXT, "pipe");

    /* Call the real IO function */
    retval = (*realfunc)(filedes);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
ssize_t pread(int fd, void *buf, size_t count, off_t offset) 
#else
ssize_t iopread(int fd, void *buf, size_t count, off_t offset) 
#endif
{    
    ssize_t retval;
    io_event event;

    bool_t dotrace = io_do_trace("pread");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pread");

    /* Call the real IO function */
    retval = (*realfunc)(fd, buf, count, offset);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
ssize_t pread64(int fd, void *buf, size_t count, __off64_t offset) 
#else
ssize_t iopread64(int fd, void *buf, size_t count, off_t offset) 
#endif
{    
    ssize_t retval;
    io_event event;

    bool_t dotrace = io_do_trace("pread64");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pread64");

    /* Call the real IO function */
    retval = (*realfunc)(fd, buf, count, offset);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
ssize_t pwrite(int fd, __const void *buf, size_t count, __off_t offset) 
#else
ssize_t iopwrite(int fd, __const void *buf, size_t count, __off64_t offset) 
#endif
{    
    ssize_t retval;
    io_event event;

    bool_t dotrace = io_do_trace("pwrite");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pwrite");

    /* Call the real IO function */
    retval = (*realfunc)(fd, buf, count, offset);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
ssize_t pwrite64(int fd, __const void *buf, size_t count, __off64_t offset) 
#else
ssize_t iopwrite64(int fd, void *buf, size_t count, off_t offset) 
#endif
{    
    ssize_t retval;
    io_event event;

    bool_t dotrace = io_do_trace("pwrite64");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pwrite64");

    /* Call the real IO function */
    retval = (*realfunc)(fd, buf, count, offset);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if !defined(OPENSS_OFFLINE)
#if defined(OPENSS_OFFLINE)
ssize_t readv(int fd, const struct iovec *vector, size_t count) 
#else
ssize_t ioreadv(int fd, const struct iovec *vector, size_t count) 
#endif
{    
    ssize_t retval;
    io_event event;

    bool_t dotrace = io_do_trace("readv");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "readv");

    /* Call the real IO function */
    retval = (*realfunc)(fd, vector, count);

    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}


#if defined(OPENSS_OFFLINE)
ssize_t writev(int fd, const struct iovec *vector, size_t count) 
#else
ssize_t iowritev(int fd, const struct iovec *vector, size_t count) 
#endif
{    
    ssize_t retval;
    io_event event;

    bool_t dotrace = io_do_trace("writev");

    if (dotrace) {
        io_start_event(&event);
        event.start_time = OpenSS_GetTime();
    }

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "writev");

    /* Call the real IO function */
    retval = (*realfunc)(fd, vector, count);


    if (dotrace) {
        event.stop_time = OpenSS_GetTime();
    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        io_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
#endif
