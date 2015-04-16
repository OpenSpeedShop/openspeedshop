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
 * MPI function wrappers for the MPI extended event tracing collector.
 *
 */

#include "RuntimeAPI.h"
#include "runtime.h"

#include <mpi.h>

#if defined (OPENSS_OFFLINE)
extern int OpenSS_mpi_rank;
#endif

static int debug_trace = 0;

/*
 * MPI_Irecv
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Irecv(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Irecv(
#else
int mpit_PMPI_Irecv(
#endif
		    void* buf, int count, MPI_Datatype datatype, int source, 
		    int tag, MPI_Comm comm, MPI_Request* request)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Irecv");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();
    event.source = source;

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);

    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;

    /* Initialize unused arguments */
    event.destination = -1;


    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Irecv));

    }

    return retval;
}

/*
 * MPI_Recv
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Recv(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Recv(
#else
int mpit_PMPI_Recv(
#endif
    void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int source, 
    int tag, 
    MPI_Comm comm, 
    MPI_Status *status)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Recv");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Recv(buf, count, datatype, source,  tag, comm, status);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.destination = -1;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Recv));

    }

    return retval;
}
  

/*
 * MPI_Recv_init
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Recv_init(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Recv_init(
#else
int mpit_PMPI_Recv_init(
#endif
    void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int source, 
    int tag, 
    MPI_Comm comm, 
    MPI_Request *request)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Recv_init");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Recv_init(buf, count, datatype, source,  tag, comm, request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.destination = -1;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Recv_init));

    }

    return retval;
}
  
/*
 * MPI_Iprobe
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Iprobe(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Iprobe(
#else
int mpit_PMPI_Iprobe(
#endif
    int source, 
    int tag, 
    MPI_Comm comm, 
    int *flag, 
    MPI_Status *status)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Iprobe");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Iprobe(source, tag, comm, flag, status);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.retval = retval;

    /* Initialize unused arguments */
    event.destination = -1;
    event.datatype = 0;
    event.size = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Iprobe));

    }

    return retval;
}
  
/*
 * MPI_probe
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Probe(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Probe(
#else
int mpit_PMPI_Probe(
#endif
    int source, 
    int tag, 
    MPI_Comm comm, 
    MPI_Status *status)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Probe");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Probe(source, tag, comm, status);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.retval = retval;

    /* Initialize unused arguments */
    event.destination = -1;
    event.datatype = 0;
    event.size = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Probe));

    }

    return retval;
}
  


