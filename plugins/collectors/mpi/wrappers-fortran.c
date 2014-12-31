/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2015 The Krell Institute. All Rights Reserved.
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
 * MPI FORTRAN function wrappers for the MPI tracing collector.
 * NOTE: The fortran wrappers call the C wrapper implementation
 * in the wrappers.c file (where the actual tracing is done).
 * This means that stacktraces from fortran applications will
 * have one extra frame through this file.
 *
 */

#include "RuntimeAPI.h"
#include "runtime.h"

#include <mpi.h>

#if defined (SGI_MPT)
#define MPI_Status_c2f(c,f) *(MPI_Status *)f=*(MPI_Status *)c
#define MPI_Status_f2c(f,c) *(MPI_Status *)c=*(MPI_Status *)f 
#endif

/* Is there a better way to do this? */
#ifndef MPI_STATUS_SIZE
#define MPI_STATUS_SIZE 5
#endif

/* This macro is inspired by the vampirtrace macro and
 * is specific to the needs of OpenSpeedShop. We need to
 * wrap fortran functions such that we can use the wrappers
 * with libmonitor.so AND libmonitor_wrap.a.
 */

#define OSS_WRAP_FORTRAN( upper_case, wrapper_function, \
	static_name, signature, params) \
  void wrapper_function##_ signature { wrapper_function params; } \
  void static_name##_ signature { wrapper_function params; } \
  void wrapper_function##__ signature { wrapper_function params; } \
  void static_name##__ signature { wrapper_function params; } \
  void upper_case signature { wrapper_function params; }


/*
 * MPI_Irecv
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_irecv
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_irecv
#endif
  (char* buf,
    MPI_Fint* count,
    MPI_Fint* datatype,
    MPI_Fint* source,
    MPI_Fint* tag,
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Irecv(buf, *count, MPI_Type_f2c(*datatype), *source, *tag,
                    MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_IRECV,mpi_irecv,__wrap_mpi_irecv,
   (char* buf,
    MPI_Fint* count,
    MPI_Fint *datatype, 
    MPI_Fint* source,
    MPI_Fint* tag,
    MPI_Fint *comm, 
    MPI_Fint *request,
    MPI_Fint *ierr),
    (buf, count, datatype, source, tag, comm, request, ierr))

/*
 * MPI_Recv
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_recv
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_recv
#endif
   (char* buf,
    MPI_Fint* count,
    MPI_Fint *datatype, 
    MPI_Fint* source,
    MPI_Fint* tag,
    MPI_Fint *comm, 
    MPI_Fint *status,
    MPI_Fint *ierr)
{
  MPI_Status c_status;
  *ierr = MPI_Recv(buf, *count, MPI_Type_f2c(*datatype), *source, *tag,
                   MPI_Comm_f2c(*comm), &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_RECV,mpi_recv,__wrap_mpi_recv,
   (char* buf,
    MPI_Fint* count,
    MPI_Fint *datatype, 
    MPI_Fint* source,
    MPI_Fint* tag,
    MPI_Fint *comm, 
    MPI_Fint *status,
    MPI_Fint *ierr),
    (buf, count, datatype, source, tag, comm, status, ierr))

/*
 * MPI_Recv_init
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_recv_init
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_recv_init
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Datatype* datatype, 
    MPI_Fint* source, 
    MPI_Fint* tag, 
    MPI_Comm* comm, 
    MPI_Request *request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Recv_init(buf, *count, MPI_Type_f2c(*datatype), *source, *tag, MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request); 
}
OSS_WRAP_FORTRAN(MPI_RECV_INIT,mpi_recv_init,__wrap_mpi_recv_init,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Datatype* datatype, 
    MPI_Fint* source, 
    MPI_Fint* tag, 
    MPI_Comm* comm, 
    MPI_Request *request,
    MPI_Fint* ierr),
    (buf, count, datatype, source, tag, comm, request, ierr))

  
/*
 * MPI_Iprobe
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_iprobe
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_iprobe
#endif
   (MPI_Fint* source, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint *flag, 
    MPI_Fint *status,
    MPI_Fint* ierr)
{
  MPI_Status c_status;
  *ierr = MPI_Iprobe(*source, *tag, MPI_Comm_f2c(*comm), flag, &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_IPROBE,mpi_iprobe,__wrap_mpi_iprobe,
   (MPI_Fint* source, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint *flag, 
    MPI_Fint *status,
    MPI_Fint* ierr),
    (source, tag, comm, flag, status, ierr))
  
/*
 * MPI_probe
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_probe
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_probe
#endif
   (MPI_Fint* source, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint *status,
    MPI_Fint* ierr)
{
  MPI_Status c_status;
  *ierr = MPI_Probe(*source, *tag, MPI_Comm_f2c(*comm), &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_PROBE,mpi_probe,__wrap_mpi_probe,
   (MPI_Fint* source, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint *status,
    MPI_Fint* ierr),
   (source, tag, comm, status, ierr))


/*
 * MPI_Isend
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_isend
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_isend
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest,
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Isend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                    MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_ISEND,mpi_isend,__wrap_mpi_isend,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))


/*
 * MPI_Bsend
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_bsend
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_bsend
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Comm* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Bsend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                    MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_BSEND,mpi_bsend,__wrap_mpi_bsend,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Comm* comm, 
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, ierr))
  

/*
 * MPI_Bsend_init
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_bsend_init
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_bsend_init
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Bsend_init(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                         MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_BSEND_INIT,mpi_bsend_init,__wrap_mpi_bsend_init,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))

  
/*
 * MPI_Ibsend
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_ibsend
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_ibsend
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint *request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Ibsend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                     MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_IBSEND,mpi_ibsend,__wrap_mpi_ibsend,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))
  

/*
 * MPI_Irsend
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_irsend    
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_irsend    
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint *request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Irsend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                     MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_IRSEND,mpi_irsend,__wrap_mpi_irsend,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))
    

/*
 * MPI_Issend
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_issend
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_issend
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Issend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                     MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_ISSEND,mpi_issend,__wrap_mpi_issend,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))
  
/*
 * MPI_Rsend
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_rsend
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_rsend
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint *ierr)
{
  *ierr = MPI_Rsend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                    MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_RSEND,mpi_rsend,__wrap_mpi_rsend,
   (char* buf,
    MPI_Fint* count,
    MPI_Fint* datatype,
    MPI_Fint* dest,
    MPI_Fint* tag,
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, ierr))
  
  
/*
 * MPI_Rsend_init
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_rsend_init
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_rsend_init
#endif
   (char* buf,
    MPI_Fint* count,
    MPI_Fint* datatype,
    MPI_Fint* dest,
    MPI_Fint* tag,
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Rsend_init(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                         MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_RSEND_INIT,mpi_rsend_init,__wrap_mpi_rsend_init,
   (char* buf,
    MPI_Fint* count,
    MPI_Fint* datatype,
    MPI_Fint* dest,
    MPI_Fint* tag,
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))



#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_send
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_send
#endif
   (char* buf,
    MPI_Fint* count,
    MPI_Fint* datatype,
    MPI_Fint* dest,
    MPI_Fint* tag,
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr =  MPI_Send(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
		    MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_SEND,mpi_send,__wrap_mpi_send,
   (char* buf,
    MPI_Fint* count,
    MPI_Fint* datatype,
    MPI_Fint* dest,
    MPI_Fint* tag,
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, ierr))

  

/*
 * MPI_Send_init
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_send_init
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_send_init
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Send_init(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                        MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_SEND_INIT,mpi_send_init,__wrap_mpi_send_init,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))

  
/*
 * MPI_Ssend
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_ssend
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_ssend
#endif
   (char* buf,
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Ssend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                    MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_SSEND,mpi_ssend,__wrap_mpi_ssend,
   (char* buf,
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, ierr))

  
/*
 * MPI_Ssend_init
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_ssend_init
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_ssend_init
#endif
   (char* buf,
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Ssend_init(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                         MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_SSEND_INIT,mpi_ssend_init,__wrap_mpi_ssend_init,
   (char* buf,
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))


  
/*
 * MPI_Waitall
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_waitall
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_waitall
#endif
   (MPI_Fint* count, 
    MPI_Fint array_of_requests[], 
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr)
{
  int i;
  MPI_Request* l_request = 0;
  MPI_Status* c_status = 0;

  if (*count > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*count));
    c_status = (MPI_Status*)malloc(sizeof(MPI_Status)*(*count));
    for (i=0; i<*count; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Waitall(*count, l_request, c_status);
  for (i=0; i<*count; i++) {
    array_of_requests[i] = MPI_Request_c2f(l_request[i]);
  }
  if (*ierr == MPI_SUCCESS) {
    for (i=0; i<*count; i++) {
      MPI_Status_c2f(&(c_status[i]), &(array_of_statuses[i][0]));
    }
  }
}
OSS_WRAP_FORTRAN(MPI_WAITALL,mpi_waitall,__wrap_mpi_waitall,
   (MPI_Fint* count, 
    MPI_Fint array_of_requests[], 
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr),
   (count, array_of_requests, array_of_statuses, ierr))


/*
 * MPI_Finalize
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_finalize
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_finalize
#endif
    (MPI_Fint *ierr )
{
    *ierr = MPI_Finalize();
}
OSS_WRAP_FORTRAN(MPI_FINALIZE,mpi_finalize,__wrap_mpi_finalize,
    (MPI_Fint *ierr ),
    (ierr))


/*
 * MPI_Waitsome
 
    What do I do with out and in count?
    
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_waitsome
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_waitsome
#endif
   (MPI_Fint* incount,
    MPI_Fint array_of_requests[],
    MPI_Fint* outcount,
    MPI_Fint array_of_indices[],
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr)
{
  int i, j, found;
  MPI_Request *l_request = 0;
  MPI_Status  *c_status = 0;

  if (*incount > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*incount));
    c_status = (MPI_Status*)malloc(sizeof(MPI_Status)*(*incount));
    for (i=0; i<*incount; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Waitsome(*incount, l_request, outcount, array_of_indices,
                       c_status);
  if (*ierr == MPI_SUCCESS) {
    for (i=0; i<*incount; i++) {
      if (i < *outcount) {
        if (array_of_indices[i] >= 0) {
          array_of_requests[array_of_indices[i]] =
            MPI_Request_c2f(l_request[array_of_indices[i]]);
        }
      } else {
        found = j = 0;
        while ( (!found) && (j<*outcount) ) {
          if (array_of_indices[j++] == i) found = 1;
        }
        if (!found) array_of_requests[i] = MPI_Request_c2f(l_request[i]);
      }
    }
    for (i=0; i<*outcount; i++) {
      MPI_Status_c2f(&c_status[i], &(array_of_statuses[i][0]));
      /* See the description of waitsome in the standard;
         the Fortran index ranges are from 1, not zero */
      if (array_of_indices[i] >= 0) array_of_indices[i]++;
    }
  }
}
OSS_WRAP_FORTRAN(MPI_WAITSOME,mpi_waitsome,__wrap_mpi_waitsome,
   (MPI_Fint* incount,
    MPI_Fint array_of_requests[],
    MPI_Fint* outcount,
    MPI_Fint array_of_indices[],
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr),
   (incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierr))


