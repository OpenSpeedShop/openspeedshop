/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2008-2016 The Krell Institute. All Rights Reserved.
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

#define _GNU_SOURCE

#include "RuntimeAPI.h"

#include "blobs.h"
/* Start part 1 of 2 for Hack to get around inconsistent syscall definitions */
/* #include <syscall.h> */
/* End part 1 of 2 for Hack to get around inconsistent syscall definitions */

#if !defined(OPENSS_OFFLINE)
#include <syscall.h>
#include <unistd.h>
#include <fcntl.h>
#endif

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


#if defined(OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
ssize_t read(int fd, void *buf, size_t count) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
ssize_t __wrap_read(int fd, void *buf, size_t count) 
#else
ssize_t iotread(int fd, void *buf, size_t count) 
#endif
{
    ssize_t retval;
    int status = -1;
    char namebuf[1024];
    char pf[256];
    memset(namebuf, 0, sizeof(namebuf));
    memset(pf, 0, sizeof(pf));

    iot_event event;

    bool_t dotrace = iot_do_trace("read");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_read(fd, buf, count);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "read");
    retval = (*realfunc)(fd, buf, count);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_read;
    event.nsysargs = 3;
    event.sysargs[0] = fd;
    event.sysargs[1] = (long) buf;
    event.sysargs[2] = count;
    event.retval = retval;

#ifdef DEBUG_IOT
    printf("iotread, fd=%d, namebuf=%s\n", fd, namebuf);
#endif
    /* use that to get the path into /proc. */
    sprintf(pf,"/proc/self/fd/%d",fd);

    /* Read the link the file descriptor points to in the /proc filesystem */
    status = readlink(pf,namebuf,1024);
    if (status > 1024) {
      printf("ERROR, name too large\n");
    }
    namebuf[status] = 0;
    strncpy(currentpathname,namebuf,strlen(namebuf));
/*
    event.sysargs[3] = namebuf;
*/

#ifdef DEBUG_IOT
    printf("iotread, status=%d, namebuf=%s\n", status, namebuf);
#endif


    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_read));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_read);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#ifndef DEBUG
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
ssize_t write(int fd, __const void *buf, size_t count) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
ssize_t __wrap_write(int fd, __const void *buf, size_t count) 
#else
ssize_t iotwrite(int fd, void *buf, size_t count) 
#endif
{    
    ssize_t retval;
    int status = -1;
    char namebuf[1024];
    char pf[256];
    memset(namebuf, 0, sizeof(namebuf));
    memset(pf, 0, sizeof(pf));

    iot_event event;

    bool_t dotrace = iot_do_trace("write");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_write(fd, buf, count);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "write");
    retval = (*realfunc)(fd, buf, count);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_write;
    event.nsysargs = 3;
    event.sysargs[0] = fd;
    event.sysargs[1] = (long) buf;
    event.sysargs[2] = count;
    event.retval = retval;

#ifdef DEBUG_IOT
    printf("iotwrite, fd=%d, namebuf=%s\n", fd, namebuf);
#endif
    /* use that to get the path into /proc. */
    sprintf(pf,"/proc/self/fd/%d",fd);

    /* Read the link the file descriptor points to in the /proc filesystem */
    status = readlink(pf,namebuf,1024);
    if (status > 1024) {
      printf("ERROR, name too large\n");
    }
    namebuf[status] = 0;
    strncpy(currentpathname,namebuf,strlen(namebuf));


    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_write));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_write);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    

    /* Return the real IO function's return value to the caller */
    return retval;
}
#endif

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
off_t lseek(int fd, off_t offset, int whence) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
off_t __wrap_lseek(int fd, off_t offset, int whence) 
#else
off_t iotlseek(int fd, off_t offset, int whence) 
#endif
{
    off_t retval;
    int status = -1;
    char namebuf[1024];
    char pf[256];
    memset(namebuf, 0, sizeof(namebuf));
    memset(pf, 0, sizeof(pf));

    iot_event event;

    bool_t dotrace = iot_do_trace("lseek");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_lseek(fd, offset, whence);
#else
    off_t (*realfunc)() = dlsym (RTLD_NEXT, "lseek");
    retval = (*realfunc)(fd, offset, whence);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_lseek;
    event.nsysargs = 3;
    event.sysargs[0] = fd;
    event.sysargs[1] = offset;
    event.sysargs[2] = whence;
    event.retval = retval;

    /* use that to get the path into /proc. */
    sprintf(pf,"/proc/self/fd/%d",fd);

    /* Read the link the file descriptor points to in the /proc filesystem */
    status = readlink(pf,namebuf,1024);
    if (status > 1024) {
      printf("ERROR, name too large\n");
    }
    namebuf[status] = 0;
    strncpy(currentpathname,namebuf,strlen(namebuf));
#ifdef DEBUG_IOT
    printf("iotlseek, fd=%d, namebuf=%s\n", fd, namebuf);
#endif


    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_lseek));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_lseek);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
