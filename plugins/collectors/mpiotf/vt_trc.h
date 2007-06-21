/**
 * VampirTrace
 * http://www.tu-dresden.de/zih/vampirtrace
 *
 * Copyright (c) 2005-2007, ZIH, TU Dresden, Federal Republic of Germany
 *
 * Copyright (c) 1998-2005, Forschungszentrum Juelich GmbH, Federal
 * Republic of Germany
 *
 * See the file COPYRIGHT in the package base directory for details
 **/

#ifndef _VT_TRC_H
#define _VT_TRC_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "vt_defs.h"
#include "vt_error.h"

EXTERN void vt_open               ( );

EXTERN void vt_close              ( );

/*
  The next two functions have to be called immediately after
  initializing and immediately before finalizing the communication
  middle-ware, e.g. after MPI_Init() and before MPI_Finalize().  
*/

EXTERN void vt_mpi_init           ( );
EXTERN void vt_mpi_finalize       ( );

/*
  For non-MPI and non-OpenMP programs, the next function has to be
  called after vt_open() to register a unique id (0..num-1) for the
  calling process and the number of processes
*/

EXTERN void vt_init_trc_id        ( int my_id, int num_procs );

/*
 *-----------------------------------------------------------------------------
 * Definition records
 *-----------------------------------------------------------------------------
 */

/*
  The records needed for describing the machine are generated
  automatically.

  The control of hardware performance counters to be monitored is not
  supported by this interface. A way of controlling performance
  counters will be provided in a later version.  
*/
 
EXTERN uint32_t vt_def_file        ( const char* fname );

EXTERN uint32_t vt_def_region      ( const char* rname,
				     uint32_t fid,
				     uint32_t begln,
				     uint32_t endln,
				     const char* rdesc,
				     uint8_t rtype );

EXTERN void vt_def_mpi_comm        ( uint32_t cid,
				     uint32_t grpc,
				     uint8_t grpv[] );

/*
 *-----------------------------------------------------------------------------
 * Event records
 *-----------------------------------------------------------------------------
 */

/* for MPI, Process ids of message sources and destinations are mapped onto
   location ids by the merging process. */

/* -- Region -- */

EXTERN void vt_enter(uint64_t* time, uint32_t rid);

EXTERN void vt_exit(uint64_t* time);

/* -- MPI-1 -- */

EXTERN void vt_mpi_send(uint64_t* time, uint32_t dpid, uint32_t cid,
       uint32_t tag, uint32_t sent);

EXTERN void vt_mpi_recv(uint64_t* time, uint32_t spid, uint32_t cid,
       uint32_t tag, uint32_t recvd);

EXTERN void vt_mpi_collexit(uint64_t* time, uint64_t* etime, uint32_t rid,
       uint32_t rpid, uint32_t cid, uint32_t sent, uint32_t recvd);

/* -- OpenMP -- */

EXTERN void vt_omp_fork(uint64_t* time);

EXTERN void vt_omp_join(uint64_t* time);

EXTERN void vt_omp_alock(uint64_t* time, uint32_t lkid);

EXTERN void vt_omp_rlock(uint64_t* time, uint32_t lkid);

EXTERN void vt_omp_collenter(uint64_t* time, uint32_t rid);

EXTERN void vt_omp_collexit(uint64_t* etime);

/* -- VampirTrace Internal -- */

EXTERN void vt_enter_user(uint64_t* time);

EXTERN void vt_exit_user(uint64_t* time);

EXTERN void vt_enter_tracing(uint64_t* time);

EXTERN void vt_exit_tracing(uint64_t* time);

int vt_open_called;
int vt_close_called;

#endif