/*
 * MPI_Testsome
 
    What do I do with out and in count?
    
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_testsome
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_testsome
#endif
   (MPI_Fint* incount,
    MPI_Fint array_of_requests[],
    MPI_Fint *outcount,
    MPI_Fint array_of_indices[],
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr)
{
  int i, j, found;
  MPI_Request *l_request = 0;
  MPI_Status  *c_status = 0;

  if (*incount > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*incount));
    c_status = (MPI_Status*)malloc(sizeof(MPI_Status)*(*incount));
    for (i=0; i<*incount; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Testsome(*incount, l_request, outcount, array_of_indices,
                         c_status);
  if (*ierr == MPI_SUCCESS) {
    for (i=0; i<*incount; i++) {
      if (i < *outcount) {
        array_of_requests[array_of_indices[i]] =
          MPI_Request_c2f(l_request[array_of_indices[i]]);
      } else {
        found = j = 0;
        while ( (!found) && (j<*outcount) ) {
          if (array_of_indices[j++] == i) found = 1;
        }
        if (!found) array_of_requests[i] = MPI_Request_c2f(l_request[i]);
      }
    }
    for (i=0; i<*outcount; i++) {
      MPI_Status_c2f(&c_status[i], &(array_of_statuses[i][0]));
      /* See the description of testsome in the standard;
         the Fortran index ranges are from 1, not zero */
      if (array_of_indices[i] >= 0) array_of_indices[i]++;
    }
  }
}
OSS_WRAP_FORTRAN(MPI_TESTSOME,mpi_testsome,__wrap_mpi_testsome,
   (MPI_Fint* incount,
    MPI_Fint array_of_requests[],
    MPI_Fint *outcount,
    MPI_Fint array_of_indices[],
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr),
   (incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierr))



/*
 * MPI_Waitany
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_waitany
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_waitany
#endif
   (MPI_Fint* count,   
    MPI_Fint array_of_requests[],
    MPI_Fint* index, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{
  int i;
  MPI_Request *l_request = 0;
  MPI_Status c_status;

  if (*count > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*count));
    for (i=0; i<*count; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Waitany(*count, l_request, index, &c_status);
  if (*ierr == MPI_SUCCESS) {
    if (*index >= 0) {
      /* index may be MPI_UNDEFINED if all are null */
      array_of_requests[*index] = MPI_Request_c2f(l_request[*index]);

      /* See the description of waitany in the standard;
         the Fortran index ranges are from 1, not zero */
      (*index)++;
    }
    MPI_Status_c2f(&c_status, status);
  }
}
OSS_WRAP_FORTRAN(MPI_WAITANY,mpi_waitany,__wrap_mpi_waitany,
   (MPI_Fint* count,   
    MPI_Fint array_of_requests[],
    MPI_Fint* index, 
    MPI_Fint* status,
    MPI_Fint* ierr),
   (count, array_of_requests, index, status, ierr))