/*
 * MPI_Isend
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Isend(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Isend(
#else
int mpit_PMPI_Isend(
#endif
#if MPI_VERSION >= 3
    const void* buf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* buf,
#else
    void* buf,
#endif
    int count, 
    MPI_Datatype datatype, 
    int dest,
    int tag, 
    MPI_Comm comm, 
    MPI_Request* request)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Isend");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    event.destination = dest;
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Isend));

    }

    return retval;
}

/*
 * MPI_Bsend
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Bsend(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Bsend(
#else
int mpit_PMPI_Bsend(
#endif
#if MPI_VERSION >= 3
    const void* buf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* buf,
#else
    void* buf,
#endif
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Bsend");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Bsend(buf, count, datatype, dest, tag, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Bsend));

    }

    return retval;
}
  

/*
 * MPI_Bsend_init
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Bsend_init(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Bsend_init(
#else
int mpit_PMPI_Bsend_init(
#endif
#if MPI_VERSION >= 3
    const void* buf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* buf,
#else
    void* buf,
#endif
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm,
    MPI_Request* request)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("Bsend_init");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Bsend_init));

    }

    return retval;
}
  
/*
 * MPI_Ibsend
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Ibsend(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Ibsend(
#else
int mpit_PMPI_Ibsend(
#endif
#if MPI_VERSION >= 3
    const void* buf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* buf,
#else
    void* buf,
#endif
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm, 
    MPI_Request *request)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Ibsend");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Ibsend(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Ibsend));

    }

    return retval;
}
  
/*
 * MPI_Irsend
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Irsend(    
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Irsend(    
#else
int mpit_PMPI_Irsend(    
#endif

#if MPI_VERSION >= 3
    const void* buf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* buf,
#else
    void* buf,
#endif
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm, 
    MPI_Request *request)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Irsend");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Irsend(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Irsend));

    }

    return retval;
}
    
/*
 * MPI_Issend
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Issend(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Issend(
#else
int mpit_PMPI_Issend(
#endif
#if MPI_VERSION >= 3
    const void* buf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* buf,
#else
    void* buf,
#endif
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm, 
    MPI_Request *request)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Issend");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Issend(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Issend));

    }

    return retval;
}
  
/*
 * MPI_Rsend
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Rsend(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Rsend(
#else
int mpit_PMPI_Rsend(
#endif
#if MPI_VERSION >= 3
    const void* buf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* buf,
#else
    void* buf,
#endif
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Rsend");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Rsend(buf, count, datatype, dest, tag, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Rsend));

    }

    return retval;
}
  
  
/*
 * MPI_Rsend_init
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Rsend_init(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Rsend_init(
#else
int mpit_PMPI_Rsend_init(
#endif
#if MPI_VERSION >= 3
    const void* buf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* buf,
#else
    void* buf,
#endif
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm,
    MPI_Request* request 
)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Rsend_init");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Rsend_init));

    }

    return retval;
}
  
/*
 * MPI_Send
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Send(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Send(
#else
int mpit_PMPI_Send(
#endif
#if MPI_VERSION >= 3
    const void* buf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* buf,
#else
    void* buf,
#endif
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Send");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Send(buf, count, datatype, dest, tag, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Send));

    }

    return retval;
}
  
  
/*
 * MPI_Send_init
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Send_init(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Send_init(
#else
int mpit_PMPI_Send_init(
#endif
#if MPI_VERSION >= 3
    const void* buf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* buf,
#else
    void* buf,
#endif
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm,
    MPI_Request* request
)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Send_init");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Send_init));

    }

    return retval;
}
  
/*
 * MPI_Ssend
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Ssend(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Ssend(
#else
int mpit_PMPI_Ssend(
#endif
#if MPI_VERSION >= 3
    const void* buf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* buf,
#else
    void* buf,
#endif
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Ssend");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Ssend(buf, count, datatype, dest, tag, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Ssend));

    }

    return retval;
}
  
  
/*
 * MPI_Ssend_init
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Ssend_init(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Ssend_init(
#else
int mpit_PMPI_Ssend_init(
#endif
#if MPI_VERSION >= 3
    const void* buf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* buf,
#else
    void* buf,
#endif
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm,
    MPI_Request* request
)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Ssend_init");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Ssend_init));

    }

    return retval;
}
  
/*
 * MPI_Waitall
 */
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Waitall(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Waitall(
#else
int mpit_PMPI_Waitall(
#endif
    int count, 
    MPI_Request *array_of_requests, 
    MPI_Status *status)
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Waitall");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Waitall(count, array_of_requests, status);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Waitall));

    }

    return retval;
}

/*
 * MPI_Finalize
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Finalize()
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Finalize()
#else
int mpit_PMPI_Finalize()
#endif
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Finalize");

    if (dotrace) {

    mpit_start_event(&event);

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));

    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Finalize();

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    event.retval = retval;

    /* Initialize unused arguments */
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.source = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Finalize));

    }

    return retval;
}