__off64_t lseek64(int fd, __off64_t offset, int whence) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
__off64_t __wrap_lseek64(int fd, __off64_t offset, int whence) 
#else
off_t iotlseek64(int fd, off_t offset, int whence) 
#endif
{
    off_t retval;
    int status = -1;
    char namebuf[1024];
    char pf[256];
    memset(namebuf, 0, sizeof(namebuf));
    memset(pf, 0, sizeof(pf));

    iot_event event;

    bool_t dotrace = iot_do_trace("lseek64");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_lseek64(fd, offset, whence);
#else
    off_t (*realfunc)() = dlsym (RTLD_NEXT, "lseek64");
    retval = (*realfunc)(fd, offset, whence);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_lseek;
    event.nsysargs = 3;
    event.sysargs[0] = fd;
    event.sysargs[1] = offset;
    event.sysargs[2] = whence;
    event.retval = retval;

    /* use that to get the path into /proc. */
    sprintf(pf,"/proc/self/fd/%d",fd);

    /* Read the link the file descriptor points to in the /proc filesystem */
    status = readlink(pf,namebuf,1024);
    if (status > 1024) {
      printf("ERROR, name too large\n");
    }
    namebuf[status] = 0;
    strncpy(currentpathname,namebuf,strlen(namebuf));
#ifdef DEBUG_IOT
    printf("iotlseek64, fd=%d, namebuf=%s\n", fd, namebuf);
#endif


    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_lseek64));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_lseek64);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int open(const char *pathname, int flags, mode_t mode) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_open(const char *pathname, int flags, mode_t mode) 
#else
int iotopen(const char *pathname, int flags, mode_t mode) 
#endif
{
    int retval = 0;

    iot_event event;

    bool_t dotrace = iot_do_trace("open");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_open(pathname, flags, mode);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "open");
    retval = (*realfunc)(pathname, flags, mode);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.retval = retval;
    event.syscallno = SYS_open;
    event.nsysargs = 3;
    event.sysargs[0] = (long) pathname;
    event.sysargs[1] = flags;
    event.sysargs[2] = mode;

    strncpy(currentpathname,pathname,strlen(pathname));

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_open));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_open);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int open64(const char *pathname, int flags, mode_t mode) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_open64(const char *pathname, int flags, mode_t mode) 
#else
int iotopen64(const char *pathname, int flags, mode_t mode) 
#endif
{
    int retval = 0;

    iot_event event;

    bool_t dotrace = iot_do_trace("open64");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_open64(pathname, flags, mode);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "open64");
    retval = (*realfunc)(pathname, flags, mode);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_open;
    event.nsysargs = 3;
    event.sysargs[0] = (long) pathname;
    event.sysargs[1] = flags;
    event.sysargs[2] = mode;
    event.retval = retval;
    strncpy(currentpathname,pathname,strlen(pathname));


    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_open64));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_open64);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int close(int fd) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_close(int fd) 
