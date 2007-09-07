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

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "vt_fbindings.h"
#include <stdlib.h>
#include <mpi.h>

#if defined(__sun) || defined(_SX) || defined(OPEN_MPI) || defined(HP_MPI)
#  define NEED_F2C_CONV
#endif

#ifndef NEED_F2C_CONV

/* -- MPI_Init -- */

void vt_mpi_init_f(MPI_Fint* ierr) {
  *ierr = MPI_Init(0, (char***)0);
} VT_GENERATE_F77_BINDINGS(mpi_init, MPI_INIT,
			   vt_mpi_init_f,
			   (MPI_Fint* ierr),
			   (ierr))

/* -- MPI_Finalize -- */

void vt_mpi_finalize_f(MPI_Fint* ierr) {
  *ierr = MPI_Finalize();  
} VT_GENERATE_F77_BINDINGS(mpi_finalize, MPI_FINALIZE,
			   vt_mpi_finalize_f,
			   (MPI_Fint* ierr),
			   (ierr))

/* -- MPI_Comm_create -- */

void vt_mpi_comm_create_f(MPI_Comm* comm, MPI_Group* group, MPI_Comm* newcomm,
			  MPI_Fint* ierr) {
  *ierr = MPI_Comm_create(*comm, *group, newcomm);
} VT_GENERATE_F77_BINDINGS(mpi_comm_create, MPI_COMM_CREATE,
			   vt_mpi_comm_create_f,
			   (MPI_Comm* comm, MPI_Group* group, MPI_Comm* newcomm, MPI_Fint* ierr),
			   (comm, group, newcomm, ierr))

/* -- MPI_Comm_dup -- */

void vt_mpi_comm_dup_f(MPI_Comm* comm, MPI_Comm* newcomm, MPI_Fint* ierr) {
  *ierr = MPI_Comm_dup(*comm, newcomm);
} VT_GENERATE_F77_BINDINGS(mpi_comm_dup, MPI_COMM_DUP,
			   vt_mpi_comm_dup_f,
			   (MPI_Comm* comm, MPI_Comm* newcomm, MPI_Fint* ierr),
			   (comm, newcomm, ierr))

/* -- MPI_Comm_split -- */

void vt_mpi_comm_split_f(MPI_Comm* comm, MPI_Fint* color, MPI_Fint* key,
			 MPI_Comm* newcomm, MPI_Fint* ierr) {
  *ierr = MPI_Comm_split(*comm, *color, *key, newcomm);
} VT_GENERATE_F77_BINDINGS(mpi_comm_split, MPI_COMM_SPLIT,
			   vt_mpi_comm_split_f,
			   (MPI_Comm* comm, MPI_Fint* color, MPI_Fint* key, MPI_Comm* newcomm, MPI_Fint* ierr),
			   (comm, color, key, newcomm, ierr))  

/* -- MPI_Cart_create -- */

void vt_mpi_cart_create_f(MPI_Comm* old_comm, MPI_Fint* ndims, MPI_Fint* dims,
			  MPI_Fint* periods, MPI_Fint* reorder,
			  MPI_Comm* comm_cart, MPI_Fint* ierr) {
  *ierr = MPI_Cart_create(*old_comm, *ndims, dims, periods,
			  *reorder, comm_cart);
} VT_GENERATE_F77_BINDINGS(mpi_cart_create, MPI_CART_CREATE,
			   vt_mpi_cart_create_f,
			   (MPI_Comm* old_comm, MPI_Fint* ndims, MPI_Fint* dims, MPI_Fint* periods, MPI_Fint* reorder, MPI_Comm* comm_cart, MPI_Fint* ierr),
			   (old_comm, ndims, dims, periods, reorder, comm_cart, ierr))

/* -- MPI_Cart_sub -- */

void vt_mpi_cart_sub_f(MPI_Comm* comm, MPI_Fint* remain_dims,
		       MPI_Comm* new_comm, MPI_Fint* ierr) {
  *ierr = MPI_Cart_sub(*comm, remain_dims, new_comm);
} VT_GENERATE_F77_BINDINGS(mpi_cart_sub, MPI_CART_SUB,
			   vt_mpi_cart_sub_f,
			   (MPI_Comm* comm, MPI_Fint* remain_dims, MPI_Comm* new_comm, MPI_Fint* ierr),
			   (comm, remain_dims, new_comm, ierr))

/* -- MPI_Graph_create -- */

void vt_mpi_graph_create_f(MPI_Comm* comm_old, MPI_Fint* nnodes,
			   MPI_Fint* index, MPI_Fint* edges,
			   MPI_Fint* reorder, MPI_Comm* comm_graph,
			   MPI_Fint* ierr) {
  *ierr = MPI_Graph_create(*comm_old, *nnodes, index, edges,
			   *reorder, comm_graph);
} VT_GENERATE_F77_BINDINGS(mpi_graph_create, MPI_GRAPH_CREATE,
			   vt_mpi_graph_create_f,
			   (MPI_Comm* comm_old, MPI_Fint* nnodes, MPI_Fint* index, MPI_Fint* edges, MPI_Fint* reorder, MPI_Comm* comm_graph, MPI_Fint* ierr),
			   (comm_old, nnodes, index, edges, reorder, comm_graph, ierr))

/* -- MPI_Intercomm_create -- */

void vt_mpi_intercomm_create_f(MPI_Comm* local_comm, MPI_Fint* local_leader,
			       MPI_Comm* bridge_comm,
			       MPI_Fint* remote_leader, MPI_Fint* tag,
			       MPI_Comm* newintercomm,
			       MPI_Fint* ierr) {
  *ierr = MPI_Intercomm_create(*local_comm, *local_leader, *bridge_comm,
			       *remote_leader, *tag, newintercomm);
} VT_GENERATE_F77_BINDINGS(mpi_intercomm_create, MPI_INTERCOMM_CREATE,
			   vt_mpi_intercomm_create_f,
			   (MPI_Comm* local_comm, MPI_Fint* local_leader, MPI_Comm* bridge_comm, MPI_Fint* remote_leader, MPI_Fint* tag, MPI_Comm* newintercomm, MPI_Fint* ierr),
			   (local_comm, local_leader, bridge_comm, remote_leader, tag, newintercomm, ierr))

/* -- MPI_Intercomm_merge -- */

void vt_mpi_intercomm_merge_f(MPI_Comm* intercomm, MPI_Fint* high,
			      MPI_Comm* newintercomm,
			      MPI_Fint* ierr) {
  *ierr = MPI_Intercomm_merge(*intercomm, *high, newintercomm);
} VT_GENERATE_F77_BINDINGS(mpi_intercomm_merge, MPI_INTERCOMM_MERGE,
			   vt_mpi_intercomm_merge_f,
			   (MPI_Comm* intercomm, MPI_Fint* high, MPI_Comm* newintercomm, MPI_Fint* ierr),
			   (intercomm, high, newintercomm, ierr))

/* -- MPI_Comm_free -- */

void vt_mpi_comm_free_f(MPI_Comm* comm, MPI_Fint* ierr) {
  *ierr = MPI_Comm_free(comm);
} VT_GENERATE_F77_BINDINGS(mpi_comm_free, MPI_COMM_FREE,
			   vt_mpi_comm_free_f,
			   (MPI_Comm* comm, MPI_Fint* ierr),
			   (comm, ierr))

/* -- MPI_Send -- */

void vt_mpi_send_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
		   MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm,
		   MPI_Fint* ierr) {
  *ierr = MPI_Send(buf, *count, *datatype, *dest, *tag, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_send, MPI_SEND,
			   vt_mpi_send_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, ierr))

/* -- MPI_Bsend -- */

void vt_mpi_bsend_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
		    MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm,
		    MPI_Fint *ierr) {
  *ierr = MPI_Bsend(buf, *count, *datatype, *dest, *tag, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_bsend, MPI_BSEND,
			   vt_mpi_bsend_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm, MPI_Fint *ierr),
			   (buf, count, datatype, dest, tag, comm, ierr))

/* -- MPI_Rsend -- */

