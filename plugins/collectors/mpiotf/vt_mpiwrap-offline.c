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

#include "vt_trc.h"
#include "vt_memhook.h"
#include "vt_mpicom.h"
#include "vt_mpireg.h"
#include "vt_mpireq.h"
#include "vt_pform.h"
#include "vt_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "vt_openss.h"

#define debug_trace 0

static MPI_Status *my_status_array = 0;
static int my_status_array_size = 0;

static MPI_Status* vt_get_status_array(int size) {
  if (my_status_array_size == 0) {
    /* -- never used: initialize -- */
    my_status_array = (MPI_Status*)malloc(size * sizeof(MPI_Status));
    if ( my_status_array == NULL ) vt_error();
    my_status_array_size = size;
  } else if (size > my_status_array_size) {
    /* -- not enough room: expand -- */
    my_status_array = (MPI_Status*)realloc(my_status_array, size * sizeof(MPI_Status));
    if ( my_status_array == NULL ) vt_error();
    my_status_array_size = size;
  }
  return my_status_array;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Init and finalize
 *
 *-----------------------------------------------------------------------------
 */

#if 0
//extern int vt_mpi_notrace = 0;
#endif

/* -- MPI_Init -- */

int MPI_Init( int *argc, char ***argv )
{
  int returnVal, numprocs, i;
  unsigned char* grpv;
  uint64_t time;

  /* shall I trace MPI events? */
  vt_mpi_trace_is_on = vt_mpitrace = vt_env_mpitrace();
  
#if 0
//  debugging/testing - take out 
//  vt_mpi_trace_is_on = 1;
//  vt_mpitrace = 1;
#endif

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Init called, IS_TRACE_ON = %d, vt_enter_user_called=%d, vt_open_called=%d \n", 
            IS_TRACE_ON, vt_enter_user_called, vt_open_called);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      /* first event?
	 -> initialize VT and enter dummy function 'user' */

      if (!vt_open_called) {

        if (debug_trace) {
          fprintf(stderr, "OFFLINE WRAPPER, MPI_Init called, before calling vt_open()\n");
          fflush(stderr);
        }

	vt_open();

        if (debug_trace) {
          fprintf(stderr, "OFFLINE WRAPPER, MPI_Init called, after calling vt_open()\n");
          fflush(stderr);
        }

	time = vt_pform_wtime();
	vt_enter_user(&time);
	vt_enter_user_called = 1;

        if (debug_trace) {
          fprintf(stderr, "OFFLINE WRAPPER, MPI_Init called, setting vt_enter_user_called to 1\n");
          fflush(stderr);
        }

      } else {
	time = vt_pform_wtime();
      }

      vt_enter(&time, vt_mpi_regid[VT__MPI_INIT]);

      returnVal = PMPI_Init(argc, argv);

      if (debug_trace) {
         fprintf(stderr, "OFFLINE WRAPPER, MPI_Init called, returnVal from PMPI_Init=%d\n", returnVal);
         fflush(stderr);
      }

      /* initialize mpi event handling */
      vt_mpi_init();

      PMPI_Comm_size(MPI_COMM_WORLD, &numprocs);

      /* define communicator for MPI_COMM_WORLD */
      grpv = (unsigned char*)calloc(numprocs / 8 + (numprocs % 8 ? 1 : 0), sizeof(unsigned char));
      for (i = 0; i < numprocs; i++)
        grpv[i / 8] |= (1 << (i % 8));
      vt_def_mpi_comm(0, numprocs / 8 + (numprocs % 8 ? 1 : 0), grpv);

      free(grpv);

      if (debug_trace) {
         fprintf(stderr, "OFFLINE WRAPPER, MPI_Init called, calling vt_comm_init()\n");
         fflush(stderr);
      }

      /* initialize communicator management */
      vt_comm_init();

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();

    } else {

      returnVal = PMPI_Init(argc, argv);

      /* initialize mpi event handling */
      vt_mpi_init();

      PMPI_Comm_size(MPI_COMM_WORLD, &numprocs);

      /* define communicator for MPI_COMM_WORLD */
      grpv = (unsigned char*)calloc(numprocs / 8 + (numprocs % 8 ? 1 : 0), sizeof(unsigned char));
      for (i = 0; i < numprocs; i++)
        grpv[i / 8] |= (1 << (i % 8));
      vt_def_mpi_comm(0, numprocs / 8 + (numprocs % 8 ? 1 : 0), grpv);

      free(grpv);

      /* initialize communicator management */
      vt_comm_init();
    }

  if (debug_trace) {
      fprintf(stderr, "OFFLINE WRAPPER, exiting MPI_Init, returnVal=%d\n", returnVal);
      fflush(stderr);
  }

  return returnVal;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Finalize
 *
 *-----------------------------------------------------------------------------
 */


/* -- MPI_Finalize -- */

int MPI_Finalize()
{
  int returnVal;
  uint64_t time;


  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Finalize called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON) {

      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_FINALIZE]);

      /* finalize communicator and request management */
      vt_comm_finalize();
      vt_request_finalize();

      if (debug_trace) {
        fprintf(stderr, "OFFLINE WRAPPER, MPI_Finalize called, calling vt_mpi_finalize()\n");
        fflush(stderr);
      }

      /* finalize mpi event handling */
      vt_mpi_finalize();

      returnVal = PMPI_Finalize();

      if (debug_trace) {
        fprintf(stderr, "OFFLINE WRAPPER, MPI_Finalize called, after calling PMPI_Finalize()\n");
        fflush(stderr);
      }

      time = vt_pform_wtime();
      vt_exit(&time);

      /* exit dummy function 'user', if necessary */
      if (vt_enter_user_called)
	vt_exit_user(&time);

      TRACE_ON();
    } else {
      vt_comm_finalize();
      vt_request_finalize();

      /* finalize mpi event handling */
      vt_mpi_finalize();
      returnVal = PMPI_Finalize();
    }

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Finalize EXITING, returnVal=%d\n", returnVal);
    fflush(stderr);
  }
  return returnVal;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Communicator management
 *
 *-----------------------------------------------------------------------------
 */