/*
 * MPI_Waitsome
 
    What do I do with out and in count?
    
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Waitsome(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Waitsome(
#else
int mpit_PMPI_Waitsome(
#endif
    int incount, 
    MPI_Request *array_of_requests, 
    int *outcount, 
    int *array_of_indices, 
    MPI_Status *array_of_statuses
    )
{
    int retval;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Waitsome");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Waitsome( incount, array_of_requests, 
    	    	    	    outcount, array_of_indices, 
			    array_of_statuses);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Waitsome));

    }

    return retval;
}

/*
 * MPI_Testsome
 
    What do I do with out and in count?
    
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Testsome(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Testsome(
#else
int mpit_PMPI_Testsome(
#endif
    int incount, 
    MPI_Request *array_of_requests, 
    int *outcount, 
    int *array_of_indices, 
    MPI_Status *array_of_statuses
    )
{
    int retval;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Testsome");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Testsome( incount, array_of_requests, 
    	    	    	    outcount, array_of_indices, 
			    array_of_statuses);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Testsome));

    }

    return retval;
}

/*
 * MPI_Waitany
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Waitany(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Waitany(
#else
int mpit_PMPI_Waitany(
#endif
    int count,   
    MPI_Request *array_of_requests, 
    int *index, 
    MPI_Status *status
    )
{
    int retval;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Waitany");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Waitany(count,  array_of_requests, index, status);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Waitany));

    }

    return retval;
}

/*
 * MPI_Unpack
 
    Which size do I use, insize or outsize?
    I'm going to use outsize.
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Unpack(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Unpack(
#else
int mpit_PMPI_Unpack(
#endif
#if MPI_VERSION >= 3
    const void* inbuf, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* inbuf, 
#else
    void* inbuf, 
#endif
    int insize, 
    int *position, 
    void *outbuf, 
    int outcount, 
    MPI_Datatype datatype, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Unpack");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Unpack(inbuf, insize, position, outbuf, 
    	    	    	 outcount, datatype, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = outcount * datatype_size;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
    event.communicator = -1;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Unpack));

    }

    return retval;
}

/*
 * MPI_Wait
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Wait(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Wait(
#else
int mpit_PMPI_Wait(
#endif
    MPI_Request *request, 
    MPI_Status *status
    )
{
    int retval;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Wait");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Wait(request, status);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Wait));

    }

    return retval;
}

/*
 * MPI_Testany
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Testany(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Testany(
#else
int mpit_PMPI_Testany(
#endif
    int count,
    MPI_Request *array_of_requests,
    int *index,
    int *flag,
    MPI_Status *status
    )
{
    int retval;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Testany");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Testany(count,array_of_requests, index, flag, status);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Testany));

    }

    return retval;
}

/*
 * MPI_Testall
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Testall(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Testall(
#else
int mpit_PMPI_Testall(
#endif
    int count,
    MPI_Request *array_of_requests, 
    int *flag, 
    MPI_Status *status
    )
{
    int retval;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Testall");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Testall(count, array_of_requests, flag, status);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Testall));

    }

    return retval;
}

/*
 * MPI_Test
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Test(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Test(
#else
int mpit_PMPI_Test(
#endif
    MPI_Request *request, 
    int *flag, 
    MPI_Status *status
    )
{
    int retval;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Test");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Test(request, flag, status);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Test));

    }

    return retval;
}

/*
 * MPI_Scan
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Scan(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Scan(
#else
int mpit_PMPI_Scan(
#endif
#if MPI_VERSION >= 3
    const void* sendbuf, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* sendbuf, 
#else
    void* sendbuf, 
#endif
    void* recvbuf, 
    int count, 
    MPI_Datatype datatype, 
    MPI_Op op, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Scan");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Scan));

    }

    return retval;
}

/*
 * MPI_Request_free
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Request_free(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Request_free(
#else
int mpit_PMPI_Request_free(
#endif
    MPI_Request *request
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Request_free");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Request_free(request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Request_free));

    }

    return retval;
}

/*
 * MPI_Reduce_scatter
 
    This is questionable with recvcounts.
    
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Reduce_scatter(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Reduce_scatter(
#else
int mpit_PMPI_Reduce_scatter(
#endif
#if MPI_VERSION >= 3
    const void* sendbuf, 
    void* recvbuf, 
    const int *recvcounts, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* sendbuf, 
    void* recvbuf, 
    int *recvcounts, 
#else
    void* sendbuf, 
    void* recvbuf, 
    int *recvcounts, 
#endif
    MPI_Datatype datatype, 
    MPI_Op op, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Reduce_scatter");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Reduce_scatter(sendbuf, recvbuf, recvcounts,  datatype, op, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = *recvcounts * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Reduce_scatter));

    }

    return retval;
}

/*
 * MPI_Reduce
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Reduce(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Reduce(
#else
int mpit_PMPI_Reduce(
#endif
#if MPI_VERSION >= 3
    const void* sendbuf, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* sendbuf, 
#else
    void* sendbuf, 
#endif
    void* recvbuf, 
    int count, 
    MPI_Datatype datatype, 
    MPI_Op op, 
    int root, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Reduce");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Reduce));

    }

    return retval;
}

/*
 * MPI_Pack
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Pack(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Pack(
#else
int mpit_PMPI_Pack(
#endif
#if MPI_VERSION >= 3
    const void* inbuf, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* inbuf, 
#else
    void* inbuf, 
#endif
    int incount, 
    MPI_Datatype datatype, 
    void *outbuf, 
    int outsize, 
    int *position, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Pack");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Pack( inbuf, incount, datatype, outbuf, 
    	    	    	outsize, position, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = incount * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Pack));

    }

    return retval;
}

/*
 * MPI_Init
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Init(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Init(
#else
int mpit_PMPI_Init(
#endif
    int *argc, 
    char ***argv
    )
{
    int retval;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Init");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Init(argc, argv);

#if defined (OPENSS_OFFLINE)
    int oss_rank = -1;
    PMPI_Comm_rank(MPI_COMM_WORLD, &oss_rank);
    OpenSS_mpi_rank = oss_rank;
#endif

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Init));

    }

    return retval;
}

/*
 * MPI_Get_count
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Get_count(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Get_count(
#else
int mpit_PMPI_Get_count(
#endif
#if MPI_VERSION >= 3
    const MPI_Status *status, 
#else
    MPI_Status *status, 
#endif
    MPI_Datatype datatype, 
    int *count
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Get_count");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Get_count(status, datatype, count);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = *count * datatype_size;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
    event.communicator = -1;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Get_count));

    }

    return retval;
}


/* This must be commented out (#if 0) for kestral to work */
#if 1
/*
 * MPI_Gatherv
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Gatherv(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Gatherv(
#else
int mpit_PMPI_Gatherv(
#endif
#if MPI_VERSION >= 3
    const void* sendbuf, 
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    const int *recvcounts, 
    const int *displs, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* sendbuf, 
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int *recvcounts, 
    int *displs, 
#else
    void* sendbuf, 
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int *recvcounts, 
    int *displs, 
#endif
    MPI_Datatype recvtype, 
    int root, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Gatherv");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, 
    	    	    	  recvcounts, displs, recvtype, root, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    event.size = *recvcounts * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) recvtype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
    event.communicator = -1;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Gatherv));

    }

    return retval;
}

#endif

/*
 * MPI_Gather
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Gather(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Gather(
#else
int mpit_PMPI_Gather(
#endif
#if MPI_VERSION >= 3
    const void* sendbuf, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* sendbuf, 
#else
    void* sendbuf, 
#endif
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int recvcount, 
    MPI_Datatype recvtype, 
    int root, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Gather");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Gather(sendbuf, sendcount, sendtype, 
    	    	    	 recvbuf, recvcount, recvtype, 
			 root, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    event.size = recvcount * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) recvtype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Gather));

    }

    return retval;
}

/*
 * MPI_Cancel
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Cancel(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Cancel(
#else
int mpit_PMPI_Cancel(
#endif
    MPI_Request *request
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Cancel");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Cancel(request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Cancel));

    }

    return retval;
}

/*
 * MPI_Bcast
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Bcast(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Bcast(
#else
int mpit_PMPI_Bcast(
#endif
    void* buffer, 
    int count, 
    MPI_Datatype datatype, 
    int root, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Bcast");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Bcast(buffer, count, datatype, root, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Bcast));

    }

    return retval;
}

/*
 * MPI_Barrier
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Barrier(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Barrier(
#else
int mpit_PMPI_Barrier(
#endif
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Barrier");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Barrier(comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.communicator = (int64_t) comm;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Barrier));

    }

    return retval;
}

/*
 * MPI_Alltoallv
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Alltoallv(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Alltoallv(
#else
int mpit_PMPI_Alltoallv(
#endif
#if MPI_VERSION >= 3
    const void* sendbuf, 
    const int *sendcounts, 
    const int *sdispls, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    const int *recvcounts, 
    const int *rdispls, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* sendbuf, 
    int *sendcounts, 
    int *sdispls, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int *recvcounts, 
    int *rdispls, 
#else
    void* sendbuf, 
    int *sendcounts, 
    int *sdispls, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int *recvcounts, 
    int *rdispls, 
#endif
    MPI_Datatype recvtype, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Alltoallv");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Alltoallv(sendbuf, sendcounts, sdispls, 
    	    	    	    sendtype, recvbuf, recvcounts, 
			    rdispls, recvtype, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    event.size = *recvcounts * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) recvtype;
    event.retval = retval;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Alltoallv));

    }

    return retval;
}

/*
 * MPI_Alltoall
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Alltoall(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Alltoall(
#else
int mpit_PMPI_Alltoall(
#endif
#if MPI_VERSION >= 3
    const void* sendbuf, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* sendbuf, 
#else
    void* sendbuf, 
#endif
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int recvcount, 
    MPI_Datatype recvtype, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Alltoall");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Alltoall( sendbuf, sendcount, sendtype, 
    	    	    	    recvbuf, recvcount, recvtype, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    event.size = recvcount * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) recvtype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Alltoall));

    }

    return retval;
}

/*
 * MPI_Allreduce
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Allreduce(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Allreduce(
#else
int mpit_PMPI_Allreduce(
#endif
#if MPI_VERSION >= 3
    const void* sendbuf, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* sendbuf, 
#else
    void* sendbuf, 
#endif
    void* recvbuf, 
    int count, 
    MPI_Datatype datatype, 
    MPI_Op op, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Allreduce");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Allreduce));

    }

    return retval;
}

/*
 * MPI_Allgatherv
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Allgatherv(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Allgatherv(
#else
int mpit_PMPI_Allgatherv(
#endif
#if MPI_VERSION >= 3
    const void* sendbuf, 
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    const int *recvcounts, 
    const int *displs, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* sendbuf, 
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int *recvcounts, 
    int *displs, 
#else
    void* sendbuf, 
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int *recvcounts, 
    int *displs, 
#endif
    MPI_Datatype recvtype, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Allgatherv");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Allgatherv(sendbuf, sendcount, sendtype, 
    	    	    	     recvbuf, recvcounts, displs, 
			     recvtype, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    event.size = *recvcounts * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) recvtype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Allgatherv));

    }

    return retval;
}

/*
 * MPI_Allgather
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Allgather(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Allgather(
#else
int mpit_PMPI_Allgather(
#endif
#if MPI_VERSION >= 3
    const void* sendbuf, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* sendbuf, 
#else
    void* sendbuf, 
#endif
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int recvcount, 
    MPI_Datatype recvtype, 
    MPI_Comm comm
    )
{
    int retval;

    int datatype_size;
    mpit_event event;
    
    bool_t dotrace = mpit_do_trace("MPI_Allgather");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, 
    	    	    	    recvcount, recvtype, comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    event.size = recvcount * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) recvtype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Allgather));

    }

    return retval;
}

/*
 * MPI_Scatter
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
MPI_Scatter(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
__wrap_MPI_Scatter(
#else
mpit_PMPI_Scatter(
#endif
#if MPI_VERSION >= 3
    const void* sendbuf, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    const void* sendbuf, 
#else
    void* sendbuf, 
#endif
    	int 	    	sendcount, 
    	MPI_Datatype	sendtype, 

    	void*     	recvbuf, 
    	int 	    	recvcount, 
    	MPI_Datatype	recvtype, 

    	int 	    	root, 
    	MPI_Comm    	comm)
{
    int retval;

    int datatype_size;
    mpit_event send_event;
    mpit_event recv_event;
    
    bool_t dotrace = mpit_do_trace("MPI_Scatter");

    if (dotrace) {

    mpit_start_event(&send_event);
    mpit_start_event(&recv_event);

    /* Set up the send record */
    send_event.source = root;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(send_event.source));
    PMPI_Type_size(sendtype, &datatype_size);
    send_event.size = sendcount * datatype_size;
    send_event.datatype = (int64_t) sendtype;
    
    send_event.start_time = OpenSS_GetTime();

    /* Initialize unused arguments */
    send_event.tag = 0;

    }

    retval = PMPI_Scatter( sendbuf, sendcount, sendtype,  
    	    	    	    recvbuf, recvcount, recvtype, 
			    root, comm);

    if (dotrace) {

    send_event.stop_time = OpenSS_GetTime();
    
    send_event.communicator = (int64_t) comm;
    send_event.retval = retval;
    recv_event.start_time = send_event.start_time;
    recv_event.stop_time = send_event.stop_time;
    mpit_record_event(&send_event, OpenSS_GetAddressOfFunction(PMPI_Scatter));

    /* Set up the recv record */
    PMPI_Type_size(recvtype, &datatype_size);
    recv_event.size = recvcount * datatype_size;
    recv_event.datatype = (int64_t) recvtype;

    recv_event.communicator = (int64_t) comm;
    recv_event.retval = retval;

    /* Initialize unused arguments */
    recv_event.tag = 0;

    mpit_record_event(&recv_event, OpenSS_GetAddressOfFunction(PMPI_Scatter));

    }

    return retval;
}
/*
 * MPI_Scatterv
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
MPI_Scatterv(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
__wrap_MPI_Scatterv(
#else
mpit_PMPI_Scatterv(
#endif

#if MPI_VERSION >= 3
    	const void*     	sendbuf, 
    	const int* 	    	sendcounts, 
	const int*	    	displs,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    	const void*     	sendbuf, 
    	int* 	    	sendcounts, 
	int*	    	displs,
#else
    	void*     	sendbuf, 
    	int* 	    	sendcounts, 
	int*	    	displs,
#endif
    	MPI_Datatype	sendtype, 

    	void*     	recvbuf, 
    	int 	    	recvcount, 
    	MPI_Datatype	recvtype, 

    	int 	    	root, 
    	MPI_Comm    	comm)
{
    int retval;

    int datatype_size;
    mpit_event send_event;
    mpit_event recv_event;
    
    bool_t dotrace = mpit_do_trace("MPI_Scatterv");

    if (dotrace) {

    mpit_start_event(&send_event);
    mpit_start_event(&recv_event);

    /* Set up the send record */
    send_event.source = root;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(send_event.source));
    PMPI_Type_size(sendtype, &datatype_size);
    /* This is surly wrong */
    send_event.size = sendcounts[0] * datatype_size;
    send_event.datatype = (int64_t) sendtype;
    
    send_event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Scatterv( sendbuf, sendcounts, displs, sendtype,  
    	    	    	    recvbuf, recvcount, recvtype, 
			    root, comm);

    if (dotrace) {

    send_event.stop_time = OpenSS_GetTime();
    
    send_event.communicator = (int64_t) comm;
    send_event.retval = retval;
    recv_event.start_time = send_event.start_time;
    recv_event.stop_time = send_event.stop_time;

    /* Initialize unused arguments */
    send_event.tag = 0;

    mpit_record_event(&send_event, OpenSS_GetAddressOfFunction(PMPI_Scatterv));

    /* Set up the recv record */
    PMPI_Type_size(recvtype, &datatype_size);
    recv_event.size = recvcount * datatype_size;
    recv_event.datatype = (int64_t) recvtype;

    recv_event.communicator = (int64_t) comm;
    recv_event.retval = retval;

    /* Initialize unused arguments */
    recv_event.tag = 0;

    mpit_record_event(&recv_event, OpenSS_GetAddressOfFunction(PMPI_Scatterv));

    }

    return retval;
}