void vt_mpi_rsend_f(char* ibuf, MPI_Fint* count, MPI_Datatype* datatype,
		    MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm,
		    MPI_Fint *ierr) {
  *ierr = MPI_Rsend(ibuf, *count, *datatype, *dest, *tag, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_rsend, MPI_RSEND,
			   vt_mpi_rsend_f,
			   (char* ibuf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm, MPI_Fint *ierr),
			   (ibuf, count, datatype, dest, tag, comm, ierr))

/* -- MPI_Ssend -- */

void vt_mpi_ssend_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
		    MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm,
		    MPI_Fint* ierr) {
  *ierr = MPI_Ssend(buf, *count, *datatype, *dest, *tag, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_ssend, MPI_SSEND,
			   vt_mpi_ssend_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, ierr))

/* -- MPI_Recv -- */

void vt_mpi_recv_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
		   MPI_Fint* source, MPI_Fint* tag, MPI_Comm* comm,
		   MPI_Status* status, MPI_Fint* ierr) {
  *ierr = MPI_Recv(buf, *count, *datatype, *source, *tag, *comm, status);
} VT_GENERATE_F77_BINDINGS(mpi_recv, MPI_RECV,
			   vt_mpi_recv_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Comm* comm, MPI_Status* status, MPI_Fint* ierr),
			   (buf, count, datatype, source, tag, comm, status, ierr))

/* -- MPI_Probe -- */

void vt_mpi_probe_f(MPI_Fint* source, MPI_Fint* tag, MPI_Comm* comm,
		    MPI_Status* status, MPI_Fint* ierr) {
  *ierr = MPI_Probe(*source, *tag, *comm, status);
} VT_GENERATE_F77_BINDINGS(mpi_probe, MPI_PROBE,
			   vt_mpi_probe_f,
			   (MPI_Fint* source, MPI_Fint* tag, MPI_Comm* comm, MPI_Status* status, MPI_Fint* ierr),
			   (source, tag, comm, status, ierr))

/* -- MPI_Sendrecv -- */

void vt_mpi_sendrecv_f(char* sendbuf, MPI_Fint* sendcount,
		       MPI_Datatype* sendtype, MPI_Fint* dest,
		       MPI_Fint* sendtag, char* recvbuf, MPI_Fint* recvcount,
		       MPI_Datatype* recvtype, MPI_Fint* source,
		       MPI_Fint* recvtag, MPI_Comm* comm, MPI_Status* status,
		       MPI_Fint* ierr) {
  *ierr = MPI_Sendrecv(sendbuf, *sendcount, *sendtype, *dest, 
		       *sendtag, recvbuf, *recvcount, *recvtype,
		       *source, *recvtag, *comm, status);
} VT_GENERATE_F77_BINDINGS(mpi_sendrecv, MPI_SENDRECV,
			   vt_mpi_sendrecv_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Datatype* sendtype, MPI_Fint* dest, MPI_Fint* sendtag, char* recvbuf, MPI_Fint* recvcount, MPI_Datatype* recvtype, MPI_Fint* source, MPI_Fint* recvtag, MPI_Comm* comm, MPI_Status* status, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status, ierr))

/* -- MPI_Sendrecv_replace -- */

void vt_mpi_sendrecv_replace_f(char* buf, MPI_Fint* count,
			       MPI_Datatype* datatype, MPI_Fint* dest,
			       MPI_Fint* sendtag, MPI_Fint* source,
			       MPI_Fint* recvtag, MPI_Comm* comm,
			       MPI_Status* status, MPI_Fint* ierr) {
  *ierr = MPI_Sendrecv_replace(buf, *count, *datatype, *dest, *sendtag,
			       *source, *recvtag, *comm, status);
} VT_GENERATE_F77_BINDINGS(mpi_sendrecv_replace, MPI_SENDRECV_REPLACE,
			   vt_mpi_sendrecv_replace_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* sendtag, MPI_Fint* source, MPI_Fint* recvtag, MPI_Comm* comm, MPI_Status* status, MPI_Fint* ierr),
			   (buf, count, datatype, dest, sendtag, source, recvtag, comm, status, ierr))

/* -- MPI_Isend -- */

void vt_mpi_isend_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
		    MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm,
		    MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Isend(buf, *count, *datatype, *dest, *tag, *comm, request);
} VT_GENERATE_F77_BINDINGS(mpi_isend, MPI_ISEND,
			   vt_mpi_isend_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm, MPI_Request* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Irecv -- */

void vt_mpi_irecv_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
		    MPI_Fint* source, MPI_Fint* tag, MPI_Comm* comm,
		    MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Irecv(buf, *count, *datatype, *source, *tag, *comm, request);
} VT_GENERATE_F77_BINDINGS(mpi_irecv, MPI_IRECV,
			   vt_mpi_irecv_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Comm* comm, MPI_Request* request, MPI_Fint* ierr),
			   (buf, count, datatype, source, tag, comm, request, ierr))

/* -- MPI_Ibsend -- */