/* ------- Constructors ------- */

int MPI_Comm_dup( MPI_Comm comm,
                  MPI_Comm* newcomm )
{
  int result;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Comm_dup called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_COMM_DUP]);

      result = PMPI_Comm_dup(comm, newcomm);
      vt_comm_create(*newcomm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Comm_dup(comm, newcomm);
    }

  return result;
}

int MPI_Comm_create( MPI_Comm comm,
                     MPI_Group group,
                     MPI_Comm* newcomm )
{
  int result;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Comm_create called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_COMM_CREATE]);

      result = PMPI_Comm_create(comm, group, newcomm);
      if ( *newcomm != MPI_COMM_NULL)
        vt_comm_create(*newcomm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Comm_create(comm, group, newcomm);
    }

  return result;
}

int MPI_Comm_split( MPI_Comm comm,
                    int color,
                    int key,
                    MPI_Comm* newcomm )
{
  int result;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Comm_split called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_COMM_SPLIT]);

      result = PMPI_Comm_split(comm, color, key, newcomm);
      if ( *newcomm != MPI_COMM_NULL)
        vt_comm_create(*newcomm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Comm_split(comm, color, key, newcomm);
    }

  return result;
}


/*
 *-----------------------------------------------------------------------------
 *
 * Cartesian Toplogy functions
 *
 *-----------------------------------------------------------------------------
 */

int MPI_Cart_create( MPI_Comm comm_old,
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
  int result;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Cart_create called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_CART_CREATE]);

      result = PMPI_Cart_create(comm_old, ndims, dims, periodv, reorder, comm_cart);
      if ( *comm_cart != MPI_COMM_NULL)
	vt_comm_create(*comm_cart);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Cart_create(comm_old, ndims, dims, periodv, reorder, comm_cart);
    }

  return result;
}

int MPI_Cart_sub ( MPI_Comm comm,
#if MPI_VERSION >= 3
                   const int *rem_dims,
#else
                   int *rem_dims,
#endif
                   MPI_Comm *newcomm)
{
  int result;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Cart_sub called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_CART_SUB]);

      result = PMPI_Cart_sub(comm, rem_dims, newcomm);
      if ( *newcomm != MPI_COMM_NULL)
        vt_comm_create(*newcomm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Cart_sub(comm, rem_dims, newcomm );
    }

  return result;
}



int MPI_Graph_create( MPI_Comm comm_old,
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
  int result;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Graph_create called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_GRAPH_CREATE]);

      result = PMPI_Graph_create(comm_old, nnodes, index, edges, reorder, comm_graph);
      if ( *comm_graph != MPI_COMM_NULL)
        vt_comm_create(*comm_graph);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Graph_create(comm_old, nnodes, index, edges, reorder, comm_graph);
    }

  return result;
}

int MPI_Intercomm_create (MPI_Comm local_comm,
                          int local_leader,
                          MPI_Comm peer_comm,
                          int remote_leader,
                          int tag,
                          MPI_Comm *newintercomm)

{
  int result;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Intercomm_create called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_INTERCOMM_CREATE]);

      result = PMPI_Intercomm_create(local_comm, local_leader, peer_comm,
                                     remote_leader, tag, newintercomm);
      if ( *newintercomm != MPI_COMM_NULL)
        vt_comm_create(*newintercomm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Intercomm_create(local_comm, local_leader, peer_comm,
                                     remote_leader, tag, newintercomm);
    }

  return result;
}

int MPI_Intercomm_merge (MPI_Comm intercomm,
                         int high,
                         MPI_Comm *newcomm)
{
  int result;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Intercomm_merge called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_INTERCOMM_MERGE]);

      result = PMPI_Intercomm_merge(intercomm, high, newcomm);
      if ( *newcomm != MPI_COMM_NULL)
        vt_comm_create(*newcomm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Intercomm_merge ( intercomm, high, newcomm);
    }

  return result;
}


/* ------- Destructors ------- */

int MPI_Comm_free( MPI_Comm* comm )
{
  int result;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Comm_free called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_COMM_FREE]);

      vt_comm_free(*comm);
      result = PMPI_Comm_free(comm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Comm_free(comm);
    }

  return result;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Point-to-point communication
 *
 *-----------------------------------------------------------------------------
 */

/* ------- Synchronous ------- */


/* -- MPI_Send -- */

int MPI_Send( 

/* int  MPI_Send(const void *, int, MPI_Datatype, int, int, MPI_Comm); */

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
	      MPI_Comm comm )
{
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Send called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_SEND]);

      if (dest != MPI_PROC_NULL)
	{
	  PMPI_Type_size(datatype, &sz);
	  vt_mpi_send(&time, VT_RANK_TO_PE(dest, comm), VT_COMM_ID(comm),
		      tag, count * sz);
	}
      result = PMPI_Send(buf, count, datatype, dest, tag, comm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Send(buf, count, datatype, dest, tag, comm);
    }

  return result;
}

/* -- MPI_Bsend -- */

/* int  MPI_Bsend(const void *, int, MPI_Datatype, int, int, MPI_Comm); */


int MPI_Bsend( 
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
	      MPI_Comm comm )
{
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Bsend called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_BSEND]);

      if (dest != MPI_PROC_NULL)
	{
	  PMPI_Type_size(datatype, &sz);
	  vt_mpi_send(&time, VT_RANK_TO_PE(dest, comm), VT_COMM_ID(comm),
		      tag, count * sz);
	}
      result = PMPI_Bsend(buf, count, datatype, dest, tag, comm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Bsend(buf, count, datatype, dest, tag, comm);
    }

  return result;
}