/*
 * MPI_Sendrecv
 */

int 
#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
MPI_Sendrecv(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
__wrap_MPI_Sendrecv(
#else
mpit_PMPI_Sendrecv(
#endif
#if MPI_VERSION >= 3
    	const void*     	sendbuf, 
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
    	const void*     	sendbuf, 
#else
    	void*     	sendbuf, 
#endif
    	int 	    	sendcount, 
    	MPI_Datatype	sendtype, 
    	int 	    	dest, 
    	int	    	sendtag, 

    	void*     	recvbuf, 
    	int 	    	recvcount, 
    	MPI_Datatype	recvtype, 
    	int 	    	source, 
    	int	    	recvtag, 

    	MPI_Comm comm, 
    	MPI_Status* status)
{
    int retval;

    int datatype_size;
    mpit_event send_event;
    mpit_event recv_event;
    
    bool_t dotrace = mpit_do_trace("MPI_Sendrecv");

    if (dotrace) {

    mpit_start_event(&send_event);
    mpit_start_event(&recv_event);

    /* Set up the send record */
    send_event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(send_event.source));
    PMPI_Type_size(sendtype, &datatype_size);
    send_event.size = sendcount * datatype_size;
    send_event.tag = sendtag;
    send_event.datatype = (int64_t) sendtype;
    
    send_event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Sendrecv( sendbuf, sendcount, sendtype, dest, sendtag, 
    	    	    	    recvbuf, recvcount, recvtype, source, recvtag,
			    comm, status);

    if (dotrace) {

    send_event.stop_time = OpenSS_GetTime();
    
    send_event.communicator = (int64_t) comm;
    send_event.retval = retval;
    recv_event.start_time = send_event.start_time;
    recv_event.stop_time = send_event.stop_time;
    mpit_record_event(&send_event, OpenSS_GetAddressOfFunction(PMPI_Sendrecv));

    /* Set up the recv record */
    recv_event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(recv_event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    recv_event.size = recvcount * datatype_size;
    recv_event.tag = recvtag;
    recv_event.datatype = (int64_t) recvtype;

    recv_event.communicator = (int64_t) comm;
    recv_event.retval = retval;

    mpit_record_event(&recv_event, OpenSS_GetAddressOfFunction(PMPI_Sendrecv));

    }

    return retval;
}

/*
 * MPI_Sendrecv
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Sendrecv_replace(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Sendrecv_replace(
#else
int mpit_PMPI_Sendrecv_replace(
#endif
    	void*     	buf, 
    	int 	    	count, 
    	MPI_Datatype	datatype, 
    	int 	    	dest, 
    	int	    	sendtag, 

    	int 	    	source, 
    	int	    	recvtag, 

    	MPI_Comm    	comm, 
    	MPI_Status* 	status)
{
    int retval;

    int datatype_size;
    mpit_event send_event;
    mpit_event recv_event;
    
    bool_t dotrace = mpit_do_trace("MPI_Sendrecv_replace");

    if (dotrace) {

    mpit_start_event(&send_event);
    mpit_start_event(&recv_event);

    /* Set up the send record */
    send_event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(send_event.source));
    PMPI_Type_size(datatype, &datatype_size);
    send_event.size = count * datatype_size;
    send_event.tag = sendtag;
    send_event.datatype = (int64_t) datatype;
    
    send_event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Sendrecv_replace( buf, count, datatype, dest, sendtag, 
    	    	    	    	    source, recvtag,
			    	    comm, status);

    if (dotrace) {

    send_event.stop_time = OpenSS_GetTime();
    
    send_event.communicator = (int64_t) comm;
    send_event.retval = retval;
    recv_event.start_time = send_event.start_time;
    recv_event.stop_time = send_event.stop_time;
    mpit_record_event(&send_event, OpenSS_GetAddressOfFunction(PMPI_Sendrecv_replace));

    /* Set up the recv record */
    recv_event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(recv_event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    recv_event.size = count * datatype_size;
    recv_event.tag = recvtag;
    recv_event.datatype = (int64_t) datatype;

    recv_event.communicator = (int64_t) comm;
    recv_event.retval = retval;
    mpit_record_event(&recv_event, OpenSS_GetAddressOfFunction(PMPI_Sendrecv_replace));

    }

    return retval;
}



/*
 *-----------------------------------------------------------------------------
 *
 * Cartesian Toplogy functions
 *
 *-----------------------------------------------------------------------------
 */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Cart_create(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Cart_create(
#else
int mpit_PMPI_Cart_create(
#endif
		     MPI_Comm comm_old,
                     int ndims,
#if MPI_VERSION >= 3
                     const int* dims,
                     const int* periodv,
#else
                     int* dims,
                     int* periodv,
#endif
                     int reorder,
                     MPI_Comm* comm_cart)
{

    int retval;

    int datatype_size;
    mpit_event event;

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpit_PMPI_Cart_create called, comm_old = %d \n", comm_old);
      fflush(stderr);
    }

    bool_t dotrace = mpit_do_trace("MPI_Cart_create");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Cart_create(comm_old, ndims, dims, periodv, reorder, comm_cart);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Cart_create));

    }

    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Cart_sub (
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Cart_sub (
#else
int mpit_PMPI_Cart_sub (
#endif
		   MPI_Comm comm,
#if MPI_VERSION >= 3
                   const int *rem_dims,
#else
                   int *rem_dims,
#endif
                   MPI_Comm *newcomm)
{

    int retval;

    int datatype_size;
    mpit_event event;

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpit_PMPI_Cart_sub called, comm = %d \n", comm);
      fflush(stderr);
    }

    bool_t dotrace = mpit_do_trace("MPI_Cart_sub");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Cart_sub(comm, rem_dims, newcomm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Cart_sub));

    }

    return retval;
}