void vt_mpi_ibsend_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
		     MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm,
		     MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Ibsend(buf, *count, *datatype, *dest, *tag, *comm, request);
} VT_GENERATE_F77_BINDINGS(mpi_ibsend, MPI_IBSEND,
			   vt_mpi_ibsend_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm, MPI_Request* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Irsend -- */

void vt_mpi_irsend_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
		     MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm,
		     MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Irsend(buf, *count, *datatype, *dest, *tag, *comm, request);
} VT_GENERATE_F77_BINDINGS(mpi_irsend, MPI_IRSEND,
			   vt_mpi_irsend_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm, MPI_Request* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Issend -- */

void vt_mpi_issend_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
		     MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm,
		     MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Issend(buf, *count, *datatype, *dest, *tag, *comm, request);
} VT_GENERATE_F77_BINDINGS(mpi_issend, MPI_ISSEND,
			   vt_mpi_issend_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm, MPI_Request* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Wait -- */

void vt_mpi_wait_f(MPI_Request* request, MPI_Status* status, MPI_Fint* ierr) {
  *ierr = MPI_Wait(request, status);
} VT_GENERATE_F77_BINDINGS(mpi_wait, MPI_WAIT,
			   vt_mpi_wait_f,
			   (MPI_Request* request, MPI_Status* status, MPI_Fint* ierr),
			   (request, status, ierr))

/* -- MPI_Waitall -- */

void vt_mpi_waitall_f(MPI_Fint* count, MPI_Request* array_of_requests,
		      MPI_Status* array_of_statuses, MPI_Fint* ierr) {
  *ierr = MPI_Waitall(*count, array_of_requests, array_of_statuses);
} VT_GENERATE_F77_BINDINGS(mpi_waitall, MPI_WAITALL,
			   vt_mpi_waitall_f,
			   (MPI_Fint* count, MPI_Request* array_of_requests, MPI_Status* array_of_statuses, MPI_Fint* ierr),
			   (count, array_of_requests, array_of_statuses, ierr))

/* -- MPI_Waitany -- */

void vt_mpi_waitany_f(MPI_Fint* count, MPI_Request* array_of_requests,
		      MPI_Fint* index, MPI_Status* status, MPI_Fint* ierr) {
  *ierr = MPI_Waitany(*count, array_of_requests, index, status);
  /* See the description of waitany in the standard;
     the Fortran index ranges are from 1, not zero */
  if (*index >= 0) (*index)++;
} VT_GENERATE_F77_BINDINGS(mpi_waitany, MPI_WAITANY,
			   vt_mpi_waitany_f,
			   (MPI_Fint* count, MPI_Request* array_of_requests, MPI_Fint* index, MPI_Status* status, MPI_Fint* ierr),
			   (count, array_of_requests, index, status, ierr))

/* -- MPI_Waitsome -- */

void vt_mpi_waitsome_f(MPI_Fint* incount, MPI_Request* array_of_requests,
		       MPI_Fint* outcount, MPI_Fint* array_of_indices,
		       MPI_Status* array_of_statuses, MPI_Fint* ierr) {
  int i;

  *ierr = MPI_Waitsome(*incount, array_of_requests, outcount,
		array_of_indices, array_of_statuses); 
  for (i=0; i<*outcount; i++) {
    if (array_of_indices[i] >= 0) array_of_indices[i]++;
  }
} VT_GENERATE_F77_BINDINGS(mpi_waitsome, MPI_WAITSOME,
			   vt_mpi_waitsome_f,
			   (MPI_Fint* incount, MPI_Request* array_of_requests, MPI_Fint* outcount, MPI_Fint* array_of_indices, MPI_Status* array_of_statuses, MPI_Fint* ierr),
			   (incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierr))

/* -- MPI_Test -- */

void vt_mpi_test_f(MPI_Request* request, MPI_Fint* flag, MPI_Status* status,
		   MPI_Fint* ierr) {
  *ierr = MPI_Test(request, flag, status);
} VT_GENERATE_F77_BINDINGS(mpi_test, MPI_TEST,
			   vt_mpi_test_f,
			   (MPI_Request* request, MPI_Fint* flag, MPI_Status* status, MPI_Fint* ierr),
			   (request, flag, status, ierr))

/* -- MPI_Testany -- */

void vt_mpi_testany_f(MPI_Fint* count, MPI_Request* array_of_requests,
		      MPI_Fint* index, MPI_Fint* flag, MPI_Status* status,
		      MPI_Fint* ierr) {
  *ierr  = MPI_Testany(*count, array_of_requests, index, flag, status);
  if (*flag && *index >= 0) (*index)++;
} VT_GENERATE_F77_BINDINGS(mpi_testany, MPI_TESTANY,
			   vt_mpi_testany_f,
			   (MPI_Fint* count, MPI_Request* array_of_requests, MPI_Fint* index, MPI_Fint* flag, MPI_Status* status, MPI_Fint* ierr),
                           (count, array_of_requests, index, flag, status, ierr))

/* -- MPI_Testall -- */

void vt_mpi_testall_f(MPI_Fint* count, MPI_Request* array_of_requests,
		      MPI_Fint* flag, MPI_Status* array_of_statuses,
		      MPI_Fint* ierr) {
  *ierr = MPI_Testall(*count, array_of_requests, flag, array_of_statuses);
} VT_GENERATE_F77_BINDINGS(mpi_testall, MPI_TESTALL,
			   vt_mpi_testall_f,
			   (MPI_Fint* count, MPI_Request* array_of_requests, MPI_Fint* flag, MPI_Status* array_of_statuses, MPI_Fint* ierr),
			   (count, array_of_requests, flag, array_of_statuses, ierr))

/* -- MPI_Testsome -- */

void vt_mpi_testsome_f(MPI_Fint* incount, MPI_Request* array_of_requests,
		       MPI_Fint* outcount, MPI_Fint* array_of_indices,
		       MPI_Status* array_of_statuses, MPI_Fint* ierr) {
  int i;

  *ierr = MPI_Testsome(*incount, array_of_requests, outcount, array_of_indices,
		       array_of_statuses);
  for (i=0; i<*outcount; i++) {
    if (array_of_indices[i] >= 0) array_of_indices[i]++;
  }
} VT_GENERATE_F77_BINDINGS(mpi_testsome, MPI_TESTSOME,
			   vt_mpi_testsome_f,
			   (MPI_Fint* incount, MPI_Request* array_of_requests, MPI_Fint* outcount, MPI_Fint* array_of_indices, MPI_Status* array_of_statuses, MPI_Fint* ierr),
			   (incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierr))

/* -- MPI_Send_init -- */

void vt_mpi_send_init_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
			MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm,
			MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Send_init(buf, *count, *datatype, *dest, *tag, *comm, request);
} VT_GENERATE_F77_BINDINGS(mpi_send_init, MPI_SEND_INIT,
			   vt_mpi_send_init_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm, MPI_Request* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Bsend_init -- */

void vt_mpi_bsend_init_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
			 MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm,
			 MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Bsend_init(buf, *count, *datatype, *dest, *tag, *comm, request);
} VT_GENERATE_F77_BINDINGS(mpi_bsend_init, MPI_BSEND_INIT,
			   vt_mpi_bsend_init_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm, MPI_Request* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Rsend_init -- */

void vt_mpi_rsend_init_f(char* buf, MPI_Fint* count,
			 MPI_Datatype* datatype, MPI_Fint* dest,
			 MPI_Fint* tag, MPI_Comm* comm,
			 MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Rsend_init(buf, *count, *datatype, *dest, *tag, *comm, request);
} VT_GENERATE_F77_BINDINGS(mpi_rsend_INIT, MPI_RSEND_INIT,
			   vt_mpi_rsend_init_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm, MPI_Request* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Ssend_init -- */

void vt_mpi_ssend_init_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
			 MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm,
			 MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Ssend_init(buf, *count, *datatype, *dest, *tag, *comm, request);
} VT_GENERATE_F77_BINDINGS(mpi_ssend_init, MPI_SSEND_INIT,
			   vt_mpi_ssend_init_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Comm* comm, MPI_Request* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Recv_init -- */

void vt_mpi_recv_init_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
			MPI_Fint* source, MPI_Fint* tag, MPI_Comm* comm,
			MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Recv_init(buf, *count, *datatype, *source, *tag, *comm,
			request);
} VT_GENERATE_F77_BINDINGS(mpi_recv_init, MPI_RECV_INIT,
			   vt_mpi_recv_init_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Comm* comm, MPI_Request* request, MPI_Fint* ierr),
			   (buf, count, datatype, source, tag, comm, request, ierr))

/* -- MPI_Start -- */

void vt_mpi_start_f(MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Start(request);
} VT_GENERATE_F77_BINDINGS(mpi_start, MPI_START,
			   vt_mpi_start_f,
			   (MPI_Request* request, MPI_Fint* ierr),
			   (request, ierr))

/* -- MPI_Startall -- */

void vt_mpi_startall_f(MPI_Fint* count, MPI_Request* array_of_requests,
		       MPI_Fint* ierr) {
  *ierr = MPI_Startall(*count, array_of_requests);
} VT_GENERATE_F77_BINDINGS(mpi_startall, MPI_STARTALL,
			   vt_mpi_startall_f,
			   (MPI_Fint* count, MPI_Request* array_of_requests, MPI_Fint* ierr),
			   (count, array_of_requests, ierr))

/* -- MPI_Request_free -- */

void vt_mpi_request_free_f(MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Request_free(request);
} VT_GENERATE_F77_BINDINGS(mpi_request_free, MPI_REQUEST_FREE,
			   vt_mpi_request_free_f,
			   (MPI_Request* request, MPI_Fint* ierr),
			   (request, ierr))

/* -- MPI_Cancel -- */

void vt_mpi_cancel_f(MPI_Request* request, MPI_Fint* ierr) {
  *ierr = MPI_Cancel(request); 
} VT_GENERATE_F77_BINDINGS(mpi_cancel, MPI_CANCEL,
			   vt_mpi_cancel_f,
			   (MPI_Request* request, MPI_Fint* ierr),
			   (request, ierr))

/* -- MPI_Allreduce -- */

