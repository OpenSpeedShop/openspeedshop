//////////////////////////////// -*- C++ -*- //////////////////////////////
//
// AUTHOR
//    Peter Shirley, Cornell University, shirley@graphics.cornell.edu 
//
// COPYRIGHT
//    Copyright (c) 1995  Peter Shirley. All rights reserved.
//
//    Permission to use, copy, modify, and distribute this software for any
//    purpose without fee is hereby granted, provided that this entire
//    notice is included in all copies of any software which is or includes
//    a copy or modification of this software and in all copies of the
//    a copy or modification of this software and in all copies of the
//    supporting documentation for such software.
//
// DISCLAIMER
//    Neither the Peter Shirley nor Cornell nor any of their
//    employees, makes any warranty, express or implied, or assumes any
//    liability or responsibility for the accuracy, completeness, or
//    usefulness of any information, apparatus, product, or process
//    disclosed, or represents that its use would not infringe
//    privately-owned rights.  
//
///////////////////////////////////////////////////////////////////////////



#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ggErr.h>
#include <errno.h>



char *ggProgram = 0;

#ifndef HAS_ERRLIST
extern int sys_nerr;
extern char *sys_errlist[];
#endif
extern int errno;


static void err(char *func, char *fmt, va_list args);



void
ggSysErr(int rc, char *func, char *fmt ...) {
    va_list args;

    va_start(args, fmt);
    err(func, fmt, args);
#ifdef USE_STRERROR
        fprintf(stderr, "     system message: %s\n", strerror(errno));
#else
#if (0)
    if (errno < sys_nerr)
        fprintf(stderr, "     system message: %s\n", sys_errlist[errno]);
#endif
#endif
    va_end(args);
    exit(rc);
}

void
ggWarnErr(char *func, char *fmt ...) {
    va_list args;

    va_start(args, fmt);
    err(func, fmt, args);

    va_end(args);
}

void
ggFatlErr(int rc, char *func, char *fmt ...) {
    va_list args;

    va_start(args, fmt);
    err(func, fmt, args);

    va_end(args);
    exit(rc);
}

static void
err(char *func, char *fmt, va_list args) {
    
    if (ggProgram != 0)
        fprintf(stderr, "%s", ggProgram);
    if (func != 0 && strcmp(func, "") != 0)
        fprintf(stderr, "(%s)", func);
    if (func || ggProgram)
        fprintf(stderr, ": ");

    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    fflush(stderr);
}



/* vi:set ts=4 sw=4: */