#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Graph_create(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Graph_create(
#else
int mpit_PMPI_Graph_create(
#endif
		      MPI_Comm comm_old,
                      int nnodes,
#if MPI_VERSION >= 3
                      const int* index,
                      const int* edges,
#else
                      int* index,
                      int* edges,
#endif
                      int reorder,
                      MPI_Comm* comm_graph)
{
    int retval;

    int datatype_size;
    mpit_event event;

    bool_t dotrace = mpit_do_trace("MPI_Graph_create");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpit_PMPI_Graph_create called, comm_old= %d \n", comm_old);
      fflush(stderr);
    }

    }

    retval = PMPI_Graph_create(comm_old, nnodes, index, edges, reorder, comm_graph);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Graph_create));

    }

    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Intercomm_create (
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Intercomm_create (
#else
int mpit_PMPI_Intercomm_create (
#endif
			  MPI_Comm local_comm,
                          int local_leader,
                          MPI_Comm peer_comm,
                          int remote_leader,
                          int tag,
                          MPI_Comm *newintercomm)

{
    int retval;

    int datatype_size;
    mpit_event event;

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpit_PMPI_Intercomm_create called, local_comm = %d \n", local_comm);
      fflush(stderr);
    }

    bool_t dotrace = mpit_do_trace("MPI_Intercomm_create");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();


    }

    retval = PMPI_Intercomm_create(local_comm, local_leader, peer_comm,
                                 remote_leader, tag, newintercomm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Intercomm_create));

    }

    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Intercomm_merge (
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Intercomm_merge (
#else
int mpit_PMPI_Intercomm_merge (
#endif
			 MPI_Comm intercomm,
                         int high,
                         MPI_Comm *newcomm)
{
    int retval;

    int datatype_size;
    mpit_event event;

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpit_PMPI_Intercomm_merge called, intercomm = %d \n", intercomm);
      fflush(stderr);
    }

    bool_t dotrace = mpit_do_trace("MPI_Intercomm_merge");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Intercomm_merge(intercomm, high, newcomm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Intercomm_merge));

    }

    return retval;
}