#else
int iotclose(int fd) 
#endif
{
    int retval;
    int status = -1;
    char namebuf[1024];
    char pf[256];
    memset(namebuf, 0, sizeof(namebuf));
    memset(pf, 0, sizeof(pf));

    iot_event event;

    bool_t dotrace = iot_do_trace("close");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();

	/* use that to get the path into /proc. */
	sprintf(pf,"/proc/self/fd/%d",fd);

	/* Read the link the file descriptor points to in the /proc filesystem */
	status = readlink(pf,namebuf,1024);
	if (status > 1024) {
	    printf("ERROR, name too large\n");
	}
	namebuf[status] = 0;
#ifdef DEBUG_IOT
	printf("iotclose, fd=%d, namebuf=%s\n", fd, namebuf);
#endif

	strncpy(currentpathname,namebuf,strlen(namebuf));
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_close(fd);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "close");
    retval = (*realfunc)(fd);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_close;
    event.nsysargs = 1;
    event.sysargs[0] = fd;
    event.retval = retval;


    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_close));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_close);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int dup(int oldfd) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_dup(int oldfd) 
#else
int iotdup(int oldfd) 
#endif
{
    int retval;
    int status = -1;
    char namebuf[1024];
    char pf[256];
    memset(namebuf, 0, sizeof(namebuf));
    memset(pf, 0, sizeof(pf));

    iot_event event;

    bool_t dotrace = iot_do_trace("dup");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_dup(oldfd);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "dup");
    retval = (*realfunc)(oldfd);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_dup;
    event.nsysargs = 1;
    event.sysargs[0] = oldfd;
    event.retval = retval;

    /* use that to get the path into /proc. */
    sprintf(pf,"/proc/self/fd/%d",oldfd);

    /* Read the link the file descriptor points to in the /proc filesystem */
    status = readlink(pf,namebuf,1024);
    if (status > 1024) {
      printf("ERROR, name too large\n");
    }
    namebuf[status] = 0;
    strncpy(currentpathname,namebuf,strlen(namebuf));
#ifdef DEBUG_IOT
    printf("iotdup, oldfd=%d, namebuf=%s\n", oldfd, namebuf);
#endif


    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_dup));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_dup);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int dup2(int oldfd, int newfd) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_dup2(int oldfd, int newfd) 
#else
int iotdup2(int oldfd, int newfd) 
#endif
{
    int retval;
    int status = -1;
    char namebuf[1024];
    char pf[256];
    memset(namebuf, 0, sizeof(namebuf));
    memset(pf, 0, sizeof(pf));

    iot_event event;

    bool_t dotrace = iot_do_trace("dup2");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_dup2(oldfd,newfd);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "dup2");
    retval = (*realfunc)(oldfd,newfd);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_dup2;
    event.nsysargs = 2;
    event.sysargs[0] = oldfd;
    event.sysargs[1] = newfd;
    event.retval = retval;

    /* use that to get the path into /proc. */
    sprintf(pf,"/proc/self/fd/%d",oldfd);

    /* Read the link the file descriptor points to in the /proc filesystem */
    status = readlink(pf,namebuf,1024);
    if (status > 1024) {
      printf("ERROR, name too large\n");
    }
    namebuf[status] = 0;
    strncpy(currentpathname,namebuf,strlen(namebuf));
#ifdef DEBUG_IOT
    printf("iotdup2, oldfd=%d, namebuf=%s\n", oldfd, namebuf);
#endif


    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_dup2));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_dup2);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int creat(char *pathname, mode_t mode) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_creat(char *pathname, mode_t mode) 
#else
int iotcreat(char *pathname, mode_t mode) 
#endif
{
    int retval = 0;

    iot_event event;

    bool_t dotrace = iot_do_trace("creat");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_creat(pathname,mode);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "creat");
    retval = (*realfunc)(pathname,mode);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.retval = retval;
    event.syscallno = SYS_creat;
    event.nsysargs = 2;
    event.sysargs[0] = (long) pathname;
    event.sysargs[1] = mode;

    strncpy(currentpathname,pathname,strlen(pathname));

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_creat));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_creat);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }    

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int creat64(char *pathname, mode_t mode) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_creat64(char *pathname, mode_t mode) 
#else
int iotcreat64(char *pathname, mode_t mode) 
#endif
{
    int retval;

    iot_event event;

    bool_t dotrace = iot_do_trace("create64");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_creat64(pathname,mode);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "creat64");
    retval = (*realfunc)(pathname,mode);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_creat;
    event.nsysargs = 2;
    event.sysargs[0] = (long) pathname;
    event.sysargs[1] = mode;
    event.retval = retval;
    strncpy(currentpathname,pathname,strlen(pathname));

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_creat64));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_creat64);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int pipe(int filedes[2]) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_pipe(int filedes[2]) 
#else
int iotpipe(int filedes[2]) 
#endif
{
    int retval;

    iot_event event;

    bool_t dotrace = iot_do_trace("pipe");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_pipe(filedes);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pipe");
    retval = (*realfunc)(filedes);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_pipe;
    event.nsysargs = 1;
    event.sysargs[0] = (long) filedes;
    event.retval = retval;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_pipe));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_pipe);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
