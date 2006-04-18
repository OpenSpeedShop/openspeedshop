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

/** @file
 *
 * Specification of the IO and IOT collector's blobs.
 * IOT Collector defines EXTENDEDIOTRACE to build the iot collector plugin.
 *
 */

/** Structure of the blob containing our parameters. */
struct io_parameters {

    /** Flags indicating if each IO function is to be traced. */
    uint8_t traced[1024];

};

#define MAXARGS 4

/** Event structure describing a single IO call. */
struct io_event {

    uint64_t start_time;  /**< Start time of the call. */
    uint64_t stop_time;   /**< End time of the call. */
    uint16_t stacktrace;  /**< Index of the stack trace. */

#ifdef EXTENDEDIOTRACE
    uint16_t pathindex;   /**< Index of the pathnames. */
#if 0
    uint64_t sysargs[MAXARGS]; /**< Actuall arguments */
    uint16_t syscallno;        /**< System call number. */
    uint16_t nsysargs;         /**< number of arg to the syscall */
    int retval;           /**< return value from IO call. */
#endif
#endif
};

/** Structure of the blob containing our performance data. */
struct io_data {
    uint64_t stacktraces<>;  /**< Stack traces. */
    io_event events<>;       /**< IO call events. */
#ifdef EXTENDEDIOTRACEXXX
    char pathnames<>;	     /**< IO pathnames. */
#endif
};



/** Structure of the blob containing io_start_tracing()'s arguments. */
struct io_start_tracing_args {
    int experiment;  /**< Identifier of experiment to contain the data. */
    int collector;   /**< Identifier of collector gathering data. */
    int thread;      /**< Identifier of gathered data's thread. */
};