/* ------- Destructors ------- */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Comm_free(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Comm_free(
#else
int mpit_PMPI_Comm_free(
#endif
			MPI_Comm* comm )
{
    int retval;

    int datatype_size;
    mpit_event event;

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpit_PMPI_Comm_free called, comm = %d \n", comm);
      fflush(stderr);
    }

    bool_t dotrace = mpit_do_trace("MPI_Comm_free");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Comm_free(comm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Comm_free));

    }

    return retval;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Communicator management
 *
 *-----------------------------------------------------------------------------
 */

/* ------- Constructors ------- */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Comm_dup(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Comm_dup(
#else
int mpit_PMPI_Comm_dup(
#endif
			MPI_Comm comm,
                        MPI_Comm* newcomm )
{
    int retval;
    mpit_event event;

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpit_PMPI_Comm_dup called, comm = %d \n", comm);
      fflush(stderr);
    }

    bool_t dotrace = mpit_do_trace("MPI_Comm_dup");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Comm_dup(comm, newcomm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Comm_dup));

    }

    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Comm_create(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Comm_create(
#else
int mpit_PMPI_Comm_create(
#endif
		     MPI_Comm comm,
                     MPI_Group group,
                     MPI_Comm* newcomm )
{
    int retval;
    mpit_event event;

    bool_t dotrace = mpit_do_trace("MPI_Comm_create");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpit_PMPI_Comm_create called, comm = %d \n", comm);
      fflush(stderr);
    }

    }

    retval = PMPI_Comm_create(comm, group, newcomm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Comm_create));

    }

    return retval;
}

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Comm_split(
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Comm_split(
#else
int mpit_PMPI_Comm_split(
#endif
			  MPI_Comm comm,
                          int color,
                          int key,
                          MPI_Comm* newcomm )
{
    int retval;
    mpit_event event;

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpit_PMPI_Comm_split called, comm = %d \n", comm);
      fflush(stderr);
    }

    bool_t dotrace = mpit_do_trace("MPI_Comm_split");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Comm_split(comm, color, key, newcomm);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Comm_split));

    }

    return retval;
}


