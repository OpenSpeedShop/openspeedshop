/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
 * Specification of a job description.
 *
 */



/**
 * Job description entry.
 *
 * A single host/pid pair specifying one process within the job.
 */
struct OpenSS_JobEntry {
    string host<>;  /**< Name of the host on which the process resides. */
    uint32_t pid;   /**< Identifier of the process. */
};



/**
 * Job description.
 *
 * Description of the processes in a job. Contains a variable-length array of
 * host/pid pairs, one for each process in the job. Also contains a flag that
 * indicates if this array of host/pid pairs completes the job description.
 */
struct OpenSS_Job {

    /** List of processes in this job. */
    OpenSS_JobEntry processes<>;

    /** Flag indicating if this list completes the job description. */
    uint8_t is_last;
    
};