ssize_t pread(int fd, void *buf, size_t count, off_t offset) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
ssize_t __wrap_pread(int fd, void *buf, size_t count, off_t offset) 
#else
ssize_t iotpread(int fd, void *buf, size_t count, off_t offset)
#endif
{
    ssize_t retval;

    iot_event event;

    bool_t dotrace = iot_do_trace("pread");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_pread(fd, buf, count, offset);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pread");
    retval = (*realfunc)(fd, buf, count, offset);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

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

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_pread));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_pread);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
ssize_t pread64(int fd, void *buf, size_t count, __off64_t offset) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
ssize_t __wrap_pread64(int fd, void *buf, size_t count, __off64_t offset) 
#else
ssize_t iotpread64(int fd, void *buf, size_t count, off_t offset) 
#endif
{
    ssize_t retval;
    int status = -1;
    char namebuf[1024];
    char pf[256];
    memset(namebuf, 0, sizeof(namebuf));
    memset(pf, 0, sizeof(pf));

    iot_event event;

    bool_t dotrace = iot_do_trace("pread64");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_pread64(fd, buf, count, offset);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pread64");
    retval = (*realfunc)(fd, buf, count, offset);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

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

#ifdef DEBUG_IOT
    printf("iotpread64, fd=%d, namebuf=%s\n", fd, namebuf);
#endif
    /* use that to get the path into /proc. */
    sprintf(pf,"/proc/self/fd/%d",fd);

    /* Read the link the file descriptor points to in the /proc filesystem */
    status = readlink(pf,namebuf,1024);
    if (status > 1024) {
      printf("ERROR, name too large\n");
    }
    namebuf[status] = 0;
    strncpy(currentpathname,namebuf,strlen(namebuf));

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_pread64));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_pread64);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#ifndef DEBUG
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
ssize_t pwrite(int fd, __const void *buf, size_t count, __off_t offset) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
ssize_t __wrap_pwrite(int fd, __const void *buf, size_t count, __off_t offset) 
#else
ssize_t iotpwrite(int fd, void *buf, size_t count, off_t offset) 
#endif
{
    ssize_t retval;
    int status = -1;
    char namebuf[1024];
    char pf[256];
    memset(namebuf, 0, sizeof(namebuf));
    memset(pf, 0, sizeof(pf));

    iot_event event;

    bool_t dotrace = iot_do_trace("pwrite");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_pwrite(fd, buf, count, offset);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pwrite");
    retval = (*realfunc)(fd, buf, count, offset);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

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
#ifdef DEBUG_IOT
    printf("iotpwrite, fd=%d, namebuf=%s\n", fd, namebuf);
#endif
    /* use that to get the path into /proc. */
    sprintf(pf,"/proc/self/fd/%d",fd);

    /* Read the link the file descriptor points to in the /proc filesystem */
    status = readlink(pf,namebuf,1024);
    if (status > 1024) {
      printf("ERROR, name too large\n");
    }
    namebuf[status] = 0;
    strncpy(currentpathname,namebuf,strlen(namebuf));

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_pwrite));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_pwrite);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
#endif