/*
 * MPI_Unpack
 
    Which size do I use, insize or outsize?
    I'm going to use outsize.
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_unpack
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_unpack
#endif
   (char* inbuf, 
    MPI_Fint* insize, 
    MPI_Fint *position, 
    char *outbuf, 
    MPI_Fint* outcount, 
    MPI_Fint* datatype, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Unpack(inbuf,*insize,*position,outbuf,*outcount,
			MPI_Type_f2c(*datatype), MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_UNPACK,mpi_unpack,__wrap_mpi_unpack,
   (char* inbuf, 
    MPI_Fint* insize, 
    MPI_Fint *position, 
    char *outbuf, 
    MPI_Fint* outcount, 
    MPI_Fint* datatype, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (inbuf, insize, position, outbuf, outcount, datatype, comm, ierr))


/*
 * MPI_Wait
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_wait
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_wait
#endif
   (MPI_Fint *request, 
    MPI_Fint *status,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  MPI_Status c_status;
  l_request = MPI_Request_f2c(*request);
  *ierr = MPI_Wait(&l_request, &c_status);
  *request = MPI_Request_c2f(l_request);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_WAIT,mpi_wait,__wrap_mpi_wait,
   (MPI_Fint *request,
    MPI_Fint *status,
    MPI_Fint* ierr),
   (request, status, ierr))


/*
 * MPI_Testany
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_testany
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_testany
#endif
   (MPI_Fint* count,
    MPI_Fint *array_of_requests,
    MPI_Fint *index,
    MPI_Fint *flag,
    MPI_Fint *status,
    MPI_Fint* ierr)
{
  int i;
  MPI_Request *l_request = 0;
  MPI_Status c_status;

  if (*count > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*count));
    for (i=0; i<*count; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Testany(*count, l_request, index, flag, &c_status);
  if (*ierr == MPI_SUCCESS) {
    if (*flag && *index >= 0) {
      /* index may be MPI_UNDEFINED if all are null */
      array_of_requests[*index] = MPI_Request_c2f(l_request[*index]);

      /* See the description of waitany in the standard;
         the Fortran index ranges are from 1, not zero */
      (*index)++;
    }
    MPI_Status_c2f(&c_status, status);
  }
}
OSS_WRAP_FORTRAN(MPI_TESTANY,mpi_testany,__wrap_mpi_testany,
   (MPI_Fint* count,
    MPI_Fint *array_of_requests,
    MPI_Fint *index,
    MPI_Fint *flag,
    MPI_Fint *status,
    MPI_Fint* ierr),
   (count, array_of_requests, index, flag, status,ierr))



/*
 * MPI_Testall
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_testall
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_testall
#endif
   (MPI_Fint* count,
    MPI_Fint array_of_requests[],
    MPI_Fint *flag, 
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr)
{
  int i;
  MPI_Request *l_request = 0;
  MPI_Status *c_status = 0;

  if (*count > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*count));
    c_status = (MPI_Status*)malloc(sizeof(MPI_Status)*(*count));
    for (i=0; i<*count; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Testall(*count, l_request, flag, c_status);
  for (i=0; i<*count; i++) {
    array_of_requests[i] = MPI_Request_c2f(l_request[i]);
  }
  if (*ierr == MPI_SUCCESS && *flag) {
    for (i=0; i<*count; i++) {
      MPI_Status_c2f(&(c_status[i]), &(array_of_statuses[i][0]));
    }
  }
}
OSS_WRAP_FORTRAN(MPI_TESTALL,mpi_testall,__wrap_mpi_testall,
   (MPI_Fint* count,
    MPI_Fint array_of_requests[],
    MPI_Fint *flag, 
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr),
   (count, array_of_requests, flag, array_of_statuses, ierr))


/*
 * MPI_Test
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_test
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_test
#endif
   (MPI_Fint *request, 
    MPI_Fint *flag, 
    MPI_Fint *status,
    MPI_Fint* ierr)
{
  MPI_Status c_status;
  MPI_Request l_request = MPI_Request_f2c(*request);
  *ierr = MPI_Test(&l_request, flag, &c_status);
  if (*ierr != MPI_SUCCESS) return;
  *request = MPI_Request_c2f(l_request);
  if (flag) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_TEST,mpi_test,__wrap_mpi_test,
   (MPI_Fint *request, 
    MPI_Fint *flag, 
    MPI_Fint *status,
    MPI_Fint* ierr),
   (request, flag, status, ierr))


/*
 * MPI_Scan
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_scan
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_scan
#endif
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Scan(sendbuf, recvbuf, *count, MPI_Type_f2c(*datatype),
                   MPI_Op_f2c(*op), MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_SCAN,mpi_scan,__wrap_mpi_scan,
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, recvbuf, count, datatype, op, comm, ierr))


/*
 * MPI_Request_free
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_request_free
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_request_free
#endif
   (MPI_Fint *request, MPI_Fint* ierr)
{
  MPI_Request l_request = MPI_Request_f2c(*request);
  *ierr = MPI_Request_free(&l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_REQUEST_FREE,mpi_request_free,__wrap_mpi_request_free,
   (MPI_Fint *request, MPI_Fint* ierr),
   (request, ierr))


/*
 * MPI_Reduce_scatter
 
    This is questionable with recvcounts.
    
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_reduce_scatter
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_reduce_scatter
#endif
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint *recvcounts, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts,
                             MPI_Type_f2c(*datatype), MPI_Op_f2c(*op),
			     MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_REDUCE_SCATTER,mpi_reduce_scatter,__wrap_mpi_reduce_scatter,
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint *recvcounts, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, recvbuf, recvcounts, datatype, op, comm, ierr))

/*
 * MPI_Reduce
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_reduce
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_reduce
#endif
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Reduce(sendbuf, recvbuf, *count, MPI_Type_f2c(*datatype),
                     MPI_Op_f2c(*op), *root, MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_REDUCE,mpi_reduce,__wrap_mpi_reduce,
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, recvbuf, count, datatype, op, root, comm, ierr))


/*
 * MPI_Pack
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_pack
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_pack
#endif
   (char* inbuf, 
    MPI_Fint* incount, 
    MPI_Fint* datatype, 
    char *outbuf, 
    MPI_Fint* outsize, 
    MPI_Fint *position, 
    MPI_Comm* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Pack(inbuf,*incount, MPI_Type_f2c(*datatype),outbuf,
		   *outsize,*position, MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_PACK,mpi_pack,__wrap_mpi_pack,
   (char* inbuf, 
    MPI_Fint* incount, 
    MPI_Fint* datatype, 
    char *outbuf, 
    MPI_Fint* outsize, 
    MPI_Fint *position, 
    MPI_Comm* comm,
    MPI_Fint* ierr),
   (inbuf, incount, datatype, outbuf, outsize, position, comm, ierr))


/* MPI_Init */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_init
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_init
#endif
    (MPI_Fint *ierr )
{
    *ierr = MPI_Init(0, (char***)0);
}
OSS_WRAP_FORTRAN(MPI_INIT,mpi_init,__wrap_mpi_init,
    (MPI_Fint *ierr ),
    (ierr))