/* -- MPI_Start -- */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Start
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Start
#else
int mpit_PMPI_Start
#endif
		( MPI_Request* request )
{
    int retval;
    mpit_event event;

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpit_PMPI_Start called\n");
      fflush(stderr);
    }

    bool_t dotrace = mpit_do_trace("MPI_Start");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Start(request);

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Start));

    }

    return retval;
}

/* -- MPI_Startall -- */

#if defined (OPENSS_OFFLINE) && !defined(OPENSS_STATIC)
int MPI_Startall
#elif defined (OPENSS_STATIC) && defined (OPENSS_OFFLINE)
int __wrap_MPI_Startall
#else
int mpit_PMPI_Startall
#endif
			( int count,
                        MPI_Request *array_of_requests )
{
    int retval;
    mpit_event event;

    if (debug_trace) {
        fprintf(stderr, "WRAPPER, mpit_PMPI_Startall called, count = %d \n", count);
        fflush(stderr);
    }

    bool_t dotrace = mpit_do_trace("MPI_Startall");

    if (dotrace) {

    mpit_start_event(&event);
    event.start_time = OpenSS_GetTime();

    }

    retval = PMPI_Startall( count, array_of_requests );

    if (dotrace) {

    event.stop_time = OpenSS_GetTime();
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;

    mpit_record_event(&event, OpenSS_GetAddressOfFunction(PMPI_Startall));

    }

    return retval;
}