#ifndef DEBUG
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
ssize_t pwrite64(int fd, __const void *buf, size_t count, __off64_t offset) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
ssize_t __wrap_pwrite64(int fd, __const void *buf, size_t count, __off64_t offset) 
#else
ssize_t iotpwrite64(int fd, void *buf, size_t count, off_t offset) 
#endif
{
    ssize_t retval;
    int status = -1;
    char namebuf[1024];
    char pf[256];
    memset(namebuf, 0, sizeof(namebuf));
    memset(pf, 0, sizeof(pf));

    iot_event event;

    bool_t dotrace = iot_do_trace("pwrite64");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_pwrite64(fd, buf, count, offset);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pwrite64");
    retval = (*realfunc)(fd, buf, count, offset);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

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

#ifdef DEBUG_IOT
    printf("iotwrite64, fd=%d, namebuf=%s\n", fd, namebuf);
#endif
    /* use that to get the path into /proc. */
    sprintf(pf,"/proc/self/fd/%d",fd);

    /* Read the link the file descriptor points to in the /proc filesystem */
    status = readlink(pf,namebuf,1024);
    if (status > 1024) {
      printf("ERROR, name too large\n");
    }
    namebuf[status] = 0;
    strncpy(currentpathname,namebuf,strlen(namebuf));

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_pwrite64));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_pwrite64);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
#endif

#if !defined(OPENSS_OFFLINE)
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
ssize_t readv(int fd, const struct iovec *vector, size_t count) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
ssize_t __wrap_readv(int fd, const struct iovec *vector, size_t count) 
#else
ssize_t iotreadv(int fd, const struct iovec *vector, size_t count) 
#endif
{
    ssize_t retval;
    int status = -1;
    char namebuf[1024];
    char pf[256];
    memset(namebuf, 0, sizeof(namebuf));
    memset(pf, 0, sizeof(pf));

    iot_event event;

    bool_t dotrace = iot_do_trace("readv");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_readv(fd, vector, count);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "readv");
    retval = (*realfunc)(fd, vector, count);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_readv;
    event.nsysargs = 3;
    event.sysargs[0] = fd;
    event.sysargs[1] = (long) vector;
    event.sysargs[2] = count;
    event.retval = retval;
#ifdef DEBUG_IOT
    printf("iotreadv, fd=%d, namebuf=%s\n", fd, namebuf);
#endif
    /* use that to get the path into /proc. */
    sprintf(pf,"/proc/self/fd/%d",fd);

    /* Read the link the file descriptor points to in the /proc filesystem */
    status = readlink(pf,namebuf,1024);
    if (status > 1024) {
      printf("ERROR, name too large\n");
    }
    namebuf[status] = 0;
    strncpy(currentpathname,namebuf,strlen(namebuf));

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_readv));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_readv);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}


#ifndef DEBUG
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
ssize_t writev(int fd, const struct iovec *vector, size_t count) 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
ssize_t __wrap_writev(int fd, const struct iovec *vector, size_t count) 
#else
ssize_t iotwritev(int fd, const struct iovec *vector, size_t count) 
#endif
{
    ssize_t retval;
    int status = -1;
    char namebuf[1024];
    char pf[256];
    memset(namebuf, 0, sizeof(namebuf));
    memset(pf, 0, sizeof(pf));

    iot_event event;

    bool_t dotrace = iot_do_trace("writev");

    if (dotrace) {
	iot_start_event(&event);
	event.start_time = OpenSS_GetTime();
    }

    /* Call the real IO function */
#if defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
    retval = __real_writev(fd, vector, count);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "writev");
    retval = (*realfunc)(fd, vector, count);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_writev;
    event.nsysargs = 3;
    event.sysargs[0] = fd;
    event.sysargs[1] = (long) vector;
    event.sysargs[2] = count;
    event.retval = retval;

#ifdef DEBUG_IOT
    printf("iotwritev, fd=%d, namebuf=%s\n", fd, namebuf);
#endif
    /* use that to get the path into /proc. */
    sprintf(pf,"/proc/self/fd/%d",fd);

    /* Read the link the file descriptor points to in the /proc filesystem */
    status = readlink(pf,namebuf,1024);
    if (status > 1024) {
      printf("ERROR, name too large\n");
    }
    namebuf[status] = 0;
    strncpy(currentpathname,namebuf,strlen(namebuf));


    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if ! defined (OPENSS_STATIC) 
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((const void *) __real_writev));
#endif
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
        iot_record_event(&event, (uint64_t) __real_writev);
#else
        iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
#endif
#endif