/*
 * MPI_Get_count
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int mpi_get_count
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_mpi_get_count
#endif
   (MPI_Fint *status,
    MPI_Fint *datatype,
    MPI_Fint *count,
    MPI_Fint *ierr)
{
  MPI_Status c_status;
  *ierr = MPI_Get_count(&c_status,MPI_Type_f2c(*datatype),*count);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_GET_COUNT,mpi_get_count,__wrap_mpi_get_count,
   (MPI_Fint *status,
    MPI_Fint *datatype,
    MPI_Fint *count,
    MPI_Fint *ierr),
   (status, datatype, count, ierr))


/*
 * MPI_Gatherv
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int mpi_gatherv
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_mpi_gatherv
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint *recvcounts, 
    MPI_Fint *displs, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Gatherv(sendbuf, *sendcount, MPI_Type_f2c(*sendtype),
                      recvbuf, recvcounts, displs, MPI_Type_f2c(*recvtype),
                      *root, MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_GATHERV,mpi_gatherv,__wrap_mpi_gatherv,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint *recvcounts, 
    MPI_Fint *displs, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, ierr))


/*
 * MPI_Gather
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_gather
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_gather
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Gather(sendbuf, *sendcount, MPI_Type_f2c(*sendtype), recvbuf,
                     *recvcount, MPI_Type_f2c(*recvtype), *root,
                     MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_GATHER,mpi_gather,__wrap_mpi_gather,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierr))

/*
 * MPI_Cancel
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int mpi_cancel
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_mpi_cancel
#endif
   (MPI_Fint *request, MPI_Fint* ierr)
{
  MPI_Request l_request = MPI_Request_f2c(*request);
  *ierr = MPI_Cancel(&l_request);
}
OSS_WRAP_FORTRAN(MPI_CANCEL,mpi_cancel,__wrap_mpi_cancel,
   (MPI_Fint *request, MPI_Fint* ierr),
   (request,ierr))

/*
 * MPI_Bcast
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_bcast
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_bcast
#endif
   (char* buffer, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Bcast(buffer, *count, MPI_Type_f2c(*datatype), *root,
                    MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_BCAST,mpi_bcast,__wrap_mpi_bcast,
   (char* buffer, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (buffer, count, datatype, root, comm, ierr))

/*
 * MPI_Barrier
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_barrier
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_barrier
#endif
    (MPI_Fint* comm, MPI_Fint* ierr)
{
  *ierr = MPI_Barrier(MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_BARRIER,mpi_barrier,__wrap_mpi_barrier,
    (MPI_Fint* comm, MPI_Fint* ierr),
    (comm, ierr))

/*
 * MPI_Alltoallv
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_alltoallv
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_alltoallv
#endif
   (char* sendbuf, 
    MPI_Fint *sendcounts, 
    MPI_Fint *sdispls, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* *recvcounts, 
    MPI_Fint* *rdispls, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Alltoallv(sendbuf, sendcounts, sdispls, MPI_Type_f2c(*sendtype),
                        recvbuf, recvcounts, rdispls, MPI_Type_f2c(*recvtype),
                        MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_ALLTOALLV,mpi_alltoallv,__wrap_mpi_alltoallv,
   (char* sendbuf, 
    MPI_Fint *sendcounts, 
    MPI_Fint *sdispls, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* *recvcounts, 
    MPI_Fint* *rdispls, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype,comm, ierr))


/*
 * MPI_Alltoall
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_alltoall
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_alltoall
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Alltoall(sendbuf, *sendcount, MPI_Type_f2c(*sendtype), recvbuf,
                       *recvcount, MPI_Type_f2c(*recvtype),
                       MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_ALLTOALL,mpi_alltoall,__wrap_mpi_alltoall,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierr))


/*
 * MPI_Allreduce
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_allreduce
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_allreduce
#endif
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Allreduce(sendbuf, recvbuf, *count, MPI_Type_f2c(*datatype),
                        MPI_Op_f2c(*op), MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_ALLREDUCE,mpi_allreduce,__wrap_mpi_allreduce,
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, recvbuf, count, datatype, op, comm, ierr))


/*
 * MPI_Allgatherv
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_allgatherv
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_allgatherv
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint *recvcounts, 
    MPI_Fint *displs, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Allgatherv(sendbuf, *sendcount, MPI_Type_f2c(*sendtype),
                         recvbuf, recvcounts, displs, MPI_Type_f2c(*recvtype),
                         MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_ALLGATHERV,mpi_allgatherv,__wrap_mpi_allgatherv,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint *recvcounts, 
    MPI_Fint *displs, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, ierr))


/*
 * MPI_Allgather
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_allgather
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_allgather
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Allgather(sendbuf, *sendcount, MPI_Type_f2c(*sendtype),
                        recvbuf, *recvcount, MPI_Type_f2c(*recvtype),
                        MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_ALLGATHER,mpi_allgather,__wrap_mpi_allgather,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierr))


/*
 * MPI_Scatter
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_scatter
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_scatter
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Scatter(sendbuf, *sendcount, MPI_Type_f2c(*sendtype),
                      recvbuf, *recvcount, MPI_Type_f2c(*recvtype),
                      *root, MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_SCATTER,mpi_scatter,__wrap_mpi_scatter,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierr))


/*
 * MPI_Scatterv
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_scatterv
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_scatterv
#endif
   (char* sendbuf, 
    MPI_Fint* sendcounts, 
    MPI_Fint* displs,
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Scatterv(sendbuf, sendcounts, displs, MPI_Type_f2c(*sendtype),
                       recvbuf, *recvcount, MPI_Type_f2c(*recvtype),
                       *root, MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_SCATTERV,mpi_scatterv,__wrap_mpi_scatterv,
   (char* sendbuf, 
    MPI_Fint* sendcounts, 
    MPI_Fint* displs,
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, ierr))


/*
 * MPI_Sendrecv
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_sendrecv
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_sendrecv
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    MPI_Fint* dest, 
    MPI_Fint* sendtag, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* source, 
    MPI_Fint* recvtag, 
    MPI_Fint* comm, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{
  MPI_Status c_status;

  *ierr = MPI_Sendrecv(sendbuf, *sendcount, MPI_Type_f2c(*sendtype), *dest,
                       *sendtag, recvbuf, *recvcount, MPI_Type_f2c(*recvtype),
                       *source, *recvtag, MPI_Comm_f2c(*comm), &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_SENDRECV,mpi_sendrecv,__wrap_mpi_sendrecv,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    MPI_Fint* dest, 
    MPI_Fint* sendtag, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* source, 
    MPI_Fint* recvtag, 
    MPI_Fint* comm, 
    MPI_Fint* status,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status, ierr))


/*
 * MPI_Sendrecv_replace
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_sendrecv_replace
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_sendrecv_replace
#endif
    (char* buf, 
     MPI_Fint* count, 
     MPI_Fint* datatype, 
     MPI_Fint* dest, 
     MPI_Fint* sendtag, 
     MPI_Fint* source, 
     MPI_Fint* recvtag, 
     MPI_Fint* comm, 
     MPI_Fint* status,
     MPI_Fint* ierr)
{
  MPI_Status c_status;
  *ierr = MPI_Sendrecv_replace(buf, *count, MPI_Type_f2c(*datatype), *dest,
                               *sendtag, *source, *recvtag,
                               MPI_Comm_f2c(*comm), &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_SENDRECV_REPLACE,mpi_sendrecv_replace,__wrap_mpi_sendrecv_replace,
    (char* buf, 
     MPI_Fint* count, 
     MPI_Fint* datatype, 
     MPI_Fint* dest, 
     MPI_Fint* sendtag, 
     MPI_Fint* source, 
     MPI_Fint* recvtag, 
     MPI_Fint* comm, 
     MPI_Fint* status,
     MPI_Fint* ierr),
    (buf, count, datatype, dest, sendtag, source, recvtag, comm, status, ierr))

/*
 *-----------------------------------------------------------------------------
 *
 * Cartesian Toplogy functions
 *
 *-----------------------------------------------------------------------------
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_cart_create
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_cart_create
#endif
		     (MPI_Comm comm_old,
                     int ndims,
                     int* dims,
                     int* periodv,
                     int reorder,
                     MPI_Comm* comm_cart)
{
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_cart_sub 
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_cart_sub 
#endif
		   (MPI_Comm comm,
                   int *rem_dims,
                   MPI_Comm *newcomm)
{
}



#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_graph_create
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_graph_create
#endif
		     (MPI_Comm comm_old,
                      int nnodes,
                      int* index,
                      int* edges,
                      int reorder,
                      MPI_Comm* comm_graph)
{
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_intercomm_create
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_intercomm_create
#endif
     (MPI_Fint* local_comm,
      MPI_Fint* local_leader,
      MPI_Fint* peer_comm,
      MPI_Fint* remote_leader,
      MPI_Fint* tag,
      MPI_Fint *newintercomm,
      MPI_Fint* ierr)

{
  MPI_Comm l_newintercomm;
  *ierr = MPI_Intercomm_create(MPI_Comm_f2c(*local_comm), *local_leader,
                               MPI_Comm_f2c(*peer_comm), *remote_leader,
				*tag, &l_newintercomm);
  if (*ierr == MPI_SUCCESS) *newintercomm = MPI_Comm_c2f(l_newintercomm);
}
OSS_WRAP_FORTRAN(MPI_INTERCOMM_CREATE,mpi_intercomm_create,__wrap_mpi_intercomm_create,
     (MPI_Fint* local_comm,
      MPI_Fint* local_leader,
      MPI_Fint* peer_comm,
      MPI_Fint* remote_leader,
      MPI_Fint* tag,
      MPI_Fint *newintercomm,
      MPI_Fint* ierr),
     (local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm, ierr))

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_intercomm_merge
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_intercomm_merge
#endif
    (MPI_Fint* intercomm, MPI_Fint* high, MPI_Fint *newcomm, MPI_Fint* ierr)
{
  MPI_Comm l_newcomm;
  *ierr = MPI_Intercomm_merge(MPI_Comm_f2c(*intercomm), *high,
                              &l_newcomm);
  if (*ierr == MPI_SUCCESS) *newcomm = MPI_Comm_c2f(l_newcomm);
}
OSS_WRAP_FORTRAN(MPI_INTERCOMM_MERGE,mpi_intercomm_merge,__wrap_mpi_intercomm_merge,
    (MPI_Fint* intercomm, MPI_Fint* high, MPI_Fint *newcomm, MPI_Fint* ierr),
    (intercomm, high, newcomm, ierr))


/* ------- Destructors ------- */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_comm_free
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_comm_free
#endif
    (MPI_Fint* comm ,MPI_Fint* ierr)
{
  MPI_Comm l_comm = MPI_Comm_f2c(*comm);
  *ierr = MPI_Comm_free(&l_comm);
  if (*ierr == MPI_SUCCESS) *comm = MPI_Comm_c2f(l_comm);
}
OSS_WRAP_FORTRAN(MPI_COMM_FREE,mpi_comm_free,__wrap_mpi_comm_free,
    (MPI_Fint* comm ,MPI_Fint* ierr),
    (comm, ierr))


