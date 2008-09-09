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


#if defined(OPENSS_OFFLINE)
ssize_t read(int fd, void *buf, size_t count) 
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

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "read");


    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(fd, buf, count);
#else
    retval = read(fd, buf, count);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#ifndef DEBUG
#if defined(OPENSS_OFFLINE)
ssize_t write(int fd, __const void *buf, size_t count) 
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

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "write");

#if defined(OPENSS_OFFLINE)
    /* Call the real IO function */
    retval = (*realfunc)(fd, buf, count);
#else
    retval = write(fd, buf, count);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    

    /* Return the real IO function's return value to the caller */
    return retval;
}
#endif

#if defined(OPENSS_OFFLINE)
off_t lseek(int fd, off_t offset, int whence) 
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

    off_t (*realfunc)() = dlsym (RTLD_NEXT, "lseek");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(fd, offset, whence);
#else
    retval = lseek(fd, offset, whence);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
__off64_t lseek64(int fd, __off64_t offset, int whence) 
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

    off_t (*realfunc)() = dlsym (RTLD_NEXT, "lseek64");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(fd, offset, whence);
#else
    retval = lseek64(fd, offset, whence);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int open(const char *pathname, int flags, mode_t mode) 
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

    int (*realfunc)() = dlsym (RTLD_NEXT, "open");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(pathname, flags, mode);
#else
    retval = open(pathname, flags, mode);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int open64(const char *pathname, int flags, mode_t mode) 
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

    int (*realfunc)() = dlsym (RTLD_NEXT, "open64");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(pathname, flags, mode);
#else
    retval = open64(pathname, flags, mode);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int close(int fd) 
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

    int (*realfunc)() = dlsym (RTLD_NEXT, "close");


    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(fd);
#else
    retval = close(fd);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_close;
    event.nsysargs = 1;
    event.sysargs[0] = fd;
    event.retval = retval;


    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int dup(int oldfd) 
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

    int (*realfunc)() = dlsym (RTLD_NEXT, "dup");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(oldfd);
#else
    retval = dup(oldfd);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    /* defined(__i386) */
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int dup2(int oldfd, int newfd) 
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

    int (*realfunc)() = dlsym (RTLD_NEXT, "dup2");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(oldfd,newfd);
#else
    retval = dup2(oldfd,newfd);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int creat(char *pathname, mode_t mode) 
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

    int (*realfunc)() = dlsym (RTLD_NEXT, "creat");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(pathname,mode);
#else
    retval = creat(pathname,mode);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int creat64(char *pathname, mode_t mode) 
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

    int (*realfunc)() = dlsym (RTLD_NEXT, "creat64");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(pathname,mode);
#else
    retval = creat64(pathname,mode);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
int pipe(int filedes[2]) 
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

    int (*realfunc)() = dlsym (RTLD_NEXT, "pipe");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(filedes);
#else
    retval = pipe(filedes);
#endif


    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.syscallno = SYS_pipe;
    event.nsysargs = 1;
    event.sysargs[0] = (long) filedes;
    event.retval = retval;

    /* Record event and it's stacktrace*/
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
ssize_t pread(int fd, void *buf, size_t count, off_t offset) 
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

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pread");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(fd, buf, count, offset);
#else
    retval = pread(fd, buf, count, offset);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined(OPENSS_OFFLINE)
ssize_t pread64(int fd, void *buf, size_t count, __off64_t offset) 
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

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pread64");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(fd, buf, count, offset);
#else
    retval = pread64(fd, buf, count, offset);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#ifndef DEBUG
#if defined(OPENSS_OFFLINE)
ssize_t pwrite(int fd, __const void *buf, size_t count, __off_t offset) 
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

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pwrite");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(fd, buf, count, offset);
#else
    retval = pwrite(fd, buf, count, offset);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
#endif

#ifndef DEBUG
#if defined(OPENSS_OFFLINE)
ssize_t pwrite64(int fd, __const void *buf, size_t count, __off64_t offset) 
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

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pwrite64");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(fd, buf, count, offset);
#else
    retval = pwrite64(fd, buf, count, offset);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
#endif

#if !defined(OPENSS_OFFLINE)
#if defined(OPENSS_OFFLINE)
ssize_t readv(int fd, const struct iovec *vector, size_t count) 
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

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "readv");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(fd, vector, count);
#else
    retval = readv(fd, vector, count);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}


#ifndef DEBUG
#if defined(OPENSS_OFFLINE)
ssize_t writev(int fd, const struct iovec *vector, size_t count) 
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

    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "writev");

    /* Call the real IO function */
#if defined(OPENSS_OFFLINE)
    retval = (*realfunc)(fd, vector, count);
#else
    retval = writev(fd, vector, count);
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
#if defined(__linux) && defined(__x86_64)
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#else
    iot_record_event(&event, OpenSS_GetAddressOfFunction((*realfunc)));
#endif

    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
#endif
#endif