/* -- MPI_Rsend -- */

int MPI_Rsend( 
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
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Rsend called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_RSEND]);

      if (dest != MPI_PROC_NULL)
	{
	  PMPI_Type_size(datatype, &sz);
	  vt_mpi_send(&time, VT_RANK_TO_PE(dest, comm), VT_COMM_ID(comm),
		      tag, count * sz);
	}
      result = PMPI_Rsend(buf, count, datatype, dest, tag, comm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Rsend(buf, count, datatype, dest, tag, comm);
    }

  return result;
}

/* -- MPI_Ssend -- */

int MPI_Ssend( 
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
	      MPI_Comm comm )
{
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Ssend called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_SSEND]);

      if (dest != MPI_PROC_NULL)
	{
	  PMPI_Type_size(datatype, &sz);
	  vt_mpi_send(&time, VT_RANK_TO_PE(dest, comm), VT_COMM_ID(comm),
		      tag, count * sz);
	}
      result = PMPI_Ssend(buf, count, datatype, dest, tag, comm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Ssend(buf, count, datatype, dest, tag, comm);
    }

  return result;
}

/* -- MPI_Recv -- */

int MPI_Recv( void* buf,
	      int count,
	      MPI_Datatype datatype,
	      int source, int tag,
	      MPI_Comm comm,
	      MPI_Status* status )
{
  int result, sz;
  MPI_Status mystatus;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Recv called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_RECV]);

      if (status == MPI_STATUS_IGNORE) status = &mystatus;
      result = PMPI_Recv(buf, count, datatype, source, tag, comm, status);

      time = vt_pform_wtime();

      if (source != MPI_PROC_NULL && result == MPI_SUCCESS)
        {
          PMPI_Type_size(datatype, &sz);
          PMPI_Get_count(status, datatype, &count);
          vt_mpi_recv(&time, VT_RANK_TO_PE(status->MPI_SOURCE, comm),
		      VT_COMM_ID(comm), status->MPI_TAG, count * sz);
        }

      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Recv(buf, count, datatype, source, tag, comm, status);
    }

  return result;
}


/* -- MPI_Probe -- */

int MPI_Probe( int source,
               int tag,
               MPI_Comm comm,
               MPI_Status* status )
{
  int result;
  MPI_Status mystatus;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Probe called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_PROBE]);

      if (status == MPI_STATUS_IGNORE) status = &mystatus;
      result = PMPI_Probe(source, tag, comm, status);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Probe(source, tag, comm, status);
    }

  return result;
}


/* -- MPI_Sendrecv -- */

int MPI_Sendrecv(
#if MPI_VERSION >= 3
                 const void* sendbuf,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
                 const void* sendbuf,
#else
                 void* sendbuf,
#endif
		 int sendcount,
		 MPI_Datatype sendtype,
		 int dest,
		 int sendtag,
		 void* recvbuf,
		 int recvcount,
		 MPI_Datatype recvtype,
		 int source,
		 int recvtag,
		 MPI_Comm comm,
		 MPI_Status* status )
{
  int result, sendsz, recvsz;
  MPI_Status mystatus;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Sendrecv called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_SENDRECV]);

      if (dest != MPI_PROC_NULL)
        {
	  PMPI_Type_size(sendtype, &sendsz);
	  vt_mpi_send(&time, VT_RANK_TO_PE(dest, comm), VT_COMM_ID(comm),
		      sendtag, sendcount * sendsz);
        }
      if (status == MPI_STATUS_IGNORE) status = &mystatus;
      result = PMPI_Sendrecv(sendbuf, sendcount, sendtype, dest,   sendtag,
			     recvbuf, recvcount, recvtype, source, recvtag,
			     comm, status);

      time = vt_pform_wtime();

      if (source != MPI_PROC_NULL && result == MPI_SUCCESS)
        {
	  PMPI_Type_size(recvtype, &recvsz);
	  PMPI_Get_count(status, recvtype, &recvcount);
	  vt_mpi_recv(&time, VT_RANK_TO_PE(status->MPI_SOURCE, comm),
		      VT_COMM_ID(comm), status->MPI_TAG, recvcount * recvsz);
	}

      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Sendrecv(sendbuf, sendcount, sendtype, dest,   sendtag,
			     recvbuf, recvcount, recvtype, source, recvtag,
			     comm, status);
    }

  return result;
}

/* -- MPI_Sendrecv_replace -- */

int MPI_Sendrecv_replace(void* buf,
		         int count,
		         MPI_Datatype datatype,
		         int dest,
		         int sendtag,
		         int source,
		         int recvtag,
		         MPI_Comm comm,
		         MPI_Status* status )
{
  int result, sz;
  MPI_Status mystatus;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Sendrecv_replace called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_SENDRECV_REPLACE]);

      PMPI_Type_size(datatype, &sz);
      if (dest != MPI_PROC_NULL)
        {
	  vt_mpi_send(&time, VT_RANK_TO_PE(dest, comm),
		      VT_COMM_ID(comm),
		      sendtag,
		      count * sz);
        }
      if (status == MPI_STATUS_IGNORE) status = &mystatus;
      result = PMPI_Sendrecv_replace(buf, count, datatype, dest,
	                             sendtag, source, recvtag,
				     comm, status);

      time = vt_pform_wtime();

      if (source != MPI_PROC_NULL && result == MPI_SUCCESS)
        {
	  vt_mpi_recv(&time, VT_RANK_TO_PE(status->MPI_SOURCE, comm),
		      VT_COMM_ID(comm), status->MPI_TAG, count * sz);
	}

      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Sendrecv_replace(buf, count, datatype, dest,
	                             sendtag, source, recvtag,
				     comm, status);
    }

  return result;
}

