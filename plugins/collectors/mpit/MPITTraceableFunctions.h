////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008-2011 The Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Definition of the MPITCollector TraceableFunctions.
 *
 */
    static const char* TraceableFunctions[] = {

	"MPI_Allgather",
	"MPI_Allgatherv",
	"MPI_Allreduce",
	"MPI_Alltoall",
	"MPI_Alltoallv",
	"MPI_Barrier",
	"MPI_Bcast",
	"MPI_Bsend",
	"MPI_Bsend_init",
	"MPI_Cancel",
	"MPI_Cart_create",
	"MPI_Cart_sub",
	"MPI_Comm_create",
	"MPI_Comm_dup",
	"MPI_Comm_free",
	"MPI_Comm_split",
	"MPI_Finalize",
	"MPI_Gather",
	"MPI_Gatherv",
	"MPI_Get_count",
	"MPI_Graph_create",
	"MPI_Ibsend",
	"MPI_Init",
	"MPI_Intercomm_create",
	"MPI_Intercomm_merge",
	"MPI_Iprobe",
	"MPI_Irecv",
	"MPI_Irsend",
	"MPI_Isend",
	"MPI_Issend",
	"MPI_Pack",
	"MPI_Probe",
	"MPI_Recv",
	"MPI_Recv_init",
	"MPI_Reduce",
	"MPI_Reduce_scatter",
	"MPI_Request_free",
	"MPI_Rsend",
	"MPI_Rsend_init",
	"MPI_Scan",
	"MPI_Scatter",
	"MPI_Scatterv",
	"MPI_Send",
	"MPI_Sendrecv",
	"MPI_Sendrecv_replace",
	"MPI_Ssend",
	"MPI_Ssend_init",
	"MPI_Start",
	"MPI_Startall",
	"MPI_Test",
	"MPI_Testall",
	"MPI_Testany",
	"MPI_Testsome",
	"MPI_Unpack",
	"MPI_Wait",
	"MPI_Waitall",
	"MPI_Waitany",
	"MPI_Waitsome",
	
	// End Of Table Entry
	NULL
    };
    static const char* TraceableCategories[] = {

	"all",
	"asynchronous_p2p",
	"collective_com",
	"datatypes",
	"environment",
	"graphs_contexts_comms",
	"persistent_com",
	"process_topologies",
	"synchronous_p2p",
	// End Of Table Entry
	NULL
    };

    static const char* TraceableAsynchronousP2P[] = {
          "MPI_Cancel",
          "MPI_Ibsend",
          "MPI_Iprobe",
          "MPI_Irecv",
          "MPI_Irsend",
          "MPI_Isend",
          "MPI_Issend",
          "MPI_Request_free",
          "MPI_Test",
          "MPI_Testall",
          "MPI_Testany",
          "MPI_Testsome",
          "MPI_Wait",
          "MPI_Waitall",
          "MPI_Waitany",
          "MPI_Waitsome",
	// End Of Table Entry
	NULL
    };

    static const char* TraceableCollectives[] = {
         "MPI_Allgather",
         "MPI_Allgatherv",
         "MPI_Allreduce",
         "MPI_Alltoall",
         "MPI_Alltoallv",
         "MPI_Barrier",
         "MPI_Bcast",
         "MPI_Gather",
         "MPI_Gatherv",
         "MPI_Reduce",
         "MPI_Reduce_scatter",
         "MPI_Scan",
         "MPI_Scatter",
         "MPI_Scatterv",
	// End Of Table Entry
	NULL
    };

    static const char *TraceableDatatypes[] = {
          "MPI_Pack",
          "MPI_Unpack",
	// End Of Table Entry
	NULL
    };

    static const char *TraceableEnvironment[] = {
          "MPI_Finalize",
          "MPI_Init",
	// End Of Table Entry
	NULL
    };

    static const char *TraceableGraphsContexts[] = {
          "MPI_Comm_create",
          "MPI_Comm_dup",
          "MPI_Comm_free",
          "MPI_Comm_split",
          "MPI_Intercomm_create",
          "MPI_Intercomm_merge",
	// End Of Table Entry
	NULL
    };

    static const char *TraceablePersistent[] = {
          "MPI_Bsend_init",
          "MPI_Recv_init",
          "MPI_Rsend_init",
          "MPI_Send_init",
          "MPI_Ssend_init",
          "MPI_Start",
          "MPI_Startall",
	// End Of Table Entry
	NULL
    };
    static const char *TraceableProcessTopologies[] = {
          "MPI_Cart_create",
          "MPI_Cart_sub",
          "MPI_Graph_create",
	// End Of Table Entry
	NULL
    };

    static const char *TraceableSynchronousP2P[] = {
          "MPI_Bsend",
          "MPI_Get_count",
          "MPI_Probe",
          "MPI_Recv",
          "MPI_Rsend",
          "MPI_Send",
          "MPI_Sendrecv",
          "MPI_Sendrecv_replace",
          "MPI_Ssend",
	// End Of Table Entry
	NULL
    };