void vt_mpi_allreduce_f(char* sendbuf, char* recvbuf, MPI_Fint* count,
			MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm,
			MPI_Fint* ierr) {
  *ierr = MPI_Allreduce(sendbuf, recvbuf, *count, *datatype, *op, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_allreduce, MPI_ALLREDUCE,
			   vt_mpi_allreduce_f,
			   (char* sendbuf, char* recvbuf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Fint* ierr),
			   (sendbuf, recvbuf, count, datatype, op, comm, ierr))

/* -- MPI_Barrier -- */

void vt_mpi_barrier_f(MPI_Comm* comm, MPI_Fint* ierr) {
  *ierr = MPI_Barrier(*comm);
} VT_GENERATE_F77_BINDINGS(mpi_barrier, MPI_BARRIER,
			   vt_mpi_barrier_f,
			   (MPI_Comm* comm, MPI_Fint* ierr),
			   (comm, ierr))

/* -- MPI_Bcast -- */

void vt_mpi_bcast_f(char* buf, MPI_Fint* count, MPI_Datatype* datatype,
		    MPI_Fint* root, MPI_Comm* comm, MPI_Fint* ierr) {
  *ierr = MPI_Bcast(buf, *count, *datatype, *root, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_bcast, MPI_BCAST,
			   vt_mpi_bcast_f,
			   (char* buf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Fint* root, MPI_Comm* comm, MPI_Fint* ierr),
			   (buf, count, datatype, root, comm, ierr))

/* -- MPI_Gather -- */

void vt_mpi_gather_f(char* sendbuf, MPI_Fint* sendcount,
		     MPI_Datatype* sendtype, char* recvbuf,
		     MPI_Fint* recvcount, MPI_Datatype* recvtype,
		     MPI_Fint* root, MPI_Comm* comm, MPI_Fint* ierr) {
  *ierr = MPI_Gather(sendbuf, *sendcount, *sendtype, recvbuf,
		     *recvcount, *recvtype, *root, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_gather, MPI_GATHER,
			   vt_mpi_gather_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Datatype* sendtype, char* recvbuf, MPI_Fint* recvcount, MPI_Datatype* recvtype, MPI_Fint* root, MPI_Comm* comm, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierr))

/* -- MPI_Reduce -- */

void vt_mpi_reduce_f(char* sendbuf, char* recvbuf, MPI_Fint* count,
		     MPI_Datatype* datatype, MPI_Op* op,
		     MPI_Fint* root, MPI_Comm* comm, MPI_Fint* ierr) {
  *ierr = MPI_Reduce(sendbuf, recvbuf, *count, *datatype, *op, *root, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_reduce, MPI_REDUCE,
			   vt_mpi_reduce_f,
			   (char* sendbuf, char* recvbuf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Fint* root, MPI_Comm* comm, MPI_Fint* ierr),
			   (sendbuf, recvbuf, count, datatype, op, root, comm, ierr))

/* -- MPI_Gatherv -- */

void vt_mpi_gatherv_f(char* sendbuf, MPI_Fint* sendcount,
		      MPI_Datatype* sendtype, char* recvbuf,
		      MPI_Fint* recvcounts, MPI_Fint* displs,
		      MPI_Datatype* recvtype, MPI_Fint* root, MPI_Comm* comm,
		      MPI_Fint* ierr) {
  *ierr = MPI_Gatherv(sendbuf, *sendcount, *sendtype, recvbuf, recvcounts,
		      displs, *recvtype, *root, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_gatherv, MPI_GATHERV,
			   vt_mpi_gatherv_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Datatype* sendtype, char* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Datatype* recvtype, MPI_Fint* root, MPI_Comm* comm, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, ierr))		   

/* -- MPI_Allgather -- */

void vt_mpi_allgather_f(char* sendbuf, MPI_Fint* sendcount,
			MPI_Datatype* sendtype, char* recvbuf,
			MPI_Fint* recvcount, MPI_Datatype* recvtype,
			MPI_Comm* comm, MPI_Fint* ierr) {
  *ierr = MPI_Allgather(sendbuf, *sendcount, *sendtype, recvbuf, *recvcount,
			*recvtype, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_allgather, MPI_ALLGATHER,
			   vt_mpi_allgather_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Datatype* sendtype, char* recvbuf, MPI_Fint* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierr)) 

/* -- MPI_Allgatherv -- */

void vt_mpi_allgatherv_f(char* sendbuf, MPI_Fint* sendcount,
			 MPI_Datatype* sendtype, char* recvbuf,
			 MPI_Fint* recvcounts, MPI_Fint* displs,
			 MPI_Datatype* recvtype, MPI_Comm* comm,
			 MPI_Fint* ierr) {
  *ierr = MPI_Allgatherv(sendbuf, *sendcount, *sendtype, recvbuf, recvcounts,
			 displs, *recvtype, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_allgatherv, MPI_ALLGATHERV,
			   vt_mpi_allgatherv_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Datatype* sendtype, char* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, ierr))

/* -- MPI_Alltoall -- */

void vt_mpi_alltoall_f(char* sendbuf, MPI_Fint* sendcount,
		       MPI_Datatype* sendtype, char* recvbuf,
		       MPI_Fint* recvcount, MPI_Datatype* recvtype,
		       MPI_Comm* comm, MPI_Fint* ierr) {
  *ierr = MPI_Alltoall(sendbuf, *sendcount, *sendtype, recvbuf, *recvcount,
		       *recvtype, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_alltoall, MPI_ALLTOALL,
			   vt_mpi_alltoall_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Datatype* sendtype, char* recvbuf, MPI_Fint* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierr))

/* -- MPI_Alltoallv -- */

void vt_mpi_alltoallv_f(char* sendbuf, MPI_Fint* sendcounts, MPI_Fint* sdispls,
			MPI_Datatype* sendtype, char* recvbuf,
			MPI_Fint* recvcounts, MPI_Fint* rdispls,
			MPI_Datatype* recvtype, MPI_Comm* comm,
			MPI_Fint* ierr) {
  *ierr = MPI_Alltoallv(sendbuf, sendcounts, sdispls, *sendtype, recvbuf,
			recvcounts, rdispls, *recvtype, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_alltoallv, MPI_ALLTOALLV,
			   vt_mpi_alltoallv_f,
			   (char* sendbuf, MPI_Fint* sendcounts, MPI_Fint* sdispls, MPI_Datatype* sendtype, char* recvbuf, MPI_Fint* recvcounts, MPI_Fint* rdispls, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Fint* ierr),
			   (sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, ierr)) 

/* -- MPI_Scan -- */

void vt_mpi_scan_f(char* sendbuf, char* recvbuf, MPI_Fint* count,
		   MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm,
		   MPI_Fint* ierr) {
  *ierr = MPI_Scan(sendbuf, recvbuf, *count, *datatype, *op, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_scan, MPI_SCAN,
			   vt_mpi_scan_f,
			   (char* sendbuf, char* recvbuf, MPI_Fint* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Fint* ierr),
			   (sendbuf, recvbuf, count, datatype, op, comm, ierr))

/* -- MPI_Scatter -- */

void vt_mpi_scatter_f(char* sendbuf, MPI_Fint* sendcount,
		      MPI_Datatype* sendtype, char* recvbuf,
		      MPI_Fint* recvcount, MPI_Datatype* recvtype,
		      MPI_Fint* root, MPI_Comm* comm, MPI_Fint* ierr) {
  *ierr = MPI_Scatter(sendbuf, *sendcount, *sendtype, recvbuf, *recvcount,
		      *recvtype, *root, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_scatter, MPI_SCATTER,
			   vt_mpi_scatter_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Datatype* sendtype, char* recvbuf, MPI_Fint* recvcount, MPI_Datatype* recvtype, MPI_Fint* root, MPI_Comm* comm, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierr))

/* -- MPI_Scatterv -- */

void vt_mpi_scatterv_f(char* sendbuf, MPI_Fint* sendcounts,
		       MPI_Fint* displs, MPI_Datatype* sendtype,
		       char* recvbuf, MPI_Fint* recvcount,
		       MPI_Datatype* recvtype, MPI_Fint* root,
		       MPI_Comm* comm, MPI_Fint* ierr) {
  *ierr = MPI_Scatterv(sendbuf, sendcounts, displs, *sendtype, recvbuf,
		       *recvcount, *recvtype, *root, *comm);
} VT_GENERATE_F77_BINDINGS(mpi_scatterv, MPI_SCATTERV,
			   vt_mpi_scatterv_f,
			   (char* sendbuf, MPI_Fint* sendcounts, MPI_Fint* displs, MPI_Datatype* sendtype, char* recvbuf, MPI_Fint* recvcount, MPI_Datatype* recvtype, MPI_Fint* root, MPI_Comm* comm, MPI_Fint* ierr),
			   (sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, ierr))

/* -- MPI_Reduce_scatter -- */

void vt_mpi_reduce_scatter_f(char* sendbuf, char* recvbuf,
			     MPI_Fint* recvcounts, MPI_Datatype* datatype,
			     MPI_Op* op, MPI_Comm* comm, MPI_Fint* ierr) {
  *ierr = MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, *datatype, *op,
			     *comm);
} VT_GENERATE_F77_BINDINGS(mpi_reduce_scatter, MPI_REDUCE_SCATTER,
			   vt_mpi_reduce_scatter_f,
			   (char* sendbuf, char* recvbuf, MPI_Fint* recvcounts, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, MPI_Fint* ierr),
			   (sendbuf, recvbuf, recvcounts, datatype, op, comm, ierr))

#else /* NEED_F2C_CONV */

static MPI_Request *alloc_request_array(int count) {
  static MPI_Request *local_req_arr = 0;
  static int local_req_arr_size = 0;

  if (local_req_arr_size == 0) {
    /* -- never used: initialize -- */
    local_req_arr = (MPI_Request*)malloc(2 * count * sizeof(MPI_Request));
    local_req_arr_size = 2 * count;
  } else if (count > local_req_arr_size) {
    /* -- not enough room: expand -- */
    local_req_arr = (MPI_Request*)realloc(local_req_arr, count * sizeof(MPI_Request));
    local_req_arr_size = count;
  }
  return local_req_arr;
}

static MPI_Status *alloc_status_array(int count) {
  static MPI_Status *local_stat_arr = 0;
  static int local_stat_arr_size = 0;

  if (local_stat_arr_size == 0) {
    /* -- never used: initialize -- */
    local_stat_arr = (MPI_Status*)malloc(2 * count * sizeof(MPI_Status));
    local_stat_arr_size = 2 * count;
  } else if (count > local_stat_arr_size) {
    /* -- not enough room: expand -- */
    local_stat_arr = (MPI_Status*)realloc(local_stat_arr, count * sizeof(MPI_Status));
    local_stat_arr_size = count;
  }
  return local_stat_arr;
}

/* -- MPI_Init -- */

void vt_mpi_init_f(MPI_Fint* ierr) {
  *ierr = MPI_Init(0, (char***)0);
} VT_GENERATE_F77_BINDINGS(mpi_init, MPI_INIT,
			   vt_mpi_init_f,
			   (MPI_Fint* ierr),
			   (ierr))

/* -- MPI_Finalize -- */

void vt_mpi_finalize_f(MPI_Fint* ierr) {
  *ierr = MPI_Finalize();  
} VT_GENERATE_F77_BINDINGS(mpi_finalize, MPI_FINALIZE,
			   vt_mpi_finalize_f,
			   (MPI_Fint* ierr),
			   (ierr))

/* -- MPI_Comm_create -- */

void vt_mpi_comm_create_f(MPI_Fint* comm, MPI_Fint* group, MPI_Fint* newcomm,
			  MPI_Fint* ierr) {
  MPI_Comm l_newcomm;

  *ierr = MPI_Comm_create(MPI_Comm_f2c(*comm), MPI_Group_f2c(*group),
			  &l_newcomm);
  if (*ierr == MPI_SUCCESS) *newcomm = MPI_Comm_c2f(l_newcomm);
} VT_GENERATE_F77_BINDINGS(mpi_comm_create, MPI_COMM_CREATE,
			   vt_mpi_comm_create_f,
			   (MPI_Fint* comm, MPI_Fint* group, MPI_Fint* newcomm, MPI_Fint* ierr),
			   (comm, group, newcomm, ierr))

/* -- MPI_Comm_dup -- */

void vt_mpi_comm_dup_f(MPI_Fint* comm, MPI_Fint* newcomm, MPI_Fint* ierr) {
  MPI_Comm l_newcomm;

  *ierr = MPI_Comm_dup(MPI_Comm_f2c(*comm), &l_newcomm);
  if (*ierr == MPI_SUCCESS) *newcomm = MPI_Comm_c2f(l_newcomm);
} VT_GENERATE_F77_BINDINGS(mpi_comm_dup, MPI_COMM_DUP,
			   vt_mpi_comm_dup_f,
			   (MPI_Fint* comm, MPI_Fint* newcomm, MPI_Fint* ierr),
			   (comm, newcomm, ierr))

/* -- MPI_Comm_split -- */

void vt_mpi_comm_split_f(MPI_Fint* comm, MPI_Fint* color, MPI_Fint* key,
			 MPI_Fint* newcomm, MPI_Fint* ierr) {
  MPI_Comm l_newcomm;

  *ierr = MPI_Comm_split(MPI_Comm_f2c(*comm), *color, *key, &l_newcomm);
  if (*ierr == MPI_SUCCESS) *newcomm = MPI_Comm_c2f(l_newcomm);
} VT_GENERATE_F77_BINDINGS(mpi_comm_split, MPI_COMM_SPLIT,
			   vt_mpi_comm_split_f,
			   (MPI_Fint* comm, MPI_Fint* color, MPI_Fint* key, MPI_Fint* newcomm, MPI_Fint* ierr),
			   (comm, color, key, newcomm, ierr))  

/* -- MPI_Cart_create -- */

void vt_mpi_cart_create_f(MPI_Fint* old_comm, MPI_Fint* ndims, MPI_Fint* dims,
			  MPI_Fint* periods, MPI_Fint* reorder,
			  MPI_Fint* comm_cart, MPI_Fint* ierr) {
  MPI_Comm l_comm_cart;

  *ierr = MPI_Cart_create(MPI_Comm_f2c(*old_comm), *ndims, dims, periods,
			  *reorder, &l_comm_cart);
  if (*ierr == MPI_SUCCESS) *comm_cart = MPI_Comm_c2f(l_comm_cart);
} VT_GENERATE_F77_BINDINGS(mpi_cart_create, MPI_CART_CREATE,
			   vt_mpi_cart_create_f,
			   (MPI_Fint* old_comm, MPI_Fint* ndims, MPI_Fint* dims, MPI_Fint* periods, MPI_Fint* reorder, MPI_Fint* comm_cart, MPI_Fint* ierr),
			   (old_comm, ndims, dims, periods, reorder, comm_cart, ierr))

/* -- MPI_Cart_sub -- */

void vt_mpi_cart_sub_f(MPI_Fint* comm, MPI_Fint* remain_dims,
		       MPI_Fint* new_comm, MPI_Fint* ierr) {
  MPI_Comm l_new_comm;

  *ierr = MPI_Cart_sub(MPI_Comm_f2c(*comm), remain_dims, &l_new_comm);
  if (*ierr == MPI_SUCCESS) *new_comm = MPI_Comm_c2f(l_new_comm);
} VT_GENERATE_F77_BINDINGS(mpi_cart_sub, MPI_CART_SUB,
			   vt_mpi_cart_sub_f,
			   (MPI_Fint* comm, MPI_Fint* remain_dims, MPI_Fint* new_comm, MPI_Fint* ierr),
			   (comm, remain_dims, new_comm, ierr))

/* -- MPI_Graph_create -- */

void vt_mpi_graph_create_f(MPI_Fint* comm_old, MPI_Fint* nnodes,
			   MPI_Fint* index, MPI_Fint* edges,
			   MPI_Fint* reorder, MPI_Fint* comm_graph,
			   MPI_Fint* ierr) {
  MPI_Comm l_comm_graph;

  *ierr = MPI_Graph_create(MPI_Comm_f2c(*comm_old), *nnodes, index, edges,
			   *reorder, &l_comm_graph);
  if (*ierr == MPI_SUCCESS) *comm_graph = MPI_Comm_c2f(l_comm_graph);
} VT_GENERATE_F77_BINDINGS(mpi_graph_create, MPI_GRAPH_CREATE,
			   vt_mpi_graph_create_f,
			   (MPI_Fint* comm_old, MPI_Fint* nnodes, MPI_Fint* index, MPI_Fint* edges, MPI_Fint* reorder, MPI_Fint* comm_graph, MPI_Fint* ierr),
			   (comm_old, nnodes, index, edges, reorder, comm_graph, ierr))

/* -- MPI_Intercomm_create -- */

void vt_mpi_intercomm_create_f(MPI_Fint* local_comm, MPI_Fint* local_leader,
			       MPI_Fint* bridge_comm,
			       MPI_Fint* remote_leader, MPI_Fint* tag,
			       MPI_Fint* newintercomm,
			       MPI_Fint* ierr) {
  MPI_Comm l_newintercomm;

  *ierr = MPI_Intercomm_create(MPI_Comm_f2c(*local_comm), *local_leader, 
			       MPI_Comm_f2c(*bridge_comm), *remote_leader, *tag, &l_newintercomm);
  if (*ierr == MPI_SUCCESS) *newintercomm = MPI_Comm_c2f(l_newintercomm);
} VT_GENERATE_F77_BINDINGS(mpi_intercomm_create, MPI_INTERCOMM_CREATE,
			   vt_mpi_intercomm_create_f,
			   (MPI_Fint* local_comm, MPI_Fint* local_leader, MPI_Fint* bridge_comm, MPI_Fint* remote_leader, MPI_Fint* tag, MPI_Fint* newintercomm, MPI_Fint* ierr),
			   (local_comm, local_leader, bridge_comm, remote_leader, tag, newintercomm, ierr))

/* -- MPI_Intercomm_merge -- */

void vt_mpi_intercomm_merge_f(MPI_Fint* intercomm, MPI_Fint* high,
			      MPI_Fint* newintercomm,
			      MPI_Fint* ierr) {
  MPI_Comm l_newintercomm;

  *ierr = MPI_Intercomm_merge(MPI_Comm_f2c(*intercomm), *high,
			      &l_newintercomm);
  if (*ierr == MPI_SUCCESS) *newintercomm = MPI_Comm_c2f(l_newintercomm); 
} VT_GENERATE_F77_BINDINGS(mpi_intercomm_merge, MPI_INTERCOMM_MERGE,
			   vt_mpi_intercomm_merge_f,
			   (MPI_Fint* intercomm, MPI_Fint* high, MPI_Fint* newintercomm, MPI_Fint* ierr),
			   (intercomm, high, newintercomm, ierr))

/* -- MPI_Comm_free -- */

void vt_mpi_comm_free_f(MPI_Fint* comm, MPI_Fint* ierr) {
  MPI_Comm l_comm = MPI_Comm_f2c(*comm);
  *ierr = MPI_Comm_free(&l_comm);
  if (*ierr == MPI_SUCCESS) *comm = MPI_Comm_c2f(l_comm);
} VT_GENERATE_F77_BINDINGS(mpi_comm_free, MPI_COMM_FREE,
			   vt_mpi_comm_free_f,
			   (MPI_Fint* comm, MPI_Fint* ierr),
			   (comm, ierr))

/* -- MPI_Send -- */

void vt_mpi_send_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
		   MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm,
		   MPI_Fint* ierr) {
  *ierr = MPI_Send(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
		   MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_send, MPI_SEND,
			   vt_mpi_send_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, ierr))

/* -- MPI_Bsend -- */

void vt_mpi_bsend_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
		    MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm,
		    MPI_Fint *ierr) {
  *ierr = MPI_Bsend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
		    MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_bsend, MPI_BSEND,
			   vt_mpi_bsend_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, ierr))

/* -- MPI_Rsend -- */

void vt_mpi_rsend_f(char* ibuf, MPI_Fint* count, MPI_Fint* datatype,
		    MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm,
		    MPI_Fint *ierr) {
  *ierr = MPI_Rsend(ibuf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
		    MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_rsend, MPI_RSEND,
			   vt_mpi_rsend_f,
			   (char* ibuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* ierr),
			   (ibuf, count, datatype, dest, tag, comm, ierr))

/* -- MPI_Ssend -- */

void vt_mpi_ssend_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
		    MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm,
		    MPI_Fint* ierr) {
  *ierr = MPI_Ssend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
		    MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_ssend, MPI_SSEND,
			   vt_mpi_ssend_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, ierr))