/* ------- Aynchronous ------- */

/* -- MPI_Isend -- */

int MPI_Isend( 
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
	       MPI_Request* request )
{
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Isend called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_ISEND]);

      if (dest != MPI_PROC_NULL)
	{
	  PMPI_Type_size(datatype, &sz);
	  vt_mpi_send(&time, VT_RANK_TO_PE(dest, comm), VT_COMM_ID(comm),
		      tag, count * sz);
	}
      result = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
      /* no need to save send request as we already created send event,
       * so why saving request, and then have all kinds of trouble handling
       * it correctly
      if (dest != MPI_PROC_NULL && result == MPI_SUCCESS)
        vt_request_create(*request, ERF_SEND, tag, dest, count*sz, comm);
       */

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
    }

  return result;
}


/* -- MPI_Irecv -- */

int MPI_Irecv( void* buf,
	       int count,
	       MPI_Datatype datatype,
	       int source,
	       int tag,
	       MPI_Comm comm,
	       MPI_Request* request )
{
  int result, sz;
  uint64_t time;


  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Isend called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_IRECV]);

      PMPI_Type_size(datatype, &sz);

      result = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
      if (source != MPI_PROC_NULL && result == MPI_SUCCESS)
        vt_request_create(*request, ERF_RECV,
			   tag, 0, count * sz, datatype, comm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
    }

  return result;
}

/* -- MPI_Ibsend -- */

int MPI_Ibsend(
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
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Ibsend called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_IBSEND]);

      if (dest != MPI_PROC_NULL)
	{
	  PMPI_Type_size(datatype, &sz);
	  vt_mpi_send(&time, VT_RANK_TO_PE(dest, comm), VT_COMM_ID(comm),
		      tag, count * sz);
	}
      result = PMPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
      /* no need to save send request as we already created send event,
       * so why saving request, and then have all kinds of trouble handling
       * it correctly
      if (dest != MPI_PROC_NULL && result == MPI_SUCCESS)
        vt_request_create(*request, ERF_SEND, tag, dest, count*sz, comm);
       */

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
    }

  return result;
}

/* -- MPI_Issend -- */

int MPI_Issend(
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
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Issend called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_ISSEND]);

      if (dest != MPI_PROC_NULL)
	{
	  PMPI_Type_size(datatype, &sz);
	  vt_mpi_send(&time, VT_RANK_TO_PE(dest, comm), VT_COMM_ID(comm),
		      tag, count * sz);
	}
      result = PMPI_Issend(buf, count, datatype, dest, tag, comm, request);
      /* no need to save send request as we already created send event,
       * so why saving request, and then have all kinds of trouble handling
       * it correctly
      if (dest != MPI_PROC_NULL && result == MPI_SUCCESS)
        vt_request_create(*request, ERF_SEND, tag, dest, count*sz, comm);
       */

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Issend(buf, count, datatype, dest, tag, comm, request);
    }

  return result;
}

/* -- MPI_Irsend -- */

int MPI_Irsend( 
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
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Irsend called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_IRSEND]);

      if (dest != MPI_PROC_NULL)
	{
	  PMPI_Type_size(datatype, &sz);
	  vt_mpi_send(&time, VT_RANK_TO_PE(dest, comm), VT_COMM_ID(comm),
		      tag, count * sz);
	}
      result = PMPI_Irsend(buf, count, datatype, dest, tag, comm, request);
      /* no need to save send request as we already created send event,
       * so why saving request, and then have all kinds of trouble handling
       * it correctly
      if (dest != MPI_PROC_NULL && result == MPI_SUCCESS)
        vt_request_create(*request, ERF_SEND, tag, dest, count*sz, comm);
       */

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Irsend(buf, count, datatype, dest, tag, comm, request);
    }

  return result;
}

/* -- MPI_Wait -- */

int MPI_Wait( MPI_Request* request,
	      MPI_Status* status)
{
  int result;
  MPI_Status mystatus;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Wait called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      struct VTRequest* orig_req;

      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_WAIT]);

      if (status == MPI_STATUS_IGNORE) status = &mystatus;
      orig_req = vt_request_get(*request);
      result = PMPI_Wait(request, status);

      time = vt_pform_wtime();
      vt_check_request(&time, orig_req, status);

      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Wait(request, status);
    }


  return result;
}

/* -- MPI_Waitall -- */

int MPI_Waitall( int count,
		 MPI_Request* requests,
		 MPI_Status* array_of_statuses )
{
  int result, i;
  struct VTRequest* orig_req;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Waitall called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_WAITALL]);

      if (array_of_statuses == MPI_STATUSES_IGNORE) {
        array_of_statuses = vt_get_status_array(count);
      }
      vt_save_request_array(requests, count);
      result = PMPI_Waitall(count, requests, array_of_statuses);

      time = vt_pform_wtime();

      for (i = 0; i < count; i++)
        {
	  orig_req = vt_saved_request_get(i);
          vt_check_request(&time, orig_req, &(array_of_statuses[i]));
        }

      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Waitall(count, requests, array_of_statuses);
    }

  return result;
}

/* -- MPI_Waitany -- */

int MPI_Waitany( int count,
		 MPI_Request* requests,
		 int* index,
		 MPI_Status* status )
{
  int result;
  struct VTRequest* orig_req;
  MPI_Status mystatus;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Waitany called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_WAITANY]);

      if (status == MPI_STATUS_IGNORE) status = &mystatus;
      vt_save_request_array(requests, count);
      result = PMPI_Waitany(count, requests, index, status);
      orig_req = vt_saved_request_get(*index);

      time = vt_pform_wtime();
      vt_check_request(&time, orig_req, status);

      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Waitany(count, requests, index, status);
    }


  return result;
}

