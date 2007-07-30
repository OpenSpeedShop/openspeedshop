/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Specification of the FPE collector's blobs.
 *
 */


/* see /usr/include/fenv.h and /usr/include/bits/fenv.h */
enum fpe_type {
   FPE_FE_INEXACT,
   FPE_FE_UNDERFLOW,
   FPE_FE_OVERFLOW,
   FPE_FE_DIVBYZERO,
   FPE_FE_UNNORMAL,
   FPE_FE_INVALID,
   FPE_FE_UNKNOWN
};

/** Structure of the blob containing our parameters. */
struct fpe_parameters {

    /** Flags indicating if each FPE function is to be traced. */
    uint8_t traced[8];

};

/** Event structure describing a single FPE. */
struct fpe_event {
    uint64_t start_time;  /**< Time that the FPE occurred. */
    fpe_type fpexception; /**< Floating Point Exception type. */
    uint16_t stacktrace;  /**< Index of the stack trace. */
};

/** Structure of the blob containing our performance data. */
struct fpe_data {
    uint64_t stacktraces<>;  /**< Stack traces. */
    fpe_event events<>;      /**< FPE call events. */
};



/** Structure of the blob containing fpe_start_tracing()'s arguments. */
struct fpe_start_tracing_args {
    fpe_type fpe_traced;
    int experiment;  /**< Identifier of experiment to contain the data. */
    int collector;   /**< Identifier of collector gathering data. */
};