/* -- MPI_Recv -- */

void vt_mpi_recv_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
		   MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm,
		   MPI_Fint* status, MPI_Fint* ierr) {
  MPI_Status c_status;

  *ierr = MPI_Recv(buf, *count, MPI_Type_f2c(*datatype), *source, *tag,
		   MPI_Comm_f2c(*comm), &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
} VT_GENERATE_F77_BINDINGS(mpi_recv, MPI_RECV,
			   vt_mpi_recv_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr),
			   (buf, count, datatype, source, tag, comm, status, ierr))

/* -- MPI_Probe -- */

void vt_mpi_probe_f(MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm,
		    MPI_Fint* status, MPI_Fint* ierr) {
  MPI_Status c_status;

  *ierr = MPI_Probe(*source, *tag, MPI_Comm_f2c(*comm), &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
} VT_GENERATE_F77_BINDINGS(mpi_probe, MPI_PROBE,
			   vt_mpi_probe_f,
			   (MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr),
			   (source, tag, comm, status, ierr))

/* -- MPI_Sendrecv -- */

void vt_mpi_sendrecv_f(char* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype,
		       MPI_Fint* dest, MPI_Fint* sendtag, char* recvbuf,
		       MPI_Fint* recvcount, MPI_Fint* recvtype,
		       MPI_Fint* source, MPI_Fint* recvtag, MPI_Fint* comm,
		       MPI_Fint* status, MPI_Fint* ierr) {
  MPI_Status c_status;

  *ierr = MPI_Sendrecv(sendbuf, *sendcount, MPI_Type_f2c(*sendtype), *dest, 
		       *sendtag, recvbuf, *recvcount, MPI_Type_f2c(*recvtype),
		       *source, *recvtag, MPI_Comm_f2c(*comm), &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
} VT_GENERATE_F77_BINDINGS(mpi_sendrecv, MPI_SENDRECV,
			   vt_mpi_sendrecv_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, MPI_Fint* dest, MPI_Fint* sendtag, char* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* source, MPI_Fint* recvtag, MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status, ierr))

/* -- MPI_Sendrecv_replace -- */

void vt_mpi_sendrecv_replace_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
			       MPI_Fint* dest, MPI_Fint* sendtag,
			       MPI_Fint* source, MPI_Fint* recvtag,
			       MPI_Fint* comm, MPI_Fint* status,
			       MPI_Fint* ierr) {
  MPI_Status c_status;

  *ierr = MPI_Sendrecv_replace(buf, *count, MPI_Type_f2c(*datatype), *dest, 
			       *sendtag, *source, *recvtag,
			       MPI_Comm_f2c(*comm), &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
} VT_GENERATE_F77_BINDINGS(mpi_sendrecv_replace, MPI_SENDRECV_REPLACE,
			   vt_mpi_sendrecv_replace_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* sendtag, MPI_Fint* source, MPI_Fint* recvtag, MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr),
			   (buf, count, datatype, dest, sendtag, source, recvtag, comm, status, ierr))