/* -- MPI_Waitsome -- */

int MPI_Waitsome(int incount,
		 MPI_Request *array_of_requests,
		 int *outcount,
		 int *array_of_indices,
		 MPI_Status *array_of_statuses )
{
  int result, i;
  struct VTRequest* orig_req;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Waitsome called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_WAITSOME]);

      if (array_of_statuses == MPI_STATUSES_IGNORE) {
        array_of_statuses = vt_get_status_array(incount);
      }
      vt_save_request_array(array_of_requests, incount);
      result = PMPI_Waitsome(incount, array_of_requests, outcount,
	                     array_of_indices, array_of_statuses );

      time = vt_pform_wtime();

      for (i=0; i<*outcount; ++i)
        {
          orig_req = vt_saved_request_get(array_of_indices[i]);
          vt_check_request(&time, orig_req, &(array_of_statuses[i]));
        }

      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Waitsome(incount, array_of_requests, outcount,
	                     array_of_indices, array_of_statuses );
    }

  return result;
}

/* -- MPI_Test -- */

int MPI_Test( MPI_Request* request,
	      int* flag,
	      MPI_Status* status )
{
  int result;
  struct VTRequest* orig_req;
  MPI_Status mystatus;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Test called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_TEST]);

      if (status == MPI_STATUS_IGNORE) status = &mystatus;
      orig_req = vt_request_get(*request);
      result = PMPI_Test(request, flag, status);

      time = vt_pform_wtime();

      if (*flag) vt_check_request(&time, orig_req, status);

      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Test(request, flag, status);
    }

  return result;
}

/* -- MPI_Testany -- */

int MPI_Testany( int count,
                 MPI_Request *array_of_requests,
		 int *index,
		 int *flag,
		 MPI_Status *status )
{
  int result;
  struct VTRequest* orig_req;
  MPI_Status mystatus;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Testany called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_TESTANY]);

      if (status == MPI_STATUS_IGNORE) status = &mystatus;
      vt_save_request_array(array_of_requests, count);
      result = PMPI_Testany( count, array_of_requests, index, flag, status );

      time = vt_pform_wtime();

      if (*flag && *index != MPI_UNDEFINED)
        {
          orig_req = vt_saved_request_get(*index);
          vt_check_request(&time, orig_req, status);
        }

      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Testany( count, array_of_requests, index, flag, status );
    }

  return result;
}

/* -- MPI_Testall -- */

int MPI_Testall( int count,
                 MPI_Request *array_of_requests,
		 int *flag,
		MPI_Status *array_of_statuses )
{
  int result, i;
  struct VTRequest* orig_req;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Testall called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_TESTALL]);

      if (array_of_statuses == MPI_STATUSES_IGNORE) {
        array_of_statuses = vt_get_status_array(count);
      }
      vt_save_request_array(array_of_requests, count);
      result = PMPI_Testall(count, array_of_requests, flag, array_of_statuses );

      time = vt_pform_wtime();

      if (*flag)
        {
	  for (i = 0; i < count; i++)
            {
	      orig_req = vt_saved_request_get(i);
              vt_check_request(&time, orig_req, &(array_of_statuses[i]));
            }
        }

      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Testall(count, array_of_requests, flag, array_of_statuses );
    }

  return result;
}

/* -- MPI_Testsome -- */

int MPI_Testsome(int incount,
		 MPI_Request *array_of_requests,
		 int *outcount,
		 int *array_of_indices,
		 MPI_Status *array_of_statuses )
{
  int result, i;
  struct VTRequest* orig_req;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Testsome called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_TESTSOME]);

      if (array_of_statuses == MPI_STATUSES_IGNORE) {
        array_of_statuses = vt_get_status_array(incount);
      }
      vt_save_request_array(array_of_requests, incount);
      result = PMPI_Testsome( incount, array_of_requests, outcount,
	                      array_of_indices, array_of_statuses );

      time = vt_pform_wtime();

      for (i=0; i<*outcount; ++i)
        {
          orig_req = vt_saved_request_get(array_of_indices[i]);
          vt_check_request(&time, orig_req, &(array_of_statuses[i]));
        }

      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Testsome( incount, array_of_requests, outcount,
	                      array_of_indices, array_of_statuses );
    }

  return result;
}

/* ------- Persistent requests ------- */

/* -- MPI_Send_init -- */

int MPI_Send_init( 
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
		   MPI_Request* request )
{
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Send_init called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_SEND_INIT]);

      PMPI_Type_size(datatype, &sz);

      result = PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);
      if (dest != MPI_PROC_NULL && result == MPI_SUCCESS)
        vt_request_create(*request, (ERF_SEND | ERF_IS_PERSISTENT),
                           tag, dest, count*sz, datatype, comm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);
    }

  return result;
}

/* -- MPI_Recv_init -- */

int MPI_Recv_init( void* buf,
		   int count,
		   MPI_Datatype datatype,
		   int source,
		   int tag,
		   MPI_Comm comm,
		   MPI_Request* request )
{
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Recv_init called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_RECV_INIT]);

      PMPI_Type_size(datatype, &sz);

      result = PMPI_Recv_init(buf, count, datatype, source, tag, comm, request);
      if (source != MPI_PROC_NULL && result == MPI_SUCCESS)
        vt_request_create(*request, (ERF_RECV | ERF_IS_PERSISTENT),
                           tag, source, count * sz, datatype, comm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Recv_init(buf, count, datatype, source, tag, comm, request);
    }


  return result;
}

/* -- MPI_Bsend_init -- */