/* -------------------------------------------------------- */

    static const char *asynchronous_p2p =
"MPI_Cancel:MPI_Ibsend:MPI_Iprobe:MPI_Irecv:MPI_Irsend:\
MPI_Isend:MPI_Issend:MPI_Request_free:MPI_Test:MPI_Testall:\
MPI_Testany:MPI_Testsome:MPI_Wait:MPI_Waitall:MPI_Waitany:MPI_Waitsome";

    static const char *collective_com =
"MPI_Allgather:MPI_Allgatherv:MPI_Allreduce:MPI_Alltoall:\
MPI_Alltoallv:MPI_Barrier:MPI_Bcast:MPI_Gather:MPI_Gatherv:\
MPI_Reduce:MPI_Reduce_scatter:MPI_Scan:MPI_Scatter:MPI_Scatterv";

    static const char *datatypes =
"MPI_Pack:MPI_Unpack";

    static const char *environment =
"MPI_Finalize:MPI_Init";

    static const char *graphs_contexts_comms =
"MPI_Comm_create:MPI_Comm_dup:MPI_Comm_free:MPI_Comm_split:\
MPI_Intercomm_create:MPI_Intercomm_merge";

    static const char *persistent_com =
"MPI_Bsend_init:MPI_Recv_init:MPI_Rsend_init:MPI_Send_init:\
MPI_Ssend_init:MPI_Start:MPI_Startall";

    static const char *process_topologies =
"MPI_Cart_create:MPI_Cart_sub:MPI_Graph_create";

    static const char *synchronous_p2p =
"MPI_Bsend:MPI_Get_count:MPI_Probe:MPI_Recv:\
MPI_Rsend:MPI_Send:MPI_Sendrecv:MPI_Sendrecv_replace:MPI_Ssend";

    static const char *all =
"MPI_Allgather:MPI_Allgatherv:MPI_Allreduce:MPI_Alltoall:\
MPI_Alltoallv:MPI_Barrier:MPI_Bcast:MPI_Bsend:MPI_Bsend_init:\
MPI_Cancel:MPI_Cart_create:MPI_Cart_sub:MPI_Comm_create:\
MPI_Comm_dup:MPI_Comm_free:MPI_Comm_split:MPI_Finalize:\
MPI_Gather:MPI_Gatherv:MPI_Get_count:MPI_Graph_create:\
MPI_Ibsend:MPI_Init:MPI_Intercomm_create:MPI_Intercomm_merge:\
MPI_Iprobe:MPI_Irecv:MPI_Irsend:MPI_Isend:MPI_Issend:\
MPI_Pack:MPI_Probe:MPI_Recv:MPI_Recv_init:MPI_Reduce:\
MPI_Reduce_scatter:MPI_Request_free:MPI_Rsend:MPI_Rsend_init:\
MPI_Scan:MPI_Scatter:MPI_Scatterv:MPI_Send:MPI_Sendrecv:\
MPI_Sendrecv_replace:MPI_Ssend:MPI_Ssend_init:MPI_Start:\
MPI_Startall:MPI_Test:MPI_Testall:MPI_Testany:MPI_Testsome:\
MPI_Unpack:MPI_Wait:MPI_Waitall:MPI_Waitany:MPI_Waitsome";
	