/*
 *-----------------------------------------------------------------------------
 *
 * Communicator management
 *
 *-----------------------------------------------------------------------------
 */

/* ------- Constructors ------- */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_comm_dup
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_comm_dup
#endif
    (MPI_Fint* comm, MPI_Fint* newcomm, MPI_Fint* ierr)
{
  MPI_Comm l_newcomm;
  *ierr = MPI_Comm_dup(MPI_Comm_f2c(*comm), &l_newcomm);
  if (*ierr == MPI_SUCCESS) *newcomm = MPI_Comm_c2f(l_newcomm);
}
OSS_WRAP_FORTRAN(MPI_COMM_DUP,mpi_comm_dup,__wrap_mpi_comm_dup,
    (MPI_Fint* comm, MPI_Fint* newcomm, MPI_Fint* ierr),
    (comm, newcomm, ierr))

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_comm_create
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_comm_create
#endif
    (MPI_Fint* comm, MPI_Fint* group, MPI_Fint* newcomm, MPI_Fint* ierr)
{
  MPI_Comm l_newcomm;
  *ierr = MPI_Comm_create(MPI_Comm_f2c(*comm), MPI_Group_f2c(*group),
                          &l_newcomm);
  if (*ierr == MPI_SUCCESS) *newcomm = MPI_Comm_c2f(l_newcomm);
}
OSS_WRAP_FORTRAN(MPI_COMM_CREATE,mpi_comm_create,__wrap_mpi_comm_create,
    (MPI_Fint* comm, MPI_Fint* group, MPI_Fint* newcomm, MPI_Fint* ierr),
    (comm, group, newcomm, ierr))

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_comm_split
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_comm_split
#endif
    (MPI_Fint* comm, MPI_Fint* color, MPI_Fint* key,
     MPI_Fint* newcomm ,MPI_Fint* ierr )
{
  MPI_Comm l_newcomm;
  *ierr = MPI_Comm_split(MPI_Comm_f2c(*comm), *color, *key, &l_newcomm);
  if (*ierr == MPI_SUCCESS) *newcomm = MPI_Comm_c2f(l_newcomm);
}
OSS_WRAP_FORTRAN(MPI_COMM_SPLIT,mpi_comm_split,__wrap_mpi_comm_split,
    (MPI_Fint* comm, MPI_Fint* color, MPI_Fint* key,
     MPI_Fint* newcomm, MPI_Fint* ierr ),
    (comm, color, key, newcomm, ierr ))