int MPI_Bsend_init(
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
		   MPI_Request* request )
{
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Bsend_init called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_BSEND_INIT]);

      PMPI_Type_size(datatype, &sz);

      result = PMPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
      if (dest != MPI_PROC_NULL && result == MPI_SUCCESS)
        vt_request_create(*request, (ERF_SEND | ERF_IS_PERSISTENT),
                           tag, dest, count*sz, datatype, comm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
    }

  return result;
}

/* -- MPI_Ssend_init -- */

int MPI_Ssend_init(
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
		   MPI_Request* request )
{
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Ssend_init called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_SSEND_INIT]);

      PMPI_Type_size(datatype, &sz);

      result = PMPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
      if (dest != MPI_PROC_NULL && result == MPI_SUCCESS)
        vt_request_create(*request, (ERF_SEND | ERF_IS_PERSISTENT),
                           tag, dest, count*sz, datatype, comm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
    }

  return result;
}

/* -- MPI_Rsend_init -- */

int MPI_Rsend_init( 
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
		   MPI_Request* request )
{
  int result, sz;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Rsend_init called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_RSEND_INIT]);

      PMPI_Type_size(datatype, &sz);

      result = PMPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
      if (dest != MPI_PROC_NULL && result == MPI_SUCCESS)
        vt_request_create(*request, (ERF_SEND | ERF_IS_PERSISTENT),
                           tag, dest, count*sz, datatype, comm);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
    }

  return result;
}

/* -- MPI_Start -- */

int MPI_Start( MPI_Request* request )
{
  int result;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Start called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      struct VTRequest* req;

      TRACE_OFF();

      time = vt_pform_wtime();

      vt_enter(&time, vt_mpi_regid[VT__MPI_START]);

      req = vt_request_get(*request);
      if (req)
        {
	  if (req->flags & ERF_IS_PERSISTENT )
	    {
	      req->flags |= ERF_IS_ACTIVE;
	      if ((req->flags & ERF_SEND) && (req->dest != MPI_PROC_NULL))
	        vt_mpi_send(&time, VT_RANK_TO_PE(req->dest, req->comm),
			    VT_COMM_ID(req->comm), req->tag,  req->bytes);
	    }
        }
      result = PMPI_Start(request);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Start(request);
    }


  return result;
}

/* -- MPI_Startall -- */

int MPI_Startall( int count,
                  MPI_Request *array_of_requests )
{
  int result, i;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Startall called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      struct VTRequest* req;
      MPI_Request *request;

      TRACE_OFF();

      time = vt_pform_wtime();

      vt_enter(&time, vt_mpi_regid[VT__MPI_STARTALL]);

      for (i = 0; i < count; i++)
        {
          request=&array_of_requests[i];
          req = vt_request_get(*request);
          if (req)
            {
	      if (req->flags & ERF_IS_PERSISTENT )
	        {
	          req->flags |= ERF_IS_ACTIVE;
	          if ((req->flags & ERF_SEND) && (req->dest != MPI_PROC_NULL))
	            vt_mpi_send(&time, VT_RANK_TO_PE(req->dest, req->comm),
				VT_COMM_ID(req->comm), req->tag,  req->bytes);
	        }
            }
        }
      result = PMPI_Startall( count, array_of_requests );

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
  }
  else
  {
	result = PMPI_Startall( count, array_of_requests );
  }
  return result;
}

/* -- MPI_Request_free -- */

int MPI_Request_free( MPI_Request* request )
{
  int result;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Request_free called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      struct VTRequest* req;

      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_REQUEST_FREE]);

      req = vt_request_get(*request);
      if (req && (req->flags & ERF_IS_PERSISTENT))
        {
	  if (req->flags & ERF_IS_ACTIVE )
	    /* mark active requests for deallocation */
	    req->flags |= ERF_DEALLOCATE;
	  else
	    /* deallocate inactive requests -*/
	    vt_request_free(req);
        }
      /* -- else non-persistent requests:
       *    + we don't track non-persistent sends
       *    + MPI standard strongly suggests to deallocate non-persistent
       *      recv's only by waot or test
       *    ==> nothing to do here
       */
      result = PMPI_Request_free(request);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Request_free(request);
    }

  return result;
}

/* -- MPI_Cancel -- */

int MPI_Cancel( MPI_Request* request )
{
  int result;
  uint64_t time;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Cancel called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_CANCEL]);

      /* -- do not really know what to do here ?!?
       *    would need to find out if canceled communcation completed
       *    sucessfully or was canceled sucessfully (probably possible
       *    by using PMPI_Test_cancelled) but whatever we do here,
       *    we end up by an invalid trace as there we cannot remove the
       *    send events already put in the trace buffer, and so the
       *    message matching in the analysis will fail in any case
       */

      result = PMPI_Cancel(request);

      time = vt_pform_wtime();
      vt_exit(&time);

      TRACE_ON();
    }
  else
    {
      result = PMPI_Cancel(request);
    }

  return result;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Collective communication
 *
 *-----------------------------------------------------------------------------
 */

/* -- MPI_Allreduce -- */

int MPI_Allreduce ( 
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
		    MPI_Comm comm )
{
  int result, sz, N;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Allreduce called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_ALLREDUCE]);

      result = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);

      PMPI_Type_size(datatype, &sz);
      PMPI_Comm_size(comm, &N);
      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_ALLREDUCE],
		      VT_NO_ID, VT_COMM_ID(comm), N*count*sz, count*sz);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
    }

  return result;
}

/* -- MPI_Barrier -- */

int MPI_Barrier( MPI_Comm comm )
{
  int result;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Barrier called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_BARRIER]);

      result = PMPI_Barrier(comm);

      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_BARRIER],
		      VT_NO_ID, VT_COMM_ID(comm), 0, 0);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Barrier(comm);
    }

  return result;
}

/* -- MPI_Bcast -- */