/* -- MPI_Isend -- */

void vt_mpi_isend_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
		    MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm,
		    MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request;

  *ierr = MPI_Isend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
		    MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
} VT_GENERATE_F77_BINDINGS(mpi_isend, MPI_ISEND,
			   vt_mpi_isend_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Irecv -- */

void vt_mpi_irecv_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
		    MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm,
		    MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request;

  *ierr = MPI_Irecv(buf, *count, MPI_Type_f2c(*datatype), *source, *tag,
		    MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
} VT_GENERATE_F77_BINDINGS(mpi_irecv, MPI_IRECV,
			   vt_mpi_irecv_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr),
			   (buf, count, datatype, source, tag, comm, request, ierr))

/* -- MPI_Ibsend -- */

void vt_mpi_ibsend_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
		     MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm,
		     MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request;

  *ierr = MPI_Ibsend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
		     MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
} VT_GENERATE_F77_BINDINGS(mpi_ibsend, MPI_IBSEND,
			   vt_mpi_ibsend_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Irsend -- */

void vt_mpi_irsend_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
		     MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request;

  *ierr = MPI_Irsend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
		     MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
} VT_GENERATE_F77_BINDINGS(mpi_irsend, MPI_IRSEND,
			   vt_mpi_irsend_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Issend -- */

void vt_mpi_issend_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
		     MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm,
		     MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request;

  *ierr = MPI_Issend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
		     MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
} VT_GENERATE_F77_BINDINGS(mpi_issend, MPI_ISSEND,
			   vt_mpi_issend_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Wait -- */

void vt_mpi_wait_f(MPI_Fint* request, MPI_Fint* status, MPI_Fint* ierr) {
  MPI_Request l_request;
  MPI_Status c_status;

  l_request = MPI_Request_f2c(*request);
  *ierr = MPI_Wait(&l_request, &c_status);
  *request = MPI_Request_c2f(l_request);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
} VT_GENERATE_F77_BINDINGS(mpi_wait, MPI_WAIT,
			   vt_mpi_wait_f,
			   (MPI_Fint* request, MPI_Fint* status, MPI_Fint* ierr),
			   (request, status, ierr))

/* -- MPI_Waitall -- */

void vt_mpi_waitall_f(MPI_Fint* count, MPI_Fint array_of_requests[],
		      MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
		      MPI_Fint* ierr) {
  int i;
  MPI_Request* l_request = 0;
  MPI_Status* c_status = 0;

  if (*count > 0) {
    l_request = alloc_request_array(*count);
    c_status = alloc_status_array(*count);
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
} VT_GENERATE_F77_BINDINGS(mpi_waitall, MPI_WAITALL,
			   vt_mpi_waitall_f,
			   (MPI_Fint* count, MPI_Fint array_of_requests[], MPI_Fint array_of_statuses[][MPI_STATUS_SIZE], MPI_Fint* ierr),
			   (count, array_of_requests, array_of_statuses, ierr))

/* -- MPI_Waitany -- */

void vt_mpi_waitany_f(MPI_Fint* count, MPI_Fint array_of_requests[],
		      MPI_Fint* index, MPI_Fint* status, MPI_Fint* ierr) {
  int i;
  MPI_Request *l_request = 0;
  MPI_Status c_status;

  if (*count > 0) {
    l_request = alloc_request_array(*count);
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
} VT_GENERATE_F77_BINDINGS(mpi_waitany, MPI_WAITANY,
			   vt_mpi_waitany_f,
			   (MPI_Fint* count, MPI_Fint array_of_requests[], MPI_Fint* index, MPI_Fint* status, MPI_Fint* ierr),
			   (count, array_of_requests, index, status, ierr))

/* -- MPI_Waitsome -- */

void vt_mpi_waitsome_f(MPI_Fint* incount, MPI_Fint array_of_requests[],
		       MPI_Fint* outcount, MPI_Fint array_of_indices[],
		       MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
		       MPI_Fint* ierr) {
  int i, j, found;
  MPI_Request *l_request = 0;
  MPI_Status  *c_status = 0;

  if (*incount > 0) {
    l_request = alloc_request_array(*incount);
    c_status = alloc_status_array(*incount);
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
} VT_GENERATE_F77_BINDINGS(mpi_waitsome, MPI_WAITSOME,
			   vt_mpi_waitsome_f,
			   (MPI_Fint* incount, MPI_Fint array_of_requests[], MPI_Fint* outcount, MPI_Fint array_of_indices[], MPI_Fint array_of_statuses[][MPI_STATUS_SIZE], MPI_Fint* ierr),
			   (incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierr))

/* -- MPI_Test -- */

void vt_mpi_test_f(MPI_Fint* request, MPI_Fint* flag,
		   MPI_Fint* status, MPI_Fint* ierr) {
  MPI_Status c_status;
  MPI_Request l_request = MPI_Request_f2c(*request);

  *ierr = MPI_Test(&l_request, flag, &c_status);
  if (*ierr != MPI_SUCCESS) return;
  *request = MPI_Request_c2f(l_request);
  if (flag) MPI_Status_c2f(&c_status, status);
} VT_GENERATE_F77_BINDINGS(mpi_test, MPI_TEST,
			   vt_mpi_test_f,
			   (MPI_Fint* request, MPI_Fint* flag, MPI_Fint* status, MPI_Fint* ierr),
			   (request, flag, status, ierr))

/* -- MPI_Testany -- */

void vt_mpi_testany_f(MPI_Fint* count, MPI_Fint array_of_requests[],
		      MPI_Fint* index, MPI_Fint* flag, MPI_Fint* status,
		      MPI_Fint* ierr) {
  int i;
  MPI_Request *l_request = 0;
  MPI_Status c_status;

  if (*count > 0) {
    l_request = alloc_request_array(*count);
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
} VT_GENERATE_F77_BINDINGS(mpi_testany, MPI_TESTANY,
			   vt_mpi_testany_f,
			   (MPI_Fint* count, MPI_Fint array_of_requests[], MPI_Fint* index, MPI_Fint* flag, MPI_Fint* status, MPI_Fint* ierr),
                           (count, array_of_requests, index, flag, status, ierr))

/* -- MPI_Testall -- */

void vt_mpi_testall_f(MPI_Fint* count, MPI_Fint array_of_requests[],
		      MPI_Fint* flag,
		      MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
		      MPI_Fint* ierr) {
  int i;
  MPI_Request *l_request = 0;
  MPI_Status *c_status = 0;

  if (*count > 0) {
    l_request = alloc_request_array(*count);
    c_status = alloc_status_array(*count);
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
} VT_GENERATE_F77_BINDINGS(mpi_testall, MPI_TESTALL,
			   vt_mpi_testall_f,
			   (MPI_Fint* count, MPI_Fint array_of_requests[], MPI_Fint* flag, MPI_Fint array_of_statuses[][MPI_STATUS_SIZE], MPI_Fint *ierr),
			   (count, array_of_requests, flag, array_of_statuses, ierr))

/* -- MPI_Testsome -- */

void vt_mpi_testsome_f(MPI_Fint* incount, MPI_Fint array_of_requests[],
		       MPI_Fint* outcount, MPI_Fint array_of_indices[],
		       MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
		       MPI_Fint* ierr) {
  int i, j, found;
  MPI_Request *l_request = 0;
  MPI_Status  *c_status = 0;

  if (*incount > 0) {
    l_request = alloc_request_array(*incount);
    c_status = alloc_status_array(*incount);
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
} VT_GENERATE_F77_BINDINGS(mpi_testsome, MPI_TESTSOME,
			   vt_mpi_testsome_f,
			   (MPI_Fint* incount, MPI_Fint array_of_requests[], MPI_Fint* outcount, MPI_Fint array_of_indices[], MPI_Fint array_of_statuses[][MPI_STATUS_SIZE], MPI_Fint* ierr),
			   (incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierr))

/* -- MPI_Send_init -- */

void vt_mpi_send_init_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
			MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm,
			MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request;

  *ierr = MPI_Send_init(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
			MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
} VT_GENERATE_F77_BINDINGS(mpi_send_init, MPI_SEND_INIT,
			   vt_mpi_send_init_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Bsend_init -- */

void vt_mpi_bsend_init_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
			 MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm,
			 MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request;

  *ierr = MPI_Bsend_init(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
			 MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
} VT_GENERATE_F77_BINDINGS(mpi_bsend_init, MPI_BSEND_INIT,
			   vt_mpi_bsend_init_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Rsend_init -- */

void vt_mpi_rsend_init_f(char* buf, MPI_Fint* count,
			 MPI_Fint* datatype, MPI_Fint* dest,
			 MPI_Fint* tag, MPI_Fint* comm,
			 MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request;

  *ierr = MPI_Rsend_init(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
			 MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
} VT_GENERATE_F77_BINDINGS(mpi_rsend_INIT, MPI_RSEND_INIT,
			   vt_mpi_rsend_init_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Ssend_init -- */

void vt_mpi_ssend_init_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
			 MPI_Fint* dest, MPI_Fint* tag,
			 MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request;

  *ierr = MPI_Ssend_init(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
			 MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
} VT_GENERATE_F77_BINDINGS(mpi_ssend_init, MPI_SSEND_INIT,
			   vt_mpi_ssend_init_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr),
			   (buf, count, datatype, dest, tag, comm, request, ierr))

/* -- MPI_Recv_init -- */

void vt_mpi_recv_init_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
			MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm,
			MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request;

  *ierr = MPI_Recv_init(buf, *count, MPI_Type_f2c(*datatype), *source, *tag,
			MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
} VT_GENERATE_F77_BINDINGS(mpi_recv_init, MPI_RECV_INIT,
			   vt_mpi_recv_init_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr),
			   (buf, count, datatype, source, tag, comm, request, ierr))

/* -- MPI_Start -- */

void vt_mpi_start_f(MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request = MPI_Request_f2c(*request);

  *ierr = MPI_Start(&l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
} VT_GENERATE_F77_BINDINGS(mpi_start, MPI_START,
			   vt_mpi_start_f,
			   (MPI_Fint* request, MPI_Fint* ierr),
			   (request, ierr))

/* -- MPI_Startall -- */

void vt_mpi_startall_f(MPI_Fint* count, MPI_Fint array_of_requests[],
		       MPI_Fint* ierr) {
  int i;
  MPI_Request *l_request = 0;

  if (*count > 0) {
    l_request = alloc_request_array(*count);
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
} VT_GENERATE_F77_BINDINGS(mpi_startall, MPI_STARTALL,
			   vt_mpi_startall_f,
			   (MPI_Fint* count, MPI_Fint array_of_requests[], MPI_Fint* ierr),
			   (count, array_of_requests, ierr))

/* -- MPI_Request_free -- */

void vt_mpi_request_free_f(MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request = MPI_Request_f2c(*request);

  *ierr = MPI_Request_free(&l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
} VT_GENERATE_F77_BINDINGS(mpi_request_free, MPI_REQUEST_FREE,
			   vt_mpi_request_free_f,
			   (MPI_Fint* request, MPI_Fint* ierr),
			   (request, ierr))

/* -- MPI_Cancel -- */

void vt_mpi_cancel_f(MPI_Fint* request, MPI_Fint* ierr) {
  MPI_Request l_request = MPI_Request_f2c(*request);  
  *ierr = MPI_Cancel(&l_request); 
} VT_GENERATE_F77_BINDINGS(mpi_cancel, MPI_CANCEL,
			   vt_mpi_cancel_f,
			   (MPI_Fint* request, MPI_Fint* ierr),
			   (request, ierr))

/* -- MPI_Allreduce -- */

void vt_mpi_allreduce_f(char* sendbuf, char* recvbuf, MPI_Fint* count,
			MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm,
			MPI_Fint* ierr) {
  *ierr = MPI_Allreduce(sendbuf, recvbuf, *count, MPI_Type_f2c(*datatype),
			MPI_Op_f2c(*op), MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_allreduce, MPI_ALLREDUCE,
			   vt_mpi_allreduce_f,
			   (char* sendbuf, char* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr),
			   (sendbuf, recvbuf, count, datatype, op, comm, ierr))

/* -- MPI_Barrier -- */

void vt_mpi_barrier_f(MPI_Fint* comm, MPI_Fint* ierr) {
  *ierr = MPI_Barrier(MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_barrier, MPI_BARRIER,
			   vt_mpi_barrier_f,
			   (MPI_Fint* comm, MPI_Fint* ierr),
			   (comm, ierr))

/* -- MPI_Bcast -- */

void vt_mpi_bcast_f(char* buf, MPI_Fint* count, MPI_Fint* datatype,
		    MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr) {
  *ierr = MPI_Bcast(buf, *count, MPI_Type_f2c(*datatype), *root,
		    MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_bcast, MPI_BCAST,
			   vt_mpi_bcast_f,
			   (char* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr),
			   (buf, count, datatype, root, comm, ierr))

/* -- MPI_Gather -- */

void vt_mpi_gather_f(char* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype,
		     char* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype,
		     MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr) {
  *ierr = MPI_Gather(sendbuf, *sendcount, MPI_Type_f2c(*sendtype), recvbuf,
		     *recvcount, MPI_Type_f2c(*recvtype), *root,
		     MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_gather, MPI_GATHER,
			   vt_mpi_gather_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, char* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierr))

/* -- MPI_Reduce -- */

void vt_mpi_reduce_f(char* sendbuf, char* recvbuf, MPI_Fint* count,
		     MPI_Fint* datatype, MPI_Fint* op,
		     MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr) {
  *ierr = MPI_Reduce(sendbuf, recvbuf, *count, MPI_Type_f2c(*datatype),
		     MPI_Op_f2c(*op), *root, MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_reduce, MPI_REDUCE,
			   vt_mpi_reduce_f,
			   (char* sendbuf, char* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr),
			   (sendbuf, recvbuf, count, datatype, op, root, comm, ierr))

/* -- MPI_Gatherv -- */

void vt_mpi_gatherv_f(char* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype,
		      char* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs,
		      MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm,
		      MPI_Fint* ierr) {
  *ierr = MPI_Gatherv(sendbuf, *sendcount, MPI_Type_f2c(*sendtype),
		      recvbuf, recvcounts, displs, MPI_Type_f2c(*recvtype),
		      *root, MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_gatherv, MPI_GATHERV,
			   vt_mpi_gatherv_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, char* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, ierr))		   

/* -- MPI_Allgather -- */

void vt_mpi_allgather_f(char* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype,
			char* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype,
			MPI_Fint* comm, MPI_Fint* ierr) {
  *ierr = MPI_Allgather(sendbuf, *sendcount, MPI_Type_f2c(*sendtype),
			recvbuf, *recvcount, MPI_Type_f2c(*recvtype),
			MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_allgather, MPI_ALLGATHER,
			   vt_mpi_allgather_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, char* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierr)) 

/* -- MPI_Allgatherv -- */

void vt_mpi_allgatherv_f(char* sendbuf, MPI_Fint* sendcount,
			 MPI_Fint* sendtype, char* recvbuf,
			 MPI_Fint* recvcounts, MPI_Fint* displs,
			 MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr) {
  *ierr = MPI_Allgatherv(sendbuf, *sendcount, MPI_Type_f2c(*sendtype),
			 recvbuf, recvcounts, displs, MPI_Type_f2c(*recvtype),
			 MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_allgatherv, MPI_ALLGATHERV,
			   vt_mpi_allgatherv_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, char* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, ierr))

/* -- MPI_Alltoall -- */

void vt_mpi_alltoall_f(char* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype,
		       char* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype,
		       MPI_Fint* comm, MPI_Fint* ierr) {
  *ierr = MPI_Alltoall(sendbuf, *sendcount, MPI_Type_f2c(*sendtype), recvbuf,
		       *recvcount, MPI_Type_f2c(*recvtype),
		       MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_alltoall, MPI_ALLTOALL,
			   vt_mpi_alltoall_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, char* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierr))

/* -- MPI_Alltoallv -- */

void vt_mpi_alltoallv_f(char* sendbuf, MPI_Fint* sendcounts, MPI_Fint* sdispls,
			MPI_Fint* sendtype, char* recvbuf,
			MPI_Fint* recvcounts, MPI_Fint* rdispls,
			MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr) {
  *ierr = MPI_Alltoallv(sendbuf, sendcounts, sdispls, MPI_Type_f2c(*sendtype),
			recvbuf, recvcounts, rdispls, MPI_Type_f2c(*recvtype),
			MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_alltoallv, MPI_ALLTOALLV,
			   vt_mpi_alltoallv_f,
			   (char* sendbuf, MPI_Fint* sendcounts, MPI_Fint* sdispls, MPI_Fint* sendtype, char* recvbuf, MPI_Fint* recvcounts, MPI_Fint* rdispls, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr),
			   (sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, ierr)) 

/* -- MPI_Scan -- */

void vt_mpi_scan_f(char* sendbuf, char* recvbuf, MPI_Fint* count,
		   MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm,
		   MPI_Fint* ierr) {
  *ierr = MPI_Scan(sendbuf, recvbuf, *count, MPI_Type_f2c(*datatype),
		   MPI_Op_f2c(*op), MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_scan, MPI_SCAN,
			   vt_mpi_scan_f,
			   (char* sendbuf, char* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr),
			   (sendbuf, recvbuf, count, datatype, op, comm, ierr))

/* -- MPI_Scatter -- */

void vt_mpi_scatter_f(char* sendbuf, MPI_Fint* sendcount,
		      MPI_Fint* sendtype, char* recvbuf,
		      MPI_Fint* recvcount, MPI_Fint* recvtype,
		      MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr) {
  *ierr = MPI_Scatter(sendbuf, *sendcount, MPI_Type_f2c(*sendtype),
		      recvbuf, *recvcount, MPI_Type_f2c(*recvtype),
		      *root, MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_scatter, MPI_SCATTER,
			   vt_mpi_scatter_f,
			   (char* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, char* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr),
			   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierr))

/* -- MPI_Scatterv -- */

void vt_mpi_scatterv_f(char* sendbuf, MPI_Fint* sendcounts,
		       MPI_Fint* displs, MPI_Fint* sendtype,
		       char* recvbuf, MPI_Fint* recvcount,
		       MPI_Fint* recvtype, MPI_Fint* root,
		       MPI_Fint* comm, MPI_Fint* ierr) {
  *ierr = MPI_Scatterv(sendbuf, sendcounts, displs, MPI_Type_f2c(*sendtype),
		       recvbuf, *recvcount, MPI_Type_f2c(*recvtype),
		       *root, MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_scatterv, MPI_SCATTERV,
			   vt_mpi_scatterv_f,
			   (char* sendbuf, MPI_Fint* sendcounts, MPI_Fint* displs, MPI_Fint* sendtype, char* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr),
			   (sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, ierr))

/* -- MPI_Reduce_scatter -- */

void vt_mpi_reduce_scatter_f(char* sendbuf, char* recvbuf,
			     MPI_Fint* recvcounts, MPI_Fint* datatype,
			     MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr) {
  *ierr = MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts,
			     MPI_Type_f2c(*datatype), MPI_Op_f2c(*op), MPI_Comm_f2c(*comm));
} VT_GENERATE_F77_BINDINGS(mpi_reduce_scatter, MPI_REDUCE_SCATTER,
			   vt_mpi_reduce_scatter_f,
			   (char* sendbuf, char* recvbuf, MPI_Fint* recvcounts, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr),
			   (sendbuf, recvbuf, recvcounts, datatype, op, comm, ierr))

#endif /* NEED_F2C_CONV */