/* -- MPI_Start -- */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_start
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_start
#endif
    ( MPI_Fint* request, MPI_Fint* ierr)
{
  MPI_Request l_request = MPI_Request_f2c(*request);
  *ierr = MPI_Start(&l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_START,mpi_start,__wrap_mpi_start,
    ( MPI_Fint* request, MPI_Fint* ierr),
    (request, ierr))

/* -- MPI_Startall -- */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_startall
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_startall
#endif
    (MPI_Fint* count,
     MPI_Fint array_of_requests[],
     MPI_Fint* ierr)
{
  int i;
  MPI_Request *l_request = 0;

  if (*count > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*count));
    for (i=0; i<*count; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Startall(*count, l_request);
  if (*ierr == MPI_SUCCESS) {
    for (i=0; i<*count; i++) {
      array_of_requests[i] = MPI_Request_c2f(l_request[i]);
    }
  }
}
OSS_WRAP_FORTRAN(MPI_STARTALL,mpi_startall,__wrap_mpi_startall,
    (MPI_Fint* count,
     MPI_Fint array_of_requests[],
     MPI_Fint* ierr),
    (count, array_of_requests, ierr))


/* this needs to be commented out (#if 0) for kestral to work */
#if 1
/*
 * MPI_File_open
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_open
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_open
#endif
  ( MPI_Fint* comm,
    char* filename,
    MPI_Fint* amode,
    MPI_Fint* info,
    MPI_Fint* mfile,
    MPI_Fint* ierr)
{
#if 1
  MPI_File local_mfile;
  *ierr = MPI_File_open(MPI_Comm_f2c(*comm), filename, *amode, MPI_Info_f2c(*info), &local_mfile);
  *mfile = MPI_File_c2f(local_mfile);
#else
  *ierr = MPI_File_open(MPI_Comm_f2c(*comm), filename, *amode, MPI_Info_f2c(*info), MPI_Info_c2f(*mfile));
#endif
}
OSS_WRAP_FORTRAN(MPI_FILE_OPEN,mpi_file_open,__wrap_mpi_file_open,
  ( MPI_Fint* comm,
    char* filename,
    MPI_Fint* amode,
    MPI_Fint* info,
    MPI_Fint* mfile,
    MPI_Fint *ierr),
    (comm, filename, amode, info, mfile, ierr))

#endif

/*
 * MPI_File_close
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_close
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_close
#endif
  ( MPI_Fint* mfile,
    MPI_Fint* ierr)
{

#if 1
  MPI_File local_mfile;
  local_mfile = MPI_File_f2c(*mfile);

  *ierr = MPI_File_close( &local_mfile);
  *mfile = MPI_File_c2f(local_mfile);
#else
  *ierr = MPI_File_close(*mfile);
#endif

}
OSS_WRAP_FORTRAN(MPI_FILE_CLOSE,mpi_file_close,__wrap_mpi_file_close,
  ( MPI_Fint* mfile,
    MPI_Fint *ierr),
    (mfile, ierr))


/*
 * MPI_File_delete
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_delete
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_delete
#endif
  ( char* filename,
    MPI_Fint* info,
    MPI_Fint* ierr)
{

  *ierr = MPI_File_delete( filename, MPI_Info_f2c(*info));

}
OSS_WRAP_FORTRAN(MPI_FILE_DELETE,mpi_file_delete,__wrap_mpi_file_delete,
  ( char* filename,
    MPI_Fint *info,
    MPI_Fint *ierr),
    (filename, info, ierr))


/*
 * MPI_File_set_size
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_set_size
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_set_size
#endif
  ( MPI_Fint* mfile,
    MPI_Offset* size,
    MPI_Fint* ierr)
{

  *ierr = MPI_File_set_size( MPI_File_f2c(*mfile), *size);

}
OSS_WRAP_FORTRAN(MPI_FILE_SET_SIZE,mpi_file_set_size,__wrap_mpi_file_set_size,
  ( MPI_Fint* mfile,
    MPI_Offset* size,
    MPI_Fint *ierr),
    (mfile, size, ierr))

/*
 * MPI_File_get_size
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_get_size
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_get_size
#endif
  ( MPI_Fint* mfile,
    MPI_Offset* size,
    MPI_Fint* ierr)
{

  *ierr = MPI_File_get_size( MPI_File_f2c(*mfile), size);

}
OSS_WRAP_FORTRAN(MPI_FILE_GET_SIZE,mpi_file_get_size,__wrap_mpi_file_get_size,
  ( MPI_Fint* mfile,
    MPI_Offset* size,
    MPI_Fint *ierr),
    (mfile, size, ierr))

/*
 * MPI_File_get_group
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_get_group
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_get_group
#endif
  ( MPI_Fint* mfile,
    MPI_Group* group,
    MPI_Fint* ierr)
{


  MPI_Group local_group;
  *ierr = MPI_File_get_group( MPI_File_f2c(*mfile), &local_group);
  *group = MPI_Group_c2f(local_group);


}
OSS_WRAP_FORTRAN(MPI_FILE_GET_GROUP,mpi_file_get_group,__wrap_mpi_file_get_group,
  ( MPI_Fint* mfile,
    MPI_Group* group,
    MPI_Fint *ierr),
    (mfile, group, ierr))


/*
 * MPI_File_get_info
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_get_info
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_get_info
#endif
  ( MPI_Fint* mfile,
    MPI_Fint* info,
    MPI_Fint* ierr)
{

  MPI_Info local_info;
  *ierr = MPI_File_get_info( MPI_File_f2c(*mfile), &local_info);
  *info = MPI_Info_c2f(local_info);


}
OSS_WRAP_FORTRAN(MPI_FILE_GET_INFO,mpi_file_get_info,__wrap_mpi_file_get_info,
  ( MPI_Fint* mfile,
    MPI_Fint* info,
    MPI_Fint *ierr),
    (mfile, info, ierr))


/*
 * MPI_File_get_view
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_get_view
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_get_view
#endif
  ( MPI_Fint* mfile,
    MPI_Offset* offset,
    MPI_Fint* etype,
    MPI_Fint* filetype,
    char* datarep,
    MPI_Fint* ierr)
{


  MPI_Datatype local_etype;
  MPI_Datatype local_filetype;
  *ierr = MPI_File_get_view( MPI_File_f2c(*mfile), offset, &local_etype, &local_filetype, datarep);
  *filetype = MPI_Type_c2f(local_filetype);
  *etype = MPI_Type_c2f(local_etype);


}
OSS_WRAP_FORTRAN(MPI_FILE_GET_VIEW,mpi_file_get_view,__wrap_mpi_file_get_view,
  ( MPI_Fint* mfile,
    MPI_Offset* offset,
    MPI_Fint* etype,
    MPI_Fint* filetype,
    char* datarep,
    MPI_Fint *ierr),
    (mfile, offset, etype, filetype, datarep, ierr))


/*
 * MPI_File_set_view
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_set_view
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_set_view
#endif
  ( MPI_Fint* mfile,
    MPI_Offset offset,
    MPI_Fint* etype,
    MPI_Fint* filetype,
    char* datarep,
    MPI_Fint* info,
    MPI_Fint* ierr)
{

  *ierr = MPI_File_set_view( MPI_File_f2c(*mfile), offset, MPI_Type_f2c(*etype), MPI_Type_f2c(*filetype), datarep, MPI_Info_f2c(*info));


}
OSS_WRAP_FORTRAN(MPI_FILE_SET_VIEW,mpi_file_set_view,__wrap_mpi_file_set_view,
  ( MPI_Fint* mfile,
    MPI_Offset* offset,
    MPI_Fint* etype,
    MPI_Fint* filetype,
    char* datarep,
    MPI_Fint* info,
    MPI_Fint *ierr),
    (mfile, offset, etype, filetype, datarep, info, ierr))


/*
 * MPI_File_get_amode
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_get_amode
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_get_amode
#endif
  ( MPI_Fint* mfile,
    MPI_Fint* amode,
    MPI_Fint* ierr)
{

  *ierr = MPI_File_get_amode( MPI_File_f2c(*mfile), amode);

}
OSS_WRAP_FORTRAN(MPI_FILE_GET_AMODE,mpi_file_get_amode,__wrap_mpi_file_get_amode,
  ( MPI_Fint* mfile,
    MPI_Fint* amode,
    MPI_Fint *ierr),
    (mfile, amode, ierr))



/*
 * MPI_File_set_info
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_set_info
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_set_info
#endif
  ( MPI_Fint* mfile,
    MPI_Fint* info,
    MPI_Fint* ierr)
{

  *ierr = MPI_File_set_info( MPI_File_f2c(*mfile), MPI_Info_f2c(*info));


}
OSS_WRAP_FORTRAN(MPI_FILE_SET_INFO,mpi_file_set_info,__wrap_mpi_file_set_info,
  ( MPI_Fint* mfile,
    MPI_Fint* info,
    MPI_Fint *ierr),
    (mfile, info, ierr))


/*
 * MPI_File_write
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_write
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_write
#endif
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{


  MPI_Status local_status;
  *ierr = MPI_File_write( MPI_File_f2c(*mfile), buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);


}
OSS_WRAP_FORTRAN(MPI_FILE_WRITE,mpi_file_write,__wrap_mpi_file_write,
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, buf, count, datatype, status, ierr))


/*
 * MPI_File_write_at
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_write_at
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_write_at
#endif
  ( MPI_Fint* mfile,
    MPI_Offset* offset, 
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_write_at( MPI_File_f2c(*mfile), *offset, buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);


}
OSS_WRAP_FORTRAN(MPI_FILE_WRITE_AT,mpi_file_write_at,__wrap_mpi_file_write_at,
  ( MPI_Fint* mfile,
    MPI_Offset* offset, 
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, offset, buf, count, datatype, status, ierr))


/*
 * MPI_File_write_at_all
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_write_at_all
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_write_at_all
#endif
  ( MPI_Fint* mfile,
    MPI_Offset* offset, 
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_write_at_all( MPI_File_f2c(*mfile), *offset, buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);


}
OSS_WRAP_FORTRAN(MPI_FILE_WRITE_AT_ALL,mpi_file_write_at_all,__wrap_mpi_file_write_at_all,
  ( MPI_Fint* mfile,
    MPI_Offset* offset, 
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, offset, buf, count, datatype, status, ierr))


/*
 * MPI_File_write_ordered
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_write_ordered
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_write_ordered
#endif
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_write_ordered( MPI_File_f2c(*mfile), buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);


}
OSS_WRAP_FORTRAN(MPI_FILE_WRITE_ORDERED,mpi_file_write_ordered,__wrap_mpi_file_write_ordered,
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, buf, count, datatype, status, ierr))



/*
 * MPI_File_write_shared
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_write_shared
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_write_shared
#endif
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_write_shared( MPI_File_f2c(*mfile), buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);

}
OSS_WRAP_FORTRAN(MPI_FILE_WRITE_SHARED,mpi_file_write_shared,__wrap_mpi_file_write_shared,
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, buf, count, datatype, status, ierr))


/*
 * MPI_File_write_all
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_write_all
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_write_all
#endif
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_write_all( MPI_File_f2c(*mfile), buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);

}
OSS_WRAP_FORTRAN(MPI_FILE_WRITE_ALL,mpi_file_write_all,__wrap_mpi_file_write_all,
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, buf, count, datatype, status, ierr))


/*
 * MPI_File_read
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_read
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_read
#endif
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_read( MPI_File_f2c(*mfile), buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);

}
OSS_WRAP_FORTRAN(MPI_FILE_READ,mpi_file_read,__wrap_mpi_file_read,
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, buf, count, datatype, status, ierr))


/*
 * MPI_File_read_shared
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_read_shared
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_read_shared
#endif
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_read_shared( MPI_File_f2c(*mfile), buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);

}
OSS_WRAP_FORTRAN(MPI_FILE_READ_SHARED,mpi_file_read_shared,__wrap_mpi_file_read_shared,
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, buf, count, datatype, status, ierr))



/*
 * MPI_File_read_ordered
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_read_ordered
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_read_ordered
#endif
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_read_ordered( MPI_File_f2c(*mfile), buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);

}
OSS_WRAP_FORTRAN(MPI_FILE_READ_ORDERED,mpi_file_read_ordered,__wrap_mpi_file_read_ordered,
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, buf, count, datatype, status, ierr))




/*
 * MPI_File_read_all
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_read_all
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_read_all
#endif
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_read_all( MPI_File_f2c(*mfile), buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);

}
OSS_WRAP_FORTRAN(MPI_FILE_READ_ALL,mpi_file_read_all,__wrap_mpi_file_read_all,
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, buf, count, datatype, status, ierr))

/*
 * MPI_File_read_at
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_read_at
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_read_at
#endif
  ( MPI_Fint* mfile,
    MPI_Offset* offset, 
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_read_at( MPI_File_f2c(*mfile), *offset, buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);

}
OSS_WRAP_FORTRAN(MPI_FILE_READ_AT,mpi_file_read_at,__wrap_mpi_file_read_at,
  ( MPI_Fint* mfile,
    MPI_Offset* offset, 
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, offset, buf, count, datatype, status, ierr))



/*
 * MPI_File_read_at_all
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_read_at_all
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_read_at_all
#endif
  ( MPI_Fint* mfile,
    MPI_Offset* offset, 
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_read_at_all( MPI_File_f2c(*mfile), *offset, buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);

}
OSS_WRAP_FORTRAN(MPI_FILE_READ_AT_ALL,mpi_file_read_at_all,__wrap_mpi_file_read_at_all,
  ( MPI_Fint* mfile,
    MPI_Offset* offset, 
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, offset, buf, count, datatype, status, ierr))

/*
 * MPI_File_iread_at
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_iread_at
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_iread_at
#endif
  ( MPI_Fint* mfile,
    MPI_Offset* offset, 
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_iread_at( MPI_File_f2c(*mfile), *offset, buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);

}
OSS_WRAP_FORTRAN(MPI_FILE_IREAD_AT,mpi_file_iread_at,__wrap_mpi_file_iread_at,
  ( MPI_Fint* mfile,
    MPI_Offset* offset, 
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, offset, buf, count, datatype, status, ierr))


/*
 * MPI_File_iread_shared
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_iread_shared
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_iread_shared
#endif
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_iread_shared( MPI_File_f2c(*mfile), buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);

}
OSS_WRAP_FORTRAN(MPI_FILE_IREAD_SHARED,mpi_file_iread_shared,__wrap_mpi_file_iread_shared,
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, buf, count, datatype, status, ierr))


/*
 * MPI_File_iread
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_iread
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_iread
#endif
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{

  MPI_Status local_status;
  *ierr = MPI_File_iread( MPI_File_f2c(*mfile), buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);

}
OSS_WRAP_FORTRAN(MPI_FILE_IREAD,mpi_file_iread,__wrap_mpi_file_iread,
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, buf, count, datatype, status, ierr))



/*
 * MPI_File_iwrite_at
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_iwrite_at
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_iwrite_at
#endif
  ( MPI_Fint* mfile,
    MPI_Offset* offset, 
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{


  MPI_Status local_status;
  *ierr = MPI_File_iwrite_at( MPI_File_f2c(*mfile), *offset, buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);

}
OSS_WRAP_FORTRAN(MPI_FILE_IWRITE_AT,mpi_file_iwrite_at,__wrap_mpi_file_iwrite_at,
  ( MPI_Fint* mfile,
    MPI_Offset* offset, 
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, offset, buf, count, datatype, status, ierr))


/*
 * MPI_File_iwrite
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_iwrite
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_iwrite
#endif
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{


  MPI_Status local_status;
  *ierr = MPI_File_iwrite( MPI_File_f2c(*mfile), buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);


}
OSS_WRAP_FORTRAN(MPI_FILE_IWRITE,mpi_file_iwrite,__wrap_mpi_file_iwrite,
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, buf, count, datatype, status, ierr))


/*
 * MPI_File_iwrite_shared
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_iwrite_shared
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_iwrite_shared
#endif
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{


  MPI_Status local_status;
  *ierr = MPI_File_iwrite_shared( MPI_File_f2c(*mfile), buf, *count, MPI_Type_f2c(*datatype), &local_status);
  MPI_Status_c2f(&local_status, status);


}
OSS_WRAP_FORTRAN(MPI_FILE_IWRITE_SHARED,mpi_file_iwrite_shared,__wrap_mpi_file_iwrite_shared,
  ( MPI_Fint* mfile,
    char* buf, 
    MPI_Fint* count,
    MPI_Datatype* datatype, 
    MPI_Fint* status,
    MPI_Fint *ierr),
    (mfile, buf, count, datatype, status, ierr))


/*
 * MPI_File_seek
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_seek
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_seek
#endif
  ( MPI_Fint* mfile,
    MPI_Fint* offset, 
    MPI_Fint* whence,
    MPI_Fint* ierr)
{

  *ierr = MPI_File_seek( MPI_File_f2c(*mfile), *offset, *whence);

}
OSS_WRAP_FORTRAN(MPI_FILE_SEEK,mpi_file_seek,__wrap_mpi_file_seek,
  ( MPI_Fint* mfile,
    MPI_Fint* offset, 
    MPI_Fint* whence,
    MPI_Fint *ierr),
    (mfile, offset, whence, ierr))



/*
 * MPI_File_seek_shared
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_seek_shared
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_seek_shared
#endif
  ( MPI_Fint* mfile,
    MPI_Fint* offset, 
    MPI_Fint* whence,
    MPI_Fint* ierr)
{

  *ierr = MPI_File_seek_shared( MPI_File_f2c(*mfile), *offset, *whence);

}
OSS_WRAP_FORTRAN(MPI_FILE_SEEK_SHARED,mpi_file_seek_shared,__wrap_mpi_file_seek_shared,
  ( MPI_Fint* mfile,
    MPI_Fint* offset, 
    MPI_Fint* whence,
    MPI_Fint *ierr),
    (mfile, offset, whence, ierr))


/*
 * MPI_File_get_position
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_get_position
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_get_position
#endif
  ( MPI_Fint* mfile,
    MPI_Offset* offset,
    MPI_Fint* ierr)
{

  MPI_Offset local_offset;
  *ierr = MPI_File_get_position( MPI_File_f2c(*mfile), &local_offset);
  *offset = local_offset;

}
OSS_WRAP_FORTRAN(MPI_FILE_GET_POSITION,mpi_file_get_position,__wrap_mpi_file_get_position,
  ( MPI_Fint* mfile,
    MPI_Offset* offset,
    MPI_Fint *ierr),
    (mfile, offset, ierr))


/*
 * MPI_File_get_position_shared
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
void mpi_file_get_position_shared
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
void __wrap_mpi_file_get_position_shared
#endif
  ( MPI_Fint* mfile,
    MPI_Offset* offset,
    MPI_Fint* ierr)
{

  MPI_Offset local_offset;
  *ierr = MPI_File_get_position_shared( MPI_File_f2c(*mfile), &local_offset);
  *offset = local_offset;

}
OSS_WRAP_FORTRAN(MPI_FILE_GET_POSITION_SHARED,mpi_file_get_position_shared,__wrap_mpi_file_get_position_shared,
  ( MPI_Fint* mfile,
    MPI_Offset* offset,
    MPI_Fint *ierr),
    (mfile, offset, ierr))