int MPI_Bcast( void* buf,
	       int count,
	       MPI_Datatype datatype,
	       int root,
	       MPI_Comm comm )
{
  int result, sz, N, me;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Bcast called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_BCAST]);

      result = PMPI_Bcast(buf, count, datatype, root, comm);

      PMPI_Type_size(datatype, &sz);
      PMPI_Comm_rank(comm, &me);
      if ( me == root )
        PMPI_Comm_size(comm, &N);
      else
        N = 0;

      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime, vt_mpi_regid[VT__MPI_BCAST],
		      VT_RANK_TO_PE(root, comm), VT_COMM_ID(comm),
		      N*count*sz, count*sz);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Bcast(buf, count, datatype, root, comm);
    }

  return result;
}

/* -- MPI_Gather -- */

int MPI_Gather(
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
	       MPI_Comm comm )
{
  int result, ssz, rsz, N, me;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Gather called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_GATHER]);

      result = PMPI_Gather(sendbuf, sendcount, sendtype,
			   recvbuf, recvcount, recvtype,
			   root, comm);

      PMPI_Type_size(sendtype, &ssz);
      PMPI_Comm_rank(comm, &me);
      if ( me == root ) {
        PMPI_Comm_size(comm, &N);
        PMPI_Type_size(recvtype, &rsz);
      } else {
        N = rsz = 0;
      }

      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_GATHER],
		      VT_RANK_TO_PE(root, comm), VT_COMM_ID(comm),
		      sendcount*ssz, N*recvcount*rsz);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Gather(sendbuf, sendcount, sendtype,
			   recvbuf, recvcount, recvtype,
			   root, comm);
    }

  return result;
}

/* -- MPI_Reduce -- */

int MPI_Reduce( 
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
		MPI_Comm comm )
{
  int result, sz, isroot, me;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Reduce called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_REDUCE]);

      result = PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);

      PMPI_Type_size(datatype, &sz);
      PMPI_Comm_rank(comm, &me);
      isroot = ( me == root );

      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_REDUCE],
		      VT_RANK_TO_PE(root, comm), VT_COMM_ID(comm),
		      count*sz, isroot*count*sz);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
    }

  return result;
}

/* -- MPI_Gatherv -- */

int MPI_Gatherv( 

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
		 MPI_Comm comm )
{
  int result, recvsz, sendsz, recvcount, me, N, i;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Gatherv called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_GATHERV]);

      result = PMPI_Gatherv(sendbuf, sendcount, sendtype,
			    recvbuf, recvcounts, displs, recvtype,
			    root, comm);

      PMPI_Type_size(recvtype, &recvsz);
      PMPI_Type_size(sendtype, &sendsz);
      PMPI_Comm_rank(comm, &me);

      recvcount = recvsz = 0;
      if ( me == root ) {
        PMPI_Comm_size(comm, &N);
        PMPI_Type_size(recvtype, &recvsz);
        for(i = 0; i<N; i++) recvcount += recvcounts[i];
      }
      
      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_GATHERV],
		      VT_RANK_TO_PE(root, comm), VT_COMM_ID(comm),
		      sendcount * sendsz, recvcount * recvsz);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Gatherv(sendbuf, sendcount, sendtype,
			    recvbuf, recvcounts, displs, recvtype,
			    root, comm);
    }

  return result;
}

/* -- MPI_Allgather -- */

int MPI_Allgather( 
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
		   MPI_Comm comm )
{
  int result, recvsz, sendsz, N;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Allgather called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_ALLGATHER]);

      result = PMPI_Allgather(sendbuf, sendcount, sendtype,
			      recvbuf, recvcount, recvtype,
			      comm);

      PMPI_Type_size(recvtype, &recvsz);
      PMPI_Type_size(sendtype, &sendsz);
      PMPI_Comm_size(comm, &N);

      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_ALLGATHER],
		      VT_NO_ID, VT_COMM_ID(comm),
		      N * sendcount * sendsz, N * recvcount * recvsz);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Allgather(sendbuf, sendcount, sendtype,
			      recvbuf, recvcount, recvtype,
			      comm);
    }

  return result;
}

/* -- MPI_Allgatherv -- */

int MPI_Allgatherv( 
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
                    MPI_Comm comm )
{
  int result, recvcount, recvsz, sendsz, i, N;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Allgatherv called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_ALLGATHERV]);

      result = PMPI_Allgatherv(sendbuf, sendcount, sendtype,
			       recvbuf, recvcounts, displs, recvtype,
			       comm);


      PMPI_Type_size(recvtype, &recvsz);
      PMPI_Type_size(sendtype, &sendsz);
      PMPI_Comm_size(comm, &N);
      recvcount = 0;
      for(i = 0; i<N; i++) recvcount += recvcounts[i];

      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_ALLGATHERV],
		      VT_NO_ID, VT_COMM_ID(comm),
		      N * sendcount * sendsz, recvcount * recvsz);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Allgatherv(sendbuf, sendcount, sendtype,
			       recvbuf, recvcounts, displs, recvtype,
			       comm);
    }

  return result;
}

/* -- MPI_Alltoall -- */

int MPI_Alltoall( 
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
                  MPI_Comm comm)
{
  int result, recvsz, sendsz, N;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Alltoall called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_ALLTOALL]);

      result = PMPI_Alltoall(sendbuf, sendcount, sendtype,
			     recvbuf, recvcount, recvtype,
			     comm);

      PMPI_Type_size(recvtype, &recvsz);
      PMPI_Type_size(sendtype, &sendsz);
      PMPI_Comm_size(comm, &N);

      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_ALLTOALL],
		      VT_NO_ID, VT_COMM_ID(comm),
		      sendsz * sendcount * N, recvsz * recvcount * N);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Alltoall(sendbuf, sendcount, sendtype,
			     recvbuf, recvcount, recvtype,
			     comm);
    }

  return result;
}

/* -- MPI_Alltoallv -- */

int MPI_Alltoallv( 

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
                   MPI_Comm comm )
{
  int result, recvcount=0, sendcount=0, recvsz, sendsz, N, i;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Alltoallv called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_ALLTOALLV]);

      result = PMPI_Alltoallv(sendbuf, sendcounts,
                              sdispls, sendtype,
			      recvbuf, recvcounts,
                              rdispls, recvtype,
			      comm);
      PMPI_Type_size(recvtype, &recvsz);
      PMPI_Type_size(sendtype, &sendsz);
      PMPI_Comm_size(comm, &N);
      for(i = 0; i<N; i++)
        {
	  recvcount += recvcounts[i];
	  sendcount += sendcounts[i];
	}

      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_ALLTOALLV],
		      VT_NO_ID, VT_COMM_ID(comm),
		      sendsz * sendcount, recvsz * recvcount);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Alltoallv(sendbuf, sendcounts,
                              sdispls, sendtype,
			      recvbuf, recvcounts,
                              rdispls, recvtype,
			      comm);
    }

  return result;
}

/* -- MPI_Scan -- */

int MPI_Scan( 
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
              MPI_Comm comm  )
{
  int result, size, me, N;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Scan called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_SCAN]);

      result = PMPI_Scan( sendbuf, recvbuf, count, datatype, op, comm );
      PMPI_Type_size(datatype, &size);
      PMPI_Comm_rank(comm, &me);
      PMPI_Comm_size(comm, &N);

      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_SCAN],
		      VT_NO_ID, VT_COMM_ID(comm),
		      (N-me) * size * count, size * count);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Scan( sendbuf, recvbuf, count, datatype, op, comm );
    }

  return result;

}

/* -- MPI_Scatter -- */

int MPI_Scatter( 
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
                 MPI_Comm comm )
{
  int result, sendsz, recvsz, N, me;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Scatter called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_SCATTER]);

      result = PMPI_Scatter(sendbuf, sendcount, sendtype,
			   recvbuf, recvcount, recvtype,
			   root, comm);
      PMPI_Type_size(recvtype, &recvsz);
      PMPI_Comm_rank(comm, &me);
      if ( me == root ) {
        PMPI_Comm_size(comm, &N);
        PMPI_Type_size(sendtype, &sendsz);
      } else {
        N = sendsz = 0;
      }

      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_SCATTER],
		      VT_RANK_TO_PE(root, comm), VT_COMM_ID(comm),
		      N * sendcount * sendsz, recvcount * recvsz);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Scatter(sendbuf, sendcount, sendtype,
			   recvbuf, recvcount, recvtype,
			   root, comm);
    }

  return result;
}


/* -- MPI_Scatterv -- */

int MPI_Scatterv(
#if MPI_VERSION >= 3
                  const void* sendbuf,
                  const int*  sendcounts,
                  const int*  displs,
#elif defined (SGI_MPT) && MPI_VERSION >= 2 && MPI_SUBVERSION >= 2
                  const void* sendbuf,
                  int*        sendcounts,
                  int*        displs,
#else
                  void* sendbuf,
                  int*  sendcounts,
                  int*  displs,
#endif
                  MPI_Datatype sendtype,
                  void* recvbuf,
                  int recvcount,
                  MPI_Datatype recvtype,
                  int root,
                  MPI_Comm comm )
{
  int result, sendcount, recvsz, sendsz, me, N, i;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Scatterv called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_SCATTERV]);

      result = PMPI_Scatterv(sendbuf, sendcounts, displs, sendtype,
			     recvbuf, recvcount, recvtype,
			     root, comm);
      sendcount = sendsz = 0;
      PMPI_Type_size(recvtype, &recvsz);
      PMPI_Comm_rank(comm, &me);
      if ( me == root ) {
        PMPI_Comm_size(comm, &N);
        PMPI_Type_size(sendtype, &sendsz);
        for(i = 0; i<N; i++) sendcount += sendcounts[i];
      }

      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_SCATTERV],
		      VT_RANK_TO_PE(root, comm), VT_COMM_ID(comm),
		      sendcount * sendsz, recvcount * recvsz);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Scatterv(sendbuf, sendcounts, displs, sendtype,
			     recvbuf, recvcount, recvtype,
			     root, comm);
    }

  return result;
}

/* -- MPI_Reduce_scatter -- */

int MPI_Reduce_scatter(
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
			MPI_Comm comm )
{
  int result, i, size, N, count = 0;
  uint64_t time, etime;

  if (debug_trace) {
    fprintf(stderr, "OFFLINE WRAPPER, MPI_Reduce_scatter called, IS_TRACE_ON = %d \n", IS_TRACE_ON);
    fflush(stderr);
  }

  if (IS_TRACE_ON)
    {
      TRACE_OFF();

      time = vt_pform_wtime();
      vt_enter(&time, vt_mpi_regid[VT__MPI_REDUCE_SCATTER]);

      result = PMPI_Reduce_scatter(sendbuf, recvbuf, recvcounts,
	                           datatype, op, comm);
      PMPI_Type_size(datatype, &size);
      PMPI_Comm_size(comm, &N);
      for(i = 0; i<N; i++) count += recvcounts[i];

      etime = vt_pform_wtime();
      vt_mpi_collexit(&time, &etime,
		      vt_mpi_regid[VT__MPI_REDUCE_SCATTER],
		      VT_NO_ID, VT_COMM_ID(comm), count*size, count*size);
      TRACE_ON();
    }
  else
    {
      result = PMPI_Reduce_scatter(sendbuf, recvbuf, recvcounts,
	                           datatype, op, comm);
    }

  return result;
}
